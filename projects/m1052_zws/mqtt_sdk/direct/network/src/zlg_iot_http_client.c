/** 
 * 功能说明：
 *     1.HTTP 协议简单包装。
 *
 * 修改历史：
 *     1.2017-11-24 李先静<lixianjing@zlg.cn> 创建。
 */

#include <assert.h>
#include <stdlib.h>
#include "zlg_iot_log.h"
#include "zlg_iot_http_client.h"

#define STR_KEY_CONTENT_TYPE   "Content-Type"
#define STR_KEY_CONTENT_LENGTH "Content-Length"
#define STR_KEY_SET_COOKIE     "Set-Cookie"

typedef struct _url_info {
  int port;
  char protocol[8];
  char hostname[64];
  char path[256];
}url_info;

static IoT_Error_t url_parser(const char* url, url_info *info) {
  const char *start = NULL;
  const char *end = NULL;
  const char *semicolon = NULL;

  if (NULL == url || info == NULL) {
    return NULL_VALUE_ERROR;
  }

  memset(info, 0x00, sizeof(url_info));

  start = url;
  end = strstr(start, "://");
  if(NULL == end) {
    IOT_ERROR("parse protocol type error");
    return FAILURE;
  }
  snprintf(info->protocol, end - start + 1, "%s", start);

  start = end + strlen("://");
  end = strstr(start, "/");
  if(NULL == end) {
    IOT_ERROR("parse host error");
    return FAILURE;
  }
  semicolon = strstr(start, ":");
  if(NULL == semicolon) {
    snprintf(info->hostname, end - start + 1, "%s", start);
    if(0 == strcmp(info->protocol, "https")) {
      info->port = 443;
    } else {
      info->port = 80;
    }
  } else {
    char *port = strndup(semicolon + strlen(":"), end - semicolon);
    info->port = atoi(port);
    free(port);
    snprintf(info->hostname, semicolon - start + 1, "%s", start);
  }

  start = end;
  end = url + strlen(url);
  snprintf(info->path, end - start + 1, "%s", start);

  return SUCCESS;
}

static char* get_header(char* buff, char* body, const char* key) {
    char* p = buff;

    while(p < body) {
        if(strcmp(p, key) == 0) {
            return p + strlen(p) + 2;
        }
        p += strlen(p);

        while(p < body && *p == '\0') {
            p++;
        }
    }

    return NULL;
}

static unsigned char* parse_header(zlg_iot_http_client_t* c, char* buff) {
    char* p = NULL;
    char* body = NULL;

    c->status = atoi(buff+strlen("HTTP/1.1 ")); 

    body = strstr(buff, "\r\n\r\n");
    if(body != NULL) {
        *body = '\0';
        body += 4;
    }else{
        return NULL;
    }
    
    for(p = buff; p != body; p++) {
        if(*p == '\r' || *p == '\n' || *p == ':') {
            *p = '\0';
        }
    }

    p = get_header(buff, body, STR_KEY_CONTENT_TYPE);
    if(p != NULL) {
        strncpy(c->content_type, p, sizeof(c->content_type)-1);
    }
    
    p = get_header(buff, body, STR_KEY_CONTENT_LENGTH);
    if(p != NULL) {
        c->content_length = atoi(p);
    }

    if(c->on_header) {
        c->on_header(c);
    }

    return (unsigned char*)body;
}

#define goto_exit_if_fail(fret) err = fret; if(err != SUCCESS) goto fexit;

IoT_Error_t zlg_iot_https_do(zlg_iot_http_client_t* c, unsigned char* data, size_t data_size, 
        const char* type) {
    Timer  t = {0};
    size_t ret = 0;
    IoT_Error_t err = 0;
    unsigned char* body = NULL;
    Network network;
    Network* net = &network;
    unsigned char buff[ZLG_IOT_HTTP_BUFF_SIZE+1];

    memset(buff, 0x00, sizeof(buff));
    memset(net, 0x00, sizeof(Network));

    if(data != NULL) {
			snprintf((char*)buff, sizeof(buff), "POST %s HTTP/1.1\r\nHost: %s\r\n%s: %u\r\n%s: %s\r\nCookie: %s\r\n\r\n",
				      c->path, c->hostname, STR_KEY_CONTENT_LENGTH, data_size, STR_KEY_CONTENT_TYPE, type, NULL != c->cookie ? c->cookie : "");
    }else{
			snprintf((char*)buff, sizeof(buff), "GET %s HTTP/1.1\r\nHost: %s\r\nCookie: %s\r\n\r\n", c->path, c->hostname, NULL != c->cookie ? c->cookie : "");
    }

    if(0 == strncmp(c->protocol, "https", strlen("https"))) {
      goto_exit_if_fail(iot_tls_init(net, c->hostname, c->port, 10000) != SUCCESS);
    } else {
      goto_exit_if_fail(iot_socket_init(net, c->hostname, c->port, NULL, 10000) != SUCCESS);
    }
    goto_exit_if_fail(net->connect(net, NULL) != SUCCESS);

    init_timer_delta(&t, 5, 0);
    goto_exit_if_fail(net->write(net, buff, strlen((char*)buff), &t, &ret) != SUCCESS);
    if(data != NULL) {
        init_timer_delta(&t, 5, 0);
        goto_exit_if_fail(net->write(net, data, data_size, &t, &ret) != SUCCESS);
    }

    goto_exit_if_fail(net->readAny(net, buff, ZLG_IOT_HTTP_BUFF_SIZE, &ret) != SUCCESS);

    body = parse_header(c, (char*)buff);
    if(body && c->on_body) {
        size_t offset = 0;
        size_t size = ret - (body - buff);
        long int content_length = c->content_length;

        do {
            c->body = body;
            c->size = size;
            c->offset = offset;
            c->body[size] = '\0';

            c->on_body(c); 

            offset += size;
            content_length -= size;
            if(!content_length) {
                break;
            } 

            goto_exit_if_fail(net->readAny(net, buff, ZLG_IOT_HTTP_BUFF_SIZE, &ret) != SUCCESS);

            size = ret;
            body = buff;
        }while(content_length > 0);
    }
    err = SUCCESS;

fexit:
    net->disconnect(net);
    net->destroy(net);

    return err;
}

