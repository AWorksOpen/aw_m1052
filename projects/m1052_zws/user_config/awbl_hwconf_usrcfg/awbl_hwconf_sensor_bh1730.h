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

#ifndef __AWBL_HWCONF_BH1730_H
#define __AWBL_HWCONF_BH1730_H


/******************************************************************************/
/** \brief BH1730设备描述符实体 */
#ifdef AW_DEV_SENSOR_BH1730

#include "imx1050_pin.h"

#include "driver/sensor/awbl_bh1730.h"

/** \brief BH1730设备信息 */
aw_local aw_const awbl_bh1730_devinfo_t __g_bh1730_0_devinfo = {
    SENSOR_BH1730_0_START_ID,
    GPIO1_23,
    0x29,
    BH1730_GAIN_1X,
};
/** \brief BH1730设备实例 */
aw_local struct awbl_bh1730_dev __g_bh1730_0_dev;

#define AWBL_HWCONF_BH1730                      \
    {                                           \
        AWBL_BH1730_NAME,                       \
        0,                                      \
        AWBL_BUSID_I2C,                         \
        IMX1050_LPI2C1_BUSID,                   \
        (struct awbl_dev *)&__g_bh1730_0_dev,   \
        &__g_bh1730_0_devinfo                   \
    },
#else
#define AWBL_HWCONF_BH1730
#endif /* AW_DEV_EXTEND_BH1730 */

#endif

/* end of file */
