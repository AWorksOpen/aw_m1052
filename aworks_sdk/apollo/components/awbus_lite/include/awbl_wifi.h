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
 * \brief Wi-Fi �豸�����ӿ�
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ� aw_wifi.h
 *
 * \par ʹ��ʾ��
 * \code
 * #include "aw_wifi.h"
 *
 * aw_wifi_scan(0, ...);        // ɨ���ȵ�
 * aw_wifi_start_ap(0, ...);    // ���� AP
 *
 * \endcode
 *
 * \internal
 * \par modification history
 * - 1.00 17-06-22  phd, first implementation
 * \endinternal
 */

#ifndef __AWBL_WIFI_H
#define __AWBL_WIFI_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_wifi
 * \copydoc aw_wifi.h
 * @{
 */

#include "aw_refcnt.h"
#include "aw_sem.h"
#include "aw_wifi_types.h"

/**
 * \brief AWBus wifi ����ʵ���������Ϣ
 */
typedef struct awbl_wifi_serv_inf {
    /** \brief �豸���� */
    char *name;
} awbl_wifi_serv_inf_t;

/**
 * \brief AWBus wifi ������
 */
typedef struct awbl_wifi_servops {
    /** \brief ɨ���ȵ� */
    aw_err_t (*pfn_scan) (void                    *p_cookie,
                          aw_wifi_scan_callback_t  pfn_cb,
                          void                    *p_arg,
                          aw_wifi_ssid_t          *p_ssid,
                          aw_wifi_mac_t           *p_bssid,
                          aw_wifi_scan_type_t      type);

    /** \brief ��ֹ���ڽ��е�ɨ�� */
    aw_err_t (*pfn_abort_scan) (void *p_cookie);

    /** \brief ����ָ���ȵ� */
    aw_err_t (*pfn_join) (void               *p_cookie,
                          aw_wifi_mac_t      *p_bssid,
                          aw_wifi_ssid_t     *p_ssid,
                          aw_wifi_security_t  security,
                          aw_wifi_key_t      *p_key);

    /** \brief �뿪�ȵ� */
    aw_err_t (*pfn_leave) (void *p_cookie);

    /** \brief ����AP */
    aw_err_t (*pfn_start_ap) (void               *p_cookie,
                              aw_wifi_ssid_t     *p_ssid,
                              aw_wifi_security_t  security,
                              aw_wifi_key_t      *p_key,
                              uint8_t             ch,
                              int                 hide_ssid);

    /** \brief �ر�AP */
    aw_err_t (*pfn_stop_ap) (void *p_cookie);

    /** \brief ��ȡ��·״̬ */
    aw_err_t (*pfn_get_link_status) (void                *p_cookie,
                                     aw_wifi_interface_t  interface,
                                     uint32_t            *p_status);

    /** \brief ��ȡָ��AP����Ϣ */
    aw_err_t (*pfn_get_ap_info) (void                *p_cookie,
                                 aw_wifi_ssid_t      *p_ssid,
                                 aw_wifi_mac_t       *p_bssid,
                                 aw_wifi_ap_info_t   *p_info);

    /** \brief ��ȡ�ѹ�����AP��Ϣ */
    aw_err_t (*pfn_get_join_ap_info) (void                *p_cookie,
                                      aw_wifi_ap_info_t   *p_info);

    /** \brief ��ȡ�ͻ����б� */
    aw_err_t (*pfn_get_sta_list) (void                             *p_cookie,
                                  aw_wifi_get_sta_result_handler_t  handler,
                                  void                             *p_arg);

    /** \brief �Ͽ��ѹ�����AP�Ŀͻ��� */
    aw_err_t (*pfn_deauth_sta) (void *p_cookie, aw_wifi_mac_t *p_mac);

    /** \brief ������� */
    aw_err_t (*pfn_dev_ioctl) (void *p_cookie,
                               int   ops,
                               int   name,
                               void *p_arg1,
                               void *p_arg2);

    /** \brief Ӧ������ */
    aw_err_t (*pfn_config_apply) (void             *p_cookie,
                                  aw_wifi_config_t *config,
                                  aw_wifi_status_t *status,
                                  aw_bool_t         force);
} awbl_wifi_servops_t;

/**
 * \brief wifi ����ṹ��
 */
typedef struct awbl_wifi_service {

    /** \brief ָ����һ������ */
    struct awbl_wifi_service *p_next;

    /** \brief wifi ����������Ϣ */
    const awbl_wifi_serv_inf_t *p_servinf;

    /** \brief wifi �������غ��� */
    const awbl_wifi_servops_t *p_servops;

    /** \brief ���� */
    aw_wifi_config_t config;

    /** \brief ״̬ */
    aw_wifi_status_t status;

    /** \brief ���ü��� */
    struct aw_refcnt ref;

    /**< \brief ���������������豸���� */
    void *p_cookie;

    /**< \brief ������ */
    AW_SEMB_DECL(oplock);

} awbl_wifi_service_t;

/**
 * \brief ע��һ�� wifi �豸
 * \param p_dev �豸ʵ��
 * \param p_inf �豸���ò���
 */
aw_err_t awbl_wifi_add (awbl_wifi_service_t        *p_dev, 
                        const awbl_wifi_serv_inf_t *p_inf);

/**
 * \brief �Ƴ�һ�� wifi �豸
 * \param p_dev �豸ʵ��
 */
aw_err_t awbl_wifi_remove (awbl_wifi_service_t *p_dev);

/**
 * \brief �Ƴ�ָ�����Ƶ�  wifi �豸
 * \param id �豸id
 */
aw_err_t awbl_wifi_remove_by_id (aw_wifi_id_t id);

/**
 * \brief ȡ�� wifi �豸����
 */
int awbl_wifi_get_num (void);

/** @} grp_aw_wifi */

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_WIFI_H */

/* end of file */
