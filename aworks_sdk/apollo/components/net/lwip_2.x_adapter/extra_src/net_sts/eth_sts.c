/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/
/**
 * \file
 * \brief 以太网网络状态检测
 *
 * \internal
 * \par modification history:
 * - 1.00 20-03-20  vih, first implementation
 * \endinternal
 */

#include "aworks.h"
#include <stdlib.h>
#include <string.h>

#include "aw_assert.h"
#include "aw_delay.h"
#include "aw_vdebug.h"

#include "lwip/ext/net_sts.h"
#include "lwip/ext/netif_route.h"
#include "net_sts_internal.h"

#include "aw_netif.h"
#include "aw_ping.h"

/******************************************************************************/
#define PFX "ETH_STS"

#define __LOG_DEV_DEF(p_dev)  \
    aw_net_sts_log_t _pfn_log_out_ = \
            (p_dev ? ((aw_net_sts_dev_t *)p_dev)->attr.pfn_log_out : NULL )

#define __LOG_NET_STS           1
#define __MASK_NET_PING         1

/******************************************************************************/
#define __PING_SEND_TTL     64  /* 96 */
#define __PING_TIMEOUT_MS   3000
#define __PING_DATA_SIZE    8

#define __HOST_NAME_MAXLEN      128
/******************************************************************************/
#define __TIMEOUT_MSEC(tick_end, timeout) \
    for (tick_end = aw_sys_tick_get() + aw_ms_to_ticks(timeout); \
            aw_sys_tick_get() < tick_end; )


/* The limit minimum is 10S */
#define __CHK_MS_RECTIFY(tm)  ((tm) >= 10000 ? tm : 10000 )


enum { __BITMAP_SIZE = sizeof(uint8_t) * 8 };
#define BITMAP_SET(flag_array, bit) \
    {if (bit <= sizeof(flag_array) * __BITMAP_SIZE) {\
        flag_array[bit / __BITMAP_SIZE] |= 1 << (bit % __BITMAP_SIZE);\
    }}
#define BITMAP_CLR(flag_array, bit) \
    {if (bit <= sizeof(flag_array) * __BITMAP_SIZE) {\
        flag_array[bit / __BITMAP_SIZE] &= ~(1 << (bit % __BITMAP_SIZE));\
    }}
#define BITMAP_IS_VALID(flag_array, bit) \
    ((bit <= sizeof(flag_array) * __BITMAP_SIZE) ? \
        (flag_array[bit / __BITMAP_SIZE] >> (bit % __BITMAP_SIZE)) & 1:0)


/******************************************************************************/
typedef enum {
    __ETH_STAT_INIT = 0,
    __ETH_STAT_UP,
    __ETH_STAT_LINK_UP,

    __ETH_STAT_INET_OK,
    __ETH_STAT_CHK_INET_OK,

    __ETH_STAT_SNET_OK,
    __ETH_STAT_CHK_SNET_OK,
} __eth_stat_t;

typedef struct {
    aw_net_sts_dev_t         dev;
    uint8_t                  rst_num;
    int                      dev_id;
    __eth_stat_t             stat;
    aw_bool_t                flag_delay;

    struct in_addr           dest_addr;

    char                     ip_buff[2][__HOST_NAME_MAXLEN];

    AW_SEMB_DECL(            wakeup);

    uint8_t                  evt_flags[1];
    union {
        struct {
            aw_net_sts_dev_init_t  attr;
        } reinit;
    } evt_params;

} __net_sts_eth_dev_t;


enum {
    __EVT_FLAG_SUSPEND = 1,
    __EVT_FLAG_STARTUP,
    __EVT_FLAG_REINIT,
};

/******************************************************************************/

