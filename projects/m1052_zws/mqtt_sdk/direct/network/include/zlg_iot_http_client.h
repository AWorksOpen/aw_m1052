/** 
 * 功能说明：
 *     1.HTTP 协议简单包装。
 *
 * 修改历史：
 *     1.2017-11-24 李先静<lixianjing@zlg.cn> 创建。
 */

#ifndef ZLG_IOT_SDK_SRC_IOT_HTTP_CLIENT_H
#define ZLG_IOT_SDK_SRC_IOT_HTTP_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "zlg_iot_error.h"
#include "zlg_iot_config.h"

#include "network_interface.h"
#include "timer_interface.h"

#ifdef _ENABLE_THREAD_SUPPORT_
#include "threads_interface.h"
#endif

struct _zlg_iot_http_client_t;
typedef  struct _zlg_iot_http_client_t zlg_iot_http_client_t;

typedef void (*zlg_iot_http_client_on_header)(zlg_iot_http_client_t* c);
typedef void (*zlg_iot_http_client_on_body)(zlg_iot_http_client_t* c);

struct _zlg_iot_http_client_t {
    unsigned char* body;
    size_t offset;
    size_t size;

    const char* cookie;

    /*response header*/
    int status;
    size_t content_length;
    char content_type[32];

    /*url*/
    int port;
    const char* path;
    const char* protocol;
    const char* hostname;
   
    /*callback*/
    void *ctx;
    zlg_iot_http_client_on_body on_body;
    zlg_iot_http_client_on_header on_header;
};

IoT_Error_t zlg_iot_https_get(zlg_iot_http_client_t* c);
IoT_Error_t zlg_iot_https_post(zlg_iot_http_client_t* c, unsigned char* body, size_t body_size, 
        const char* type);

typedef struct _zlg_mqtt_info {
    /*input*/
    const char* username;
    const char* password;

    size_t devices_nr;
    const char** device_ids;
    const char** device_types;

    /*output*/
    char host[32];
    int port;
    int sslport;
    char clientip[32];
    char uuid[40];
    char* token;
    char owner[32];
} zlg_mqtt_info;

void zlg_mqtt_info_init(zlg_mqtt_info* info);
void zlg_mqtt_info_deinit(zlg_mqtt_info* info);

zlg_mqtt_info* zlg_iot_http_get_mqtt_info(zlg_mqtt_info* info, const char* hostname, const char* path, int port);
zlg_mqtt_info* zlg_iot_https_get_mqtt_info(zlg_mqtt_info* info, const char* hostname, const char* path, int port);

IoT_Error_t zlg_iot_download_firmware(const char* url, const char* token, zlg_iot_http_client_on_body on_data, void *ctx);

#ifdef __cplusplus
}
#endif

#endif