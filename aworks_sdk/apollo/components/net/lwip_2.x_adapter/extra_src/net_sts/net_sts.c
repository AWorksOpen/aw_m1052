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
 * \brief
 *
 * \internal
 * \par modification history:
 * - 1.00 20-03-16  vih, first implementation
 * \endinternal
 */

#include "aworks.h"
#include <stdlib.h>
#include <string.h>

#include "aw_msgq.h"
#include "aw_assert.h"
#include "aw_delay.h"

#include "lwip/ip.h"
#include "lwip/dns.h"
#include "lwip/ext/net_sts.h"
#include "lwip/netdb.h"
#include "net_sts_internal.h"

#include "lwip/netifapi.h"

/******************************************************************************/
#define PFX "NET_STS"

#if 1
#define __LOG_DEV_DEF(p_ctl)  \
    aw_net_sts_log_t _pfn_log_out_ = \
            (p_ctl ? ((aw_net_sts_ctl_t *)p_ctl)->attr.pfn_log_out : NULL ); \
            (void)_pfn_log_out_
#else
#define __LOG_DEV_DEF(p_ctl)   aw_net_sts_log_t _pfn_log_out_ = NULL
#endif

#define __MUTEX_TIMEOUT 45000

/* Check to see if it has timed out. */
#define __IS_TIMEOUT_MS(start_tick, ms) \
    (aw_ticks_to_ms(aw_sys_tick_get() - start_tick) >= ms )

/* Caculate the MS interval time of current tick and given tick. */
#define __INTERVAL_MS_GET(tick)  aw_ticks_to_ms(aw_sys_tick_get() - (tick))

/******************************************************************************/
/*
 * Defines the macro to determine the event type.
 */

#define __EVT_TYPE_IS_ANY_LINK_OK(p_evt) ((p_evt) != NULL) &&\
        ((p_evt)->type == AW_NET_STS_EVT_TYPE_LINK_INET ||\
        (p_evt)->type == AW_NET_STS_EVT_TYPE_LINK_SNET ||\
        (p_evt)->type == AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET ||\
        (p_evt)->type == AW_NET_STS_EVT_TYPE_FORCE_LINK_SNET_SET)

#define __EVT_TYPE_IS_LINK_OK(p_evt) (((p_evt) != NULL) &&\
        ((p_evt)->type == AW_NET_STS_EVT_TYPE_LINK_INET ||\
        (p_evt)->type == AW_NET_STS_EVT_TYPE_LINK_SNET))

#define __EVT_TYPE_IS_FORCE_LINK_OK(p_evt) (((p_evt) != NULL) &&\
        ((p_evt)->type == AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET ||\
        (p_evt)->type == AW_NET_STS_EVT_TYPE_FORCE_LINK_SNET_SET))


#define __EVT_TYPE_IS_ANY_LINK_DOWN(p_evt) (((p_evt) != NULL) &&\
        ((p_evt)->type == AW_NET_STS_EVT_TYPE_LINK_DOWN ||\
        (p_evt)->type == AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_CLR || \
        (p_evt)->type == AW_NET_STS_EVT_TYPE_FORCE_LINK_SNET_CLR))

#define __EVT_TYPE_IS_FORCE_LINK_DOWN(p_evt) (((p_evt) != NULL) &&\
        ((p_evt)->type == AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_CLR || \
        (p_evt)->type == AW_NET_STS_EVT_TYPE_FORCE_LINK_SNET_CLR))

#define __EVT_TYPE_IS_LINK_DOWN(p_evt) (((p_evt) != NULL) &&\
        ((p_evt)->type == AW_NET_STS_EVT_TYPE_LINK_DOWN))

/******************************************************************************/
#define __LOG_MUTEX 0
#define __LOG_CHK   0
/******************************************************************************/
#define __FORCE_LINK_SET_CLR_OFFS   2
/******************************************************************************/
#define __DNS_CACHE_MAX              6
#define __DNS_CACHE_TIMEOUT_PRE     (1700000)  /* Cache is about to expire. MS */
#define __DNS_CACHE_TIMEOUT         (1800000)  /* Cache expiration data. MS */

//#define __DNS_CACHE_TIMEOUT_PRE     (165000)  /* Cache is about to expire. MS */
//#define __DNS_CACHE_TIMEOUT         (180000)  /* Cache expiration data. MS */
/******************************************************************************/
typedef struct __dns_cache __dns_cache_t;
/******************************************************************************/
aw_local aw_err_t __dns_cache_add (const char *p_name, ip_addr_t *p_addr);
aw_local __dns_cache_t *__dns_cache_find (const char *p_name);
aw_local aw_err_t __dns_cache_update (__dns_cache_t *p_cache,
                                      const char    *p_name,
                                      ip_addr_t     *p_addr);
aw_local aw_bool_t __dns_cache_is_expired_pre (__dns_cache_t *p_cache);
aw_local aw_err_t __dns_record_foreach (struct netif **pp_netif,
                                        ip_addr_t    *p_dns);

aw_local aw_err_t __dns_record_get (struct netif *p_netif, ip_addr_t *p_dns);

aw_local aw_err_t __link_evt_proc (aw_net_sts_ctl_t *p_ctl,
                                   aw_net_sts_evt_t *p_evt);

/******************************************************************************/
#define __DWORK_TASK_STACK_SIZE  4096
#define __DWORK_MSGQ_NUM         6
#define __DWORK_MSGQ_SIZE        sizeof(__dwork_evt_t)
#define __HOST_NAME_MAXLEN       128

typedef enum {
    __DWORK_EVT_TYPE_DNS_QUERY = 0,
    __DWORK_EVT_WAIT_FORCE_NETIF,
} __dwork_evt_type_t;

typedef struct {
    __dwork_evt_type_t type;

    union {
        struct {
            aw_net_sts_ctl_t    *p_ctl;
            char                 host[__HOST_NAME_MAXLEN];
        } dns_query;

        struct {
            aw_net_sts_ctl_t    *p_ctl;
        } wati_force_netif;
    } u;
} __dwork_evt_t;

typedef struct {
    AW_TASK_DECL(            task, __DWORK_TASK_STACK_SIZE);
    AW_MSGQ_DECL(msgq, __DWORK_MSGQ_NUM, __DWORK_MSGQ_SIZE);
} __dwork_ctx_t;

aw_local __dwork_ctx_t __gp_dwork_ctx;


aw_local aw_err_t __dns_query (ip_addr_t     *p_dns_server,
                               const char    *host,
                               ip_addr_t     *p_out)
{
    aw_err_t        ret;
    struct hostent  result;
    struct hostent *p_host = &result;
    struct hostent *p_result = NULL;
    char            buf[256];  /* namelen + 36 */
    int             err;
    ip_addr_t       addr;
    ip_addr_t       dns_tmp;
    aw_assert(host && p_out);

    /* Use the default dns server if not specified. */
    if (p_dns_server) {
        /* Save the default dns and restore it after the query is completed. */
        aw_assert(dns_getserver(0));
        dns_tmp = *dns_getserver(0);

        dns_setserver(0, p_dns_server);
    }

    ret = gethostbyname_r(host, &result, buf, sizeof(buf), &p_result, &err);
    if (ret != 0) {
        ret = -AW_EADDRNOTAVAIL;
        goto exit;
    }
    aw_assert(p_host->h_addr_list[0] != NULL);

    if (AF_INET == p_host->h_addrtype) {
        ip_addr_set_ip4_u32_val(addr, *((uint32_t *)p_host->h_addr_list[0]));
    } else if (AF_INET6 == p_host->h_addrtype) {
        IP_SET_TYPE(&addr, IPADDR_TYPE_V6);
        memcpy(&(addr.u_addr.ip6.addr),
               p_host->h_addr_list[0],
               sizeof(addr.u_addr.ip6.addr));
        ip6_addr_set_zone(ip_2_ip6(&addr), IP6_NO_ZONE);
    } else {
        aw_assert(0);
    }

    *p_out = addr;
    ret = AW_OK;

exit:
    if (p_dns_server) {
        dns_setserver(0, &dns_tmp);
    }
    return ret;
}