/* 返回 >= 0 表示 reply 时间长度， < 0 响应失败 */
static int __ping_check (aw_net_sts_dev_t   *p_dev,
                         struct netif       *netif,
                         const char         *p_chk_addr)
{
    __net_sts_eth_dev_t *p_eth = AW_CONTAINER_OF(p_dev, __net_sts_eth_dev_t, dev);
    char addr_str[16];
    char addr_str1[16];

    ip_addr_t                ip_addr;
    struct sockaddr_storage  to;
    struct sockaddr_storage  from;
    struct sockaddr_in      *si_to;
    struct sockaddr_in      *si_from;
    uint8_t                  ttl;

    aw_ping_t               ping;
    int                     ret;
    int                     i;
    __LOG_DEV_DEF(p_dev);

    if (1) {
        aw_net_sts_evt_t evt;
        evt.type                            = AW_NET_STS_EVT_TYPE_DNS_QUERY;
        evt.u.dns_query.host_name           = p_chk_addr;
        evt.u.dns_query.flag_force_query    = AW_FALSE;
        ret = aw_net_sts_evt_proc(p_dev->p_ctl, &evt);
        if (ret == AW_OK) {
            p_eth->dest_addr.s_addr = evt.u.dns_query.ip_addr.u_addr.ip4.addr;
        } else {
            return -AW_ENODATA;
        }
    }

    /* 如果 网卡 ip 为 0 则失败退出 */
    if (ip_addr_isany(&netif->ip_addr)) {
        return -AW_EFAULT;
    }

#if 1
#if LWIP_ROUTE_TABLE
    ip_addr.type            = IPADDR_TYPE_V4;
    ip_addr.u_addr.ip4.addr = p_eth->dest_addr.s_addr;
    netif_route_add(netif, &ip_addr);
#endif
#endif

    if (ip4_addr_isany(ip_2_ip4(&netif->ip_addr))) {
        return -1;
    }

    memset(&to, 0, sizeof(to));
    memset(&from, 0, sizeof(from));

    to.ss_family           = AF_INET;
    to.s2_len              = sizeof(struct sockaddr_in);
    si_to                  = (struct sockaddr_in *) &to;
    si_to->sin_addr.s_addr = p_eth->dest_addr.s_addr;

    from.ss_family           = AF_INET;
    from.s2_len              = sizeof(struct sockaddr_in);
    si_from                  = (struct sockaddr_in *) &from;
    si_from->sin_addr.s_addr = (in_addr_t)netif->ip_addr.u_addr.ip4.addr;
    p_eth->dest_addr         = si_to->sin_addr;
    ttl                      = __PING_SEND_TTL;

    for (i = 0; i < 4; i++) {
        ret = aw_ping(&ping,
                      &to,
                      &from,
                      &ttl,
                      __PING_DATA_SIZE,
                      __PING_TIMEOUT_MS);
        if (ret >= 0) {
            goto exit;
        }
    }

exit:
    __ASSERT_D(inet_ntoa_r(netif->ip_addr.u_addr.ip4,
                           addr_str,
                           sizeof(addr_str)), !=, NULL);
    inet_ntoa_r(p_eth->dest_addr, addr_str1, sizeof(addr_str1));
    __LOG_RES(ret >= 0, __MASK_NET_PING, "\"%s\" ping host \"%s\" [%s]",
              addr_str, p_chk_addr, addr_str1);
    return ret;
}

