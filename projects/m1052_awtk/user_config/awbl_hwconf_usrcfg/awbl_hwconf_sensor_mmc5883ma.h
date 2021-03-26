/*******************************************************************************
*                                 Apollo
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

#ifndef __AWBL_HWCONF_SENSOR_MMC5883MA_H
#define __AWBL_HWCONF_SENSOR_MMC5883MA_H

#ifdef AW_DEV_SENSOR_MMC5883MA

#include "driver/sensor/awbl_mmc5883ma.h"

/** \brief 平台初始化 */
aw_local void __mmc5883ma_plfm_init (void)
{
    int mmc5883ma_gpios[1] = {GPIO1_29};
    aw_gpio_pin_request("mmc5883ma_gpios",
                        mmc5883ma_gpios,
                        AW_NELEMENTS(mmc5883ma_gpios));
    aw_gpio_pin_cfg(GPIO1_29, AW_GPIO_INPUT);
}

/** \brief MMC5883MA 设备信息 */
aw_local aw_const struct awbl_mmc5883ma_devinfo __g_mmc5883ma_devinfo = {
        SENSOR_MMC5883MA_START_ID,    /*< \brief 通道起始ID           */
        0x30,                         /*< \brief IIC设备地址          */
        GPIO1_29,                     /*< \brief 中断触发引脚     */
        __mmc5883ma_plfm_init         /*< \brief 平台初始化 */
};

/** \brief MMC5883MA 设备实例内存静态分配 */
aw_local struct awbl_mmc5883ma_dev __g_mmc5883ma_dev;

#define AWBL_HWCONF_MMC5883MA                     \
    {                                             \
        AWBL_MMC5883MA_NAME,                      \
        0,                                        \
        AWBL_BUSID_I2C,                           \
        IMX1050_LPI2C1_BUSID,                     \
        (struct awbl_dev *)&__g_mmc5883ma_dev,    \
        &__g_mmc5883ma_devinfo                    \
    },
#else

#define AWBL_HWCONF_MMC5883MA

#endif  /* AW_DEV_SENSOR_MMC5883MA */

#endif  /* __AWBL_HWCONF_SENSOR_MMC5883MA_H */

/* end of file */

