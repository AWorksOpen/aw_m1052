/** 
 * 功能说明：
 *     1.设备的客户端
 *
 * 修改历史：
 *     1.2017-8-6 李先静<lixianjing@zlg.cn> 创建。
 */

#include "client.h"
#include "params.h"

static const char* fmt_topic(client_t* client, const char* fmt, char topic[MAX_TOPIC_LENGTH+1]) {
    snprintf(topic, MAX_TOPIC_LENGTH, fmt, client->owner, client->type, client->id);
    topic[MAX_TOPIC_LENGTH] = '\0';

    return topic;
}

client_t* client_init(client_t* client, const char* owner, const char* type, const char* id) {
    return_value_if_fail(client != NULL && id != NULL && type != NULL, NULL);

    memset(client, 0x00, sizeof(client_t));
    client->id = id;
    client->type = type;
    client->owner = owner;

    return client;
}

void client_set_cmds(client_t* client, command_t* cmds) {
    return_if_fail(client != NULL && cmds != NULL);

    client->cmds = cmds;

    return;
}

void client_set_settings(client_t* client, settings_t* settings) {
    return_if_fail(client != NULL && settings != NULL);

    client->settings = settings;
    
    return;
}

void client_set_mqtt_client(client_t* client, mqtt_client_t* mqtt_client) {
    return_if_fail(client != NULL && mqtt_client != NULL);

    client->mqtt_client = mqtt_client;
    
    return;
}

static BOOL client_on_response(client_t* client, params_t* params) {
    const char* name = NULL;
    char topic[MAX_TOPIC_LENGTH + 1] = {0x00};

    return_value_if_fail(client != NULL && params != NULL, FALSE);
    name = params_get(params, STR_KEY_NAME);
    return_value_if_fail(name != NULL, FALSE);

    fmt_topic(client, STR_TOPIC_ONLINE, topic);
    if(strcmp(name, topic) == 0) {
        const char* curent_time = params_get(params, STR_KEY_TIME); 
        if(curent_time) {
            settings_set(client->settings, STR_KEY_TIME, curent_time);
        }
        settings_set(client->settings, STR_KEY_ALREADY_ONLIEN, "1");
        printf("%s:%s Device Already Online ok: %s\n", __func__, name, curent_time);
    } else {
      printf("%s:%s\n", __func__, name);
    }

    return TRUE;
}

static BOOL client_on_command(client_t* client, params_t* params) {
    int32_t i = 0;
    const char* name = NULL;

    return_value_if_fail(client != NULL && params != NULL, FALSE);
    name = params_get(params, STR_KEY_NAME);
    return_value_if_fail(name != NULL, FALSE);
    
    for(i = 0; client->cmds[i].exec; i++) {
        command_t* iter = client->cmds+i;
        if(strcmp(iter->name, name) == 0) {
            return iter->exec(iter, client, params);
        }
    }

    printf("%s: not found %s\n", __func__, name);

    return FALSE;
}

BOOL client_dispatch(client_t* client, const char* buff, size_t size) {
    params_t params;
    BOOL ret = FALSE;
    int32_t type = PACKET_NONE;

    response_init(&params, (char*)buff, size);
    return_value_if_fail(client != NULL && buff != NULL && size > 1, ret);
    return_value_if_fail(params_get_int(&params, STR_KEY_MSG_TYPE, &type), FALSE);

    if(type == PACKET_RESPONSE) {
        ret = client_on_response(client, &params);    
    }else if(type == PACKET_COMMAND) {
        ret = client_on_command(client, &params);    
    }

    return ret;
}

static BOOL client_send(client_t* client, const char* topic_fmt, params_t* params, int qos) {
    int retain = 0;
    char topic[MAX_TOPIC_LENGTH+1];

    fmt_topic(client, topic_fmt, topic);
    return mqtt_client_publish(client->mqtt_client, qos, retain, topic, params->buff, params->size);
}

BOOL client_report_data(client_t* client, params_t* params) {
    return_value_if_fail(client != NULL && params != NULL, FALSE);

    return client_send(client, STR_TOPIC_REPORT_DATA, params, 1);
}

