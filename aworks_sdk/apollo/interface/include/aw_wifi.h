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
#ifndef __AW_WIFI_H
#define __AW_WIFI_H
#ifdef __cplusplus
extern "C" {
#endif
/**
 * \addtogroup grp_aw_if_wifi
 * \copydoc aw_wifi.h
 * @{
 */
#include "aw_wifi_types.h"

/**
 * \brief ioctl ops
 */
typedef enum {
    AW_WO_OPT_SET,
    AW_WO_OPT_GET,
} aw_wifi_ioctl_ops_t;

/**
 * \brief ioctl names
 */
typedef enum {
    AW_WO_NON               ,
    AW_WO_POWERSAVE         ,
    AW_WO_LISTEN_INTVAL     ,
    AW_WO_DTIM              ,
    AW_WO_TXPOWER           ,
    AW_WO_80211N            ,
    AW_WO_COUNTRY_CODE      ,
    AW_WO_PMK               ,
    AW_WO_MAX_ASSOC         ,
    AW_WO_RATE              ,
    AW_WO_CH                ,
    AW_WO_MONITOR_MODE      ,
    AW_WO_RAW_PACK_PROCESSOR,
    AW_WO_AP_EVENT_HANDLER  ,
    AW_WO_FBT_OVER_DS       ,
    AW_WO_FBT_CAP           ,
    AW_WO_DEV_STAT          ,
    AW_WO_PRINT_STATS       ,
    AW_WO_DRIVER_LOG        ,
    AW_WO_ROAM              ,
    AW_WO_PWR_CTL           ,
    AW_WO_AP_HIDDEN         ,
    AW_WO_PROMISC_MODE      ,
} aw_wifi_ioctl_names_t;

/**
 * \brief AWBus wifi ģ���ʼ��
 *
 * \attention ������Ӧ���� awbl_dev_connect()֮�����
 */
void aw_wifi_init (void);

/**
 * \brief ��ָ�����Ƶ�  wifi �豸
 * \param p_name �豸����
 */
aw_wifi_id_t aw_wifi_dev_open (const char *p_name);

/**
 * \brief ��ָ����ŵ�  wifi �豸
 * \param name �豸���
 */
aw_wifi_id_t aw_wifi_open_by_no (int no);

/**
 * \brief �ر��豸
 * \param id �豸
 */
aw_err_t aw_wifi_dev_close (aw_wifi_id_t id);

/**
 * \brief ��ȡ����ģʽ
 * \param id            �豸 ID
 * \param p_mode        ����ģʽ
 *
 * \return AW_OK        �����ɹ�
 *        -AW_ENODEV    �豸������
 */
aw_err_t aw_wifi_opmode_get (aw_wifi_id_t id, aw_wifi_opmode_t *p_mode);

/**
 * \brief ���ù���ģʽ
 * \param id            �豸 ID
 * \param mode          ����ģʽ
 *
 * \return AW_OK        �����ɹ�
 *        -AW_ENODEV    �豸������
 */
aw_err_t aw_wifi_opmode_set (aw_wifi_id_t id, aw_wifi_opmode_t mode);

/**
 * \brief ��ȡ���в���
 * \param id            �豸 ID
 * \param p_config      ���в���
 *
 * \return AW_OK        �����ɹ�
 *        -AW_ENODEV    �豸������
 */
aw_err_t aw_wifi_config_get (aw_wifi_id_t id, aw_wifi_config_t *p_config);

/**
 * \brief �������в���
 * \param id            �豸 ID
 * \param p_config      ���в���
 *
 * \return AW_OK        �����ɹ�
 *        -AW_ENODEV    �豸������
 */
aw_err_t aw_wifi_config_set (aw_wifi_id_t id, aw_wifi_config_t *p_config);

/**
 * \brief Ӧ�����в���
 * \param id            �豸 ID
 * \param force         ǿ���������ã�AP��رպ����¿�����STA������
 *
 * \return AW_OK        �����ɹ�
 *        -AW_ENODEV    �豸������
 */
aw_err_t aw_wifi_config_apply (aw_wifi_id_t id, aw_bool_t force);

/**
 * \brief ��ȡ����״̬
 * \param id            �豸 ID
 * \param p_status      ����״̬
 *
 * \return AW_OK        �����ɹ�
 *        -AW_ENODEV    �豸������
 */
aw_err_t aw_wifi_status_get (aw_wifi_id_t id, aw_wifi_status_t *p_status);

/**
 * \brief ɨ����������
 * \param id            �豸 ID
 * \param pfn_cb        ����ص�
 * \param p_arg         �ص�����
 * \param p_ssid        Ŀ��SSID
 * \param p_bssid       Ŀ��BSSID
 * \param type          ɨ������
 *
 * \return AW_OK        �����ɹ�
 *        -AW_ENODEV    �豸������
 */
aw_err_t aw_wifi_scan (aw_wifi_id_t                  id,
                       aw_wifi_scan_callback_t       pfn_cb,
                       void                         *p_arg,
                       aw_wifi_ssid_t               *p_ssid,
                       aw_wifi_mac_t                *p_bssid,
                       aw_wifi_scan_type_t           type);
                       
/**
 * \brief ��ֹ���ڽ��е�ɨ��
 * \param id            �豸 ID
 *
 * \return AW_OK        �����ɹ�
 *        -AW_ENODEV    �豸������
 */
aw_err_t aw_wifi_abort_scan (aw_wifi_id_t id);

/**
 * \brief ����ָ������
 *
 * \param id            �豸 ID
 * \param p_bssid       ��ַ
 * \param p_ssid        ��������
 * \param security      ��������
 * \param p_key         ��Կ
 *
 * \return AW_OK        �����ɹ�
 *        -AW_ENODEV    �豸������
 */
aw_err_t aw_wifi_join (aw_wifi_id_t        id,
                       aw_wifi_mac_t      *p_bssid,
                       aw_wifi_ssid_t     *p_ssid,
                       aw_wifi_security_t  security,
                       aw_wifi_key_t      *p_key);
                       
/**
 * \brief ��ѯ STA �Ƿ��Ѿ�������������
 * \param id            �豸 ID
 * \param is_joined     �Ѽ������ AW_TRUE
 *
 * \return AW_OK        �����ɹ�
 *        -AW_ENODEV    �豸������
 */
aw_err_t aw_wifi_is_sta_joined (aw_wifi_id_t id, aw_bool_t *is_joined);

/**
 * \brief �뿪��������
 * \param id            �豸 ID
 *
 * \return AW_OK        �����ɹ�
 *        -AW_ENODEV    �豸������
 */
aw_err_t aw_wifi_leave (aw_wifi_id_t id);

/**
 * \brief �����ȵ�
 * \param id            �豸 ID
 * \param p_ssid        ��������
 * \param security      ��������
 * \param p_key         ��Կ
 * \param ch            �ŵ�
 *
 * \return AW_OK        �����ɹ�
 *        -AW_ENODEV    �豸������
 *        -AW_ENOTSUP   �豸��֧�� AP ����
 */
aw_err_t aw_wifi_start_ap (aw_wifi_id_t        id,
                           aw_wifi_ssid_t     *p_ssid,
                           aw_wifi_security_t  security,
                           aw_wifi_key_t      *p_key,
                           uint8_t             ch);
                           
/**
 * \brief ��ѯ AP �Ƿ��Ѿ�����
 * \param id            �豸 ID
 * \param is_on         ��������� AW_TRUE
 *
 * \return AW_OK        �����ɹ�
 *        -AW_ENODEV    �豸������
 *        -AW_ENOTSUP   �豸��֧�� AP ����
 */
aw_err_t aw_wifi_is_ap_on (aw_wifi_id_t id, aw_bool_t *is_on);

/**
 * \brief �ر��ȵ�
 * \param id            �豸 ID
 *
 * \return AW_OK        �����ɹ�
 *        -AW_ENODEV    �豸������
 *        -AW_ENOTSUP   �豸��֧�� AP ����
 */
aw_err_t aw_wifi_stop_ap (aw_wifi_id_t id);

/**
 * \brief �Ͽ��ѹ�����AP�Ŀͻ���
 * \param id            �豸 ID
 * \param p_mac         ��Ҫ���Ͽ��Ŀͻ���BSSID(�����ݻ�����ȫΪ0ʱ�Ͽ�����)
 *
 * \return AW_OK        �����ɹ�
 *        -AW_ENODEV    �豸������
 */
aw_err_t aw_wifi_deauth_sta (aw_wifi_id_t id, aw_wifi_mac_t *p_mac);

/**
 * \brief ��ȡָ���ȵ���Ϣ
 *
 * ��ǰ�������һ�������������ȵ���Ϣ
 *
 * \param id            �豸 ID
 * \param p_ssid        SSID
 * \param p_bssid       BSSID
 * \param p_info        �ȵ���Ϣ
 *
 * \return AW_OK        �����ɹ�
 *        -AW_ENODEV    �豸������
 */
aw_err_t aw_wifi_get_ap_info (aw_wifi_id_t       id,
                              aw_wifi_ssid_t    *p_ssid,
                              aw_wifi_mac_t     *p_bssid,
                              aw_wifi_ap_info_t *p_info);
                              
/**
 * \brief ��ȡSTA�Ѽ����ȵ����Ϣ
 * \param id            �豸 ID
 * \param info          �ȵ���Ϣ
 *
 * \return AW_OK        �����ɹ�
 *        -AW_ENODEV    �豸������
 */
aw_err_t aw_wifi_get_join_ap_info (aw_wifi_id_t         id,
                                   aw_wifi_ap_info_t   *p_info);
                                   
/**
 * \brief ��ȡ�����ӿͻ����б�
 * \param id            �豸 ID
 * \param handler       ����ص�
 * \param p_arg         �ص�����
 *
 * \return AW_OK        �����ɹ�
 *        -AW_ENODEV    �豸������
 */
aw_err_t aw_wifi_get_sta_list (aw_wifi_id_t                      id,
                               aw_wifi_get_sta_result_handler_t  handler,
                               void                             *p_arg);
                               
/**
 * \brief ��ȡ Wi-Fi ��·״̬
 * \param id            �豸 ID
 * \param interface     �ӿ�
 * \param p_status      ��·״̬
 *
 * \return AW_OK        �����ɹ�
 *        -AW_ENODEV    �豸������
 */
aw_err_t aw_wifi_link_status_get (aw_wifi_id_t         id,
                                  aw_wifi_interface_t  interface,
                                  uint32_t            *p_status);
                                  
/**
 * \brief IOCTL
 * \param id            �豸 ID
 * \param ops           ����
 * \param name          ����
 * \param p_arg1        ����1
 * \param p_arg2        ����2
 *
 * \return AW_OK        �����ɹ�
 *         -AW_ENODEV   �豸������
 *         -AW_EIO      ����ʧ��
 */
aw_err_t aw_wifi_ioctl (aw_wifi_id_t  id,
                        int           ops,
                        int           name,
                        void         *p_arg1,
                        void         *p_arg2);
/** @} grp_aw_if_wifi */
#ifdef __cplusplus
}
#endif
#endif /* __AW_WIFI_H */

/* end of file */
