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
 * \brief 网络管理例程
 *
 * - 操作步骤：
 *   1. 需要在 aw_prj_params.h 里使能至少一个网卡，例如：
 *      1) 以太网，配合网线使用
 *      2) Wi-Fi，配合Wi-Fi例程以及带Wi-Fi的开发板使用
 *   2. 需要在aw_prj_params.h头文件里使能
 *      - AW_COM_SHELL_SERIAL
 *   3. demo中的网络配置和系统中的网络配置有冲突，需要注释掉 AW_NET_TOOLS宏，
 *      屏蔽掉系统中已注册的网络配置的shell命令
 *
 *
 * - 实验现象，以以太网操作为例：
 *   1. 执行 "ip link" 查看所有接口的链路状态
 *   2. 执行 "ip addr" 查看所有接口的地址
 *   3. 执行 "ip link show dev eth0" 查看所有接口的地址 eth0 的链路状态
 *   4. 执行 "ip addr show dev eth0" 查看所有接口的地址 eth0 的地址
 *   5. 执行 "ip addr add 192.168.1.2/24 dev eth0" 设置 eth0 使用静态地址
 *   6. 执行 "ip addr auto dev eth0" 配置 eth0 使用 DHCP 获取动态地址
 *   7. 执行 "ip link set down dev eth0" 禁用 eth0(之前启用的 DHCP 会被同时关闭)
 *   8. 执行 "ip link set up dev eth0" 启用 eth0(之前启用的 DHCP 不会自动打开)
 *
 * \par 源代码
 * \snippet demo_netif.c src_demo_netif
 *
 * \internal
 * \par Modification history
 * - 1.00 18-05-04  phd, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_netif 网卡管理
 * \copydoc demo_netif.c
 */

/** [src_demo_netif] */
#include "aworks.h"
#include "aw_net.h"
#include "aw_hostname.h"
#include "aw_netif.h"
#include "aw_sockets.h"
#include "aw_shell.h"

#include <string.h>
#include <stdio.h>

#define APP_PRINT_HWADDR_FMT \
    "%02X:%02X:%02X:%02X:%02X:%02X"

#define APP_PRINT_HWADDR_VAL(addr) \
    (addr)[0], (addr)[1], (addr)[2], (addr)[3], (addr)[4], (addr)[5]

#define DEBUG_PING              1

#ifndef ENDL
    #define ENDL                "\r\n"
#endif

