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
 * \brief MINIPORT led + hc595驱动
 *
 *
 * \par 使用示例
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 14-12-19  ops, first implementation.
 * \endinternal
 */

#include "driver/board/awbl_miniport_hc595_led.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "driver/led/awbl_gpio_led.h"
#include "aw_gpio.h"
#include "aw_common.h"

#define __DEV_TO_MINIPORT_HC595_LED_DEV(p_dev) \
            AW_CONTAINER_OF(p_dev, struct awbl_miniport_hc595_led_dev, gpio_led_dev)

#define __DEV_GET_MINIPORT_HC595_LED_PAR(p_dev)   \
            ((void *)AWBL_DEVINFO_GET(p_dev))

#define __THIS_TO_MINIPORT_HC595_LED_DEV(p_this) \
            ((struct awbl_miniport_hc595_led_dev *)(p_this))



aw_local void __miniport_miniport_hc595_led_inst_init2 (struct awbl_dev *p_dev)
{
    uint8_t buf = 0;
    struct awbl_miniport_hc595_led_dev   *p_miniport_hc595_led  = \
             __DEV_TO_MINIPORT_HC595_LED_DEV((struct awbl_led_gpio_dev *)p_dev);

    struct awbl_miniport_hc595_led_param *p_par = \
            __DEV_GET_MINIPORT_HC595_LED_PAR(&p_miniport_hc595_led->gpio_led_dev.dev);

    /* 构造基于GPIO驱动的hc595设备 */
    awbl_gpio_hc595_dev_ctor(&p_miniport_hc595_led->gpio_hc595_dev, 
                             &p_par->gpio_hc595_par);

    /* 初始化hc595引脚 */
    aw_hc595_pin_init(&p_miniport_hc595_led->gpio_hc595_dev.hc595_dev);


    /* 初始化先熄灭LED灯 */
    buf = (p_par->active_low) ? 0xFF : 0x00;

    aw_hc595_lock_enable(&p_miniport_hc595_led->gpio_hc595_dev.hc595_dev);

    aw_hc595_shift(&p_miniport_hc595_led->gpio_hc595_dev.hc595_dev,
                   HC595_LSBFISRT,
                   buf);

    aw_hc595_lock_disable(&p_miniport_hc595_led->gpio_hc595_dev.hc595_dev);

    p_miniport_hc595_led ->hc595_laststatus = (p_par->active_low) ? ~buf : buf;

}


aw_local aw_err_t __miniport_hc595_led_set(void *p_cookie, int id, aw_bool_t on)
{
    struct awbl_miniport_hc595_led_dev   *p_miniport_hc595_led = \
                                    __THIS_TO_MINIPORT_HC595_LED_DEV(p_cookie);

    struct awbl_miniport_hc595_led_param *p_par = \
            __DEV_GET_MINIPORT_HC595_LED_PAR(&p_miniport_hc595_led->gpio_led_dev.dev);

    uint8_t buf = 0;

    int hc595_pin = 0;

    hc595_pin = id - p_par->led_servinfo.start_id;

    buf =  (on ^ p_par->active_low) ? (0xFF) : (~(1ul << hc595_pin));

    aw_hc595_lock_enable(&p_miniport_hc595_led->gpio_hc595_dev.hc595_dev);

    aw_hc595_shift(&p_miniport_hc595_led->gpio_hc595_dev.hc595_dev,
                   HC595_LSBFISRT,
                   buf);

    aw_hc595_lock_disable(&p_miniport_hc595_led->gpio_hc595_dev.hc595_dev);

    p_miniport_hc595_led ->hc595_laststatus = (p_par->active_low) ? ~buf : buf;

    return AW_OK;
}

aw_local aw_err_t __miniport_hc595_led_toggle(void *p_cookie, int id)
{
    struct awbl_miniport_hc595_led_dev  *p_miniport_hc595_led = \
                                    __THIS_TO_MINIPORT_HC595_LED_DEV(p_cookie);

    struct awbl_miniport_hc595_led_param *p_par  = \
             __DEV_GET_MINIPORT_HC595_LED_PAR(&p_miniport_hc595_led->gpio_led_dev.dev);

    int hc595_pin = 0;

    uint8_t buf = 0;


    hc595_pin = id - p_par->led_servinfo.start_id;

    if (p_miniport_hc595_led->hc595_laststatus & (1ul << hc595_pin)) {

        /* 表明翻转的是当前引脚且为点亮状态  */
        buf = (p_par->active_low) ? 0xFF : 0x00;
    } else {

        buf =  (p_par->active_low) ? (~(1ul << hc595_pin)) : (0xFF);
    }

    aw_hc595_lock_enable(&p_miniport_hc595_led->gpio_hc595_dev.hc595_dev);


    aw_hc595_shift(&p_miniport_hc595_led->gpio_hc595_dev.hc595_dev,
                   HC595_LSBFISRT,
                   buf);

    aw_hc595_lock_disable(&p_miniport_hc595_led->gpio_hc595_dev.hc595_dev);

    p_miniport_hc595_led ->hc595_laststatus = (p_par->active_low) ? ~buf : buf;

    return AW_OK;
}

aw_local aw_const struct awbl_led_servfuncs __g_led_servfuncs = {

        __miniport_hc595_led_set,
        __miniport_hc595_led_toggle
};

aw_local aw_err_t __hc595_ledserv_get (struct awbl_dev *p_dev, void *p_arg)
{
    struct awbl_miniport_hc595_led_dev   *p_miniport_hc595_led = \
             __DEV_TO_MINIPORT_HC595_LED_DEV((struct awbl_led_gpio_dev *)p_dev);

    struct awbl_miniport_hc595_led_param *p_par = \
            __DEV_GET_MINIPORT_HC595_LED_PAR(&p_miniport_hc595_led->gpio_led_dev.dev);

    struct awbl_led_service     *p_serv = \
                            &p_miniport_hc595_led->gpio_led_dev.led_serv;

    p_serv->p_next = NULL;
    p_serv->p_servinfo  = &p_par->led_servinfo;
    p_serv->p_servfuncs = &__g_led_servfuncs;
    p_serv->p_cookie    = (void *)p_miniport_hc595_led;

    *(struct awbl_led_service **)p_arg = p_serv;

    return AW_OK;
}

AWBL_METHOD_IMPORT(awbl_ledserv_get);

aw_local aw_const struct awbl_dev_method __g_miniport_hc595_led_dev_methods[] = {

        AWBL_METHOD(awbl_ledserv_get, __hc595_ledserv_get),
        AWBL_METHOD_END
};

aw_const struct awbl_drvfuncs __g_awbl_drvfuncs_miniport_hc595_led = {
    NULL,                                      /* devInstanceInit */
    __miniport_miniport_hc595_led_inst_init2,  /* devInstanceInit2 */
    NULL                                       /* devConnect */
};

aw_local aw_const struct awbl_drvinfo __g_drvinfo_miniport_hc595_led = {
    AWBL_VER_1,                                /* awb_ver         */
    AWBL_BUSID_PLB,                            /* bus_id          */
    MINIPORT_HC595_LED_NAME,                   /* p_drvname       */
    &__g_awbl_drvfuncs_miniport_hc595_led,     /* p_busfuncs      */
    &__g_miniport_hc595_led_dev_methods[0],    /* p_methods       */
    NULL                                       /* pfunc_drv_probe */
};

void awbl_miniport_miniport_hc595_led_drv_register (void)
{
    awbl_drv_register(&__g_drvinfo_miniport_hc595_led);
}

/* end of file*/