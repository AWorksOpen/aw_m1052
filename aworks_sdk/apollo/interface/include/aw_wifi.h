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
 * \brief AWBus wifi 模块初始化
 *
 * \attention 本函数应当在 awbl_dev_connect()之后调用
 */
void aw_wifi_init (void);

/**
 * \brief 打开指定名称的  wifi 设备
 * \param p_name 设备名称
 */
aw_wifi_id_t aw_wifi_dev_open (const char *p_name);

/**
 * \brief 打开指定序号的  wifi 设备
 * \param name 设备序号
 */
aw_wifi_id_t aw_wifi_open_by_no (int no);

/**
 * \brief 关闭设备
 * \param id 设备
 */
aw_err_t aw_wifi_dev_close (aw_wifi_id_t id);

/**
 * \brief 获取工作模式
 * \param id            设备 ID
 * \param p_mode        工作模式
 *
 * \return AW_OK        操作成功
 *        -AW_ENODEV    设备不存在
 */
aw_err_t aw_wifi_opmode_get (aw_wifi_id_t id, aw_wifi_opmode_t *p_mode);

/**
 * \brief 设置工作模式
 * \param id            设备 ID
 * \param mode          工作模式
 *
 * \return AW_OK        操作成功
 *        -AW_ENODEV    设备不存在
 */
aw_err_t aw_wifi_opmode_set (aw_wifi_id_t id, aw_wifi_opmode_t mode);

/**
 * \brief 获取运行参数
 * \param id            设备 ID
 * \param p_config      运行参数
 *
 * \return AW_OK        操作成功
 *        -AW_ENODEV    设备不存在
 */
aw_err_t aw_wifi_config_get (aw_wifi_id_t id, aw_wifi_config_t *p_config);

/**
 * \brief 设置运行参数
 * \param id            设备 ID
 * \param p_config      运行参数
 *
 * \return AW_OK        操作成功
 *        -AW_ENODEV    设备不存在
 */
aw_err_t aw_wifi_config_set (aw_wifi_id_t id, aw_wifi_config_t *p_config);

/**
 * \brief 应用运行参数
 * \param id            设备 ID
 * \param force         强制重新配置，AP会关闭后重新开启，STA会重连
 *
 * \return AW_OK        操作成功
 *        -AW_ENODEV    设备不存在
 */
aw_err_t aw_wifi_config_apply (aw_wifi_id_t id, aw_bool_t force);

/**
 * \brief 获取运行状态
 * \param id            设备 ID
 * \param p_status      运行状态
 *
 * \return AW_OK        操作成功
 *        -AW_ENODEV    设备不存在
 */
aw_err_t aw_wifi_status_get (aw_wifi_id_t id, aw_wifi_status_t *p_status);

/**
 * \brief 扫描无线网络
 * \param id            设备 ID
 * \param pfn_cb        结果回调
 * \param p_arg         回调参数
 * \param p_ssid        目标SSID
 * \param p_bssid       目标BSSID
 * \param type          扫描类型
 *
 * \return AW_OK        操作成功
 *        -AW_ENODEV    设备不存在
 */
aw_err_t aw_wifi_scan (aw_wifi_id_t                  id,
                       aw_wifi_scan_callback_t       pfn_cb,
                       void                         *p_arg,
                       aw_wifi_ssid_t               *p_ssid,
                       aw_wifi_mac_t                *p_bssid,
                       aw_wifi_scan_type_t           type);
                       
/**
 * \brief 中止正在进行的扫描
 * \param id            设备 ID
 *
 * \return AW_OK        操作成功
 *        -AW_ENODEV    设备不存在
 */
aw_err_t aw_wifi_abort_scan (aw_wifi_id_t id);

/**
 * \brief 加入指定网络
 *
 * \param id            设备 ID
 * \param p_bssid       地址
 * \param p_ssid        网络名称
 * \param security      加密类型
 * \param p_key         密钥
 *
 * \return AW_OK        操作成功
 *        -AW_ENODEV    设备不存在
 */
aw_err_t aw_wifi_join (aw_wifi_id_t        id,
                       aw_wifi_mac_t      *p_bssid,
                       aw_wifi_ssid_t     *p_ssid,
                       aw_wifi_security_t  security,
                       aw_wifi_key_t      *p_key);
                       
/**
 * \brief 查询 STA 是否已经加入无线网络
 * \param id            设备 ID
 * \param is_joined     已加入输出 AW_TRUE
 *
 * \return AW_OK        操作成功
 *        -AW_ENODEV    设备不存在
 */