/******************************************************************************/
aw_local aw_err_t __evt_process (__net_sts_eth_dev_t  *p_eth)
{
    aw_net_sts_dev_t *p_dev = &p_eth->dev;
    aw_net_sts_evt_t  evt;
    __LOG_DEV_DEF(p_dev);

    if (BITMAP_IS_VALID(p_eth->evt_flags, __EVT_FLAG_STARTUP)) {
        __LOG_PFX(1, "task startup :%s", p_dev->attr.task_name);
    }

    if (BITMAP_IS_VALID(p_eth->evt_flags, __EVT_FLAG_SUSPEND)) {
        __LOG_PFX(1, "task suspend :%s", p_dev->attr.task_name);
        switch (p_eth->stat) {
            case __ETH_STAT_INET_OK:
            case __ETH_STAT_SNET_OK: {
                /*
                 * 这里为该状态时，即任务还未执行该状态，修改为 INIT，
                 * 防止下次 startup 时直接 OK
                 */
                p_eth->stat = __ETH_STAT_INIT;
            } break;

            case __ETH_STAT_CHK_INET_OK:
            case __ETH_STAT_CHK_SNET_OK: {
                /*
                 * link down ，启动后需重新检测状态
                 */
                evt.type    = AW_NET_STS_EVT_TYPE_LINK_DOWN;
                evt.u.link.p_dev   = p_dev;
                evt.u.link.p_netif = p_dev->p_netif;
                evt.u.link.prioty  = p_dev->attr.prioty;
                aw_net_sts_evt_proc(p_dev->p_ctl, &evt);

                p_eth->stat = __ETH_STAT_INIT;
            } break;
            default :break;
        }

        /* 清除 p_ctl 则不会发送 ctl 的事件 */
        p_dev->p_ctl = NULL;
    }

    if (BITMAP_IS_VALID(p_eth->evt_flags, __EVT_FLAG_REINIT)) {
        aw_bool_t               flag_need_init = AW_FALSE;
        aw_net_sts_dev_init_t  *p_init = &p_eth->evt_params.reinit.attr;

#if LWIP_ROUTE_TABLE
        /* 删除 ping 时添加的路由项 */
        if (p_dev->p_netif != NULL) {
            ip_addr_t                ip_addr;

            ip_addr.type            = IPADDR_TYPE_V4;
            ip_addr.u_addr.ip4.addr = p_eth->dest_addr.s_addr;
            netif_route_del(p_dev->p_netif, &ip_addr);
        }
#endif /* LWIP_ROUTE_TABLE */

        /* 检查具体修改的参数，部分参数需要进入 INIT 状态重新检测 ，有些则不用修改状态 */
        if (memcmp(p_eth->evt_params.reinit.attr.netif_name,
                   p_dev->attr.netif_name, sizeof(p_dev->attr.netif_name)) != 0) {
            flag_need_init = AW_TRUE;

        }

        if (p_eth->evt_params.reinit.attr.prioty !=
                p_dev->attr.prioty) {
            flag_need_init = AW_TRUE;
        }

        if (p_init->p_chk_inet_addr != NULL &&
                strncmp(p_eth->ip_buff[0],
                        p_init->p_chk_inet_addr,
                       __HOST_NAME_MAXLEN) != 0) {
            flag_need_init = AW_TRUE;
            aw_assert(strlen(p_init->p_chk_inet_addr) < __HOST_NAME_MAXLEN - 1);
            strncpy(p_eth->ip_buff[0], p_init->p_chk_inet_addr, __HOST_NAME_MAXLEN);
        }
        if (p_init->p_chk_inet_addr == NULL && p_eth->ip_buff[0][0] != 0) {
            flag_need_init = AW_TRUE;
            memset(p_eth->ip_buff[0], 0, __HOST_NAME_MAXLEN);
        }

        if (p_init->p_chk_snet_addr != NULL &&
                strncmp(p_eth->ip_buff[1],
                        p_init->p_chk_snet_addr,
                       __HOST_NAME_MAXLEN) != 0) {
            flag_need_init = AW_TRUE;
            aw_assert(strlen(p_init->p_chk_snet_addr) < __HOST_NAME_MAXLEN - 1);
            strncpy(p_eth->ip_buff[1], p_init->p_chk_snet_addr, __HOST_NAME_MAXLEN);
        }
        if (p_init->p_chk_snet_addr == NULL && p_eth->ip_buff[1][0] != 0) {
            flag_need_init = AW_TRUE;
            memset(p_eth->ip_buff[1], 0, __HOST_NAME_MAXLEN);
        }

        p_dev->attr                 = p_eth->evt_params.reinit.attr;
        p_dev->attr.chk_ms          = __CHK_MS_RECTIFY(p_dev->attr.chk_ms);
        p_dev->attr.p_chk_inet_addr = p_eth->ip_buff[0];
        p_dev->attr.p_chk_snet_addr = p_eth->ip_buff[1];

        if (flag_need_init) {
            switch (p_eth->stat) {
                case __ETH_STAT_INIT:
                case __ETH_STAT_UP:
                case __ETH_STAT_LINK_UP:
                case __ETH_STAT_INET_OK:
                case __ETH_STAT_SNET_OK:
                {
                    p_dev->p_netif = NULL;
                    p_eth->stat = __ETH_STAT_INIT;
                } break;

                case __ETH_STAT_CHK_INET_OK:
                case __ETH_STAT_CHK_SNET_OK: {
                    evt.type    = AW_NET_STS_EVT_TYPE_LINK_DOWN;
                    evt.u.link.p_dev   = p_dev;
                    evt.u.link.p_netif = p_dev->p_netif;
                    evt.u.link.prioty  = p_dev->attr.prioty;
                    __ASSERT_D(p_dev->p_netif, !=, NULL);
                    aw_net_sts_evt_proc(p_dev->p_ctl, &evt);

                    p_dev->p_netif = NULL;
                    p_eth->stat = __ETH_STAT_INIT;
                    __LOG_RES(AW_FALSE, __LOG_NET_STS,
                              "%s network", p_dev->attr.netif_name);
                } break;

                default: aw_assert(0); break;
            }
        }

    }

    /* 只会存在一个事件，直接清零 */
    p_eth->evt_flags[0]  = 0;
    p_dev->flag_busy     = AW_FALSE;

    return AW_OK;
}
/******************************************************************************/