/* Delegate the mandatory NETIF event to a low-level task. */
aw_local aw_err_t __force_netif_evt_proc (aw_net_sts_ctl_t *p_ctl)
{
    aw_err_t        ret;
    __dwork_evt_t   evt;

    evt.type                          = __DWORK_EVT_WAIT_FORCE_NETIF;
    evt.u.wati_force_netif.p_ctl      = p_ctl;

    /* There may be a mailbox is full of mistakes. */
    ret = AW_MSGQ_SEND(__gp_dwork_ctx.msgq,
                   &evt,
                   sizeof(evt),
                   AW_MSGQ_NO_WAIT,
                   AW_MSGQ_PRI_NORMAL);

    return ret;
}


aw_local void __subnet_prefix_chk (aw_net_sts_ctl_t        *p_ctl,
                                   void                    *p_arg)
{
    int                  i       = 0;
    int                  next;
    aw_net_sts_dev_t    *p_dev;
    aw_net_sts_dev_t    *p_dev_tmp;
    aw_net_sts_evt_t     evt;
    aw_err_t             ret;

    next  = 0;
    while (next < __NET_STS_DEV_ARRAY_MAX - 1) {

        p_dev = NULL;
        /* To find the next effective device. */
        for (i = next; i < __NET_STS_DEV_ARRAY_MAX; i++) {
            if (p_ctl->p_dev_arr[i] == NULL) {
                continue;
            }
            if (p_ctl->p_dev_arr[i]->p_netif == NULL) {
                continue;
            }
            if (!netif_is_link_up(p_ctl->p_dev_arr[i]->p_netif)) {
                continue;
            }
            if (ip_addr_isany_val(p_ctl->p_dev_arr[i]->p_netif->ip_addr)) {
                continue;
            }

            p_dev = p_ctl->p_dev_arr[i];
            next  = i + 1;
            break;
        }
        /* No effective device. */
        if (p_dev == NULL) {
            break;
        }

        for (i = next; i < __NET_STS_DEV_ARRAY_MAX; i++) {
            aw_assert(p_dev->p_netif);

            /* If the neif is suddenly link down, then break. */
            if (!netif_is_link_up(p_dev->p_netif)) {
                break;
            }

            if (p_ctl->p_dev_arr[i] == NULL) {
                continue;
            }
            if (p_ctl->p_dev_arr[i]->p_netif == NULL) {
                continue;
            }
            if (!netif_is_link_up(p_ctl->p_dev_arr[i]->p_netif)) {
                continue;
            }

            p_dev_tmp = p_ctl->p_dev_arr[i];

            /* The same subnet prefix reports event. */
            if (ip_addr_netcmp(&p_dev->p_netif->ip_addr,
                               &p_dev_tmp->p_netif->ip_addr,
                               &p_dev->p_netif->netmask.u_addr.ip4) == AW_TRUE) {
                evt.type = AW_NET_STS_EVT_TYPE_SUBNET_PREFIX_REPEAT;
                evt.u.subnet_prefix_repeat.p_netif[0] = p_dev->p_netif;
                evt.u.subnet_prefix_repeat.p_netif[1] = p_dev_tmp->p_netif;
                ret = aw_net_sts_evt_proc(p_ctl, &evt);
                aw_assert(ret == AW_OK);
            }
        }
    }
}

/*
 * Low priority task for handing domain name queries.
 */
aw_local void __dwork_task_entry (void *p_arg)
{
    __dwork_evt_t            evt;
    ip_addr_t                dns_server;
    aw_err_t                 ret;
    aw_tick_t                prefix_check_tick;        

    prefix_check_tick = aw_sys_tick_get();

    AW_FOREVER {
        aw_mdelay(100);
        ret = AW_MSGQ_RECEIVE(__gp_dwork_ctx.msgq,
                              &evt,
                              sizeof(evt),
                              aw_ms_to_ticks(1000));

        /* Check the IP subnet prefix and report the netif of the
         * same prefix. */
        if (__INTERVAL_MS_GET(prefix_check_tick) >= 30000) {
            prefix_check_tick = aw_sys_tick_get();
            aw_net_sts_ctl_iterate(__subnet_prefix_chk, NULL);
        }

        if (ret != AW_OK) {
            continue;
        }

        /* Handing message. */
        switch (evt.type) {
            case __DWORK_EVT_WAIT_FORCE_NETIF: {
                aw_net_sts_ctl_t    *p_ctl = evt.u.wati_force_netif.p_ctl;
                aw_net_sts_dev_t    *p_dev = p_ctl->wait_force_evt.u.force_link.p_dev;
                __LOG_DEV_DEF(       p_ctl);

                aw_assert(p_ctl);
                /* It needs to be locked because the other task is
                 * operating the p_ctl directly without using the interface . */
                ret = AW_MUTEX_LOCK(p_ctl->mutex, aw_ms_to_ticks(2000));
                aw_assert(ret == AW_OK);
                if (p_ctl->flag_wait_force_netif) {
                    aw_assert(__EVT_TYPE_IS_FORCE_LINK_OK(&p_ctl->wait_force_evt));

                    if (p_dev->p_netif) {
                        __LOG_PFX(1, "The netif of p_dev is available and the "
                                "mandatory event is being reported.(%s)",
                                p_dev->attr.netif_name);

                        p_ctl->wait_force_evt.u.force_link.p_netif = p_dev->p_netif;
                        p_ctl->flag_wait_force_netif = AW_FALSE;

                        /* Report the event if the NETIF exists. */
                        __link_evt_proc(p_ctl, &p_ctl->wait_force_evt);
                    } else {
                        /* Add a new delay check event because the NETIF does not exists. */
                        ret = __force_netif_evt_proc(p_ctl);

                        __LOG_PFX(0, "Add a new delay check event "
                                  "because the NETIF does not exists.(%s)",
                                  p_dev->attr.netif_name);
                        aw_assert(ret == AW_OK);
                    }
                }
                AW_MUTEX_UNLOCK(p_ctl->mutex);

            } break;

            case __DWORK_EVT_TYPE_DNS_QUERY:{

                aw_net_sts_ctl_t    *p_ctl = evt.u.dns_query.p_ctl;
                __dns_cache_t       *p_cache;
                const char          *host = evt.u.dns_query.host;
                int                  ret = -AW_ENODATA;
                ip_addr_t            addr;
                ip_addr_t            dns_addr;
                struct  netif       *p_netif;
                aw_bool_t            expired_pre;

                __LOG_DEV_DEF(p_ctl);

                /* Set the flag that does not allow the event to be added to
                 * prevent mailbox overflow. */
                p_ctl->flag_dns_processing = AW_TRUE;

                /* Look for the cache first */
                p_cache = __dns_cache_find(host);

                expired_pre = AW_FALSE;

                if (p_cache) {
                    if (!__dns_cache_is_expired_pre(p_cache)) {
                        /* Exit if the cache is valid. */
                        goto dns_exit;
                    } else {
                        /* Need to check that the cache is valid and reset
                         * the valid time. */
                        expired_pre = AW_TRUE;
                    }
                }

                __ASSERT_D(host, !=, NULL);
                ret = -AW_ENODATA;
                if (p_ctl->p_cur_sts) {
                    /* Use the default netif and dns server when network OK. */
                    ret = __dns_query(NULL, host, &addr);
                } else {
                    p_netif = NULL;
                    while (__dns_record_foreach(&p_netif, &dns_addr) == AW_OK) {
                        /*
                         * When using multi-network card management,
                         * the default network card should be empty if
                         * there are no LINK-UP event message.
                         */
                        netifapi_netif_set_default(p_netif);
                        ret = __dns_query(&dns_addr, host, &addr);
                        netifapi_netif_set_default(NULL);

                        if (ret == AW_OK) {
                            break;
                        }

                        /* Perform an exit if the network is set to OK when
                         * a domain name query is made. */
                        if (p_ctl->p_cur_sts) {
                            break;
                        }
                    }

                    /* Restore the default netif if the network is set
                     * to OK when a domain name query is made. */
                    if (p_ctl->p_cur_sts) {
                        if (__EVT_TYPE_IS_LINK_OK(p_ctl->p_cur_sts)) {
                            p_netif = p_ctl->p_cur_sts->u.link.p_netif;
                        }
                        if (__EVT_TYPE_IS_FORCE_LINK_OK(p_ctl->p_cur_sts)) {
                            p_netif = p_ctl->p_cur_sts->u.force_link.p_netif;
                        }

                        __ASSERT_D(p_netif, !=, NULL);
                        __dns_record_get(p_netif, &dns_server);
                        if (!ip_addr_isany(&dns_server)) {
                            dns_setserver(0, &dns_server);
                        }

                        netifapi_netif_set_default(p_netif);
                    }
                }

                if (ret == AW_OK) {
                    char addr_str[64];

                    if (addr.type == IPADDR_TYPE_V4) {
                        inet_ntoa_r(addr.u_addr.ip4, addr_str, sizeof(addr_str));
                    } else {
                        aw_assert(addr.type == IPADDR_TYPE_V6);
                        inet6_ntoa_r(addr.u_addr.ip6, addr_str, sizeof(addr_str));
                    }
                    if (!expired_pre) {
                        __dns_cache_add(host, &addr);  /* Add a new cache */
                        __LOG_PFX(1, "The domain name is resolved successfully "
                                "and added to the cache table. \n\t (\"%s\" : %s) ",
                                host, addr_str);
                    } else {
                        /* Restart the valid time of the old cache  */
                        aw_assert(p_cache);
                        __dns_cache_update(p_cache, host, &addr);
                        __LOG_PFX(1, "The domain name is resolved successfully "
                                "and updated to the cache table. \n\t (\"%s\" : %s) ",
                                host, addr_str);
                    }
                } else {
                    __LOG_RES(ret == AW_OK, 1,
                              "dns_query: \"%s\" failed", host);
                }

dns_exit:
                p_ctl->flag_dns_processing = AW_FALSE;
            } break;
            default: break;
        }
    }
}

