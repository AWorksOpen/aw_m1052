/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief 网卡通信测试例程
 *
 * - 本例程用法与 Windows 的 ping 程序相同。
 *
 * - 操作步骤：
 *   1. 需要在 aw_prj_params.h 里使能至少一个网卡，例如：
 *      1) 以太网
 *      2) Wi-Fi
 *         使用Wi-Fi测试时，请配合wifi例程使用
 *   2. 需要在aw_prj_params.h头文件里使能
 *      - AW_COM_CONSOLE
 *      - 调试串口使能宏
 *   3. 在aw_prj_params.h中关闭
 *      - AW_NET_TOOLS宏
 *
 * - 实验现象：
 *   1. 执行 "ping 192.168.0.8" ,ping 一个ip地址
 *   2. 执行 "ping www.baidu.com" ,检查网络是否连接成功(需开启网络)
 *
 * \par 源代码
 * \snippet demo_ping.c src_demo_ping
 *
 * \internal
 * \par modification history:
 * - 1.00 18-06-05  phd, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_ping 网卡通信测试
 * \copydoc demo_ping.c
 */

/** [src_demo_ping] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_serial.h"
#include "aw_shell.h"

#include "aw_net.h"
#include "aw_ping.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "aw_prj_params.h"

/** ping receive timeout - in milliseconds */
#define DEFAULT_RCV_TIMEO       2000

/** ping delay - in milliseconds */
#define DEFAULT_DELAY           1000

/** max ping data size - in bytes */
#define MAX_PING_DATA_SIZE      4096

#define DEFAULT_PING_COUNT      4

#define DEFAULT_PING_TTL        96

#define DEFAULT_PING_DATA_SIZE  32


#define DEBUG_PING              0

#define HOST_STRLEN_MAX IPADDR_STRLEN_MAX
#define DEFAULT_TCP_PORT        "80"

#ifndef ENDL
    #define ENDL                "\r\n"
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

typedef struct {
    unsigned sent;
    unsigned received;
    unsigned lost;
    unsigned max_time;
    unsigned min_time;
    unsigned average;
    unsigned ping_time;
} ping_result_t;

struct ping_params {
    char s_addr[HOST_STRLEN_MAX];
    char d_name[HOST_STRLEN_MAX];
    char d_addr[HOST_STRLEN_MAX];

    char *portnr;

    struct sockaddr_storage to;
    struct sockaddr_storage from;

    size_t ping_size;
    int    count;
    int    interval_ms;
    int    timeout_ms;
    int    ttl;

    aw_bool_t eternal;
};
typedef struct {
    aw_ping_t ping;
    unsigned cnt;
    ping_result_t result;
} ping_param_t;

aw_local sa_family_t _inet_pton(const char *host, struct sockaddr_storage *to)
{
    if (inet_pton(AF_INET, host, &((struct sockaddr_in *) to)->sin_addr)) {
        to->ss_family = AF_INET;
        to->s2_len    = sizeof(struct sockaddr_in);
        return AF_INET;
#if LWIP_IPV6
    } else if (inet_pton(
                   AF_INET6, host, &((struct sockaddr_in6 *) to)->sin6_addr)) {
        to->ss_family = AF_INET6;
        to->s2_len    = sizeof(struct sockaddr_in6);
        return AF_INET6;
#endif
    }
    return AF_UNSPEC;
}
aw_local int get_opts(struct ping_params *  param,
                      int                   argc,
                      char *                argv[],
                      struct aw_shell_user *sh)
{
    int result = AW_ERROR;

    param->eternal     = AW_FALSE;
    param->ttl         = DEFAULT_PING_TTL;
    param->ping_size   = DEFAULT_PING_DATA_SIZE;
    param->count       = DEFAULT_PING_COUNT;
    param->portnr      = DEFAULT_TCP_PORT;
    param->timeout_ms  = DEFAULT_RCV_TIMEO;
    param->interval_ms = DEFAULT_DELAY;

