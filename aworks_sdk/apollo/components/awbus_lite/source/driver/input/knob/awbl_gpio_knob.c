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
 * \brief 基于标准GPIO驱动的旋钮驱动
 *
 * \internal
 * \data modification history:
 * - 1.00 130819, orz, first implementation.
 * \endinternal
 */

#include "driver/input/knob/awbl_gpio_knob.h"
#include "aw_compiler.h"
#include "aw_vdebug.h"
#include "aw_errno.h"
#include "aw_gpio.h"
#include "aw_int.h"
#include "aw_input.h"

/******************************************************************************/
#define __KNOB_EVENT_NONE       0x00    /* no event */

#define __KNOB_EVENT_LEFT_0     0xB2    /* left event sequence 0 */
#define __KNOB_EVENT_LEFT_1     0x4D    /* left event sequence 1 */

#define __KNOB_EVENT_RIGHT_0    0xE8    /* right event sequence 0 */
#define __KNOB_EVENT_RIGHT_1    0x17    /* right event sequence 1 */

/******************************************************************************/

/** \brief assert knob GPIO */
static aw_bool_t __knob_asserted (const struct gpio_knob_data *data, 
								  int                          knob, 
								  int                          n)
{
    int gpio = data->knobmap[knob].kmap[n].gpio;

    return aw_gpio_get(gpio) == 1 ? !data->active_low : data->active_low;
}

/** \brief test if the driver is in interrupt mode */
aw_inline aw_bool_t __gpio_knob_int_mode (const struct gpio_knob_data *data)
{
    return (0 == data->scan_interval);
}

/** \brief GPIO knobs int config */
static void __gpio_knob_int_cfg (struct gpio_knob_data *data, aw_bool_t on)
{
    int i;

    if (0 != data->clustered_irq) {
        if (on) {
            (void)aw_int_enable(data->clustered_irq);
        } else {
            (void)aw_int_disable(data->clustered_irq);
        }
    } else {
        for (i = 0; i < data->num_knobs; i++) {
            if (on) {
                (void)aw_gpio_trigger_on(data->knobmap[i].kmap[0].gpio);
                (void)aw_gpio_trigger_on(data->knobmap[i].kmap[1].gpio);
            } else {
                (void)aw_gpio_trigger_off(data->knobmap[i].kmap[0].gpio);
                (void)aw_gpio_trigger_off(data->knobmap[i].kmap[1].gpio);
            }
        }
    }
}

static uint32_t __gpio_knob_status_scan (struct gpio_knob_data *data, int n)
{
    uint32_t state = 0;
    int      knob;

    /* assert each knob and read the knob status out */
    for (knob = 0; knob < data->num_knobs; knob++) {
        if (__knob_asserted(data, knob, n)) {
            state |= 1u << knob;
        }
    }
    return state;
}

/** \brief scan the knobs */
static void __gpio_knob_scan (void *p_gkn)
{
    struct gpio_knob_dev  *gkn  = p_gkn;
    struct gpio_knob_data *data = to_gpio_knob_data(&gkn->dev);
    uint32_t               new_state[2];
    unsigned int           oldl, oldr, newl, newr, bit;
    unsigned int           knob, event, event_seq;

    new_state[0] = __gpio_knob_status_scan(data, 0);
    new_state[1] = __gpio_knob_status_scan(data, 1);

    for (knob = 0; knob < data->num_knobs; knob++) {
        bit  = AW_BIT(knob);
        oldl = !!(gkn->knob_state[0] & bit);
        newl = !!(new_state[0]       & bit);
        oldr = !!(gkn->knob_state[1] & bit);
        newr = !!(new_state[1]       & bit);

        if ((oldl ^ newl) && (oldr ^ newr)) {   /* both Pin changes, fault */
            gkn->event[knob] = __KNOB_EVENT_NONE;

        } else if (oldl ^ newl ^ oldr ^ newr) { /* only one Pin changes */
            event = (oldl << 3) | (newl << 2) | (oldr << 1) | newr;
            if (__KNOB_EVENT_NONE == gkn->event[knob]) {
                gkn->event[knob] = event;
            } else {
                event_seq = (gkn->event[knob] << 4) | event;
                if ((__KNOB_EVENT_LEFT_0 == event_seq)
                        || (__KNOB_EVENT_LEFT_1 == event_seq)) {
                    uint_t key_code = data->knobmap[knob].kmap[0].key;
                            
                    /* 上报一个按键事件到输入子系统 */       
                    (void)awbl_input_report_key(AW_INPUT_EV_KEY, key_code, 1);
                    AW_DBGF(("knob%u trun left\n", knob));

                } else if ((__KNOB_EVENT_RIGHT_0 == event_seq)
                        || (__KNOB_EVENT_RIGHT_1 == event_seq)) {
                    uint_t key_code = data->knobmap[knob].kmap[1].key;
                            
                    /* 上报一个按键事件到输入子系统 */  
                    (void)awbl_input_report_key(AW_INPUT_EV_KEY, key_code, 1);
                    AW_DBGF(("knob%u trun right\n", knob));

                } else {
                    gkn->event[knob] = event;
                }
            }
        }
    }
    gkn->knob_state[0] = new_state[0];
    gkn->knob_state[1] = new_state[1];

    /* if in interrupt mode, re-activate the interrupt */
    if (__gpio_knob_int_mode(data)) {
        __gpio_knob_int_cfg(data, AW_TRUE);
    }
}