#define aw_kprintl(fmt, args...) \
    aw_kprintf(fmt ENDL, ##args)

#define shell_printl(fmt, args...) \
    aw_shell_printf(sh, fmt ENDL, ##args)

#if DEBUG_PING
    #define shell_dbg_printl(fmt, args...) \
        aw_shell_printf(sh, fmt ENDL, ##args)
#else
    #define shell_dbg_printl(...)
#endif


aw_local void str_2_ip_msk (aw_const char *s,
                            struct in_addr *ip,
                            struct in_addr *nm,
                            struct in_addr *gw)
{
    char    *msk;
    uint8_t *first, *last;
    uint8_t  temp;

    msk = strstr(s, "/");

    if (msk) {
        int len, i;

        *msk = '\0';
        inet_aton(s, ip);
        len = atoi(++msk);
        nm->s_addr = 0;
        for (i = 31; i >= 32 - len; i--) {
            AW_BIT_SET(nm->s_addr, i);
        }

        first = (uint8_t *)&nm->s_addr;
        last  = ((uint8_t *)&nm->s_addr) + 3;
        for (i = 0; i < 2; i++) {
            temp = *(first + i);
            *(first + i) = *(last - i);
            *(last - i) = temp;
        }
    } else {
        aw_kprintf("Format error\r\n");
    }
}

aw_local aw_const char *netif_type_to_str (aw_netif_type_t type)
{
    aw_local aw_const char *names[] = {
        "RAW ",
        "ETH ",
        "WIFI",
    };

    return names[type];
}

aw_local void netif_addr_dumper (aw_netif_t *p_netif, void *p_arg)
{
    aw_bool_t       up;
    uint8_t         type;
    struct in_addr  ip;
    uint8_t         mac[6];
    char           *name = NULL;
    struct aw_shell_user *sh = (struct aw_shell_user *)p_arg;

    aw_netif_name_get(p_netif, &name);
    aw_netif_type_get(p_netif, &type);
    aw_netif_is_link_up(p_netif, &up);

    shell_printl("Name           : %s", name);

    shell_printl("");

    aw_netif_ipv4_ip_get(p_netif, &ip);
    shell_printl("IP Address     : %s", inet_ntoa(ip));
    aw_netif_ipv4_netmask_get(p_netif, &ip);
    shell_printl("Netmask        : %s", inet_ntoa(ip));
    aw_netif_ipv4_gw_get(p_netif, &ip);
    shell_printl("Gateway        : %s", inet_ntoa(ip));
    aw_netif_hwaddr_get(p_netif, mac, 6);
    shell_printl("MAC Address    : " APP_PRINT_HWADDR_FMT,
            APP_PRINT_HWADDR_VAL(mac));

    shell_printl("");

    aw_netif_dhcpc_is_on(p_netif, &up);
    shell_printl("DHCP           : %s", up ? "On" : "Off");
}

aw_local void netif_link_dumper (aw_netif_t *p_netif, void *p_arg)
{
    aw_bool_t   up;
    aw_bool_t   link_up;
    uint8_t     type;
    char       *name = NULL;
    struct aw_shell_user *sh = (struct aw_shell_user *)p_arg;

    aw_netif_name_get(p_netif, &name);
    aw_netif_type_get(p_netif, &type);
    aw_netif_is_up(p_netif, &up);
    aw_netif_is_link_up(p_netif, &link_up);

    shell_printl("%08s %08s %08s", name, netif_type_to_str(type), (up && link_up) ? "Up" : "Down");
}

aw_local void netif_addr_iter (aw_netif_t *p_netif, void *p_arg)
{
    struct aw_shell_user *sh = (struct aw_shell_user *)p_arg;

    shell_printl("------------------------------");
    netif_addr_dumper(p_netif, p_arg);
}

aw_local void netif_link_iter (aw_netif_t *p_netif, void *p_arg)
{
    netif_link_dumper(p_netif, p_arg);
}

aw_local int shell_ip_sub_addr (int argc, char* argv[], struct aw_shell_user *sh)
{
    if (1 > argc) {
        shell_printl(ENDL "Hostname: %s", aw_net_hostname_get());
        aw_netif_iterate(netif_addr_iter, sh);
    } else {
        if (!strcmp("show", argv[0])) {
            if (!strcmp("dev", argv[1])) {
                aw_netif_t *p_netif;
                uint32_t    state;

                if (argc > 2 && NULL != argv[2]) {
                    p_netif = aw_netif_dev_open(argv[2]);
                    if (AW_OK == aw_netif_state_get(p_netif, &state)) {
                        netif_addr_dumper(p_netif, sh);
                    } else {
                        shell_printl("%s NOT exist.", argv[2]);
                    }
                    aw_netif_dev_close(p_netif);
                } else {
                    shell_printl("need dev name");
                }
            } else {
                shell_printl(ENDL "Hostname: %s", aw_net_hostname_get());
                aw_netif_iterate(netif_addr_iter, sh);
            }
        } else if (!strcmp("add", argv[0])) {
            if (4 == argc) {
                struct in_addr ip;
                struct in_addr nm;
                struct in_addr gw;
                aw_netif_dhcpc_stop_by_name(argv[3]);
                aw_netif_ipv4_addr_get_by_name(argv[3], &ip, &nm, &gw);
                str_2_ip_msk(argv[1], &ip, &nm, &gw);
                aw_netif_ipv4_addr_set_by_name(argv[3], &ip, &nm, &gw);
            } else {
                shell_printl("ip addr add 192.168.1.2/24 dev eth0");
            }
        } else if (!strcmp("auto", argv[0])) {
            if (argc < 3) {
                shell_printl("need dev name");
            } else {
                aw_netif_dhcpc_start_by_name(argv[2]);
            }
        } else {
            shell_printl("unknow command");
        }
    }
    return 0;
}

aw_local int shell_ip_sub_link (int argc, char* argv[], struct aw_shell_user *sh)
{
    if (1 > argc) {
        shell_printl("%08s,%08s,%08s", "Name", "Type", "Link");
        aw_netif_iterate(netif_link_iter, sh);
    } else {
        if (!strcmp("show", argv[0])) {
            if (!strcmp("dev", argv[1])) {
                aw_netif_t *p_netif;
                uint32_t    state;

                if (argc > 2 && NULL != argv[2]) {
                    p_netif = aw_netif_dev_open(argv[2]);
                    if (AW_OK == aw_netif_state_get(p_netif, &state)) {
                        shell_printl("%08s,%08s,%08s", "Name", "Type", "Link");
                        netif_link_dumper(p_netif, sh);
                    } else {
                        shell_printl("%s NOT exist.", argv[2]);
                    }
                    aw_netif_dev_close(p_netif);
                } else {
                    shell_printl("need dev name");
                }
            } else {
                aw_netif_iterate(netif_link_iter, sh);
            }
        } else if (!strcmp("set", argv[0])) {
            if (3 > argc) {
                shell_printl("ip link set up dev eth0");
            } else {
                aw_netif_t *p_netif;
                uint32_t    state;

                if (3 < argc) {
                    p_netif = aw_netif_dev_open(argv[3]);
                    if (AW_OK == aw_netif_state_get(p_netif, &state)) {
                        if (!strcmp("up", argv[1])) {
                            aw_netif_up(p_netif);
                        } else if (!strcmp("down", argv[1])){
                            aw_netif_down(p_netif);
                        }
                    } else {
                        shell_printl("%s NOT exist.", argv[3]);
                    }
                    aw_netif_dev_close(p_netif);
                } else {
                    shell_printl("need dev name");
                }
            }
        } else {
            shell_printl("unknow command");
        }
    }
    return 0;
}

aw_local int shell_ip (int argc, char* argv[], struct aw_shell_user *sh)
{
    if (1 > argc) {
        return AW_ERROR;
    } else {
        if (!strcmp("addr", argv[0])) {
            shell_ip_sub_addr(argc - 1, &argv[1], sh);
        } else if (!strcmp("link", argv[0])) {
            shell_ip_sub_link(argc - 1, &argv[1], sh);
        }
    }
    return 0;
}

aw_local aw_const struct aw_shell_cmd g_shell_cmd_tbl[] = {
    {shell_ip, "ip", "ip <subcommand> [param]"},
};

/******************************************************************************/
void demo_netif_entry (void)
{
    aw_local struct aw_shell_cmd_list cl;

    (void)aw_shell_register_cmds(&cl,
                                 &g_shell_cmd_tbl[0],
                                 &g_shell_cmd_tbl[AW_NELEMENTS(g_shell_cmd_tbl)]);
}

/** [src_demo_netif] */

/* end of file */
