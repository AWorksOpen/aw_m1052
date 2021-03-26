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

#ifndef __AWBL_HWCONF_GPIO_LED_H
#define __AWBL_HWCONF_GPIO_LED_H

#ifdef AW_DEV_GPIO_LED

/*******************************************************************************
    LED配置信息
*******************************************************************************/
#include "driver/led/awbl_gpio_led.h"
#include "aw_gpio.h"
#include "imx1050_pin.h"

/* LED设备GPIO信息 */
aw_local aw_const uint16_t __g_led_gpios[] = {
    GPIO1_19,     /* LED1 */
};

/* 平台初始化  */
aw_local void __led_gpio_init (void)
{
    int led_gpios[AW_NELEMENTS(__g_led_gpios)] ;
    int i;

    for (i = 0;i<AW_NELEMENTS(led_gpios);i++ ) {
        led_gpios[i] = __g_led_gpios[i];
    }

    aw_gpio_pin_request("led_gpios",
                         led_gpios,
                         AW_NELEMENTS(led_gpios));
}

/* LED设备信息 */
aw_local aw_const struct awbl_led_gpio_param __g_led_gpio_param = {
    __g_led_gpios,                      /**< \brief led 编号及对应gpio引脚 */
    {
        0,
        AW_NELEMENTS(__g_led_gpios) - 1
    },                                  /**< \brief led 起始编号和结束编号 */
    AW_NELEMENTS(__g_led_gpios),        /**< \brief led 数量 */
    AW_TRUE,                            /**< \brief led 极性 */
    __led_gpio_init
};

/* LED设备实例内存静态分配 */
aw_local struct awbl_led_gpio_dev __g_led_gpio_dev;

#define AWBL_HWCONF_GPIO_LED        \
    {                               \
        GPIO_LED_NAME,              \
        0,                          \
        AWBL_BUSID_PLB,             \
        0,                          \
        &(__g_led_gpio_dev.dev),    \
        &(__g_led_gpio_param)       \
    },

#else
#define AWBL_HWCONF_GPIO_LED
#endif  /* AW_DEV_GPIO_LED */

#endif  /* __AWBL_HWCONF_GPIO_LED_H */

/* end of file */