aw_local void __task (aw_net_sts_dev_t *p_dev)
{
    __net_sts_eth_dev_t *p_eth = AW_CONTAINER_OF(p_dev, __net_sts_eth_dev_t, dev);
    int                  ret;
    aw_net_sts_evt_t     evt;
    aw_netif_t          *p_aw_netif = NULL;
    __LOG_DEV_DEF(p_dev);
    __ASSERT_D(p_eth, !=, NULL);

    if (p_dev->p_netif) {
        p_aw_netif = AW_CONTAINER_OF(p_dev->p_netif, aw_netif_t, netif);
        __ASSERT_D(p_aw_netif, !=, NULL);
    }

    switch (p_eth->stat) {
        case __ETH_STAT_INIT: {
            if (p_dev->p_netif == NULL) {
                p_aw_netif = aw_netif_dev_open(p_dev->attr.netif_name);
                aw_netif_dev_close(p_aw_netif);

                if (p_aw_netif == NULL) {
                    __LOG_PFX(1, "aw_netif device open failed. (name:\"%s\")",
                              p_dev->attr.netif_name);
                    p_eth->flag_delay = AW_TRUE;
                    break;
                } else {
                    p_dev->p_netif = &p_aw_netif->netif;
                }
            }

            {
                aw_bool_t            up;
                ret = aw_netif_is_up(p_aw_netif, &up);
                __ASSERT_D(ret, ==, AW_OK);
                if (up) {
                    p_eth->stat = __ETH_STAT_UP;
                } else {
                    p_eth->flag_delay = AW_TRUE;
                    __LOG_PFX(1, "aw_netif device not up. (name:\"%s\")",
                              p_dev->attr.netif_name);
                }
            }
        } break;

        case __ETH_STAT_UP: {
            aw_bool_t            up;
            ret = aw_netif_is_link_up(p_aw_netif, &up);
            __ASSERT_D(ret, ==, AW_OK);
            if (up) {
                p_eth->stat = __ETH_STAT_LINK_UP;
            } else {
                p_eth->flag_delay = AW_TRUE;
                __LOG_PFX(1, "aw_netif device not link. (\"%s\")",
                          p_dev->attr.netif_name);
            }
        } break;

        case __ETH_STAT_LINK_UP: {

            /* 优先检测公网,成功则退出  */
            if (strlen(p_dev->attr.p_chk_inet_addr)) {
                ret = __ping_check(p_dev,
                                   p_dev->p_netif,
                                   p_dev->attr.p_chk_inet_addr);
                if (ret >= 0) {
                    p_eth->stat = __ETH_STAT_INET_OK;
                    break;
                }
            }

            if (strlen(p_dev->attr.p_chk_snet_addr)) {
                ret = __ping_check(p_dev,
                                   p_dev->p_netif,
                                   p_dev->attr.p_chk_snet_addr);
                if (ret >= 0) {
                    p_eth->stat = __ETH_STAT_SNET_OK;
                    break;
                }
            }

            /* ping 都失败 */
            p_eth->stat = __ETH_STAT_INIT;
            p_eth->flag_delay = AW_TRUE;
        } break;

        case __ETH_STAT_INET_OK: {
            if (p_dev->p_ctl) {
                evt.type    = AW_NET_STS_EVT_TYPE_LINK_INET;
                evt.u.link.p_dev   = p_dev;
                evt.u.link.p_netif = p_dev->p_netif;
                evt.u.link.prioty  = p_dev->attr.prioty;
                __ASSERT_D(p_dev->p_netif, !=, NULL);
                aw_net_sts_evt_proc(p_dev->p_ctl, &evt);
            }

            p_eth->stat = __ETH_STAT_CHK_INET_OK;
            __LOG_PFX(__LOG_NET_STS,
                      "The device is connected to Internet. (if:%s)",
                      p_dev->attr.netif_name);
            p_eth->flag_delay = AW_TRUE;
        } break;

        case __ETH_STAT_CHK_INET_OK: {
            ret = __ping_check(p_dev,
                               p_dev->p_netif,
                               p_dev->attr.p_chk_inet_addr);
            if (ret < 0) {
                evt.type    = AW_NET_STS_EVT_TYPE_LINK_DOWN;
                evt.u.link.p_dev   = p_dev;
                evt.u.link.p_netif = p_dev->p_netif;
                evt.u.link.prioty  = p_dev->attr.prioty;
                __ASSERT_D(p_dev->p_netif, !=, NULL);
                aw_net_sts_evt_proc(p_dev->p_ctl, &evt);

                p_dev->p_netif = NULL;
                p_eth->stat = __ETH_STAT_INIT;

                __LOG_PFX(__LOG_NET_STS,
                          "The device is disconnected from the Internet. (if:%s)",
                          p_dev->attr.netif_name);
            }
            p_eth->flag_delay = AW_TRUE;
        } break;

        case __ETH_STAT_SNET_OK: {
            evt.type    = AW_NET_STS_EVT_TYPE_LINK_SNET;
            evt.u.link.p_dev   = p_dev;
            evt.u.link.p_netif = p_dev->p_netif;
            evt.u.link.prioty  = p_dev->attr.prioty;
            __ASSERT_D(p_dev->p_netif, !=, NULL);
            aw_net_sts_evt_proc(p_dev->p_ctl, &evt);

            p_eth->stat = __ETH_STAT_CHK_SNET_OK;

            __LOG_PFX(__LOG_NET_STS,
                      "The device is connected to private network. (if:%s)",
                      p_dev->attr.netif_name);

            p_eth->flag_delay = AW_TRUE;
        } break;

        case __ETH_STAT_CHK_SNET_OK: {
            ret = __ping_check(p_dev,
                               p_dev->p_netif,
                               p_dev->attr.p_chk_snet_addr);
            if (ret < 0) {
                evt.type    = AW_NET_STS_EVT_TYPE_LINK_DOWN;
                evt.u.link.p_dev   = p_dev;
                evt.u.link.p_netif = p_dev->p_netif;
                evt.u.link.prioty  = p_dev->attr.prioty;
                __ASSERT_D(p_dev->p_netif, !=, NULL);
                aw_net_sts_evt_proc(p_dev->p_ctl, &evt);

                p_dev->p_netif = NULL;
                p_eth->stat = __ETH_STAT_INIT;
                __LOG_PFX(__LOG_NET_STS,
                          "The device is disconnected from private network. (if:%s)",
                          p_dev->attr.netif_name);
            }
            p_eth->flag_delay = AW_TRUE;
        } break;

        default: aw_assert(0); break;
    }

    return;
}