/******************************************************************************/

typedef struct __dns_record {
    struct __dns_record *p_next;
    struct netif        *p_netif;
    ip_addr_t            dns_server;
} __dns_record_t;

aw_local __dns_record_t *__gp_dns_head = NULL;

aw_local void __dns_record_set (struct netif *p_netif, ip_addr_t *p_dns)
{
    __dns_record_t *p_record = __gp_dns_head;
    uint8_t         cnt = 0;
    __LOG_DEV_DEF(NULL);

    while (p_record) {
        if (p_record->p_netif == p_netif) {
            /* Overwrite the old record directly. */
            p_record->dns_server = *p_dns;
            return;
        }
        p_record = p_record->p_next;
        cnt++;
        __ASSERT_D(cnt, <=, 30);
    }
    /* Each netif is allocated once, not released. */
    p_record = malloc(sizeof(*p_record));
    __ASSERT_D(p_record, !=, NULL);
    memset(p_record, 0, sizeof(*p_record));
    p_record->p_netif       = p_netif;
    p_record->dns_server    = *p_dns;
    p_record->p_next        = __gp_dns_head;
    __gp_dns_head           = p_record;
}

aw_local aw_err_t __dns_record_get (struct netif *p_netif, ip_addr_t *p_dns)
{
    __dns_record_t *p_record = __gp_dns_head;

    memset(p_dns, 0, sizeof(*p_dns));
    while (p_record) {
        if (p_record->p_netif == p_netif) {
            *p_dns = p_record->dns_server;
            return AW_OK;
        }
        p_record = p_record->p_next;
    }

    return -AW_ENODATA;
}

aw_local aw_err_t __dns_record_foreach (struct netif **pp_netif,
                                        ip_addr_t    *p_dns)
{
    aw_bool_t        flag_next = AW_FALSE;
    __dns_record_t  *p_record = __gp_dns_head;

    aw_assert(pp_netif && p_dns);
    memset(p_dns, 0, sizeof(*p_dns));

    if (p_record == NULL) {
        return -AW_ENODATA;
    }

    if (*pp_netif == NULL) {
        *pp_netif = p_record->p_netif;
        *p_dns   = p_record->dns_server;
        return AW_OK;
    }

    while (p_record) {
        if (flag_next) {
            *pp_netif = p_record->p_netif;
            *p_dns   = p_record->dns_server;
            return AW_OK;
        }

        if (p_record->p_netif == *pp_netif) {
            flag_next = AW_TRUE;
        }
        p_record = p_record->p_next;
    }

    return -AW_ENODATA;
}
/******************************************************************************/
struct __dns_cache {
    char        name[128];
    ip_addr_t   ip_addr;
    aw_tick_t   tick;
};

aw_local __dns_cache_t __g_dns_cache[__DNS_CACHE_MAX];
aw_local uint8_t       __g_dns_cache_index = 0;

aw_local __dns_cache_t *__dns_cache_empty_get (void)
{
    uint8_t i = __g_dns_cache_index;

    __g_dns_cache_index++;
    if (__g_dns_cache_index >= __DNS_CACHE_MAX) {
        __g_dns_cache_index = 0;
    }
    return &__g_dns_cache[i];
}

aw_local aw_err_t __dns_cache_add (const char *p_name, ip_addr_t *p_addr)
{
    __dns_cache_t *p_cache;
    p_cache = __dns_cache_empty_get();
    aw_assert(p_cache && p_name && p_addr);

    strncpy((char *)p_cache->name, (char *)p_name, sizeof(p_cache->name));
    p_cache->ip_addr = *p_addr;
    p_cache->tick    = aw_sys_tick_get();

    return AW_OK;
}

aw_local __dns_cache_t *__dns_cache_find (const char *p_name)
{
    int         i;
    aw_tick_t   cur = aw_sys_tick_get();
    __dns_cache_t *p_cache;

    for (i = 0; i < __DNS_CACHE_MAX; i++) {
        if (strcmp(p_name, __g_dns_cache[i].name) == 0) {
            p_cache = &__g_dns_cache[i];

            if (__DNS_CACHE_TIMEOUT <= aw_ticks_to_ms(cur - p_cache->tick)) {
                /* Handle expired cache  */
                memset(p_cache->name, 0, sizeof(p_cache->name));
                return NULL;
            }

            /* return the valid cache. */
            return p_cache;
        }
    }
    return NULL;
}

aw_local aw_err_t __dns_cache_update (__dns_cache_t *p_cache,
                                      const char    *p_name,
                                      ip_addr_t     *p_addr)
{
    aw_assert(p_cache && p_name && p_addr);
    strncpy((char *)p_cache->name, (char *)p_name, sizeof(p_cache->name));
    p_cache->ip_addr = *p_addr;
    p_cache->tick    = aw_sys_tick_get();

    return AW_OK;
}


/* Query whether the cache is about to expire. */
aw_local aw_bool_t __dns_cache_is_expired_pre (__dns_cache_t *p_cache)
{
    aw_tick_t   cur = aw_sys_tick_get();

    aw_assert(p_cache);
    if (__DNS_CACHE_TIMEOUT_PRE <= aw_ticks_to_ms(cur - p_cache->tick)) {
        return AW_TRUE;
    }
    return AW_FALSE;
}

