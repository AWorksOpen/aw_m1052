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

#ifndef  __AWBL_HWCONF_GPIO_WDT_H
#define  __AWBL_HWCONF_GPIO_WDT_H

#ifdef  AW_DEV_GPIO_WDT

#include "driver/wdt/awbl_gpio_wdt.h"


aw_local void __gpio_wdt_plfm_init (void)
{
    int gpio_wdt_gpios[] = {
        GPIO5_2
    };

    aw_gpio_pin_request("gpio_wdt_gpios",
                        gpio_wdt_gpios,
                        AW_NELEMENTS(gpio_wdt_gpios));
}


/* GPIO看门狗设备信息 */
aw_local aw_const struct awbl_gpio_wdt_param __g_gpio_wdt_param = {
    GPIO5_2,                 /**< \brief 喂狗的 GPIO 引脚 */
    1000,                    /**< \brief WDT 周期(ms) */
    __gpio_wdt_plfm_init     /**< \brief 平台初始化 */
};

/* GPIO看门狗设备实例内存静态分配 */
aw_local struct awbl_gpio_wdt_dev __g_gpio_wdt_dev;

#define  AWBL_HWCONF_GPIO_WDT   \
    {                           \
        GPIO_WDT_NAME,          \
        0,                      \
        AWBL_BUSID_PLB,         \
        0,                      \
        &(__g_gpio_wdt_dev.dev),\
        &(__g_gpio_wdt_param)   \
    },

#else   /* AW_DEV_GPIO_WDT */
#define AWBL_HWCONF_GPIO_WDT

#endif

#endif  /* __AWBL_HWCONF_GPIO_WDT_H */
