/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

#ifndef __AWBL_HWCONF_SENSOR_BMP280_H
#define __AWBL_HWCONF_SENSOR_BMP280_H


#ifdef AW_DEV_SENSOR_BMP280

#include "driver/sensor/awbl_bmp280.h"

/**
 * \addtogroup grp_aw_plfm_ext_bmp280_usrcfg
 * @{
 */

/* BMP280 设备信息 */
aw_local aw_const awbl_bmp280_param_t __g_bmp280_devinfo = {
    SENSOR_BMP280_START_ID,
    0x77,
};

/* bmp280 设备实例内存静态分配 */
aw_local awbl_bmp280_dev_t __g_bmp280_dev0;

#define AWBL_HWCONF_BMP280                    \
    {                                         \
        AWBL_BMP280_NAME,                     \
        0,                                    \
        AWBL_BUSID_I2C,                       \
        IMX1050_LPI2C1_BUSID,                 \
        (struct awbl_dev *)&__g_bmp280_dev0,  \
        &__g_bmp280_devinfo                   \
    },

/** @} */

#else
#define AWBL_HWCONF_BMP280
#endif

#endif