aw_local aw_err_t __dns_query_evt_proc (aw_net_sts_ctl_t *p_ctl,
                                        aw_net_sts_evt_t *p_evt)
{
    __dns_cache_t   *p_cache;
    __dwork_evt_t    evt;
    const char      *host = p_evt->u.dns_query.host_name;
    int              ret = -AW_ENODATA;
    __LOG_DEV_DEF(p_ctl);
    __ASSERT_D(host, !=, NULL);

    /* Obtain the cache first.  */
    p_cache = __dns_cache_find(p_evt->u.dns_query.host_name);

    /* Has a mandatory query flag, which deletes the cache. */
    if (p_cache && p_evt->u.dns_query.flag_force_query) {
        memset(p_cache->name, 0, sizeof(p_cache->name));
        p_cache = NULL;
    }

    if (p_cache) {
        aw_tick_t   cur = aw_sys_tick_get();

        /* The cache is about to expire, initiating a retest of the domain name
         * used to refresh the cache valid time. */
        if (__DNS_CACHE_TIMEOUT_PRE <= aw_ticks_to_ms(cur - p_cache->tick)) {
            __CHECK_D(1, strlen(host) +1, <, __HOST_NAME_MAXLEN, return -AW_EINVAL);

            if (!p_ctl->flag_dns_processing) {
                evt.type = __DWORK_EVT_TYPE_DNS_QUERY;
                strncpy(evt.u.dns_query.host, host, __HOST_NAME_MAXLEN);
                evt.u.dns_query.p_ctl = p_ctl;
                AW_MSGQ_SEND(__gp_dwork_ctx.msgq,
                               &evt,
                               sizeof(evt),
                               AW_MSGQ_NO_WAIT,
                               AW_MSGQ_PRI_NORMAL);
            }
        }

        p_evt->u.dns_query.ip_addr    = p_cache->ip_addr;
        return AW_OK;
    } else {
        __CHECK_D(1, strlen(host) +1, <, __HOST_NAME_MAXLEN, return -AW_EINVAL);

        /* The task is processing the event. */
        if (p_ctl->flag_dns_processing) {
            return -AW_ENODATA;
        }

        evt.type = __DWORK_EVT_TYPE_DNS_QUERY;
        strncpy(evt.u.dns_query.host, host, __HOST_NAME_MAXLEN);
        evt.u.dns_query.p_ctl = p_ctl;
        AW_MSGQ_SEND(__gp_dwork_ctx.msgq,
                       &evt,
                       sizeof(evt),
                       AW_MSGQ_NO_WAIT,
                       AW_MSGQ_PRI_NORMAL);

        /* Need to wait for the delayed task query to have data. */
        ret = -AW_ENODATA;
    }

    return ret;
}

/******************************************************************************/

aw_local void __evt_handler_default (aw_net_sts_evt_t *p_evt)
{
    __LOG_DEV_DEF(NULL);

    switch (p_evt->type) {
        case AW_NET_STS_EVT_TYPE_LINK_INET:
        case AW_NET_STS_EVT_TYPE_LINK_SNET:{

            aw_assert(p_evt->u.link.p_netif);
            netifapi_netif_set_default(p_evt->u.link.p_netif);
            __LOG_PFX(1, "netifapi_netif_set_default(\"%s\")",
                      p_evt->u.link.p_dev->attr.netif_name);

            dns_setserver(0, &p_evt->u.link.dns_server);

        } break;

        case AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET:
        case AW_NET_STS_EVT_TYPE_FORCE_LINK_SNET_SET:{
            aw_assert(p_evt->u.force_link.p_dev->p_netif);
            netifapi_netif_set_default(p_evt->u.force_link.p_dev->p_netif);
            __LOG_PFX(1, "netifapi_netif_set_default(\"%s\")",
                      p_evt->u.force_link.p_dev->attr.netif_name);

            dns_setserver(0, &p_evt->u.force_link.dns_server);

        } break;

        case AW_NET_STS_EVT_TYPE_LINK_DOWN:
        case AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_CLR:
        case AW_NET_STS_EVT_TYPE_FORCE_LINK_SNET_CLR:{
            netifapi_netif_set_default(NULL);
            __LOG_PFX(1, "netifapi_netif_set_default(NULL)");
        } break;

        default: break;
    }
}

/******************************************************************************/
aw_local aw_err_t __dns_ipaddr_to_str (ip_addr_t *p_dns_server,
                                       char      *p_buf,
                                       size_t     buf_size)
{
    aw_assert(p_dns_server && p_buf && buf_size);

#if LWIP_IPV4
    if (p_dns_server->type == IPADDR_TYPE_V4) {
        p_buf = inet_ntoa_r(p_dns_server->u_addr.ip4, p_buf, buf_size);
        aw_assert(p_buf);   /* NULL if buf was too small */
    }
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
    if (p_dns_server->type == IPADDR_TYPE_V6) {
        p_buf = inet6_ntoa_r(p_dns_server->u_addr.ip6, p_buf, buf_size);
        aw_assert(p_buf);
    }
#endif /* LWIP_IPV6 */

    return AW_OK;
}

/**
 * Report event message to the application.
 */
aw_local aw_err_t __link_stat_update (aw_net_sts_ctl_t *p_ctl,
                                      aw_net_sts_evt_t *p_evt)
{
    __LOG_DEV_DEF(p_ctl);
    __ASSERT_D(p_evt, !=, NULL);
    char    dns_str[64];


    if (__EVT_TYPE_IS_LINK_OK(p_evt)) {

        __ASSERT_D(p_evt->u.link.p_netif, ==, p_evt->u.link.p_dev->p_netif);
        netifapi_netif_set_default(p_evt->u.link.p_netif);

        __dns_record_get(p_evt->u.link.p_netif, &p_evt->u.link.dns_server);
        if (!ip_addr_isany(&p_evt->u.link.dns_server)) {
            dns_setserver(0, &p_evt->u.link.dns_server);
        }
        __dns_ipaddr_to_str(&p_evt->u.link.dns_server, dns_str, sizeof(dns_str));
        __LOG_PFX(1, "Report a LINK_UP event. (dev:%s, prio:%d, dns:%s)",
                  p_evt->u.link.p_dev->attr.netif_name,
                  p_evt->u.link.p_dev->attr.prioty,
                  dns_str);

        /* The FORCE_LINK_SET/CLR event causes an automatic LINK UP/DOWN,
         * and it may occur that the event twice. */

        p_ctl->attr.pfn_evt_handler(p_evt);

        /* Save the current state event. */
        p_ctl->p_cur_sts = p_evt;

    } else if (__EVT_TYPE_IS_FORCE_LINK_OK(p_evt)) {

        __ASSERT_D(p_evt->u.force_link.p_netif, ==,
                   p_evt->u.force_link.p_dev->p_netif);

        netifapi_netif_set_default(p_evt->u.force_link.p_netif);

        __dns_record_get(p_evt->u.force_link.p_netif,
                         &p_evt->u.force_link.dns_server);

        if (!ip_addr_isany(&p_evt->u.force_link.dns_server)) {
            dns_setserver(0, &p_evt->u.force_link.dns_server);
        }
        __dns_ipaddr_to_str(&p_evt->u.force_link.dns_server,
                            dns_str,
                            sizeof(dns_str));
        __LOG_PFX(1, "Report a LINK_UP event of mandatory type. "
                  "(dev:%s, prio:%d, dns:%s)",
                          p_evt->u.force_link.p_dev->attr.netif_name,
                          p_evt->u.force_link.p_dev->attr.prioty,
                          dns_str);

        p_ctl->attr.pfn_evt_handler(p_evt);
        p_ctl->p_cur_sts = p_evt;

    } else if (__EVT_TYPE_IS_ANY_LINK_DOWN(p_evt)) {
        netifapi_netif_set_default(NULL);

        __LOG_PFX(1, "Report a LINK_DOWN event of %s type. (def_netif:NULL)",
                  __EVT_TYPE_IS_LINK_DOWN(p_evt) ? "automatic":"mandatory");

        p_ctl->attr.pfn_evt_handler(p_evt);

        /* Clean the current state event. */
        p_ctl->p_cur_sts = NULL;
    }

    return AW_OK;
}
/******************************************************************************/

/* Check to see if the two event messages have the same netif.
 * (event type can be LINK UP/DOWN)  */
