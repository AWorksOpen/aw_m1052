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
 * \brief Wi-Fi 设备操作接口
 *
 * 使用本服务需要包含头文件 aw_wifi.h
 *
 * \par 使用示例
 * \code
 * #include "aw_wifi.h"
 *
 * aw_wifi_scan(0, ...);        // 扫描热点
 * aw_wifi_start_ap(0, ...);    // 启动 AP
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
 * \brief AWBus wifi 服务实例的相关信息
 */
typedef struct awbl_wifi_serv_inf {
    /** \brief 设备名称 */
    char *name;
} awbl_wifi_serv_inf_t;

/**
 * \brief AWBus wifi 服务函数
 */
typedef struct awbl_wifi_servops {
    /** \brief 扫描热点 */
    aw_err_t (*pfn_scan) (void                    *p_cookie,
                          aw_wifi_scan_callback_t  pfn_cb,
                          void                    *p_arg,
                          aw_wifi_ssid_t          *p_ssid,
                          aw_wifi_mac_t           *p_bssid,
                          aw_wifi_scan_type_t      type);

    /** \brief 中止正在进行的扫描 */
    aw_err_t (*pfn_abort_scan) (void *p_cookie);

    /** \brief 加入指定热点 */
    aw_err_t (*pfn_join) (void               *p_cookie,
                          aw_wifi_mac_t      *p_bssid,
                          aw_wifi_ssid_t     *p_ssid,
                          aw_wifi_security_t  security,
                          aw_wifi_key_t      *p_key);

    /** \brief 离开热点 */
    aw_err_t (*pfn_leave) (void *p_cookie);

    /** \brief 创建AP */
    aw_err_t (*pfn_start_ap) (void               *p_cookie,
                              aw_wifi_ssid_t     *p_ssid,
                              aw_wifi_security_t  security,
                              aw_wifi_key_t      *p_key,
                              uint8_t             ch,
                              int                 hide_ssid);

    /** \brief 关闭AP */
    aw_err_t (*pfn_stop_ap) (void *p_cookie);

    /** \brief 获取链路状态 */
    aw_err_t (*pfn_get_link_status) (void                *p_cookie,
                                     aw_wifi_interface_t  interface,
                                     uint32_t            *p_status);

    /** \brief 获取指定AP的信息 */
    aw_err_t (*pfn_get_ap_info) (void                *p_cookie,
                                 aw_wifi_ssid_t      *p_ssid,
                                 aw_wifi_mac_t       *p_bssid,
                                 aw_wifi_ap_info_t   *p_info);

    /** \brief 获取已关联的AP信息 */
    aw_err_t (*pfn_get_join_ap_info) (void                *p_cookie,
                                      aw_wifi_ap_info_t   *p_info);

    /** \brief 获取客户端列表 */
    aw_err_t (*pfn_get_sta_list) (void                             *p_cookie,
                                  aw_wifi_get_sta_result_handler_t  handler,
                                  void                             *p_arg);

    /** \brief 断开已关联到AP的客户端 */
    aw_err_t (*pfn_deauth_sta) (void *p_cookie, aw_wifi_mac_t *p_mac);

    /** \brief 杂项操作 */
    aw_err_t (*pfn_dev_ioctl) (void *p_cookie,
                               int   ops,
                               int   name,
                               void *p_arg1,
                               void *p_arg2);

    /** \brief 应用配置 */
    aw_err_t (*pfn_config_apply) (void             *p_cookie,
                                  aw_wifi_config_t *config,
                                  aw_wifi_status_t *status,
                                  aw_bool_t         force);
} awbl_wifi_servops_t;

/**
 * \brief wifi 服务结构体
 */
typedef struct awbl_wifi_service {

    /** \brief 指向下一个服务 */
    struct awbl_wifi_service *p_next;

    /** \brief wifi 服务的相关信息 */
    const awbl_wifi_serv_inf_t *p_servinf;

    /** \brief wifi 服务的相关函数 */
    const awbl_wifi_servops_t *p_servops;

    /** \brief 配置 */
    aw_wifi_config_t config;

    /** \brief 状态 */
    aw_wifi_status_t status;

    /** \brief 引用计数 */
    struct aw_refcnt ref;

    /**< \brief 用于向驱动传递设备对象 */
    void *p_cookie;

    /**< \brief 操作锁 */
    AW_SEMB_DECL(oplock);

} awbl_wifi_service_t;

/**
 * \brief 注册一个 wifi 设备
 * \param p_dev 设备实例
 * \param p_inf 设备配置参数
 */
aw_err_t awbl_wifi_add (awbl_wifi_service_t        *p_dev, 
                        const awbl_wifi_serv_inf_t *p_inf);

/**
 * \brief 移除一个 wifi 设备
 * \param p_dev 设备实例
 */
aw_err_t awbl_wifi_remove (awbl_wifi_service_t *p_dev);

/**
 * \brief 移除指定名称的  wifi 设备
 * \param id 设备id
 */
aw_err_t awbl_wifi_remove_by_id (aw_wifi_id_t id);

/**
 * \brief 取得 wifi 设备数量
 */
int awbl_wifi_get_num (void);

/** @} grp_aw_wifi */

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_WIFI_H */

/* end of file */
