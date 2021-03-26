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
 * \brief N58 模块驱动头文件
 */

#ifndef __AWBL_GPRS_N58_H__
#define __AWBL_GPRS_N58_H__


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

#define AWBL_GPRS_N58_NAME   "n58"

/** \brief N58设备状态 */
typedef enum n58_status {
    /** \brief 模块启动复位 */
    AWBL_GPRS_N58_RST_ON             = 0x00000001,
    /** \brief 内部协议栈的ppp开启  */
    AWBL_GPRS_N58_GPRS_PPP_OK        = 0x00000001<<3,
    /** \brief gprs的PDP设置完成  */
    AWBL_GPRS_N58_GPRS_PDP_OK        = 0x00000001<<5,
    /** \brief SIM卡正常 （如果sim卡正常，信号正常，却发不出短信，则欠费） */
    AWBL_GPRS_N58_SIM_OK             = 0x00000001<<6,
} awbl_gprs_n58_status_e;

/** \brief N58设备信息*/
struct awbl_gprs_n58_devinfo {

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

/** \brief N58设备控制结构体 */
struct awbl_gprs_n58_dev {
    struct awbl_dev                       dev;              /**< \brief 基础设备*/
    struct awbl_gprs_dev                  gprs;             /**< \brief GPRS结构体*/
    const struct awbl_gprs_n58_devinfo   *p_devinfo;        /**< \brief GM510设备信息*/
    int                                   status;           /**< \brief 设备的状态 */
    void                                 *p_pipe;           /**< \brief GM510 AT管道句柄*/
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

void awbl_gprs_n58_drv_register (void);


/** @}  grp_aw_if_gprs_n58 */

#ifdef __cplusplus
}
#endif /* __cplusplus   */

#endif /* __AWBL_GPRS_N58_H__ */