aw_local aw_bool_t __sts_evt_netif_is_match (aw_net_sts_evt_t *p_evt1,
                                             aw_net_sts_evt_t *p_evt2)
{
    if (p_evt1 == NULL || p_evt2 == NULL) {
        return AW_FALSE;
    }

    /* Event type must be LINK UP/DOWN */
    do {
        if (__EVT_TYPE_IS_ANY_LINK_OK(p_evt1)) {
            break;
        }
        if (__EVT_TYPE_IS_ANY_LINK_DOWN(p_evt1)) {
            break;
        }
        if (__EVT_TYPE_IS_ANY_LINK_OK(p_evt2)) {
            break;
        }
        if (__EVT_TYPE_IS_ANY_LINK_DOWN(p_evt2)) {
            break;
        }

        return AW_FALSE;
    } while (0);

    /*
     * Note that both u.link and u.force_link must be the head of the structure.
     */
    if (p_evt1->u.link.p_netif == p_evt2->u.link.p_netif) {
        return AW_TRUE;
    }

    return AW_FALSE;
}


/*
 * Save the event message to the p_ctl and report it to the apllication.
 * Ability to handle LINK and FORCE_LINK event.
 *
 * Note: the contents of p_ctl->p_cur_sts can only be modified by this funciton.
 *
 * \param p_evt     If null, only try to restore the network state from saved
 *                  message.
 */
aw_local aw_err_t __link_evt_proc (aw_net_sts_ctl_t *p_ctl,
                                   aw_net_sts_evt_t *p_evt)
{
    uint32_t                i;
    aw_err_t                ret;
    aw_net_sts_evt_t       *p_prim;
    aw_net_sts_evt_t       *p_tmp;
    aw_net_sts_evt_t        tmp;
    __LOG_DEV_DEF(p_ctl);
    (void)ret;

    /*
     * Process the LINK_DOWN event.
     */
    if (__EVT_TYPE_IS_LINK_DOWN(p_evt)) {

        /* Report DONW event to the application if the message has
         * been reported OK. */
        if (__sts_evt_netif_is_match(p_ctl->p_cur_sts, p_evt)) {
            __link_stat_update(p_ctl, p_evt);
            p_ctl->p_cur_sts = NULL;
        }

        /* Remove it from array. */
        for (i = 0; i < __NET_STS_EVT_ARRAY_MAX; i++) {
            p_tmp = &p_ctl->sts_arr[i];
            if (!__EVT_TYPE_IS_LINK_OK(p_tmp)) {
                continue;
            }

            if (__sts_evt_netif_is_match(p_evt, p_tmp)) {
                /* Reset evt value.  */
                memset(p_tmp, 0, sizeof(*p_tmp));
                p_tmp->type = AW_NET_STS_EVT_TYPE_LINK_DOWN;
                break;
            }
        }
    }

    if (__EVT_TYPE_IS_FORCE_LINK_DOWN(p_evt)) {

        /* Report DONW event to the application if the message has
         * been reported OK. */
        if (__sts_evt_netif_is_match(p_ctl->p_cur_sts, p_evt)) {
            __link_stat_update(p_ctl, p_evt);
            p_ctl->p_cur_sts = NULL;
        }

        /* Remove it from array. */
        for (i = 0; i < __NET_STS_EVT_ARRAY_MAX; i++) {
            p_tmp = &p_ctl->sts_arr[i];
            if (!__EVT_TYPE_IS_FORCE_LINK_OK(p_tmp)) {
                continue;
            }

            if (__sts_evt_netif_is_match(p_evt, p_tmp)) {
                /* Reset evt value.  */
                memset(p_tmp, 0, sizeof(*p_tmp));
                p_tmp->type = AW_NET_STS_EVT_TYPE_LINK_DOWN;
                break;
            }
        }
    }

    /* Process UP event.*/
    if (__EVT_TYPE_IS_LINK_OK(p_evt)) {

        /* Check if the array already has the UP message for the netif,
         * or add if it doesn't. */
        for (i = 0; i < __NET_STS_EVT_ARRAY_MAX; i++) {
            p_tmp = &p_ctl->sts_arr[i];

            /* Find it and exit. */
            if (__EVT_TYPE_IS_LINK_OK(p_tmp) &&
                    __sts_evt_netif_is_match(p_tmp, p_evt)) {
                break;
            }

            /* Save the empty item using p_prim. */
            if (p_tmp->type == AW_NET_STS_EVT_TYPE_LINK_DOWN) {
                p_prim = p_tmp;
            }

            /* Adds it when the last item in the array still does not match. */
            if (i == __NET_STS_EVT_ARRAY_MAX - 1) {
                /* Affirm the array is sufficient. */
                aw_assert(p_prim);
                *p_prim = *p_evt;
                break;
            }
        }
    }
    if (__EVT_TYPE_IS_FORCE_LINK_OK(p_evt)) {

        /* Check if the array already has the UP message for the netif,
         * or add if it doesn't. */
        for (i = 0; i < __NET_STS_EVT_ARRAY_MAX; i++) {
            p_tmp = &p_ctl->sts_arr[i];
            aw_assert(__EVT_TYPE_IS_FORCE_LINK_DOWN(p_tmp) == AW_FALSE);

            /* Find it and exit. */
            if (__EVT_TYPE_IS_FORCE_LINK_OK(p_tmp) &&
                    __sts_evt_netif_is_match(p_tmp, p_evt)) {
                break;
            }

            /* Save the empty item using p_prim. */
            if (p_tmp->type == AW_NET_STS_EVT_TYPE_LINK_DOWN) {
                p_prim = p_tmp;
            }

            /* Adds it when the last item in the array still does not match. */
            if (i == __NET_STS_EVT_ARRAY_MAX - 1) {
                /* Affirm the array is sufficient. */
                aw_assert(p_prim);
                *p_prim = *p_evt;
                break;
            }
        }
    }

    if (p_ctl->flag_wait_force_netif) {
        /* No further steps are required if the flag is present. */
        return AW_OK;
    }

    /* Obtain the item with the highest priority. */
    p_prim = NULL;
    for (i = 0; i < __NET_STS_EVT_ARRAY_MAX; i++) {
        p_tmp = &p_ctl->sts_arr[i];

        /* the mandatory netif has the highest priority and can only exist one. */
        if (__EVT_TYPE_IS_FORCE_LINK_OK(p_tmp)) {
            p_prim = p_tmp;
            break;
        }

        if (__EVT_TYPE_IS_LINK_OK(p_tmp) && p_prim == NULL) {
            /* Set it to p_prim when find the first UP message.*/
            p_prim = p_tmp;
            continue;
        }

        /* Find items of higher priority.  */
        if (__EVT_TYPE_IS_LINK_OK(p_tmp) &&
                p_prim->u.link.prioty > p_tmp->u.link.prioty) {
            p_prim = p_tmp;
        }
    }

    /* Has a UP event message. */
    if (p_prim) {
        /* If the message is same, there is no operation. */
        if (p_ctl->p_cur_sts == p_prim) {
            return AW_OK;
        }

        /* If an UP event is not currently reported, the lastest event is reported. */
        if (p_ctl->p_cur_sts == NULL) {
            __link_stat_update(p_ctl, p_prim);
            p_ctl->p_cur_sts = p_prim;
            return AW_OK;
        }

        /* If the reported message is a mandatory type, it is of the
         * highest priority,with no action. */
        if (__EVT_TYPE_IS_FORCE_LINK_OK(p_ctl->p_cur_sts)) {
            return AW_OK;
        }

        /* Process the new mandatory event. */
        if (__EVT_TYPE_IS_FORCE_LINK_OK(p_prim)) {
            tmp = *p_ctl->p_cur_sts;
            tmp.type = AW_NET_STS_EVT_TYPE_LINK_DOWN;

            /* Puts the current message LINK_DOWN. */
            __link_stat_update(p_ctl, &tmp);

            /* Repores new UP message. */
            __link_stat_update(p_ctl, p_prim);
            p_ctl->p_cur_sts = p_prim;

            return AW_OK;
        }

        /* Process the event that p_prim is higher priority. */
        if (p_prim->u.link.prioty < p_ctl->p_cur_sts->u.link.prioty) {

            /* It will be LINK_UP/DOWN, and the FORCE_LINK type will only
             * be set manually.  */
            tmp = *p_ctl->p_cur_sts;
            tmp.type = AW_NET_STS_EVT_TYPE_LINK_DOWN;

            /* Puts the old state down. */
            __link_stat_update(p_ctl, &tmp);

            /* Report the new UP message. */
            __link_stat_update(p_ctl, p_prim);
            p_ctl->p_cur_sts = p_prim;

            return AW_OK;
        }

        /* No priority higher than current stat, no operation. */
        if (p_prim->u.link.prioty >= p_ctl->p_cur_sts->u.link.prioty) {
            return AW_OK;
        }

        /* Will not be performed there, the above steps should be able to handle
         * all conditions.  */
        aw_assert(0);
    }

    return AW_OK;
}

