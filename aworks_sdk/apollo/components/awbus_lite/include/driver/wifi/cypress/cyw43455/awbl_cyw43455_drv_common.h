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
 * #include "awbl_cyw43455_common.h"
 * \endcode
 *
 * \internal
 * \par Modification History
 * - 1.00 19-04-17  mex, first implementation
 * \endinternal
 */

#ifndef __AWBL_CYW43455_DRV_COMMON_H
#define __AWBL_CYW43455_DRV_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * \addtogroup grp_awbl_cyw43455_drv_common
 * \copydoc awbl_cyw43455_drv_common.h
 * @{
 */

#include "aworks.h"
#include "aw_common.h"
#include "aw_delay.h"
#include "aw_gpio.h"
#include "aw_ioctl.h"
#include "aw_sem.h"
#include "aw_serial.h"
#include "aw_time.h"
#include "aw_wifi.h"
#include "awbl_wifi.h"
#include "awbus_lite.h"
#include <string.h>

#include "lwip/err.h"
#include "lwip/inet.h"
#include "netif/etharp.h"

#include "internal/wwd_internal.h"
#include "internal/wwd_thread.h"
#include "network/wwd_network_constants.h"
#include "platform/wwd_bus_interface.h"
#include "wiced_internal_api.h"
#include "wiced_utilities.h"
#include "wiced_wifi.h"
#include "wwd_wifi.h"
#include "wwd_wlioctl.h"

#include "awbl_cyw43455_common.h"

/******************************************************************************/

aw_err_t awbl_wiced_scan (void                    *p_cookie,
                          aw_wifi_scan_callback_t  pfn_cb,
                          void                    *p_arg,
                          aw_wifi_ssid_t          *p_ssid,
                          aw_wifi_mac_t           *p_bssid,
                          aw_wifi_scan_type_t      type);


aw_err_t awbl_wiced_abort_scan (void *p_cookie);


aw_err_t awbl_wiced_join (void               *p_cookie,
                          aw_wifi_mac_t      *p_bssid,
                          aw_wifi_ssid_t     *p_ssid,
                          aw_wifi_security_t  security,
                          aw_wifi_key_t      *p_key);

aw_err_t awbl_wiced_leave (void *p_cookie);


aw_err_t awbl_wiced_start_ap (void               *p_cookie,
                              aw_wifi_ssid_t     *p_ssid,
                              aw_wifi_security_t  security,
                              aw_wifi_key_t      *p_key,
                              uint8_t             ch,
                              int                 hide_ssid);

aw_err_t awbl_wiced_stop_ap (void *p_cookie);

aw_err_t awbl_wiced_get_sta_list (void                             *p_cookie,
                                  aw_wifi_get_sta_result_handler_t  handler,
                                  void                             *p_arg);

aw_err_t awbl_wiced_deauth_sta (void *p_cookie, aw_wifi_mac_t *p_mac);

aw_err_t awbl_wiced_get_ap_info (void              *p_cookie,
                                 aw_wifi_ssid_t    *p_ssid,
                                 aw_wifi_mac_t     *p_bssid,
                                 aw_wifi_ap_info_t *p_info);

aw_err_t awbl_wiced_get_join_ap_info (void *p_cookie, aw_wifi_ap_info_t *p_info);

aw_err_t awbl_wiced_get_link_status (void                *p_cookie,
                                     aw_wifi_interface_t  interface,
                                     uint32_t            *p_status);

aw_err_t awbl_wiced_dev_ioctl (void *p_cookie,
                               int   ops,
                               int   name,
                               void *p_arg1,
                               void *p_arg2);

aw_err_t awbl_wiced_config_apply (void             *p_cookie,
                                  aw_wifi_config_t *config,
                                  aw_wifi_status_t *status,
                                  aw_bool_t         force);

aw_err_t awbl_wiced_nic_set_link_up (awbl_dev_t         *p_dev, 
                                     aw_wifi_interface_t interface);

aw_err_t awbl_wiced_nic_set_link_down (awbl_dev_t         *p_dev, 
                                       aw_wifi_interface_t interface);

void awbl_wiced_eth_pkt_processor (awbl_dev_t     *p_dev,
                                   wiced_buffer_t  p,
                                   wwd_interface_t interface);

void awbl_wiced_inst_connect (struct awbl_dev *p_dev);


/** @} grp_awbl_cyw43455_drv_common */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __AWBL_CYW43455_DRV_COMMON_H */

/* end of file */
