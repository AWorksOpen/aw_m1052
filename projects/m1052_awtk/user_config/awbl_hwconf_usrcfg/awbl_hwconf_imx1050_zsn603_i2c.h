/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

#ifndef __AWBL_HWCONF_ZSN603_I2C_H__
#define __AWBL_HWCONF_ZSN603_I2C_H__

#include "imx1050_reg_base.h"
#include "imx1050_pin.h"
#include "imx1050_inum.h"
#include "driver/gpio/awbl_imx1050_gpio.h"
#include "driver/rfid/awbl_zsn603_i2c.h"
#include "driver/rfid/zsn603.h"
#include "aw_prj_params.h"
#include "aw_serial.h"
#ifdef   AW_DEV_I2C_ZSN603

/* ZSN603 (I2C mode) 设备信息 */
aw_local aw_const awbl_zsn603_i2c_devinfo_t __g_zsn603_i2c_devinfo = {
    0xb2,
    GPIO1_8
};

/* ZSN603 (I2C mode) 设备实例内存静态分配 */
aw_local awbl_zsn603_i2c_dev_t __g_zsn603_i2c_dev;

#define AWBL_HWCONF_ZSN603_I2C       \
    {                                \
        AWBL_ZSN603_I2C_NAME,        \
        0,                           \
        AWBL_BUSID_I2C,              \
        MM32F103_I2C1_BUSID,         \
       &__g_zsn603_i2c_dev.super.super,   \
       &__g_zsn603_i2c_devinfo      \
    },
#else
#define AWBL_HWCONF_ZSN603_I2C
#endif /* __AWBL_HWCONF_ZSN603_I2C_H__ */

#endif /* __AWBL_HWCONF_ZSN603_I2C_H__ */

/* end of file */