aw_local void __task_entry (void *p_arg)
{
    aw_net_sts_dev_t    *p_dev = p_arg;
    __net_sts_eth_dev_t *p_eth = AW_CONTAINER_OF(p_dev, __net_sts_eth_dev_t, dev);
    aw_tick_t            tick;

    for (;;) {
        aw_mdelay(10);
        AW_SEMB_TAKE(p_dev->task_sem, AW_SEM_WAIT_FOREVER);
        __task(p_dev);
        AW_SEMB_GIVE(p_dev->task_sem);
        /**
         * 在任务外处理延时检测，在 suspend 时便能快速获取锁
         */
        if (p_eth->flag_delay) {
            aw_assert(p_dev->attr.chk_ms >= __CHK_MS_RECTIFY(p_dev->attr.chk_ms));
            __TIMEOUT_MSEC (tick, p_dev->attr.chk_ms) {
                aw_tick_t cur_tick = aw_sys_tick_get();
                uint32_t  ms;

                /* 使用信号值，使延时时能被唤醒去处理 evt */
                AW_SEMB_TAKE(p_eth->wakeup, tick - cur_tick);
                if (p_eth->evt_flags[0]) {
                    ms = p_dev->attr.chk_ms;
                    __evt_process(p_eth);

                    /* 如果延时时间变小则立即跳出延时循环 */
                    if (ms > p_dev->attr.chk_ms) {
                        break;
                    }
                }
            }
            p_eth->flag_delay = AW_FALSE;
        }

        if (p_eth->evt_flags[0]) {
            __evt_process(p_eth);
        }
    }
}

aw_local aw_err_t __startup (aw_net_sts_dev_t *p_dev)
{
    __net_sts_eth_dev_t  *p_eth = AW_CONTAINER_OF(p_dev, __net_sts_eth_dev_t, dev);

    /* 只能存在一个 evt */
    if (p_eth->evt_flags[0]) {
        return -AW_EBUSY;
    }

    BITMAP_SET(p_eth->evt_flags, __EVT_FLAG_STARTUP);
    p_dev->flag_busy = AW_TRUE;
    AW_SEMB_GIVE(p_eth->wakeup);
    return AW_OK;
}

