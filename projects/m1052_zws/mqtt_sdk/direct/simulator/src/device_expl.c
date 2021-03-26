/** 
 * 功能说明：
 *     1.模拟设备。只用于示例，切不可直接拷贝用于实际开发中。
 *
 * 修改历史：
 *     1.2017-11-24 李先静<lixianjing@zlg.cn> 创建。
 *     2.2019-04-24 王文坤<wangwenkun@zlg.cn> 修改。
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include "client.h"
#include "device.h"

#include "zlg_iot_config.h"
#include "zlg_iot_version.h"
#include "zlg_iot_http_client.h"
#include "zlg_iot_mqtt_client.h"
#include "zlg_iot_mqtt_client_interface.h"

#include "log.h"

/*设备对象*/
static client_t a_device;
static zlg_mqtt_info mqtt_info;

BOOL mqtt_publish(mqtt_client_t *client, int qos, int r, const char *topic, const char *buff, size_t size);

static void on_message(ZLG_IoT_Client *pClient, char *topicName, uint16_t topicNameLen,
                       IoT_Publish_Message_Params *params, void *pData) {
  char topic[64] = {0x00};
  const char *payload = params->payload;
  size_t payloadlen = params->payloadLen;

  assert(topicNameLen < sizeof(topic));
  if (topicNameLen >= sizeof(topic)) {
    LOG_WARN("topic is too long");
    return;
  }

  strncpy(topic, topicName, topicNameLen);
  LOG_INFO("on message: %s", topic);
  
  if (strstr(topic, "raw")) {
  	LOG_INFO("receive raw data: %s", payload);
  } else if (strcmp(topic, STR_TOPIC_BROADCAST) == 0) {
    LOG_INFO("broadcast message %s", topic);
    client_dispatch(&a_device, payload, payloadlen);
  } else if (strstr(topic, a_device.type) || strstr(topic, a_device.id)) {
    client_dispatch(&a_device, payload, payloadlen);
  } else {
  	LOG_INFO("receive data: %s", payload);
  }

  IOT_UNUSED(pData);
  IOT_UNUSED(pClient);
}

static void client_register_topics(client_t *c, ZLG_IoT_Client *mq) {
  int qos = QOS1;
  char topic[64] = {0x00};

  snprintf(topic, sizeof(topic), STR_TOPIC_DEVICE, c->owner, c->type, c->id);
  LOG_INFO("subscrite: %s", topic);
  zlg_iot_mqtt_subscribe(mq, strdup(topic), strlen(topic), qos, on_message, NULL);
  
  snprintf(topic, sizeof(topic), STR_TOPIC_DEVICE_RAW, c->owner, c->type, c->id);
  LOG_INFO("subscrite: %s", topic);
  zlg_iot_mqtt_subscribe(mq, strdup(topic), strlen(topic), qos, on_message, NULL);

  snprintf(topic, sizeof(topic), STR_TOPIC_BROADCAST_OWNER_DEVTYPE, c->owner, c->type);
  LOG_INFO("subscrite: %s", topic);
  zlg_iot_mqtt_subscribe(mq, strdup(topic), strlen(topic), qos, on_message, NULL);

  snprintf(topic, sizeof(topic), STR_TOPIC_BROADCAST_DEVTYPE, c->type);
  LOG_INFO("subscrite: %s", topic);
  zlg_iot_mqtt_subscribe(mq, strdup(topic), strlen(topic), qos, on_message, NULL);

  LOG_INFO("subscrite: %s", STR_TOPIC_BROADCAST);
  zlg_iot_mqtt_subscribe(mq, STR_TOPIC_BROADCAST, strlen(STR_TOPIC_BROADCAST), qos, on_message, NULL);
}

static void disconnectCallbackHandler(ZLG_IoT_Client *pClient, void *data) {
  LOG_WARN("MQTT Disconnect");
  IoT_Error_t rc = FAILURE;

  if (NULL == pClient) {
    return;
  }

  IOT_UNUSED(data);

  if (zlg_iot_is_autoreconnect_enabled(pClient)) {
    LOG_INFO("Auto Reconnect is enabled, Reconnecting attempt will start now");
  } else {
    LOG_WARN("Auto Reconnect not enabled. Starting manual reconnect...");
    rc = zlg_iot_mqtt_attempt_reconnect(pClient);
    if (NETWORK_RECONNECTED == rc) {
      LOG_WARN("Manual Reconnect Successful");
    } else {
      LOG_WARN("Manual Reconnect Failed - %d", rc);
    }
  }
}

