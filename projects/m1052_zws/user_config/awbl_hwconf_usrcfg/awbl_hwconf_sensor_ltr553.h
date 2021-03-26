/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

#ifndef __AWBL_HWCONF_LTR553_H
#define __AWBL_HWCONF_LTR553_H


/******************************************************************************/
/** \brief LTR553设备描述符实体 */
#ifdef AW_DEV_SENSOR_LTR553

#include "imx1050_pin.h"

#include "driver/sensor/awbl_ltr553.h"

aw_local aw_const awbl_ltr553_devinfo_t __g_ltr553_0_devinfo = {
    SENSOR_LTR553_0_START_ID,
    GPIO1_22,
    LTR553_INTEGRATION_TIME_100MS,
    0X23,
    LTR553_GAIN_1X,
};

aw_local struct awbl_ltr553_dev __g_ltr553_0_dev;
#define AWBL_HWCONF_LTR553                      \
    {                                           \
        AWBL_LTR553_NAME,                       \
        0,                                      \
        AWBL_BUSID_I2C,                         \
        IMX1050_LPI2C1_BUSID,                   \
        (struct awbl_dev *)&__g_ltr553_0_dev,   \
        &__g_ltr553_0_devinfo                   \
    },
#else
#define AWBL_HWCONF_LTR553
#endif /* AW_DEV_EXTEND_LTR553 */

#endif

/* end of file */
