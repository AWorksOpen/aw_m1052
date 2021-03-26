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
 * \brief AWBus cyw43438 driver
 *
 * ʹ�ñ�ģ����Ҫ��������ͷ�ļ���
 * \code
 * #include "awbl_cyw43438_spi.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 20-04-16  hsg, first implementation
 * \endinternal
 */

#ifndef __AWBL_CYW43438_COMMON_H
#define __AWBL_CYW43438_COMMON_H

#include "aw_sem.h"
#include "aw_spinlock.h"
#include "awbl_spibus.h"
#include "awbl_wifi.h"

#include "aw_netif.h"

#include "driver/wifi/cypress/WICED/WWD/include/wwd_constants.h"
#include "driver/wifi/cypress/WICED/WWD/include/wwd_structures.h"


typedef struct __wiced_netif {
    /** \brief ethernet if */
    aw_netif_t ethif;
    /** \brief WICED interface */
    wwd_interface_t interface;
} __wiced_netif_t;


typedef struct __wifi_if {
    __wiced_netif_t sta;
    __wiced_netif_t ap;
} __wifi_if_t;


typedef struct __wifi_resource_hdl {
    aw_err_t (*pfn_size) (uint32_t *size_out);
    aw_err_t (*pfn_read) (uint32_t  offset,
                          void     *buffer,
                          uint32_t  size_in,
                          uint32_t *size_out);
} __wifi_resource_hdl_t;


typedef struct awbl_cyw43438_cfg_info {

    /** \brief cyw43438 ����������Ϣ  */
    awbl_wifi_serv_inf_t servinf;

    struct {
        const aw_netif_info_get_entry_t *p_sta;
        const aw_netif_info_get_entry_t *p_ap;
    } ethif_infos;

    struct {
        char *p_sta;
        char *p_ap;
    } nic_names;

    struct {
        __wifi_resource_hdl_t fw;
        __wifi_resource_hdl_t nv;
        __wifi_resource_hdl_t clm;
    } resource_hdls;

    aw_wifi_country_code_t country_code;

    int reset_pin;
    int irq_pin;

    aw_bool_t (*roam_cfg) (uint32_t *);
    aw_bool_t (*enable_sdio_int) (void *);
    aw_bool_t (*unmask_sdio_int) (void *);

} awbl_cyw43438_cfg_info_t;

typedef struct wwd_callback {
    /** \brief Soft AP event handler */
    aw_wifi_ap_event_handler_t pfn_ap_event_hdl;

    /** \brief �����ݽ��յĻص����û����� */
    aw_wifi_raw_packet_processor_t pfn_raw_packet_hdl;
    void *p_raw_packet_arg;

    /** \brief ɨ�������б�Ļص����û����� */
    aw_wifi_scan_callback_t pfn_scan_hdl;
    void *p_scan_arg;

} wwd_callback_t;

typedef struct cyw43438_dev_common {
    /** \brief wlan���� */
    awbl_wifi_service_t serv;

    /** \brief ���� */
    __wifi_if_t netif;

    /** \brief AP ���� */
    aw_wifi_ap_info_t local_ap_info;

    /** \brief �¼��ص��� */
    wwd_callback_t cb;

    AW_SEMB_DECL(op_lock);

} cyw43438_dev_common_t;

#endif /* __AWBL_CYW43438_COMMON_H */

/* end of file */