/******************************************************************************/
aw_local aw_net_sts_ctl_t *__gp_ctl_lists = NULL;

aw_err_t aw_net_sts_ctl_iterate (
                aw_net_sts_ctl_iterate_cb_t  pfn_iterate_cb,
                void                        *p_cb_arg)
{
    aw_net_sts_ctl_t *p_ctl = __gp_ctl_lists;
    __LOG_DEV_DEF(p_ctl);

    __CHECK_D(1, p_ctl, !=, NULL, return -AW_NET_STS_ECTL);
    __CHECK_D(1, pfn_iterate_cb, !=, NULL, return -AW_EINVAL);

    while (p_ctl) {
        pfn_iterate_cb(p_ctl, p_cb_arg);
        p_ctl = p_ctl->p_next;
    }

    return AW_OK;
}

aw_net_sts_ctl_t * aw_net_sts_ctl_create (aw_net_sts_ctl_init_t *p_init)
{
    aw_net_sts_ctl_t *p_ctl;
    aw_net_sts_ctl_t *p_tmp;
    int               i;
    __LOG_DEV_DEF(NULL);

    p_ctl = malloc(sizeof(aw_net_sts_ctl_t));
    __ASSERT_D(p_ctl, !=, NULL);
    memset(p_ctl, 0, sizeof(*p_ctl));

    if (p_init) {
        p_ctl->attr = *p_init;
    }
    if (p_ctl->attr.pfn_evt_handler == NULL) {
        p_ctl->attr.pfn_evt_handler = __evt_handler_default;
    }
    AW_MUTEX_INIT(p_ctl->mutex, AW_SEM_Q_PRIORITY);

    {
        AW_MSGQ_INIT(__gp_dwork_ctx.msgq,
                     __DWORK_MSGQ_NUM,
                     __DWORK_MSGQ_SIZE,
                     AW_MSGQ_Q_PRIORITY);

        AW_TASK_INIT(__gp_dwork_ctx.task,
                     "net_sts:defer work",
                     10,
                     __DWORK_TASK_STACK_SIZE,
                     __dwork_task_entry,
                     (void*)NULL);
        AW_TASK_STARTUP(__gp_dwork_ctx.task);
    }

    /* Set the default value for the array of status event message. */
    for (i = 0; i < __NET_STS_EVT_ARRAY_MAX; i++) {
        p_ctl->sts_arr[i].type = AW_NET_STS_EVT_TYPE_LINK_DOWN;
    }

    /* Add to list.*/
    if (__gp_ctl_lists == NULL) {
        __gp_ctl_lists = p_ctl;
    } else {
        p_tmp          = __gp_ctl_lists;
        __gp_ctl_lists = p_ctl;
        p_ctl->p_next  = p_tmp;
    }

    return p_ctl;
}

aw_net_sts_dev_t * aw_net_sts_dev_create (aw_net_sts_dev_init_t *p_init)
{
    aw_net_sts_dev_t *p_dev = NULL;
    aw_assert(p_init);

    switch (p_init->type) {
        case AW_NET_STS_DEV_TYPE_4G :{
            p_dev = aw_net_sts_4g_dev_create(p_init);
        } break;
        case AW_NET_STS_DEV_TYPE_ETH :{
            p_dev = aw_net_sts_eth_dev_create(p_init);
        } break;
        case AW_NET_STS_DEV_TYPE_WIFI :{
            p_dev = aw_net_sts_wifi_dev_create(p_init);
        } break;
        default: aw_assert(0); break;
    }

    aw_assert(p_dev);
    return p_dev;
}

aw_err_t aw_net_sts_dev_reinit (aw_net_sts_ctl_t      *p_ctl,
                                aw_net_sts_dev_t      *p_dev,
                                aw_net_sts_dev_init_t *p_init)
{
    int ret;
    __LOG_DEV_DEF(p_ctl);
    __ASSERT_D(p_ctl, !=, NULL);
    __ASSERT_D(p_dev, !=, NULL);

    if (p_dev->flag_busy == AW_TRUE) {
        return -AW_EBUSY;
    }

    if (p_dev->p_ctl == NULL) {
        return -AW_ENOSR;
    }

    /* Unsupported type */
    __CHECK_D(__LOG_CHK, p_dev->attr.type, ==, p_init->type, return -AW_EINVAL);

    ret = AW_MUTEX_LOCK(p_ctl->mutex, aw_ms_to_ticks(__MUTEX_TIMEOUT));
    __ASSERT_D(ret, ==, AW_OK);

    p_dev->p_ctl = p_ctl;
    if (p_dev->pfn_reinit) {
        ret = p_dev->pfn_reinit(p_dev, p_init);
    } else {
        ret = -AW_ENOTSUP;
    }
    AW_MUTEX_UNLOCK(p_ctl->mutex);
    return ret;
}

aw_err_t aw_net_sts_dev_destroy (aw_net_sts_dev_t  *p_dev)
{
    if (p_dev->flag_busy == AW_TRUE) {
        return -AW_EBUSY;
    }
    return -AW_ENOTSUP;
}

aw_bool_t aw_net_sts_dev_is_free (aw_net_sts_dev_t *p_dev)
{
    __LOG_DEV_DEF(NULL);
    __ASSERT_D(p_dev, !=, NULL);
    if (p_dev->flag_busy == AW_TRUE) {
        return AW_FALSE;
    }
    return (NULL == p_dev->p_ctl) ? AW_TRUE : AW_FALSE;
}

aw_bool_t aw_net_sts_dev_is_busy (aw_net_sts_dev_t *p_dev)
{
    __LOG_DEV_DEF(NULL);
    __ASSERT_D(p_dev, !=, NULL);
    return p_dev->flag_busy;
}

aw_err_t aw_net_sts_add (aw_net_sts_ctl_t *p_ctl, aw_net_sts_dev_t *p_dev)
{
    aw_err_t  ret = AW_OK;
    __LOG_DEV_DEF(p_ctl);
    __ASSERT_D(p_ctl, !=, NULL);
    __ASSERT_D(p_dev, !=, NULL);

    if (p_dev->flag_busy == AW_TRUE) {
        return -AW_EBUSY;
    }

    ret = AW_MUTEX_LOCK(p_ctl->mutex, aw_ms_to_ticks(__MUTEX_TIMEOUT));
    __ASSERT_D(ret, ==, AW_OK);
    __LOG_PFX(__LOG_MUTEX, "<LOCK> p_dev:%p task:%s", p_dev, p_ctl->mutex.owner->name);

    ret = AW_OK;
    if (p_dev->p_ctl == NULL) {
        p_dev->p_ctl = p_ctl;
        aw_assert(p_dev->pfn_startup);
        ret = p_dev->pfn_startup(p_dev);
    } else {
        __CHECK_D(__LOG_CHK, p_dev->p_ctl, !=, p_ctl, ret = -AW_EEXIST);
        __CHECK_D(__LOG_CHK, p_dev->p_ctl, ==, p_ctl, ret = -AW_EFAULT);
    }
    __ASSERT_D(p_dev->p_ctl, ==, p_ctl);

    if (ret == AW_OK) {
        /* Check to see if it has been added repeatedly. */
        for (int i = 0; i < __NET_STS_DEV_ARRAY_MAX; i++) {
            aw_assert(p_ctl->p_dev_arr[i] != p_dev);
        }

        for (int i = 0; i < __NET_STS_DEV_ARRAY_MAX; i++) {
            if (p_ctl->p_dev_arr[i] == NULL) {
                p_ctl->p_dev_arr[i] = p_dev;
                break;
            }
            aw_assert(i != __NET_STS_DEV_ARRAY_MAX - 1);
        }
    }
    AW_MUTEX_UNLOCK(p_ctl->mutex);
    __LOG_PFX(__LOG_MUTEX, "<UNLOCK> p_dev:%p", p_dev);
    return ret;
}

