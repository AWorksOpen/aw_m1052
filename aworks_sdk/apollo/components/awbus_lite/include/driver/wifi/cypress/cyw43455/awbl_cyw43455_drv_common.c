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
 * \brief TODO文件描述
 *
 * 使用本服务需要包含头文件
 * \code
 * #include "awbl_cyw43455_drv_common.h"
 * \endcode
 *
 * \internal
 * \par Modification History
 * - 1.00 19-04-17  mex, first implementation
 * \endinternal
 */

#include "driver/wifi/cypress/cyw43455/awbl_cyw43455_drv_common.h"
#include "driver/wifi/cypress/cyw43455/awbl_cyw43455_sdio.h"
#include "driver/wifi/cypress/cyw43455/private/awbl_cyw43455_sdio.h"

#include "driver/wifi/cypress/awbl_cyw_wiced_common.h"

#include "wwd_events.h"

#define WIFI_PWRCTL_API_EN 1

#define WWD_API_RETRY_DELAY         10

#define WWD_API_LOCK_TIMEOUT        1000

#ifndef ENDL
    #define ENDL                    "\r\n"
#endif

#define __NULL_MAC(a)   (((a[0])==0)&& \
                         ((a[1])==0)&& \
                         ((a[2])==0)&& \
                         ((a[3])==0)&& \
                         ((a[4])==0)&& \
                         ((a[5])==0))

#define __BOARD_MAC(a)   (((a[0])==0xff)&& \
                          ((a[1])==0xff)&& \
                          ((a[2])==0xff)&& \
                          ((a[3])==0xff)&& \
                          ((a[4])==0xff)&& \
                          ((a[5])==0xff))

#define __DRV_LOCK_INIT(thiz)                \
                    AW_SEMB_INIT(thiz->base.op_lock, AW_SEM_FULL, AW_SEM_Q_FIFO)
#define __DRV_TRY_LOCK(thiz)                 do { \
        aw_err_t ret = \
        AW_SEMB_TAKE(thiz->base.op_lock, aw_ms_to_ticks(WWD_API_LOCK_TIMEOUT));\
        if (AW_OK != ret) { \
            return ret; \
        } \
    } while (0);

#define __DRV_UNLOCK(thiz)                   AW_SEMB_GIVE(thiz->base.op_lock)
#define __DRV_UNLOCK_RETURN(thiz, err)       __DRV_UNLOCK(thiz);return err

aw_inline aw_local aw_err_t __wifi_if_to_wwd_if (void            *wifiif, 
                                                 wwd_interface_t *interface);
aw_local void __raw_pkt_processor (wiced_buffer_t  buffer, 
                                   wwd_interface_t interface);
aw_local void *softap_event_handler (const wwd_event_header_t *event_header,
                                     const uint8_t            *event_data,
                                     void                     *p_arg);

extern struct awbl_dev *__gp_cyw43455_sdio_dev;

/*******************************************************************************
  GLOBAL
*******************************************************************************/
/* service functions */
aw_const awbl_wifi_servops_t __g_wifi_cyw43455_drv_srv_ops = {
    .pfn_scan             = awbl_wiced_scan,
    .pfn_abort_scan       = awbl_wiced_abort_scan,
    .pfn_join             = awbl_wiced_join,
    .pfn_leave            = awbl_wiced_leave,
    .pfn_start_ap         = awbl_wiced_start_ap,
    .pfn_stop_ap          = awbl_wiced_stop_ap,
    .pfn_get_sta_list     = awbl_wiced_get_sta_list,
    .pfn_get_ap_info      = awbl_wiced_get_ap_info,
    .pfn_get_link_status  = awbl_wiced_get_link_status,
    .pfn_dev_ioctl        = awbl_wiced_dev_ioctl,
    .pfn_get_join_ap_info = awbl_wiced_get_join_ap_info,
    .pfn_deauth_sta       = awbl_wiced_deauth_sta,
    .pfn_config_apply     = awbl_wiced_config_apply,
};

/*******************************************************************************
  WICED
*******************************************************************************/

static const wwd_event_num_t ap_client_events[] = {
    WLC_E_DEAUTH,
    WLC_E_DEAUTH_IND,
    WLC_E_DISASSOC,
    WLC_E_DISASSOC_IND,
    WLC_E_ASSOC_IND,
    WLC_E_REASSOC_IND,
    WLC_E_NONE
};

aw_local inline wiced_security_t __aw_security_to_wiced_security (
    aw_wifi_security_t security)
{
    return (wiced_security_t) security;
}

aw_local inline aw_wifi_security_t __wiced_security_to_aw_security (
    wiced_security_t security)
{
    return (aw_wifi_security_t) security;
}

aw_local inline wiced_country_code_t __aw_ccode_to_wiced_ccode (
    aw_wifi_country_code_t code)
{
    return (wiced_country_code_t) code;
}

aw_local inline aw_wifi_country_code_t __wiced_ccode_to_aw_ccode (
    wiced_country_code_t code)
{
    return (aw_wifi_country_code_t) code;
}

typedef struct {
    AW_SEMB_DECL(signal);
    aw_wifi_scan_callback_t pfn_cb;
    void *p_arg;
} __scan_user_data_t;

aw_local void __wiced_scan_result_to_aw (aw_wifi_scan_result_t *p_res,
                                         wiced_scan_result_t   *wiced_result)
{
    if (p_res) {
#if 0 
        aw_wifi_country_code_t ccode = 
            __wiced_ccode_to_aw_ccode(wiced_result->ccode);
#endif 
        p_res->ssid.length     = wiced_result->SSID.length    ;
        memset(p_res->ssid.value, 0, sizeof(p_res->ssid.value));
        memcpy(p_res->ssid.value, 
               wiced_result->SSID.value, 
               wiced_result->SSID.length);
        memcpy(p_res->bssid.octet, wiced_result->BSSID.octet, 6);
        p_res->signal_strength = wiced_result->signal_strength;
        p_res->max_data_rate   = wiced_result->max_data_rate  ;
        p_res->bss_type        = wiced_result->bss_type       ;
        p_res->security        = 
            __wiced_security_to_aw_security(wiced_result->security);
        p_res->channel         = wiced_result->channel        ;
        p_res->band            = wiced_result->band           ;
#if 0 
        memcpy(p_res->ccode, &ccode, sizeof(p_res->ccode));
#endif 
        p_res->ccode[0]        = wiced_result->ccode[0]       ;
        p_res->ccode[1]        = wiced_result->ccode[1]       ;
        p_res->ccode[2]        = '\0'                         ;
        p_res->ccode[3]        = '\0'                         ;
        p_res->flags           = wiced_result->flags          ;
    }
}

aw_local wiced_result_t __scan_result_handler (
    wiced_scan_handler_result_t *malloced_scan_result)
{
    if (NULL == malloced_scan_result) {
        return WICED_SUCCESS;
    } else {
        aw_wifi_scan_cb_result_t res;
        __DEV_CONVERT(p_this, malloced_scan_result->user_data);

        /* 传递用户参数 */
        res.p_arg = p_this->base.cb.p_scan_arg;
        if (malloced_scan_result->status == WICED_SCAN_INCOMPLETE) {
            /* 转换为 AW 格式 */
            __wiced_scan_result_to_aw(&res.ap_details, 
                                      &malloced_scan_result->ap_details);
            p_this->base.cb.pfn_scan_hdl(&res, AW_WIFI_SCAN_INCOMPLETE);
        } else if (malloced_scan_result->status == 
                   WICED_SCAN_COMPLETED_SUCCESSFULLY) {
            p_this->base.cb.pfn_scan_hdl(&res,
                                         AW_WIFI_SCAN_COMPLETED_SUCCESSFULLY);
        } else {
            p_this->base.cb.pfn_scan_hdl(&res, AW_WIFI_SCAN_ABORTED);
        }
    }

    /**
     * 2017-10-25 18:01:32 phd
     * 原设计逻辑:
     * malloced_scan_result 初始在 wiced_wifi_scan_networks_ex 中分配,
     * (scan_result_ptr = 
     *     MALLOC_OBJECT("scan result", wiced_scan_handler_result_t);)
     * 每次扫描结果到来时执行回调(wiced_scan_result_handler),
     * 回调 wiced_scan_result_handler 中发送消息给
     * WICED_NETWORKING_WORKER_THREAD
     * 由该任务执行 int_scan_handler->results_handler(scan_result_ptr)
     * (即 wiced_wifi_scan_networks_ex 的用户回调参数 
     *     wiced_scan_result_handler_t results_handler)
     * 发送消息之后在  wiced_scan_result_handler 中再次分配,
     * 故此处需要立即释放,
     * 原设计为配合异步处理, 这里虽改为同步回调,
     * 但是暂不改动该内存使用逻辑
     */
#if DBG_SCAN_MALLOC
    debug_printt("scan_result_cb:free:%p", malloced_scan_result);
#endif /* if DBG_SCAN_MALLOC */
    free(malloced_scan_result);

    return WWD_SUCCESS;
}

