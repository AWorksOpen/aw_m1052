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
 * \brief wifi 模块网络状态检查
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
#include "net_sts_internal.h"

#include "aw_netif.h"
#include "aw_ping.h"

#define __REUSE_ETH_CREATE 1

#if !__REUSE_ETH_CREATE
/******************************************************************************/
#define __LOG_DEV_DEF(p_dev)  \
    aw_net_sts_log_t _pfn_log_out_ = \
            (p_dev ? ((aw_net_sts_dev_t *)p_dev)->attr.pfn_log_out : NULL )


#define __LOG_NET_STS           1
#define __MASK_NET_PING         1

/******************************************************************************/
#define __PING_SEND_TTL     64  /* 96 */
#define __PING_TIMEOUT_MS   3000
#define __PING_DATA_SIZE    8

/******************************************************************************/
typedef enum {
    __WIFI_STAT_INIT = 0,
    __WIFI_STAT_UP,
    __WIFI_STAT_LINK_UP,
    __WIFI_STAT_OK,
    __WIFI_STAT_CHK_OK,
} __wifi_stat_t;

typedef struct {
    aw_net_sts_dev_t         dev;
    uint8_t                  rst_num;
    int                      dev_id;
    __wifi_stat_t            stat;

    /*
     *  dns 解析出来的主机名和ip地址，保存下来，防止重复做 dns
     */
    char                    *host;
    struct in_addr           dest_addr;
} __net_sts_wifi_dev_t;

/******************************************************************************/
aw_local aw_err_t gwifiostbyname4 (const char      *host,
                                  struct in_addr  *addr,
                                  char           **official_name)
{
    struct hostent *p_host = NULL;
    if (NULL == p_host || (NULL == p_host->h_addr_list[0])) {
        return -AW_EADDRNOTAVAIL;
    }
    if (AF_INET != p_host->h_addrtype) {
        return -AW_EAFNOSUPPORT;
    }
    memcpy(&addr->s_addr, p_host->h_addr_list[0], sizeof(addr->s_addr));

    if (official_name) {
        *official_name = p_host->h_name;
    }
    return AW_OK;
}


/* 返回 >= 0 表示 reply 时间长度， < 0 响应失败 */
static int __ping_check (aw_net_sts_dev_t *p_dev, struct netif *netif)
{
    __net_sts_wifi_dev_t *p_wifi = AW_CONTAINER_OF(p_dev, __net_sts_wifi_dev_t, dev);
    char addr_str[16];
    char addr_str1[16];
    __LOG_DEV_DEF(p_dev);

    struct sockaddr_storage  to;
    struct sockaddr_storage  from;
    struct sockaddr_in      *si_to;
    struct sockaddr_in      *si_from;
    uint8_t                  ttl;

    aw_ping_t               ping;
    int                     ret;
    int                     i;

#if 0
    if (p_wifi->dest_addr.s_addr == 0) {
        ret = gwifiostbyname4(
                    (const char *) __g_net_manager_data.arg_cfg.arg.cfg.dest_ip,
                     &dest_addr,
                     &host);
        __CHECK_D(ret, ==, AW_OK, return ret);
    }
#else
    if (p_wifi->host == NULL) {
        ret = gwifiostbyname4((const char *) p_dev->attr.p_chk_ip,
                             &p_wifi->dest_addr,
                             &p_wifi->host);
        __CHECK_D(ret, ==, AW_OK, return ret);
    }
#endif

#if 1
#ifdef LWIP_ROUTE_TABLE
    extern struct netif *netif_route_find (ip_addr_t *p_ipaddr);
    extern void netif_route_add (struct netif *netif, ip_addr_t *p_ipaddr);
    extern int netif_route_change (ip_addr_t *p_old_ipaddr,
                                   ip_addr_t *p_new_ipaddr,
                                   struct netif *new_netif);

    if (netif_route_find((ip_addr_t *)&p_wifi->dest_addr) != NULL) {
        netif_route_change((ip_addr_t *) &p_wifi->dest_addr,
                           (ip_addr_t *) &p_wifi->dest_addr,
                           netif);
    } else {
        netif_route_add(netif, (ip_addr_t *)&p_wifi->dest_addr);
    }
#endif
#endif

    memset(&to, 0, sizeof(to));
    memset(&from, 0, sizeof(from));

    to.ss_family           = AF_INET;
    to.s2_len              = sizeof(struct sockaddr_in);
    si_to                  = (struct sockaddr_in *) &to;
    si_to->sin_addr.s_addr = p_wifi->dest_addr.s_addr;

    from.ss_family           = AF_INET;
    from.s2_len              = sizeof(struct sockaddr_in);
    si_from                  = (struct sockaddr_in *) &from;
    si_from->sin_addr.s_addr = (in_addr_t)netif->ip_addr.u_addr.ip4.addr;
    p_wifi->dest_addr         = si_to->sin_addr;
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
    __LOG_RES(ret >= 0, __MASK_NET_PING, "\"%s\" ping host \"%s\" [%s]",
              inet_ntoa_r(netif->ip_addr.u_addr.ip4, addr_str, sizeof(addr_str)),
              p_wifi->host,
              inet_ntoa_r(p_wifi->dest_addr, addr_str1, sizeof(addr_str1)));
    return ret;
}


