/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief EC20 模块驱动头文件
 */

#ifndef __AWBL_GPRS_EC20_H__
#define __AWBL_GPRS_EC20_H__


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aworks.h"
#include "awbl_gprs.h"

#if AWBL_GPRS_USE_LWIP_2_X
#include "netif/ppp/ppp.h"
#else
#include "ppp/ppp.h"
#endif

#define AWBL_GPRS_EC20_NAME   "ec20"

/** \brief EC20设备状态 */
typedef enum ec20_status {
    /** \brief 模块启动复位 */
    AWBL_GPRS_EC20_RST_ON             = 0x00000001,
    /** \brief 内部协议栈的ppp开启  */
    AWBL_GPRS_EC20_GPRS_PPP_OK        = 0x00000001<<3,
    /** \brief gprs的PDP设置完成  */
    AWBL_GPRS_EC20_GPRS_PDP_OK        = 0x00000001<<5,
    /** \brief SIM卡正常 （如果sim卡正常，信号正常，却发不出短信，则欠费） */
    AWBL_GPRS_EC20_SIM_OK             = 0x00000001<<6,
} awbl_gprs_ec20_status_e;

/** \brief EC20设备信息*/
struct awbl_gprs_ec20_devinfo {

    /** \brief gprs设备的唯一ID，用于给中间层获取设备 */
    int      id;
    char    *buf;
    size_t   buf_size;

    /** \brief gprs模块的参数 */
    struct awbl_gprs_info       *p_gprs_info;

    /** \brief 短信的数据和信息 */
    struct aw_gprs_sms_buf      *p_sms_info;

    /** \brief 用户配置信息 */
    awbl_gprs_usbh_info         *p_usbh_info;

    void (*pfn_init) (void);
};

/** \brief EC20设备控制结构体 */
struct awbl_gprs_ec20_dev {
    struct awbl_dev                       dev;              /**< \brief 基础设备*/
    struct awbl_gprs_dev                  gprs;             /**< \brief GPRS结构体*/
    const struct awbl_gprs_ec20_devinfo  *p_devinfo;        /**< \brief EC20设备信息*/
    int                                   status;           /**< \brief 设备的状态 */
    void                                 *p_pipe;           /**< \brief EC20 AT管道句柄*/
    struct ip_addr                        ppp_ipaddr;       /**< \brief 网络侧分配的 IP地址 */
    AW_SEMB_DECL(                         lock);            /**< \brief 信号量 */

    uint8_t                               net_mode;
    uint8_t                               init_step;        /**< \brief 当前初始化所在步骤 */

    /** \brief 使用内部协议栈时的socket数量    */
    #define __INTERNAL_SOCKET_NUM         5
    /** \brief 使用内部协议栈时的socket的值   */
    #define __INTERNAL_SOCKET_VAL         {1,2,3,4,5}
    uint8_t                               socket[__INTERNAL_SOCKET_NUM];
    uint8_t                               socket_used[__INTERNAL_SOCKET_NUM];
    /** \brief GNSS 相关变量 */
    void                                 *gnss_pipe;        /**< \brief GNSS 相关管道*/
};

void awbl_gprs_ec20_drv_register (void);


/** @}  grp_aw_if_gprs_ec20 */

#ifdef __cplusplus
}
#endif /* __cplusplus   */

#endif /* __AWBL_GPRS_EC20_H__ */