aw_const char *cyw43455_analyse_failed_join_result (wiced_result_t join_result)
{
    /*
     * Note that DHCP timeouts and EAPOL key timeouts may happen at the edge of 
     * the cell. If possible move closer to the AP. 
     */
    /* 
     * Also note that the default number of join attempts is three and the join 
     * result is returned for the last attempt. 
     */
    char *msg;

    switch (join_result) {

    case WICED_ERROR:
    
        /* 
         * Getting a DHCP address may fail if at the edge of a cell and the join
         * may timeout before DHCP has completed. 
         */
        msg = "General error"; 
        break;

    case WWD_NETWORK_NOT_FOUND:
    
        /* Check that he SSID is correct and that the AP is up */
        msg = "Failed to find network"; 
        break;

    case WWD_NOT_AUTHENTICATED:
    
        /* May happen at the edge of the cell. Try moving closer to the AP. */
        msg = "Failed to authenticate"; 
        break;

    case WWD_EAPOL_KEY_PACKET_M1_TIMEOUT:
        msg = "Timeout waiting for first EAPOL key frame from AP";
        break;

    case WWD_EAPOL_KEY_PACKET_M3_TIMEOUT:
    
        /* The M3 timeout will occur if the passphrase is incorrect */
        msg = "Check the passphrase and try again"; 
        break;

    case WWD_EAPOL_KEY_PACKET_G1_TIMEOUT:
        msg = "Timeout waiting for group key from AP";
        break;

    case WWD_INVALID_JOIN_STATUS:
    
        /* May happen at the edge of the cell. Try moving closer to the AP. */
        msg = "Some part of the join process did not complete"; 
        break;

    default:
        msg = "?";
        break;
    }

    return msg;
}

aw_err_t awbl_wiced_scan (void                    *p_cookie,
                          aw_wifi_scan_callback_t  pfn_cb,
                          void                    *p_arg,
                          aw_wifi_ssid_t          *p_ssid,
                          aw_wifi_mac_t           *p_bssid,
                          aw_wifi_scan_type_t      type)
{
    __DEV_DECL(p_this, p_cookie);
    wiced_result_t result;

    wiced_ssid_t  wiced_ssid;
    wiced_mac_t   wiced_mac;
    wiced_ssid_t *p_wiced_ssid = NULL;
    wiced_mac_t  *p_wiced_mac = NULL;

    __DRV_TRY_LOCK(p_this);

    if (p_ssid) {
        p_wiced_ssid = &wiced_ssid;
        memcpy(p_wiced_ssid->value, p_ssid->value, p_ssid->length);
        p_wiced_ssid->length = p_ssid->length;
    }
    if (p_bssid) {
        p_wiced_mac = &wiced_mac;
        memcpy(p_wiced_mac->octet, p_bssid->octet, sizeof(p_wiced_mac->octet));
    }

    p_this->base.cb.pfn_scan_hdl = pfn_cb;
    p_this->base.cb.p_scan_arg = p_arg;

    result = wiced_wifi_scan_networks_ex(__scan_result_handler,
                                         p_this,
                                         type,
                                         WICED_BSS_TYPE_ANY,
                                         p_wiced_ssid,
                                         p_wiced_mac,
                                         NULL,
                                         NULL,
                                         WICED_STA_INTERFACE);

    __DRV_UNLOCK(p_this);

    return (WICED_SUCCESS == result) ? AW_OK : AW_ERROR;
}

aw_err_t awbl_wiced_abort_scan (void *p_cookie)
{
    __DEV_DECL(p_this, p_cookie);
    wiced_result_t result;

    __DRV_TRY_LOCK(p_this);

    result = wiced_wifi_scan_disable();

    __DRV_UNLOCK(p_this);

    return (WICED_SUCCESS == result) ? AW_OK : AW_ERROR;
}

aw_err_t awbl_wiced_join (void               *p_cookie,
                          aw_wifi_mac_t      *p_bssid,
                          aw_wifi_ssid_t     *p_ssid,
                          aw_wifi_security_t  security,
                          aw_wifi_key_t      *p_key)
{
    __DEV_DECL(p_this, p_cookie);
    wiced_ap_info_t ap;
    wiced_result_t  result;

    __DRV_TRY_LOCK(p_this);

    if (WWD_SUCCESS == wwd_wifi_is_ready_to_transceive(WWD_STA_INTERFACE)) {
        AW_INFOF(("Error: already joined" ENDL));
        __DRV_UNLOCK_RETURN(p_this, AW_OK);
    }
    if (WWD_SUCCESS == wwd_wifi_is_ready_to_transceive(WWD_AP_INTERFACE)) {
        AW_WARNF(("Warn: AP will be moved to the STA assoc channel" ENDL));
    }

    memset(&ap, 0, sizeof(ap));
    memcpy(ap.SSID.value, p_ssid->value, sizeof(ap.SSID.value));
    ap.SSID.length = p_ssid->length;
    ap.bss_type = WICED_BSS_TYPE_INFRASTRUCTURE;
    ap.security = __aw_security_to_wiced_security(security);

    if (p_bssid != NULL && !NULL_MAC(p_bssid->octet)) {
        memcpy(&ap.BSSID, p_bssid, sizeof(ap.BSSID));
    }

    result = wiced_join_ap_specific(&ap,
                                    p_key->length,
                                    (char *) p_key->value);
    if (result != WICED_SUCCESS) {
        AW_INFOF(("Join result %u: %s" ENDL,
                (unsigned int) result,
                cyw43455_analyse_failed_join_result(result)));
        __DRV_UNLOCK_RETURN(p_this, (aw_err_t) result);
    }

    /**
     * phd
     * 2017-08-29 15:54:30
     *
     * if ethernet_if_*() call func in netif.h
     * conn->current_msg == NULL
     * lwip_1.4.x/src/api/api_msg.c:402:(0)
     *
     * if ethernet_if_*() call func in netifapi.h
     * no assert
     *
     * situation:
     * task0: run in ligher priority(shell)
     * task1: run in lowest priority(tcp_client)
     * 1. task0 call lwip_connect()
     * 2. task1 call aw_wifi_join() which will call ethernet_if_set_ipaddr()
     * 3. ethernet_if_set_ipaddr() at task1 context
     * 4. err_tcp() called
     * 5. assert
     */
    aw_netif_set_link_up(&p_this->base.netif.sta.ethif);

    __DRV_UNLOCK_RETURN(p_this, AW_OK);
}

aw_err_t awbl_wiced_leave (void *p_cookie)
{
    __DEV_DECL(p_this, p_cookie);

    aw_netif_set_link_down(&p_this->base.netif.sta.ethif);
    __DRV_TRY_LOCK(p_this);
    wiced_leave_ap(WICED_STA_INTERFACE);
    __DRV_UNLOCK(p_this);
    return AW_OK;
}

