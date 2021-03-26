/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief 基于gpio的led驱动
 *
 * \internal
 * \par modification history:
 * - 1.00 14-11-22  ops, first implementation.
 * \endinternal
 */

#include "driver/led/awbl_gpio_led.h"
#include "aw_gpio.h"
#include "aw_common.h"

#define __DEV_TO_LED_GPIO_DEV(p_dev)   AW_CONTAINER_OF(p_dev, \
                                                       struct awbl_led_gpio_dev, \
                                                       dev)

#define __DEV_GET_LED_GPIO_PAR(p_dev)  ((void *)AWBL_DEVINFO_GET(p_dev))

#define __THIS_TO_LED_GPIO_DEV(p_this) ((struct awbl_led_gpio_dev *)(p_this))

aw_local aw_err_t __led_gpio_set(void *p_cookie, int id, aw_bool_t on)
{
    struct awbl_led_gpio_dev   *p_led_gpio = __THIS_TO_LED_GPIO_DEV(p_cookie);
    struct awbl_led_gpio_param *p_par      = __DEV_GET_LED_GPIO_PAR(&p_led_gpio->dev);

    int gpio_pin = 0;

    gpio_pin = id - p_par->led_servinfo.start_id;

    aw_gpio_set(p_par->p_led_gpios[gpio_pin], on ^ p_par->active_low);

    return AW_OK;
}

aw_local aw_err_t __led_gpio_toggle(void *p_cookie, int id)
{
    struct awbl_led_gpio_dev   *p_led_gpio = __THIS_TO_LED_GPIO_DEV(p_cookie);
    struct awbl_led_gpio_param *p_par      = __DEV_GET_LED_GPIO_PAR(&p_led_gpio->dev);

    int gpio_pin = 0;

    gpio_pin = id - p_par->led_servinfo.start_id;

    aw_gpio_toggle(p_par->p_led_gpios[gpio_pin]);

    return AW_OK;
}

aw_local aw_const struct awbl_led_servfuncs __g_led_servfuncs = {

        __led_gpio_set,
        __led_gpio_toggle
};

aw_local aw_err_t __gpio_ledserv_get (struct awbl_dev *p_dev, void *p_arg)
{
    struct awbl_led_gpio_dev   *p_led_gpio = __DEV_TO_LED_GPIO_DEV(p_dev);
    struct awbl_led_gpio_param *p_par      = __DEV_GET_LED_GPIO_PAR(p_dev);
    struct awbl_led_service  *p_serv       = &p_led_gpio->led_serv;

    p_serv->p_next = NULL;
    p_serv->p_servinfo = &p_par->led_servinfo;
    p_serv->p_servfuncs = &__g_led_servfuncs;
    p_serv->p_cookie = (void *)p_led_gpio;

    *(struct awbl_led_service **)p_arg = p_serv;

    return AW_OK;
}

AWBL_METHOD_IMPORT(awbl_ledserv_get);

aw_local aw_const struct awbl_dev_method __g_led_gpio_dev_methods[] = {

        AWBL_METHOD(awbl_ledserv_get, __gpio_ledserv_get),
        AWBL_METHOD_END
};

aw_local void __led_gpio_inst_init2 (struct awbl_dev *p_dev)
{
    struct awbl_led_gpio_param *p_par = __DEV_GET_LED_GPIO_PAR(p_dev);

    int i = 0, gpio_pin = 0;

    if (p_par->pfn_plfm_init != NULL) {
        p_par->pfn_plfm_init();
    }

    for (i = 0; i < p_par->num_leds; i++) {

        gpio_pin = p_par->p_led_gpios[i];
        aw_gpio_pin_cfg(gpio_pin, AW_GPIO_OUTPUT);
        aw_gpio_set(gpio_pin, p_par->active_low);
    }
}

aw_const struct awbl_drvfuncs __g_awbl_drvfuncs_led_gpio = {

        NULL,
        __led_gpio_inst_init2,
        NULL
};

aw_local aw_const struct awbl_drvinfo __g_drvinfo_led_gpio = {

        AWBL_VER_1,
        AWBL_BUSID_PLB,
        GPIO_LED_NAME,
        &__g_awbl_drvfuncs_led_gpio,
        &__g_led_gpio_dev_methods[0],
        NULL
};

void awbl_led_gpio_drv_register(void)
{
    awbl_drv_register(&__g_drvinfo_led_gpio);
}

/* end of file*/