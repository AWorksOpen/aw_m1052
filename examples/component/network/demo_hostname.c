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
 * \brief 网络管理例程（主机名管理）
 *
 * - 操作步骤：
 *   1. 需要在 aw_prj_params.h 里使能至少一个网卡，例如：
 *      1) 以太网
 *      2) Wi-Fi
 *   2. 需要在aw_prj_params.h头文件里使能
 *      - AW_COM_SHELL_SERIAL
 *
 * - 实验现象：
 *   1. 执行 "hostname" 查看系统主机名
 *   2. 执行 "hostname set AWorksPlatform" 设置主机名为 AWorksPlatform
 *
 * \par 源代码
 * \snippet demo_hostname.c src_demo_hostname
 *
 * \internal
 * \par Modification history
 * - 1.00 18-06-05  phd, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_hostname 主机名管理
 * \copydoc demo_hostname.c
 */

/** [src_demo_hostname] */

#include "aworks.h"
#include "aw_shell.h"

#include "aw_sockets.h"
#include "aw_hostname.h"

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


aw_local int shell_hostname (int argc, char* argv[], struct aw_shell_user *sh)
{
    aw_local aw_const char help_info[] = {
        "Examples:" ENDL
        "    hostname set AWorksPlatform     Set hostname to AWorksPlatform" ENDL
    };

    switch (argc) {

    case 0:
        shell_printl("hostname = %s", aw_net_hostname_get());
        break;

    case 2:
        if (!strcmp("set", argv[0])) {
            shell_dbg_printl("New hostname = %s", aw_net_hostname_set(argv[1]));
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

aw_local aw_const struct aw_shell_cmd g_shell_cmd_tbl[] = {
    {
        shell_hostname,
        "hostname",
        "hostname <subcommand> [param]"
    },
};

/******************************************************************************/
void demo_hostname_entry (void)
{
    aw_local struct aw_shell_cmd_list cl;

    (void)aw_shell_register_cmds(&cl,
                                 &g_shell_cmd_tbl[0],
                                 &g_shell_cmd_tbl[AW_NELEMENTS(g_shell_cmd_tbl)]);
}

/** [src_demo_hostname] */

/* end of file */