aw_err_t aw_net_sts_del (aw_net_sts_ctl_t *p_ctl, aw_net_sts_dev_t *p_dev)
{
    int ret;
    __LOG_DEV_DEF(p_ctl);
    __ASSERT_D(p_ctl, !=, NULL);
    __ASSERT_D(p_dev, !=, NULL);

    if (p_dev->flag_busy == AW_TRUE) {
        return -AW_EBUSY;
    }

    __CHECK_D(__LOG_CHK, p_dev->p_ctl, !=, NULL,  return -AW_EEXIST);
    __CHECK_D(__LOG_CHK, p_dev->p_ctl, ==, p_ctl, return -AW_EFAULT);

    ret = AW_MUTEX_LOCK(p_ctl->mutex, aw_ms_to_ticks(__MUTEX_TIMEOUT));
    __ASSERT_D(ret, ==, AW_OK);
    __LOG_PFX(__LOG_MUTEX, "<LOCK> p_dev:%p", p_dev);
    ret = p_dev->pfn_suspend(p_dev);

    /* Report the FORCE_LINK_DOWN event of the device
     * if it has sent a mandatory UP event. And the step of pfn_suspend will
     * process the LINK_DOWN event.
     */
    if (__EVT_TYPE_IS_FORCE_LINK_OK(p_ctl->p_cur_sts)) {
        /* Not allow to alter the contents of p_cur_sts directly. */
        aw_net_sts_evt_t evt = *p_ctl->p_cur_sts;
        if (evt.u.force_link.p_dev == p_dev) {
            if (evt.type == AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET) {
                evt.type = AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_CLR;
            } else {
                evt.type = AW_NET_STS_EVT_TYPE_FORCE_LINK_SNET_CLR;
            }
            __LOG_PFX(1, "Report the LINK_DOWN event of the device "
                    "because it has sent a mandatory UP event..");
            __link_evt_proc(p_ctl, &evt);
        }
    }
    if (p_ctl->flag_wait_force_netif &&
            p_ctl->wait_force_evt.u.force_link.p_dev == p_dev) {
        p_ctl->flag_wait_force_netif = AW_FALSE;
        __LOG_PFX(1, "Clear the flag that waiting device NETIF.(%s)",
                  p_dev->attr.netif_name);
    }

    /* Try to restore the network state from saved evt. */
    __link_evt_proc(p_ctl, NULL);

    for (int i = 0; i < __NET_STS_DEV_ARRAY_MAX; i++) {
        if (p_ctl->p_dev_arr[i] == p_dev) {
            p_ctl->p_dev_arr[i] = NULL;
            break;
        }
        aw_assert(i != __NET_STS_DEV_ARRAY_MAX - 1);
    }

    AW_MUTEX_UNLOCK(p_ctl->mutex);
    __LOG_PFX(__LOG_MUTEX, "<UNLOCK> p_dev:%p", p_dev);
    return ret;
}

aw_err_t aw_net_sts_get (aw_net_sts_ctl_t *p_ctl, aw_net_sts_evt_t *p_out)
{
    int     ret;
    __LOG_DEV_DEF(p_ctl);
    __ASSERT_D(p_ctl, !=, NULL);
    __ASSERT_D(p_out, !=, NULL);

    ret = AW_MUTEX_LOCK(p_ctl->mutex, aw_ms_to_ticks(__MUTEX_TIMEOUT));
    __ASSERT_D(ret, ==, AW_OK);
    __LOG_PFX(__LOG_MUTEX, "<LOCK>");


    if (p_ctl->flag_wait_force_netif) {
        *p_out = p_ctl->wait_force_evt;
        ret = AW_OK;
        goto exit;
    }

    if (p_ctl->p_cur_sts) {
        *p_out = *p_ctl->p_cur_sts;
    } else {
        memset(p_out, 0, sizeof(*p_out));
        p_out->type = AW_NET_STS_EVT_TYPE_LINK_DOWN;
    }
    ret = AW_OK;

exit:
    AW_MUTEX_UNLOCK(p_ctl->mutex);
    __LOG_PFX(__LOG_MUTEX, "<UNLOCK>");
    return ret;
}