aw_err_t awbl_wiced_start_ap (void               *p_cookie,
                              aw_wifi_ssid_t     *p_ssid,
                              aw_wifi_security_t  security,
                              aw_wifi_key_t      *p_key,
                              uint8_t             ch,
                              int                 hide_ssid)
{
    __DEV_DECL(p_this, p_cookie);
    aw_netif_t *p_ethif;

    wiced_ssid_t ssid;
    wiced_security_t auth_type;
    wwd_result_t result;

    __DRV_TRY_LOCK(p_this);

    p_ethif = &p_this->base.netif.ap.ethif;
    /* check the sec_type */
    if (security != AW_WIFI_SECURITY_WPA2_AES_PSK
            && security != AW_WIFI_SECURITY_OPEN
            && security != AW_WIFI_SECURITY_WPA2_MIXED_PSK) {
        AW_INFOF(("Error: Invalid security type" ENDL));
        __DRV_UNLOCK_RETURN(p_this, -AW_EINVAL);
    }

    if (wwd_wifi_is_ready_to_transceive(WICED_AP_INTERFACE) == WWD_SUCCESS) {
        AW_INFOF(("Error: AP already started" ENDL));
        __DRV_UNLOCK_RETURN(p_this, -AW_EPERM);
    }

    memcpy(ssid.value, p_ssid->value, p_ssid->length);
    ssid.length = p_ssid->length;

    auth_type = __aw_security_to_wiced_security(security);

    result = wwd_wifi_start_ap(&ssid, 
                               auth_type, 
                               p_key->value, 
                               p_key->length,
                               ch,
                               hide_ssid);
    if (WWD_SUCCESS == result) {
        aw_netif_set_link_up(p_ethif);
        result = (wwd_result_t) AW_OK;
    }
    __DRV_UNLOCK(p_this);
    return (aw_err_t) result;
}

aw_err_t awbl_wiced_stop_ap (void *p_cookie)
{
    __DEV_DECL(p_this, p_cookie);
    aw_netif_t *p_ethif;
    wwd_result_t result;

    __DRV_TRY_LOCK(p_this);
    p_ethif = &p_this->base.netif.ap.ethif;

    if (wwd_wifi_is_ready_to_transceive(WWD_AP_INTERFACE) != WWD_SUCCESS) {
        AW_INFOF(("Error: ap is down" ENDL));
        __DRV_UNLOCK_RETURN(p_this, AW_OK);
    }

    aw_netif_set_link_down(p_ethif);

    (void) wwd_wifi_deauth_all_associated_client_stas(WWD_DOT11_RC_UNSPECIFIED, 
                                                      WWD_AP_INTERFACE);

    result = wwd_wifi_stop_ap();
    if (WWD_SUCCESS != result) {
        aw_mdelay(WWD_API_RETRY_DELAY);
        AW_INFOF(("stop ap error %d retry." ENDL, result));
        result = wwd_wifi_stop_ap();
        if (WWD_SUCCESS != result) {
            AW_INFOF(("Can not stop ap %d." ENDL, result));
            __DRV_UNLOCK_RETURN(p_this, AW_ERROR);
        }
    }
    __DRV_UNLOCK(p_this);
    return AW_OK;
}

aw_err_t awbl_wiced_deauth_sta (void *p_cookie, aw_wifi_mac_t *p_mac)
{
    __DEV_DECL(p_this, p_cookie);
    wwd_result_t result;

    __DRV_TRY_LOCK(p_this);

    if (wwd_wifi_is_ready_to_transceive(WWD_AP_INTERFACE) != WWD_SUCCESS) {
        AW_INFOF(("AP not start." ENDL));
        __DRV_UNLOCK_RETURN(p_this, AW_OK);
    }

    if (NULL == p_mac || NULL_MAC(p_mac->octet)) {
        result = wwd_wifi_deauth_all_associated_client_stas(
                     WWD_DOT11_RC_UNSPECIFIED, 
                     WWD_AP_INTERFACE);
    } else {
        result = wwd_wifi_deauth_sta((wiced_mac_t *) p_mac,
                                     WWD_DOT11_RC_UNSPECIFIED,
                                     WWD_AP_INTERFACE);
    }

    __DRV_UNLOCK(p_this);
    return (WWD_SUCCESS == result) ? AW_OK : AW_ERROR;
}

aw_err_t awbl_wiced_get_sta_list (void                             *p_cookie,
                                  aw_wifi_get_sta_result_handler_t  handler,
                                  void                             *p_arg)
{
    __DEV_DECL(p_this, p_cookie);

    /* 通过wwd_wifi_get_max_associations获得可支持的最多连接数为32 */
    static const uint32_t max_clients = 32;
    uint8_t               buffer[sizeof(uint32_t) + (max_clients * sizeof(wiced_mac_t))];
    size_t                size = sizeof(buffer);

    wiced_maclist_t   *clients;
    wiced_mac_t       *current;
    wl_bss_info_t      ap_info;
    wwd_result_t       result;
    wiced_security_t   sec;

    aw_wifi_sta_info_t sta_info;

    __DRV_TRY_LOCK(p_this);

    clients = (wiced_maclist_t*) buffer;
    clients->count = max_clients;
    memset(&ap_info, 0, sizeof(wl_bss_info_t));

    result = wwd_wifi_get_associated_client_list(clients, (uint16_t) size);
    if (result != WWD_SUCCESS) {
        AW_INFOF(("Failed to get client list" ENDL));
        __DRV_UNLOCK(p_this);
        return result;
    }

    current = &clients->mac_list[0];
    wwd_wifi_get_ap_info(&ap_info, &sec);

    while (clients->count > 0) {
        if (memcmp(current->octet, &(ap_info.BSSID), sizeof(wiced_mac_t)) != 0) {
            memset(&sta_info, 0, sizeof(sta_info));
            memcpy(sta_info.bssid.octet, current->octet, 6);
            wwd_wifi_get_ap_client_rssi((int32_t *) &sta_info.signal_strength,
                                        (wiced_mac_t *) &current->octet[0]);

            handler(p_arg, &sta_info, AW_FALSE);
        }
        --clients->count;
        ++current;
    }
    handler(p_arg, NULL, AW_TRUE);

    __DRV_UNLOCK(p_this);
    return AW_OK;
}

typedef struct {
    AW_SEMB_DECL(signal);
    aw_wifi_ssid_t    *p_ssid;
    aw_wifi_mac_t     *p_bssid;
    aw_wifi_ap_info_t *p_info;
    uint32_t           comp_level;
    aw_bool_t          success;
} __get_ap_info_user_data_t;

aw_local void __copy_ap_info (aw_wifi_ap_info_t     *p_to, 
                              aw_wifi_scan_result_t *p_from)
{
    p_to->ssid            = p_from->ssid;
    p_to->bssid           = p_from->bssid;
    p_to->signal_strength = p_from->signal_strength;
    p_to->max_data_rate   = p_from->max_data_rate;
    p_to->bss_type        = p_from->bss_type;
    p_to->security        = p_from->security;
    p_to->channel         = p_from->channel;
    p_to->band            = p_from->band;
}