BOOL client_report_raw(client_t* client, const unsigned char* buff, size_t size, BOOL save) {
    BOOL ret = FALSE;
    params_t params;
    raw_header_t header;
    char* temp_buff = NULL;
    return_value_if_fail(client != NULL && buff != NULL, FALSE);

    temp_buff = (char *)calloc(sizeof(raw_header_t) + size, sizeof(char));
    return_value_if_fail(temp_buff != NULL, FALSE);

    header.magic = 6;
    header.version = 1;
    header.save = save ? 1 : 0;
    memcpy(temp_buff, &header, sizeof(raw_header_t));
    memcpy(temp_buff + sizeof(raw_header_t), buff, size);

    params.buff = temp_buff;
    params.size = sizeof(raw_header_t) + size;

    ret = client_send(client, STR_TOPIC_REPORT_RAW, &params, 1);
    free(temp_buff);

    return ret;
}

BOOL client_report_status(client_t* client, params_t* params) {
    return_value_if_fail(client != NULL && params != NULL, FALSE);

    return client_send(client, STR_TOPIC_REPORT_STATUS, params, 1); 
}

BOOL client_report_error(client_t* client, params_t* params) {
    return_value_if_fail(client != NULL && params != NULL, FALSE);

    return client_send(client, STR_TOPIC_REPORT_ERROR, params, 1); 
}

BOOL client_report_warning(client_t* client, params_t* params) {
    return_value_if_fail(client != NULL && params != NULL, FALSE);

    return client_send(client, STR_TOPIC_REPORT_WARNING, params, 1);    
}

BOOL client_report_log(client_t* client, params_t* params) {
    return_value_if_fail(client != NULL && params != NULL, FALSE);

    return client_send(client, STR_TOPIC_REPORT_LOG, params, 1);    
}

BOOL client_online(client_t* client, params_t* params) {
    return_value_if_fail(client != NULL, FALSE);
    return_value_if_fail(params != NULL, FALSE);

    return client_send(client, STR_TOPIC_ONLINE, params, 1); 
}

BOOL client_reply_cmd(client_t* client, int cmdid, BOOL result, const char* msg) {
    params_t params;
    char buff[MAX_MESSAGE_LENGTH];
    return_value_if_fail(client != NULL, FALSE);
    request_init(&params, buff, sizeof(buff));
    return_value_if_fail(params_append_int(&params, STR_KEY_CMDID, cmdid), FALSE);
    return_value_if_fail(params_append_int(&params, STR_KEY_RESULT, result), FALSE);
    return_value_if_fail(params_append_string(&params, STR_KEY_MSG, msg), FALSE);

    return client_send(client, STR_TOPIC_REPORT_RESULT, &params, 1); 
}

BOOL client_offline(client_t* client) {
    params_t params;
    char buff[MAX_MESSAGE_LENGTH];
    return_value_if_fail(client != NULL, FALSE);
    request_init(&params, buff, sizeof(buff));

    return client_send(client, STR_TOPIC_OFFLINE, &params, 1); 
}

BOOL client_bind_subdev(client_t* client, const char* subdev_type, const char* subdev_id, params_t* params) {
    return_value_if_fail(client != NULL, FALSE);
    return_value_if_fail(subdev_type != NULL, FALSE);
    return_value_if_fail(subdev_id != NULL, FALSE);
    return_value_if_fail(params != NULL, FALSE);

    return_value_if_fail(params_append_string(params, STR_KEY_SUBDEV_ID, subdev_id), FALSE);
    return_value_if_fail(params_append_string(params, STR_KEY_SUBDEV_TYPE, subdev_type), FALSE);

    return client_send(client, STR_TOPIC_BIND_SUBDEV, params, 1);
}

BOOL client_unbind_subdev(client_t* client, const char* subdev_type, const char* subdev_id) {
    params_t params;
    char buff[MAX_MESSAGE_LENGTH];
    return_value_if_fail(client != NULL, FALSE);
    return_value_if_fail(subdev_type != NULL, FALSE);
    return_value_if_fail(subdev_id != NULL, FALSE);

    request_init(&params, buff, sizeof(buff));
    return_value_if_fail(params_append_string(&params, STR_KEY_SUBDEV_ID, subdev_id), FALSE);
    return_value_if_fail(params_append_string(&params, STR_KEY_SUBDEV_TYPE, subdev_type), FALSE);


    return client_send(client, STR_TOPIC_BIND_SUBDEV, &params, 1);
}

BOOL client_forward_message(client_t* client, const char* forward_topic, params_t* params) {
    char topic[256] = {0};
    return_value_if_fail(client != NULL, FALSE);
    return_value_if_fail(forward_topic != NULL, FALSE);
    return_value_if_fail(params != NULL, FALSE);

    snprintf(topic, 256, STR_TOPIC_FORWARD_MESSAGE, client->owner, client->type, client->id, forward_topic + 1);

    return mqtt_client_publish(client->mqtt_client, 1, 0, topic, params->buff, params->size);
}
