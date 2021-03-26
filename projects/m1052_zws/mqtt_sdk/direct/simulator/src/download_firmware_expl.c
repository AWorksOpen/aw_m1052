/** 
 * 功能说明：
 *     1.模拟固件下载。只用于示例，切不可直接拷贝用于实际开发中。
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

static zlg_mqtt_info mqtt_info;

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

void on_data(zlg_iot_http_client_t* c) {
  LOG_INFO("firmware content:");
  LOG_INFO("%s", c->body);
}

int main(int argc, const char* argv[]) {
  IoT_Error_t rc = FAILURE;
  dev_meta_info_t meta;

  LOG_INFO("Usage: %s download_url type id secret", argv[0]);

  if (argc < 5) {
    LOG_FATAL("invalid arguments");
  }

  memset(&meta, 0x00, sizeof(meta));
  strncpy(meta.type, argv[2], strlen(argv[2]));
  strncpy(meta.id, argv[3], strlen(argv[3]));
  strncpy(meta.secret, argv[4], strlen(argv[4]));

  zlg_mqtt_info_init(&mqtt_info);

  if (!get_mqtt_info(&mqtt_info, ZLG_IOT_AUTH_HOST, ZLG_IOT_TERMINAL_AUTH_PATH, ZLG_IOT_AUTH_PORT, &meta)) {
    LOG_FATAL("get mqtt info error");
  }

  rc = zlg_iot_download_firmware(argv[1], mqtt_info.token, on_data, NULL);
  if (rc != SUCCESS) {
    LOG_ERROR("download firmware failure");
  }

  zlg_mqtt_info_deinit(&mqtt_info);

  return rc;
}