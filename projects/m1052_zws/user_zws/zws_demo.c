#include "aworks.h"
#include "aw_task.h"
#include "aw_mem.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "app_config.h"
#include "aw_list.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include "client.h"
//#include "device_consts.h"
#include "zlg_iot_config.h"
#include "zlg_iot_log.h"
#include "zlg_iot_version.h"
#include "zlg_iot_http_client.h"
#include "zlg_iot_mqtt_client.h"
#include "zlg_iot_mqtt_client_interface.h"


#ifndef ENDL
    #define ENDL                    "\r\n"
#endif

#define __ZWS_PRINT(fmt, args...) \
            aw_kprintf(fmt ENDL, ##args)


struct __demo_zws {

    const char *dev_type;           /* 云设备类型 */
    const char *dev_id;             /* 云设备ID */
    const char *dev_key;            /* 云设备密钥 */

    zlg_mqtt_info   info;           /* MQTT信息对象 */
    ZLG_IoT_Client  mqclient;       /* IoT客户端 */
    client_t        a_device;       /* 本地设备客户端*/

    mqtt_client_t   mq;             /* MQTT连接客户端 */
    char            client_id[64];

    char            topic[64];

    aw_task_id_t    task;

    struct aw_list_head *sensor_list;
    char            *wifi_status;
};


aw_local aw_err_t __mqtt_info_update (struct __demo_zws *p_this);
BOOL mqtt_publish(mqtt_client_t *client, int qos, int r, const char *topic, const char *buff, size_t size);

extern BOOL sdk_demo_devtype_get_all_data(params_t* params,
                                               struct aw_list_head *sensor_list,
                                               char *keybuff);

aw_local void __disconnect_cb (ZLG_IoT_Client *pClient, void *data)
{
    IoT_Error_t rc = FAILURE;

    __ZWS_PRINT("MQTT Disconnect !");

    if (NULL == pClient) {
        return;
    }

    if (zlg_iot_is_autoreconnect_enabled(pClient)) {
        __ZWS_PRINT("Auto Reconnect is enabled, Reconnecting attempt will start now.");
    } else {
        __ZWS_PRINT("Auto Reconnect not enabled. Starting manual reconnect...");
        rc = zlg_iot_mqtt_attempt_reconnect(pClient);
        if (NETWORK_RECONNECTED == rc) {
            __ZWS_PRINT("Manual Reconnect Successful");
        } else {
            __ZWS_PRINT("Manual Reconnect Failed - %d", rc);
        }
    }
}


aw_local void __reconnect_cb (ZLG_IoT_Client *pClient, void *data)
{
    struct __demo_zws *p_this = (struct __demo_zws *)data;

    __ZWS_PRINT("MQTT Will Reconnect...");
    if (NULL == pClient) {
        return;
    }

    if (__mqtt_info_update(p_this) == AW_OK) {
        pClient->clientData.options.pPassword = p_this->info.token;
        pClient->clientData.options.passwordLen = (uint16_t)strlen(pClient->clientData.options.pPassword);
    } else {
        __ZWS_PRINT("reconnect failed.");
    }
}


static void on_message(ZLG_IoT_Client *pClient, char *topicName, uint16_t topicNameLen,
                       IoT_Publish_Message_Params *params, void *pData)
{
  char topic[64] = {0x00};
  const char *payload = params->payload;
  size_t payloadlen = params->payloadLen;
  struct __demo_zws *p_this = (struct __demo_zws *)pData;

  assert(topicNameLen < sizeof(topic));
  if (topicNameLen >= sizeof(topic))
  {
    printf("topic is too long\n");
    return;
  }

  strncpy(topic, topicName, topicNameLen);
  printf("on message: %s\n", topic);

  if (strstr(topic, "raw")) {
    printf("receive raw data: %s\n", payload);
  } else if (strcmp(topic, STR_TOPIC_BROADCAST) == 0) {
    printf("broadcast message %s\n", topic);
    client_dispatch(&p_this->a_device, payload, payloadlen);
  } else if (strstr(topic, p_this->a_device.type) || strstr(topic, p_this->a_device.id)) {
    client_dispatch(&p_this->a_device, payload, payloadlen);
  } else {
    printf("receive data: %s\n", payload);
  }

  IOT_UNUSED(pData);
  IOT_UNUSED(pClient);
}


extern command_t* sdk_demo_devtype_get_commands (void);
extern settings_t* settings_get_instance();

aw_local void __device_create (struct __demo_zws *p_this)
{
    params_t    params;
    char        buff[256] = {0};
    IoT_Error_t ret;

    p_this->mq.ctx     = &p_this->mqclient;
    p_this->mq.publish = mqtt_publish;

    /* init client device */
    client_init(&p_this->a_device, p_this->info.owner, p_this->dev_type, p_this->dev_id);
    client_set_cmds(&p_this->a_device, sdk_demo_devtype_get_commands());
    client_set_settings(&p_this->a_device, settings_get_instance());
    client_set_mqtt_client(&p_this->a_device, &p_this->mq);

    /* register a topic(server send message to device) */
    snprintf(p_this->topic, sizeof(p_this->topic), STR_TOPIC_DEVICE,
             p_this->a_device.owner, p_this->a_device.type, p_this->a_device.id);

    ret = zlg_iot_mqtt_subscribe(&p_this->mqclient,
                                 p_this->topic,
                                 strlen(p_this->topic),
                                 QOS1,
                                 on_message,
                                 p_this);
    if (ret != SUCCESS) {
        __ZWS_PRINT("register topic failed %d.", ret);
    }

    request_init(&params, buff, sizeof(buff));
    params_append_string(&params, "clientip",p_this->info.clientip);
    /*add other status info at here*/
    client_online(&p_this->a_device, &params);

    request_init(&params, buff, sizeof(buff));
    params_append_string(&params, STR_KEY_VERSION, "V1.0.0");
    /*add other status info at here*/
    client_report_status(&p_this->a_device, &params);
}


aw_local aw_err_t __mqtt_info_update (struct __demo_zws *p_this)
{
    const char *device_ids[1];
    const char *device_types[1];

    if (p_this->info.token) {
        zlg_mqtt_info_deinit(&p_this->info);
    }

    /* get ZWS MQTT information by HTTP */
    zlg_mqtt_info_init(&p_this->info);

    device_ids[0]   = p_this->dev_id;
    device_types[0] = p_this->dev_type;

    /* 传入云设备相关配置信息 */
    p_this->info.username     = p_this->dev_type;
    p_this->info.password     = p_this->dev_key;
    p_this->info.devices_nr   = 1;
    p_this->info.device_ids   = device_ids;
    p_this->info.device_types = device_types;

    /* 通过HTTP获取MQTT设备信息 */
    if (zlg_iot_https_get_mqtt_info(&p_this->info,
                                    ZLG_IOT_AUTH_HOST,
                                    ZLG_IOT_TERMINAL_AUTH_PATH,
                                    ZLG_IOT_AUTH_PORT) == NULL) {
        printf("get mqtt info error, line:%d\r\n", __LINE__);
        return AW_ERROR;
    }

    printf("host:%s\n",     p_this->info.host);
    printf("port:%d\n",     p_this->info.port);
    printf("sslport:%d\n",  p_this->info.sslport);
    printf("uuid:%s\n",     p_this->info.uuid);
    printf("clientip:%s\n", p_this->info.clientip);
    printf("owner:%s\n",    p_this->info.owner);

    return AW_OK;
}



aw_local aw_err_t __iot_client_create (struct __demo_zws *p_this)
{
    IoT_Client_Init_Params    init_params    = iotClientInitParamsDefault;
    IoT_Client_Connect_Params connect_params = iotClientConnectParamsDefault;
    IoT_Error_t             ret;

    /* create IoT client */
    init_params.enableAutoReconnect       = false;
    init_params.pHostURL                  = p_this->info.host;
    init_params.port                      = p_this->info.port;
    init_params.mqttCommandTimeout_ms     = 10000;
    init_params.tlsHandshakeTimeout_ms    = 1000;
    init_params.isSSLHostnameVerify       = false;
    init_params.disconnectHandlerData     = NULL;
    init_params.disconnectHandler         = __disconnect_cb;
    init_params.reconnectingHandler       = __reconnect_cb;
    init_params.reconnectingHandlerData   = p_this;
    init_params.reconnectedHandler        = NULL;
    init_params.reconnectedHandlerData    = NULL;

    ret = zlg_iot_mqtt_init(&p_this->mqclient, &init_params);
    if (SUCCESS != ret) {
        __ZWS_PRINT("ZLG IoT MQTT init failed %d.", ret);
        return AW_ERROR;
    }

    snprintf(p_this->client_id,
             sizeof(p_this->client_id),
             "%s:%s",
             p_this->dev_type,
             p_this->dev_id);

    connect_params.isCleanSession          = true;
    connect_params.isWillMsgPresent        = false;
    connect_params.MQTTVersion             = MQTT_3_1_1;
    connect_params.keepAliveIntervalInSec  = 600;
    connect_params.pClientID               = p_this->client_id;
    connect_params.clientIDLen             = (uint16_t)strlen(p_this->client_id);
    connect_params.pUsername               = p_this->info.clientip;
    connect_params.usernameLen             = (uint16_t)strlen(p_this->info.clientip);
    connect_params.pPassword               = p_this->info.token;
    connect_params.passwordLen             = (uint16_t)strlen(p_this->info.token);

    ret = zlg_iot_mqtt_connect(&p_this->mqclient, &connect_params);
    if (SUCCESS != ret) {
        __ZWS_PRINT("connecting to %s:%d failed %d",
                    init_params.pHostURL,
                    init_params.port, ret);
        return ret;
    }

    ret = zlg_iot_mqtt_autoreconnect_set_status(&p_this->mqclient, true);
    if (SUCCESS != ret) {
        __ZWS_PRINT("Unable to set Auto Reconnect to true - %d", ret);
        return ret;
    }

    return AW_OK;
}

aw_local void __zws_task_handle (void *p_arg)
{
    struct __demo_zws      *p_this = (struct __demo_zws *)p_arg;

    IoT_Error_t             rc = SUCCESS;
    aw_err_t                ret = AW_OK;
    char                    buff[2048];             /* 所有需要上报的数据的缓存，当前配置下单次最大上报2K字节 */
    params_t                params;

    uint32_t                keybuff_size = 512;     /* 重组关键字的缓存 */
    char                   *keybuff = NULL;
    keybuff = (char *)aw_mem_alloc(keybuff_size);
    if (keybuff == NULL) {
        __ZWS_PRINT("\r\nThere is no enough memory for keywords reform!\r\n");
        return;
    }
    memset((void *)keybuff, 0x00, keybuff_size);

__retry:
    if (ret != AW_OK) {
        aw_mdelay(1000);
    }

    /* 更新mqtt信息 */
    ret = __mqtt_info_update(p_this);
    if (ret != AW_OK) {
        goto __retry;
    }

    /* 创建云设备上报数据的客户端*/
    ret = __iot_client_create(p_this);
    if (ret != AW_OK) {
        goto __retry;
    }

    /* 创建云设备处理服务器下发命令的客户端 */
    __device_create(p_this);

    /* ZWS云应用主循环 */
    while ((NETWORK_ATTEMPTING_RECONNECT == rc ||
            NETWORK_RECONNECTED == rc ||
            SUCCESS == rc)) {

        if (*p_this->wifi_status == WIFI_CONNECTED) {
            request_init(&params, buff, sizeof(buff));

            /* 更新所有需要上报的数据到params对象中*/
            sdk_demo_devtype_get_all_data(&params,              /* params参数对象 */
                                          p_this->sensor_list,  /* 传感器设备链表 */
                                          keybuff);             /* 关键字重组缓存 */
            /* 上报数据 */
            client_report_data(&p_this->a_device, &params);

            /* 接收云端下发的设备控制命令并处理 */
            rc = zlg_iot_mqtt_yield(&p_this->mqclient, 2 * 1000);
            if (NETWORK_ATTEMPTING_RECONNECT == rc) {
                continue;
            }
        } else if (*p_this->wifi_status == WIFI_DISCONNECT) {
//            __ZWS_PRINT("\r\nWi-Fi was disconnect...\r\n");
            aw_mdelay(5000);
        } else {
            __ZWS_PRINT("\r\nWi-Fi status error!...\r\n");
            aw_mdelay(5000);
        }
    }
}



void *demo_zws (struct app *p_dev)
{
    struct __demo_zws *p_this;

    /* 创建云设备对象 */
    p_this = (struct __demo_zws *)aw_mem_alloc(sizeof(*p_this));
    if (p_this == NULL) {
        __ZWS_PRINT("no memory for object.");
        return NULL;
    }
    memset(p_this, 0, sizeof(*p_this));

    /* 获取云设备信息 */
    p_this->dev_type = (const char *)&p_dev->p_cfg->zws_devinfo->type;
    p_this->dev_id   = (const char *)&p_dev->p_cfg->zws_devinfo->id;
    p_this->dev_key  = (const char *)&p_dev->p_cfg->zws_devinfo->key;

    /* 获取传感器设备链表 */
    p_this->sensor_list = &p_dev->p_cfg->sensor_head;
    /* 获取WIFI连接状态 */
    p_this->wifi_status = &p_dev->p_cfg->net->connect_status;

    /* 创建云设备任务 */
    p_this->task = aw_task_create("zws_task", 10, 32 * 1024, (aw_task_func_t)__zws_task_handle, p_this);
    if (p_this->task == NULL) {
        goto __failed;
    }

    aw_task_startup(p_this->task);

    return p_this;

__failed:
  if (p_this) {
      if (p_this->task) {
          aw_task_terminate(p_this->task);
      }
      aw_mem_free(p_this);
  }
  return NULL;
}
