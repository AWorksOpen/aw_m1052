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
 * \brief 4G ģ������״̬���
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
#include "aw_gprs.h"
#include "aw_delay.h"
#include "aw_vdebug.h"

#include "lwip/ext/net_sts.h"
#include "lwip/ext/netif_route.h"
#include "net_sts_internal.h"

#include "aw_netif.h"
#include "aw_ping.h"

/******************************************************************************/
#define PFX       "4G_STS"

#define __LOG_DEV_DEF(p_dev)  \
    aw_net_sts_log_t _pfn_log_out_ = \
            (p_dev ? ((aw_net_sts_dev_t *)p_dev)->attr.pfn_log_out : NULL )

#define __4G_STS_FAST_RECOVERRY_EN      1


#define __LOG_NET_STS           1
#define __MASK_NET_PING         1
#define __LOG_BUSY_FLAG         0
/******************************************************************************/
#define __TIMEOUT_MSEC(tick_end, timeout) \
    for (tick_end = aw_sys_tick_get() + aw_ms_to_ticks(timeout); \
            aw_sys_tick_get() < tick_end; )

/* The limit minimum is 10S */
#define __CHK_MS_RECTIFY(tm)  ((tm) >= 10000 ? tm : 10000 )
/******************************************************************************/
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
#define __HOST_NAME_MAXLEN      128
/******************************************************************************/
typedef enum __4g_thread_status {
    __4G_THREAD_STAT_POWER_ON = 0,

    __4G_THREAD_STAT_IP_CHK_FIRST,
    __4G_THREAD_STAT_IP_CHK,
    __4G_THREAD_STAT_WAIT_IP_UPDATE,

    __4G_THREAD_STAT_LINK_INET,
    __4G_THREAD_STAT_LINK_INET_CHK,

    __4G_THREAD_STAT_LINK_SNET,
    __4G_THREAD_STAT_LINK_SNET_CHK,

    __4G_THREAD_STAT_SOFT_RESET,
    __4G_THREAD_STAT_POWER_OFF,
    __4G_THREAD_STAT_ABORT,
} __4g_thread_status_t;

/* Gets the response value of the request. */
#define __4G_LOW_TASK_REQ_TO_RSP(req)  ((req) + __4G_LOW_TASK_REQ_MAX)

typedef enum {
    __4G_LOW_TASK_REQ_NONE = 0,
    __4G_LOW_TASK_REQ_PWR_ON,  /* Request to perform power on. */
    __4G_LOW_TASK_REQ_SRESET,  /* Request to perform software reset. */
    __4G_LOW_TASK_REQ_IP_CHK,  /* Request to check network state. */
    __4G_LOW_TASK_REQ_LINK_CHK,

    __4G_LOW_TASK_REQ_MAX = 100,
    __4G_LOW_TASK_REQ_END = 0xFF,
} __4g_low_task_req_t;

typedef union {
    struct {
        aw_net_sts_dev_init_t  attr;
    } reinit;

    struct {
        __4g_thread_status_t     new_status;
    } status_alter;
} __4g_task_evt_params_t;

typedef struct {
    aw_net_sts_dev_t         dev;
    uint8_t                  rst_num;
    __4g_thread_status_t     status;
    int                      dev_id;

    struct in_addr           dest_addr;

    AW_SEMB_DECL(            wakeup);

    AW_MUTEX_DECL(           evt_mutex);
    uint8_t                  evt_flags[1];
    __4g_task_evt_params_t   evt_params;

    aw_bool_t                flag_ppp_close;

    char                     ip_buff[2][__HOST_NAME_MAXLEN];

    char                     low_task_name[32];
    AW_TASK_DECL(            low_task, AW_NET_STS_TASK_STACK_SIZE);
    __4g_low_task_req_t      low_task_req;

    struct {
        aw_err_t             ret;
        __4g_thread_status_t stat;
    } low_task_result;

    aw_bool_t                flag_delay;
} __net_sts_4g_dev_t;


enum {
    __EVT_FLAG_SUSPEND = 1,
    __EVT_FLAG_STARTUP,
    __EVT_FLAG_REINIT,
    __EVT_FLAG_STATUS_ALTER,
};
/******************************************************************************/
#define __PING_SEND_TTL     64  /* 96 */
#define __PING_TIMEOUT_MS   3000
#define __PING_DATA_SIZE    8

/******************************************************************************/
extern struct netif *netif_list;
aw_local struct netif *__gprs_netif_get (const char *name)
{
    struct netif *netif = NULL;

    for(netif = netif_list; netif != NULL; netif = netif->next) {
#if 0
        if (memcmp(netif->name, name, 2) == 0 &&
                netif->num == (name[2] - '0')) {
            return netif;
        }
#else
        if (memcmp(netif->name, name, 2) == 0) {
            return netif;
        }
#endif
    }
    return NULL;
}
/******************************************************************************/
/* Gets the request that low_task is currently processing. */
aw_local aw_err_t __low_task_req_get (__net_sts_4g_dev_t   *p_4g,
                                       __4g_low_task_req_t *p_req)
{
    aw_assert(p_4g && p_req);
    *p_req = p_4g->low_task_req;
    return AW_OK;
}


aw_local aw_bool_t __low_task_is_busy (__net_sts_4g_dev_t  *p_4g)
{
    aw_assert(p_4g);
    if (p_4g->low_task_req == __4G_LOW_TASK_REQ_NONE) {
        return AW_FALSE;
    }
    return AW_TRUE;
}
aw_local aw_bool_t __low_task_is_completed (__net_sts_4g_dev_t  *p_4g)
{
    aw_assert(p_4g);
    if (p_4g->low_task_req == __4G_LOW_TASK_REQ_NONE) {
        return AW_FALSE;
    }
    if (p_4g->low_task_req < __4G_LOW_TASK_REQ_MAX) {
        return AW_FALSE;
    }
    return AW_TRUE;
}

/*
 * \brief
 * Make a request to low_task, because unlocked only aollows called in 4g_task
 *
 * \retval AW_OK        Response received successfully.
 * \retval -AW_EEXIST   Added successfully or the request is being processed.
 * \retval -AW_EBUSY    Additional requests are being processed.
 */
aw_local aw_err_t __low_task_request (__net_sts_4g_dev_t  *p_4g,
                                      __4g_low_task_req_t req)
{
    __4g_low_task_req_t cur_req;
    aw_assert(req < __4G_LOW_TASK_REQ_MAX);

    __low_task_req_get(p_4g, &cur_req);
    if (cur_req == __4G_LOW_TASK_REQ_NONE) {
        /* Add the request */
        p_4g->low_task_req = req;
        return -AW_EEXIST;

    } else if (cur_req == req) {
        /* The request is being processed. */
        return -AW_EEXIST;

    } else if (cur_req == __4G_LOW_TASK_REQ_TO_RSP(req)) {
        /* Receive a response to this request */

        /* To clear the low_task_req event completion status,
         * otherwise low_task cannot longer be driven.  */
        p_4g->low_task_req = __4G_LOW_TASK_REQ_NONE;
        return AW_OK;
    }

    /* Additional requests are being processed. */
    return -AW_EBUSY;
}