aw_local aw_err_t __get_ap_info_cb (aw_wifi_scan_cb_result_t *p_result,
                                    aw_wifi_scan_status_t     status)
{
    aw_assert(p_result != NULL);
    __get_ap_info_user_data_t *p_arg = (__get_ap_info_user_data_t *) 
                                        p_result->p_arg;

    if (AW_WIFI_SCAN_INCOMPLETE != status) {
        AW_SEMB_GIVE(p_arg->signal);
    } else {
        if (p_arg->comp_level == 1) {
            if (memcmp(p_arg->p_ssid->value,
                       p_result->ap_details.ssid.value, 
                       p_arg->p_ssid->length) == 0) {
                __copy_ap_info(p_arg->p_info, &p_result->ap_details);
                p_arg->success = AW_TRUE;
            }
        } else if (p_arg->comp_level == 2) {
            if (memcmp(p_arg->p_bssid, &p_result->ap_details.bssid, 6) == 0) {
                __copy_ap_info(p_arg->p_info, &p_result->ap_details);
                p_arg->success = AW_TRUE;
            }
        } else if (p_arg->comp_level == 3) {
            if ((memcmp(p_arg->p_ssid->value, 
                        p_result->ap_details.ssid.value, 
                        p_arg->p_ssid->length) == 0) &&
                (memcmp(p_arg->p_bssid, &p_result->ap_details.bssid, 6) == 0)) {
                __copy_ap_info(p_arg->p_info, &p_result->ap_details);
                p_arg->success = AW_TRUE;
            }
        }
    }
    return AW_OK;
}

aw_err_t awbl_wiced_get_ap_info (void              *p_cookie,
                                 aw_wifi_ssid_t    *p_ssid,
                                 aw_wifi_mac_t     *p_bssid,
                                 aw_wifi_ap_info_t *p_info)
{
    aw_err_t err;
    __get_ap_info_user_data_t user_data;
    
    memset(&user_data,0,sizeof(user_data));
    user_data.comp_level = 0;
    if (p_ssid != NULL && p_ssid->length != 0) {
        user_data.comp_level |= 0x01;
    }
    if (p_bssid != NULL && !NULL_MAC(p_bssid->octet)) {
        user_data.comp_level |= 0x02;
    } else {
        p_bssid = NULL;
    }

    if (0 == user_data.comp_level) {
        err = -AW_EPERM;
    } else {
        /* scan */
        user_data.p_info  = p_info;
        user_data.p_ssid  = p_ssid;
        user_data.p_bssid = p_bssid;
        user_data.success = AW_FALSE;
        AW_SEMB_INIT(user_data.signal, AW_SEM_EMPTY, AW_SEM_Q_FIFO);

        /* todo : stop on target ap found */
        err = awbl_wiced_scan(p_cookie,
                              __get_ap_info_cb,
                              &user_data,
                              p_ssid,
                              p_bssid,
                              AW_WIFI_SCAN_TYPE_ACTIVE);
        if (AW_OK != err) {
            AW_SEMB_TERMINATE(user_data.signal);
            return err;
        }

        AW_SEMB_TAKE(user_data.signal, AW_SEM_WAIT_FOREVER);
        AW_SEMB_TERMINATE(user_data.signal);

        err = (AW_TRUE == user_data.success) ? AW_OK : AW_ERROR;
    }

    return err;
}

aw_err_t awbl_wiced_get_join_ap_info (void              *p_cookie,
                                      aw_wifi_ap_info_t *p_info)
{
    __DEV_DECL(p_this, p_cookie);
    aw_err_t err = AW_OK;

    wl_bss_info_t ap_info;
    wiced_security_t sec;

    __DRV_TRY_LOCK(p_this);

    memset(p_info, 0, sizeof(*p_info));

    if (WWD_SUCCESS == wwd_wifi_get_ap_info(&ap_info, &sec)) {
        int      i;
        uint32_t max_rate = 0;

        memcpy(p_info->ssid.value, ap_info.SSID, ap_info.SSID_len);
        memcpy(p_info->bssid.octet,
               ap_info.BSSID.octet, 
               sizeof(p_info->bssid.octet));
        p_info->ssid.length      = ap_info.SSID_len;
        p_info->signal_strength  = ap_info.RSSI;
        p_info->channel          = ap_info.ctl_ch;
        p_info->security         = __wiced_security_to_aw_security(sec);

        for (i = 0; i < ap_info.rateset.count; i++) {
            if (ap_info.rateset.rates[i] > max_rate) {
                max_rate = ap_info.rateset.rates[i];
            }
        }
        p_info->max_data_rate = max_rate * 500;
    }

    __DRV_UNLOCK(p_this);
    return err;
}

aw_err_t awbl_wiced_get_link_status (void                *p_cookie,
                                     aw_wifi_interface_t  interface,
                                     uint32_t            *p_status)
{
    __DEV_DECL(p_this, p_cookie);
    aw_err_t err = -AW_ENODEV;

    __DRV_TRY_LOCK(p_this);

    switch (interface) {

    case AW_WIFI_STA_INTERFACE:
        *p_status = 
            (WWD_SUCCESS == wwd_wifi_is_ready_to_transceive(WWD_STA_INTERFACE)) ? 
                1 : 0;
        err = AW_OK;
        break;

    case AW_WIFI_AP_INTERFACE:
        *p_status = 
            (WWD_SUCCESS == wwd_wifi_is_ready_to_transceive(WWD_AP_INTERFACE)) ? 
                1 : 0;
        err = AW_OK;
        break;

    default:
        break;
    }

    __DRV_UNLOCK(p_this);
    return err;
}

static aw_err_t awbl_wiced_power_off(void *p_cookie)
{
    wiced_result_t result;
    __DEV_DECL(p_this, p_cookie);

    if (!wiced_wlan_is_initialised()) {
        AW_INFOF(("WiFi already power off." ENDL));
        return -AW_EPERM;
    }

    aw_netif_set_link_down(&p_this->base.netif.sta.ethif);
    aw_netif_set_link_down(&p_this->base.netif.ap.ethif);

    result = wiced_wlan_connectivity_deinit();
    if (result != WICED_SUCCESS) {
        AW_ERRF(("Error %d while stoping WiFi!" ENDL, result));
    }

    return result;
}

