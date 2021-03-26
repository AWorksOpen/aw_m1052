/** 
 * 功能说明：
 *     1.用户演示获取mqtt服务器相关信息。
 *
 * 修改历史：
 *     1.2017-11-24 李先静<lixianjing@zlg.cn> 创建。
 */

#include "zlg_iot_http_client.h"

int main(int argc, char* argv[]) {
    zlg_mqtt_info info;
    const char* username = NULL;
    const char* password = NULL;
    const char* device_ids[] = {"11111111", "22222222"};
    const char* device_types[] = {"inverter", "collector"};

    if(argc < 3) {
        printf("Usage: username password\n");

        return 0;
    }

    username = argv[1];
    password = argv[2];

    zlg_mqtt_info_init(&info);

    info.username = username;
    info.password = password;
    info.devices_nr = 2;
    info.device_ids = device_ids;
    info.device_types = device_types;

    if(zlg_iot_https_get_mqtt_info(&info, ZLG_IOT_AUTH_HOST, ZLG_IOT_TERMINAL_AUTH_PATH, ZLG_IOT_AUTH_PORT)) {
        printf("host:%s\n", info.host);
        printf("port:%d\n", info.port);
        printf("uuid:%s\n", info.uuid);
        printf("clientip:%s\n", info.clientip);
        printf("owner:%s\n", info.owner);
    }

    zlg_mqtt_info_deinit(&info);

    return 0;
}
