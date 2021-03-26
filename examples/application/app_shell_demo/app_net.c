/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief 通过shell命令配置网卡
 *
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能
 *      - 网卡
 *   2. 在shell界面进入".test"子目录后执行"ipconfig -select 0"选择网卡0
 *   3. 执行"ipconfig -config 192.168.12.236 255.255.255.0 192.168.12.254"修改IP地
 *      址为192.168.12.236，子网掩码为255.255.255.0，网关为192.168.12.254
 *   4. 执行"ipconfig -dhcpc on"打开DHCP
 *
 * \par 源代码
 * \snippet app_net.c app_net
 *
 * \internal
 * \par Modification History
 * - 1.00 18-07-11  sdq, first implementation.
 * \endinternal
 */

/** [src_app_net] */
#include "aworks.h"                     /* 此头文件必须被首先包含 */
#include "aw_shell.h"
#include "aw_serial.h"
#include "aw_delay.h"
#include "aw_netif.h"
#include "lwip/inet.h"
#include "string.h"
#include "app_config.h"

#ifdef APP_NET

#ifndef ENDL
    #define ENDL            "\r\n"
#endif

#define APP_PRINT_IPADDR_FMT \
    "%u.%u.%u.%u"

#define APP_PRINT_HWADDR_FMT \
    "%02X:%02X:%02X:%02X:%02X:%02X"

#define APP_PRINT_HWADDR_VAL(addr) \
    (addr)[0], (addr)[1], (addr)[2], (addr)[3], (addr)[4], (addr)[5]

#define APP_PRINT_IPADDR_VAL(addr) \
    addr & 0x000000ff, \
   (addr & 0x0000ff00) >> 8, \
   (addr & 0x00ff0000) >> 16, \
    addr >> 24


aw_local char *__dev_idx[] = {
        "eth0",
        "eth1",
        "st",
        "ap"
};


/* 设备索引 */
int  __g_netif_idx = 0;

/******************************************************************************/
aw_local const char *nic_type_to_str (aw_netif_type_t type)
{
    aw_local aw_const char *names[] = {
        "RAW ",
        "ETH ",
        "WIFI",
    };

    return names[type];
}




/******************************************************************************/
void __net_info (aw_netif_t *p_netif)
{
    aw_bool_t          up;
    uint8_t         type;
    struct in_addr       ip;
    aw_eth_mac_t    mac;
    aw_bool_t          dhcp;
    char           *name = NULL;

    aw_netif_name_get(p_netif, &name);
    aw_netif_type_get(p_netif, &type);
    aw_netif_is_link_up(p_netif, &up);

    aw_kprintf("Name:       %s" ENDL, name);
    aw_kprintf("Type:       %s" ENDL, nic_type_to_str(type));
    aw_kprintf("Link:       %s" ENDL, up ? "up  " : "down");

    aw_netif_ipv4_ip_get(p_netif, &ip);
    aw_kprintf("IP:         "APP_PRINT_IPADDR_FMT ENDL, APP_PRINT_IPADDR_VAL(ip.s_addr));

    aw_netif_ipv4_netmask_get(p_netif, &ip);
    aw_kprintf("Netmask:    "APP_PRINT_IPADDR_FMT ENDL, APP_PRINT_IPADDR_VAL(ip.s_addr));

    aw_netif_ipv4_gw_get(p_netif, &ip);
    aw_kprintf("Gateway:    "APP_PRINT_IPADDR_FMT ENDL, APP_PRINT_IPADDR_VAL(ip.s_addr));

    aw_netif_hwaddr_get(p_netif, mac.octet, 6);
    aw_kprintf("Mac:        "APP_PRINT_HWADDR_FMT ENDL, APP_PRINT_HWADDR_VAL(mac.octet));

    aw_netif_dhcpc_is_on(p_netif, &dhcp);
    aw_kprintf("DHCPC:      %s" ENDL, dhcp ? "on" : "off");
}




/******************************************************************************/
int app_net (int argc, char **argv, struct aw_shell_user *p_user)
{
    struct in_addr      addr;
    static aw_netif_t  *p_netif;
    char          *ip = NULL, *mask = NULL, *gate = NULL;
    int            idx = -1;
    char *         p_dhcpc = NULL;
    static uint8_t init_flag = 0;

    /* 默认打开eth0 */
    if (init_flag == 0) {
        init_flag = 1;
        p_netif = aw_netif_dev_open("eth0");
        aw_kprintf("open eth0\r\n");
    }

    if (aw_shell_args_get(p_user, "-select", 1) == AW_OK) {
        idx = aw_shell_long_arg(p_user, 1);
        if (idx > AW_NELEMENTS(__dev_idx)) {
            aw_kprintf("invalid index %d!\r\n", idx);
            return AW_OK;
        } else {
            if (idx < 0) {
                /* 无索引更新，使用默认 */
                idx = __g_netif_idx;
            }

            /* 引用网卡设备 */
            p_netif = aw_netif_dev_open(__dev_idx[idx]);
            if (p_netif == NULL) {
                aw_kprintf("can not find net device %d!\r\n", idx);
                return AW_OK;
            }

            /* 更新新的索引 */
            __g_netif_idx = idx;
        }

    } else if (aw_shell_args_get(p_user, "-dhcpc", 1) == AW_OK) {
        p_dhcpc = aw_shell_str_arg(p_user, 1);
        if (!strcmp(p_dhcpc, "off")) {
             aw_netif_dhcpc_stop(p_netif);
        } else if (!strcmp(p_dhcpc, "on")) {
            aw_netif_dhcpc_start(p_netif);
        }
    } else if (aw_shell_args_get(p_user, "-config", 3) == AW_OK) {
        ip   = aw_shell_str_arg(p_user, 1);
        mask = aw_shell_str_arg(p_user, 2);
        gate = aw_shell_str_arg(p_user, 3);

        if (ip) {
            addr.s_addr = inet_addr(ip);
            aw_netif_ipv4_ip_set(p_netif, &addr);
        }

        if (mask) {
            addr.s_addr = inet_addr(mask);
            aw_netif_ipv4_netmask_set(p_netif, &addr);
        }

        if (gate) {
            addr.s_addr = inet_addr(gate);
            aw_netif_ipv4_gw_set(p_netif, &addr);
        }

    } else {
        __net_info(p_netif);
    }

    return AW_OK;
}
/** [src_app_net] */

#endif /* APP_NET */