IoT_Error_t zlg_iot_https_post(zlg_iot_http_client_t* c, unsigned char* body, size_t body_size,
        const char* type) {
    return zlg_iot_https_do(c, body, body_size, type);
}

IoT_Error_t zlg_iot_https_get(zlg_iot_http_client_t* c) {
    return zlg_iot_https_do(c, NULL, 0, NULL);
}

void zlg_mqtt_info_init(zlg_mqtt_info* info) {
    memset(info, 0x00, sizeof(*info));
}

void zlg_mqtt_info_deinit(zlg_mqtt_info* info) {
    if (NULL != info->token) {
      free(info->token);
    }
    memset(info, 0x00, sizeof(*info));
}

static size_t prepare_request(zlg_mqtt_info* info, char* buff, size_t buff_size) {
    int ret = 0;
    size_t i = 0;
    size_t size = 0;

    ret = snprintf(buff, buff_size, "{\"username\": \"%s\", \"password\": \"%s\", \"devices\": [",
        info->username, info->password);

    size += ret;
    assert(size < buff_size);
    if(size >= buff_size) {
        return buff_size;
    }

    for(i = 0; i < info->devices_nr; i++) {
        const char* devid = info->device_ids[i];
        const char* devtype = info->device_types[i];
        if(i > 0) {
            ret = snprintf(buff+size, buff_size-size, ",{\"devtype\": \"%s\", \"devid\": \"%s\"}",
                devtype, devid); 
        }else{
            ret = snprintf(buff+size, buff_size-size, "{\"devtype\": \"%s\", \"devid\": \"%s\"}",
                devtype, devid); 
        }
        size += ret; 
        assert(size < buff_size);
        if(size >= buff_size) {
            break;
        }
    }

    ret = snprintf(buff+size, buff_size-size, "]}");
    size += ret; 
    assert(size < buff_size);
    if(size >= buff_size) {
        return buff_size;
    }

    return size;
}

#include "jsmn.h"

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

static void mqtt_info_on_body(zlg_iot_http_client_t* c) {
	int i = 0;
	int r = 0;
	char* str;
	jsmn_parser p;
	size_t size = 0;
	jsmntok_t t[64]; 
  char* JSON_STRING = (char*)(c->body);
  zlg_mqtt_info* info = (zlg_mqtt_info*)c->ctx;

	jsmn_init(&p);
	r = jsmn_parse(&p, JSON_STRING, c->size, t, sizeof(t)/sizeof(t[0]));
	if (r < 0) {
		IOT_ERROR("Failed to parse JSON: %d\n", r);
		return;
	}
	
	for (i = 1; i < r; i++) {
		if (jsoneq(JSON_STRING, &t[i], "token") == 0) {
		    str = JSON_STRING + t[i+1].start;
		    size = t[i+1].end-t[i+1].start;
		    str[size] = '\0';
        info->token = strdup(str);

			i++;
		} else if (jsoneq(JSON_STRING, &t[i], "clientip") == 0) {
		    str = JSON_STRING + t[i+1].start;
		    size = t[i+1].end-t[i+1].start;
		    str[size] = '\0';
        strncpy(info->clientip, str, sizeof(info->clientip));

			i++;
		} else if (jsoneq(JSON_STRING, &t[i], "host") == 0) {
		    str = JSON_STRING + t[i+1].start;
		    size = t[i+1].end-t[i+1].start;
		    str[size] = '\0';
        strncpy(info->host, str, sizeof(info->host));

			i++;
		} else if (jsoneq(JSON_STRING, &t[i], "port") == 0) {
		    str = JSON_STRING + t[i+1].start;
		    info->port = atoi(str);

			i++;
		} else if (jsoneq(JSON_STRING, &t[i], "sslport") == 0) {
        str = JSON_STRING + t[i+1].start;
        info->sslport = atoi(str);
      
      i++;
    } else if (jsoneq(JSON_STRING, &t[i], "uuid") == 0) {
		    str = JSON_STRING + t[i+1].start;
		    size = t[i+1].end-t[i+1].start;
		    str[size] = '\0';
        strncpy(info->uuid, str, sizeof(info->uuid));

			i++;
		} else if (jsoneq(JSON_STRING, &t[i], "owner") == 0) {
        str = JSON_STRING + t[i+1].start;
        size = t[i+1].end-t[i+1].start;
        str[size] = '\0';
        strncpy(info->owner, str, sizeof(info->owner));
    }
	}
}

