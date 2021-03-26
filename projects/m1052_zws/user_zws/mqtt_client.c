

#include "mqtt_client.h"
#include "zlg_iot_config.h"
#include "zlg_iot_log.h"
#include "zlg_iot_version.h"
#include "zlg_iot_mqtt_client_interface.h"

static void print_buff(const char* buff, size_t size, const char* topic, int ret) {
    const char* p = buff;
    printf("topic:%s ret:%d content[", topic, ret);
    while(p < (buff + size)) {
        printf("%s:", p);
        p += strlen(p) + 1;
    }
    printf("]\n");
}

BOOL mqtt_publish(mqtt_client_t* stream, int qos, int r, const char* topic, const char* buff, size_t size) {
    IoT_Error_t rc = FAILURE;
    IoT_Publish_Message_Params params;
    ZLG_IoT_Client* mq = (ZLG_IoT_Client*)stream->ctx;

    (void)r;
    params.qos = qos;
    params.isRetained = 0;
    params.payloadLen = size;
    params.payload = (void *)buff;

    rc = zlg_iot_mqtt_publish(mq, topic, strlen(topic), &params);

    print_buff(buff, size, topic, rc);

    return SUCCESS == rc;
}