aw_err_t awbl_wiced_dev_ioctl(void *p_cookie,
                              int   ops,
                              int   name,
                              void *p_arg1,
                              void *p_arg2)
{
    __DEV_DECL(p_this, p_cookie);

    aw_err_t err = AW_OK;
    wwd_result_t res;

    __DRV_TRY_LOCK(p_this);

    switch (ops) {

    case AW_WO_OPT_SET:
        switch (name) {

        case AW_WO_POWERSAVE:
            if ((int) p_arg1) {
                res = wwd_wifi_enable_powersave();
            } else {
                res = wwd_wifi_disable_powersave();
            }
            if (res != WWD_SUCCESS) {
                err = -AW_EIO;
            }
            break;

        case AW_WO_LISTEN_INTVAL: {
            uint8_t val = (uint8_t)(uint32_t) p_arg1;
            if (WWD_SUCCESS
                    != wwd_wifi_set_listen_interval(val,
                            WICED_LISTEN_INTERVAL_TIME_UNIT_BEACON)) {
                err = -AW_EIO;
            }
        }
            break;

        case AW_WO_TXPOWER: {
            uint8_t val = (uint8_t)(uint32_t) p_arg1;
            if (WWD_SUCCESS != wwd_wifi_set_tx_power(val)) {
                err = -AW_EIO;
            }
        }
            break;

        case AW_WO_80211N: {
            wwd_interface_t interface;

            err = __wifi_if_to_wwd_if(p_arg2, &interface);
            if (AW_OK == err) {
                wiced_11n_support_t value;
                value = (0 == (int) p_arg1) ? WICED_11N_SUPPORT_DISABLED :
                            WICED_11N_SUPPORT_ENABLED;
                if (WWD_SUCCESS != wwd_wifi_set_11n_support(interface, value)) {
                     err = -AW_EIO;
                }
            }
       }
            break;

        case AW_WO_COUNTRY_CODE: {
            extern wwd_result_t wwd_set_country (wiced_country_code_t code);
            if (WWD_SUCCESS != wwd_set_country((wiced_country_code_t) p_arg1)) {
                err = -AW_EIO;
            }
        }
            break;

        case AW_WO_CH: {
            wwd_interface_t interface;

            err = __wifi_if_to_wwd_if(p_arg2, &interface);
            if (AW_OK == err) {
                if (WWD_SUCCESS != 
                    wwd_wifi_set_channel(interface, (uint32_t) p_arg1)) {
                     err = -AW_EIO;
                }
            }
        }
            break;

        case AW_WO_MONITOR_MODE: {
            wwd_result_t result;

            if (AW_FALSE == (aw_bool_t) p_arg1) {
                p_this->base.cb.pfn_raw_packet_hdl = NULL;
                wwd_wifi_set_raw_packet_processor(NULL);
                result = wwd_wifi_disable_monitor_mode();
                if (WWD_SUCCESS != result) {
                    aw_mdelay(WWD_API_RETRY_DELAY);
                    AW_INFOF(("disable monitor error %d retry." ENDL, result));
                    result = wwd_wifi_disable_monitor_mode();
                    if (WWD_SUCCESS != result) {
                        AW_INFOF(("Can not disable monitor %d." ENDL, result));
                        err = -AW_EIO;
                    }
                }
            } else {
                result = wwd_wifi_enable_monitor_mode();
                if (WWD_SUCCESS != result) {
                    err = -AW_EIO;
                } else {
                    wwd_wifi_set_raw_packet_processor(__raw_pkt_processor);
                }
            }
        }
            break;

        case AW_WO_RAW_PACK_PROCESSOR:
            p_this->base.cb.pfn_raw_packet_hdl = (aw_wifi_raw_packet_processor_t) 
                                                  p_arg1;
            p_this->base.cb.p_raw_packet_arg = p_arg2;
            break;

        case AW_WO_AP_EVENT_HANDLER: {
            if (NULL == p_arg1) {
                err = wwd_management_set_event_handler(ap_client_events, 
                                                       NULL,
                                                       NULL, 
                                                       WWD_AP_INTERFACE);
            } else {
                p_this->base.cb.pfn_ap_event_hdl = p_arg1;
                err = wwd_management_set_event_handler(ap_client_events,
                                                       softap_event_handler,
                                                       p_arg2,
                                                       WWD_AP_INTERFACE);
            }
        }
            break;

        case AW_WO_FBT_OVER_DS: {
            wwd_interface_t interface;

            err = __wifi_if_to_wwd_if(p_arg2, &interface);
            if (AW_OK == err) {
                wwd_result_t result;
                int value = 0;

                result = wwd_wifi_fast_bss_transition_over_distribution_system(
                             interface,
                             WICED_TRUE,
                             &value);
                if (WWD_SUCCESS != result) {
                     err = -AW_EIO;
                } else {
                    AW_INFOF(("set/reset of Fast BSS Transition over DS success" ENDL));
                }
            }
        }
            break;

#ifdef WWD_LOGGING_STDOUT_ENABLE
        case AW_WO_DRIVER_LOG: {
            extern int wwd_logging_enabled;

            wwd_logging_enabled = (int) p_arg1;
        }
            break;
#endif /* ifdef WWD_LOGGING_STDOUT_ENABLE */

        case AW_WO_ROAM: {
            if (NULL != p_arg1) {
                uint32_t *p = (uint32_t *) p_arg1;

                wwd_wifi_set_roam_trigger((int32_t) p[0]);
                wwd_wifi_set_roam_delta((int32_t) p[1]);
                wwd_wifi_set_roam_scan_period(p[2]);
            }
        }
            break;

#if WIFI_PWRCTL_API_EN
        case AW_WO_PWR_CTL:
            if ((int) p_arg1) {
                uint32_t roam_param[3];
                __INF_DECL(p_info, p_cookie);

                if (p_info->base.roam_cfg && AW_FALSE != p_info->base.roam_cfg(roam_param)) {
                    res = awbl_wiced_power_on(p_info->base.country_code, roam_param);
                } else {
                    res = awbl_wiced_power_on(p_info->base.country_code, NULL);
                }
            } else {
                res = awbl_wiced_power_off(p_cookie);
            }
            if (res != WWD_SUCCESS) {
                err = -AW_EIO;
            }
            break;
#endif

        default :
            err = -AW_ENOSYS;
            break;
        }
        break;

    case AW_WO_OPT_GET:
        switch (name) {

        case AW_WO_POWERSAVE:
            *((int *) p_arg1) = wiced_wifi_get_powersave_mode();
            break;

        case AW_WO_LISTEN_INTVAL: {
            wiced_listen_interval_t listen_intval;

            if (WWD_SUCCESS != wwd_wifi_get_listen_interval(&listen_intval)) {
                err = -AW_EIO;
            } else {
                *((int *) p_arg1) = listen_intval.beacon;
            }
            break;
        }

        case AW_WO_TXPOWER:
            if (WWD_SUCCESS != wwd_wifi_get_tx_power((uint8_t *) p_arg1)) {
                err = -AW_EIO;
            }
            break;

        case AW_WO_COUNTRY_CODE:
        {
            extern wwd_result_t wwd_get_country (wiced_country_code_t *code);
            if (WWD_SUCCESS != 
                    wwd_get_country((wiced_country_code_t *) p_arg1)) {
                err = -AW_EIO;
            }
        }
            break;

        case AW_WO_PMK: {
            char *psk = (char *) p_arg1;
            uint8_t len = strlen(psk);

            if (WWD_SUCCESS != wwd_wifi_get_pmk(psk, len, psk)) {
                err = -AW_EIO;
            }
        }
            break;

        case AW_WO_MAX_ASSOC:
            if (WWD_SUCCESS != 
                    wwd_wifi_get_max_associations((uint32_t *) p_arg1)) {
                err = -AW_EIO;
            }
            break;

        case AW_WO_RATE: {
            wwd_interface_t interface;

            err = __wifi_if_to_wwd_if(p_arg2, &interface);
            if (AW_OK == err) {
                if (WWD_SUCCESS != 
                        wwd_wifi_get_rate(interface, (uint32_t *) p_arg1)) {
                     err = -AW_EIO;
                }
            }
        }
            break;

        case AW_WO_CH: {
            wwd_interface_t interface;

            err = __wifi_if_to_wwd_if(p_arg2, &interface);
            if (AW_OK == err) {
                if (WWD_SUCCESS != 
                        wwd_wifi_get_channel(interface, (uint32_t *) p_arg1)) {
                     err = -AW_EIO;
                }
            }
        }
            break;

        case AW_WO_MONITOR_MODE:
            if (WICED_TRUE == wwd_wifi_monitor_mode_is_enabled()) {
                *(aw_bool_t *) p_arg1 = AW_TRUE;
            } else {
                *(aw_bool_t *) p_arg1 = AW_FALSE;
            }
            break;

        case AW_WO_RAW_PACK_PROCESSOR:
            if (p_arg1) {
                *(aw_wifi_raw_packet_processor_t *) p_arg1 = 
                    p_this->base.cb.pfn_raw_packet_hdl;
            }
            if (p_arg2) {
                *(unsigned *) p_arg2 = 
                    (unsigned) p_this->base.cb.p_raw_packet_arg;
            }
            break;

        case AW_WO_AP_EVENT_HANDLER: {
            if (p_arg1) {
                *(uint32_t *) p_arg1 = 
                    (uint32_t) p_this->base.cb.pfn_ap_event_hdl;
            }
        }
            break;

        case AW_WO_FBT_OVER_DS: {
            wwd_interface_t interface;

            err = __wifi_if_to_wwd_if(p_arg2, &interface);
            if (AW_OK == err) {
                wwd_result_t result;
                int value = 0;

                result = wwd_wifi_fast_bss_transition_over_distribution_system(
                             interface,
                             WICED_FALSE,
                             &value);
                if (WWD_SUCCESS != result) {
                     err = -AW_EIO;
                     AW_INFOF(("wwd_wifi_fast_bss_transition_over_distribution_system.. Failed:%d" ENDL, \
                                   result));
                } else {
                    *(int *) p_arg1 = value;
                    AW_INFOF(("get Fast BSS Transition over DS success WLFTBT:%d" ENDL, \
                                 value));
                }
            }
        }
            break;

        case AW_WO_FBT_CAP: {
            wwd_interface_t interface;

            err = __wifi_if_to_wwd_if(p_arg2, &interface);
            if (AW_OK == err) {
                wwd_result_t result;
                wiced_bool_t enable;

                result = wwd_wifi_fast_bss_transition_capabilities(interface,
                                                                   &enable);
                if (WWD_SUCCESS != result) {
                    err = -AW_EIO;
                    AW_INFOF(("wwd_wifi_get_fbt_enable.. Failed:%d" ENDL, result));
                } else {
                    *(wiced_bool_t *) p_arg1 = enable;
                    AW_INFOF(("WLFBT Capabilities:%d" ENDL, enable));
                }
            }
        }
            break;

#ifdef WWD_ENABLE_STATS
        case AW_WO_PRINT_STATS:
            wwd_print_stats((wiced_bool_t) p_arg1);
            break;
#endif /* ifdef WWD_ENABLE_STATS */

#ifdef WWD_LOGGING_STDOUT_ENABLE
        case AW_WO_DRIVER_LOG: {
            extern int wwd_logging_enabled;

            *(int *) p_arg1 = wwd_logging_enabled;
        }
            break;
#endif /* ifdef WWD_LOGGING_STDOUT_ENABLE */

        case AW_WO_ROAM: {
            if (NULL != p_arg1) {
                uint32_t *p = (uint32_t *) p_arg1;

                wwd_wifi_get_roam_trigger((int32_t *) &p[0]);
                wwd_wifi_get_roam_delta((int32_t *) &p[1]);
                wwd_wifi_get_roam_scan_period(&p[2]);
            }
        }
            break;

        case AW_WO_DEV_STAT: {
            if (NULL != p_arg1) {
                *(uint32_t *) p_arg1 = p_this->base.serv.status.state;
            }
        }
            break;

#if WIFI_PWRCTL_API_EN
        case AW_WO_PWR_CTL:
            *((int *) p_arg1) = 0;
            break;
#endif

        default :
            err = -AW_ENOSYS;
            break;
        }
        break;

    default:
        err = -AW_ENOSYS;
        break;
    }

    __DRV_UNLOCK(p_this);
    return err;
}