aw_err_t aw_wifi_is_sta_joined (aw_wifi_id_t id, aw_bool_t *is_joined);

/**
 * \brief 离开无线网络
 * \param id            设备 ID
 *
 * \return AW_OK        操作成功
 *        -AW_ENODEV    设备不存在
 */
aw_err_t aw_wifi_leave (aw_wifi_id_t id);

/**
 * \brief 开启热点
 * \param id            设备 ID
 * \param p_ssid        网络名称
 * \param security      加密类型
 * \param p_key         密钥
 * \param ch            信道
 *
 * \return AW_OK        操作成功
 *        -AW_ENODEV    设备不存在
 *        -AW_ENOTSUP   设备不支持 AP 功能
 */
aw_err_t aw_wifi_start_ap (aw_wifi_id_t        id,
                           aw_wifi_ssid_t     *p_ssid,
                           aw_wifi_security_t  security,
                           aw_wifi_key_t      *p_key,
                           uint8_t             ch);
                           
/**
 * \brief 查询 AP 是否已经启动
 * \param id            设备 ID
 * \param is_on         已启动输出 AW_TRUE
 *
 * \return AW_OK        操作成功
 *        -AW_ENODEV    设备不存在
 *        -AW_ENOTSUP   设备不支持 AP 功能
 */
aw_err_t aw_wifi_is_ap_on (aw_wifi_id_t id, aw_bool_t *is_on);

/**
 * \brief 关闭热点
 * \param id            设备 ID
 *
 * \return AW_OK        操作成功
 *        -AW_ENODEV    设备不存在
 *        -AW_ENOTSUP   设备不支持 AP 功能
 */
aw_err_t aw_wifi_stop_ap (aw_wifi_id_t id);

/**
 * \brief 断开已关联到AP的客户端
 * \param id            设备 ID
 * \param p_mac         将要被断开的客户端BSSID(无内容或内容全为0时断开所有)
 *
 * \return AW_OK        操作成功
 *        -AW_ENODEV    设备不存在
 */
aw_err_t aw_wifi_deauth_sta (aw_wifi_id_t id, aw_wifi_mac_t *p_mac);

/**
 * \brief 获取指定热点信息
 *
 * 当前仅输出第一个满足条件的热点信息
 *
 * \param id            设备 ID
 * \param p_ssid        SSID
 * \param p_bssid       BSSID
 * \param p_info        热点信息
 *
 * \return AW_OK        操作成功
 *        -AW_ENODEV    设备不存在
 */
aw_err_t aw_wifi_get_ap_info (aw_wifi_id_t       id,
                              aw_wifi_ssid_t    *p_ssid,
                              aw_wifi_mac_t     *p_bssid,
                              aw_wifi_ap_info_t *p_info);
                              
/**
 * \brief 获取STA已加入热点的信息
 * \param id            设备 ID
 * \param info          热点信息
 *
 * \return AW_OK        操作成功
 *        -AW_ENODEV    设备不存在
 */
aw_err_t aw_wifi_get_join_ap_info (aw_wifi_id_t         id,
                                   aw_wifi_ap_info_t   *p_info);
                                   
/**
 * \brief 获取已连接客户端列表
 * \param id            设备 ID
 * \param handler       结果回调
 * \param p_arg         回调参数
 *
 * \return AW_OK        操作成功
 *        -AW_ENODEV    设备不存在
 */
aw_err_t aw_wifi_get_sta_list (aw_wifi_id_t                      id,
                               aw_wifi_get_sta_result_handler_t  handler,
                               void                             *p_arg);
                               
/**
 * \brief 获取 Wi-Fi 链路状态
 * \param id            设备 ID
 * \param interface     接口
 * \param p_status      链路状态
 *
 * \return AW_OK        操作成功
 *        -AW_ENODEV    设备不存在
 */
aw_err_t aw_wifi_link_status_get (aw_wifi_id_t         id,
                                  aw_wifi_interface_t  interface,
                                  uint32_t            *p_status);
                                  
/**
 * \brief IOCTL
 * \param id            设备 ID
 * \param ops           操作
 * \param name          名称
 * \param p_arg1        参数1
 * \param p_arg2        参数2
 *
 * \return AW_OK        操作成功
 *         -AW_ENODEV   设备不存在
 *         -AW_EIO      操作失败
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
