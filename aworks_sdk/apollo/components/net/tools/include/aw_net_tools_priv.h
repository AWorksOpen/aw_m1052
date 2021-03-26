/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/
#include "aworks.h"

#include "aw_delay.h"
#include "aw_shell.h"
#include "aw_vdebug.h"

#include "aw_netif.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NET_TOOLS_DEBUG_DETAIL_PRINT 0

#ifndef ENDL
#    define ENDL "\r\n"
#endif

#define NET_TOOLS_PRINT_IPADDR_FMT "%s"

#define NET_TOOLS_PRINT_HWADDR_FMT "%02X:%02X:%02X:%02X:%02X:%02X"

#define NET_TOOLS_PRINT_CCODE_FMT "%c%c/%d"

#define NET_TOOLS_PRINT_CCODE_VAL(c)                                          \
    (c >> 0 & 0xFF), (c >> 8 & 0xFF), (c >> 16 & 0xFF)

#define NET_TOOLS_PRINT_HWADDR_VAL(addr)                                      \
    (addr)[0], (addr)[1], (addr)[2], (addr)[3], (addr)[4], (addr)[5]

#define aw_kprintl(fmt, args...) aw_kprintf(fmt ENDL, ##args)

#define shell_printf(args...) aw_shell_printf(sh, ##args)

#define shell_printl(fmt, args...) aw_shell_printf(sh, fmt ENDL, ##args)

#if NET_TOOLS_DEBUG_DETAIL_PRINT
#    define shell_dbg_printl(fmt, args...)                                    \
        aw_shell_printf(sh, fmt ENDL, ##args)
#else
#    define shell_dbg_printl(...)
#endif

#define NET_TOOLS_RAISE_ERROR(e)                                              \
    e = AW_TRUE;                                                              \
    break

#define NET_TOOLS_SHELL_REQ_HELP(argc, argv)                                  \
    ((1 == argc)                                                              \
     && (('?' == argv[0][0]) || !strcmp("help", argv[0])                      \
         || !strcmp("--help", argv[0])))

#define NET_TOOLS_CHECK_SHELL_ARGC(exp)                                       \
    do {                                                                      \
        if (!(exp) || NET_TOOLS_SHELL_REQ_HELP(argc, argv)) {                 \
            shell_printl("%s", help_info);                                    \
            return 0;                                                         \
        }                                                                     \
    } while (0);

void __net_tools_hlp_fmt_err(struct aw_shell_user *sh);

/* end of file */
