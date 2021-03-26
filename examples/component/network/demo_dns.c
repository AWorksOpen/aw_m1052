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
 * \brief 网络管理例程（DNS解析管理）
 *
 * - 操作步骤：
 *   1. 需要在 aw_prj_params.h 里使能至少一个网卡，例如：
 *      1) 以太网
 *      2) Wi-Fi
 *   2. 需要在aw_prj_params.h头文件里使能
 *      - AW_COM_SHELL_SERIAL
 *
 * - 实验现象：
 *   1. 执行 "dns" 查看系统 DNS 服务器配置
 *   2. 执行 "dns set 0 addr 10.1.1.1" 设置主 DNS 服务器地址为 10.1.1.1
 *   3. 执行 "dns set 1 addr 172.16.1.1" 设置备份 DNS 服务器地址为 172.16.1.1
 *
 *
 * \par 源代码
 * \snippet demo_dns.c src_demo_dns
 *
 * \internal
 * \par Modification history
 * - 1.00 18-06-05  phd, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_dns DNS解析管理
 * \copydoc demo_dns.c
 */

/** [src_demo_dns] */

#include "aworks.h"
#include "aw_shell.h"

#include "aw_sockets.h"
#include "aw_dns.h"
#include "lwip/ip_addr.h"
#include "lwip/inet.h"
#include <string.h>

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


aw_local int __shell_dns (int argc, char* argv[], struct aw_shell_user *sh)
{
    aw_local aw_const char help_info[] = {
        "Examples:" ENDL
        "    dns set 0 addr 10.1.1.1     Set nameserver 0 to 10.1.1.1" ENDL
    };

    aw_err_t        err;
    int             idx;
    struct in_addr  dest_addr;

    switch (argc) {

    case 0:
        for (idx = 0; idx < DNS_MAX_SERVERS; idx++) {
            aw_dns_get(idx, &dest_addr);
            shell_printl("nameserver%d = %s", idx, inet_ntoa(dest_addr));
        }
        break;

    case 4:
        if (!strcmp("set", argv[0]) && !strcmp("addr", argv[2]) && inet_aton(argv[3], &dest_addr)) {
            idx = atoi(argv[1]);
            shell_dbg_printl("nameserver = %d", idx);
            shell_dbg_printl("Address    = %s", inet_ntoa(dest_addr));
            err = aw_dns_set(idx, &dest_addr);
            #ifdef LWIP_1_4_X
            ip_addr_t dnsserver;
            inet_addr_to_ipaddr(&dnsserver, &dest_addr);
            /*LWIP1.4不支持设置dns域名为0.0.0.*/
            if(ip_addr_isany(&dnsserver)){
               err = AW_ERROR;
            }
            #endif
            shell_printl("%s", (AW_OK == err) ? "OK" : "Failed.");
        } else {
            shell_printl("%s", help_info);
        }
        break;

    default:
        shell_printl("%s", help_info);
        break;
    }

    return 0;
}

aw_local aw_const struct aw_shell_cmd __g_shell_cmd_tbl[] = {
    {
        __shell_dns,
        "dns",
        "dns <subcommand> [param]"
    },
};

/******************************************************************************/
void demo_dns_entry (void)
{
    aw_local struct aw_shell_cmd_list cl;

    (void)aw_shell_register_cmds(&cl,
                                 &__g_shell_cmd_tbl[0],
                                 &__g_shell_cmd_tbl[AW_NELEMENTS(__g_shell_cmd_tbl)]);
}

/** [src_demo_dns] */

/* end of file */
