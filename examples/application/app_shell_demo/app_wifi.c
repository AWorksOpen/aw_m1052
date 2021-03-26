/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/
/**
 * \file
 * \brief Wi-Fi管理演示例程，本例程适用所有平台。
 *
 * - 说明：
 *   1. 需要在 aw_prj_params.h 里使能Wi-Fi，例如：
 *      1) SDIO Wi-Fi
 *          - AW_DEV_CYW43362_WIFI_SDIO_0
 *      2) SPI Wi-Fi
 *          - AW_DEV_CYW43362_WIFI_SPI_0
 *   2. 在头文件app_config.h中修改APP_BOARD为__BOARD_WIFI
 *
 * - 实验现象：
 *   01. 首先执行 ".test"进入子命令集
 *   02. 执行 "wifi scan" 可以查看附近的可见热点
 *   03. 执行 "wifi scan passive" 可以查看附近的所有热点
 *   04. 执行 "wifi join ZYWIFI meiyoumima" 加入名称为 ZYWIFI, 密码为 meiyoumima 的热点
 *   05. 执行 "wifi getapinfo" 获取已加入的热点的信息（信号强度等）
 *   06. 执行 "wifi leave" 离开已加入的热点
 *   07. 执行 "wifi startap AWWIFI wpa2_aes meiyoumima 6" 在 6 号 频道创建名称为 AWWIFI, 密码为 meiyoumima 的热点
 *   08. 执行 "wifi maxassoc" 获取最多可加入本地创建的热点的客户端的数量
 *   09. 执行 "wifi getstalist" 获取已加入本地创建的热点的客户端信息
 *   10. 执行 "wifi stopap" 关闭已创建的的热点
 *   11. 执行 "wifi getlinkstatus" 获取 Wi-Fi 链路状态
 *
 * - 备注：
 *   只支持open，wpa2_aes，wpa2加密模式。
 *
 * \par 源代码
 * \snippet demo_wifi.c src_demo_wifi
 *
 * \internal
 * \par Modification history
 * - 1.00 18-05-04  phd, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_wifi
 * \copydoc demo_wifi.c
 */

/** [src_demo_wifi] */
#include "aworks.h"
#include "app_config.h"
#ifdef APP_WIFI

#include "aw_delay.h"
#include "aw_shell.h"
#include "aw_vdebug.h"
#include "aw_wifi.h"

#include <stdio.h>
#include <string.h>

#include "aw_dns.h"
#include "aw_hostname.h"
#include "aw_netif.h"
#include "aw_sockets.h"
#include "dhcp_server.h"

#include <stdio.h>
#include <string.h>

#define WIFI_OPERATION_RETRY_MAX    3

#define DEBUG_PING                  1

#ifndef ENDL
    #define ENDL                    "\r\n"
#endif