/*登录认证服务器获取MQTT服务器的信息*/
static BOOL get_mqtt_info(zlg_mqtt_info *info, const char* hostname, const char* path, int port, dev_meta_info_t *meta) {
  char *device_ids[1];
  char *device_types[1];
  
  device_ids[0] = meta->id;
  device_types[0] = meta->type;

  info->username = meta->type;
  info->password = meta->secret;
  info->devices_nr = 1;
  info->device_ids = device_ids;
  info->device_types = device_types;

  if (zlg_iot_https_get_mqtt_info(info, hostname, path, port)) {
    LOG_INFO("host:%s", info->host);
    LOG_INFO("port:%d", info->port);
    LOG_INFO("sslport:%d", info->sslport);
    LOG_INFO("uuid:%s", info->uuid);
    LOG_INFO("clientip:%s", info->clientip);
    LOG_INFO("owner:%s", info->owner);

    return TRUE;
  }

  return FALSE;
}

/*断开重连【时】重新登录认证服务器获取新的MQTT登录密码（MQTT的登录密码具有时效性）*/
static void reconnnectingCallbackHandler(ZLG_IoT_Client *pClient, void *data) {
  LOG_INFO("MQTT Will Reconnecting");

  dev_meta_info_t *meta = (dev_meta_info_t *)data;

  if (NULL == pClient || data == NULL) {
    return;
  }

  get_mqtt_info(&mqtt_info, ZLG_IOT_AUTH_HOST, ZLG_IOT_TERMINAL_AUTH_PATH, ZLG_IOT_AUTH_PORT, meta);

  pClient->clientData.options.pPassword = mqtt_info.token;
  pClient->clientData.options.passwordLen = (uint16_t)strlen(pClient->clientData.options.pPassword);
}

/*重连【成功后】的回调函数*/
static void reconnectedCallbackHandler(ZLG_IoT_Client *pClient, void *data) {
  LOG_INFO("MQTT Reconnected");

  params_t params;
  char buff[256] = {0};
  client_t *c = (client_t *)data;

  if (NULL == pClient || data == NULL) {
    return;
  }

  //TODO
  request_init(&params, buff, sizeof(buff));
  params_append_string(&params, STR_KEY_CLIENTIP, mqtt_info.clientip);
  client_online(c, &params);
}

static void init_devices(ZLG_IoT_Client *mqclient, mqtt_client_t *mq, const char *owner, const char *devid) {
  params_t params;
  char buff[256] = {0};

  myinvert_init(&a_device, owner, devid);

  client_set_mqtt_client(&a_device, mq);

  client_register_topics(&a_device, mqclient);

  request_init(&params, buff, sizeof(buff));
  params_append_string(&params, STR_KEY_CLIENTIP, mqtt_info.clientip);
  client_online(&a_device, &params);
}

