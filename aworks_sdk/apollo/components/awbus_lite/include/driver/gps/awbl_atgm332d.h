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
 * \brief ATGM332D BDS/GNSS全星座定位导航模块的驱动
 *
 * \par 使用示例
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 20-09-03  vih, first implementation.
 * \endinternal
 */

#ifndef __AWBL_ATGM332D_H
#define __AWBL_ATGM332D_H

#ifdef __cplusplus
extern "C" {
#endif

#include "awbl_gnss.h"
#include "awbl_nmea_0183_decode.h"
#include "aw_delayed_work.h"
#include "aw_time.h"
#include "aw_sys_defer.h"

/** \brief   atgm332d 驱动名 */
#define AWBL_ATGM332D_GNSS  "atgm332d_gnss"
/**
 * \brief   atgm332d GNSS 设备实例内部使用的配置参数
 */
struct awbl_atgm332d_dev_cfg_par {
    /** \brief 配置串口*/
    uint32_t                reserved;
    uint32_t                baud;

    /** \brief  卫星系统配置  */
    uint32_t                sys_mask;
};

/**
 * \brief  GNSS 设备实例信息
 */
typedef struct awbl_atgm332d_devinfo {

    struct awbl_gnss_com       com;

    /** \brief 服务配置信息 */
    struct awbl_gnss_servinfo  gnss_servinfo;

} awbl_atgm332d_devinfo_t;


/**
 * \brief  GNSS 设备实例
 */
typedef struct awbl_atgm332d_dev {

    /** \brief 驱动设备 */
    struct awbl_dev        super;

    /** \brief 设备的配置参数 */
    struct awbl_atgm332d_dev_cfg_par cfg_par;

    /** \brief GNSS 服务 */
    struct awbl_gnss_service gnss_serv;

    /** \brief 输出一次数据所用时间，单位ms */
    uint32_t                data_rate;

    struct aw_defer_djob    djob;

} awbl_atgm332d_dev_t;

void awbl_atgm332d_drv_register (void);

#ifdef __cplusplus
}
#endif

#endif  /* __AWBL_ATGM332D_H */

/* end of file */