#define aw_kprintl(fmt, args...) \
    aw_kprintf(fmt ENDL, ##args)

#define shell_printf(args...) \
    aw_shell_printf(sh, ##args)

#define shell_printl(fmt, args...) \
    aw_shell_printf(sh, fmt ENDL, ##args)

#if DEBUG_PING
    #define shell_dbg_printl(fmt, args...) \
        aw_shell_printf(sh, fmt ENDL, ##args)
#else
    #define shell_dbg_printl(...)
#endif

#define APP_PRINT_IPADDR_FMT \
    "%s"

#define APP_PRINT_HWADDR_FMT \
    "%02X:%02X:%02X:%02X:%02X:%02X"

#define APP_PRINT_HWADDR_VAL(addr) \
    (addr)[0], (addr)[1], (addr)[2], (addr)[3], (addr)[4], (addr)[5]


#define SHELL_CHECK_ARGC(exp) do { \
        if (!(exp)) { \
            shell_printl("Failed " #exp); \
            return 0; \
        } \
    } while (0);


#define WIFI_DEVICE_OPEN(id) \
    id = aw_wifi_dev_open(WIFI_DEV_NAME); \
    if (AW_WIFI_ID_INVALID == id) { \
        shell_printl("get wifi device failed."); \
        return 0; \
    }

#define WIFI_DEVICE_CLOSE(id)                aw_wifi_dev_close(id)

#ifndef APP_WIFI_STA_IF_NAME
    #define APP_WIFI_STA_IF_NAME            "st"
#endif

#ifndef APP_WIFI_AP_IF_NAME
    #define APP_WIFI_AP_IF_NAME             "ap"
#endif

#define WIFI_DEV_NAME                       "wifi43362"

typedef struct app_scan_data {
    AW_SEMB_DECL(signal);               /* Semaphore used for signaling scan complete */
    uint32_t     count;                 /* Count to measure the total scan results    */
    struct aw_shell_user *sh;
} app_scan_data_t;

/******************************************************
 *               Static Function Declarations
 ******************************************************/

aw_local aw_err_t __get_sta_result_handler (void               *p_arg,
                                            aw_wifi_sta_info_t *sta,
                                            aw_bool_t              complete);

aw_local void __wifi_softap_event_handler (void                *p_arg,
                                           aw_wifi_ap_event_t   event,
                                           const aw_wifi_mac_t *mac_addr);

/******************************************************
 *               Variable Definitions
 ******************************************************/

aw_local aw_bool_t g_sta_is_assocated = AW_FALSE;

/******************************************************
 *               Function Definitions
 ******************************************************/

aw_local struct dhcp_server *ap_dhcp_server (void)
{
    aw_local struct dhcp_server dhcp_server;
    return &dhcp_server;
}

aw_local aw_wifi_security_t str_to_security (char *str)
{
    if (strcmp(str, "open") == 0) {
        return AW_WIFI_SECURITY_OPEN;
    } else if (strcmp(str, "wep") == 0) {
        return AW_WIFI_SECURITY_WEP_PSK;
    } else if (strcmp(str, "wpa2_tkip") == 0) {
        return AW_WIFI_SECURITY_WPA2_TKIP_PSK;
    } else if (strcmp(str, "wpa2_aes") == 0) {
        return AW_WIFI_SECURITY_WPA2_AES_PSK;
    } else if (strcmp(str, "wpa2") == 0) {
        return AW_WIFI_SECURITY_WPA2_MIXED_PSK;
    } else if (strcmp(str, "wpa_aes") == 0) {
        return AW_WIFI_SECURITY_WPA_AES_PSK;
    } else if (strcmp(str, "wpa_tkip") == 0) {
        return AW_WIFI_SECURITY_WPA_TKIP_PSK;
    } else {
        aw_kprintl("Bad auth type: '%s'", str);
        return AW_WIFI_SECURITY_UNKNOWN;
    }
}

aw_local const char *security_to_str (aw_wifi_security_t security)
{
    char *name;

    switch (security) {

    case AW_WIFI_SECURITY_OPEN:
        name = "Open          ";
        break;

    case AW_WIFI_SECURITY_WEP_PSK:
        name = "WEP PSK       ";
        break;

    case AW_WIFI_SECURITY_WEP_SHARED:
        name = "WEP SHARED    ";
        break;

    case AW_WIFI_SECURITY_WPA_TKIP_PSK:
        name = "WPA  TKIP  PSK";
        break;

    case AW_WIFI_SECURITY_WPA_AES_PSK:
        name = "WPA  AES   PSK";
        break;

    case AW_WIFI_SECURITY_WPA_MIXED_PSK:
        name = "WPA  Mixed PSK";
        break;

    case AW_WIFI_SECURITY_WPA2_AES_PSK:
        name = "WPA2 AES   PSK";
        break;

    case AW_WIFI_SECURITY_WPA2_TKIP_PSK:
        name = "WPA2 TKIP  PSK";
        break;

    case AW_WIFI_SECURITY_WPA2_MIXED_PSK:
        name = "WPA2 Mixed PSK";
        break;

    default:
        name = "Unknown       ";
        break;
    }

    return name;
}


/*!
 ******************************************************************************
 * Prints partial details of a scan result on a single line
 *
 * @param[in] record  A pointer to the wiced_scan_result_t record
 *
 */
aw_local void print_scan_result (aw_wifi_scan_result_t *record)
{
    int i;

    aw_kprintf("%5s", (record->bss_type == AW_WIFI_BSS_TYPE_ADHOC) ? "Adhoc" : "Infra");
    aw_kprintf(" " APP_PRINT_HWADDR_FMT, APP_PRINT_HWADDR_VAL(record->bssid.octet));
    if (record->flags & AW_WIFI_SCAN_RESULT_FLAG_RSSI_OFF_CHANNEL) {
        aw_kprintf("  OFF");
    } else {
        aw_kprintf("  %3d", record->signal_strength);
    }
    aw_kprintf(" %3dM", (record->max_data_rate / 1000));
    aw_kprintf(" %3d", record->channel);
    aw_kprintf(" %s", security_to_str(record->security));
    if (record->ccode[0] != '\0') {
        aw_kprintf(" %c%c   ", record->ccode[0], record->ccode[1]);
    } else {
        aw_kprintf("      ");
    }

    if (record->flags & AW_WIFI_SCAN_RESULT_FLAG_BEACON) {
        aw_kprintf(" BEACON ");
    } else {
        aw_kprintf(" PROBE  ");
    }
    for (i = 0; i < record->ssid.length; i++) {
        aw_kprintf("%c", record->ssid.value[i]);
    }
    aw_kprintl("");
}

/*
 * Callback function to handle scan results
 */
aw_local aw_err_t __scan_result_handler (aw_wifi_scan_cb_result_t *result,
                                         aw_wifi_scan_status_t     status)
{
    if (result != NULL) {
        app_scan_data_t *scan_data = (app_scan_data_t *)result->p_arg;

        if (status == AW_WIFI_SCAN_INCOMPLETE) {
            struct aw_shell_user  *sh = scan_data->sh;
            aw_wifi_scan_result_t *detail = &result->ap_details;

            shell_printf("%3d ", scan_data->count);
            print_scan_result(detail);
            scan_data->count++;
        } else {
            AW_SEMB_GIVE(scan_data->signal);
        }
    }
    return AW_OK;
}

/*!
 ******************************************************************************
 * Scans for access points and prints out results
 *
 * @return  0 for success, otherwise error
 */
aw_local int __shell_scan (int argc, char* argv[], struct aw_shell_user *sh)
{
    aw_wifi_id_t id;

    WIFI_DEVICE_OPEN(id);
    do {
        int scan_start_time;
        int scan_end_time;
        app_scan_data_t scan_data;

        /* Initialize the semaphore that will tell us when the scan is complete */
        AW_SEMB_INIT(scan_data.signal, AW_SEM_EMPTY, AW_SEM_Q_FIFO);
        scan_data.count = 0;
        scan_data.sh = sh;

        shell_printl("Waiting for %s scan results...", argc ? "Passive" : "Active");
        shell_printl("  # Type  BSSID             RSSI Rate  CH Security       CCode Flag   SSID");
        shell_printl("-----------------------------------------------------------------------------");

        /* Start the scan */
        scan_start_time = aw_ticks_to_ms(aw_sys_tick_get());
        if (AW_OK != aw_wifi_scan(id,
                                  __scan_result_handler,
                                  (void *) &scan_data,
                                  NULL,
                                  NULL,
                                  argc ? AW_WIFI_SCAN_TYPE_PASSIVE : AW_WIFI_SCAN_TYPE_ACTIVE)) {
            shell_printl("Error starting scan!");
            break;
        }

        /* Wait until scan is complete */
        AW_SEMB_TAKE(scan_data.signal, AW_SEM_WAIT_FOREVER);
        scan_end_time = aw_ticks_to_ms(aw_sys_tick_get());

        shell_printl(ENDL "Scan complete in %u milliseconds", (scan_end_time - scan_start_time));

        /* Clean up */
        AW_SEMB_TERMINATE(scan_data.signal);
    } while (0);
    WIFI_DEVICE_CLOSE(id);

    return 0;
}

aw_local int __shell_join (int argc, char* argv[], struct aw_shell_user *sh)
{
    /* <ssid> <key> [ip] [netmask gateway] [dns] */
    aw_wifi_ap_info_t info;
    aw_wifi_ssid_t    ssid;
    aw_wifi_key_t     pkey;
    struct in_addr    ip;
    aw_wifi_mac_t     mac;
    aw_err_t          err;
    aw_wifi_id_t      id;
    aw_netif_t       *p_netif;

    if (!(argc == 1 || argc == 2 || argc == 3 || argc == 5 || argc == 6)) {
        return 0;
    }

    WIFI_DEVICE_OPEN(id);

    do {
        aw_wifi_leave(id);

        ssid.length = snprintf((char *) ssid.value, sizeof(ssid.value), "%s", argv[0]);

        {
            int i = WIFI_OPERATION_RETRY_MAX;
            do {
                err = aw_wifi_get_ap_info(id, &ssid, NULL, &info);
            } while (--i && AW_OK != err);
            if (AW_OK == err) {
                shell_printl("Target security is: %s", security_to_str(info.security));
            } else {
                shell_printl("Get AP info failed!");
                break;
            }
        }

        if(info.security != AW_WIFI_SECURITY_OPEN){
            pkey.length = snprintf((char *)pkey.value, sizeof(pkey.value),"%s", argv[1]);
        }

        err = aw_wifi_join(id,
                           NULL,
                           &ssid,
                           info.security,
                           &pkey);
        if (AW_OK != err) {
            shell_printl("Join failed.");
            break;
        }
        g_sta_is_assocated = AW_TRUE;

        p_netif = aw_netif_dev_open(APP_WIFI_STA_IF_NAME);
        if (p_netif == NULL) {
            shell_printl("Find netif failed.");
            err = -AW_ENODEV;
            break;
        }

        if (argc > 2) {
            aw_netif_dhcpc_stop(p_netif);

            inet_aton(argv[2], &ip);
            aw_netif_ipv4_ip_set(p_netif, &ip);

            if (argc > 3) {
                inet_aton(argv[3], &ip);
                aw_netif_ipv4_netmask_set(p_netif, &ip);

                inet_aton(argv[4], &ip);
                aw_netif_ipv4_gw_set(p_netif, &ip);
            } else {
                inet_aton("255.255.255.0", &ip);
                aw_netif_ipv4_netmask_set(p_netif, &ip);

                inet_aton("0.0.0.0", &ip);
                aw_netif_ipv4_gw_set(p_netif, &ip);
            }

            if (argc > 5 && inet_aton(argv[5], &ip)) {
                aw_dns_set(0, &ip);
            }
        } else {
            aw_bool_t bound = AW_FALSE;
            int dhcp_wait_time = 20 * 1000;

            shell_printl("DHCP working...");
            aw_netif_dhcpc_start(p_netif);

            while (AW_FALSE == bound && dhcp_wait_time) {
                aw_mdelay(100);
                dhcp_wait_time -= 100;
                aw_netif_dhcpc_is_bound(p_netif, &bound);
            }
            if (AW_FALSE == bound) {
                shell_printl("DHCP Timed out!");
                if (AW_OK == aw_wifi_leave(id)) {
                    g_sta_is_assocated = AW_FALSE;
                } else {
                    aw_wifi_leave(id);
                }
                err = AW_ERROR;
                break;
            }
        }
        aw_netif_ipv4_ip_get(p_netif, &ip);
        shell_printf(APP_PRINT_IPADDR_FMT " / ", inet_ntoa(ip));

        aw_netif_ipv4_netmask_get(p_netif, &ip);
        shell_printf(APP_PRINT_IPADDR_FMT " -> ", inet_ntoa(ip));

        aw_netif_ipv4_gw_get(p_netif, &ip);
        shell_printf(APP_PRINT_IPADDR_FMT " @ ", inet_ntoa(ip));

        aw_netif_hwaddr_get(p_netif, mac.octet, 6);
        shell_printf(APP_PRINT_HWADDR_FMT " ", APP_PRINT_HWADDR_VAL(mac.octet));

        aw_netif_dev_close(p_netif);
    } while (0);

    WIFI_DEVICE_CLOSE(id);

    return err;
}

/*!
 ******************************************************************************
 * Leaves an associated access point
 *
 * @return  0 for success, otherwise error
 */

aw_local int __shell_leave (int argc, char* argv[], struct aw_shell_user *sh)
{
    aw_err_t err;
    aw_wifi_id_t id;

    WIFI_DEVICE_OPEN(id);
    err = aw_wifi_leave(id);
    WIFI_DEVICE_CLOSE(id);

    if (AW_OK == err) {
        g_sta_is_assocated = AW_FALSE;
    }
    return err;
}

/*!
 ******************************************************************************
 * Starts a soft AP as specified by the provided arguments
 *
 * @return  0 for success, otherwise error
 */

aw_local int __shell_start_ap (int argc, char* argv[], struct aw_shell_user *sh)
{
    /* <ssid> <open|wpa2|wpa2_aes> <key> <channel> [ip netmask gateway] */
    aw_err_t            err;
    aw_wifi_ssid_t      ssid;
    aw_wifi_security_t  security;
    aw_wifi_key_t       pkey;
    uint8_t             channel;
    aw_wifi_id_t        id;

    if (!(argc == 3 || argc == 4 || argc == 6 || argc == 7)) {
        return -AW_EINVAL;
    }

    WIFI_DEVICE_OPEN(id);

    do {
        ssid.length = snprintf((char *)ssid.value, sizeof(ssid.value), "%s", argv[0]);
        security = str_to_security(argv[1]);

        if(security == AW_WIFI_SECURITY_OPEN){
            if(!(argc == 3 || argc == 6)){
                return -AW_EINVAL;
            }
            channel  = strtol(argv[2], NULL, 10);
        }else{
            if(!(argc == 4 || argc == 7)){
                return -AW_EINVAL;
            }
            pkey.length = snprintf((char *)pkey.value, sizeof(pkey.value), "%s", argv[2]);
            channel  = strtol(argv[3], NULL, 10);
        }

        err = aw_wifi_ioctl(id,
                            AW_WO_OPT_SET,
                            AW_WO_AP_EVENT_HANDLER,
                            (void *)__wifi_softap_event_handler,
                            NULL);
        if (AW_OK != err) {
            shell_printl("Event Registry Failed:%d", err);
        }

        err = aw_wifi_start_ap(id,
                               &ssid,
                               security,
                               &pkey,
                               channel);
        if (AW_OK != err) {
            shell_printl("Failed.");
            break;
        }

        if (argc == 7) {
            struct in_addr ip;
            struct in_addr nm;
            struct in_addr gw;

            inet_aton(argv[4], &ip);
            inet_aton(argv[5], &nm);
            inet_aton(argv[6], &gw);

            aw_netif_ipv4_addr_set_by_name(APP_WIFI_AP_IF_NAME, &ip, &nm, &gw);
        }
    } while (0);

    if (AW_OK == err) {
        aw_wifi_mac_t  mac;
        struct in_addr ip;
        struct in_addr nm;
        struct in_addr gw;

        shell_printl("Started. SSID:%s, ch:%d", ssid.value, channel);

        dhcps_bind_netif(ap_dhcp_server(), APP_WIFI_AP_IF_NAME);
        dhcps_start(ap_dhcp_server());

        aw_netif_hwaddr_get_by_name(APP_WIFI_AP_IF_NAME, mac.octet, 6);
        aw_netif_ipv4_addr_get_by_name(APP_WIFI_AP_IF_NAME, &ip, &nm, &gw);
        shell_printf(APP_PRINT_IPADDR_FMT " / ", inet_ntoa(ip));
        shell_printf(APP_PRINT_IPADDR_FMT " -> ", inet_ntoa(nm));
        shell_printf(APP_PRINT_IPADDR_FMT " @ ", inet_ntoa(gw));
        shell_printl(APP_PRINT_HWADDR_FMT, APP_PRINT_HWADDR_VAL(mac.octet));
    } else {
        shell_printl("Failed.");
    }
    WIFI_DEVICE_CLOSE(id);

    return err;
}

/*!
 ******************************************************************************
 * Stops a running soft AP
 *
 * @return  0 for success, otherwise error
 */

aw_local int __shell_stop_ap (int argc, char* argv[], struct aw_shell_user *sh)
{
    aw_err_t err;
    aw_wifi_id_t id;

    WIFI_DEVICE_OPEN(id);
    do {
        err = aw_wifi_stop_ap(id);
        if (AW_OK != err) {
            shell_printl("Stop AP failed.");
            break;
        }
        dhcps_stop(ap_dhcp_server());
        shell_printf("Stop AP OK.");

        err = aw_wifi_ioctl(id, AW_WO_OPT_SET, AW_WO_AP_EVENT_HANDLER, NULL, NULL);
        if (AW_OK != err) {
            shell_printl("Event UnRegistry Failed:%d", err);
        }
    } while (0);
    WIFI_DEVICE_CLOSE(id);

    return 0;
}

/*!
 ******************************************************************************
 * Get STA list
 *
 * @return  0 for success, otherwise error
 */

aw_local aw_err_t __get_sta_result_handler (void               *p_arg,
                                            aw_wifi_sta_info_t *sta,
                                            aw_bool_t              complete)
{
    app_scan_data_t *sta_data = (app_scan_data_t *) p_arg;

    if (complete == AW_TRUE) {
        AW_SEMB_GIVE(sta_data->signal);
    } else {
        struct aw_shell_user *sh = sta_data->sh;
        int ip_str_length = 0;
        struct in_addr ip;
        char str[16];

        sta_data->count++;
        shell_printf("%2d ", sta_data->count);
        shell_printf(APP_PRINT_HWADDR_FMT "  ", APP_PRINT_HWADDR_VAL(sta->bssid.octet));
        shell_printf("%3ddBm  ", sta->signal_strength);
        dhcps_get_ip_by_macaddr(ap_dhcp_server(), (aw_eth_mac_t *) &sta->bssid, &ip);
        ipaddr_ntoa_r((ip_addr_t *) &ip, str, 16);
        ip_str_length = shell_printf(APP_PRINT_IPADDR_FMT, str);
        for (; ip_str_length < 17; ip_str_length++) {
            shell_printf(" ");
        }
        shell_printl("%s",
                dhcps_get_host_name_by_macaddr(ap_dhcp_server(), (aw_eth_mac_t *) &sta->bssid));
    }
    return AW_OK;
}

aw_local int __shell_get_sta_list (int argc, char* argv[], struct aw_shell_user *sh)
{
    app_scan_data_t sta_data;
    aw_wifi_id_t id;

    /* Initialize the semaphore that will tell us when the scan is complete */
    AW_SEMB_INIT(sta_data.signal, AW_SEM_EMPTY, AW_SEM_Q_FIFO);
    sta_data.count = 0;
    sta_data.sh = sh;

    WIFI_DEVICE_OPEN(id);
    do {
        shell_printl(" # BSSID                RSSI  IP address       Name");
        shell_printl("-----------------------------------------------------------------------------");

        if (AW_OK != aw_wifi_get_sta_list(id, __get_sta_result_handler, (void *) &sta_data)) {
            shell_printl("Get STA list failed.");
            break;
        }

        /* Wait until scan is complete */
        AW_SEMB_TAKE(sta_data.signal, AW_SEM_WAIT_FOREVER);
    } while (0);
    WIFI_DEVICE_CLOSE(id);
    /* Done! */
    shell_printl(ENDL "End");

    return 0;
}

aw_local int __shell_get_ap_info (int argc, char* argv[], struct aw_shell_user *sh)
{
    aw_err_t err;
    aw_wifi_ssid_t ssid;
    aw_wifi_ap_info_t info;
    aw_wifi_id_t id;

    WIFI_DEVICE_OPEN(id);
    do {
        if (0 == argc) {
            err = aw_wifi_get_join_ap_info(id, &info);
        } else {
            memcpy(ssid.value, argv[0], strlen(argv[0]));
            ssid.length = strlen(argv[0]);
            err = aw_wifi_get_ap_info(id,
                                      &ssid,
                                      NULL,
                                      &info);
        }
        if (AW_OK != err) {
            shell_printl("Get AP info failed!");
            break;
        }

        shell_printl("SSID     : %s", info.ssid.value);
        shell_printl("BSSID    : " APP_PRINT_HWADDR_FMT, APP_PRINT_HWADDR_VAL(info.bssid.octet));
        shell_printl("RSSI     : %d dBm", info.signal_strength);
        shell_printl("Channel  : %d", info.channel);
        shell_printl("Max Rate : %dM", info.max_data_rate / 1000);
        shell_printl("Security : %s",
                        (info.security == AW_WIFI_SECURITY_OPEN)           ? "Open" :
                        (info.security == AW_WIFI_SECURITY_WEP_PSK)        ? "WEP" :
                        (info.security == AW_WIFI_SECURITY_WPA_TKIP_PSK)   ? "WPA TKIP" :
                        (info.security == AW_WIFI_SECURITY_WPA_AES_PSK)    ? "WPA AES" :
                        (info.security == AW_WIFI_SECURITY_WPA2_AES_PSK)   ? "WPA2 AES" :
                        (info.security == AW_WIFI_SECURITY_WPA2_TKIP_PSK)  ? "WPA2 TKIP" :
                        (info.security == AW_WIFI_SECURITY_WPA2_MIXED_PSK) ? "WPA2 Mixed" :
                                                                             "Unknown" );
} while (0);
    WIFI_DEVICE_CLOSE(id);

    return 0;
}

/*!
 ******************************************************************************
 * Returns the link status of the Wi-Fi interface
 *
 * @return  0 for success, otherwise error
 */

static int __shell_link_status (int argc, char* argv[], struct aw_shell_user *sh)
{
    aw_err_t err_sta;
    aw_err_t err_ap;
    uint32_t link_status_sta;
    uint32_t link_status_ap;
    char *status_sta;
    char *status_ap;
    aw_wifi_id_t id;

    WIFI_DEVICE_OPEN(id);
    err_sta = aw_wifi_link_status_get(id, AW_WIFI_STA_INTERFACE, &link_status_sta);
    err_ap = aw_wifi_link_status_get(id, AW_WIFI_AP_INTERFACE, &link_status_ap);
    WIFI_DEVICE_CLOSE(id);

    if (AW_OK != err_sta) {
        status_sta = "fail";
    } else {
        status_sta = link_status_sta ? "up" : "down";
    }
    if (AW_OK != err_ap) {
        status_ap = "fail";
    } else {
        status_ap = link_status_ap ? "up" : "down";
    }
    shell_printl("Link status: STA(%s) AP(%s)", status_sta, status_ap);

    return 0;
}

aw_local int __shell_set_tx_pwr (int argc, char* argv[], struct aw_shell_user *sh)
{
    uint32_t tx_pwr = 0;
    aw_err_t err;
    aw_wifi_id_t id;

    if (argc != 1) {
        shell_printl("Set tx power failed, argc != 1");
        return 0;
    }
    tx_pwr = (uint32_t) atoi(argv[0]);

    WIFI_DEVICE_OPEN(id);
    err = aw_wifi_ioctl(id, AW_WO_OPT_SET, AW_WO_TXPOWER, (void *) tx_pwr, NULL);
    WIFI_DEVICE_CLOSE(id);
    if (err != AW_OK) {
        shell_printl("Set tx power failed! %d", err);
    }
    return 0;
}

aw_local int __shell_get_tx_pwr (int argc, char* argv[], struct aw_shell_user *sh)
{
    int tx_pwr = 0;
    aw_err_t err;
    aw_wifi_id_t id;

    WIFI_DEVICE_OPEN(id);
    err = aw_wifi_ioctl(id, AW_WO_OPT_GET, AW_WO_TXPOWER, (void *) &tx_pwr, NULL);
    WIFI_DEVICE_CLOSE(id);
    if (err != AW_OK) {
        shell_printl("Get tx power failed! %d", err);
    } else {
        shell_printl("WLAN TX power: %d dbm", tx_pwr);
    }
    return 0;
}

aw_local int __shell_get_country (int argc, char* argv[], struct aw_shell_user *sh)
{
    aw_err_t err;
    aw_wifi_country_code_t country_code;
    aw_wifi_id_t id;

    WIFI_DEVICE_OPEN(id);
    err = aw_wifi_ioctl(id, AW_WO_OPT_GET, AW_WO_COUNTRY_CODE, (void *) &country_code, NULL);
    WIFI_DEVICE_CLOSE(id);
    if (err != AW_OK) {
        shell_printl("Get country code failed! %d", err);
    } else {
        shell_printf("Country code: %c%c/%d",
                     (country_code >>  0 & 0xFF),
                     (country_code >>  8 & 0xFF),
                     (country_code >> 16 & 0xFF));
    }
    return 0;
}

aw_local int __shell_set_country (int argc, char* argv[], struct aw_shell_user *sh)
{
    aw_err_t err;
    char *country;
    aw_wifi_country_code_t code;
    aw_wifi_id_t id;

    if (argc != 1) {
        shell_printl("Set country code failed, argc != 1");
        return 0;
    }

    country = argv[0];
    if (strcmp(country, "cn") == 0) {
        code = AW_WIFI_COUNTRY_CHINA;
    } else if (strcmp(country, "us") == 0) {
        code = AW_WIFI_COUNTRY_UNITED_STATES;
    } else {
        shell_printl("Set country code failed, not support %s", country);
        return 0;
    }

    WIFI_DEVICE_OPEN(id);
    err = aw_wifi_ioctl(id, AW_WO_OPT_SET, AW_WO_COUNTRY_CODE, (void *) code, NULL);
    WIFI_DEVICE_CLOSE(id);
    if (err != AW_OK) {
        shell_printl("Set country code failed! %d", err);
    } else {
        shell_printl("Set country code: %s OK", country);
    }
    return 0;
}

aw_local int __shell_powersave (int argc, char* argv[], struct aw_shell_user *sh)
{
    aw_err_t err;
    int en = 0;
    int listen_interval = 0;
    aw_wifi_id_t id;

    if (argc != 1) {
        shell_printl("Powersave failed, argc != 1");
        return 0;
    }

    en = atoi(argv[0]);
    if (en) {
        en = 1;
        listen_interval = 10;
        shell_printl("enable powersave");
    } else {
        en = 0;
        listen_interval = 0;
        shell_printl("disable powersave");
    }

    WIFI_DEVICE_OPEN(id);
    err = aw_wifi_ioctl(id,
                        AW_WO_OPT_SET,
                        AW_WO_LISTEN_INTVAL,
                        (void *) listen_interval,
                        NULL);
    err = aw_wifi_ioctl(id,
                        AW_WO_OPT_SET,
                        AW_WO_POWERSAVE,
                        (void *) en,
                        NULL);
    WIFI_DEVICE_CLOSE(id);

    if (err != AW_OK) {
        shell_printl("Set powersave failed! %d", err);
    }
    return 0;
}

aw_local int __shell_11n (int argc, char* argv[], struct aw_shell_user *sh)
{
    int en;
    aw_err_t err;
    aw_wifi_id_t id;

    if (argc != 1) {
        shell_printl("Set 802.11n mode failed, argc != 1");
        return 0;
    }
    en = atoi(argv[0]);

    WIFI_DEVICE_OPEN(id);
    err = aw_wifi_ioctl(id,
                        AW_WO_OPT_SET,
                        AW_WO_80211N,
                        (void *) en,
                        NULL);
    WIFI_DEVICE_CLOSE(id);
    if (err != AW_OK) {
        shell_printl("Set 802.11n mode failed! %d", err);
    } else {
        shell_printl("802.11n %s.", en ? "enabled" : "disable");
    }

    return 0;
}

aw_local int __shell_get_max_assoc (int argc, char* argv[], struct aw_shell_user *sh)
{
    aw_err_t err;
    uint32_t max_assoc = 0;
    aw_wifi_id_t id;

    WIFI_DEVICE_OPEN(id);
    err = aw_wifi_ioctl(id, AW_WO_OPT_GET, AW_WO_MAX_ASSOC, (void *) &max_assoc, NULL);
    WIFI_DEVICE_CLOSE(id);
    if (err != AW_OK) {
        shell_printl("Get max associations failed! %d", err);
    } else {
        shell_printl("Max associations: %d", max_assoc);
    }
    return 0;
}

aw_local int __shell_get_rate (int argc, char* argv[], struct aw_shell_user *sh)
{
    aw_err_t err;
    uint32_t rate = 0;
    aw_wifi_interface_t wifiif;
    aw_wifi_id_t id;

    wifiif = (aw_wifi_interface_t) atoi(argv[0]);

    WIFI_DEVICE_OPEN(id);
    err = aw_wifi_ioctl(id, AW_WO_OPT_GET, AW_WO_RATE, (void *) wifiif, (void *) &rate);
    WIFI_DEVICE_CLOSE(id);
    if (err != AW_OK) {
        shell_printl("Get rate failed! %d", err);
    } else {
        shell_printl("Rate: %d Mbits/s", rate >> 1);
    }
    return 0;
}

aw_local int __shell_get_channel (int argc, char* argv[], struct aw_shell_user *sh)
{
    aw_err_t err;
    uint32_t ch = 0;
    aw_wifi_interface_t wifiif;
    aw_wifi_id_t id;

    wifiif = (aw_wifi_interface_t) atoi(argv[0]);
    WIFI_DEVICE_OPEN(id);
    err = aw_wifi_ioctl(id, AW_WO_OPT_GET, AW_WO_CH, (void *) wifiif, (void *) &ch);
    WIFI_DEVICE_CLOSE(id);
    if (err != AW_OK) {
        shell_printl("Get channel failed! %d", err);
    } else {
        shell_printl("channel: %d", ch);
    }
    return 0;
}

aw_local int __shell_set_channel (int argc, char* argv[], struct aw_shell_user *sh)
{
    aw_err_t err;
    uint32_t ch = 0;
    aw_wifi_interface_t wifiif;
    aw_wifi_id_t id;

    ch = atoi(argv[0]);
    wifiif = (aw_wifi_interface_t) atoi(argv[1]);
    WIFI_DEVICE_OPEN(id);
    err = aw_wifi_ioctl(id, AW_WO_OPT_SET, AW_WO_CH, (void *) wifiif, (void *) ch);
    WIFI_DEVICE_CLOSE(id);
    if (err != AW_OK) {
        shell_printl("Set channel failed! %d", err);
    } else {
        shell_printl("done");
    }
    return 0;
}

aw_local int __shell_fbt_over_ds (int argc, char* argv[], struct aw_shell_user *sh)
{
    aw_err_t err;
    int value = 0;
    aw_wifi_id_t id;

    WIFI_DEVICE_OPEN(id);
    err = aw_wifi_ioctl(id,
                        's' == argv[0][0] ? AW_WO_OPT_SET : AW_WO_OPT_GET,
                        AW_WO_FBT_OVER_DS,
                        (void *) &value,
                        AW_WIFI_STA_INTERFACE);
    WIFI_DEVICE_CLOSE(id);
    if (err != AW_OK) {
        shell_printl("Failed! %d", err);
    } else {
        if ('s' == argv[0][0]) {
            shell_printl("Set Fast BSS Transition over DS success");
        } else {
            shell_printl("Get Fast BSS Transition over DS success WLFTBT:%d", value);
        }
    }

    return 0;
}

aw_local int __shell_get_fbtcap (int argc, char* argv[], struct aw_shell_user *sh)
{
    aw_err_t err;
    int value = 0;
    aw_wifi_id_t id;

    WIFI_DEVICE_OPEN(id);
    err = aw_wifi_ioctl(id,
                        AW_WO_OPT_GET,
                        AW_WO_FBT_CAP,
                        (void *) value,
                        AW_WIFI_STA_INTERFACE);
    WIFI_DEVICE_CLOSE(id);
    if (err != AW_OK) {
        shell_printl("Failed! %d", err);
    } else {
        shell_printl("WLAN Fast BSS Transition Capabilities:%d", value);
    }

    return 0;
}

aw_local void __raw_pkt_processor (char *p_buf, size_t size)
{
    aw_kprintl("\traw <= (p,len) "
               "0x%x,%u",
               p_buf,
               size);
}

aw_local int __shell_monitor_mode (int argc, char* argv[], struct aw_shell_user *sh)
{
    int en;
    aw_err_t err;
    aw_wifi_id_t id;

    if (argc != 1) {
        shell_printl("Set monitor mode failed, argc != 1");
        return 0;
    }
    en = atoi(argv[0]);

    WIFI_DEVICE_OPEN(id);
    if (en) {
        err = aw_wifi_ioctl(id,
                            AW_WO_OPT_SET,
                            AW_WO_RAW_PACK_PROCESSOR,
                            (void *) __raw_pkt_processor,
                            NULL);
        err = aw_wifi_ioctl(id,
                            AW_WO_OPT_SET,
                            AW_WO_MONITOR_MODE,
                            (void *) AW_TRUE,
                            NULL);
    } else {
        err = aw_wifi_ioctl(id,
                            AW_WO_OPT_SET,
                            AW_WO_MONITOR_MODE,
                            (void *) AW_FALSE,
                            NULL);
    }
    WIFI_DEVICE_CLOSE(id);
    if (err != AW_OK) {
        shell_printl("Set monitor mode failed! %d", err);
    } else {
        shell_printl("monitor mode %s.", en ? "on" : "off");
    }

    return 0;
}

aw_local const char *nic_type_to_str (aw_netif_type_t type)
{
    aw_local aw_const char *names[] = {
        "RAW ",
        "ETH ",
        "WIFI",
    };

    return names[type];
}

aw_local void __iterate_cb (aw_netif_t *p_netif, void *p_arg)
{
    aw_bool_t up;
    uint8_t type;
    struct in_addr ip;
    aw_wifi_mac_t mac;
    struct aw_shell_user *sh = (struct aw_shell_user *) p_arg;
    char *name = NULL;

    aw_netif_name_get(p_netif, &name);
    aw_netif_type_get(p_netif, &type);
    aw_netif_is_link_up(p_netif, &up);
    shell_printf("%-4s, %s, %s\t",
            name,
            nic_type_to_str(type),
            up ? "up  " : "down");

    aw_netif_ipv4_ip_get(p_netif, &ip);
    shell_printf(APP_PRINT_IPADDR_FMT " / ", inet_ntoa(ip));
    aw_netif_ipv4_netmask_get(p_netif, &ip);
    shell_printf(APP_PRINT_IPADDR_FMT " -> ", inet_ntoa(ip));
    aw_netif_ipv4_gw_get(p_netif, &ip);
    shell_printf(APP_PRINT_IPADDR_FMT " @ ", inet_ntoa(ip));
    aw_netif_hwaddr_get(p_netif, mac.octet, 6);
    shell_printl(APP_PRINT_HWADDR_FMT, APP_PRINT_HWADDR_VAL(mac.octet));
}

aw_local int __shell_nics (int argc, char* argv[], struct aw_shell_user *sh)
{
    aw_wifi_id_t id;
    aw_wifi_status_t status;

    shell_printl("--- nic ---");
    aw_netif_iterate(__iterate_cb, sh);
    shell_printl("--- end ---");

    WIFI_DEVICE_OPEN(id);
    aw_wifi_status_get(id, &status);
    WIFI_DEVICE_CLOSE(id);
    shell_printl("wifi %x: opmode=%x, state=%x", id, status.last_opmode, status.state);

    return 0;
}

aw_local int __shell_driver_log (int argc, char* argv[], struct aw_shell_user *sh)
{
    aw_err_t err;
    aw_wifi_id_t id;

    WIFI_DEVICE_OPEN(id);

    if (0 < argc && !strcmp("start", argv[0])) {
        shell_printl("start driver log");
        err = aw_wifi_ioctl(id,
                            AW_WO_OPT_SET,
                            AW_WO_DRIVER_LOG,
                            (void *) AW_TRUE,
                            NULL);
    } else {
        shell_printl("stop driver log");
        err = aw_wifi_ioctl(id,
                            AW_WO_OPT_SET,
                            AW_WO_DRIVER_LOG,
                            (void *) AW_FALSE,
                            NULL);
    }

    WIFI_DEVICE_CLOSE(id);
    return err;
}

/******************************************************************************/

aw_local void __wifi_softap_event_handler (void                *p_arg,
                                           aw_wifi_ap_event_t   event,
                                           const aw_wifi_mac_t *mac_addr)
{
    if (event == AW_WIFI_AP_STA_JOINED_EVENT) {
        aw_kprintl("STA joined: " APP_PRINT_HWADDR_FMT, APP_PRINT_HWADDR_VAL(mac_addr->octet));
    } else if (event == AW_WIFI_AP_STA_LEAVE_EVENT) {
        aw_kprintl("STA left: " APP_PRINT_HWADDR_FMT, APP_PRINT_HWADDR_VAL(mac_addr->octet));
    }
}

/**
 * \brief wifi 初始化
 */
void test_wifi_init (void)
{
    aw_wifi_id_t id;
    uint32_t stat;
    int i = WIFI_OPERATION_RETRY_MAX;
    aw_netif_t *p_netif;

    id = aw_wifi_dev_open(WIFI_DEV_NAME);
    if (AW_WIFI_ID_INVALID == id) {
        aw_kprintl("wifi not ready.");
    } else {
        aw_wifi_ioctl(id, AW_WO_OPT_GET, AW_WO_DEV_STAT, (void *) &stat, NULL);
        aw_wifi_dev_close(id);
        aw_kprintl("wifi stat: %x", stat);
    }

    do {
        aw_mdelay(10);
        p_netif = aw_netif_dev_open(APP_WIFI_STA_IF_NAME);
    } while ((--i && NULL == p_netif));

    if (p_netif) {
        aw_wifi_mac_t mac;
        char old_hostname[AW_NET_HOSTNAME_LEN_MAX];
        char new_hostname[AW_NET_HOSTNAME_LEN_MAX];

        aw_netif_hwaddr_get(p_netif, mac.octet, 6);
        aw_netif_dev_close(p_netif);
        snprintf(old_hostname, sizeof(old_hostname), "%s", aw_net_hostname_get());
        snprintf(new_hostname, sizeof(new_hostname), "%s-%02X%02X%02X",
                 old_hostname, mac.octet[3], mac.octet[4], mac.octet[5]);
        aw_net_hostname_set(new_hostname);
    }
}

int app_wifi (int argc, char **argv, struct aw_shell_user *p_user)
{
    int ret  = AW_OK;
    if (argc > 0) {
        argc--;
        argv = &argv[1];
    }

    if ((aw_shell_args_get(p_user, "scan", 0) == AW_OK) ||
        (aw_shell_args_get(p_user, "scan", 1) == AW_OK)) {
        ret = __shell_scan(argc, argv, p_user);
    } else if ((aw_shell_args_get(p_user, "join", 1) == AW_OK) ||
               (aw_shell_args_get(p_user, "join", 2) == AW_OK) ||
               (aw_shell_args_get(p_user, "join", 3) == AW_OK) ||
               (aw_shell_args_get(p_user, "join", 5) == AW_OK) ||
               (aw_shell_args_get(p_user, "join", 6) == AW_OK)) {
        ret = __shell_join(argc, argv, p_user);
    } else if (aw_shell_args_get(p_user, "getapinfo", 0) == AW_OK) {
        ret = __shell_get_ap_info(argc, argv, p_user);
    } else if (aw_shell_args_get(p_user, "leave", 0) == AW_OK) {
        ret = __shell_leave(argc, argv, p_user);
    } else if ((aw_shell_args_get(p_user, "startap", 3) == AW_OK) ||
               (aw_shell_args_get(p_user, "startap", 4) == AW_OK) ||
               (aw_shell_args_get(p_user, "startap", 6) == AW_OK) ||
               (aw_shell_args_get(p_user, "startap", 7) == AW_OK)) {
        ret = __shell_start_ap(argc, argv, p_user);
    }  else if (aw_shell_args_get(p_user, "stopap", 0) == AW_OK) {
        ret = __shell_stop_ap(argc, argv, p_user);
    } else if (aw_shell_args_get(p_user, "maxassoc", 0) == AW_OK) {
        ret = __shell_get_max_assoc(argc, argv, p_user);
    } else if (aw_shell_args_get(p_user, "getstalist", 0) == AW_OK) {
        ret = __shell_get_sta_list(argc, argv, p_user);
    } else if (aw_shell_args_get(p_user, "getlinkstatus", 0) == AW_OK) {
        ret = __shell_link_status(argc, argv, p_user);
    } else if (aw_shell_args_get(p_user, "settxpwr", 1) == AW_OK) {
        ret = __shell_set_tx_pwr(argc, argv, p_user);
    } else if (aw_shell_args_get(p_user, "gettxpwr", 0) == AW_OK) {
        ret = __shell_get_tx_pwr(argc, argv, p_user);
    } else if (aw_shell_args_get(p_user, "setcc", 1) == AW_OK) {
        ret = __shell_set_country(argc, argv, p_user);
    } else if (aw_shell_args_get(p_user, "getcc", 0) == AW_OK) {
        ret = __shell_get_country(argc, argv, p_user);
    } else if (aw_shell_args_get(p_user, "set11n", 1) == AW_OK) {
        ret = __shell_11n(argc, argv, p_user);
    } else if (aw_shell_args_get(p_user, "powersave", 1) == AW_OK) {
        ret = __shell_powersave(argc, argv, p_user);
    } else if ((aw_shell_args_get(p_user, "rate", 0) == AW_OK) ||
               (aw_shell_args_get(p_user, "rate", 1) == AW_OK)) {
        ret = __shell_get_rate(argc, argv, p_user);
    } else if (aw_shell_args_get(p_user, "getch", 1) == AW_OK) {
        ret = __shell_get_channel(argc, argv, p_user);
    } else if ((aw_shell_args_get(p_user, "setch", 1) == AW_OK) ||
               (aw_shell_args_get(p_user, "setch", 2) == AW_OK)) {
        ret = __shell_set_channel(argc, argv, p_user);
    } else if ((aw_shell_args_get(p_user, "setmm", 0) == AW_OK) ||
               (aw_shell_args_get(p_user, "setmm", 1) == AW_OK)){
        ret = __shell_monitor_mode(argc, argv, p_user);
    } else if (aw_shell_args_get(p_user, "fbt", 1) == AW_OK) {
        ret = __shell_fbt_over_ds(argc, argv, p_user);
    } else if (aw_shell_args_get(p_user, "fbtcap", 0) == AW_OK) {
        ret = __shell_get_fbtcap(argc, argv, p_user);
    } else if (aw_shell_args_get(p_user, "nics", 0) == AW_OK) {
        ret = __shell_nics(argc, argv, p_user);
    } else if (aw_shell_args_get(p_user, "drvlog", 1) == AW_OK) {
        ret = __shell_driver_log(argc, argv, p_user);
    } else {
        return -EINVAL;
    }

    return ret;
}

/** [src_app_wifi] */

#endif /* APP_WIFI */