aw_err_t awbl_wiced_config_apply (void             *p_cookie,
                                  aw_wifi_config_t *config,
                                  aw_wifi_status_t *status,
                                  aw_bool_t         force)
{
    aw_err_t err = AW_OK;

    if (AW_FALSE != force) {
        if (AW_WIFI_STATE_STA_JOINED & status->state) {
            aw_kprintf("force stop STA");
            err = awbl_wiced_leave(p_cookie);
            AW_BIT_CLR_MASK(status->state, AW_WIFI_STATE_STA_JOINED);
        }
        if (AW_WIFI_STATE_AP_ON & status->state) {
            aw_kprintf("force stop AP");
            err = awbl_wiced_stop_ap(p_cookie);
            AW_BIT_CLR_MASK(status->state, AW_WIFI_STATE_AP_ON);
        }
        status->last_opmode = AW_WIFI_MODE_STA_ONLY;
    } else {
        /* 旧有新无 -> 关闭 */
        if ((AW_WIFI_MODE_AP_ONLY & status->last_opmode) && 
            !(AW_WIFI_MODE_AP_ONLY & config->opmode)) {
            if (AW_WIFI_STATE_AP_ON & status->state) {
                aw_kprintf("stop AP");
                err = awbl_wiced_stop_ap(p_cookie);
                AW_BIT_CLR_MASK(status->state, AW_WIFI_STATE_AP_ON);
            }
        }
        if ((AW_WIFI_MODE_STA_ONLY & status->last_opmode) && 
            !(AW_WIFI_MODE_STA_ONLY & config->opmode)) {
            if (AW_WIFI_STATE_STA_JOINED & status->state) {
                aw_kprintf("stop STA");
                err = awbl_wiced_leave(p_cookie);
                AW_BIT_CLR_MASK(status->state, AW_WIFI_STATE_STA_JOINED);
            }
        }
    }

    switch (config->opmode) {

    case AW_WIFI_MODE_STOP: {
    }
        break;

    case AW_WIFI_MODE_STA_ONLY: {
        aw_wifi_security_t security;

        if (0 == config->sta.security_present) {
            aw_wifi_ap_info_t info;

            err = awbl_wiced_get_ap_info(p_cookie,
                                         &config->sta.ssid,
                                         &config->sta.bssid,
                                         &info);
            if (AW_OK != err) {
                break;
            }
            security = info.security;
        } else {
            security = config->sta.security;
        }
        err = awbl_wiced_join(p_cookie,
                              &config->sta.bssid,
                              &config->sta.ssid,
                              security,
                              &config->sta.key);
        if (AW_OK == err) {
            AW_BIT_SET_MASK(status->state, AW_WIFI_STATE_STA_JOINED);
        } else {
            AW_BIT_CLR_MASK(status->state, AW_WIFI_STATE_STA_JOINED);
        }
    }
        break;

    case AW_WIFI_MODE_AP_ONLY: {
        if (AW_WIFI_STATE_AP_ON & status->state) {
            int hide_ssid = config->ap.flags & AW_WIFI_AP_HIDDEN;

            err = awbl_wiced_start_ap(p_cookie,
                                      &config->ap.ssid,
                                      config->ap.security,
                                      &config->ap.key,
                                      config->ap.channel,
                                      hide_ssid);
            if (AW_OK == err) {
                AW_BIT_SET_MASK(status->state, AW_WIFI_STATE_AP_ON);
            } else {
                AW_BIT_CLR_MASK(status->state, AW_WIFI_STATE_AP_ON);
            }
        }
    }
        break;

    case AW_WIFI_MODE_APSTA: {
        if (!(AW_WIFI_STATE_AP_ON & status->state)) {
            int hide_ssid = config->ap.flags & AW_WIFI_AP_HIDDEN;

            err = awbl_wiced_start_ap(p_cookie,
                                      &config->ap.ssid,
                                      config->ap.security,
                                      &config->ap.key,
                                      config->ap.channel,
                                      hide_ssid);
            if (AW_OK == err) {
                AW_BIT_SET_MASK(status->state, AW_WIFI_STATE_AP_ON);
            } else {
                AW_BIT_CLR_MASK(status->state, AW_WIFI_STATE_AP_ON);
            }
        }

        if (!(AW_WIFI_STATE_STA_JOINED & status->state)) {
            aw_wifi_security_t security;

            if (0 == config->sta.security_present) {
                aw_wifi_ap_info_t info;

                err = awbl_wiced_get_ap_info(p_cookie,
                                             &config->sta.ssid,
                                             &config->sta.bssid,
                                             &info);
                if (AW_OK != err) {
                    break;
                }
                security = info.security;
            } else {
                security = config->sta.security;
            }
            err = awbl_wiced_join(p_cookie,
                                  &config->sta.bssid,
                                  &config->sta.ssid,
                                  security,
                                  &config->sta.key);
            if (AW_OK == err) {
                AW_BIT_SET_MASK(status->state, AW_WIFI_STATE_STA_JOINED);
            } else {
                AW_BIT_CLR_MASK(status->state, AW_WIFI_STATE_STA_JOINED);
            }
        }
    }
        break;

    default:
        err = -AW_ENOTSUP;
        break;
    }
    status->last_opmode = config->opmode;

    return err;
}

/*******************************************************************************
  ETHERNET
*******************************************************************************/

