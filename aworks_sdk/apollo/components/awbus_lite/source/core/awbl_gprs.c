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

/**
 * \file
 * \brief GPRS interface implementation
 *
 * \internal
 * \par modification history:
 * - 1.00 2017.11.29  vih, first implementation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/
#include "aworks.h"
#include "awbus_lite.h"
#include "aw_delay.h"
#include "aw_vdebug.h"

#include "driver/gprs/gb2312_unicode_table.h"
#include "awbl_gprs.h"
#include "lwip/sockets.h"
#include "lwip/err.h"
#include "lwip/tcp.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "lwip/opt.h"
#include "lwip/netifapi.h"

#include "string.h"

#include "aw_ping.h"
#include "aw_time.h"
/******************************************************************************/
#define __LOG_FCOLOR_RED        "\x1B[31m"
#define __LOG_FCOLOR_GREEN      "\x1B[32m"

#define __LOG_BASE(mask, color, fmt, ...)   \
    if ((mask)) {\
        if (p_gprs->pfn_log_out) \
            p_gprs->pfn_log_out(color, __FUNCTION__, __LINE__, \
                                fmt, ##__VA_ARGS__);        \
    }
#define __LOG_BASE_ERR   __LOG_BASE

#define __LOG(mask, fmt, ...)  __LOG_BASE(mask, NULL, fmt, ##__VA_ARGS__)
#define __LOG_PFX(mask, fmt, ...) \
    __LOG_BASE(mask, NULL, "[%s:%d] " fmt "\n", \
               __func__, __LINE__, ##__VA_ARGS__)

#define __LOG_RES(is_true, mask, fmt, ...) \
    if (is_true) {\
        __LOG_BASE(mask, __LOG_FCOLOR_GREEN, "[ OK ] "fmt"\n", ##__VA_ARGS__);\
    } else {\
        __LOG_BASE_ERR(__MASK_ERR, \
                       __LOG_FCOLOR_RED,   \
                       "[ FAILED ] [%s:%d] " fmt "\n", \
                       __func__, \
                       __LINE__ , \
                       ##__VA_ARGS__);\
    }
#define __ASSERT_DETAIL(cond, fmt, ...) \
    if (!(cond)) {\
        __LOG_BASE_ERR(1, __LOG_FCOLOR_RED,\
                   "[ ASSERT Failed ][%s:%d] " #cond "\n", \
                   __func__, __LINE__); \
        if (strlen(fmt) != 0) {\
            __LOG_BASE_ERR(1, NULL,"    [ DETAIL ] " fmt "\n", \
                       ##__VA_ARGS__); \
        } \
        while(1); \
    }
#define __WARN_DETAIL_IF(cond, fmt, ...)  \
    if ((cond)) {\
        __LOG_BASE_ERR(1, __LOG_FCOLOR_RED, \
                   "[ WARN ][%s:%d] " #cond "\n",\
                   __func__, __LINE__); \
        if (strlen(fmt) != 0) {\
            __LOG_BASE_ERR(1, NULL,"    [ DETAIL ] " fmt "\n", ##__VA_ARGS__);\
        }\
    }
#define __RETURN_DETAIL_IF(cond, ret_exp, fmt, ...)  \
    if ((cond)) {\
        __LOG_BASE_ERR(1, __LOG_FCOLOR_RED, \
                   "[ ERROR:%d ][%s:%d] " #cond "\n", \
                   (int)(ret_exp), __func__, __LINE__); \
        if (strlen(fmt) != 0) {\
            __LOG_BASE_ERR(1, NULL, "    [ DETAIL ] " fmt "\n", ##__VA_ARGS__); \
        }\
        return (ret_exp); \
    }

#define __EXIT_DETAIL_IF(cond, fmt, ...)  \
    if ((cond)) {\
        __LOG_BASE_ERR(1, AW_SHELL_FCOLOR_RED, \
                   "[ ERROR ][%s:%d] " #cond "\n", __func__, __LINE__); \
        if (strlen(fmt) != 0) {\
            __LOG_BASE_ERR(1, NULL,"    [ DETAIL ] " fmt "\n", ##__VA_ARGS__);\
        }\
        return; \
    }
#define __GOTO_EXIT_DETAIL_IF(cond, ret_val, fmt, ...) \
    if ((cond)) {\
        __LOG_BASE_ERR(1, __LOG_FCOLOR_RED, \
                   "[ ERROR:%d ][%s:%d] " #cond "\n", \
                   (int)(ret_val), __func__, __LINE__); \
        ret = (ret_val); \
        if (strlen(fmt) != 0) {\
            __LOG_BASE_ERR(1, NULL,"    [ DETAIL ] " fmt "\n", ##__VA_ARGS__);\
        }\
        goto exit; \
    }

#define __ASSERT(cond)                __ASSERT_DETAIL(cond, "")
#define __WARN_IF(cond)               __WARN_DETAIL_IF(cond, "")
#define __RETURN_IF(cond, ret_exp)    __RETURN_DETAIL_IF(cond, ret_exp, "")
#define __EXIT_IF(cond)               __EXIT_DETAIL_IF(cond, "");
#define __GOTO_EXIT_IF(cond, ret_val) __GOTO_EXIT_DETAIL_IF(cond, ret_val, "")

/******************************************************************************/
#define __MASK_ERR              1
#define __MASK_DBG              1
#define __MASK_INF              1
#define __MASK_NET_PING         1
#define __DBG_MASK_TRACE        1
#define __DBG_MASK_AT_RECV      2
#define __DBG_MASK_AT_SEND      1
#define __DBG_MASK_AT_RECV_AP   0
/******************************************************************************/
#define __DBG_TRACE(fmt, ...)  __LOG(__DBG_MASK_TRACE, fmt, ##__VA_ARGS__)
/******************************************************************************/
#ifdef __VIH_DEBUG
#define __DEBUG_SECTION(cond) if (cond)
#else
#define __DEBUG_SECTION(cond) if (0)
#endif
/******************************************************************************/
#define __TIMEOUT_SEC(tick_tmp, timeout) for (tick_tmp = aw_sys_tick_get() + \
                                              aw_ms_to_ticks((timeout)*1000); \
                                              aw_sys_tick_get() < tick_tmp; )

#define __TIMEOUT_MSEC(tick_tmp, timeout) for (tick_tmp = aw_sys_tick_get() + \
                                               aw_ms_to_ticks(timeout); \
                                               aw_sys_tick_get() < tick_tmp; )

/******************************************************************************/
aw_local aw_err_t __gprs_ppp_close (struct awbl_gprs_dev *p_gprs);
/*aw_local void __ppp_link_status_cb (void *ctx, int err_code, void *arg); */
aw_local aw_err_t __gprs_ppp_open (struct awbl_gprs_dev *p_gprs);
/*****************************************************************************/


AWBL_METHOD_DEF(awbl_gprs_dev_get, "awbl_gprs_dev_get_name");

/** \brief pointer to first gprs service */
aw_local struct awbl_gprs_dev *__gp_gprs_dev_head = NULL;

/**
 * \brief allocate gprs service from instance (helper)
 *
 * \param[in]       p_dev   the current processing  instance
 * \param[in,out]   p_arg   argument
 *
 * \retval          AW_OK
 *
 * \see awbl_alldevs_method_run()
 */
aw_local aw_err_t __gprs_serv_alloc_helper (struct awbl_dev *p_dev, void *p_arg)
{
    awbl_method_handler_t      pfunc_gprs_dev  = NULL;
    struct awbl_gprs_dev      *p_gprs_dev       = NULL;

    /* find handler */

    pfunc_gprs_dev = awbl_dev_method_get(p_dev,
                                         AWBL_METHOD_CALL(awbl_gprs_dev_get));

    /* call method handler to allocate GPRS service */
    if (pfunc_gprs_dev != NULL) {

        pfunc_gprs_dev(p_dev, &p_gprs_dev);

        /* found a gprs service, insert it to the service list */

        if (p_gprs_dev != NULL) {
            struct awbl_gprs_dev **pp_serv_cur = &__gp_gprs_dev_head;

            while (*pp_serv_cur != NULL) {
                pp_serv_cur = &(*pp_serv_cur)->p_next;
            }

            *pp_serv_cur = p_gprs_dev;
            p_gprs_dev->p_next = NULL;
        }
    }

    return AW_OK;   /* iterating continue */
}

/**
 * \brief allocate gprs services from instance tree
 */
aw_local aw_err_t __gprs_serv_alloc (void)
{
    /* todo: count the number of service */

    /* Empty the list head, as every gprs device will be find */
    __gp_gprs_dev_head = NULL;

    awbl_dev_iterate(__gprs_serv_alloc_helper, NULL, AWBL_ITERATE_INSTANCES);

    return AW_OK;
}

/**
 * \brief find out a device who accept the id
 */
aw_local struct awbl_gprs_dev * __gprs_id_to_dev (int id)
{
    struct awbl_gprs_dev *p_serv_cur = __gp_gprs_dev_head;

    while ((p_serv_cur != NULL)) {

        if (id == p_serv_cur->dev_id) {

            return p_serv_cur;
        }

        p_serv_cur = p_serv_cur->p_next;
    }

    return NULL;
}

void awbl_gprs_init (void)
{
    /* allocate gprs services from instance tree */

    __gprs_serv_alloc();
}
/*****************************************************************************/
static void __4g_log_handler ( void         *p_color,
                               const char   *func_name,
                               int           line,
                               const char   *fmt, ...)
{
    return;
}
/*****************************************************************************/
/**
 * gprs 事件上报
 */
aw_local aw_err_t __evt_cb_array_notify (struct awbl_gprs_dev *p_gprs,
                                         aw_gprs_evt_t        *p_evt)
{
    int                  i;
    aw_gprs_evt_cb_t     pfn_evt_cb;

    /* 保存最新的事件 */
    p_gprs->last_evt = *p_evt;

    switch (p_evt->type) {

        /* 上报 PWR_OFF_PRE 事件，需要检查 retval 值均为 OK 才可继续 */
        case AW_GPRS_EVT_TYPE_PWR_OFF_PRE: {

            for (i = 0; i < AWBL_GPRS_EVT_CB_MAX; i++) {
                pfn_evt_cb = p_gprs->pfn_evt_cb_array[i];
                if (pfn_evt_cb != NULL) {
                    p_evt->u.pwr_off_pre.retval = AW_OK;
                    pfn_evt_cb(p_gprs->dev_id, p_evt);

                    /* 检查 retval 不为 OK 则退出 */
                    if (p_evt->u.pwr_off_pre.retval != AW_OK) {
                        return -AW_EBUSY;
                    }
                }
            }
        } break;

        /* 上报 RESET_PRE 事件，需要检查 retval 值均为 OK 才可继续 */
        case AW_GPRS_EVT_TYPE_RESET_PRE: {
            for (i = 0; i < AWBL_GPRS_EVT_CB_MAX; i++) {
                pfn_evt_cb = p_gprs->pfn_evt_cb_array[i];
                if (pfn_evt_cb != NULL) {
                    p_evt->u.reset_pre.retval = AW_OK;
                    pfn_evt_cb(p_gprs->dev_id, p_evt);

                    /* 检查 retval 不为 OK 则退出 */
                    if (p_evt->u.reset_pre.retval != AW_OK) {
                        return -AW_EBUSY;
                    }
                }
            }
        } break;

#if AWBL_GPRS_LL_TYPE_ETH_ENABLE
        case AW_GPRS_EVT_TYPE_ETH_LINK_UP: {
            p_evt->u.eth_link_up.ip      = p_gprs->ethernet.ethernetaddr.our_ipaddr;
            p_evt->u.eth_link_up.netmask = p_gprs->ethernet.ethernetaddr.netmask;
            p_evt->u.eth_link_up.gateway = p_gprs->ethernet.ethernetaddr.gateway;
            p_evt->u.eth_link_up.dns     = p_gprs->ethernet.ethernetaddr.dns1;
            for (i = 0; i < AWBL_GPRS_EVT_CB_MAX; i++) {
                pfn_evt_cb = p_gprs->pfn_evt_cb_array[i];
                if (pfn_evt_cb != NULL) {
                    pfn_evt_cb(p_gprs->dev_id, p_evt);
                }
            }
        } break;
#endif /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */
        /*
         * 只上报事件，无其它处理
         */
        case AW_GPRS_EVT_TYPE_PWR_ON:
        case AW_GPRS_EVT_TYPE_PWR_OFF:
        case AW_GPRS_EVT_TYPE_RESET_COMPLETED:
        case AW_GPRS_EVT_TYPE_ETH_LINK_DOWN:
        case AW_GPRS_EVT_TYPE_PPP_LINK_UP:
        case AW_GPRS_EVT_TYPE_PPP_LINK_DOWN: {
            for (i = 0; i < AWBL_GPRS_EVT_CB_MAX; i++) {
                pfn_evt_cb = p_gprs->pfn_evt_cb_array[i];
                if (pfn_evt_cb != NULL) {
                    pfn_evt_cb(p_gprs->dev_id, p_evt);
                }
            }
        } break;
    }

    return AW_OK;
}

/******************************************************************************/
aw_local aw_err_t __ioctl_req_at_trans (int                   id,
                                       aw_gprs_ioctl_args_t *p_arg)
{
    struct awbl_gprs_dev *p_gprs = __gprs_id_to_dev(id);
    aw_err_t              ret = -AW_EINVAL;

    if (p_arg->at_trans.p_at_cmd) {
        awbl_gprs_at_send(p_gprs, p_arg->at_trans.p_at_cmd);
    }

    ret = AW_OK;
    p_arg->at_trans.recvlen = 0;
    if (p_arg->at_trans.p_recv_buf) {
        aw_assert(p_arg->at_trans.bufsize > 0);

        if (p_arg->at_trans.p_recv_expect) {
            char    *arg[1];
            char     buf[256];

            arg[0] = buf;
            if (sizeof(buf) <= strlen(p_arg->at_trans.p_recv_expect)) {
                /* The expected response is too long. */
                return -AW_ENOMEM;
            }
            memcpy(buf, p_arg->at_trans.p_recv_expect, 256);
            ret = awbl_gprs_at_recv(p_gprs,
                                    (char *)p_arg->at_trans.p_recv_buf,
                                    p_arg->at_trans.bufsize,
                                    arg,
                                    1,
                                    p_arg->at_trans.recv_tm);
        } else {
            ret = awbl_gprs_at_recv(p_gprs,
                                    (char *)p_arg->at_trans.p_recv_buf,
                                    p_arg->at_trans.bufsize,
                                    NULL,
                                    0,
                                    p_arg->at_trans.recv_tm);
        }

        if (ret >= 0) {
            p_arg->at_trans.recvlen = strlen((char *)p_arg->at_trans.p_recv_buf);
            ret = AW_OK;
        }
    }
    return ret;
}

aw_local aw_err_t __ioctl_evt_cb_set (int                   id,
                                      aw_gprs_ioctl_args_t *p_arg)
{
    struct awbl_gprs_dev *p_gprs = __gprs_id_to_dev(id);
    aw_err_t              ret = -AW_EINVAL;
    aw_gprs_evt_t         evt;
    int                   i  = 0 ;

    /* 检查是否已设置过该回调 */
    for (i = 0; i < AWBL_GPRS_EVT_CB_MAX; i++) {
        if (p_gprs->pfn_evt_cb_array[i] == p_arg->pfn_evt_handler) {
            return -AW_EEXIST;
        }
    }

    /* 查找为 NULL 的项，保存回调指针   */
    ret = -AW_ENOMEM;
    for (i = 0; i < AWBL_GPRS_EVT_CB_MAX; i++) {
        if (p_gprs->pfn_evt_cb_array[i] == NULL) {
            p_gprs->pfn_evt_cb_array[i] = p_arg->pfn_evt_handler;
            ret = AW_OK;
            break;
        }
    }

    if (ret == AW_OK) {
        /* 设置回调后，立即上报当前事件 */
        if (p_gprs->last_evt.type != AW_GPRS_EVT_TYPE_NONE) {
            p_arg->pfn_evt_handler(id, &p_gprs->last_evt);
        }
    }

    return ret;
}

/*****************************************************************************/
#if 0
static int __at_test (struct awbl_gprs_dev *p_gprs, int trys)
{
    int ret;
    char    *arg[1];
    char     buf[16];
    char     rsp[3];

    if (trys == 0) {
        return -AW_EINVAL;
    }

    while (trys--) {
        arg[0] = rsp;
        memcpy(rsp, "OK", 3);
        memset(buf, 0, 16);

        awbl_gprs_at_send(p_gprs, "AT\r");
        ret = awbl_gprs_at_recv(p_gprs, buf, 16, arg, 1, 1000);
        if (ret == AW_OK) {
            return ret;
        }
    }

    return -AW_ENOMSG;
}
#endif

#ifdef __VIH_DEBUG
/**
 * \brief   获取运行时间
 *
 * \param   opt = 1: 启动   opt = 0 ：结束并打印运行时间
 * \param   p_last_time ：用于保存计时时的时间
 *
 * \return  结束计算时返回计算的时间(ms)
 */
static void __get_run_time (char opt, u32_t *p_last_ms)
{
    aw_timespec_t timespec;
    u32_t         ms = 0;

    aw_timespec_get(&timespec);
    ms = timespec.tv_nsec / 1000000 + timespec.tv_sec * 1000;

    if (opt == 0) {
        AW_INFOF(("\n[USE TIME] %d mS \n", ms - (*p_last_ms)));
        AW_INFOF(("--------------------------------------------------\n"));
    }
    *p_last_ms = ms;    /* 保存毫秒 */

}
#else
#define __get_run_time(a,b)
#endif

/*****************************************************************************/
extern struct netif *netif_list;
aw_local struct netif *__gprs_netif_get (void)
{
    struct netif *netif = NULL;

    for(netif = netif_list; netif != NULL; netif = netif->next) {
        if (memcmp(netif->name, "pp", 2) == 0) {
            break;
        }
    }
    return netif;
}

#if AWBL_GPRS_USE_LWIP_1_4

/** \brief  PPP 链路状态回调函数   */
aw_local void __ppp_link_status_cb (void *ctx, int err_code, void *arg)
{
    struct awbl_gprs_dev *p_this = (struct awbl_gprs_dev *)ctx;
    struct awbl_gprs_dev *p_gprs = p_this;
    struct netif *netif = NULL;

    if (err_code == PPPERR_NONE) {

        netif = __gprs_netif_get();
        if (arg) {
            char _buf[32];
            char _buf1[32];

            p_this->ppp.p_pppaddr = (struct ppp_addrs *)arg;
#if LWIP_DNS_FILETER
            if (dns_server_filter(netif, &p_this->ppp.p_pppaddr->dns1)) {
                dns_setserver(0, &p_this->ppp.p_pppaddr->dns1);
            }
#else
            dns_setserver(0, &p_this->ppp.p_pppaddr->dns1);
#endif
            dns_setserver(1, &p_this->ppp.p_pppaddr->dns2);

            ipaddr_ntoa_r(&p_this->ppp.p_pppaddr->dns1, _buf, 32);
            ipaddr_ntoa_r(&p_this->ppp.p_pppaddr->dns2, _buf1, 32);
            __DBG_TRACE("set dns server [%s] [%s].\n", _buf, _buf1);
        }

        p_this->ppp.phase = AWBL_GPRS_PPP_PHASE_RUNNING;

        if (netif) {
            netif_set_default(netif);
        }

    } else if (err_code == PPPERR_USER ||
               err_code == PPPERR_PROTOCOL) {
        __DBG_TRACE("__ppp_link_status_cb: ppp phase is DEAD.\n ");
        AW_SEMB_GIVE(p_this->ppp.link_close);

        __gprs_ppp_close(p_this);
    }
}
#endif /* AWBL_GPRS_USE_LWIP_1_4 */


#if AWBL_GPRS_USE_LWIP_2_X
/*
 * PPP status callback
 * ===================
 *
 * PPP status callback is called on PPP status change (up, down, …) from lwIP
 * core thread
 */

/* PPP status callback example */
static void __ppp_link_status_cb (ppp_pcb *pcb, int err_code, void *ctx)
{
    struct awbl_gprs_dev *p_this = (struct awbl_gprs_dev *)ctx;
    struct awbl_gprs_dev *p_gprs = p_this;
    struct netif *pppif = ppp_netif(pcb);
    LWIP_UNUSED_ARG(ctx);

    (void)pppif;

    switch(err_code) {
        case PPPERR_NONE: {
#if LWIP_DNS
            const ip_addr_t *ns;

            (void)ns;
#endif /* LWIP_DNS */

#if !LWIP_ROUTE_TABLE
            netif_set_default(pppif);
#endif

            p_this->ppp.phase = AWBL_GPRS_PPP_PHASE_RUNNING;
            __DBG_TRACE("__ppp_link_status_cb: Connected\n");

#if PPP_IPV4_SUPPORT
            __DBG_TRACE("   our_ipaddr  = %s\n", ipaddr_ntoa(&pppif->ip_addr));
            __DBG_TRACE("   his_ipaddr  = %s\n", ipaddr_ntoa(&pppif->gw));
            __DBG_TRACE("   netmask     = %s\n", ipaddr_ntoa(&pppif->netmask));
#if LWIP_DNS && 0
            /* ipcp_up() 中会设置 dns */
            ns = dns_getserver(0);
            __DBG_TRACE("   dns1        = %s\n", ipaddr_ntoa(ns));
            ns = dns_getserver(1);
            __DBG_TRACE("   dns2        = %s\n", ipaddr_ntoa(ns));
#endif /* LWIP_DNS */
#endif /* PPP_IPV4_SUPPORT */
#if PPP_IPV6_SUPPORT
            __DBG_TRACE("   our6_ipaddr = %s\n", ip6addr_ntoa(netif_ip6_addr(pppif, 0)));
#endif /* PPP_IPV6_SUPPORT */
            break;
        }
        case PPPERR_PARAM: {
            __DBG_TRACE("status_cb: Invalid parameter\n");
            break;
        }
        case PPPERR_OPEN: {
            __DBG_TRACE("status_cb: Unable to open PPP session\n");
            break;
        }
        case PPPERR_DEVICE: {
            __DBG_TRACE("status_cb: Invalid I/O device for PPP\n");
            break;
        }
        case PPPERR_ALLOC: {
            __DBG_TRACE("status_cb: Unable to allocate resources\n");
            break;
        }
        case PPPERR_USER: {
            __DBG_TRACE("status_cb: User interrupt\n");
            break;
        }
        case PPPERR_CONNECT: {
            __DBG_TRACE("status_cb: Connection lost\n");
            break;
        }
        case PPPERR_AUTHFAIL: {
            __DBG_TRACE("status_cb: Failed authentication challenge\n");
            break;
        }
        case PPPERR_PROTOCOL: {
            __DBG_TRACE("status_cb: Failed to meet protocol\n");
            break;
        }
        case PPPERR_PEERDEAD: {
            __DBG_TRACE("status_cb: Connection timeout\n");
            break;
        }
        case PPPERR_IDLETIMEOUT: {
            __DBG_TRACE("status_cb: Idle Timeout\n");
            break;
        }
        case PPPERR_CONNECTTIME: {
            __DBG_TRACE("status_cb: Max connect time reached\n");
            break;
        }
        case PPPERR_LOOPBACK: {
            __DBG_TRACE("status_cb: Loopback detected\n");
            break;
        }
        default: {
            __DBG_TRACE("status_cb: Unknown error code %d\n", err_code);
            break;
        }
    }

    /*
     * This should be in the switch case, this is put outside of the switch
     * case for example readability.
     */
    if (err_code == PPPERR_NONE) {
        return;
    }

    /* ppp_close() was previously called, don't reconnect */
    if (err_code == PPPERR_USER) {
        /* ppp_free(); -- can be called here */
        return;
    }

    /*
     * Try to reconnect in 30 seconds, if you need a modem chatscript you have
     * to do a much better signaling here ;-)
     */
    /* ppp_connect(pcb, 30); */

    /* OR ppp_listen(pcb); */
}


/*
 * PPPoS serial output callback
 *
 * ppp_pcb, PPP control block
 * data, buffer to write to serial port
 * len, length of the data buffer
 * ctx, optional user-provided callback context pointer
 *
 * Return value: len if write succeed
 */
static u32_t __pppos_output_cb(ppp_pcb *pcb, u8_t *data, u32_t len, void *ctx)
{
    struct awbl_gprs_dev *p_gprs = (struct awbl_gprs_dev *)ctx;

    __DEBUG_SECTION(0) {
        __LOG_PFX(1, "output len:%d", len);
    }
    return sio_write(p_gprs->ppp.sio_mgn.fd, data, len);
}

static void __ppp_input_thread (void *arg)
{
    struct awbl_gprs_dev    *p_gprs = (struct awbl_gprs_dev *)arg;
    ppp_pcb                 *p_pcb  = p_gprs->ppp.p_pcb;
    uint32_t                 len;
    int                      ret;
    uint32_t                 rxbuf_size;
    aw_gprs_evt_t            evt;
    
    len = sizeof(p_gprs->ppp.rxbuf);
    __ASSERT(len > (PPP_MRU + PPP_HDRLEN));
    __ASSERT((len & (len - 1)) == 0);

    rxbuf_size = 1024;
    __ASSERT((rxbuf_size & (rxbuf_size - 1)) == 0);
    __ASSERT(rxbuf_size <= sizeof(p_gprs->ppp.rxbuf));
    

    while (p_pcb->phase != PPP_PHASE_DEAD) {
#if 0
        __ASSERT(p_gprs->ppp.sio_mgn.fd != NULL);
#else
        if (p_gprs->ppp.sio_mgn.fd == NULL) {
            __WARN_IF(p_gprs->ppp.sio_mgn.fd == NULL);
            break;
        }
#endif

        len = sio_read(p_gprs->ppp.sio_mgn.fd,
                       p_gprs->ppp.rxbuf,
                       rxbuf_size);

        if (len > 0) {
            __DEBUG_SECTION(0) {
                __LOG_PFX(1, "enter len:%d", len);
            }
            ret = pppos_input_tcpip(p_pcb, p_gprs->ppp.rxbuf, len);
            if (ret != ERR_OK) {
                __LOG_PFX(1, "[ERROR] ret:%d", ret);
            }
            __DEBUG_SECTION(0) {
                __LOG_PFX(1, "exit");
            }
        } else {
            __DEBUG_SECTION(0) {
                __LOG_PFX(1, "delay");
            }
            aw_mdelay(1);
        }
    }

    /* sys_thread_delete(PPP_THREAD_PRIO); */

    AW_SEMB_GIVE(p_gprs->ppp.thread_close);

    __DBG_TRACE("__ppp_input_thread(%d): terminate %c%c%d input threaad\n",
                __LINE__, p_gprs->ppp.netif.name[0], p_gprs->ppp.netif.name[1], p_gprs->ppp.netif.num);

    AW_TASK_TERMINATE(p_gprs->ppp.thread);

    evt.type = AW_GPRS_EVT_TYPE_PPP_LINK_DOWN;
    __evt_cb_array_notify(p_gprs, &evt);

    /* 加锁处理 ppp 关闭 */
    ret = AW_MUTEX_LOCK(p_gprs->mutex, aw_ms_to_ticks(10000));
    if (ret == AW_OK) {
        __gprs_ppp_close(p_gprs);
        AW_MUTEX_UNLOCK(p_gprs->mutex);
    } else {
        aw_assert(p_gprs->ppp.phase < AWBL_GPRS_PPP_PHASE_RUNNING);
    }
}
#endif /* AWBL_GPRS_USE_LWIP_2_X */

#if AWBL_GPRS_LL_TYPE_ETH_ENABLE

/* GPRS设备打开以太网函数 ---*
 * p_gprs ：GPRS设备结构体
 * 返回             ：成功返回AW_OK*/
aw_local aw_err_t __gprs_ethernet_open (struct awbl_gprs_dev *p_gprs)
{
    aw_err_t ret;
    int      csq;

    /* 检查以太网拨号函数*/
    if (p_gprs->ethernet.pfn_dial_down == NULL) {
        return -AW_EPERM;
    }
    /* 先检测信号强度 */
    __LOG_PFX(1, "* Checking Ethernet device signal strength,PLease Wait...\r\n");
    ret = p_gprs->p_serv->pfn_gprs_ioctl(p_gprs, AW_GPRS_REQ_CSQ_GET, (void *)&csq);
    if (ret != AW_OK || csq == 99 || csq < 9) {
        __LOG_PFX(1, "* Ethernet device signal strength : %d\r\n",csq);
        return -AW_EPERM;
    }
    __LOG_PFX(1, "* Ethernet device signal strength : %d\r\n",csq);
    /* 拨号*/
    ret = p_gprs->ethernet.pfn_dial_up(p_gprs);
    if (ret == AW_OK) {
        aw_gprs_evt_t evt;
        evt.type = AW_GPRS_EVT_TYPE_ETH_LINK_UP;
        __evt_cb_array_notify(p_gprs, &evt);

        p_gprs->ethernet.phase = AWBL_GPRS_ETHERNET_PHASE_DIAL_UP;
    } else {
        return ret;
    }
    return ret;
}

/* GPRS设备关闭以太网函数 ---*
 * p_gprs ：GPRS设备结构体
 * 返回             ：成功返回AW_OK*/
aw_local aw_err_t __gprs_ethernet_close (struct awbl_gprs_dev *p_gprs){
    aw_err_t ret;
    aw_gprs_evt_t evt;

    evt.type = AW_GPRS_EVT_TYPE_ETH_LINK_DOWN;
    __evt_cb_array_notify(p_gprs, &evt);

    /* 检查以太网状态*/
    if (p_gprs->ethernet.phase == AWBL_GPRS_ETHERNET_PHASE_DIAL_UP) {
        ret = p_gprs->ethernet.pfn_dial_down(p_gprs);
        __WARN_DETAIL_IF(ret != AW_OK, "->pfn_dial_down()");
    }
    p_gprs->ethernet.phase = AWBL_GPRS_ETHERNET_PHASE_INIT;
    return AW_OK;
}
#endif /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */

aw_local aw_err_t __gprs_ppp_open (struct awbl_gprs_dev *p_gprs)
{
    aw_err_t ret;
    int      i;
    int      csq;

    if (p_gprs->ppp.pfn_dial_up == NULL) {
        return AW_EPERM;
    }

    if (p_gprs->ppp.phase == AWBL_GPRS_PPP_PHASE_RUNNING) {
        return AW_OK;
    }

    /* 先检测信号强度 */
    ret = p_gprs->p_serv->pfn_gprs_ioctl(p_gprs, AW_GPRS_REQ_CSQ_GET, (void *)&csq);
    __LOG_RES(csq >= p_gprs->csq_min && csq < 31, __MASK_INF,
              "check signal strength.(csq:%d, ret:%d)", csq, ret);
    if (ret != AW_OK || csq == 99 || csq < 9) {
        return -AW_EPERM;
    }

    if (p_gprs->ppp.phase < AWBL_GPRS_PPP_PHASE_INIT) {
#if AWBL_GPRS_USE_LWIP_1_4
        pppInit();
#endif /* AWBL_GPRS_USE_LWIP_1_4 */

        p_gprs->ppp.phase = AWBL_GPRS_PPP_PHASE_INIT;
    }

    if (p_gprs->ppp.phase < AWBL_GPRS_PPP_PHASE_DIAL_UP) {
        ret = p_gprs->ppp.pfn_dial_up(p_gprs);
        if (ret == AW_OK) {
            p_gprs->ppp.phase = AWBL_GPRS_PPP_PHASE_DIAL_UP;
        } else {
            return ret;
        }
    }

    if (p_gprs->ppp.phase < AWBL_GPRS_PPP_PHASE_CREATE) {

#if AWBL_GPRS_USE_LWIP_1_4
        p_gprs->ppp.ppp_fd = pppOpen((void *)(uint32_t)p_gprs->ppp.sio_mgn.fd,
                                        __ppp_link_status_cb,
                                         (void*)p_gprs);
        __RETURN_DETAIL_IF(p_gprs->ppp.ppp_fd < 0, -AW_EPERM, "pppOpen()");

        p_gprs->ppp.phase = AWBL_GPRS_PPP_PHASE_CONNECT;
#endif /* AWBL_GPRS_USE_LWIP_1_4 */

#if AWBL_GPRS_USE_LWIP_2_X
        p_gprs->ppp.p_pcb = pppos_create(&p_gprs->ppp.netif, 
                                         __pppos_output_cb, 
                                         __ppp_link_status_cb, 
                                         p_gprs);

        __RETURN_DETAIL_IF(p_gprs->ppp.p_pcb == NULL, -AW_EPERM, "pppos_create()");
        p_gprs->ppp.phase = AWBL_GPRS_PPP_PHASE_CREATE;
#endif /* AWBL_GPRS_USE_LWIP_2_X */
    }

#if AWBL_GPRS_USE_LWIP_2_X
    ret = ppp_connect(p_gprs->ppp.p_pcb, 0);
    __RETURN_DETAIL_IF(ret != AW_OK, ret, "ppp_connect()");

    p_gprs->ppp.phase = AWBL_GPRS_PPP_PHASE_CONNECT;

    AW_TASK_INIT(p_gprs->ppp.thread,
                 PPP_THREAD_NAME,
                 PPP_THREAD_PRIO,
                 PPP_THREAD_STACKSIZE,
                 __ppp_input_thread,
                 p_gprs);
    AW_TASK_STARTUP(p_gprs->ppp.thread);
#endif /* AWBL_GPRS_USE_LWIP_2_X */

    /* 等待 ppp 连接成功 */
    for (i = 0; i < 300; i++) {
        if (p_gprs->ppp.phase == AWBL_GPRS_PPP_PHASE_RUNNING) {
            aw_gprs_evt_t evt;
            evt.type = AW_GPRS_EVT_TYPE_PPP_LINK_UP;
            __evt_cb_array_notify(p_gprs, &evt);
            return AW_OK;
        }
        aw_mdelay(100);
    }

    __gprs_ppp_close(p_gprs);

    return -AW_EPERM;
}

aw_local aw_err_t __gprs_ppp_close (struct awbl_gprs_dev *p_gprs)
{
    aw_err_t ret;

    if (p_gprs->ppp.pfn_dial_down == NULL) {
        return AW_EPERM;
    }

    if (p_gprs->ppp.phase <= AWBL_GPRS_PPP_PHASE_INIT) {
        return AW_OK;
    }

    if (p_gprs->ppp.phase == AWBL_GPRS_PPP_PHASE_RUNNING) {

#if AWBL_GPRS_USE_LWIP_1_4
        pppClose(p_gprs->ppp.ppp_fd);
#endif /* AWBL_GPRS_USE_LWIP_1_4 */

#if AWBL_GPRS_USE_LWIP_2_X
        ppp_close(p_gprs->ppp.p_pcb, 0);
#endif /* AWBL_GPRS_USE_LWIP_2_X */

        p_gprs->ppp.phase = AWBL_GPRS_PPP_PHASE_CONNECT;
    }

    if (p_gprs->ppp.phase == AWBL_GPRS_PPP_PHASE_CONNECT) {

#if AWBL_GPRS_USE_LWIP_1_4
        /* 
         * 需要等待 ppp 协议栈调用 __ppp_link_status_cb 回调释放 lock，否则，
         * 未释放锁时一关闭模组会导致 ppp_fd 管道为 NULL，ppp 使用该管道引起崩溃
         */
        ret = AW_SEMB_TAKE(p_gprs->ppp.link_close, aw_ms_to_ticks(30000));
        __WARN_DETAIL_IF(ret != AW_OK, "AW_SEMB_TAKE(link_close)");

#endif /* AWBL_GPRS_USE_LWIP_1_4 */

#if AWBL_GPRS_USE_LWIP_2_X
        /* 等待ppp线程关闭的信号量 */
        ret = AW_SEMB_TAKE(p_gprs->ppp.thread_close, aw_ms_to_ticks(30000));
        __WARN_DETAIL_IF(ret != AW_OK, "AW_SEMB_TAKE(thread_close)");
#endif /* AWBL_GPRS_USE_LWIP_2_X */

        p_gprs->ppp.phase = AWBL_GPRS_PPP_PHASE_CREATE;
    }

    if (p_gprs->ppp.phase == AWBL_GPRS_PPP_PHASE_CREATE) {
#if AWBL_GPRS_USE_LWIP_2_X
        ppp_free(p_gprs->ppp.p_pcb);
#endif /* AWBL_GPRS_USE_LWIP_2_X */

        p_gprs->ppp.phase = AWBL_GPRS_PPP_PHASE_DIAL_UP;
    }

    if (p_gprs->ppp.phase == AWBL_GPRS_PPP_PHASE_DIAL_UP) {
        ret = p_gprs->ppp.pfn_dial_down(p_gprs);
        __WARN_DETAIL_IF(ret != AW_OK, "->pfn_dial_down()");
    }

    p_gprs->ppp.phase = AWBL_GPRS_PPP_PHASE_INIT;

    return ret;
}
/*****************************************************************************/
void awbl_gprs_at_send (struct awbl_gprs_dev     *p_this, char *p_str)
{
    struct awbl_gprs_dev *p_gprs = p_this;
    if (p_this->p_serv->pfn_at_cmd_send != NULL) {
        p_this->p_serv->pfn_at_cmd_send(p_this, p_str);

        __LOG(__DBG_MASK_AT_SEND,
                       "awbl_gprs_at_send(%d): %s\n",
                       strlen(p_str), 
                       p_str);
    }
}

int awbl_gprs_at_recv (struct awbl_gprs_dev     *p_this,
                       char                     *p_buf,
                       uint32_t                  buf_size,
                       char                     *p_cond[],
                       uint32_t                  arg_num,
                       uint32_t                  wait_ms)
{
    uint32_t    j      = 0;
    uint32_t    offset = 0;
    int         cnt   = 0;
    int         ret;
    aw_tick_t   tick_tmp;
    struct awbl_gprs_dev *p_gprs = p_this;

#ifdef __VIH_DEBUG
    u32_t run_time = 0;
    if (__DBG_MASK_AT_RECV) {
        __get_run_time(1, &run_time);
    }
#endif
    if (p_this->p_serv->pfn_at_cmd_recv == NULL) {
        return -AW_ENOTSUP;
    }
    memset(p_buf, 0, buf_size);

    ret = -AW_ETIMEDOUT;
    __TIMEOUT_MSEC(tick_tmp, wait_ms) {
        /* gprs模块可能回返回一些上报的数据  */

        /* __LOG_PFX(1, "pfn_at_cmd_recv [ start ]"); */
        cnt = p_this->p_serv->pfn_at_cmd_recv(p_this,
                                              p_buf + offset, 
                                              buf_size - offset);
        /* __LOG_PFX(1, "pfn_at_cmd_recv [ finish ]"); */

        /* 接收到数据 */
        if (cnt > 0) {
            /* 没有指定要接收的数据，则直接退出 */
            if (p_cond == NULL || arg_num == 0) {
                return AW_OK;
            }
            offset += cnt;
            if (offset >= buf_size) {
                offset = 0;
            }

            j = 0;

            if (offset == cnt) {
                __LOG_PFX(__DBG_MASK_AT_RECV && 0, " ");
            }
            __LOG(__DBG_MASK_AT_RECV, "%s", (offset ? p_buf + offset - cnt : 0));
            while (j < arg_num) {
                if (strstr(p_buf, p_cond[j]) != 0) {
                    ret = j;
                    goto exit;      /* 获取成功，退出*/
                }
                if (strstr(p_buf, p_this->p_info->p_at_err)) {
                    ret = -AW_ENOMSG;
                    goto exit;
                }
                j++;
            }
        } else {
            aw_mdelay(100);
        }
    }

exit:
    if (ret >= 0) {
        __LOG(__DBG_MASK_AT_RECV_AP, "awbl_gprs_at_recv(%d): %s [OK]\n", cnt, p_buf);
    }

#ifdef __VIH_DEBUG
    if (__DBG_MASK_AT_RECV) {
        __get_run_time(0, &run_time);
    }
#endif

    return ret;
}

/*****************************************************************************/
/** @brief 把GSM基本字符集转换成ASCII字符集, 不能转换的用' '代替 */
aw_const uint8_t gsm_basic_ascii[256]={\
/*    0     1     2    3     4     5     6     7     8     9     A     B     C      D    E     F   */
/*0*/ 64,  156,   36, 157,  138,  130,  151,  141,  149,  128,   10,  237,  ' ',   13,  143,  134,\
/*1*/' ',   95,  232, 226,  ' ',  234,  252,  ' ',  228,  233,  ' ',   27,  146,  145,  225,  144,\
/*2*/ 32,   33,   34,  35,  ' ',   37,   38,   39,   40,   41,   42,   43,   44,   45,   46,   47,\
/*3*/ 48,   49,   50,  51,   52,   53,   54,   55,   56,   57,   58,   59,   60,   61,   62,   63,\
/*4*/173,   65,   66,  67,   68,   69,   70,   71,   72,   73,   74,   75,   76,   77,   78,   79,\
/*5*/ 80,   81,   82,  83,   84,   85,   86,   87,   88,   89,   90,  142,  153,  165,  154,  ' ',\
/*6*/168,   97,   98,  99,  100,  101,  102,  103,  104,  105,  106,  107,  108,  109,  110,  111,\
/*7*/112,  113,  114, 115,  116,  117,  118,  119,  120,  121,  122,  132,  148,  164,  129,  133 \
};

/** @brief 把GSM扩展字符集转换成ASCII字符集, 不能转换的用' '代替 */
aw_const uint8_t gsm_extern_ascii[256]={\
/*    0     1     2    3     4     5     6     7     8     9     A     B     C      D    E     F   */
/*0*/ 64,  156,   36, 157,  138,  130,  151,  141,  149,  128,   10,  237,  ' ',   13,  143,  134,\
/*1*/' ',   95,  232, 226,   94,  234,  252,  ' ',  228,  233,  ' ',  ' ',  146,  145,  225,  144,\
/*2*/ 32,   33,   34,  35,  ' ',   37,   38,   38,  123,  125,   42,   43,   44,   45,   46,   92,\
/*3*/ 48,   49,   50,  51,   52,   53,   54,   55,   56,   57,   58,   59,   91,  126,   93,   63,\
/*4*/124,   65,   66,  67,   68,   69,   70,   71,   72,   73,   74,   75,   76,   77,   78,   79,\
/*5*/ 80,   81,   82,  83,   84,   85,   86,   87,   88,   89,   90,  142,  153,  165,  154,  ' ',\
/*6*/168,   97,   98,  99,  100,  101,  102,  103,  104,  105,  106,  107,  108,  109,  110,  111,\
/*7*/112,  113,  114, 115,  116,  117,  118,  119,  120,  121,  122,  132,  148,  164,  129,  133 \
};


/** \brief  查找国标码   */
aw_local uint16_t gb_search(uint16_t gb)
{
    uint16_t i;
    uint16_t max_index = AW_NELEMENTS(gb2312_unicode_table);

    for (i=0; i<max_index; i++) {
        if (gb2312_unicode_table[i][0] == gb) {
            return gb2312_unicode_table[i][1];
        }
    }
    return 0x0020;      /* 表中查找不到返回空格*/
}

/** \brief  把国标码转换为Unicode编码   */
aw_local uint16_t gb2unicode(uint8_t *pgb, uint16_t *puni)
{
    uint16_t gb;
    uint16_t len = 0;

    while (*pgb) {
        if (*pgb < 0x80) {
            *puni++ = gb_search(*pgb);
            pgb++;
            len++;
        } else {
            gb  = (*pgb++ << 8) & 0xFF00;
            gb |= *pgb++;
            *puni++ = gb_search(gb);
            len++;
        }
    }
    return len;
}

/** \brief  查找Unicode编码   */
aw_local uint16_t unicode_search(uint16_t uni)
{
    uint16_t i;
    uint16_t max_index = AW_NELEMENTS(gb2312_unicode_table);

    for (i=0; i<max_index; i++) {
        if (gb2312_unicode_table[i][1] == uni) {
            return gb2312_unicode_table[i][0];
        }
    }
    return 0x0020;      /* 表中查找不到返回空格*/
}

/** \brief  把Unicode编码转换为国标码   */
aw_local void unicode2gb(uint16_t *puni, uint8_t *pgb)
{
    uint16_t gb;

    while (*puni) {
        gb = unicode_search(*puni);
        if (gb < 0x100) {
            *pgb++ = gb & 0xff;
        } else {
            *pgb++ = (gb>>8) & 0xff;
            *pgb++ = gb & 0xff;
        }
        puni++;
    }
}

/** \brief 把数字1~15转换成字符'1','2','3',....'A','B','C','D','E','F' */
aw_local char ctoh(uint8_t num)
{
    if (num > 15) {
        return 0;
    } else if(num >= 10) {
        return (num+0x37);
    } else {
        return (num+0x30);
    }
}

/** \brief 把8位的数值转换成两个16进制数字符 */
aw_local void ctox(uint8_t num, char *cnum)
{
    cnum[0] = ctoh(num/16);
    cnum[1] = ctoh(num%16);
}

/** \brief 把两个16进制数字符转换成8位的数值 */
aw_local uint8_t xtoc(char *cnum)
{
    uint8_t a=0;

    if ((cnum[0]>='A') && (cnum[0]<='F')) {
        a = (cnum[0]-55)*16;
    } else if ((cnum[0]>='a') && (cnum[0]<='f')) {
        a = (cnum[0]-87)*16;
    } else if ((cnum[0]>='0') && (cnum[0]<='9')) {
        a = (cnum[0]-0x30)*16;
    } else {
        return 0;
    }

    if ((cnum[1]>='A') && (cnum[1]<='F')) {
        a += cnum[1]-55;
    } else if ((cnum[1]>='a') && (cnum[1]<='f')) {
        a += cnum[1]-87;
    } else if ((cnum[1]>='0') && (cnum[1]<='9')) {
        a += cnum[1]-0x30;
    } else {
        return 0;
    }

    return a;
}


/** \brief 把GSM编码转换成ASCII编码   */
aw_local uint16_t gsm2ascii(uint8_t *pgsm, uint8_t *pascii, uint16_t len)
{
    uint16_t i, j;
    uint8_t esc=0;
    uint16_t ret = len;

    for (i=0, j=0; i<len; i++) {
        if (pgsm[i] == 0x1B) {
            esc = 1;
            ret--;
        } else {
            if (esc == 1) {
                esc = 0;
                pascii[j++] = gsm_extern_ascii[pgsm[i]];
            } else {
                pascii[j++] = gsm_basic_ascii[pgsm[i]];
            }
        }
    }
    return ret;
}

/** \brief 超长短信7位解码   */
aw_local void sms_7bit_decode_for_super(char *cmsg, uint8_t len, char *msg)
{
    aw_const uint8_t mask[]={0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};
    uint8_t i, j, k;
    uint8_t  mess[170]={0};
    uint8_t tmp=0;
    uint8_t pdu_len;
    uint8_t data;

    pdu_len = (len-1)-(len-1)/8+1;

    data = xtoc(cmsg);;
    cmsg += 2;
    mess[0] = (data>>1) & 0x7f;
    j=1;
    k=1;
    for (i=1; i<pdu_len; i++) {
        data = xtoc(cmsg);
        cmsg += 2;
        if (i%7 == 0) {
            mess[k++] = ((data<<j)|((tmp>>(8-j))&mask[j])) & 0x7f;
            mess[k++] = (data>>1) & 0x7f;
            continue;
        }
        if ((i-1)%7 == 0) {
            mess[k++] = data & 0x7f;
            tmp = data;
            j=1;
            continue;
        }
        mess[k++] = ((data<<j)|((tmp>>(8-j))&mask[j])) & 0x7f;
        tmp = data;
        j++;
    }
    gsm2ascii(mess, (uint8_t *)msg, len);
}

/** \brief 普通短信7位解码   */
aw_local void sms_7bit_decode_for_normal(char *cmsg, uint8_t len, char *msg)
{
    aw_const uint8_t mask[]={0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};
    uint8_t i, j, k;
    uint8_t mess[170]={0};
    uint8_t tmp=0;
    uint8_t pdu_len;
    uint8_t data;

    pdu_len = len-len/8;

    j=1;
    k=0;
    for (i=0; i<pdu_len; i++) {
        data = xtoc(cmsg);
        cmsg += 2;
        if ((i+1)%7 == 0) {
            mess[k++] = ((data<<j)|((tmp>>(8-j))&mask[j])) & 0x7f;
            mess[k++] = (data>>1) & 0x7f;
            continue;
        }
        if (i%7 == 0) {
            mess[k++] = data & 0x7f;
            tmp = data;
            j=1;
            continue;
        }
        mess[k++] = ((data<<j)|((tmp>>(8-j))&mask[j])) & 0x7f;
        tmp = data;
        j++;
    }
    gsm2ascii(mess, (uint8_t *)msg, len);
}

/*
 * dst：目的地址
 * src：源地址
 * dpos：目的字节的位偏移
 * spos：源字节的位偏移
 * copy_bits：要拷贝的位数
 */
#define __BIT_COPY(dst, dpos, src, spos, copy_bits)     \
do {                                                    \
    if (dpos == 0) {                                    \
        *dst = ((*src) >> spos);                        \
    } else if (spos == 0) {                             \
        *dst |= ((*src) << dpos);                       \
    }                                                   \
} while (0)


/** \brief 将 7bit 的 ascii 码存到字节中,如8个字符可压缩为7个字节 */
static int __7bit_to_byte (uint8_t *p_dst, int dst_size, char *p_src, int src_len)
{
    int dpos = 0;
    int spos = 0;
    int dsize = 0;
    int ssize = 0;
    uint8_t *dtmp;
    char    *stmp;

    int len;

    len = ((src_len * 7) + 7) / 8;

    if (len > dst_size) {
        return -AW_ENOMEM;
    }

    dtmp = p_dst;
    stmp = p_src;
    dsize = 8;
    ssize = 7;

    memset(p_dst , 0, dst_size);

    while (1) {

        if (dsize > ssize) {
            /* dtmp 字节中bit空间大于 stmp 中的，则将 ssize 全存入 */
            __BIT_COPY(dtmp, dpos, stmp, spos, ssize);

            dpos    = dpos + ssize;
            dsize   = dsize - ssize;

            stmp    = stmp + 1;
            spos    = 0;
            ssize   = 7;
        } else if (dsize == ssize) {
            /* 剩余空间刚好相等 */
            __BIT_COPY(dtmp, dpos, stmp, spos, dsize);

            dtmp    = dtmp + 1;
            dpos    = 0;
            dsize   = 8;

            stmp    = stmp + 1;
            spos    = 0;
            ssize   = 7;
        } else if ((dsize < ssize)) {
            /* stmp 中剩余的bit较多 */
            __BIT_COPY(dtmp, dpos, stmp, spos, dsize);

            spos    = spos + dsize;
            ssize   = ssize - dsize;

            dtmp    = dtmp + 1;
            dpos    = 0;
            dsize   = 8;
        }

        /* 判断 src 中的字符是否全存入了 dst，dst 最后一个字符剩余的 bit 全填0 */
        if (stmp == p_src + src_len) {
            return len;
        }
    }

    return len;
}

/**
 * UTF-8转ucs2编码
 */
uint32_t utf8_to_ucs2 (const char   *utf8,
                       int           utf8_length,
                       char         *ucs2,
                       int           ucs2_length)
{
    if((utf8_length < 1) || (utf8_length > ucs2_length - 1)){
        return 0;
    }
    if((utf8 == NULL) || (ucs2 == NULL)){
        return 0;
    }

    uint32_t utf8_pos = 0;
    uint32_t ucs2_pos = 0;

    while(1){
        if((utf8[utf8_pos] & 0x80) == 0){
            ucs2[ucs2_pos] = 0;
            ucs2[ucs2_pos + 1] = utf8[utf8_pos] & 0x7f;
            utf8_pos += 1;
            ucs2_pos += 2;
        }
        else if((utf8[utf8_pos] & 0xe0) == 0){
            ucs2[ucs2_pos] = (utf8[utf8_pos] & 0x1f) >> 2;
            ucs2[ucs2_pos + 1] = ((utf8[utf8_pos] & 0x03) << 6) |
                    (utf8[utf8_pos + 1] & 0x3f);
            utf8_pos += 2;
            ucs2_pos += 2;
        }
        else if((utf8[utf8_pos] & 0xf0) == 0xe0){
            ucs2[ucs2_pos] = ((utf8[utf8_pos] & 0x0f) << 4) |
                    ((utf8[utf8_pos + 1] & 0x3f) >> 2);
            ucs2[ucs2_pos + 1] = ((utf8[utf8_pos + 1] & 0x03) << 6) |
                    (utf8[utf8_pos + 2] & 0x3f);
            utf8_pos += 3;
            ucs2_pos += 2;
        }
        else{
            return 0;
        }

        if((utf8_pos >= utf8_length) || (ucs2_pos >= ucs2_length - 1)){
            ucs2[ucs2_pos] = '\0';
            break;
        }
    }
    return ucs2_pos;
}

/**
 * UCS2转UTF-8编码
 */
uint32_t ucs2_to_utf8 (const char   *ucs2,
                       int           ucs2_length,
                       char         *utf8,
                       int           utf8_length)
{
    if((utf8_length < 1) || (utf8_length - 1 < ucs2_length)){
        return 0;
    }
    if((utf8 == NULL) || (ucs2 == NULL)){
        return 0;
    }

    uint32_t utf8_pos = 0;
    uint32_t ucs2_pos = 0;

    while(1){
        if((ucs2[ucs2_pos] == 0x00) && (ucs2[ucs2_pos + 1] <= 0x7f)){
            utf8[utf8_pos] = ucs2[ucs2_pos + 1] & 0x7f;
            ucs2_pos += 2;
            utf8_pos += 1;
        }
        else if((ucs2[ucs2_pos] & 0xf8) == 0){
            utf8[utf8_pos] = 0xc0 | ((ucs2[ucs2_pos] & 0x07) << 2) |
                    ((ucs2[ucs2_pos + 1] & 0xc0) >> 6);
            utf8[utf8_pos + 1] = 0x80 | (ucs2[ucs2_pos + 1] & 0x3f);
            ucs2_pos += 2;
            utf8_pos += 2;
        }
        else{
            utf8[utf8_pos] = 0xe0 | ((ucs2[ucs2_pos] & 0xf0) >> 4);
            utf8[utf8_pos + 1] = 0x80 | ((ucs2[ucs2_pos] & 0x0f) <<2) |
                    ((ucs2[ucs2_pos + 1] & 0xc0) >> 6);
            utf8[utf8_pos + 2] = 0x80 | (ucs2[ucs2_pos + 1] & 0x3f);
            ucs2_pos += 2;
            utf8_pos += 3;
        }
        if((ucs2_pos >= ucs2_length) || ((utf8_pos >= utf8_length - 1))){
            utf8[utf8_pos] = '\0';
            break;
        }
    }
    return utf8_pos;
}

aw_err_t awbl_gprs_pdu_pack (char     *message,
                             size_t    msg_size,
                             char     *p_sca,
                             uint8_t  *p_sms,
                             uint8_t   len,
                             char     *p_num,
                             uint8_t   code)
{
    aw_err_t ret;
    char num_tmp[50]={0};
    char num[50]={0};

    char plen[3]={0};
    uint16_t headlen;
    int i;
    aw_const char *p_dcs_code;
    aw_const char PHONE_END[]="F";              /* 手机号码填充 */
    aw_const char PDU_TYPE_S[]="1100";          /* PDU Type 表示单条短信 */
    aw_const char PID[]="00";                   /* PID 固定为00 */
    aw_const char CODE_ASC[]="00";              /* DCS 表示7位编码 */
    aw_const char CODE_HEX[]="04";              /* DCS 表示HEX编码(8位编码) */
    aw_const char CODE_UCS2[]="08";          /* DCS 表示 UCS2 编码 */
    aw_const char VP[]="FF";                    /* VP 短信有效时间 */

    if (msg_size < 370) {
        return -AW_ENOMEM;
    }
    (void)ret;

    strncpy(num_tmp, p_num, sizeof(num_tmp)-1);
    if (num_tmp[0] == '+') {                        /* 目标号码为国际号码  */

        ctox((strlen(num_tmp)-1), num);
        num[2] = '9';
        num[3] = '1';

        if ((strlen(num_tmp)-1)%2 != 0) {  /* 号码除了'+'后为奇数，需要在号码后面添加F */
            strcat(num_tmp, PHONE_END);
        }

        for (i=0; i<strlen(num_tmp)-1; i+=2) {
            num[4+i] = num_tmp[2+i];
            num[5+i] = num_tmp[1+i];
        }
        num[4+strlen(num_tmp)-1] = 0;
    } else {                                        /* 目标号码为国家号码  */

        ctox((strlen(num_tmp)), num);
        num[2] = '8';
        num[3] = '1';

        if (strlen(num_tmp)%2 != 0) {               /* 号码为奇数，需要在号码后面添加F  */
            strcat(num_tmp, PHONE_END);
        }

        for (i=0; i<strlen(num_tmp); i+=2) {
            num[4+i] = num_tmp[1+i];
            num[5+i] = num_tmp[i];
        }
        num[4+strlen(num_tmp)] = 0;
    }
    memset(message, 0, msg_size);
    strcat(message, "00");                          /* SCA length */
    strcat(message, PDU_TYPE_S);                    /* PDU Type */
    strcat(message, num);                           /* DA */
    strcat(message, PID);                           /* PID */

    if (code == AW_GPRS_SMS_CODE_7BIT) {
        p_dcs_code = CODE_ASC;
    } else if (code == AW_GPRS_SMS_CODE_8BIT) {
        p_dcs_code = CODE_HEX;
    } else if (code == AW_GPRS_SMS_CODE_UCS2 || code == AW_GPRS_SMS_CODE_UTF8) {
        p_dcs_code = CODE_UCS2;
    } else {
        aw_assert(AW_FALSE);
    }

    strcat(message, p_dcs_code);                    /* DCS 指定编码方式 */
    strcat(message, VP);                            /* VP 短信有效时间 */

    if (code == AW_GPRS_SMS_CODE_7BIT) {
        int sms_len = 0;
        uint8_t _buf[160];

        /* UDL 指定短信内容长度  */
        ctox(len, plen);
        strcat(message, plen);

        /* 处理 7bit 编码 */
        sms_len = __7bit_to_byte(_buf, sizeof(_buf), (char *)p_sms, len);

        headlen = strlen(message);
        for (i=0; i<sms_len; i++) {
            ctox((uint8_t)_buf[i], &message[headlen]);
            headlen+=2;
        }
    } else if (code == AW_GPRS_SMS_CODE_8BIT || code == AW_GPRS_SMS_CODE_UCS2) {
        /*  UDL 指定短信内容长度  */
        ctox(len, plen);
        strcat(message, plen);

        /* 处理 8bit 编码 */
        headlen = strlen(message);
        for (i=0; i<len; i++) {
            ctox((uint8_t)p_sms[i], &message[headlen]);
            headlen+=2;
        }
    } else if (code == AW_GPRS_SMS_CODE_UCS2 || code == AW_GPRS_SMS_CODE_UTF8) {

        /* 把utf8编码的短信内容转换成ucs2*/
        char ucs2_temp[256] = {0};

        len = utf8_to_ucs2((char *)p_sms, len, ucs2_temp, sizeof(ucs2_temp));
        if(len == 0){
            return -AW_EBADMSG;
        }

        /*  UDL 指定短信内容长度  */
        ctox(len, plen);
        strcat(message, plen);

        /* 处理 8bit 编码 */
        headlen = strlen(message);
        for (i = 0; i < len; i++) {
            ctox((uint8_t)ucs2_temp[i], &message[headlen]);
            headlen+=2;
        }
    }

    return AW_OK;
}

aw_err_t awbl_gprs_pdu_unpack (char                   *p_pdu_data,
                               struct aw_gprs_sms_buf *p_to_sms)
{
    aw_err_t  ret;
    uint8_t   i;
    char     *pret;
    uint16_t  len;
    uint16_t  pdu_remain_len;
    uint8_t   char_set;

    (void) ret;

    struct {
        struct {
            char    number[25];
            uint8_t len;
            char    type[2];
        } sca;

        uint8_t pdu_type;
        uint8_t mr;
        struct {
            uint8_t len;
            uint8_t type;
            char    number[13];
        } addr;             /* OA or DA */

        uint8_t pid;
        uint8_t dcs;
        char    scts[32];
        uint8_t vp;
        uint8_t udl;
        uint8_t udh;        /* */
        char    ud[512];
    } pdu_fields;

    pret           = p_pdu_data;
    pdu_remain_len = strlen(p_pdu_data);
    memset(&pdu_fields, 0, sizeof(pdu_fields));

    /*  解析 Service Center Address  */
    pdu_fields.sca.len = xtoc(pret);
    pret += 2;
    if (pdu_remain_len >= 2 + pdu_fields.sca.len * 2) {
        /* type :81 为国内， 91 为国外  */
        memcpy(pdu_fields.sca.type, pret, 2);
        pret += 2;
        pdu_remain_len -= 2;

        /* len 包含 type 的长度 */
        pdu_fields.sca.len--;

        aw_assert(sizeof(pdu_fields.sca.number) > pdu_fields.sca.len * 2);
        memcpy(pdu_fields.sca.number, pret, pdu_fields.sca.len * 2);
        pret += pdu_fields.sca.len * 2;
        pdu_remain_len  -= pdu_fields.sca.len * 2;
    } else {
        return -AW_EBADMSG;
    }

    /* 解析  pdu type */
    if (pdu_remain_len < 2) {
        return -AW_EBADMSG;
    }
    pdu_fields.pdu_type = xtoc(pret);
    pret += 2;
    pdu_remain_len -= 2;

    /* 解析 Originator Address */
    if (pdu_remain_len < 4) {
        return -AW_EBADMSG;
    }
    pdu_fields.addr.len     = xtoc(pret);               /*  短信号码长度   */
    pret += 2;
    pdu_fields.addr.type    = xtoc(pret);
    pret += 2;
    pdu_remain_len -= 4;

    len = pdu_fields.addr.len;
    if (pdu_remain_len < len+len%2) {
        return -AW_EBADMSG;
    }

    for(i=0; i<len; i+=2) {
        pdu_fields.addr.number[i]   = pret[i+1];
        pdu_fields.addr.number[i+1] = pret[i];
    }
    pdu_fields.addr.number[len] = 0;
    pret            += len+len%2;
    pdu_remain_len  -= len+len%2;
    strncpy(p_to_sms->p_num_buff, pdu_fields.addr.number, p_to_sms->num_buff_len);

    /* 解析 PID  */
    if (pdu_remain_len < 2) {
        return -AW_EBADMSG;
    }
    pdu_fields.pid = xtoc(pret);
    pret += 2;
    pdu_remain_len -= 2;

    /* 解析 DCS (DataCoding Scheme) */
    if (pdu_remain_len < 2) {
        return -AW_EBADMSG;
    }
    pdu_fields.dcs = xtoc(pret);
    pret += 2;
    pdu_remain_len -= 2;

    /* 解析 SCTS(Service Center Time Stamp) */
    if (pdu_remain_len < 14) {
        return -AW_EBADMSG;
    }
    pdu_fields.scts[0] = pret[1];          /*  年      */
    pdu_fields.scts[1] = pret[0];
    pdu_fields.scts[2] = '/';
    if (pdu_remain_len >= 2) {
        pret            += 2;
        pdu_remain_len  -= 2;
    } else {
        return -AW_EBADMSG;
    }
    pdu_fields.scts[3] = pret[1];          /*  月     */
    pdu_fields.scts[4] = pret[0];
    pdu_fields.scts[5] = '/';
    if (pdu_remain_len >= 2) {
        pret            += 2;
        pdu_remain_len  -= 2;
    } else {
        return -AW_EBADMSG;
    }
    pdu_fields.scts[6] = pret[1];          /*  日     */
    pdu_fields.scts[7] = pret[0];
    pdu_fields.scts[8] = ',';
    if (pdu_remain_len >= 2) {
        pret            += 2;
        pdu_remain_len  -= 2;
    } else {
        return -AW_EBADMSG;
    }
    pdu_fields.scts[9] = pret[1];          /*  时     */
    pdu_fields.scts[10] = pret[0];
    pdu_fields.scts[11] = ':';
    if (pdu_remain_len >= 2) {
        pret            += 2;
        pdu_remain_len  -= 2;
    } else {
        return -AW_EBADMSG;
    }
    pdu_fields.scts[12] = pret[1];         /*  分     */
    pdu_fields.scts[13] = pret[0];
    pdu_fields.scts[14] = ':';
    if (pdu_remain_len >= 2) {
        pret            += 2;
        pdu_remain_len  -= 2;
    } else {
        return -AW_EBADMSG;
    }
    pdu_fields.scts[15] = pret[1];         /*  秒     */
    pdu_fields.scts[16] = pret[0];
    pdu_fields.scts[17] = '+';
    if (pdu_remain_len >= 2) {
        pret            += 2;
        pdu_remain_len  -= 2;
    } else {
        return -AW_EBADMSG;
    }
    pdu_fields.scts[18] = pret[1];         /*  时区     */
    pdu_fields.scts[19] = pret[0];
    if (pdu_remain_len >= 2) {
        pret            += 2;
        pdu_remain_len  -= 2;
    } else {
        return -AW_EBADMSG;
    }
    strncpy(p_to_sms->p_time_buff, pdu_fields.scts, p_to_sms->time_buff_len);

    /* UDL (User Data Length : 1~140) */
    if (pdu_remain_len < 2) {
        return -AW_EBADMSG;
    }
    pdu_fields.udl = xtoc(pret);
    pret += 2;
    pdu_remain_len -= 2;

    /* UD */
    if (pdu_remain_len < pdu_fields.udl) {
        return -AW_EBADMSG;
    }
    aw_assert(sizeof(pdu_fields.ud) > pdu_fields.udl * 2);
    memcpy(pdu_fields.ud, pret, pdu_fields.udl * 2);

    /*
     * 处理 User Data 格式
     */

    /* 判断是否有 UDHI 标志（UD 中包含用户数据头信息） :
     * SMS 消息无用户数据头；EMS(增强消息业务)才有该标志，如图片、铃声、长短信等;
     */
    if ((pdu_fields.pdu_type & 0x40) != 0) {
        /* 暂不支持 UDHI 标志 */
        return -AW_ENOTSUP;
    }

    if ((pdu_fields.udl * 2) > p_to_sms->sms_buff_len) {
        return -AW_ENOMEM;
    }

    char_set = (pdu_fields.dcs >> 2) & 0x03;    /* 获取使用的字符集 */
    switch (char_set) {
        /* 7Bit */
        case 0: {
            char message[200];
            memset(message, 0, sizeof(message));
            sms_7bit_decode_for_normal(pdu_fields.ud, pdu_fields.udl, message);
            strncpy(p_to_sms->p_sms_buff, message, p_to_sms->sms_buff_len);
            p_to_sms->sms_code = AW_GPRS_SMS_CODE_8BIT;
        } break;

        /* 8Bit */
        case 1: {
            for(i = 0; i < pdu_fields.udl; i++) {
                p_to_sms->p_sms_buff[i] = xtoc(pdu_fields.ud + (2 * i));
            }
            p_to_sms->sms_code = AW_GPRS_SMS_CODE_8BIT;
        } break;

        /* UCS2 */
        case 2: {
            for(i = 0; i < pdu_fields.udl; i++) {
                p_to_sms->p_sms_buff[i] = xtoc(pdu_fields.ud + (2 * i));
            }
            p_to_sms->sms_code = AW_GPRS_SMS_CODE_UCS2;
        } break;

#if 0
        case 2: {
            int j;
            uint16_t unicode_tmp[70]={0};
            char message[200];

            for(i=0, j=0; i < pdu_fields.udl / 2; i++, j++) {
                unicode_tmp[j] = xtoc(pdu_fields.ud + 2 * i);
                unicode_tmp[j] = (unicode_tmp[j] << 8) & 0xFF00;
                unicode_tmp[j] |= xtoc(pdu_fields.ud + 2 * i + 1);
            }
            unicode2gb(unicode_tmp, (uint8_t *)message);
            strncpy(p_to_sms->p_sms_buff, message, p_to_sms->sms_buff_len);
            p_to_sms->sms_code = AW_GPRS_SMS_CODE_GB;
        } break;
#endif
    }

    return AW_OK;
}

/*****************************************************************************/
aw_err_t awbl_gprs_init_tail (struct awbl_gprs_dev *p_gprs)
{
    p_gprs->ppp.phase         = AWBL_GPRS_PPP_PHASE_UNINIT;

    if (!p_gprs->already_init) {
        p_gprs->already_init = AW_TRUE;

        if (p_gprs->ll_type == AWBL_GPRS_LL_TYPE_PPP) {
            p_gprs->ppp.sio_mgn.serial_cmd_state        = AW_FALSE;
            p_gprs->ppp.sio_mgn.flow_statistic_download = 0;
            p_gprs->ppp.sio_mgn.flow_statistic_upload   = 0;

            sys_sem_new(&p_gprs->ppp.sio_mgn.serial_abort_sem, 0);

            /* 添加ppp的管理器 */
            sio_list_add(&p_gprs->ppp.sio_mgn);

#if AWBL_GPRS_USE_LWIP_1_4
            AW_SEMB_INIT(p_gprs->ppp.link_close, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);
#endif /* AWBL_GPRS_USE_LWIP_1_4 */

#if AWBL_GPRS_USE_LWIP_2_X
            AW_SEMB_INIT(p_gprs->ppp.thread_close, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);
#endif /* AWBL_GPRS_USE_LWIP_2_X */
        }
    }

    /* 默认使用外部协议栈 */
    if (PPP_SUPPORT || p_gprs->ll_type == AWBL_GPRS_LL_TYPE_ETH) {
        p_gprs->use_external_protocol = AW_TRUE;
    } else {
        p_gprs->use_external_protocol = AW_FALSE;
    }

    p_gprs->socket_num = 0;
    p_gprs->power_on   = AW_FALSE;
    p_gprs->csq_min    = 10;

    if (p_gprs->pfn_log_out == NULL) {
        p_gprs->pfn_log_out =__4g_log_handler;
    }

    /* Initializes the type value of last_evt, whick will not be notified. */
    p_gprs->last_evt.type = AW_GPRS_EVT_TYPE_NONE;

    p_gprs->gnss_en = AW_FALSE;

    return AW_OK;
}
/*****************************************************************************/
aw_err_t aw_gprs_power_on (int   id)
{
    struct awbl_gprs_dev    *p_gprs = __gprs_id_to_dev(id);
    aw_err_t                 ret;
    aw_gprs_evt_t            evt;

    if (p_gprs == NULL) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_gprs->mutex, AW_SEM_WAIT_FOREVER);

    if (p_gprs->power_on ) {
        ret = AW_OK;
        goto exit;
    }
    if (p_gprs->p_serv->pfn_gprs_power_on == NULL) {
        ret = -AW_ENOTSUP;
        goto exit;
    }

    if (p_gprs->p_info->pfn_power_on) {
        p_gprs->p_info->pfn_power_on();
    }

    if (p_gprs->p_info->pfn_write_en) {
        p_gprs->p_info->pfn_write_en();
    }

    ret = p_gprs->p_serv->pfn_gprs_power_on(p_gprs);
    __WARN_DETAIL_IF(ret != AW_OK, "power on.");
    if (ret == AW_OK) {
        p_gprs->power_on = AW_TRUE;

        evt.type = AW_GPRS_EVT_TYPE_PWR_ON;
        __evt_cb_array_notify(p_gprs, &evt);
    }

exit:
    AW_MUTEX_UNLOCK(p_gprs->mutex);

    return ret;
}


aw_err_t aw_gprs_power_off (int   id)
{
    struct awbl_gprs_dev    *p_gprs = __gprs_id_to_dev(id);
    aw_err_t                 ret;
    aw_gprs_evt_t            evt;

    if (p_gprs == NULL) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_gprs->mutex, AW_SEM_WAIT_FOREVER);

    if (!p_gprs->power_on ) {
        ret = AW_OK;
        goto exit;
    }
    if (p_gprs->p_serv->pfn_gprs_power_off == NULL) {
        ret = -AW_ENOTSUP;
        goto exit;
    }

    /* 上报 PWR_OFF_PRE 事件，需要检查 retval 值均为 OK 才可继续 */
    evt.type = AW_GPRS_EVT_TYPE_PWR_OFF_PRE;
    ret = __evt_cb_array_notify(p_gprs, &evt);
    if (ret != AW_OK) {
        __LOG_PFX(1, "Power off failed because some devices were not allowed!");
        goto exit;
    }

    /* 需要关闭socket和ppp */
    __GOTO_EXIT_DETAIL_IF(p_gprs->socket_num != 0,
                          -AW_EPERM,
                          "need close socket.(socket num:%d)",
                          p_gprs->socket_num);

#if AWBL_GPRS_LL_TYPE_ETH_ENABLE
    if (p_gprs->ll_type == AWBL_GPRS_LL_TYPE_ETH) {
        ret =__gprs_ethernet_close(p_gprs);
    }
#endif /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */

    if (p_gprs->ll_type == AWBL_GPRS_LL_TYPE_PPP) {
        ret = __gprs_ppp_close(p_gprs);
    }

    ret = p_gprs->p_serv->pfn_gprs_power_off(p_gprs);
    __WARN_DETAIL_IF(ret != AW_OK, "device power off.");

    if (ret == AW_OK) {
        if (p_gprs->ll_type == AWBL_GPRS_LL_TYPE_PPP) {
            sys_sem_free(&p_gprs->ppp.sio_mgn.serial_abort_sem);
            sio_list_del(&p_gprs->ppp.sio_mgn);
        }

        p_gprs->already_init = AW_FALSE;
        p_gprs->power_on     = AW_FALSE;
    }

    if (p_gprs->p_info->pfn_power_off) {
        p_gprs->p_info->pfn_power_off();
    }
    p_gprs->status = AW_GPRS_STS_AT_ERROR;
    p_gprs->gnss_en = AW_FALSE;

    /* 上报 PWR_OFF 事件 */
    evt.type = AW_GPRS_EVT_TYPE_PWR_OFF;
    __evt_cb_array_notify(p_gprs, &evt);

exit:
    AW_MUTEX_UNLOCK(p_gprs->mutex);
    return ret;
}

int aw_gprs_start (int                 id,
                   const char         *p_mode,
                   char               *p_ip,
                   uint16_t            port,
                   uint32_t            timeout)
{
    struct awbl_gprs_dev *p_gprs = __gprs_id_to_dev(id);
    aw_err_t              ret;
    int                   csq = 0;
    int                   hd;
    int                   local_port = 50000;
    uint8_t               mode;

    if (p_gprs == NULL) {
        return -AW_EINVAL;
    }

    (void)csq;
    (void)local_port;

    AW_MUTEX_LOCK(p_gprs->mutex, AW_SEM_WAIT_FOREVER);

    if (!p_gprs->already_init ) {
        ret = -AW_EPERM;
        goto exit;
    }

    __DBG_TRACE("aw_gprs_start[%d]: proto:%s, mode:%s, ip:%s, port:%d \n",
                  id, (p_gprs->use_external_protocol? "external":"internal"),
                  p_mode, p_ip, port);

    /* 使用外部协议栈 */
    if (p_gprs->use_external_protocol) {
        struct sockaddr_in  server;
        struct sockaddr_in  client;
#if AWBL_GPRS_USE_LWIP_1_4
        int                 rcv_timeout = 10;
#endif
#if AWBL_GPRS_USE_LWIP_2_X
        struct timeval      rcv_timeout;
#endif
        int                 link_err    = 0;
        struct in_addr      addr;
        struct netif       *ppp_netif;

        (void)addr;
        (void)client;
        (void)ppp_netif;

#if AWBL_GPRS_LL_TYPE_ETH_ENABLE
        if (p_gprs->ll_type == AWBL_GPRS_LL_TYPE_ETH) {
            p_gprs->ethernet.phase = AWBL_GPRS_ETHERNET_PHASE_INIT;
            ret = __gprs_ethernet_open(p_gprs);
            if(ret == AW_OK) {
                __LOG_PFX(1, "* Ethernet device dail up successful\r\n");
            } else{
                __LOG_PFX(1, "* Ethernet device dail up fail\r\n");
                return ret;
            }
        }
#endif /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */

        if (p_gprs->ll_type == AWBL_GPRS_LL_TYPE_PPP) {
            if (p_gprs->ppp.phase != AWBL_GPRS_PPP_PHASE_RUNNING) {
                ret = __gprs_ppp_open(p_gprs);
                __WARN_DETAIL_IF(ret != AW_OK,  "ppp dial up.");
            }

            if (p_gprs->ppp.phase != AWBL_GPRS_PPP_PHASE_RUNNING) {
                AW_MUTEX_UNLOCK(p_gprs->mutex);
                return -AW_EPERM;
            }
        }

        while (1) {
            if (link_err != 0) {
                closesocket(hd);
            }
            if (link_err >= 1) {
                link_err = 0;
                ret = -AW_ETIMEDOUT;
                goto exit;
            }

            if (strcmp("TCP", p_mode) == 0) {
                hd = socket(AF_INET, SOCK_STREAM, 0);
            } else if (strcmp("UDP", p_mode) == 0) {
                hd = socket(AF_INET, SOCK_DGRAM, 0);
            } else {
                ret = -AW_ENOTSUP;
            }

            if (hd == -1) {
                link_err++;
                continue;
            }

#if AWBL_GPRS_USE_LWIP_2_X
            rcv_timeout.tv_sec  = 0;
            rcv_timeout.tv_usec = 30000;
#endif
            ret = setsockopt(hd, SOL_SOCKET, SO_RCVTIMEO, &rcv_timeout, sizeof(rcv_timeout));
            if (ret != 0) {
                link_err++;
                __DBG_TRACE("aw_gprs_start(%d)[%d]: set rec_timeout failed\n", __LINE__, id);
                continue;
            }

            memset(&server, 0, sizeof(struct sockaddr_in));
            server.sin_family = AF_INET;
            server.sin_port   = htons(port);

            /* 将字符串型的ip地址转换为32位的网络序列号 */
            ret = inet_aton(p_ip, &(server.sin_addr));
            if (ret == 0) {
                struct hostent * host;

                host = gethostbyname(p_ip);
                server.sin_addr = *((struct in_addr *)host->h_addr);
                __DBG_TRACE("aw_gprs_start[%d]: DNS: %s  IP: %s\n)", 
                            id, 
                            p_ip, 
                            inet_ntoa(server.sin_addr));
            }

            /* UDP也使用connect的方式 */
            ret = connect(hd, (struct sockaddr *)(&server), sizeof(struct sockaddr));
            if (ret == AW_OK) {
                /* 连接成功，设置状态后退出 */
                if (strcmp("TCP", p_mode) == 0) {
                   /* status_set(p_gprs, GPRS_TCP_OK); */
                } else if (strcmp("UDP", p_mode) == 0) {
                   /* status_set(p_gprs, GPRS_UDP_OK); */
                }
                p_gprs->socket_num++;
                break;
            } else {
                link_err++;
                __DBG_TRACE("aw_gprs_start[%d]: connect failed.(socket:%d)\n", id, hd);
                continue;
            }
        }

    /* 使用内部协议栈 */
    } else {
        if (p_gprs->p_serv->pfn_gprs_start == NULL) {
            AW_MUTEX_UNLOCK(p_gprs->mutex);
            return -AW_ENOTSUP;
        }
        if (strcmp("TCP", p_mode) == 0) {
            mode = 0;
        } else if (strcmp("UDP", p_mode) == 0) {
            mode = 1;
        }
        hd = p_gprs->p_serv->pfn_gprs_start(p_gprs, mode, p_ip, port, timeout);
        if (hd >= 0) {
            ret = AW_OK;
        } else {
            ret = hd;
        }
    }

exit:
    AW_MUTEX_UNLOCK(p_gprs->mutex);

    return (ret == AW_OK ? hd : ret);
}

aw_err_t aw_gprs_close(int   id, int    hd)
{
    struct awbl_gprs_dev *p_gprs = __gprs_id_to_dev(id);
    aw_err_t ret;

    if (p_gprs == NULL) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_gprs->mutex, AW_SEM_WAIT_FOREVER);

    if (!p_gprs->already_init ) {
        ret = -AW_EPERM;
        goto exit;
    }

    /* 使用外部协议栈 */
    if (p_gprs->use_external_protocol) {
        ret = closesocket(hd);
        __WARN_DETAIL_IF(ret != AW_OK,  "close socket.(hd:%d)", hd);

        if (p_gprs->socket_num > 0) {
            p_gprs->socket_num--;
        }
        if (p_gprs->socket_num == 0) {

#if AWBL_GPRS_USB_USBH_SERIAL
            if (p_gprs->ll_type == AWBL_GPRS_LL_TYPE_ETH) {
                //ret = __gprs_ethernet_close(p_gprs);
                __WARN_DETAIL_IF(ret != AW_OK,  "ethernet close.");
            }
#endif /* AWBL_GPRS_USB_USBH_SERIAL */

            if (p_gprs->ll_type == AWBL_GPRS_LL_TYPE_PPP) {
                ret = __gprs_ppp_close(p_gprs);
                __WARN_DETAIL_IF(ret != AW_OK,  "ppp close.");
            }
        }
    /* 使用内部协议栈 */
    } else {
        if (p_gprs->p_serv->pfn_gprs_close) {
            ret = p_gprs->p_serv->pfn_gprs_close(p_gprs, hd);
        } else {
            ret = -AW_ENOTSUP;
            goto exit;
        }
    }
exit:
    __WARN_DETAIL_IF(ret != AW_OK,  "dev[%d] socket[%d] close.", id, hd);
    AW_MUTEX_UNLOCK(p_gprs->mutex);
    return ret;
}

ssize_t aw_gprs_send (int                 id,
                      int                 hd,
                      void               *p_data,
                      size_t              len,
                      uint32_t            timeout)
{
    struct awbl_gprs_dev *p_gprs = __gprs_id_to_dev(id);
    ssize_t ret;


    if (p_gprs == NULL) {
        return -AW_EINVAL;
    }
    AW_MUTEX_LOCK(p_gprs->mutex, AW_SEM_WAIT_FOREVER);

    if (!p_gprs->already_init ) {
        ret = -AW_EPERM;
        goto exit;
    }

    if (p_gprs->use_external_protocol) {
        ret = send(hd, p_data, len, 0);
        /* __DBG_TRACE("aw_gprs_send: send(hd, p_data, len, 0);\n"); */
    } else {
        if (p_gprs->p_serv->pfn_gprs_send) {
            ret = p_gprs->p_serv->pfn_gprs_send(p_gprs, hd, p_data, len, timeout);
        } else {
            ret = -AW_ENOTSUP;
            goto exit;
        }
    }

exit:
    AW_MUTEX_UNLOCK(p_gprs->mutex);
    return ret;
}


ssize_t aw_gprs_recv (int                 id,
                      int                 hd,
                      void               *p_buf,
                      size_t              len)
{
    struct awbl_gprs_dev *p_gprs = __gprs_id_to_dev(id);
    ssize_t ret;

    if (p_gprs == NULL) {
        return -AW_EINVAL;
    }
    AW_MUTEX_LOCK(p_gprs->mutex, AW_SEM_WAIT_FOREVER);

    if (!p_gprs->already_init ) {
        ret = -AW_EPERM;
        goto exit;
    }

    memset(p_buf, 0, len);
    if (p_gprs->use_external_protocol) {
        ret = recv(hd, p_buf, len, 0);
    } else {
        if (p_gprs->p_serv->pfn_gprs_recv) {
            ret = p_gprs->p_serv->pfn_gprs_recv(p_gprs, hd, p_buf, len);
        } else {
            ret = -AW_ENOTSUP;
            goto exit;
        }
    }

exit:
    AW_MUTEX_UNLOCK(p_gprs->mutex);
    return ret;
}

aw_err_t aw_gprs_sms_send  (int                 id,
                            uint8_t            *p_sms,
                            uint8_t             len,
                            char               *p_num)
{
    struct awbl_gprs_dev *p_gprs = __gprs_id_to_dev(id);
    aw_err_t ret;

    if (p_gprs == NULL) {
        return -AW_EINVAL;
    }
    AW_MUTEX_LOCK(p_gprs->mutex, AW_SEM_WAIT_FOREVER);

    if (!p_gprs->already_init ) {
        ret = -AW_EPERM;
        goto exit;
    }

    if (p_gprs->p_serv->pfn_gprs_sms_send) {
        ret = p_gprs->p_serv->pfn_gprs_sms_send(p_gprs, p_sms, len, p_num);
    } else {
        ret = -AW_ENOTSUP;
    }

exit:
    AW_MUTEX_UNLOCK(p_gprs->mutex);
    return ret;
}

aw_err_t aw_gprs_sms_hex_send  (int                 id,
                                uint8_t            *p_sms,
                                uint8_t             len,
                                char               *p_num)
{
    struct awbl_gprs_dev *p_gprs = __gprs_id_to_dev(id);
    aw_err_t ret;

    if (p_gprs == NULL) {
        return -AW_EINVAL;
    }
    AW_MUTEX_LOCK(p_gprs->mutex, AW_SEM_WAIT_FOREVER);

    if (!p_gprs->already_init ) {
        ret = -AW_EPERM;
        goto exit;
    }

    if (p_gprs->p_serv->pfn_gprs_sms_hex_send) {
        ret = p_gprs->p_serv->pfn_gprs_sms_hex_send(p_gprs, p_sms, len, p_num);
    } else {
        ret = -AW_ENOTSUP;
    }

exit:
    AW_MUTEX_UNLOCK(p_gprs->mutex);
    return ret;
}

aw_err_t aw_gprs_sms_recv  (int                      id,
                            struct aw_gprs_sms_buf  *p_sms,
                            uint32_t                 timeout)
{
    struct awbl_gprs_dev *p_gprs = __gprs_id_to_dev(id);
    aw_err_t ret;

    if (p_gprs == NULL) {
        return -AW_EINVAL;
    }
    AW_MUTEX_LOCK(p_gprs->mutex, AW_SEM_WAIT_FOREVER);

    if (!p_gprs->already_init ) {
        ret = -AW_EPERM;
        goto exit;
    }
    if (p_gprs->p_serv->pfn_gprs_sms_recv == NULL) {
        ret = -AW_ENOTSUP;
        goto exit;
    }

    ret = p_gprs->p_serv->pfn_gprs_sms_recv(p_gprs, p_sms, timeout);

exit:
    AW_MUTEX_UNLOCK(p_gprs->mutex);
    return ret;
}

aw_err_t aw_gprs_all_sms_del(int       id)
{
    struct awbl_gprs_dev *p_gprs = __gprs_id_to_dev(id);
    aw_err_t ret;

    if (p_gprs == NULL) {
        return -AW_EINVAL;
    }
    AW_MUTEX_LOCK(p_gprs->mutex, AW_SEM_WAIT_FOREVER);

    if (!p_gprs->already_init ) {
        AW_MUTEX_UNLOCK(p_gprs->mutex);
        return -AW_EPERM;
    }
    if (p_gprs->p_serv->pfn_gprs_all_sms_del == NULL) {
        AW_MUTEX_UNLOCK(p_gprs->mutex);
        return -AW_ENOTSUP;
    }

    ret = p_gprs->p_serv->pfn_gprs_all_sms_del(p_gprs);
    AW_MUTEX_UNLOCK(p_gprs->mutex);
    return ret;
}

/* 返回 >= 0 表示 reply 时间长度， < 0 响应失败 */
static int __ping_check (struct awbl_gprs_dev *p_gprs)
{
#if AWBL_GPRS_USE_LWIP_2_X
    struct sockaddr_storage  to;
    struct sockaddr_storage  from;
    struct sockaddr_in      *si_to;
    struct sockaddr_in      *si_from;
    uint8_t                  ttl;
#endif
    struct in_addr           dest_addr = {0};
    aw_ping_t                ping;
    int                      ret;
    int                      i;
    struct  netif           *netif = __gprs_netif_get();

    __RETURN_IF(netif == NULL, -AW_ENODEV);

#if AWBL_GPRS_USE_LWIP_1_4
    dest_addr.s_addr = p_gprs->ppp.p_pppaddr->dns1.addr;
#endif /* AWBL_GPRS_USE_LWIP_1_4 */

#if AWBL_GPRS_USE_LWIP_2_X
    memset(&to, 0, sizeof(to));
    memset(&from, 0, sizeof(from));

    to.ss_family = AF_INET;
    to.s2_len = sizeof(struct sockaddr_in);

    si_to = (struct sockaddr_in *) &to;

#if 0
    si_to->sin_addr.s_addr =
            (in_addr_t)ip4_addr_get_u32(ip_2_ip4(&p_gprs->ppp.netif.gw));
#elif 1
    si_to->sin_addr.s_addr =
            (in_addr_t)p_gprs->ppp.p_pcb->ipcp_gotoptions.dnsaddr[0];
#else
    ret = inet_aton("121.33.243.38", &si_to->sin_addr);
    __RETURN_IF(ret == 0, -AW_EINVAL);
#endif

    from.ss_family = AF_INET;
    from.s2_len = sizeof(struct sockaddr_in);

    si_from = (struct sockaddr_in *) &from;
    si_from->sin_addr.s_addr =
            (in_addr_t)ip4_addr_get_u32(ip_2_ip4(&p_gprs->ppp.netif.ip_addr));

    dest_addr = si_to->sin_addr;

    __RETURN_IF(!IP_IS_V4(&p_gprs->ppp.netif.ip_addr), -AW_EINVAL);
    __RETURN_IF(si_to->sin_addr.s_addr == IPADDR_ANY, -AW_EINVAL);
    __RETURN_IF(si_from->sin_addr.s_addr == IPADDR_ANY, -AW_EINVAL);
#endif /* AWBL_GPRS_USE_LWIP_2_X */

#if LWIP_ROUTE_TABLE
    extern struct netif *netif_route_find (ip_addr_t *p_ipaddr);
    extern void netif_route_add (struct netif *netif, ip_addr_t *p_ipaddr);
    extern int netif_route_change (ip_addr_t *p_old_ipaddr,
                                   ip_addr_t *p_new_ipaddr,
                                   struct netif *new_netif);


    if (netif_route_find((ip_addr_t *)&dest_addr) != NULL) {
        netif_route_change((ip_addr_t *)&dest_addr, (ip_addr_t *)&dest_addr, netif);
    } else {
        netif_route_add(netif, (ip_addr_t *)&dest_addr);
    }
#endif

    for (i = 0; i < 4; i++) {
#if AWBL_GPRS_USE_LWIP_1_4
        ret = aw_ping(&ping, &dest_addr, NULL, 96, 0, 8, 2000);
#else
        ttl = 96;
        ret = aw_ping(&ping, &to, &from, &ttl, 8, 2000);
#endif /* AWBL_GPRS_USE_LWIP_1_4 */
        if (ret >= 0) {
            break;
        }
    }

    __LOG_RES(ret >= 0, __MASK_NET_PING, "gprs dev ping \"%s\" ", inet_ntoa(dest_addr));
    return ret;
}

/* tag:sts */

/**
 * 返回 AW_OK 时,表明 p_sts 中的值为效(指明 OK 或具体错误); 
 * 返回其他值则 p_sts 值无效,查询失败
 */ 
aw_local aw_err_t __gprs_status_check (struct awbl_gprs_dev *p_gprs)
{
    aw_err_t  ret;
    char      *arg[1];
    char      buf[256];
    aw_tick_t tick;

    /* 旧状态是 ok,则直接检测 ping 过程 */
    if (p_gprs->status == AW_GPRS_STS_OK) {
        tick = aw_sys_tick_get();

        /* ok 后间隔 6s*/
        if ((tick - p_gprs->last_status_tick) <= aw_ms_to_ticks(6000)) {
            return AW_OK;
        } else {
            goto ping_check;
        }
    }

    memset(buf, 0, sizeof(buf));
    /* 测试 AT 命令 */
    __TIMEOUT_MSEC(tick, 3000) {
        aw_mdelay(500);
        awbl_gprs_at_send(p_gprs, "AT\r");

        arg[0] = "OK";
        ret = awbl_gprs_at_recv(p_gprs, buf, sizeof(buf), arg, 1, 1000);
        if (ret != AW_OK) {
            p_gprs->status = AW_GPRS_STS_AT_ERROR;
            continue;
        }

        p_gprs->status = AW_GPRS_STS_OK;
        break;
    }

    if (p_gprs->status != AW_GPRS_STS_OK) {
        __LOG_RES(AW_FALSE, __MASK_ERR, "status: AW_GPRS_STS_AT_ERROR");
        return AW_OK;
    }

    /* 检查 APN */
    __TIMEOUT_MSEC(tick, 3000) {
        aw_mdelay(1000);
        ret = aw_gprs_ioctl(p_gprs->dev_id, AW_GPRS_REQ_APN_GET, (void *)buf);
        if (ret != AW_OK) {
            p_gprs->status = AW_GPRS_STS_APN_ERROR;
        } else {
            p_gprs->status = AW_GPRS_STS_OK;
            break;
        }
    }

    if (p_gprs->status != AW_GPRS_STS_OK) {
        __LOG_RES(AW_FALSE, __MASK_ERR, "status: AW_GPRS_STS_APN_ERROR");
        return AW_OK;
    }

    /* 检查信号强度 */
    __TIMEOUT_MSEC(tick, 10000) {
        int      csq;

        aw_mdelay(1000);
        ret = aw_gprs_ioctl(p_gprs->dev_id, AW_GPRS_REQ_CSQ_GET, (void *)&csq);

        if (ret != AW_OK) {
            p_gprs->status = AW_GPRS_STS_CSQ_ERROR;
        } else {
            p_gprs->status = AW_GPRS_STS_OK;
            break;
        }
    }

    if (p_gprs->status != AW_GPRS_STS_OK) {
        __LOG_RES(AW_FALSE, __MASK_ERR, "status: AW_GPRS_STS_CSQ_ERROR");
        return AW_OK;
    }

    /* 检查 ppp */
    if (p_gprs->ll_type == AWBL_GPRS_LL_TYPE_PPP) {
        ret = __gprs_ppp_open(p_gprs);
        if (ret != AW_OK) {
            p_gprs->status = AW_GPRS_STS_PPP_ERROR;
            __LOG_RES(AW_FALSE, __MASK_ERR, "status: AW_GPRS_STS_PPP_ERROR");
        } else {
            /* ppp 拨号成功，则所有状态正常，返回 */
            p_gprs->status = AW_GPRS_STS_OK;
            goto exit;
        }
    }

#if AWBL_GPRS_LL_TYPE_ETH_ENABLE
    /* 检查 eth */
    if (p_gprs->ll_type == AWBL_GPRS_LL_TYPE_ETH) {
        ret = __gprs_ethernet_open(p_gprs);
        if (ret != AW_OK) {
            p_gprs->status = AW_GPRS_STS_ETH_ERROR;
            __LOG_RES(AW_FALSE, __MASK_ERR, "status: AW_GPRS_STS_ETH_ERROR");
        } else {
            /* eth 拨号成功，则所有状态正常，返回 */
            p_gprs->status = AW_GPRS_STS_OK;
            goto exit;
        }
    }
#endif /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */

    /*
     * 如果拨号失败则, 由具体设备检查
     * AW_GPRS_STS_PIN_ERROR 和 AW_GPRS_STS_CEREG_ERROR  错误
     */
    if (1) {
        aw_gprs_ioctl_args_t    arg;
        ret = p_gprs->p_serv->pfn_gprs_ioctl(p_gprs, AW_GPRS_REQ_STS, &arg);
        aw_assert(ret == AW_OK);
        if (arg.sts == AW_GPRS_STS_OK) {
            p_gprs->status = AW_GPRS_STS_OK;
        } else {
            p_gprs->status = arg.sts;
        }
    }

    if (p_gprs->status != AW_GPRS_STS_OK) {
        return AW_OK;
    }

    /* ping dns1 */
ping_check:
    if (0) {
        int ms;

        ms = __ping_check(p_gprs);
        if (ms >= 0) {
            p_gprs->status = AW_GPRS_STS_OK;
        } else {
            p_gprs->status = AW_GPRS_STS_PING_ERROR;
            __LOG_RES(AW_FALSE, __MASK_ERR, "status: AW_GPRS_STS_PING_ERROR");
            return AW_OK;
        }
    } else {
        if (p_gprs->ppp.phase == AWBL_GPRS_PPP_PHASE_RUNNING) {
            p_gprs->status = AW_GPRS_STS_OK;
        } else {
            /* ppp 连接断开时，会触发 __ppp_input_thread 线程关闭 ppp */
            p_gprs->status = AW_GPRS_STS_PPP_ERROR;
            __LOG_RES(AW_FALSE, __MASK_ERR, "status: AW_GPRS_STS_PPP_ERROR");
        }
    }

exit:
    p_gprs->last_status_tick = aw_sys_tick_get();
    return AW_OK;
}

#if __VIH_DEBUG_HTTPS_AT
aw_err_t aw_gprs_https_get (int id,
                            char *p_url,
                            size_t   url_len,
                            void (*p_response_cb) (void *rdata,
                                                   size_t rsize),
                            uint32_t sec_timeout)
{
    struct awbl_gprs_dev *p_gprs = __gprs_id_to_dev(id);

    __RETURN_IF(id != 3, -AW_EINVAL);

    return awbl_gprs_ec20_https_get(p_gprs, p_url, url_len, p_response_cb, sec_timeout);
}
#endif


/******************************************************************************/
/* tag:ioctl */

aw_err_t aw_gprs_ioctl (int                   id,
                        aw_gprs_ioctl_req_t   req,
                        aw_gprs_ioctl_args_t *p_arg)
{
    struct awbl_gprs_dev *p_gprs = __gprs_id_to_dev(id);
    aw_err_t              ret = -AW_EINVAL;
    aw_gprs_evt_t         evt;

    if (!p_gprs) {
        return -AW_EINVAL;
    }
    AW_MUTEX_LOCK(p_gprs->mutex, AW_SEM_WAIT_FOREVER);

    /*
     * 不需要 power on 即可设置的选项
     */
    switch (req) {
        case AW_GPRS_LOG_SET: {
            p_gprs->pfn_log_out = p_arg->pfn_log_out;
            ret                 = AW_OK;
            goto exit;
        } break;

        case AW_GPRS_EVT_CB_SET: {
            /**
             * 设置事件回调，支持同时设置多个回调；
             * 掉电不清除事件回调
             */
            ret = __ioctl_evt_cb_set(id, p_arg);
            goto exit;
        } break;

        case AW_GPRS_EVT_CB_CLR: {
            int i = 0;

            ret = -AW_EINVAL;
            for (; i < AWBL_GPRS_EVT_CB_MAX; i++) {
                if (p_gprs->pfn_evt_cb_array[i] == p_arg->pfn_evt_handler) {
                    p_gprs->pfn_evt_cb_array[i] = NULL;
                    ret = AW_OK;
                }
            }

            goto exit;
        } break;
        default:break;
    }

    if (!p_gprs->already_init) {
        AW_MUTEX_UNLOCK(p_gprs->mutex);
        return -AW_EPERM;
    }

    /*
     * 需要 power on 后才可设置的选项
     */
    switch (req) {
    case AW_GPRS_REQ_PROTOCOL_SET:
        /* 网络已经连接的情况不允许切换协议栈  */
        __GOTO_EXIT_DETAIL_IF(p_gprs->socket_num > 0,
                              -AW_EPERM,
                              "switch %S protocol.",
                              0 == p_arg->proto_opt ? "external" : "internal");

        if (0 == p_arg->proto_opt) {
            p_gprs->use_external_protocol = AW_TRUE;
        } else {
            p_gprs->use_external_protocol = AW_FALSE;
        }
        ret = AW_OK;
        break;

    case AW_GPRS_REQ_NET_LINK_UP: {

        /* 检查使用的是模块内部协议栈则失败退出 */
        if (!p_gprs->use_external_protocol) {
            ret = -AW_EPERM;
            __WARN_DETAIL_IF(ret != AW_OK,
                             "The network link cannot be established "
                             "currently using the internal protocol stack!");
        }

#if AWBL_GPRS_LL_TYPE_ETH_ENABLE
        if (p_gprs->ll_type == AWBL_GPRS_LL_TYPE_ETH) {
            ret = __gprs_ethernet_open(p_gprs);
            __WARN_DETAIL_IF(ret != AW_OK,  "__gprs_ethernet_open()");
        }
#endif /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */

        if (p_gprs->ll_type == AWBL_GPRS_LL_TYPE_PPP) {
            ret = __gprs_ppp_open(p_gprs);
            __WARN_DETAIL_IF(ret != AW_OK,  "__gprs_ppp_open()");
        }
    } break;

    case AW_GPRS_REQ_NET_LINK_DOWN: {
        /* 检查使用的是模块内部协议栈则失败退出 */
        if (!p_gprs->use_external_protocol) {
            ret = -AW_EPERM;
        }

#if AWBL_GPRS_LL_TYPE_ETH_ENABLE
        if (p_gprs->ll_type == AWBL_GPRS_LL_TYPE_ETH) {
            ret = __gprs_ethernet_close(p_gprs);
            __WARN_DETAIL_IF(ret != AW_OK,  "__gprs_ethernet_open()");
        }
#endif /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */

        if (p_gprs->ll_type == AWBL_GPRS_LL_TYPE_PPP) {
            ret = __gprs_ppp_close(p_gprs);
            __WARN_DETAIL_IF(ret != AW_OK,  "__gprs_ppp_open()");
        }
    } break;

    case AW_GPRS_REQ_APN_SET: {
        p_gprs->p_info->p_apn = p_arg->p_apn;
        ret = AW_OK;
        break;
    }
    case AW_GPRS_REQ_APN_GET: {
        if (p_gprs->p_info->p_apn != NULL) {
            p_arg->p_apn = p_gprs->p_info->p_apn;
            ret = AW_OK;
        } else {
            if (p_gprs->p_serv->pfn_gprs_ioctl) {
                ret = p_gprs->p_serv->pfn_gprs_ioctl(p_gprs, req, p_arg);
            } else {
                AW_MUTEX_UNLOCK(p_gprs->mutex);
                return -AW_ENOTSUP;
            }
        }
        break;
    }
    case AW_GPRS_REQ_AT_TRANS: {
        ret = __ioctl_req_at_trans(id, p_arg);
    } break;

    case AW_GPRS_REQ_STS:
        ret = __gprs_status_check(p_gprs);
        if (ret == AW_OK) {
            p_arg->sts = p_gprs->status;
        } else {
            p_arg->sts = AW_GPRS_STS_AT_ERROR;
        }
        break;

    case AW_GPRS_REQ_IP_GET: {

#if AWBL_GPRS_USE_LWIP_1_4

    #if AWBL_GPRS_LL_TYPE_ETH_ENABLE
        if (p_gprs->ll_type == AWBL_GPRS_LL_TYPE_ETH) {
            p_arg->ip_addr = p_gprs->ethernet.ethernetaddr.our_ipaddr;
        }
    #endif /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */

        if (p_gprs->ll_type == AWBL_GPRS_LL_TYPE_PPP) {
            p_arg->ip_addr = p_gprs->ppp.p_pppaddr->our_ipaddr;
        }

        ret = AW_OK;
#endif /* AWBL_GPRS_USE_LWIP_1_4 */

#if AWBL_GPRS_USE_LWIP_2_X

    #if AWBL_GPRS_LL_TYPE_ETH_ENABLE
        if (p_gprs->ll_type == AWBL_GPRS_LL_TYPE_ETH) {
            p_arg->ip_addr = p_gprs->ethernet.ethernetaddr.our_ipaddr;
        }
    #endif /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */

        if (p_gprs->ll_type == AWBL_GPRS_LL_TYPE_PPP) {
            p_arg->ip_addr = p_gprs->ppp.netif.ip_addr;
        }
        ret = AW_OK;
#endif /* AWBL_GPRS_USE_LWIP_2_X */

    } break;

    case AW_GPRS_REQ_CSQ_MIN_SET: {
        p_gprs->csq_min = p_arg->csq;
        ret = AW_OK;
    } break;

    case AW_GPRS_REQ_CSQ_GET:

        /* ret < 0 表示信号弱到无法使用或查询不到信号强度 */
        if (p_gprs->p_serv->pfn_gprs_ioctl) {
            ret = p_gprs->p_serv->pfn_gprs_ioctl(p_gprs, req, p_arg);
        } else {
            AW_MUTEX_UNLOCK(p_gprs->mutex);
            return -AW_ENOTSUP;
        }
        break;
    case AW_GPRS_EXCEPTION:

        if (p_gprs->p_serv->pfn_gprs_ioctl) {
            ret = p_gprs->p_serv->pfn_gprs_ioctl(p_gprs, req, p_arg);
        } else {
            AW_MUTEX_UNLOCK(p_gprs->mutex);
            return -AW_ENOTSUP;
        }
        break;

    case AW_GPRS_REQ_RESET: {
        /*
         * 0:软件复位, else:硬件复位;
         * 应断开拨号、socket连接等
         */
        char net_recover;

        __GOTO_EXIT_DETAIL_IF(p_gprs->socket_num != 0,
                              -AW_EPERM,
                              "need close socket.(socket num:%d)",
                              p_gprs->socket_num);

        if (p_gprs->ll_type == AWBL_GPRS_LL_TYPE_PPP) {
            net_recover = (p_gprs->ppp.phase == AWBL_GPRS_PPP_PHASE_RUNNING ?
                                               AW_TRUE:AW_FALSE);
            __gprs_ppp_close(p_gprs);
        }

#if AWBL_GPRS_LL_TYPE_ETH_ENABLE
        if (p_gprs->ll_type == AWBL_GPRS_LL_TYPE_ETH) {
            net_recover = (p_gprs->ethernet.phase ==
                    AWBL_GPRS_ETHERNET_PHASE_DIAL_UP ? AW_TRUE:AW_FALSE);
            __gprs_ethernet_close(p_gprs);
        }
#endif /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */

        if (p_gprs->p_serv->pfn_gprs_ioctl) {
            evt.type = AW_GPRS_EVT_TYPE_RESET_PRE;
            ret = __evt_cb_array_notify(p_gprs, &evt);
            if (ret != AW_OK) {
                __LOG_PFX(1, "The request reset failed because some devices "
                          "were not allowed! (ret:%d)", ret);
                goto exit;
            }

            ret = p_gprs->p_serv->pfn_gprs_ioctl(p_gprs, req, p_arg);

            /* A notification is issued if the reset is successful. */
            if (ret == AW_OK) {
                evt.type = AW_GPRS_EVT_TYPE_RESET_COMPLETED;
                __evt_cb_array_notify(p_gprs, &evt);
            }
        } else {
            AW_MUTEX_UNLOCK(p_gprs->mutex);
            return -AW_ENOTSUP;
        }

        /* 如果之前是拨号成功的状态，则复位后恢复拨号状态  */
        if (net_recover) {
            if (p_gprs->ll_type == AWBL_GPRS_LL_TYPE_PPP) {
                __gprs_ppp_open(p_gprs);
            }

#if AWBL_GPRS_LL_TYPE_ETH_ENABLE
            if (p_gprs->ll_type == AWBL_GPRS_LL_TYPE_ETH) {
                __gprs_ethernet_open(p_gprs);
            }
#endif /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */
        }

        p_gprs->status = AW_GPRS_STS_AT_ERROR;

    } break;

    case AW_GPRS_REQ_CCID_GET:
    case AW_GPRS_REQ_VERSION_GET:
    case AW_GPRS_REQ_IMEI_GET:
    case AW_GPRS_REQ_IMSI_GET:
    case AW_GPRS_REQ_SCA_GET:
    case AW_GPRS_REQ_NET_MODE_GET:
    case AW_GPRS_REQ_BS_GET:
    case AW_GPRS_REQ_PING:
    default:
        ret = -AW_ENOTSUP;
        if (p_gprs->p_serv->pfn_gprs_ioctl) {
            ret = p_gprs->p_serv->pfn_gprs_ioctl(p_gprs, req, p_arg);
        } else {
            AW_MUTEX_UNLOCK(p_gprs->mutex);
            return -AW_ENOTSUP;
        }
        break;
    }

exit:
    AW_MUTEX_UNLOCK(p_gprs->mutex);
    return ret;
}


int aw_gprs_any_get (uint32_t timeout_sec)
{
    aw_local int            id = -1;
    struct awbl_gprs_dev    *p_serv_cur = __gp_gprs_dev_head;
    aw_tick_t               tick_tmp;
    struct awbl_gprs_dev    *p_gprs;

    if (id >= 0) {
        p_gprs = __gprs_id_to_dev(id);
        if (!p_gprs->power_on) {
            id = -1;
        } else {
            return id;
        }
    }

    if (timeout_sec < 1) {
        timeout_sec = 1;
    }

    __TIMEOUT_SEC(tick_tmp, timeout_sec) {
        aw_mdelay(1000);

        p_serv_cur = __gp_gprs_dev_head;

        while ((p_serv_cur != NULL)) {
            if (p_serv_cur->p_serv->pfn_probe) {
                if (p_serv_cur->p_serv->pfn_probe(p_serv_cur) ) {
                    id = p_serv_cur->dev_id;
                    return id;
                }
            }

            p_serv_cur = p_serv_cur->p_next;
        }
    }

    return -1;
}

/*******************************************************************************
 * GNSS
 ******************************************************************************/
aw_err_t aw_gprs_gnss_enable (int dev_id, aw_gprs_gnss_cfg_t *p_gnss_cfg)
{
    struct awbl_gprs_dev *p_gprs = __gprs_id_to_dev(dev_id);
    aw_err_t              ret = -AW_ERROR;
    aw_gprs_ioctl_args_t  ioctl_args;

    if (!p_gprs || !p_gnss_cfg) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_gprs->mutex, AW_SEM_WAIT_FOREVER);

    if (!p_gprs->already_init ) {
        ret = -AW_EPERM;
        goto exit;
    }

    if (p_gprs->gnss_en == AW_TRUE) {
        ret = -AW_EEXIST;       /* gnss 已被使能 */
        goto exit;
    }

    /* 添加回调，失败则退出   */
    if (p_gnss_cfg->pfn_evt_cb) {
        ioctl_args.pfn_evt_handler = p_gnss_cfg->pfn_evt_cb;
        ret = aw_gprs_ioctl(dev_id, AW_GPRS_EVT_CB_SET, &ioctl_args);
        if (ret != AW_OK) {
            goto exit;
        }
    }

    if (p_gprs->p_serv->pfn_gnss_enable) {
        ret = p_gprs->p_serv->pfn_gnss_enable(p_gprs, p_gnss_cfg);
    } else {
        ret = -AW_ENOTSUP;
    }

    if (ret == AW_OK) {
        p_gprs->gnss_en = AW_TRUE;
    }

exit:
    AW_MUTEX_UNLOCK(p_gprs->mutex);
    return ret;
}

aw_err_t aw_gprs_gnss_disable (int dev_id)
{
    struct awbl_gprs_dev *p_gprs = __gprs_id_to_dev(dev_id);
    aw_err_t              ret = -AW_ERROR;

    if (p_gprs == NULL) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_gprs->mutex, AW_SEM_WAIT_FOREVER);

    if (!p_gprs->already_init) {
        ret = -AW_EPERM;
        goto exit;
    }

    if (p_gprs->gnss_en == AW_FALSE) {
        ret = AW_OK;
        goto exit;
    }

    if (p_gprs->p_serv->pfn_gnss_disable) {
        ret = p_gprs->p_serv->pfn_gnss_disable(p_gprs);
    } else {
        ret = -AW_ENOTSUP;
    }

    if (ret == AW_OK) {
        p_gprs->gnss_en = AW_FALSE;
    }

exit:
    AW_MUTEX_UNLOCK(p_gprs->mutex);
    return ret;
}

aw_err_t aw_gprs_gnss_nmeasrc_get (int      dev_id,
                                   uint8_t *p_buf,
                                   size_t   bufsize,
                                   uint32_t timeout)
{
    struct awbl_gprs_dev *p_gprs = __gprs_id_to_dev(dev_id);
    aw_err_t              ret = -AW_ERROR;

    if (p_gprs == NULL) {
        return -AW_EINVAL;
    }

    AW_MUTEX_LOCK(p_gprs->mutex, AW_SEM_WAIT_FOREVER);

    if (!p_gprs->already_init) {
        ret = -AW_EPERM;
        goto exit;
    }

    if (p_gprs->gnss_en == AW_FALSE) {
        ret = -AW_EEXIST;   /* 设备未使能 gnss 功能 */
        goto exit;
    }

    if (p_gprs->p_serv->pfn_gnss_nmeasrc_get) {
        ret = p_gprs->p_serv->pfn_gnss_nmeasrc_get(p_gprs,
                                                   p_buf,
                                                   bufsize,
                                                   timeout);
    } else {
        ret = -AW_ENOTSUP;
    }

exit:
    AW_MUTEX_UNLOCK(p_gprs->mutex);
    return ret;
}



/* end of file*/

