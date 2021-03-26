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

#ifndef __AWBL_HWCONF_GPIO_I2C0_H
#define __AWBL_HWCONF_GPIO_I2C0_H

#ifdef AW_DEV_GPIO_I2C_0

#include "aw_gpio.h"
#include "driver/busctlr/awbl_gpio_i2c.h"

aw_local void __awbl_gpio_i2c0_plfm_init (void);

/**
 * \brief IMX1050 GPIO I2C 总线速度
 *
 * 单位: Hz， 范围  1000 ~ 1000000 (1k ~ 1M)
 */
#define AW_CFG_GPIO_I2C_BUS_SPEED       200000

/** \brief 提供给驱动内部进行引脚输入输出的配置(可以用户自己设置SDA具体是开漏或者上拉)  */
aw_local void __awbl_sda_set_input(struct awbl_gpio_i2c_devinfo *p_dev_info)
{
    aw_gpio_pin_cfg(p_dev_info->pin_sda, AW_GPIO_INPUT | AW_GPIO_PULL_UP);
}

aw_local void __awbl_sda_set_output(struct awbl_gpio_i2c_devinfo *p_dev_info)
{
    aw_gpio_pin_cfg(p_dev_info->pin_sda, AW_GPIO_OUTPUT_INIT_HIGH | AW_GPIO_PULL_UP);
}

/* 设备信息 */
aw_local aw_const struct awbl_gpio_i2c_devinfo __g_gpio_i2c0_devinfo = {
    {
        IMX1050_GPIO_I2C0_BUSID,       /**< \brief 控制器所对应的总线编号 */
        AW_CFG_GPIO_I2C_BUS_SPEED,     /**< \brief 控制器总线速度 */
        AWBL_I2C_WAITFOREVER,          /**< \brief 超时 */
    },

    GPIO1_16,                           /**< \brief SCL引脚 */
    GPIO1_17,                           /**< \brief SDA引脚 */

    500000,

    __awbl_sda_set_input,
    __awbl_sda_set_output,
    __awbl_gpio_i2c0_plfm_init         /**< \brief 平台相关初始化 */
};

/* 平台相关初始化 */
aw_local void __awbl_gpio_i2c0_plfm_init (void)
{
    int gpio_i2c0[2];

    gpio_i2c0[0] = __g_gpio_i2c0_devinfo.pin_sda;
    gpio_i2c0[1] = __g_gpio_i2c0_devinfo.pin_scl;

    if (aw_gpio_pin_request("gpio_i2c0",
                            gpio_i2c0,
                            AW_NELEMENTS(gpio_i2c0)) == AW_OK) {
        aw_gpio_pin_cfg(gpio_i2c0[0], AW_GPIO_OUTPUT_INIT_HIGH | AW_GPIO_PULL_UP );
        aw_gpio_pin_cfg(gpio_i2c0[1], AW_GPIO_OUTPUT_INIT_HIGH | AW_GPIO_PULL_UP );
    }
}

/* 设备实例内存静态分配 */
aw_local struct awbl_gpio_i2c_dev __g_gpio_i2c0_dev;

#define AWBL_HWCONF_GPIO_I2C0                   \
    {                                           \
        AWBL_GPIO_I2C_NAME,                     \
        0,                                      \
        AWBL_BUSID_PLB,                         \
        0,                                      \
        &__g_gpio_i2c0_dev.super.super.super,   \
        &__g_gpio_i2c0_devinfo                  \
    },

#else
#define AWBL_HWCONF_GPIO_I2C0

#endif /* AW_DEV_GPIO_I2C_0 */

#endif /* __AWBL_HWCONF_GPIO_I2C0_H */

/* end of file */
