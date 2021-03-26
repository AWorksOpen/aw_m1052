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

#ifndef __AWBL_HWCONF_SENSOR_LIS3MDL_H
#define __AWBL_HWCONF_SENSOR_LIS3MDL_H


#ifdef AW_DEV_SENSOR_LIS3MDL
#include "driver/sensor/awbl_lis3mdl.h"

/** \brief 平台相关初始化 */
aw_local void __lis3mdl_plfm_init (void)
{
    int lis3mdl_gpios[] = {GPIO2_28, GPIO1_30};

    aw_gpio_pin_request("lis3mdl_gpios",
                        lis3mdl_gpios,
                        AW_NELEMENTS(lis3mdl_gpios));
    aw_gpio_pin_cfg(GPIO2_28, AW_GPIO_INPUT);
    aw_gpio_pin_cfg(GPIO1_30, AW_GPIO_INPUT);
}

/** \brief LIS3MDL 设备信息 */
aw_local aw_const struct awbl_lis3mdl_devinfo __g_lis3mdl_devinfo = {

        SENSOR_LIS3MDL_START_ID, /*< \brief 通道起始ID           */
        GPIO2_28,                /*< \brief 数据准备就绪触发引脚 */
        GPIO1_30,                /*< \brief 阈值触发引脚         */
        0x1C,                    /*< \brief IIC设备地址          */
        __lis3mdl_plfm_init      /*< \brief 注册引脚已经占用     */

};

/** \brief LIS3MDL 设备实例内存静态分配 */
aw_local struct awbl_lis3mdl_dev __g_lis3mdl_dev;

#define AWBL_HWCONF_LIS3MDL                       \
    {                                             \
        AWBL_LIS3MDL_NAME,                        \
        0,                                        \
        AWBL_BUSID_I2C,                           \
        IMX1050_LPI2C1_BUSID,                     \
        (struct awbl_dev *)&__g_lis3mdl_dev,      \
        &__g_lis3mdl_devinfo                      \
    },

#else
#define AWBL_HWCONF_LIS3MDL

#endif  /* AW_DEV_SENSOR_LIS3MDL */

#endif  /* __AWBL_HWCONF_SENSOR_LIS3MDL_H */

/* end of file */