static int __netif_check (aw_net_sts_dev_t  *p_dev,
                          struct netif      *target_netif)
{
    struct netif    *netif = NULL;

    /* 当找到第一个可用时退出，否则会检查完所有的 4G 网卡 */
    for (netif = netif_list; netif != NULL; netif = netif->next) {
        if (target_netif != netif || netif_is_link_up(netif) == AW_FALSE) {
            continue;
        }

        if (ip4_addr_isany(ip_2_ip4(&netif->ip_addr))) {
            break;
        }
        if (__ping_check(p_dev, netif) >= 0) {
            return AW_OK;
        } else {
            break;
        }
    }

    return -AW_EHOSTUNREACH;
}

/******************************************************************************/

aw_local void __task (aw_net_sts_dev_t *p_dev)
{
    __net_sts_wifi_dev_t *p_wifi = AW_CONTAINER_OF(p_dev, __net_sts_wifi_dev_t, dev);
    int                   ret;
    aw_net_sts_evt_t      evt;
    aw_netif_t           *p_aw_netif = NULL;
    __LOG_DEV_DEF(p_dev);
    __ASSERT_D(p_wifi, !=, NULL);

    switch (p_wifi->stat) {
        case __WIFI_STAT_INIT: {
            if (p_dev->p_netif == NULL) {
                p_aw_netif = aw_netif_dev_open(p_dev->attr.netif_name);
                aw_netif_dev_close(p_aw_netif);

                if (p_aw_netif == NULL) {
                    __LOG_PFX(1, "[ERROR] aw_netif_dev_open(\"%s\")",
                              p_dev->attr.netif_name);
                    aw_mdelay(p_dev->attr.chk_ms);
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
                    p_wifi->stat = __WIFI_STAT_UP;
                } else {
                    __LOG_PFX(1, "[ERROR] aw_netif_is_up(\"%s\")",
                              p_dev->attr.netif_name);
                    aw_mdelay(p_dev->attr.chk_ms);
                }
            }
        } break;

        case __WIFI_STAT_UP: {
            aw_bool_t            up;
            ret = aw_netif_is_link_up(p_aw_netif, &up);
            __ASSERT_D(ret, ==, AW_OK);
            if (up) {
                p_wifi->stat = __WIFI_STAT_LINK_UP;
            } else {
                aw_mdelay(p_dev->attr.chk_ms);
            }
        } break;

        case __WIFI_STAT_LINK_UP: {
            ret = __netif_check(p_dev, p_dev->p_netif);
            if (ret == AW_OK) {
                p_wifi->stat = __WIFI_STAT_OK;
            } else {
                p_wifi->stat = __WIFI_STAT_INIT;
                aw_mdelay(p_dev->attr.chk_ms);
            }
        } break;

        case __WIFI_STAT_OK: {
            evt.type    = AW_NET_STS_EVT_TYPE_LINK_UP;
            evt.p_dev   = p_dev;
            evt.p_netif = p_dev->p_netif;
            evt.prioty  = p_dev->attr.prioty;
            __ASSERT_D(p_dev->p_netif, !=, NULL);
            aw_net_sts_evt_proc(p_dev->p_ctl, &evt);

            p_wifi->stat = __WIFI_STAT_CHK_OK;
            __LOG_RES(AW_TRUE, __LOG_NET_STS,
                      "%s network", p_dev->attr.netif_name);
            aw_mdelay(p_dev->attr.chk_ms);
        } break;

        case __WIFI_STAT_CHK_OK: {
            ret = __netif_check(p_dev, p_dev->p_netif);
            if (ret != AW_OK) {
                evt.type    = AW_NET_STS_EVT_TYPE_LINK_DOWN;
                evt.p_dev   = p_dev;
                evt.p_netif = p_dev->p_netif;
                evt.prioty  = p_dev->attr.prioty;
                __ASSERT_D(p_dev->p_netif, !=, NULL);
                aw_net_sts_evt_proc(p_dev->p_ctl, &evt);

                p_dev->p_netif = NULL;
                p_wifi->stat = __WIFI_STAT_INIT;
                __LOG_RES(AW_FALSE, __LOG_NET_STS,
                          "%s network", p_dev->attr.netif_name);
            }
            aw_mdelay(p_dev->attr.chk_ms);

        } break;

        default: aw_assert(0); break;
    }

    return;
}

