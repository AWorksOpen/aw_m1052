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

#ifndef __AWBL_HWCONF_SENSOR_LSM6DSL_H
#define __AWBL_HWCONF_SENSOR_LSM6DSL_H


#ifdef AW_DEV_SENSOR_LSM6DSL
#include "driver/sensor/awbl_lsm6dsl.h"

/** \brief 平台相关初始化 */
aw_local void __lsm6dsl_plfm_init (void)
{
    int lsm6dsl_gpios[] = {GPIO1_14, GPIO1_15};

    aw_gpio_pin_request("lsm6dsl_gpios",
                        lsm6dsl_gpios,
                        AW_NELEMENTS(lsm6dsl_gpios));
    aw_gpio_pin_cfg(GPIO1_14, AW_GPIO_INPUT);
    aw_gpio_pin_cfg(GPIO1_15, AW_GPIO_INPUT);
}

/** \brief LSM6DSL 设备信息 */
aw_local aw_const struct awbl_lsm6dsl_devinfo __g_lsm6dsl_devinfo = {

        SENSOR_LSM6DSL_START_ID,    /*< \brief 通道起始ID           */
        GPIO1_14,                   /*< \brief 传感器外部触发引脚1 */
        GPIO1_15,                   /*< \brief 传感器外部触发引脚2  */
        0x6B,                       /*< \brief IIC设备地址          */
        __lsm6dsl_plfm_init,        /*< \brief 注册引脚已经占用     */
        1,                          /*< \brief 三轴加速度高性能打开     */
        1,                          /*< \brief 三轴陀螺仪高性能打开     */
        0                           /*< \brief 三轴加速度偏移补偿因子选择     */
};

/** \brief LSM6DSL 设备实例内存静态分配 */
aw_local struct awbl_lsm6dsl_dev __g_lsm6dsl_dev;

#define AWBL_HWCONF_LSM6DSL                       \
    {                                             \
        AWBL_LSM6DSL_NAME,                        \
        0,                                        \
        AWBL_BUSID_I2C,                           \
        IMX1050_LPI2C1_BUSID,                     \
        (struct awbl_dev *)&__g_lsm6dsl_dev,      \
        &__g_lsm6dsl_devinfo                      \
    },

#else
#define AWBL_HWCONF_LSM6DSL

#endif  /* AW_DEV_SENSOR_LSM6DSL */

#endif  /* __AWBL_HWCONF_SENSOR_LSM6DSL_H */

/* end of file */
