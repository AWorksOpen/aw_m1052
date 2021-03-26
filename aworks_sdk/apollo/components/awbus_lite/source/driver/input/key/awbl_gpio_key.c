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
 * \brief 基于标准GPIO驱动的按键驱动
 *
 * \internal
 * \data modification history:
 * - 1.00 13-03-11  orz, first implementation.
 * \endinternal
 */

#include "driver/input/key/awbl_gpio_key.h"
#include "aw_compiler.h"
#include "aw_vdebug.h"
#include "aw_errno.h"
#include "aw_gpio.h"
#include "aw_int.h"
#include "aw_input.h"

/** \brief assert key GPIO */
static aw_bool_t __key_asserted (const struct gpio_key_data *data, int key)
{
    return aw_gpio_get(data->keymap[key].gpio) == 1 ? !data->active_low
                                                    : data->active_low;
}

/** \brief test if the driver is in interrupt mode */
aw_static_inline aw_bool_t __gpio_key_int_mode (const struct gpio_key_data *data)
{
    return (0 == data->scan_interval);
}

/** \brief GPIO keys int config */
static void __gpio_key_int_cfg (struct gpio_key_data *data, aw_bool_t on)
{
    int i;

    if (0 != data->clustered_irq) {
        if (on) {
            (void)aw_int_enable(data->clustered_irq);
        } else {
            (void)aw_int_disable(data->clustered_irq);
        }
    } else {
        for (i = 0; i < data->num_keys; i++) {
            if (on) {
                (void)aw_gpio_trigger_on(data->keymap[i].gpio);
            } else {
                (void)aw_gpio_trigger_off(data->keymap[i].gpio);
            }
        }
    }
}

static uint32_t __gpio_key_status_scan (struct gpio_key_data *data)
{
    uint32_t state = 0;
    int      key;

    /* assert each key and read the key status out */
    for (key = 0; key < data->num_keys; key++) {
        if (__key_asserted(data, key)) {
            state |= 1u << key;
        }
    }
    return state;
}

/** \brief scan the keys */
static void __gpio_key_scan (void *p_gkd)
{
    struct gpio_key_dev  *gkd  = p_gkd;
    struct gpio_key_data *data = to_gpio_key_data(&gkd->dev);
    uint32_t              new_state;
    uint32_t              bits_changed;
    int                   key;

    new_state    = __gpio_key_status_scan(data);
    bits_changed = gkd->key_state ^ new_state;

    if (0 != bits_changed) {

        for (key = 0; key < data->num_keys; key++) {
            if ((bits_changed & (1u << key)) == 0) {
                continue;
            }
            aw_input_report_key(AW_INPUT_EV_KEY, data->keymap[key].key,
                                new_state & (1u << key));
            AW_DBGF(("key%u %s\n", key, new_state & (1u << key) ? "down":"up"));

        }
        gkd->key_state = new_state;
    }

    /* if in interrupt mode, re-activate the interrupt */
    if (__gpio_key_int_mode(data)) {
        __gpio_key_int_cfg(data, AW_TRUE);
    }
}

/** \brief gpio key interrupt handler */
static void __gpio_key_irq (void *p_gkd)
{
    struct gpio_key_dev  *gkd  = p_gkd;
    struct gpio_key_data *data = to_gpio_key_data(&gkd->dev);

    if (__gpio_key_int_mode(data)) {
        /* interrupt mode */
        __gpio_key_int_cfg(data, AW_FALSE);
        (void)aw_delayed_work_start(&gkd->work, data->debounce_ms);
    } else {
        /* timer scanning period mode */
        __gpio_key_scan(gkd);
        (void)aw_delayed_work_start(&gkd->work, data->scan_interval);
    }
}

/** \brief initialize gpio key GPIOs */
static void __gpio_key_init_gpio (struct gpio_key_dev *gkd)
{
    struct gpio_key_data *data = to_gpio_key_data(&gkd->dev);
    int                   i;
    int                   pin;

    /* initializing key GPIOs as inputs */
    for (i = 0; i < data->num_keys; i++) {
        pin = data->keymap[i].gpio;
        aw_gpio_pin_request("key_gpios", &pin, 1);
        (void)aw_gpio_pin_cfg(data->keymap[i].gpio, AW_GPIO_INPUT);
    }
}

/** \brief initialize gpio key mode, interrupt or scanning period */
static void __gpio_key_init_mode (struct gpio_key_dev *gkd)
{
    struct gpio_key_data *data = to_gpio_key_data(&gkd->dev);
    int i, gpio, err;

    /* timer scanning period mode */
    if (!__gpio_key_int_mode(data)) {
        (void)aw_delayed_work_init(&gkd->work, __gpio_key_irq, gkd);
        (void)aw_delayed_work_start(&gkd->work, data->scan_interval);
        return;
    }

    /* interrupt mode */
    if (0 != data->clustered_irq) {
        (void)aw_int_connect(data->clustered_irq, __gpio_key_irq, gkd);
    } else {
        for (i = 0; i < data->num_keys; i++) {
            gpio = data->keymap[i].gpio;
            err  = aw_gpio_trigger_connect(gpio, __gpio_key_irq, gkd);
            if (AW_OK != err) {
                AW_DBGF(("gpio_trigger_connect(%d) fail: %d\n", gpio, err));
            }
            (void)aw_gpio_trigger_cfg(gpio, AW_GPIO_TRIGGER_BOTH_EDGES);
        }
    }
    (void)aw_delayed_work_init(&gkd->work, __gpio_key_scan, gkd);
    __gpio_key_int_cfg(data, AW_TRUE);
}

/******************************************************************************/

/** instance initializing stage 2 */
static void __gpio_key_inst_init2 (struct awbl_dev *p_dev)
{
    struct gpio_key_dev  *gkd   = to_gpio_key(p_dev);
    struct gpio_key_data *data  = to_gpio_key_data(p_dev);

    if (data->plat_init != NULL) {
        data->plat_init();
    }

    __gpio_key_init_gpio(gkd);

    /* set initial key status */
    gkd->key_state = __gpio_key_status_scan(data);

    __gpio_key_init_mode(gkd);

}

/******************************************************************************/

/** \brief driver functions (must defined as aw_const) */
aw_const struct awbl_drvfuncs __g_awbl_drvfuncs_gpio_key = {
    NULL,                   /* pfunc_dev_init1 */
    __gpio_key_inst_init2,  /* pfunc_dev_init2 */
    NULL                    /* pfunc_dev_connect */
};

/** driver registration (must defined as aw_const) */
aw_local aw_const struct awbl_drvinfo __g_drvinfo_gpio_key = {
    AWBL_VER_1,                     /* awb_ver */
    AWBL_BUSID_PLB,                 /* bus_id */
    GPIO_KEY_DEV_NAME,              /* p_drvname */
    &__g_awbl_drvfuncs_gpio_key,    /* p_busfuncs */
    NULL,                           /* p_methods */
    NULL                            /* pfunc_drv_probe */
};

/** \brief register GPIO key driver */
void awbl_gpio_key_drv_register (void)
{
    awbl_drv_register(&__g_drvinfo_gpio_key);
}

/* end of file */
