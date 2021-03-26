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
 * \brief 基于 GPRS 模块的 GNSS 驱动
 *
 * \par 使用示例
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 20-07-09  vih, first implementation.
 * \endinternal
 */

#ifndef __AWBL_GNSS_GPRS_H
#define __AWBL_GNSS_GPRS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "apollo.h"
#include "awbus_lite.h"
#include "awbl_gnss.h"
#include "aw_gnss.h"
#include "aw_gprs.h"

#include "awbl_nmea_0183_decode.h"
#include "aw_delayed_work.h"
#include "aw_time.h"
#include "aw_sys_defer.h"

/** \brief  驱动名 */
#define AWBL_GNSS_GPRS  "gnss_gprs"

/**
 * \brief GNSS 设备状态
 */
typedef enum {
    AWBL_GNSS_GPRS_STS_PWR_OFF = 0,
    AWBL_GNSS_GPRS_STS_PWR_OFF_PRE,
    AWBL_GNSS_GPRS_STS_PWR_ON,
    AWBL_GNSS_GPRS_STS_RUNNING,
} awbl_gnss_gprs_sts_t;

/**
 * \brief  GNSS 设备实例信息
 */
typedef struct awbl_gnss_gprs_devinfo {

    /** \brief 使用 gprs 设备的 id  */
    int                         gprs_dev_id;

    aw_gprs_gnss_cfg_t          gnss_cfg;

    /** \brief 服务配置信息 */
    struct awbl_gnss_servinfo   gnss_servinfo;

} awbl_gnss_gprs_devinfo_t;


/**
 * \brief  GNSS 设备实例
 */
typedef struct awbl_gnss_gprs_dev {

    /** \brief 驱动设备 */
    struct awbl_dev          super;

    /** \brief GNSS 服务 */
    struct awbl_gnss_service gnss_serv;

    /** \brief gprs 状态，只有在 PWR_ON 时才可使用 gnss 功能 */
    awbl_gnss_gprs_sts_t       gnss_sts;

    struct awbl_gnss_gprs_dev *p_next;
} awbl_gnss_gprs_dev_t;

void awbl_gnss_gprs_drv_register (void);

#endif