/* Respond a request, it can only be called by low_task */
aw_local aw_err_t __low_task_respond (__net_sts_4g_dev_t   *p_4g,
                                      __4g_low_task_req_t   req,
                                      aw_err_t             *p_result,
                                      __4g_thread_status_t *p_stat)
{
    __LOG_DEV_DEF(&p_4g->dev);
    aw_assert(req < __4G_LOW_TASK_REQ_MAX);
    p_4g->low_task_req         = __4G_LOW_TASK_REQ_TO_RSP(req);

    if (p_result) {
        p_4g->low_task_result.ret  = *p_result;
    }
    if (p_stat) {
        p_4g->low_task_result.stat = *p_stat;
    }

    __LOG_PFX(1, "Respond to %d request. ", req);
    AW_SEMB_GIVE(p_4g->wakeup);
    return AW_OK;
}

/******************************************************************************/
aw_local aw_err_t __4g_task_evt_add (__net_sts_4g_dev_t     *p_this,
                                     uint8_t                 evt_flag,
                                     __4g_task_evt_params_t *params)
{
    aw_err_t ret;
    __LOG_DEV_DEF(&p_this->dev);

    ret = AW_MUTEX_LOCK(p_this->evt_mutex, aw_ms_to_ticks(3000));
    aw_assert(ret == AW_OK);

    /* ֻ�ܴ���һ�� evt */
    if (p_this->evt_flags[0]) {
        return -AW_EBUSY;
    }

    BITMAP_SET(p_this->evt_flags, evt_flag);
    if (params) {
        p_this->evt_params = *params;
    }
    p_this->dev.flag_busy = AW_TRUE;
    AW_SEMB_GIVE(p_this->wakeup);
    __LOG_PFX(__LOG_BUSY_FLAG, "Wait for 4g task process the %d event.", evt_flag);

    AW_MUTEX_UNLOCK(p_this->evt_mutex);
    return AW_OK;
}

aw_local aw_err_t __4g_task_evt_clr (__net_sts_4g_dev_t     *p_this)
{
    aw_err_t ret;
    __LOG_DEV_DEF(&p_this->dev);

    ret = AW_MUTEX_LOCK(p_this->evt_mutex, aw_ms_to_ticks(3000));
    aw_assert(ret == AW_OK);

    /* ֻ�����һ���¼���ֱ������ */
    p_this->evt_flags[0]  = 0;
    p_this->dev.flag_busy = AW_FALSE;
    __LOG_PFX(__LOG_BUSY_FLAG, "4g task completed the event and cleaned the busy flag.");

    AW_MUTEX_UNLOCK(p_this->evt_mutex);

    return AW_OK;
}
/******************************************************************************/

