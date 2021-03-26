#include "aw_wifi.h"
#include "lwip/inet.h"
#include "stdio.h"
#include "aw_netif.h"
#include "dhcp_server.h"

#define APP_WIFI_AP_IF_NAME             "ap"

void app_wifi_ap_entry(void){

    aw_wifi_id_t id;
    uint32_t stat;
    aw_err_t err;
    aw_wifi_ssid_t ssid;
    aw_wifi_key_t pkey;
    char wifi_name[12] = "AWWIFI";          /*wifi名称*/
    char wifi_password[12] = "meiyoumima";    /*wifi密码*/
    aw_wifi_security_t security = AW_WIFI_SECURITY_WPA2_AES_PSK;    /*wifi加密类型*/
    uint8_t channel = 6;    /*wifi信道*/

    ssid.length = snprintf((char *) ssid.value, sizeof(ssid.value), "%s", wifi_name);
    pkey.length = snprintf((char *) pkey.value, sizeof(pkey.value), "%s", wifi_password);

    /*wifi43362 在 awbl_hwconf_cyw43362_wifi_spi.h 配置文件中查看wifi名称*/
    id = aw_wifi_dev_open("wifi43362"); /*打开wifi设备*/
    if (id != AW_WIFI_ID_INVALID) {

        /*配置wifi*/
        err = aw_wifi_ioctl(id, AW_WO_OPT_GET, AW_WO_DEV_STAT, (void *) &stat, NULL);
        if (err != AW_OK){
            aw_kprintf("Event Registry Failed:%d", err);
        }

        /*开启wifi热点*/
        err = aw_wifi_start_ap(id, &ssid, security, &pkey, channel);
        if (AW_OK != err) {
            aw_kprintf("Failed.");
            return ;
        }

        aw_wifi_mac_t mac;
        struct in_addr ip;
        struct in_addr nm;
        struct in_addr gw;
        aw_local struct dhcp_server dhcp_server;

        aw_kprintf("Started. SSID:%s, ch:%d", ssid.value, channel);

        dhcps_bind_netif(&dhcp_server, APP_WIFI_AP_IF_NAME);
        dhcps_start(&dhcp_server);

        aw_netif_hwaddr_get_by_name(APP_WIFI_AP_IF_NAME, mac.octet, 6);
        aw_netif_ipv4_addr_get_by_name(APP_WIFI_AP_IF_NAME, &ip, &nm, &gw);
        aw_kprintf("%s" "/ ", inet_ntoa(ip));
        aw_kprintf("%s" " -> ", inet_ntoa(nm));
        aw_kprintf("%s" " @ ", inet_ntoa(gw));
//        aw_kprintl("%s", APP_PRINT_HWADDR_VAL(mac.octet));
    }

    aw_wifi_dev_close(id);

}
