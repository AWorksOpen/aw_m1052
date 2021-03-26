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
 * \brief AWBus-lite RTC PCF85263 驱动
 *
 * \internal
 * \par modification history:
 * - 1.00 18-08-13  mex, first implementation
 * \endinternal
 */

#ifndef __AWBL_PCF85263_H
#define __AWBL_PCF85263_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aworks.h"
#include "awbus_lite.h"
#include "awbl_i2cbus.h"
#include "awbl_rtc.h"


/** \brief PCF85263 驱动名 */
#define AWBL_PCF85263_NAME   "pcf85263"


/**
 * \brief PCF85263 设备信息
 */
typedef struct awbl_pcf85263_devinfo {

    /** \brief RTC 服务配置信息 */
    struct awbl_rtc_servinfo  rtc_servinfo;

    /** \brief 设备从机地址 */
    uint8_t addr;

} awbl_pcf85263_devinfo_t;

/**
 * \brief PCF85263 设备实例
 */
typedef struct awbl_pcf85263_dev {

    /** \brief 继承自 AWBus I2C device 设备 */
    struct awbl_i2c_device super;

    /** \brief RTC 服务 */
    struct awbl_rtc_service rtc_serv;

    /** \brief 设备锁 */
    AW_MUTEX_DECL(devlock);
    
} awbl_pcf85263_dev_t;


/**
 * \brief pcf85263 driver register
 */
void awbl_pcf85263_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_PCF85263_H */

/* end of file */