static void firmware_info_on_body(zlg_iot_http_client_t* c) {
	int r = 0;
  size_t size = 0;
	char* str;
	jsmn_parser p;
	jsmntok_t t[64];
  char* JSON_STRING = (char*)(c->body);

  const char *url = NULL;
  url_info* uinfo = (url_info*)c->ctx;

	jsmn_init(&p);
	r = jsmn_parse(&p, JSON_STRING, c->size, t, sizeof(t)/sizeof(t[0]));
	if (r < 0) {
		IOT_ERROR("Failed to parse JSON: %d\n", r);
		return;
	}

  if (jsoneq(JSON_STRING, &t[1], "url") == 0) {
    str = JSON_STRING + t[2].start;
    size = t[2].end-t[2].start;
    str[size] = '\0';
    url = strdup(str);
    url_parser(url, uinfo);
    free(url);
  }
}

static zlg_mqtt_info* zlg_iot_get_mqtt_info(zlg_mqtt_info* info, const char* protocol, const char* hostname, const char* path, int port) {
    IoT_Error_t err = 0;
    size_t buff_size = 0;
    zlg_iot_http_client_t c;
    unsigned char buff[ZLG_IOT_HTTP_BUFF_SIZE+1];

    if(NULL == info || NULL == hostname || path == NULL) {
        return NULL;
    }

    memset(&c, 0x00, sizeof(c));
    
    c.ctx = info;
    c.protocol = protocol;
    c.hostname = hostname; 
    c.port = port;
    c.path = path;
    c.on_body = mqtt_info_on_body;

    buff_size = prepare_request(info, (char*)buff, ZLG_IOT_HTTP_BUFF_SIZE);
    buff[ZLG_IOT_HTTP_BUFF_SIZE] = '\0';

    err = zlg_iot_https_post(&c, buff, buff_size, "application/json");

    return (err == SUCCESS && c.status == 200) ? info : NULL;
}

zlg_mqtt_info* zlg_iot_http_get_mqtt_info(zlg_mqtt_info* info, const char* hostname, const char* path, int port) {
  return zlg_iot_get_mqtt_info(info, "http", hostname, path, port);
}

zlg_mqtt_info* zlg_iot_https_get_mqtt_info(zlg_mqtt_info* info, const char* hostname, const char* path, int port) {
  return zlg_iot_get_mqtt_info(info, "https", hostname, path, port);
}

IoT_Error_t zlg_iot_download_firmware(const char* url, const char* token, zlg_iot_http_client_on_body on_data, void *ctx) {
  IoT_Error_t err = 0;
  zlg_iot_http_client_t c;
  url_info uinfo;
  char cookies[1024] = {0x00};

  if (NULL == url || NULL == token || NULL == on_data) {
    return NULL_VALUE_ERROR;
  }

  memset(&c, 0x00, sizeof(c));
  if (SUCCESS != url_parser(url, &uinfo)) {
    IOT_ERROR("parse url error");
    return FAILURE;
  }
  snprintf(cookies, sizeof(cookies), "jwt=%s", token);

  printf("%s %s %s %d\n", uinfo.protocol, uinfo.hostname, uinfo.path, uinfo.port);

  c.hostname = uinfo.hostname;
  c.path = uinfo.path;
  c.port = uinfo.port;
  c.cookie = cookies;
  c.ctx = &uinfo;
  c.on_body = firmware_info_on_body;
  c.protocol = uinfo.protocol;

  err = zlg_iot_https_get(&c);
  if (err != SUCCESS) {
    return FAILURE;
  }

  if (c.status != 200) {
    IOT_ERROR("%s", c.body);
    return FAILURE;
  }

  c.hostname = uinfo.hostname;
  c.path = uinfo.path;
  c.port = uinfo.port;
  c.ctx = ctx;
  c.on_body = on_data;
  c.protocol = uinfo.protocol;

  printf("%s %s %s %d\n", uinfo.protocol, uinfo.hostname, uinfo.path, uinfo.port);

  err = zlg_iot_https_get(&c);
  if (err == SUCCESS && c.status != 200) {
    IOT_ERROR("%s", c.body);
  }

  return err == SUCCESS && c.status == 200 ? SUCCESS : FAILURE;
}
