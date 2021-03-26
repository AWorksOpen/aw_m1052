/** 
 * 功能说明：
 *     1.MQTT客户端接口定义。
 *
 * 修改历史：
 *     1.2017-8-5 李先静<lixianjing@zlg.cn> 创建。
 */

#include "types_def.h"

#ifndef DEVICE_MQTT_CLIENT_H
#define DEVICE_MQTT_CLIENT_H

struct _mqtt_client_t;
typedef struct _mqtt_client_t mqtt_client_t;

typedef BOOL (*mqtt_publish_t)(mqtt_client_t* mqtt, int qos, int retain, 
        const char* topic, const char* buff, size_t size);

struct _mqtt_client_t {
    mqtt_publish_t publish;
    void* ctx;
};

static inline BOOL mqtt_client_publish(mqtt_client_t* mqtt, int qos, int retain, 
        const char* topic, const char* buff, size_t size) {
    return_value_if_fail(mqtt != NULL && topic != NULL && buff != NULL, -1);

    return mqtt->publish(mqtt, qos, retain, topic, buff, size);
}

#endif/*DEVICE_MQTT_CLIENT_H*/