aw_err_t aw_net_sts_evt_proc (aw_net_sts_ctl_t *p_ctl, aw_net_sts_evt_t *p_evt)
{
    int                  ret;
    aw_net_sts_dev_t    *p_dev = NULL;
    __LOG_DEV_DEF(p_ctl);
    __CHECK_D(__LOG_CHK, p_ctl, !=, NULL, return -AW_NET_STS_ECTL);
    __CHECK_D(__LOG_CHK, p_evt, !=, NULL, return -AW_NET_STS_EDEV);

    ret = AW_MUTEX_LOCK(p_ctl->mutex, aw_ms_to_ticks(__MUTEX_TIMEOUT));
    __ASSERT_D(ret, ==, AW_OK);
    __LOG_PFX(__LOG_MUTEX, "<LOCK> type:%d", p_evt->type);

    ret = AW_OK;
    switch (p_evt->type) {
        case AW_NET_STS_EVT_TYPE_LINK_INET:
        case AW_NET_STS_EVT_TYPE_LINK_SNET:
        case AW_NET_STS_EVT_TYPE_LINK_DOWN: {
            __link_evt_proc(p_ctl, p_evt);
            __LOG_PFX(__LOG_MUTEX, "<LOCK> type:%d finish", p_evt->type);
        } break;

        case AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET:
        case AW_NET_STS_EVT_TYPE_FORCE_LINK_SNET_SET: {

            __ASSERT_D(AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET +
                           __FORCE_LINK_SET_CLR_OFFS, ==,
                       AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_CLR);
            __ASSERT_D(AW_NET_STS_EVT_TYPE_FORCE_LINK_SNET_SET +
                           __FORCE_LINK_SET_CLR_OFFS, ==,
                       AW_NET_STS_EVT_TYPE_FORCE_LINK_SNET_CLR);

            if (__EVT_TYPE_IS_FORCE_LINK_OK(p_ctl->p_cur_sts) ||
                    p_ctl->flag_wait_force_netif) {
                /* Duplicate settings are not supported. */
                ret = -AW_NET_STS_EREMAN;
                __LOG_PFX(1, "Duplicate settings are not supported. (ret:%d)", ret);
                goto exit;
            }

            __CHECK_D(1, p_evt->u.force_link.p_dev, !=, NULL,
                      ret = -AW_NET_STS_EEVTDEV;
                      goto exit);

            /* If the device has not yet acquired NETIF, set the flag and
             * wait for background processing to report mandatory UP event. */
            if (p_evt->u.force_link.p_dev->p_netif == NULL) {
                p_evt->u.force_link.p_netif  = NULL;
                p_ctl->wait_force_evt        = *p_evt;

                /* Report a DOWN event directly if the current state is LINK_UP. */
                if (__EVT_TYPE_IS_LINK_OK(p_ctl->p_cur_sts)) {
                    aw_net_sts_evt_t evt = *p_ctl->p_cur_sts;
                    evt.type = AW_NET_STS_EVT_TYPE_LINK_DOWN;
                    __link_stat_update(p_ctl, &evt);
                }

                __LOG_PFX(1, "Wait for background processing to report "
                        "mandatory UP event because NETIF is currently invalid.(%s)",
                          p_evt->u.force_link.p_dev->attr.netif_name);

                p_ctl->flag_wait_force_netif = AW_TRUE;
                ret = __force_netif_evt_proc(p_ctl);
                aw_assert(ret == AW_OK);
                break;
            }

            /* All application to do is set the p_dev parameter for this event.*/
            p_evt->u.force_link.p_netif = p_evt->u.force_link.p_dev->p_netif;

            /* The previous UP event will be automatically shut down and
             * the mandatory UP event will be reported.  */
            __link_evt_proc(p_ctl, p_evt);

            __LOG_PFX(__LOG_MUTEX, "<LOCK> type:%d finish", p_evt->type);
        } break;

        case AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_CLR:
        case AW_NET_STS_EVT_TYPE_FORCE_LINK_SNET_CLR: {
            __CHECK_D(1, p_evt->u.force_link.p_dev, !=, NULL,
                      ret = -AW_NET_STS_EEVTDEV;
                      goto exit);

            if (p_ctl->flag_wait_force_netif) {
                if (p_ctl->wait_force_evt.u.force_link.p_dev !=
                            p_evt->u.force_link.p_dev) {
                    /* Device is not match. */
                    ret = -AW_NET_STS_EEVTDEV;
                    goto exit;
                }
                if (p_evt->type != p_ctl->wait_force_evt.type +
                                __FORCE_LINK_SET_CLR_OFFS) {
                    /* Type is not match. */
                    ret = -AW_NET_STS_EEVTTYPE_MAN;
                    goto exit;
                }

                p_ctl->flag_wait_force_netif = AW_FALSE;

                /* Try to restore the network state.  */
                __link_evt_proc(p_ctl, NULL);
                break;
            }

            if (!__EVT_TYPE_IS_FORCE_LINK_OK(p_ctl->p_cur_sts)) {
                break;
            }

            if (p_evt->type == AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_CLR) {
                __CHECK_D(1, p_ctl->p_cur_sts->type, ==,
                          AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET,
                          ret = -AW_NET_STS_EEVTTYPE_MAN;
                          goto exit);
            }

            if (p_evt->type == AW_NET_STS_EVT_TYPE_FORCE_LINK_SNET_CLR) {
                __CHECK_D(1, p_ctl->p_cur_sts->type, ==,
                          AW_NET_STS_EVT_TYPE_FORCE_LINK_SNET_SET,
                          ret = -AW_NET_STS_EEVTTYPE_MAN;
                          goto exit);
            }

            /* Set the parameter automatically. */
            p_evt->u.force_link.p_netif = p_evt->u.force_link.p_dev->p_netif;

            /* The event of mandatory type will be reported, and the current
             * network status is automatically checked and reported. */
            __link_evt_proc(p_ctl, p_evt);
        } break;


        case AW_NET_STS_EVT_TYPE_DNS: {
            aw_net_sts_evt_t     sts;

            __ASSERT_D(p_evt->u.dns.p_netif, !=, NULL);
            __ASSERT_D(ip_addr_isany(&p_evt->u.dns.dns_server), ==, AW_FALSE);

            __LOG_PFX(1, "AW_NET_STS_EVT_TYPE_DNS: netif:%c%c%d dns:%s ",
                      p_evt->u.dns.p_netif->name[0],
                      p_evt->u.dns.p_netif->name[1],
                      p_evt->u.dns.p_netif->num,
                      ipaddr_ntoa(&p_evt->u.dns.dns_server));
            __dns_record_set(p_evt->u.dns.p_netif, &p_evt->u.dns.dns_server);

            memset(&sts, 0, sizeof(sts));
            __ASSERT_D(aw_net_sts_get(p_ctl, &sts), ==, AW_OK);
            if (sts.type == AW_NET_STS_EVT_TYPE_LINK_INET ||
                    sts.type == AW_NET_STS_EVT_TYPE_LINK_SNET) {
                /* When the network is valid, only the dns server of the default
                 * netif can be set. */
                if (p_evt->u.dns.p_netif == sts.u.link.p_netif) {
                    dns_setserver(0, &p_evt->u.dns.dns_server);
                }
            } else {
                dns_setserver(0, &p_evt->u.dns.dns_server);
            }
        } break;

        case AW_NET_STS_EVT_TYPE_DNS_QUERY: {
            ret = __dns_query_evt_proc(p_ctl, p_evt);
            if (ret != AW_OK) {
                aw_mdelay(100);
                ret = __dns_query_evt_proc(p_ctl, p_evt);
                if (ret != AW_OK) {
                    goto exit;
                }
            }

        } break;

        case AW_NET_STS_EVT_TYPE_NETIF_SUBNET_CHECK_ADD: {
            ret = AW_OK;

            __CHECK_D(1, p_evt->u.netif_subnet_check.p_netif, !=, NULL,
                      ret = -AW_NET_STS_ENETIF;
                      goto exit);

            /* Check if set by repeat. */
            for (int i = 0; i < __NET_STS_DEV_ARRAY_MAX; i++) {
                if (p_ctl->p_dev_arr[i] == NULL) {
                    continue;
                }
                if (p_ctl->p_dev_arr[i]->p_netif ==
                        p_evt->u.netif_subnet_check.p_netif) {
                    /* Return error directly. */
                    ret = -AW_EEXIST;
                    goto exit;
                }
            }

            /* Allocate an instance. */
            p_dev = (aw_net_sts_dev_t*)malloc(sizeof(aw_net_sts_dev_t));
            aw_assert(p_dev);
            memset(p_dev, 0, sizeof(*p_dev));
            p_dev->p_netif = p_evt->u.netif_subnet_check.p_netif;

            /* Add to net_sts_dev array. */
            for (int i = 0; i < __NET_STS_DEV_ARRAY_MAX; i++) {
                if (p_ctl->p_dev_arr[i] == NULL) {
                    p_ctl->p_dev_arr[i] = p_dev;
                    ret = AW_OK;
                    break;
                }
            }

        } break;

        case AW_NET_STS_EVT_TYPE_NETIF_SUBNET_CHECK_DEL: {
            struct netif *p_netif = p_evt->u.netif_subnet_check.p_netif;
            ret = AW_OK;

            __CHECK_D(1, p_netif, !=, NULL, ret = -AW_NET_STS_ENETIF; goto exit);

            /* Check if set by repeat. */
            for (int i = 0; i < __NET_STS_DEV_ARRAY_MAX; i++) {
                p_dev = p_ctl->p_dev_arr[i];
                if (p_ctl->p_dev_arr[i] == NULL) {
                    continue;
                }
                if (p_ctl->p_dev_arr[i]->p_netif != p_netif) {
                    continue;
                }
                p_ctl->p_dev_arr[i] = NULL;

                /* The p_ctl and pfn_startup for p_dev added by
                 * AW_NET_STS_EVT_TYPE_NETIF_SUBNET_CHECK_ADD should be NULL. */
                aw_assert(p_dev->p_ctl == NULL);
                aw_assert(p_dev->pfn_startup == NULL);
                free(p_dev);
                ret = AW_OK;
                goto exit;
            }

            ret = -AW_EEXIST;

        } break;

        case AW_NET_STS_EVT_TYPE_SUBNET_PREFIX_REPEAT: {
            aw_assert(p_evt->u.subnet_prefix_repeat.p_netif[0]);
            aw_assert(p_evt->u.subnet_prefix_repeat.p_netif[1]);
            p_ctl->attr.pfn_evt_handler(p_evt);
        } break;

        default: aw_assert(0); break;
    }


    AW_MUTEX_UNLOCK(p_ctl->mutex);
    __LOG_PFX(__LOG_MUTEX, "<UNLOCK>");
    return AW_OK;

exit:
    AW_MUTEX_UNLOCK(p_ctl->mutex);
    __LOG_PFX(__LOG_MUTEX, "<UNLOCK> err exit");
    return ret;
}


/* end of file */