/* ���� >= 0 ��ʾ reply ʱ�䳤�ȣ� < 0 ��Ӧʧ�� */
static int __ping_check (aw_net_sts_dev_t   *p_dev,
                         struct netif       *netif,
                         const char         *p_chk_addr)
{
    __net_sts_4g_dev_t *p_4g = AW_CONTAINER_OF(p_dev, __net_sts_4g_dev_t, dev);
    char                     addr_str[16];
    char                     addr_str1[16];
    ip_addr_t                ip_addr;
    struct sockaddr_storage  to;
    struct sockaddr_storage  from;
    struct sockaddr_in      *si_to;
    struct sockaddr_in      *si_from;
    uint8_t                  ttl;
    aw_ping_t                ping;
    int                      ret;
    int                      i;
    __LOG_DEV_DEF(p_dev);

    i = 3;
    while (1) {
        aw_net_sts_evt_t evt;
        evt.type                            = AW_NET_STS_EVT_TYPE_DNS_QUERY;
        evt.u.dns_query.host_name           = p_chk_addr;
        evt.u.dns_query.flag_force_query    = AW_FALSE;
        ret = aw_net_sts_evt_proc(p_dev->p_ctl, &evt);
        if (ret == AW_OK) {
            p_4g->dest_addr.s_addr = evt.u.dns_query.ip_addr.u_addr.ip4.addr;
            break;
        } else {
            aw_mdelay(3000);
        }

        i--;
        if (i == 0) {
            return -1;
        }
    }

#if LWIP_ROUTE_TABLE
    ip_addr.type            = IPADDR_TYPE_V4;
    ip_addr.u_addr.ip4.addr = p_4g->dest_addr.s_addr;
    netif_route_add(netif, &ip_addr);
#endif

    memset(&to, 0, sizeof(to));
    memset(&from, 0, sizeof(from));

    to.ss_family           = AF_INET;
    to.s2_len              = sizeof(struct sockaddr_in);
    si_to                  = (struct sockaddr_in *) &to;
    si_to->sin_addr.s_addr = p_4g->dest_addr.s_addr;

    from.ss_family           = AF_INET;
    from.s2_len              = sizeof(struct sockaddr_in);
    si_from                  = (struct sockaddr_in *) &from;
    si_from->sin_addr.s_addr = (in_addr_t)netif->ip_addr.u_addr.ip4.addr;
    p_4g->dest_addr          = si_to->sin_addr;
    ttl                      = __PING_SEND_TTL;

    for (i = 0; i < 4; i++) {
        __ASSERT_D(netif, !=, NULL);
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
    __ASSERT_D(inet_ntoa_r(si_from->sin_addr.s_addr,
                           addr_str,
                           sizeof(addr_str)), !=, NULL);
    inet_ntoa_r(p_4g->dest_addr, addr_str1, sizeof(addr_str1));
    __LOG_RES(ret >= 0, __MASK_NET_PING, "\"%s\" ping host \"%s\" [%s]",
              addr_str, p_chk_addr, addr_str1);
    return ret;
}

/******************************************************************************/
aw_local __net_sts_4g_dev_t *__gp_4g_evt_handler_ctx;

aw_local void __4g_evt_handler (int dev_id, aw_gprs_evt_t *p_evt)
{
    aw_err_t                 ret;
    __net_sts_4g_dev_t      *p_4g = __gp_4g_evt_handler_ctx;
    aw_net_sts_dev_t        *p_dev = &p_4g->dev;
    aw_net_sts_evt_t         evt;
    __4g_task_evt_params_t   params;
    int                      time_ms;
    __LOG_DEV_DEF(p_dev);
    (void)ret;
    __ASSERT_D(p_4g->dev_id, ==, dev_id);

    switch (p_evt->type) {

        case AW_GPRS_EVT_TYPE_PWR_ON: {
            if (p_dev->attr.u._4g.pfn_event_notify) {
                aw_net_sts_4g_evt_t evt;
                evt.devid = p_4g->dev_id;
                evt.type  = AW_NET_STS_4G_EVT_TYPE_POWER_ON;
                p_dev->attr.u._4g.pfn_event_notify(&evt);
            }
        } break;
        case AW_GPRS_EVT_TYPE_ETH_LINK_UP: {
            p_dev->p_netif = &p_evt->u.eth_link_up.p_if->netif;
        } break;

        case AW_GPRS_EVT_TYPE_ETH_LINK_DOWN:
        case AW_GPRS_EVT_TYPE_PPP_LINK_DOWN: {

            /* ����δ OK ʱ������Ҫ�ϱ�  LINK DOWN �¼�  */
            if (p_4g->status != __4G_THREAD_STAT_LINK_INET_CHK &&
                    p_4g->status != __4G_THREAD_STAT_LINK_SNET_CHK &&
                    p_4g->status != __4G_THREAD_STAT_LINK_INET &&
                    p_4g->status != __4G_THREAD_STAT_LINK_SNET) {

                /* ֪ͨӦ�� ppp ���� */
                if (p_dev->attr.u._4g.pfn_event_notify) {
                    aw_net_sts_4g_evt_t evt;
                    evt.devid = p_4g->dev_id;
                    evt.type  = AW_NET_STS_4G_EVT_TYPE_NET_ERR;
                    p_dev->attr.u._4g.pfn_event_notify(&evt);
                }

                /*
                 * ��ʱ��״̬��Ϊ  __4G_THREAD_STAT_IP_CHK ��
                 * __4G_THREAD_STAT_WAIT_IP_UPDATE��ר�������������� snet_ip ��д����ʱ��
                 * ping �Ǻ�ʱ����ʧ�ܵ�
                 */
                p_4g->flag_ppp_close = 1;
                break;
            }

            /* ֻ������ OK ʱ���д�����¼��������������ȡ 4g task ���ź�������Ϊ��ǰ״̬����
             * 4g_task �д���ʱ�������� aw_gprs �ӿڣ�������� */

            if (p_dev->p_ctl) {
                evt.type    = AW_NET_STS_EVT_TYPE_LINK_DOWN;
                evt.u.link.p_dev   = p_dev;
                evt.u.link.p_netif = p_dev->p_netif;
                evt.u.link.prioty  = p_dev->attr.prioty;
                ret = aw_net_sts_evt_proc(p_dev->p_ctl, &evt);

                __LOG_PFX(__LOG_NET_STS,
                          "The device <%s> is disconnected from network "
                          "due to driver \r\n\t NET_LINK_DOWN event callback.",
                          p_dev->attr.netif_name, ret);
            }

            if (p_dev->attr.u._4g.pfn_event_notify) {
                aw_net_sts_4g_evt_t evt;
                evt.devid = p_4g->dev_id;
                evt.type  = AW_NET_STS_4G_EVT_TYPE_NET_ERR;
                p_dev->attr.u._4g.pfn_event_notify(&evt);
            }

            /* ��ʱ����״̬�϶��� OK ��(���ǽ��� suspend)��ֱ���� task ������λ����  */
            params.status_alter.new_status = __4G_THREAD_STAT_SOFT_RESET;
            time_ms = 1000;
            while (time_ms >= 50) {
                ret = __4g_task_evt_add(p_4g, __EVT_FLAG_STATUS_ALTER, &params);
                if (ret == AW_OK) {
                    break;
                } else {
                    aw_mdelay(50);
                    time_ms -= 50;
                    aw_assert(time_ms >= 50);
                }
            }
        } break;

        default: break;
    }
}


/******************************************************************************/
aw_local aw_err_t __evt_process (__net_sts_4g_dev_t  *p_4g)
{
    aw_net_sts_dev_t    *p_dev = &p_4g->dev;
    aw_net_sts_evt_t     evt;
    aw_err_t             ret;
    __4g_low_task_req_t  req;

    __LOG_DEV_DEF(p_dev);

    if (BITMAP_IS_VALID(p_4g->evt_flags, __EVT_FLAG_STATUS_ALTER)) {
        __low_task_req_get(p_4g, &req);
        if (req != __4G_LOW_TASK_REQ_NONE) {
            /* Wait for response. */
            return AW_OK;
        }
        __LOG_PFX(1, "Process STATUS_ALTER event.", p_dev->attr.task_name);
        p_4g->status = p_4g->evt_params.status_alter.new_status;
    }

    if (BITMAP_IS_VALID(p_4g->evt_flags, __EVT_FLAG_STARTUP)) {

        __LOG_PFX(1, "task startup :%s", p_dev->attr.task_name);
        switch (p_4g->status) {
            case __4G_THREAD_STAT_LINK_INET:
            case __4G_THREAD_STAT_LINK_SNET: {
                /*
                 * ��״̬ʱ�������л�δִ�� link up �¼��ķ��ͣ�
                 */
            } break;

            case __4G_THREAD_STAT_LINK_INET_CHK:
            case __4G_THREAD_STAT_LINK_SNET_CHK: {
                /*
                 * ��״̬ʱ������������ p_ctl ��δ���� link up��������з���
                 */

                if (p_4g->status == __4G_THREAD_STAT_LINK_INET_CHK) {
                    evt.type    = AW_NET_STS_EVT_TYPE_LINK_INET;
                } else {
                    evt.type    = AW_NET_STS_EVT_TYPE_LINK_SNET;
                }

                evt.u.link.p_dev   = p_dev;
                evt.u.link.p_netif = p_dev->p_netif;
                evt.u.link.prioty  = p_dev->attr.prioty;
                aw_net_sts_evt_proc(p_dev->p_ctl, &evt);

            } break;
            default :break;
        }
    }

    if (BITMAP_IS_VALID(p_4g->evt_flags, __EVT_FLAG_SUSPEND)) {
        __LOG_PFX(1, "task suspend :%s", p_dev->attr.task_name);
        switch (p_4g->status) {
            case __4G_THREAD_STAT_LINK_INET:
            case __4G_THREAD_STAT_LINK_SNET: {
                /*
                 * ��״̬ʱ����δ���� link up
                 */
            } break;

            case __4G_THREAD_STAT_LINK_INET_CHK:
            case __4G_THREAD_STAT_LINK_SNET_CHK: {
                /*
                 * ������ֱ�ӷ��� link down����������û�� p_ctl �����ٷ� net_sts �¼�
                 */
                evt.type    = AW_NET_STS_EVT_TYPE_LINK_DOWN;
                evt.u.link.p_dev   = p_dev;
                evt.u.link.p_netif = p_dev->p_netif;
                evt.u.link.prioty  = p_dev->attr.prioty;
                aw_net_sts_evt_proc(p_dev->p_ctl, &evt);
            } break;

            default :break;
        }

        /* ��� p_ctl �򲻻ᷢ�� ctl ���¼� */
        p_dev->p_ctl = NULL;
    }

    if (BITMAP_IS_VALID(p_4g->evt_flags, __EVT_FLAG_REINIT)) {
        aw_bool_t               flag_need_init = AW_FALSE;
        aw_net_sts_dev_init_t  *p_init = &p_4g->evt_params.reinit.attr;
        __4g_low_task_req_t     req;

        /* ���ڴ��������ʱ�������޸Ĳ�������ʱ������ʹ�� ping ��ַ */
        __low_task_req_get(p_4g, &req);
        if (__4G_LOW_TASK_REQ_IP_CHK == req) {
            return AW_OK;
        }

        __LOG_PFX(1, "The device %s reinitializes parameters.", p_dev->attr.task_name);

#if LWIP_ROUTE_TABLE
        /* ɾ�� ping ʱ��ӵ�·���� */
        if (p_dev->p_netif != NULL) {
            ip_addr_t                ip_addr;

            ip_addr.type            = IPADDR_TYPE_V4;
            ip_addr.u_addr.ip4.addr = p_4g->dest_addr.s_addr;
            netif_route_del(p_dev->p_netif, &ip_addr);
        }
#endif /* LWIP_ROUTE_TABLE */

        /* �������޸ĵĲ��������ֲ�����Ҫ���� INIT ״̬���¼�� ����Щ�����޸�״̬ */

        if (memcmp(p_4g->evt_params.reinit.attr.netif_name,
                   p_dev->attr.netif_name, sizeof(p_dev->attr.netif_name)) != 0) {
            flag_need_init = AW_TRUE;
        }

        if (p_4g->evt_params.reinit.attr.prioty != p_dev->attr.prioty) {
            flag_need_init = AW_TRUE;
        }

        if (p_init->p_chk_inet_addr != NULL &&
                strncmp(p_4g->ip_buff[0],
                        p_init->p_chk_inet_addr,
                       __HOST_NAME_MAXLEN) != 0) {
            flag_need_init = AW_TRUE;
            aw_assert(strlen(p_init->p_chk_inet_addr) < __HOST_NAME_MAXLEN - 1);
            strncpy(p_4g->ip_buff[0], p_init->p_chk_inet_addr, __HOST_NAME_MAXLEN);
        }
        if (p_init->p_chk_inet_addr == NULL && p_4g->ip_buff[0][0] != 0) {
            flag_need_init = AW_TRUE;
            memset(p_4g->ip_buff[0], 0, __HOST_NAME_MAXLEN);
        }

        if (p_init->p_chk_snet_addr != NULL &&
                strncmp(p_4g->ip_buff[1],
                        p_init->p_chk_snet_addr,
                       __HOST_NAME_MAXLEN) != 0) {
            flag_need_init = AW_TRUE;
            aw_assert(strlen(p_init->p_chk_snet_addr) < __HOST_NAME_MAXLEN - 1);
            strncpy(p_4g->ip_buff[1], p_init->p_chk_snet_addr, __HOST_NAME_MAXLEN);
        }
        if (p_init->p_chk_snet_addr == NULL && p_4g->ip_buff[1][0] != 0) {
            flag_need_init = AW_TRUE;
            memset(p_4g->ip_buff[1], 0, __HOST_NAME_MAXLEN);
        }

        if (p_dev->attr.pfn_log_out != p_4g->evt_params.reinit.attr.pfn_log_out) {
            aw_gprs_ioctl_args_t gprs_arg;
            gprs_arg.pfn_log_out = p_4g->evt_params.reinit.attr.pfn_log_out;
            ret = aw_gprs_ioctl(p_4g->dev_id, AW_GPRS_LOG_SET, &gprs_arg);
            __ASSERT_D(ret, ==, AW_OK);
        }

        p_dev->attr                 = p_4g->evt_params.reinit.attr;
        p_dev->attr.chk_ms          = __CHK_MS_RECTIFY(p_dev->attr.chk_ms);
        p_dev->attr.p_chk_inet_addr = p_4g->ip_buff[0];
        p_dev->attr.p_chk_snet_addr = p_4g->ip_buff[1];

        if (flag_need_init) {
            switch (p_4g->status) {
                case __4G_THREAD_STAT_LINK_INET:
                case __4G_THREAD_STAT_LINK_SNET:
                {
                    /* Ҫ�޸� 4g ��״̬����Ҫ�ȴ�������ɣ�����λ���󣬷����޷������µ�����  */
                    if (__low_task_is_busy(p_4g)) {
                        while (!__low_task_is_completed(p_4g)) {
                            aw_mdelay(10);
                        }
                        p_4g->low_task_req = __4G_LOW_TASK_REQ_NONE;
                    }

                    /* Ϊ��״̬ʱ����δ���� LINK UP �¼�����Ҫ���¼��ip  */
                    p_4g->status = __4G_THREAD_STAT_IP_CHK_FIRST;
                } break;

                case __4G_THREAD_STAT_IP_CHK: {
                    /* The response is consumed, and 4G will restart the request. */
                    if (__low_task_is_busy(p_4g)) {
                        while (!__low_task_is_completed(p_4g)) {
                            aw_mdelay(10);
                        }
                        p_4g->low_task_req = __4G_LOW_TASK_REQ_NONE;
                    }
                } break;

                case __4G_THREAD_STAT_WAIT_IP_UPDATE: {
                    if (__low_task_is_busy(p_4g)) {
                        while (!__low_task_is_completed(p_4g)) {
                            aw_mdelay(10);
                        }
                        p_4g->low_task_req = __4G_LOW_TASK_REQ_NONE;
                    }
                    p_4g->status = __4G_THREAD_STAT_IP_CHK;
                } break;

                case   __4G_THREAD_STAT_LINK_INET_CHK:
                case   __4G_THREAD_STAT_LINK_SNET_CHK: {
                    /*
                     * link down �����޸�״̬ Ϊ CHK �����������¼��
                     */
                    evt.type    = AW_NET_STS_EVT_TYPE_LINK_DOWN;
                    evt.u.link.p_dev   = p_dev;
                    evt.u.link.p_netif = p_dev->p_netif;
                    evt.u.link.prioty  = p_dev->attr.prioty;
                    aw_net_sts_evt_proc(p_dev->p_ctl, &evt);

                    if (__low_task_is_busy(p_4g)) {
                        while (!__low_task_is_completed(p_4g)) {
                            aw_mdelay(10);
                        }
                        p_4g->low_task_req = __4G_LOW_TASK_REQ_NONE;
                    }
                    p_4g->status = __4G_THREAD_STAT_IP_CHK_FIRST;
                } break;

                default: break;
            }
        }
    }

    __4g_task_evt_clr(p_4g);

    return AW_OK;
}
/******************************************************************************/
/* tag low task */

aw_local void __low_task_entry (void *p_arg)
{
    aw_net_sts_dev_t    *p_dev = p_arg;
    __net_sts_4g_dev_t  *p_4g = AW_CONTAINER_OF(p_dev, __net_sts_4g_dev_t, dev);
    int                  ret;
    aw_err_t             result;
    __4g_thread_status_t stat;
    aw_gprs_ioctl_args_t gprs_arg;
    __LOG_DEV_DEF(p_dev);
    __ASSERT_D(p_4g, !=, NULL);

    AW_FOREVER {
        aw_mdelay(100);
        /*
         * This task is driven by the low_task_req, and is not locked.
         *
         * When low_task_req is NONE, other tasks can start this task with
         * __4g_low_task_req_t request.
         *
         * Set the low_task_req to NONE when other tasks detect that it is
         * the the request completion .
         */
        switch (p_4g->low_task_req) {
            case __4G_LOW_TASK_REQ_PWR_ON: {
                /* Not perform power on if not perform pfn_startup. */
                if (p_dev->p_ctl == NULL) {
                    break;
                }

                p_4g->dev_id = aw_gprs_any_get(3);
                if (p_4g->dev_id < 0) {
                    __LOG_RES(AW_FALSE, 1, "4G module get id failed.");
                    break;
                }

                gprs_arg.pfn_log_out = p_dev->attr.pfn_log_out;
                ret = aw_gprs_ioctl(p_4g->dev_id, AW_GPRS_LOG_SET, &gprs_arg);
                __ASSERT_D(ret, ==, AW_OK);

                if (__gp_4g_evt_handler_ctx == NULL) {
                    __gp_4g_evt_handler_ctx = p_4g;
                } else {
                    aw_assert(__gp_4g_evt_handler_ctx == p_4g);
                }
                gprs_arg.pfn_evt_handler = __4g_evt_handler;
                ret = aw_gprs_ioctl(p_4g->dev_id, AW_GPRS_EVT_CB_SET, &gprs_arg);
                if (ret != AW_OK && ret != -AW_EEXIST) {
                    __ASSERT_D(ret, ==, AW_OK);
                }

                ret = aw_gprs_power_on(p_4g->dev_id);
                __LOG_RES(ret == AW_OK, 1, "4G module power-on");
                if (ret != AW_OK) {
                    aw_gprs_power_off(p_4g->dev_id);
                    break;
                }

                ret = aw_gprs_ioctl(aw_gprs_any_get(15), AW_GPRS_REQ_STS, &gprs_arg);
                __LOG_PFX(__LOG_NET_STS, "4G network status:%d", gprs_arg.sts);
                if (ret == AW_OK && gprs_arg.sts == AW_GPRS_STS_OK) {
                    result = AW_OK;
                } else {
                    result = -AW_ERROR;
                }

                /* Respond the request. */
                __low_task_respond(p_4g, p_4g->low_task_req, &result, NULL);
            } break;

            case __4G_LOW_TASK_REQ_SRESET: {
                /* Not perform software reset if not perform pfn_startup. */
                if (p_dev->p_ctl == NULL) {
                    p_4g->rst_num = 0;
                    result        = -AW_ERROR;
                    /* Respond the request. */
                    __low_task_respond(p_4g, p_4g->low_task_req, &result, NULL);
                    break;
                }

                if (p_4g->rst_num < 4) {
                    p_4g->rst_num++;

                    gprs_arg.reset.flag_hw_rst = 0;
                    gprs_arg.reset.delay_ms    = 30000;
                    ret = aw_gprs_ioctl(p_4g->dev_id, AW_GPRS_REQ_RESET, &gprs_arg);
                    __LOG_RES(ret == AW_OK, 1, "4G module performs a software reset.");
                    if (ret != AW_OK) {
                        /* Reset failed. */
                        p_4g->rst_num = 0;
                        result        = -AW_ERROR;

                        /* Respond the request. */
                        __low_task_respond(p_4g, p_4g->low_task_req, &result, NULL);
                        break;
                    }

                    ret = aw_gprs_ioctl(p_4g->dev_id, AW_GPRS_REQ_STS, &gprs_arg);
                    if (ret != AW_OK) {
                        p_4g->rst_num = 0;
                        result        = -AW_ERROR;

                        /* Respond the request. */
                        __low_task_respond(p_4g, p_4g->low_task_req, &result, NULL);
                        break;
                    }

                    if (gprs_arg.sts == AW_GPRS_STS_OK) {
                        /* Software reset and check network status are successfully. */
                        __LOG_RES(AW_TRUE,
                                  __LOG_NET_STS,
                                  "4G network recovered by software reset. (rst_num:%d)",
                                  p_4g->rst_num);
                        p_4g->rst_num = 0;
                        result        = AW_OK;
                        /* Respond the request. */
                        __low_task_respond(p_4g, p_4g->low_task_req, &result, NULL);
                    } else {
                        /* Continue to try software reset.  */
                        aw_mdelay(p_dev->attr.chk_ms);
                        break;
                    }

                } else {
                    p_4g->rst_num = 0;
                    result        = -AW_ERROR;
                    /* Respond the request. */
                    __low_task_respond(p_4g, p_4g->low_task_req, &result, NULL);
                    break;
                }

            } break;

            case __4G_LOW_TASK_REQ_IP_CHK: {
                struct netif * netif = NULL;

                aw_assert(strlen(p_dev->attr.p_chk_inet_addr) ||
                          strlen(p_dev->attr.p_chk_snet_addr));

                /* ���ȼ�⹫��,�ɹ����˳�  */
                if (strlen(p_dev->attr.p_chk_inet_addr)) {
                    netif = __gprs_netif_get(p_dev->attr.netif_name);
                    if (netif) {
                        ret = __ping_check(p_dev, netif, p_dev->attr.p_chk_inet_addr);
                        if (ret >= 0) {
                            result = AW_OK;
                            stat   = __4G_THREAD_STAT_LINK_INET;

                            /* Respond the request. */
                            __low_task_respond(p_4g,
                                               p_4g->low_task_req,
                                               &result,
                                               &stat);
                            break;
                        }
                    }
                }

                if (strlen(p_dev->attr.p_chk_snet_addr)) {
                    netif = __gprs_netif_get(p_dev->attr.netif_name);
                    if (netif) {
                        ret = __ping_check(p_dev, netif, p_dev->attr.p_chk_snet_addr);
                        if (ret >= 0) {
                            result = AW_OK;
                            stat   = __4G_THREAD_STAT_LINK_SNET;

                            /* Respond the request. */
                            __low_task_respond(p_4g,
                                               p_4g->low_task_req,
                                               &result,
                                               &stat);
                            break;
                        } else {
                            __LOG_RES(AW_FALSE, 1, "p_chk_snet_addr(%s) invalid.",
                                      p_dev->attr.p_chk_snet_addr)
                        }
                    }
                }

                /* Respond the request. */
                result = -AW_ERROR;
                __low_task_respond(p_4g, p_4g->low_task_req, &result, &stat);
            } break;

            case __4G_LOW_TASK_REQ_LINK_CHK: {
                ret = aw_gprs_ioctl(aw_gprs_any_get(15), AW_GPRS_REQ_STS, &gprs_arg);
                if (ret == AW_OK && gprs_arg.sts == AW_GPRS_STS_OK) {
                    /* Respond the request. */
                    result = AW_OK;
                    __low_task_respond(p_4g, p_4g->low_task_req, &result, &stat);
                } else {
                    /* Respond the request. */
                    result = -AW_ERROR;
                    __low_task_respond(p_4g, p_4g->low_task_req, &result, &stat);
                }
            } break;

            default:{
                /* NONE and REQ_TO_RSP(req) will be executed here */
                aw_mdelay(1000);
            } break;
        }
    }
}

aw_local void __task (aw_net_sts_dev_t *p_dev)
{
    __net_sts_4g_dev_t  *p_4g = AW_CONTAINER_OF(p_dev, __net_sts_4g_dev_t, dev);
    int                  ret;
    aw_net_sts_evt_t     evt;
    __LOG_DEV_DEF(p_dev);
    __ASSERT_D(p_4g, !=, NULL);

    if (p_dev->p_netif == NULL) {
        p_dev->p_netif = __gprs_netif_get(p_dev->attr.netif_name);
    }

    switch (p_4g->status) {
        case __4G_THREAD_STAT_POWER_ON: {

            ret = __low_task_request(p_4g, __4G_LOW_TASK_REQ_PWR_ON);
            aw_assert(ret != -AW_EBUSY);
            if (ret == AW_OK) {
                /* Module status check are successful. */
                if (p_4g->low_task_result.ret == AW_OK) {
                    p_4g->status = __4G_THREAD_STAT_IP_CHK_FIRST;
                } else {
                    p_4g->status = __4G_THREAD_STAT_SOFT_RESET;
                    p_4g->flag_delay = AW_TRUE;
                }
            } else {
                p_4g->flag_delay = AW_TRUE;
            }

        } break;

        case __4G_THREAD_STAT_POWER_OFF: {
            if (p_dev->attr.u._4g.pfn_event_notify) {
                aw_net_sts_4g_evt_t evt;
                evt.devid = p_4g->dev_id;
                evt.type  = AW_NET_STS_4G_EVT_TYPE_POWER_OFF_PRE;
                p_dev->attr.u._4g.pfn_event_notify(&evt);
            }

            aw_gprs_power_off(p_4g->dev_id);
            __LOG_RES(AW_TRUE, 1, "4G module power-off");
            p_4g->status = __4G_THREAD_STAT_POWER_ON;

            if (p_dev->attr.u._4g.pfn_event_notify) {
                aw_net_sts_4g_evt_t evt;
                evt.devid = p_4g->dev_id;
                evt.type  = AW_NET_STS_4G_EVT_TYPE_POWER_OFF;
                p_dev->attr.u._4g.pfn_event_notify(&evt);
            }
            p_4g->flag_delay = AW_TRUE;
        } break;

        case __4G_THREAD_STAT_IP_CHK_FIRST: {

            /**
             * �����״̬��4g ģ�鲦��link�ѽ���
             */
            if (p_dev->attr.u._4g.pfn_event_notify) {
                aw_net_sts_4g_evt_t evt;
                evt.devid = p_4g->dev_id;
                evt.type  = AW_NET_STS_4G_EVT_TYPE_NET_LINK_UP;
                p_dev->attr.u._4g.pfn_event_notify(&evt);
            }

            p_4g->status = __4G_THREAD_STAT_IP_CHK;
        } break;

        case __4G_THREAD_STAT_IP_CHK: {
            p_4g->flag_delay = AW_TRUE;

            /* û�м�� ip ֱ������Ϊ�ɷ��ʹ���  */
            if (strlen(p_dev->attr.p_chk_inet_addr) == 0 &&
                    strlen(p_dev->attr.p_chk_snet_addr) == 0) {
                aw_assert(!__low_task_is_busy(p_4g));
                p_4g->status = __4G_THREAD_STAT_LINK_INET;
                p_4g->flag_delay = AW_FALSE;
                break;
            }

            ret = __low_task_request(p_4g, __4G_LOW_TASK_REQ_IP_CHK);
            aw_assert(ret != -AW_EBUSY);
            if (ret == AW_OK) {
                if (p_4g->low_task_result.ret == AW_OK) {
                    p_4g->status     = p_4g->low_task_result.stat;
                    p_4g->flag_delay = AW_FALSE;
                    break;
                } else {
                    /* The PPP connection was successful, but the ping test
                     * failed, waiting for the test IP to be altered. */
                    p_4g->status = __4G_THREAD_STAT_WAIT_IP_UPDATE;
                    break;
                }
            }
        } break;

        case __4G_THREAD_STAT_WAIT_IP_UPDATE: {
            p_4g->flag_delay = AW_TRUE;

            if (p_4g->flag_ppp_close) {
                p_4g->flag_ppp_close    = 0;
                p_4g->status            = __4G_THREAD_STAT_SOFT_RESET;
            } else {
                p_4g->status            = __4G_THREAD_STAT_IP_CHK;
            }
        } break;

        case __4G_THREAD_STAT_LINK_INET:
        case __4G_THREAD_STAT_LINK_SNET: {
            p_4g->flag_delay = AW_TRUE;

            /*
             * �� p_ctl ʱ������ net_sts �¼����� startup �� suspend �д���
             */
            if (p_dev->p_ctl) {
                if (p_4g->status == __4G_THREAD_STAT_LINK_INET) {
                    evt.type    = AW_NET_STS_EVT_TYPE_LINK_INET;
                    __LOG_PFX(__LOG_NET_STS, "4g module is connected to Internet.");
                } else {
                    evt.type    = AW_NET_STS_EVT_TYPE_LINK_SNET;
                    __LOG_PFX(__LOG_NET_STS, "4g module is connected to private network.");
                }
                evt.u.link.p_dev   = p_dev;
                evt.u.link.p_netif = p_dev->p_netif;
                evt.u.link.prioty  = p_dev->attr.prioty;
                __ASSERT_D(p_dev->p_netif, !=, NULL);
                aw_net_sts_evt_proc(p_dev->p_ctl, &evt);
            }

            if (p_4g->status == __4G_THREAD_STAT_LINK_INET) {
                p_4g->status = __4G_THREAD_STAT_LINK_INET_CHK;
            } else {
                p_4g->status = __4G_THREAD_STAT_LINK_SNET_CHK;
            }

            if (p_dev->attr.u._4g.pfn_event_notify) {
                aw_net_sts_4g_evt_t evt;
                evt.devid = p_4g->dev_id;
                evt.type  = AW_NET_STS_4G_EVT_TYPE_NET_OK_FIRST;
                p_dev->attr.u._4g.pfn_event_notify(&evt);
            }
        } break;

        case __4G_THREAD_STAT_LINK_INET_CHK:
        case __4G_THREAD_STAT_LINK_SNET_CHK: {
            p_4g->flag_delay = AW_TRUE;
            if (0) {
                break;
            }

            /* If the check IP is not set, it is directly set to accessible
             * Internet.  */
            if (strlen(p_dev->attr.p_chk_inet_addr) == 0 &&
                    strlen(p_dev->attr.p_chk_snet_addr) == 0) {
                aw_assert(!__low_task_is_busy(p_4g));
                p_4g->status = __4G_THREAD_STAT_LINK_INET_CHK;
                break;
            }

            /*
             * The ping test is repeated in this state, and the state value
             * is not changed until it fails.
             */

            ret = __low_task_request(p_4g, __4G_LOW_TASK_REQ_IP_CHK);
            aw_assert(ret != -AW_EBUSY);
            if (ret == AW_OK) {
                if (p_4g->low_task_result.ret == AW_OK) {
                    if (p_dev->attr.u._4g.pfn_event_notify) {
                        aw_net_sts_4g_evt_t evt;
                        evt.devid = p_4g->dev_id;
                        evt.type  = AW_NET_STS_4G_EVT_TYPE_NET_OK;
                        p_dev->attr.u._4g.pfn_event_notify(&evt);
                    }
                } else {
                    if (p_dev->p_ctl) {
                        evt.type    = AW_NET_STS_EVT_TYPE_LINK_DOWN;
                        evt.u.link.p_dev   = p_dev;
                        evt.u.link.p_netif = p_dev->p_netif;
                        evt.u.link.prioty  = p_dev->attr.prioty;
                        aw_net_sts_evt_proc(p_dev->p_ctl, &evt);

                        p_4g->status = __4G_THREAD_STAT_SOFT_RESET;
                        __LOG_PFX(__LOG_NET_STS,
                                  "Reported a LINK_DOWN event to network "
                                  "controller. (if:%s)", p_dev->attr.netif_name);
                    }

                    if (p_dev->attr.u._4g.pfn_event_notify) {
                        aw_net_sts_4g_evt_t evt;
                        evt.devid = p_4g->dev_id;
                        evt.type  = AW_NET_STS_4G_EVT_TYPE_NET_ERR;
                        p_dev->attr.u._4g.pfn_event_notify(&evt);
                    }
                }
            }
        } break;

        case __4G_THREAD_STAT_SOFT_RESET: {
            p_4g->flag_delay = AW_TRUE;
            ret = __low_task_request(p_4g, __4G_LOW_TASK_REQ_SRESET);
            aw_assert(ret != -AW_EBUSY);
            if (ret == AW_OK) {
                if (p_4g->low_task_result.ret == AW_OK) {
                    p_4g->status = __4G_THREAD_STAT_IP_CHK_FIRST;

                    if (p_dev->attr.u._4g.pfn_event_notify) {
                        aw_net_sts_4g_evt_t evt;
                        evt.devid = p_4g->dev_id;
                        evt.type  = AW_NET_STS_4G_EVT_TYPE_SOFT_RESET;
                        p_dev->attr.u._4g.pfn_event_notify(&evt);
                    }
                } else {
                    p_4g->status = __4G_THREAD_STAT_POWER_OFF;
                }
            }

        } break;

        case __4G_THREAD_STAT_ABORT: {
            p_4g->flag_delay = AW_TRUE;
        } break;
    }

    return;
}

aw_local void __task_entry (void *p_arg)
{
    aw_net_sts_dev_t    *p_dev = p_arg;
    __net_sts_4g_dev_t  *p_4g = AW_CONTAINER_OF(p_dev, __net_sts_4g_dev_t, dev);
    __4g_thread_status_t    stat;
    __LOG_DEV_DEF(          p_dev);
    aw_tick_t               tick;

    for (;;) {
        aw_mdelay(10);
        AW_SEMB_TAKE(p_dev->task_sem, AW_SEM_WAIT_FOREVER);
        __task(p_dev);
        stat = p_4g->status;
        AW_SEMB_GIVE(p_dev->task_sem);

        __LOG_PFX(0, "status:%d", stat);

        /**
         * �������⴦����ʱ��⣬�� suspend ʱ���ܿ��ٻ�ȡ��;
         * ���� low_task �����״̬��Ҫ��ʱ��
         */
        if (p_4g->flag_delay) {
            __TIMEOUT_MSEC (tick, p_dev->attr.chk_ms) {
                aw_tick_t cur_tick = aw_sys_tick_get();
                aw_assert(p_dev->attr.chk_ms >=
                          __CHK_MS_RECTIFY(p_dev->attr.chk_ms));

                /* ʹ���ź�ֵ��ʹ��ʱʱ�ܱ�����ȥ���� evt */
                AW_SEMB_TAKE(p_4g->wakeup, tick - cur_tick);
                if (p_4g->evt_flags[0]) {
                    __evt_process(p_4g);
                }
                /* ���Ѻ�ֱ���˳���low_task ����ʹ�øû��������� task ����  */
                break;
            }
            p_4g->flag_delay = AW_FALSE;
        }

        if (p_4g->evt_flags[0]) {
            __evt_process(p_4g);
        }
    }
}


#if __4G_STS_FAST_RECOVERRY_EN
aw_local aw_err_t __startup (aw_net_sts_dev_t *p_dev)
{
    __net_sts_4g_dev_t  *p_4g = AW_CONTAINER_OF(p_dev, __net_sts_4g_dev_t, dev);

    return __4g_task_evt_add(p_4g, __EVT_FLAG_STARTUP, NULL);
}

aw_local aw_err_t __suspend (aw_net_sts_dev_t *p_dev)
{
    __net_sts_4g_dev_t  *p_4g = AW_CONTAINER_OF(p_dev, __net_sts_4g_dev_t, dev);

    if (p_4g->evt_flags[0]) {
        return -AW_EBUSY;
    }
    return __4g_task_evt_add(p_4g, __EVT_FLAG_SUSPEND, NULL);
}

aw_local aw_err_t __reinit (aw_net_sts_dev_t        *p_dev,
                            aw_net_sts_dev_init_t   *p_init)
{
    __net_sts_4g_dev_t  *p_4g = AW_CONTAINER_OF(p_dev, __net_sts_4g_dev_t, dev);
    __4g_task_evt_params_t   evt_params;

    if (p_4g->evt_flags[0]) {
        return -AW_EBUSY;
    }

    evt_params.reinit.attr = *p_init;
    return __4g_task_evt_add(p_4g, __EVT_FLAG_REINIT, &evt_params);
}


#else
aw_local void __startup (aw_net_sts_dev_t *p_dev)
{
    __LOG_DEV_DEF(p_dev);
    __LOG_PFX(1, "task startup :%s", p_dev->attr.task_name);

    AW_SEMB_GIVE(p_dev->task_sem);
}

aw_local void __suspend (aw_net_sts_dev_t *p_dev)
{
    __net_sts_4g_dev_t  *p_4g = AW_CONTAINER_OF(p_dev, __net_sts_4g_dev_t, dev);
    aw_net_sts_evt_t     evt;

    __LOG_DEV_DEF(p_dev);
    __LOG_PFX(1, "task suspend :%s", p_dev->attr.task_name);

    /*
     * �ȴ�ִ���굱ǰ�ļ������, ��4G���� �ϵ�ʱ��ȴ��ϳ�ʱ��;
     */
    AW_SEMB_TAKE(p_dev->task_sem, AW_SEM_WAIT_FOREVER);
    switch (p_4g->status) {
        case __4G_THREAD_STAT_LINK_INET:
        case __4G_THREAD_STAT_LINK_SNET: {
            /*
             * ��״̬ʱ����δ���� link up���ȴ��´� startup ��ֱ�ӷ���
             */
        } break;

        case __4G_THREAD_STAT_LINK_INET_CHK:
        case __4G_THREAD_STAT_LINK_SNET_CHK: {
            /*
             * �� link down�����޸�״̬ Ϊ OK �������� startup ��ֱ���ϱ� link up
             */
            evt.u.link.type    = AW_NET_STS_EVT_TYPE_LINK_DOWN;
            evt.u.link.p_dev   = p_dev;
            evt.u.link.p_netif = p_dev->p_netif;
            evt.u.link.prioty  = p_dev->attr.prioty;
            aw_net_sts_evt_proc(p_dev->p_ctl, &evt);

            p_4g->status = __4G_THREAD_STAT_IP_CHK_FIRST;
        } break;
        default :break;
    }
}
aw_local aw_err_t __reinit (aw_net_sts_dev_t *p_dev,
                            aw_net_sts_dev_init_t *p_init)
{
    __net_sts_4g_dev_t  *p_4g = AW_CONTAINER_OF(p_dev, __net_sts_4g_dev_t, dev);
    aw_net_sts_evt_t     evt;

    __suspend(p_dev);

#if LWIP_ROUTE_TABLE
    /* ɾ�� ping ʱ��ӵ�·���� */
    {
        ip_addr_t                ip_addr;
        ip_addr_t                src_ip_addr;

        src_ip_addr.type            = IPADDR_TYPE_V4;
        src_ip_addr.u_addr.ip4.addr =
                (in_addr_t)p_dev->p_netif->ip_addr.u_addr.ip4.addr;

        ip_addr.type            = IPADDR_TYPE_V4;
        ip_addr.u_addr.ip4.addr = p_4g->dest_addr.s_addr;

        netif_route_del(&src_ip_addr, &ip_addr);
    }
#endif /* LWIP_ROUTE_TABLE */

    p_dev->attr = *p_init;

    switch (p_4g->status) {
        case __4G_THREAD_STAT_LINK_INET:
        case __4G_THREAD_STAT_LINK_SNET:
        {
            /* Ϊ��״̬ʱ����δ���� LINK UP �¼�����Ҫ���¼��ip  */
            p_4g->status = __4G_THREAD_STAT_IP_CHK_FIRST;
        } break;

        case   __4G_THREAD_STAT_LINK_INET_CHK:
        case   __4G_THREAD_STAT_LINK_SNET_CHK: {
            /*
             * link down �����޸�״̬ Ϊ OK �������� startup ��ֱ���ϱ� link up
             */
            evt.u.link.type    = AW_NET_STS_EVT_TYPE_LINK_DOWN;
            evt.u.link.p_dev   = p_dev;
            evt.u.link.p_netif = p_dev->p_netif;
            evt.u.link.prioty  = p_dev->attr.prioty;
            aw_net_sts_evt_proc(p_dev->p_ctl, &evt);

            p_4g->status = __4G_THREAD_STAT_IP_CHK_FIRST;
        } break;

        default: break;
    }
    __startup(p_dev); /* �������� */

    return AW_OK;
}
#endif /* __4G_STS_FAST_RECOVERRY_EN */


aw_net_sts_dev_t * aw_net_sts_4g_dev_create (aw_net_sts_dev_init_t *p_init)
{
    __net_sts_4g_dev_t  *p_4g;
    aw_net_sts_dev_t    *p_dev = NULL;

    p_4g = malloc(sizeof(__net_sts_4g_dev_t));
    aw_assert(p_4g);
    memset(p_4g, 0, sizeof(*p_4g));

    p_4g->status        = __4G_THREAD_STAT_POWER_ON;
    p_dev               = &p_4g->dev;
    p_dev->attr         = *p_init;
    p_dev->attr.chk_ms  = __CHK_MS_RECTIFY(p_dev->attr.chk_ms);
    p_dev->pfn_startup  = __startup;
    p_dev->pfn_suspend  = __suspend;
    p_dev->pfn_reinit   = __reinit;

    p_dev->attr.p_chk_inet_addr = p_4g->ip_buff[0];
    p_dev->attr.p_chk_snet_addr = p_4g->ip_buff[1];

    if (p_init->p_chk_inet_addr) {
        aw_assert(strlen(p_init->p_chk_inet_addr) < __HOST_NAME_MAXLEN - 1);
        strncpy(p_4g->ip_buff[0], p_init->p_chk_inet_addr, __HOST_NAME_MAXLEN);
    }
    if (p_init->p_chk_snet_addr) {
        aw_assert(strlen(p_init->p_chk_snet_addr) < __HOST_NAME_MAXLEN - 1);
        strncpy(p_4g->ip_buff[1], p_init->p_chk_snet_addr, __HOST_NAME_MAXLEN);
    }

    if (p_dev->attr.task_name[0] == 0) {
        aw_snprintf(p_dev->attr.task_name,
                    sizeof(p_dev->attr.task_name),
                    "net_sts:%s",
                    p_dev->attr.netif_name);
    }

    AW_MUTEX_INIT(p_4g->evt_mutex, AW_SEM_Q_FIFO);

    /* ʹ����Ϣ����ֹͣ���񣬲�ֱ��ʹ�� AW_TASK_TERMINATE */
    AW_SEMB_INIT(p_dev->task_sem, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);
    AW_SEMB_INIT(p_4g->wakeup, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);

#if __4G_STS_FAST_RECOVERRY_EN
    AW_SEMB_GIVE(p_dev->task_sem);
#endif /* __4G_STS_FAST_RECOVERRY_EN */

    aw_snprintf(p_4g->low_task_name,
                sizeof(p_4g->low_task_name),
                "%s_low",
                p_dev->attr.task_name);

    /* �ͼ��������ڴ�����������  */
    p_4g->low_task_req = __4G_LOW_TASK_REQ_NONE;
    AW_TASK_INIT(p_4g->low_task,
                 p_4g->low_task_name,
                 15 + p_dev->attr.prioty,
                 AW_NET_STS_TASK_STACK_SIZE,
                 __low_task_entry,
                 (void*)p_dev);
    AW_TASK_STARTUP(p_4g->low_task);

    /* 4g ������������ */
    AW_TASK_INIT(p_dev->task,
                 p_dev->attr.task_name,
                 10 + p_dev->attr.prioty,
                 AW_NET_STS_TASK_STACK_SIZE,
                 __task_entry,
                 (void*)p_dev);
    AW_TASK_STARTUP(p_dev->task);

    return p_dev;
}


/* end of file */