aw_local void wiced_if_link_init_internal (struct awbl_dev *p_dev,
                                           aw_wifi_mac_t   *p_mac)
{
    aw_local aw_const uint8_t default_mac[6] = {0x00, 0x14, 0x9f, 0x00, 0x01, 
                                                0x16};
    wiced_mac_t soc_addr;

    /* only WWD_STA_INTERFACE has set/get method */
    wwd_result_t result = wwd_wifi_get_mac_address(&soc_addr, WWD_STA_INTERFACE);

    (void) p_dev;
    if (__BOARD_MAC(p_mac->octet) || __NULL_MAC(p_mac->octet)) {
        /* invalid input: load from SoC */
        AW_INFOF(("No user MAC addr provide" ENDL));
        if (WWD_SUCCESS != result) {
            AW_ERRF(("Load from SoC failed(%d)! Use default." ENDL, result));
            memcpy(&soc_addr, default_mac, sizeof(soc_addr));
        }
        if (__BOARD_MAC(soc_addr.octet) || __NULL_MAC(soc_addr.octet)) {
            AW_ERRF(("SoC MAC invalid: " DBG_PRINT_ETHHWADDR_FMT ENDL \
                    "Use default." ENDL, \
                    DBG_PRINT_ETHHWADDR_VAL(soc_addr.octet)));
            memcpy(&soc_addr, default_mac, sizeof(soc_addr));
        }
        memcpy(p_mac, &soc_addr, sizeof(aw_wifi_mac_t));
    } else {
        /* valid input: set to SoC */
        wiced_mac_t user_addr;

        memcpy(&user_addr, p_mac, sizeof(user_addr));
        wwd_wifi_set_down();
        wwd_wifi_set_mac_address(user_addr);
        wwd_wifi_set_up();
        result = wwd_wifi_get_mac_address(&soc_addr, WWD_STA_INTERFACE);
        if (WWD_SUCCESS != result || 
            0 != memcmp(&user_addr, &soc_addr, sizeof(wiced_mac_t))) {
            /* set failed: use SoC content */
            memcpy(p_mac, &soc_addr, sizeof(aw_wifi_mac_t));
            AW_ERRF(("Read back from SoC failed(%d)!" ENDL \
                    "use SoC content " DBG_PRINT_ETHHWADDR_FMT ENDL, \
                    result, DBG_PRINT_ETHHWADDR_VAL(p_mac->octet)));
        }
    }

    AW_INFOF(("Wi-Fi MAC: " DBG_PRINT_ETHHWADDR_FMT ENDL, 
             DBG_PRINT_ETHHWADDR_VAL(p_mac->octet)));
}

aw_local aw_err_t wiced_if_tx (aw_netif_t *p_ethif, aw_net_buf_t *p)
{
    aw_bool_t up;
    __wiced_netif_t *p_wiced_if;

    if (0 == p->tot_len) {
        return AW_OK;
    }

    if (p->tot_len > WICED_PAYLOAD_MTU + PBUF_LINK_HLEN) {
        AW_ERRF(("Can not send packet, p->tot_len is too big!" ENDL));
        return AW_ERROR;
    }

    aw_netif_is_link_up(p_ethif, &up);
    if (AW_TRUE != up) {
        return AW_ERROR;
    }
    else {
#if ETH_PAD_SIZE
        struct pbuf *p_orig = p; /* anti-reclaim the padding word */
#endif

        p_wiced_if = 
            (__wiced_netif_t *) AW_CONTAINER_OF(p_ethif, __wiced_netif_t, ethif);
        if (WWD_SUCCESS == wwd_wifi_is_ready_to_transceive(p_wiced_if->interface)) {
            if (!((p->next == NULL) && ((p->tot_len == p->len)))) {
                struct pbuf *q;

                q = pbuf_alloc(PBUF_RAW,
                               p->tot_len + 
                                   WICED_LINK_OVERHEAD_BELOW_ETHERNET_FRAME_MAX, 
                               PBUF_POOL);
                if (q == NULL) {
                    AW_ERRF(("Can not send Chained packet, pbuf alloc failed!" ENDL));
                    return -AW_ENOMEM;
                }
                aw_assert(q->tot_len == q->len);
                pbuf_header(q, -WICED_LINK_OVERHEAD_BELOW_ETHERNET_FRAME_MAX);
                pbuf_copy(q, p);
                p = q;
            } else {
                /**
                 * sizeof(sdpcm_data_header_t) == 26
                 * WICED_LINK_OVERHEAD_BELOW_ETHERNET_FRAME_MAX = 12 + 18 = 30
                 */
                if ((u8_t) 0 != 
                    pbuf_header(p, WICED_LINK_OVERHEAD_BELOW_ETHERNET_FRAME_MAX)) {
                    /* todo: remove this */
                    struct pbuf *q;

                    /* PBUF_LINK_HLEN >= 26 */
                    q = pbuf_alloc(PBUF_RAW, 
                                   p->tot_len + 
                                       WICED_LINK_OVERHEAD_BELOW_ETHERNET_FRAME_MAX, 
                                   PBUF_POOL);

                    if (q == NULL) {
                        AW_ERRF(("Can not send Narrow packet, pbuf alloc failed!" ENDL));
                        return -AW_ENOMEM;
                    } else {
                        aw_assert(q->tot_len == q->len);
#if 0 
                        AW_WARNF(("Narrow packet, pbuf realloc" ENDL));
#endif 
                    }
                    pbuf_header(q,
                                -WICED_LINK_OVERHEAD_BELOW_ETHERNET_FRAME_MAX);
                    pbuf_copy(q, p);
                    p = q;
                } else {
                    pbuf_header(p, 
                                -WICED_LINK_OVERHEAD_BELOW_ETHERNET_FRAME_MAX);
                    /**
                     * Take a reference to original
                     * Waiting for WWD thread to eventually sent and than free it.
                     */
                    pbuf_ref(p);
                    AW_DBGF(("refer only" ENDL));
                }
            }
            wwd_network_send_ethernet_data(p, p_wiced_if->interface);
#if ETH_PAD_SIZE
            if (p_orig == p) {
                
                /* anti-reclaim the padding word */
                pbuf_header(p, -ETH_PAD_SIZE); 
            }
#endif

            return AW_OK;
        } else {
            AW_ERRF(("%s not ready, send %u failed" ENDL,
                      p_wiced_if->interface == WWD_STA_INTERFACE ? "STA" : "AP",
                      p->tot_len));
            return -AW_EAGAIN;
        }
    }
}

aw_local void wiced_if_release (aw_netif_t *p_ethif)
{
    awbl_cyw43455_sdio_release(p_ethif);
#if 0 
    mem_free(整个设备)
#endif 
}

/*******************************************************************************
  NIC
*******************************************************************************/
aw_err_t awbl_wiced_nic_set_link_up (awbl_dev_t          *p_dev,
                                     aw_wifi_interface_t  interface)
{
    __DEV_DECL(p_this, p_dev);

    switch (interface) {

    case AW_WIFI_STA_INTERFACE:
        AW_INFOF(("link_up(STA)" ENDL));
        aw_netif_set_link_up(&p_this->base.netif.sta.ethif);
        break;

    case AW_WIFI_AP_INTERFACE:
        AW_INFOF(("link_up(AP)" ENDL));
        aw_netif_set_link_up(&p_this->base.netif.ap.ethif);
        break;

    default:
        return AW_ERROR;
    }

    return AW_OK;
}

aw_err_t awbl_wiced_nic_set_link_down (awbl_dev_t          *p_dev,
                                       aw_wifi_interface_t  interface)
{
    __DEV_DECL(p_this, p_dev);
    switch (interface) {

    case AW_WIFI_STA_INTERFACE:
        AW_INFOF(("link_down(STA)" ENDL));
        aw_netif_set_link_down(&p_this->base.netif.sta.ethif);
        break;

    case AW_WIFI_AP_INTERFACE:
        AW_INFOF(("link_down(AP)" ENDL));
        aw_netif_set_link_down(&p_this->base.netif.ap.ethif);
        break;

    default:
        return AW_ERROR;
    }

    return AW_OK;
}

