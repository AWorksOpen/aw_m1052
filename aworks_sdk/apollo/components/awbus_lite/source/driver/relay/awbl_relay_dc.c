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
 * \brief driver for DC-relay driven by GPIO.
 *
 * \internal
 * \par modification history:
 * - 1.00 14-07-30  hbt, first implementation.
 * \endinternal
 */

#include "driver/relay/awbl_relay_dc.h"
#include "awbl_driver_if.h"
#include "aw_compiler.h"
#include "aw_gpio.h"

/******************************************************************************/

/** \brief the first relay device */
static struct dc_relay *__gp_relay_dev_first;

/******************************************************************************/

/**
 * \brief instance initializing stage 1
 */
static void __dc_relay_inst_init (struct awbl_dev *p_dev)
{
    struct dc_relay       *relay  = dev_to_dc_relay(p_dev);
    struct dc_relay_param *par = dev_get_dc_relay_par(p_dev);
    int                     i, gpio;

    /* initialize the relay GPIOs */
    for (i = 0; i < par->relay_num; i++) {
        gpio = par->relay_gpios[i];
        (void)aw_gpio_pin_cfg(gpio, AW_GPIO_OUTPUT);
        (void)aw_gpio_set(gpio, par->active_low);
    }

    /* Add relay device to the global list */
    relay->next          = __gp_relay_dev_first;
    __gp_relay_dev_first = relay;
}

/******************************************************************************/

/** \brief driver functions (must defined as aw_const) */
aw_const struct awbl_drvfuncs __g_awbl_drvfuncs_dc_relay = {
    __dc_relay_inst_init,       /**< \brief devInstanceInit */
    NULL,                       /**< \brief devInstanceInit2 */
    NULL                        /**< \brief devConnect */
};

/** \brief driver registration (must defined as aw_const) */
aw_local aw_const struct awbl_drvinfo __g_drvinfo_dc_relay = {
    AWBL_VER_1,                         /**< \brief awbl_ver */
    AWBL_BUSID_PLB,                     /**< \brief bus_id */
    DC_RELAY_NAME,                      /**< \brief p_drvname */
    &__g_awbl_drvfuncs_dc_relay,        /**< \brief p_busfuncs */
    NULL,                               /**< \brief p_methods */
    NULL                                /**< \brief pfunc_drv_probe */
};

/**
 * \brief register DC-Relay driver
 */
void awbl_dc_relay_drv_register (void)
{
    awbl_drv_register(&__g_drvinfo_dc_relay);
}

/******************************************************************************/

/**
 * \brief active/de-active relay
 */
void awbl_relay_set (unsigned int id, aw_bool_t on)
{
    struct dc_relay       *prelay;
    struct dc_relay_param *par;
    unsigned int           gpio;

    /* iterate each relay device to find the GPIO that driver the relay */
    for (prelay = __gp_relay_dev_first; NULL != prelay; prelay = prelay->next) {
        par = dev_get_dc_relay_par(&prelay->dev);
        if (id >= par->base_num) {
            gpio = id - par->base_num;
            if (gpio < par->relay_num) {
                /* the GPIO fond */
                aw_gpio_set(par->relay_gpios[gpio], on ^ par->active_low);
                break;
            }
        }
    }
}

/* end of file */
