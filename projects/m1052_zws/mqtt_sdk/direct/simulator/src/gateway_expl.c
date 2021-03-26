/** 
 * 功能说明：
 *     1.模拟网关。只用于示例，切不可直接拷贝用于实际开发中。
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

/*网关对象*/
static client_t a_gateway;
static zlg_mqtt_info mqtt_info;

/*子设备信息*/
static const char *sub_type = "myinvert";
static const char *sub_id = "sdk_sub_id";

BOOL mqtt_publish(mqtt_client_t *client, int qos, int r, const char *topic, const char *buff, size_t size);

static void on_message(ZLG_IoT_Client *pClient, char *topicName, uint16_t topicNameLen, IoT_Publish_Message_Params *params, void *pData) {
  char topic[256] = {0x00};
  const char *payload = params->payload;
  size_t payloadlen = params->payloadLen;

  assert(topicNameLen < sizeof(topic));
  if (topicNameLen >= sizeof(topic)) {
    LOG_WARN("topic is too long");
    return;
  }

  strncpy(topic, topicName, topicNameLen);
  LOG_INFO("on_message: %s", topic);

  if (strstr(topic, STR_TOPIC_GATEWAY_PREFIX)) {
    client_t a_device;
    char sub_topic[64] = {0x00};

    myinvert_init(&a_device, a_gateway.owner, sub_id);
    client_set_mqtt_client(&a_device, a_gateway.mqtt_client);

    strcpy(sub_topic, strstr(topic, "/s2d"));
    LOG_INFO("handle subdevice topics: %s", sub_topic);
    //TODO
  } else {
    client_dispatch(&a_gateway, payload, payloadlen);
  }

  IOT_UNUSED(pData);
  IOT_UNUSED(pClient);
}

static void client_register_topics(client_t *c, ZLG_IoT_Client *mq) {
  int qos = QOS1;
  char topic[64] = {0x00};

  /*订阅网关相关的主题*/
  snprintf(topic, sizeof(topic), STR_TOPIC_GATEWAY, c->owner, c->type, c->id, "#");
  LOG_INFO("subscrite: %s", topic);
  zlg_iot_mqtt_subscribe(mq, strdup(topic), strlen(topic), qos, on_message, NULL);

  /*订阅设备相关的主题*/
  snprintf(topic, sizeof(topic), STR_TOPIC_DEVICE, c->owner, c->type, c->id);
  LOG_INFO("subscrite: %s", topic);
  zlg_iot_mqtt_subscribe(mq, strdup(topic), strlen(topic), qos, on_message, NULL);
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

static void init_gateway(ZLG_IoT_Client *mqclient, mqtt_client_t *mq, const char *owner, const char *devid) {
  params_t params;
  char buff[256] = {0};

  myzggw_init(&a_gateway, owner, devid);

  client_set_mqtt_client(&a_gateway, mq);

  client_register_topics(&a_gateway, mqclient);

  request_init(&params, buff, sizeof(buff));
  params_append_string(&params, STR_KEY_CLIENTIP, mqtt_info.clientip);
  client_online(&a_gateway, &params);
}

int main(int argc, const char* argv[]) {
  int first = 1;
  char buff[512];
  char client_id[64];
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
  LOG_INFO("Readme: The basetype must be \"zggw\", type must be \"myzggw0\"");

  if (argc < 4) {
    LOG_FATAL("invalid arguments");
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
  mqttInitParams.reconnectedHandlerData = &a_gateway;

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

  init_gateway(&mqclient, &mq, mqtt_info.owner, argv[2]);

  /*子设备数据主题*/
  char sub_data_topic[MAX_TOPIC_LENGTH] = {0};
  snprintf(sub_data_topic, MAX_TOPIC_LENGTH, STR_TOPIC_REPORT_DATA, a_gateway.owner, sub_type, sub_id);

  while ((NETWORK_ATTEMPTING_RECONNECT == rc || NETWORK_RECONNECTED == rc || SUCCESS == rc)) {
    /*处理来自服务器的消息，以及定时发送心跳包*/
    rc = zlg_iot_mqtt_yield(&mqclient, 200);
    if (NETWORK_ATTEMPTING_RECONNECT == rc) {
      // If the mqclient is attempting to reconnect we will skip the rest of the loop.
      continue;
    }

    //示例中对大部分函数的返回值都不做处理，调用者实际使用中应该要根据返回值进行相应的处理
    //比如client_report_data返回false时，表明该消息发送失败，应做相应重发处理

    if (first) {
      first = 0;
      /*网关添加子设备(此步骤非必选，网关也可以不添加子设备)*/
      request_init(&params, buff, sizeof(buff));
      client_bind_subdev(&a_gateway, sub_type, sub_id, &params);
    }

    /*网关上报当前的时间*/
    request_init(&params, buff, sizeof(buff));
    params_append_uint(&params, STR_KEY_TIME, 1563174449);
    client_report_status(&a_gateway, &params);

    /*网关转发子设备的数据主题，其他主题可类比，这里不一一列举*/
    request_init(&params, buff, sizeof(buff));
    myinvert_get_all_data(&params);
    client_forward_message(&a_gateway, sub_data_topic, &params);
  }

  if (SUCCESS != rc) {
    LOG_ERROR("An error occurred in the loop:%d\n", rc);
  } else {
    LOG_INFO("Quit\n");
  }

  zlg_mqtt_info_deinit(&mqtt_info);

  return rc;
}