aw_local void __task_entry (void *p_arg)
{
    aw_net_sts_dev_t     *p_dev = p_arg;
    __net_sts_wifi_dev_t *p_wifi = AW_CONTAINER_OF(p_dev, __net_sts_wifi_dev_t, dev);
    __wifi_stat_t         old_stat;
    __wifi_stat_t         stat;
    for (;;) {
        AW_MUTEX_LOCK(p_dev->task_mutex, AW_SEM_WAIT_FOREVER);
        old_stat = p_wifi->stat;
        __task(p_dev);
        stat = p_wifi->stat;
        AW_MUTEX_UNLOCK(p_dev->task_mutex);
        /**
         * 在任务外处理延时检测，在 suspend 时便能快速获取锁
         */
        switch (stat) {
            case __WIFI_STAT_INIT:
            case __WIFI_STAT_CHK_OK:
            case __WIFI_STAT_LINK_UP:
            {
                aw_mdelay(p_dev->attr.chk_ms);
            } break;

            /* 前后状态一样的，进行延时 */
            case __WIFI_STAT_UP:
            {
                if (stat == old_stat) {
                    aw_mdelay(p_dev->attr.chk_ms);
                }
            } break;
            default:break;
        }
    }
}

aw_local void __startup (aw_net_sts_dev_t *p_dev)
{
    AW_MUTEX_UNLOCK(p_dev->task_mutex);
}

aw_local void __suspend (aw_net_sts_dev_t *p_dev)
{
    __net_sts_wifi_dev_t *p_wifi = AW_CONTAINER_OF(p_dev, __net_sts_wifi_dev_t, dev);
    aw_net_sts_evt_t     evt;
    AW_MUTEX_LOCK(p_dev->task_mutex, AW_SEM_WAIT_FOREVER);
    switch (p_wifi->stat) {
        case __WIFI_STAT_OK: {
            /*
             * 状态，防止下次 startup 时直接 OK
             */
            p_wifi->stat = __WIFI_STAT_INIT;
        } break;

        case __WIFI_STAT_CHK_OK: {
            /*
             * 状态不用变，只先 link down ，等重新重新检测该状态
             */
            evt.type    = AW_NET_STS_EVT_TYPE_LINK_DOWN;
            evt.p_dev   = p_dev;
            evt.p_netif = p_dev->p_netif;
            evt.prioty  = p_dev->attr.prioty;
            aw_net_sts_evt_proc(p_dev->p_ctl, &evt);
        } break;
        default :break;
    }
}

