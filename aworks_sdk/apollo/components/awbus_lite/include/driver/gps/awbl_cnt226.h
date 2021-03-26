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
 * \brief CNT226 双模BDS/GPS的驱动
 *
 * \par 使用示例
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 16-12-30  vih, first implementation.
 * \endinternal
 */

#ifndef __AWBL_CNT226_H
#define __AWBL_CNT226_H

#ifdef __cplusplus
extern "C" {
#endif

#include "awbl_gnss.h"
#include "awbl_nmea_0183_decode.h"
#include "aw_delayed_work.h"
#include "aw_time.h"
#include "aw_sys_defer.h"

/** \brief   cnt226 驱动名 */
#define AWBL_CNT226_GNSS  "cnt226_gnss"

/**
 * \brief   cnt226 GNSS 设备实例内部使用的配置参数
 */
struct awbl_cnt226_dev_cfg_par {
    /** \brief 配置串口*/
    int                     uart_num;
    uint32_t                reserved;
    uint32_t                baud;
    uint32_t                inProto;
    uint32_t                outProto;

    /** \brief  消息输出频度配置  */
    uint32_t                msg_type;
    char                    msg_id;
    uint32_t                msg_rate;

    /** \brief  定位配置  */
    uint32_t                meas_rate;
    uint32_t                nav_rate;
    uint32_t                correction_mask;

    /** \brief  授时脉冲配置  */
    uint32_t                interval;
    uint32_t                length;
    uint32_t                flag;
    int                     ant_delay;
    int                     rf_delay;
    int                     usr_delay;

    /** \brief  外部触发事件配置  */
    uint32_t                enable;
    uint32_t                palarity;
    uint32_t                clock_sync;

    /** \brief  卫星系统配置  */
    uint32_t                sys_mask;

    /** \brief  动态配置  */
    uint32_t                dyn_mask;
    uint32_t                dyn_model;
    uint32_t                static_hold_thresh;

};

/**
 * \brief  GNSS 设备实例信息
 */
typedef struct awbl_cnt226_devinfo {

    struct awbl_gnss_com       com;

    /** \brief 服务配置信息 */
    struct awbl_gnss_servinfo  gnss_servinfo;

    int rst_pin;
    int en_pin;

} awbl_cnt226_devinfo_t;


/**
 * \brief  GNSS 设备实例
 */
typedef struct awbl_cnt226_dev {

    /** \brief 驱动设备 */
    struct awbl_dev        super;

    /** \brief 设备的配置参数 */
    struct awbl_cnt226_dev_cfg_par cfg_par;

    /** \brief GNSS 服务 */
    struct awbl_gnss_service gnss_serv;

    /** \brief 输出一次数据所用时间，单位ms */
    uint32_t                data_rate;

    struct aw_defer_djob    djob;

} awbl_cnt226_dev_t;


#ifdef __cplusplus
}
#endif

#endif  /* __AWBL_CNT226_H */

/* end of file */