void awbl_wiced_eth_pkt_processor (awbl_dev_t     *p_dev,
                                   wiced_buffer_t  p,
                                   wwd_interface_t interface)
{
    struct eth_hdr* ethernet_header;
    struct netif*   tmp_netif = NULL;
    u8_t            result;
    uint16_t        ethertype;

    __DEV_DECL(p_this, p_dev);

    if (p == NULL) {
        return;
    }

    if (interface == WWD_AP_INTERFACE) {
        tmp_netif = &p_this->base.netif.ap.ethif.netif;
    } else if (interface == WWD_STA_INTERFACE) {
        tmp_netif = &p_this->base.netif.sta.ethif.netif;
    } else {
        goto __drop_exit;
    }

    PBUF_HEADER_RECLAIM_PAD(p); /* reclaim the padding word */
    /* points to packet payload, which starts with an Ethernet header */
    ethernet_header = (struct eth_hdr *) p->payload;

    ethertype = htons(ethernet_header->type);

    /* Check if this is an 802.1Q VLAN tagged packet */
    if (ethertype == WICED_ETHERTYPE_8021Q) {
        /* 
         * Need to remove the 4 octet VLAN Tag, by moving src and dest addresses
         * 4 octets to the right,and then read the actual ethertype. The VLAN ID 
         *and priority fields are currently ignored. 
         */
        uint8_t temp_buffer[12];
        memcpy(temp_buffer, p->payload, 12);
        memcpy(((uint8_t*) p->payload) + 4, temp_buffer, 12);

        p->payload = ((uint8_t*) p->payload) + 4;
        p->len = (u16_t) (p->len - 4);

        ethernet_header = (struct eth_hdr *) p->payload;
        ethertype = htons(ethernet_header->type);
    }

    switch (ethertype) {

        case WICED_ETHERTYPE_IPv4:
        case WICED_ETHERTYPE_ARP:
#if PPPOE_SUPPORT
        /* PPPoE packet? */
        case ETHTYPE_PPPOEDISC:
        case ETHTYPE_PPPOE:
#endif /* PPPOE_SUPPORT */

            /* Send to packet to tcpip_thread to process */
            if (tmp_netif->input(p, tmp_netif) != ERR_OK) {
                LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error" ENDL));

             /* Stop lint warning - packet has not been released in this case */
                /*@-usereleased@*/
                result = pbuf_free(p);
                /*@+usereleased@*/
                LWIP_ASSERT("Failed to release packet p", (result != (u8_t) 0));
                p = NULL;
            }
            break;

#ifdef ADD_LWIP_EAPOL_SUPPORT
        case WICED_ETHERTYPE_EAPOL:
            wwd_eapol_receive_eapol_packet(p, interface);
            break;
#endif
        default: {
__drop_exit:
            result = pbuf_free(p);
            LWIP_ASSERT("Failed to release packet p", (result != (u8_t) 0));
            p = NULL;
            break;
        }
    }
}

void awbl_wiced_inst_connect (struct awbl_dev *p_dev)
{
    aw_local aw_const aw_netif_ops_t ethif_ops = {
        NULL,
        NULL,
        NULL,
        wiced_if_tx,
        NULL,
        wiced_if_release,
    };
    aw_wifi_mac_t mac;
    uint32_t roam_param[3];
    int ret;
    wiced_result_t result;
    __INF_DECL(p_info, p_dev);
    __DEV_DECL(p_this, p_dev);

    AW_INFOF(("initializing cyw43455" ENDL));

    __DRV_LOCK_INIT(p_this);

    if (p_info->base.roam_cfg && AW_FALSE != p_info->base.roam_cfg(roam_param)) {
        result = awbl_wiced_pre_power_on(p_info->base.country_code, roam_param);
    } else {
        result = awbl_wiced_pre_power_on(p_info->base.country_code, NULL);
    }

    if (result != WICED_SUCCESS) {
        AW_ERRF(("Error %d while starting cyw43455!" ENDL, result));
        return;
    }
#ifdef AW_VDEBUG
    {
        char cap[256];
        if (wwd_wifi_get_cap(cap, sizeof(cap), NULL) == WWD_SUCCESS) {
            WPRINT_NETWORK_INFO(("WLAN Capabilities: %s" ENDL, cap));
        }
    }
#endif /* if AW_VDEBUG */

    memset(&mac, 0, sizeof(mac));
    if (p_info->base.ethif_infos.p_sta->get_hwaddr) {
        p_info->base.ethif_infos.p_sta->get_hwaddr(mac.octet,
                                                   NETIF_MAX_HWADDR_LEN);
    }
    wiced_if_link_init_internal(p_dev, &mac);

    ret = aw_netif_add(&p_this->base.netif.sta.ethif,
                       p_info->base.nic_names.p_sta,
                       &ethif_ops,
                       p_info->base.ethif_infos.p_sta,
                       AW_NETIF_TYPE_WIFI,
                       mac.octet,
                       WICED_PAYLOAD_MTU);

    ret = aw_netif_add(&p_this->base.netif.ap.ethif,
                       p_info->base.nic_names.p_ap,
                       &ethif_ops,
                       p_info->base.ethif_infos.p_ap,
                       AW_NETIF_TYPE_WIFI,
                       mac.octet,
                       WICED_PAYLOAD_MTU);

    (void) ret;

    p_this->base.serv.status.last_opmode = AW_WIFI_MODE_STA_ONLY;
    AW_BIT_SET_MASK(p_this->base.serv.status.state, AW_WIFI_STATE_UP);
}

/*******************************************************************************
  LOCAL
*******************************************************************************/
aw_inline aw_local aw_err_t __wifi_if_to_wwd_if (void            *wifiif, 
                                                 wwd_interface_t *interface)
{
    aw_err_t err = AW_OK;

    switch ((aw_wifi_interface_t) wifiif) {

    case AW_WIFI_STA_INTERFACE:
        *interface = WWD_STA_INTERFACE;
        break;

    case AW_WIFI_AP_INTERFACE:
        *interface = WWD_AP_INTERFACE;
        break;

    default :
        err = -AW_EIO;
        break;
    }

    return err;
}

/*******************************************************************************
  CALLBACK
*******************************************************************************/
aw_local void __raw_pkt_processor (wiced_buffer_t  buffer, 
                                   wwd_interface_t interface)
{

    __DEV_DECL(p_this, __gp_cyw43455_sdio_dev);

    if (NULL != p_this->base.cb.pfn_raw_packet_hdl) {
        p_this->base.cb.pfn_raw_packet_hdl(
            p_this->base.cb.p_raw_packet_arg,
            host_buffer_get_current_piece_data_pointer(buffer),
            host_buffer_get_current_piece_size(buffer));
    }

    host_buffer_release(buffer, WWD_NETWORK_RX);
}

aw_local void *softap_event_handler (const wwd_event_header_t *event_header,
                                     const uint8_t            *event_data,
                                     void                     *p_arg)
{
    __DEV_DECL(p_this, __gp_cyw43455_sdio_dev);

    aw_wifi_ap_event_t event = AW_WIFI_AP_UNKNOWN_EVENT;

    UNUSED_PARAMETER(event_data);

    if (p_this->base.cb.pfn_ap_event_hdl != NULL) {
        if (event_header->interface == WWD_AP_INTERFACE) {
            if ((event_header->event_type == WLC_E_DEAUTH)
                    || (event_header->event_type == WLC_E_DEAUTH_IND)
                    || (event_header->event_type == WLC_E_DISASSOC)
                    || (event_header->event_type == WLC_E_DISASSOC_IND)) {
                event = AW_WIFI_AP_STA_LEAVE_EVENT;
            } else if (event_header->event_type == WLC_E_ASSOC_IND
                    || event_header->event_type == WLC_E_REASSOC_IND) {
                event = AW_WIFI_AP_STA_JOINED_EVENT;
            }

            p_this->base.cb.pfn_ap_event_hdl(
                p_arg,
                event, 
                (const aw_wifi_mac_t *) &event_header->addr);
        }
    }

    return NULL;
}


/* end of file */