aw_local aw_err_t __suspend (aw_net_sts_dev_t *p_dev)
{
    __net_sts_eth_dev_t  *p_eth = AW_CONTAINER_OF(p_dev, __net_sts_eth_dev_t, dev);

    if (p_eth->evt_flags[0]) {
        return -AW_EBUSY;
    }

    BITMAP_SET(p_eth->evt_flags, __EVT_FLAG_SUSPEND);
    p_dev->flag_busy = AW_TRUE;
    AW_SEMB_GIVE(p_eth->wakeup);
    return AW_OK;
}

aw_local aw_err_t __reinit (aw_net_sts_dev_t        *p_dev,
                            aw_net_sts_dev_init_t   *p_init)
{
    __net_sts_eth_dev_t  *p_eth = AW_CONTAINER_OF(p_dev, __net_sts_eth_dev_t, dev);

    if (p_eth->evt_flags[0]) {
        return -AW_EBUSY;
    }

    p_eth->evt_params.reinit.attr = *p_init;
    BITMAP_SET(p_eth->evt_flags, __EVT_FLAG_REINIT);
    p_dev->flag_busy = AW_TRUE;
    AW_SEMB_GIVE(p_eth->wakeup);
    return AW_OK;
}

aw_net_sts_dev_t * aw_net_sts_eth_dev_create (aw_net_sts_dev_init_t *p_init)
{
    __net_sts_eth_dev_t  *p_eth;
    aw_net_sts_dev_t    *p_dev = NULL;
    __LOG_DEV_DEF(p_dev);

    /* eth 必须要有一个检测地址  */
    aw_assert(p_init->p_chk_inet_addr || p_init->p_chk_snet_addr);

    p_eth = malloc(sizeof(__net_sts_eth_dev_t));
    aw_assert(p_eth);
    memset(p_eth, 0, sizeof(*p_eth));

    p_eth->stat         = __ETH_STAT_INIT;
    p_dev               = &p_eth->dev;
    p_dev->attr         = *p_init;
    p_dev->attr.chk_ms  = __CHK_MS_RECTIFY(p_dev->attr.chk_ms);
    p_dev->pfn_startup  = __startup;
    p_dev->pfn_suspend  = __suspend;
    p_dev->pfn_reinit   = __reinit;

    p_dev->attr.p_chk_inet_addr = p_eth->ip_buff[0];
    p_dev->attr.p_chk_snet_addr = p_eth->ip_buff[1];

    if (p_init->p_chk_inet_addr && strlen(p_init->p_chk_inet_addr)) {
        aw_assert(strlen(p_init->p_chk_inet_addr) < __HOST_NAME_MAXLEN - 1);
        strncpy(p_eth->ip_buff[0], p_init->p_chk_inet_addr, __HOST_NAME_MAXLEN);
    }
    if (p_init->p_chk_snet_addr && strlen(p_init->p_chk_snet_addr)) {
        aw_assert(strlen(p_init->p_chk_snet_addr) < __HOST_NAME_MAXLEN - 1);
        strncpy(p_eth->ip_buff[1], p_init->p_chk_snet_addr, __HOST_NAME_MAXLEN);
    }

    if (p_dev->attr.task_name[0] == 0) {
        aw_snprintf(p_dev->attr.task_name,
                    sizeof(p_dev->attr.task_name),
                    "net_sts:%s",
                    p_dev->attr.netif_name);
    }
    /* 使用 semb 来停止任务，不直接使用 AW_TASK_TERMINATE */
    AW_SEMB_INIT(p_dev->task_sem, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);
    AW_SEMB_INIT(p_eth->wakeup, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);

    /* 直接给信号量，suspend 后也会在后台一直检测  */
    AW_SEMB_GIVE(p_dev->task_sem);

    AW_TASK_INIT(p_dev->task,
                 p_dev->attr.task_name,
                 10 + p_dev->attr.prioty,
                 AW_NET_STS_TASK_STACK_SIZE,
                 __task_entry,
                 (void*)p_dev);
    AW_TASK_STARTUP(p_dev->task);


    __LOG_PFX(1, "Create a network check device.\n"
            "\t(attr: name:%s type:%d priority:%d delay:%d)",
              p_dev->attr.netif_name,
              p_dev->attr.type,
              p_dev->attr.prioty,
              p_dev->attr.chk_ms);
    return p_dev;
}


/* end of file */