aw_local aw_err_t __reinit (aw_net_sts_dev_t *p_dev,
                            aw_net_sts_dev_init_t *p_init)
{
    __net_sts_wifi_dev_t *p_wifi = AW_CONTAINER_OF(p_dev, __net_sts_wifi_dev_t, dev);
    aw_net_sts_evt_t     evt;
    __LOG_DEV_DEF(p_dev);
    __suspend(p_dev);
    p_dev->attr = *p_init;

    switch (p_wifi->stat) {
        case __WIFI_STAT_INIT:
        case __WIFI_STAT_UP:
        case __WIFI_STAT_LINK_UP:
        case __WIFI_STAT_OK:
        {
            p_dev->p_netif = NULL;
            p_wifi->stat = __WIFI_STAT_INIT;
        } break;

        case __WIFI_STAT_CHK_OK: {
            {
                evt.type    = AW_NET_STS_EVT_TYPE_LINK_DOWN;
                evt.p_dev   = p_dev;
                evt.p_netif = p_dev->p_netif;
                evt.prioty  = p_dev->attr.prioty;
                __ASSERT_D(p_dev->p_netif, !=, NULL);
                aw_net_sts_evt_proc(p_dev->p_ctl, &evt);

                p_dev->p_netif = NULL;
                p_wifi->stat = __WIFI_STAT_INIT;
                __LOG_RES(AW_FALSE, __LOG_NET_STS,
                          "%s network", p_dev->attr.netif_name);
            }
            p_dev->p_netif = NULL;
            p_wifi->stat = __WIFI_STAT_INIT;
        } break;

        default: aw_assert(0); break;
    }
    __startup(p_dev); /* 重新启动 */    

    return AW_OK;
}

aw_net_sts_dev_t * aw_net_sts_wifi_dev_create (aw_net_sts_dev_init_t *p_init)
{
    __net_sts_wifi_dev_t    *p_wifi;
    aw_net_sts_dev_t        *p_dev = NULL;

    p_wifi = malloc(sizeof(__net_sts_wifi_dev_t));
    aw_assert(p_wifi);
    memset(p_wifi, 0, sizeof(*p_wifi));

    p_wifi->stat        = __WIFI_STAT_INIT;
    p_dev               = &p_wifi->dev;
    p_dev->attr         = *p_init;
    p_dev->pfn_startup  = __startup;
    p_dev->pfn_suspend  = __suspend;
    p_dev->pfn_reinit   = __reinit;

    if (p_dev->attr.task_name[0] == 0) {
        aw_snprintf(p_dev->attr.task_name,
                    sizeof(p_dev->attr.task_name),
                    "net_sts:%s",
                    p_dev->attr.netif_name);
    }
    /* 使用 mutex 锁来停止任务，不直接使用 AW_TASK_TERMINATE */
    AW_MUTEX_INIT(p_dev->task_mutex, AW_SEM_Q_PRIORITY);
    AW_TASK_INIT(p_dev->task,
                 p_dev->attr.task_name,
                 10 + p_dev->attr.prioty,
                 AW_NET_STS_TASK_STACK_SIZE,
                 __task_entry,
                 (void*)p_dev);
    AW_TASK_STARTUP(p_dev->task);


    return p_dev;
}

#else
aw_net_sts_dev_t * aw_net_sts_wifi_dev_create (aw_net_sts_dev_init_t *p_init)
{
    return aw_net_sts_eth_dev_create(p_init);
}

#endif /* __REUSE_ETH_CREATE */

/* end of file */