int main(int argc, const char* argv[]) {
  char buff[512];
  char client_id[64] = {0x00};
  dev_meta_info_t meta;
  params_t params;
  mqtt_client_t mq;
  ZLG_IoT_Client mqclient;
  IoT_Error_t rc = FAILURE;
  IoT_Client_Init_Params mqttInitParams = iotClientInitParamsDefault;
  IoT_Client_Connect_Params connectParams = iotClientConnectParamsDefault;

  mq.ctx = &mqclient;
  mq.publish = mqtt_publish;

  LOG_INFO("Usage: %s devtype devid devsecret", argv[0]);
  LOG_INFO("Readme: The basetype must be \"invert\", type must be \"myinvert\"");

  if (argc < 4) {
    LOG_FATAL("Invalid arguments");
  }

  memset(&meta, 0x00, sizeof(meta));
  strncpy(meta.type, argv[1], strlen(argv[1])); //设备类型
  strncpy(meta.id, argv[2], strlen(argv[2])); //设备ID
  strncpy(meta.secret, argv[3], strlen(argv[3])); //设备密钥

  zlg_mqtt_info_init(&mqtt_info);

  if (!get_mqtt_info(&mqtt_info, ZLG_IOT_AUTH_HOST, ZLG_IOT_TERMINAL_AUTH_PATH, ZLG_IOT_AUTH_PORT, &meta)) {
    LOG_FATAL("get mqtt info error");
  }

  LOG_INFO("\nZLG IoT SDK Version %d.%d.%d-%s\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_TAG);

  mqttInitParams.enableAutoReconnect = false;
  mqttInitParams.pHostURL = mqtt_info.host;
  mqttInitParams.port = mqtt_info.port;
  mqttInitParams.mqttCommandTimeout_ms = 20000;
  mqttInitParams.tlsHandshakeTimeout_ms = 5000;
  mqttInitParams.isSSLHostnameVerify = false;
  mqttInitParams.disconnectHandlerData = NULL;
  mqttInitParams.disconnectHandler = disconnectCallbackHandler;
  mqttInitParams.reconnectingHandler = reconnnectingCallbackHandler;
  mqttInitParams.reconnectingHandlerData = &meta;
  mqttInitParams.reconnectedHandler = reconnectedCallbackHandler;
  mqttInitParams.reconnectedHandlerData = &a_device;
  //mqttInitParams.interfaceName = "eth0";  //绑定指定的网卡

  rc = zlg_iot_mqtt_init(&mqclient, &mqttInitParams);
  if (SUCCESS != rc) {
    LOG_ERROR("zlg_iot_mqtt_init returned error : %d ", rc);
    return rc;
  }

  memset(client_id, 0x00, sizeof(client_id));
  snprintf(client_id, sizeof(client_id), "%s:%s", meta.type, meta.id);

  connectParams.isCleanSession = true;
  connectParams.isWillMsgPresent = false;
  connectParams.MQTTVersion = MQTT_3_1_1;
  connectParams.keepAliveIntervalInSec = 60;
  connectParams.pClientID = client_id; /*注意clientId必须为devtype:devid的格式*/
  connectParams.clientIDLen = (uint16_t)strlen(connectParams.pClientID);
  connectParams.pUsername = mqtt_info.clientip;
  connectParams.usernameLen = (uint16_t)strlen(connectParams.pUsername);
  connectParams.pPassword = mqtt_info.token;
  connectParams.passwordLen = (uint16_t)strlen(connectParams.pPassword);

  LOG_INFO("Connecting...");
  rc = zlg_iot_mqtt_connect(&mqclient, &connectParams);
  if (SUCCESS != rc) {
    LOG_ERROR("Error(%d) connecting to %s:%d", rc, mqttInitParams.pHostURL, mqttInitParams.port);
    return rc;
  }

  rc = zlg_iot_mqtt_autoreconnect_set_status(&mqclient, true);
  if (SUCCESS != rc) {
    LOG_ERROR("Unable to set Auto Reconnect to true - %d", rc);
    return rc;
  }

  init_devices(&mqclient, &mq, mqtt_info.owner, argv[2]);

  while ((NETWORK_ATTEMPTING_RECONNECT == rc || NETWORK_RECONNECTED == rc || SUCCESS == rc)) {
    /*处理来自服务器的消息，以及定时发送心跳包*/
    rc = zlg_iot_mqtt_yield(&mqclient, 200);
    if (NETWORK_ATTEMPTING_RECONNECT == rc) {
      // If the mqclient is attempting to reconnect we will skip the rest of the loop.
      continue;
    }

    //示例中对大部分函数的返回值都不做处理，调用者实际使用中应该要根据返回值进行相应的处理
    //比如client_report_data返回false时，表明该消息发送失败，应做相应重发处理

    request_init(&params, buff, sizeof(buff));
    myinvert_get_all_data(&params);
    if (!client_report_data(&a_device, &params)) {
      //发送失败重发，这里只是简单示例，具体重发机制由调用者自行实现（也可以在mqtt_client.c文件处理重发）。
      client_report_data(&a_device, &params);
    }

    request_init(&params, buff, sizeof(buff));
    params_append_string(&params, STR_KEY_CLIENTIP, "127.0.0.1");
    if (!client_report_status(&a_device, &params)) {
      client_report_status(&a_device, &params);
    }
  }

  if (SUCCESS != rc) {
    LOG_ERROR("An error occurred in the loop:%d", rc);
  } else {
    LOG_INFO("Quit");
  }

  zlg_mqtt_info_deinit(&mqtt_info);

  return rc;
}