    for (;;) {
        if (AW_OK == aw_shell_args_get(sh, "-t", 0)) {
            param->eternal = AW_TRUE;
            shell_printl("Ctrl + C to stop.");
        } else if (AW_OK == aw_shell_args_get(sh, "-i", 1)) {
            param->ttl = aw_shell_long_arg(sh, 1);
        } else if (AW_OK == aw_shell_args_get(sh, "-l", 1)) {
            param->ping_size = aw_shell_long_arg(sh, 1);
        } else if (AW_OK == aw_shell_args_get(sh, "-n", 1)) {
            param->count = aw_shell_long_arg(sh, 1);
        } else if (AW_OK == aw_shell_args_get(sh, "-p", 1)) {
            param->portnr = aw_shell_str_arg(sh, 1);
        } else if (AW_OK == aw_shell_args_get(sh, "-w", 1)) {
            param->timeout_ms = aw_shell_long_arg(sh, 1);
        } else if (AW_OK == aw_shell_args_get(sh, "-I", 1)) {
            param->interval_ms = aw_shell_long_arg(sh, 1);
        } else if (AW_OK == aw_shell_args_get(sh, "-S", 1)) {
            _inet_pton(aw_shell_str_arg(sh, 1), &param->from);
        } else if (AW_OK == aw_shell_args_get(sh, NULL, 1)) {
            strncpy(
                param->d_name, aw_shell_str_arg(sh, 0), sizeof(param->d_name));
            result = AW_OK;
        } else {
            break;
        }
    }
    return result;
}
aw_local int lookup_icmp(const char *host, struct addrinfo **res)
{
    struct addrinfo hints;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_ICMP;

    return getaddrinfo(host, NULL, &hints, res);
}
aw_local aw_bool_t ctrlc_pressed(struct aw_shell_user *sh)
{
    int ch = aw_shell_try_getc(sh);

    return (3 == ch) ? AW_TRUE : AW_FALSE;
}
aw_local aw_err_t is_any_addr(const struct sockaddr_storage *sa)
{
    switch (sa->ss_family) {

    case AF_INET: {
        struct sockaddr_in *addr = (struct sockaddr_in *) sa;
        if (INADDR_ANY == addr->sin_addr.s_addr) {
            return AW_OK;
        }
    } break;

#if LWIP_IPV6
    case AF_INET6: {
        struct sockaddr_in6 *addr = (struct sockaddr_in6 *) sa;
        if (!memcmp(&in6addr_any, &addr->sin6_addr, sizeof(in6addr_any))) {
            return AW_OK;
        }
    } break;
#endif

    default:
        break;
    }
    return AW_ERROR;
}
aw_local const char *
_inet_ntop(int af, const void *src, char *dst, socklen_t size)
{
    if (AF_INET == af) {
        return inet_ntop(
            af, &((struct sockaddr_in *) src)->sin_addr, dst, size);
#if LWIP_IPV6
    } else if (AF_INET6 == af) {
        return inet_ntop(
            af, &((struct sockaddr_in6 *) src)->sin6_addr, dst, size);
#endif
    } else {
        return NULL;
    }
}
aw_local int __shell_ping(int argc, char *argv[], struct aw_shell_user *sh)
{
    aw_local aw_const char help_info[] = {
        "Command:ping [-t] [-n count] [-l size] [-i TTL]" ENDL
        "        ping [-I interval] [-w timeout] [-S srcaddr] target_name" ENDL
            ENDL "Options:" ENDL
        "    -t             Ping the specified host until stopped." ENDL
        "                   To see statistics and continue - type "
        "Control-Break;" ENDL
        "                   To stop - type Control-C." ENDL
        "    -n count       Number of echo requests to send." ENDL
        "    -l size        Send buffer size." ENDL
        "    -i TTL         Time To Live." ENDL
        "    -I interval    Wait interval ms before next send." ENDL
        "    -w timeout     Timeout in milliseconds to wait for each "
        "reply." ENDL "    -S srcaddr     Source address to use." ENDL
    };

    struct ping_params pp;
    aw_ping_t          ping;
    sa_family_t        domain;
    int                send_cnt;
    int                recv_cnt;
    int                rtt_ms_min;
    int                rtt_ms_max;
    int                rtt_ms_tot;
    uint8_t            ttl_recv;
    aw_bool_t          target_name_is_ipaddr;
    struct addrinfo *  resolved;

    NET_TOOLS_CHECK_SHELL_ARGC(0 < argc);

    memset(&pp, 0, sizeof(pp));
    memset(&ping, 0, sizeof(ping));

    if (AW_OK != get_opts(&pp, argc, argv, sh)) {
        shell_printl("%s", help_info);
        return 0;
    }

    domain = _inet_pton(pp.d_name, &pp.to);
    switch (domain) {

#if LWIP_IPV6
    case AF_INET6:
        /* FALL-THROUGH */
#endif
    case AF_INET:
        target_name_is_ipaddr = AW_TRUE;
        break;

    default:
        target_name_is_ipaddr = AW_FALSE;
        break;
    }

    if (lookup_icmp(pp.d_name, &resolved)) {
        shell_printl("%s request could not find host \"%s\"",
                     "Ping",
                     pp.d_name);
        shell_printl("    Please check the name and try again.");
        return 0;
    }

    if (AF_INET == resolved->ai_family) {
        memcpy(&pp.to,
               (struct sockaddr_in *) resolved->ai_addr,
               sizeof(struct sockaddr_in));
#if LWIP_IPV6
    } else if (AF_INET6 == resolved->ai_family) {
        memcpy(&pp.to,
               (struct sockaddr_in6 *) resolved->ai_addr,
               sizeof(struct sockaddr_in6));
#endif
    }
    strncpy(pp.d_name, resolved->ai_canonname, sizeof(pp.d_name));
    if (AW_FALSE == target_name_is_ipaddr) {
        domain = pp.to.ss_family;
    }

    if ((NULL == _inet_ntop(domain, &pp.to, pp.d_addr, sizeof(pp.d_addr)))
        || (AW_OK == is_any_addr(&pp.to))) {
        shell_printl("Invalid target address.");
        goto fail_exit;
    }

    if (target_name_is_ipaddr) {
        shell_printl(
            ENDL "Pinging %s with %d bytes of data:", pp.d_name, pp.ping_size);
    } else {
        shell_printl(ENDL "Pinging %s [%s] with %d bytes of data:",
                     pp.d_name,
                     pp.d_addr,
                     pp.ping_size);
    }

    rtt_ms_min = pp.timeout_ms;
    rtt_ms_max = 0;
    rtt_ms_tot = 0;
    recv_cnt   = 0;

    for (send_cnt = 0; pp.eternal || send_cnt < pp.count; send_cnt++) {
        int result;

        ttl_recv = pp.ttl;

#if defined(LWIP_2_X)
        result = aw_ping(&ping,
                         &pp.to,
                         &pp.from,
                         (uint8_t *) &ttl_recv,
                         pp.ping_size,
                         pp.timeout_ms);
#elif defined(LWIP_1_4_X)
        result = aw_ping(&ping,
                         &((struct sockaddr_in *) (&pp.to))->sin_addr,
                         &((struct sockaddr_in *) (&pp.from))->sin_addr,
                         pp.ttl,
                         (uint8_t *) &ttl_recv,
                         pp.ping_size,
                         pp.timeout_ms);
#endif
        if (0 > result) {
            if (-AW_EAGAIN == result) {
                shell_printl("PING: transmit failed. General failure.");
            } else {
                shell_printl("Request timed out.");
            }
        } else {
            recv_cnt++;

            if (result > rtt_ms_max) {
                rtt_ms_max = result;
            }
            if (result < rtt_ms_min) {
                rtt_ms_min = result;
            }

            rtt_ms_tot += result;

            if (result == 0) {
                shell_printl("Reply from %s: bytes=%d time<1ms",
                             pp.d_addr,
                             pp.ping_size);
            } else {
                shell_printl("Reply from %s: bytes=%d time=%dms",
                             pp.d_addr,
                             pp.ping_size,
                             result);
            }
        }

        if (ctrlc_pressed(sh)) {
            send_cnt++;
            break;
        }

        if (pp.eternal || send_cnt < pp.count - 1) {
            aw_mdelay(pp.interval_ms);
        }
    }

    shell_printl(ENDL "Ping statistics for %s:", pp.d_addr);
    shell_printl(
        "    Packets: Sent = %d, Received = %d, Lost = %d (%d%% loss),",
        send_cnt,
        recv_cnt,
        send_cnt - recv_cnt,
        (send_cnt - recv_cnt) * 100 / send_cnt);

    if (recv_cnt > 0) {
        int rtt_ms_avg = rtt_ms_tot / recv_cnt;
        shell_printl("Approximate round trip times in milli-seconds:");
        shell_printl("    Minimum = %dms, Maximum = %dms, Average = %dms",
                     rtt_ms_min,
                     rtt_ms_max,
                     rtt_ms_avg);
    }
    if (pp.eternal) {
        shell_printl("Control-C" ENDL "^C");
    }
fail_exit:
    freeaddrinfo(resolved);
    return 0;
}

/******************************************************************************/
void demo_ping_entry (void)
{
    aw_local struct aw_shell_cmd_list cl;
    aw_local aw_const struct aw_shell_cmd tbl[] = {
        {
            __shell_ping,
            "ping",
            "[-t] [-n count] [-l size] [-i TTL] [-w timeout] [-S srcaddr] target_name"
        }
    };

    (void) aw_shell_register_cmds(&cl, &tbl[0], &tbl[AW_NELEMENTS(tbl)]);
}
/** [src_demo_ping] */

/* end of file */