/** \brief gpio knob interrupt handler */
static void __gpio_knob_irq (void *p_gkn)
{
    struct gpio_knob_dev  *gkn  = p_gkn;
    struct gpio_knob_data *data = to_gpio_knob_data(&gkn->dev);

    if (__gpio_knob_int_mode(data)) {
        /* interrupt mode */
        __gpio_knob_int_cfg(data, AW_FALSE);
        (void)aw_delayed_work_start(&gkn->work, data->debounce_ms);
    } else {
        /* timer scanning period mode */
        __gpio_knob_scan(gkn);
        (void)aw_delayed_work_start(&gkn->work, data->scan_interval);
    }
}

/** \brief initialize gpio knob GPIOs */
static void __gpio_knob_init_gpio (struct gpio_knob_dev *gkn)
{
    struct gpio_knob_data *data = to_gpio_knob_data(&gkn->dev);
    int                   i;

    /* initializing knob GPIOs as inputs */
    for (i = 0; i < data->num_knobs; i++) {
        (void)aw_gpio_pin_cfg(data->knobmap[i].kmap[0].gpio, AW_GPIO_INPUT);
        (void)aw_gpio_pin_cfg(data->knobmap[i].kmap[1].gpio, AW_GPIO_INPUT);
    }
}

/** \brief initialize gpio knob mode, interrupt or scanning period */
static void __gpio_knob_init_mode (struct gpio_knob_dev *gkn)
{
    struct gpio_knob_data *data = to_gpio_knob_data(&gkn->dev);
    int i, j, gpio, err;

    /* timer scanning period mode */
    if (!__gpio_knob_int_mode(data)) {
        (void)aw_delayed_work_init(&gkn->work, __gpio_knob_irq, gkn);
        (void)aw_delayed_work_start(&gkn->work, data->scan_interval);
        return;
    }

    /* interrupt mode */
    if (0 != data->clustered_irq) {
        (void)aw_int_connect(data->clustered_irq, __gpio_knob_irq, gkn);
    } else {
        for (i = 0; i < data->num_knobs; i++) {
            for (j = 0; j < 2; j++) {
                gpio = data->knobmap[i].kmap[j].gpio;
                err  = aw_gpio_trigger_connect(gpio, __gpio_knob_irq, gkn);
                if (AW_OK != err) {
                    AW_DBGF(("gpio_trigger_connect(%d) fail: %d\n", gpio, err));
                }
                (void)aw_gpio_trigger_cfg(gpio, AW_GPIO_TRIGGER_BOTH_EDGES);
            }
        }
    }
    (void)aw_delayed_work_init(&gkn->work, __gpio_knob_scan, gkn);
    __gpio_knob_int_cfg(data, AW_TRUE);
}

/******************************************************************************/

/* instance initializing stage 2 */
static void __gpio_knob_inst_init2 (struct awbl_dev *p_dev)
{
    struct gpio_knob_dev  *gkn   = to_gpio_knob(p_dev);
    struct gpio_knob_data *data  = to_gpio_knob_data(p_dev);

    if (data->plat_init != NULL) {
        data->plat_init();
    }

    __gpio_knob_init_gpio(gkn);

    /* set initial knob status */
    gkn->knob_state[0] = __gpio_knob_status_scan(data, 0);
    gkn->knob_state[1] = __gpio_knob_status_scan(data, 1);

    __gpio_knob_init_mode(gkn);

}

/******************************************************************************/

/** \brief driver functions (must defined as aw_const) */
aw_const struct awbl_drvfuncs __g_awbl_drvfuncs_gpio_knob = {
    NULL,                   /* pfunc_dev_init1 */
    __gpio_knob_inst_init2, /* pfunc_dev_init2 */
    NULL                    /* pfunc_dev_connect */
};

/** driver registration (must defined as aw_const) */
aw_local aw_const struct awbl_drvinfo __g_drvinfo_gpio_knob = {
    AWBL_VER_1,                     /* awb_ver */
    AWBL_BUSID_PLB,                 /* bus_id */
    GPIO_KNOB_DEV_NAME,             /* p_drvname */
    &__g_awbl_drvfuncs_gpio_knob,   /* p_busfuncs */
    NULL,                           /* p_methods */
    NULL                            /* pfunc_drv_probe */
};

/** \brief register GPIO knob driver */
void awbl_gpio_knob_drv_register (void)
{
    awbl_drv_register(&__g_drvinfo_gpio_knob);
}

/* end of file */
