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
    LED������Ϣ
*******************************************************************************/
#include "driver/led/awbl_gpio_led.h"
#include "aw_gpio.h"
#include "imx1050_pin.h"

/* LED�豸GPIO��Ϣ */
aw_local aw_const uint16_t __g_led_gpios[] = {
    GPIO1_19,     /* LED1 */
};

/* ƽ̨��ʼ��  */
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

/* LED�豸��Ϣ */
aw_local aw_const struct awbl_led_gpio_param __g_led_gpio_param = {
    __g_led_gpios,                      /**< \brief led ��ż���Ӧgpio���� */
    {
        0,
        AW_NELEMENTS(__g_led_gpios) - 1
    },                                  /**< \brief led ��ʼ��źͽ������ */
    AW_NELEMENTS(__g_led_gpios),        /**< \brief led ���� */
    AW_TRUE,                            /**< \brief led ���� */
    __led_gpio_init
};

/* LED�豸ʵ���ڴ澲̬���� */
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
