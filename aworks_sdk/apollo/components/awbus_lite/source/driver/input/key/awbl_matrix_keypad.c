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
 * \brief 矩阵键盘驱动
 *
 * \internal
 * \par modification history:
 * - 1.00 13-02-26  orz, first implementation.
 * \endinternal
 */

#include "driver/input/key/awbl_matrix_keypad.h"
#include "aw_delayed_work.h"
#include "aw_compiler.h"
#include "aw_vdebug.h"
#include "aw_errno.h"
#include "aw_gpio.h"
#include "aw_int.h"
#include <string.h>
#include "awbl_input.h"
#include "aw_input.h"

/**
 * \brief activate/de-activate the column GPIO
 * \note if active_low flag is set, the GPIO has to be set output high to
 *       de-activate the column GPIO
 */
static void __col_activate (const struct matrix_keypad_par *par,
                            int                             col,
                            aw_bool_t                       on)
{
    (void)aw_gpio_set(par->col_gpios[col],
                      on ? !par->active_low : par->active_low);
}

/** \brief activate all column GPIOs */
static void __all_cols_activate (const struct matrix_keypad_par *par, 
                                 aw_bool_t                       on)
{
    int col;

    for (col = 0; col < par->num_col; col++) {
        __col_activate(par, col, on);
    }
}

/** \brief assert row GPIO */
static aw_bool_t __row_asserted (const struct matrix_keypad_par *par, int row)
{
    return aw_gpio_get(par->row_gpios[row]) == 1 ? !par->active_low
                                                 : par->active_low;
}

/** \brief test if the driver is in interrupt mode */
aw_static_inline aw_bool_t __matrix_keypad_int_mode (
    const struct matrix_keypad_par *par)
{
    return (0 == par->scan_interval);
}

/** \brief keypad int config */
static void __matrix_keypad_int_cfg (struct matrix_keypad_par *par, aw_bool_t on)
{
    int i;

    if (0 != par->clustered_irq) {
        if (on) {
            (void)aw_int_enable(par->clustered_irq);
        } else {
            (void)aw_int_disable(par->clustered_irq);
        }
    } else {
        for (i = 0; i < par->num_row; i++) {
            if (on) {
                (void)aw_gpio_trigger_on(par->row_gpios[i]);
            } else {
                (void)aw_gpio_trigger_off(par->row_gpios[i]);
            }
        }
    }
}

/** \brief initialize matrix keypad GPIOs */
static void __matrix_keypad_init_gpio (struct matrix_keypad *mkb)
{
    struct matrix_keypad_par *par = dev_get_matrix_keypad_par(&mkb->dev);

    if (NULL != par->plat_init) {
        par->plat_init();                /* 将输入管脚设置为上拉 */
    }

    __all_cols_activate(par, AW_FALSE);
}

static void __matrix_keypad_status_scan (struct matrix_keypad_par *par,
                                         uint16_t                 *state)
{
    int row, col;

    /* assert each column and read the row status out */
    for (col = 0; col < par->num_col; col++) {
        __col_activate(par, col, AW_TRUE);      /* 顺序把每一列都置为有效电平 */
        for (row = 0; row < par->num_row; row++) {
            if (__row_asserted(par, row)) {
                state[col] |= 1u << row;        /* 每一列存下一个键值 */
                AW_DBGF(("key at (row %u, col %u) pressed\n", row, col));
            }
        }
        __col_activate(par, col, AW_FALSE);     /* 扫描结束 */
    }
}

/** \brief scan matrix keypad */
static void __matrix_keypad_scan (void *p_mkb)
{
    struct matrix_keypad     *mkb = p_mkb;
    struct matrix_keypad_par *par = dev_get_matrix_keypad_par(&mkb->dev);
    uint16_t                  new_state[MATRIX_MAX_COLS];
    uint16_t                  bits_changed;
    int                       row, col, code;

    /* de-activate all columns for scanning */
    __all_cols_activate(par, AW_FALSE);

    memset(new_state, 0, sizeof(new_state));
    __matrix_keypad_status_scan(par, new_state);  /* 一次扫描结束，得到键值 */

    for (col = 0; col < par->num_col; col++) {    /* 按列查看是否得到了新值 */
        bits_changed = mkb->last_key_state[col] ^ new_state[col];
        if (0 == bits_changed) {
            continue;
        }
        for (row = 0; row < par->num_row; row++) {   /* 查看该列中存在的新值 */
            if ((bits_changed & (1u << row)) == 0) {
                continue;
            }
            code = MATRIX_SCAN_CODE(row, col, par->num_col);

            awbl_input_report_key(AW_INPUT_EV_KEY, par->keymap[code],
                                   new_state[col] & (1u << row));
            AW_DBGF(("raw key %u (row %u, col %u) %s\n", code, row, col,
                    new_state[col] & (1u << row) ? "down" : "up"));
        }
    }

    memcpy(mkb->last_key_state, new_state, sizeof(new_state));

    if (NULL != par->digitron) {
        /* da-activate all GPIOs if we are sharing GPIOs with a digitron dev */
        if (par->dd_col_active_low != par->active_low) {
            __all_cols_activate(par, par->active_low);
        }
    } else if (__matrix_keypad_int_mode(par)) {
        /* if in interrupt mode, re-activate the interrupt */
        __all_cols_activate(par, AW_TRUE);
        __matrix_keypad_int_cfg(par, AW_TRUE);
    }
}

/** \brief matrix keypad interrupt handler */
static void __matrix_keypad_irq (void *p_mkb)
{
    struct matrix_keypad     *mkb = p_mkb;
    struct matrix_keypad_par *par = dev_get_matrix_keypad_par(&mkb->dev);

    if (__matrix_keypad_int_mode(par)) {
        /* interrupt mode */
        __matrix_keypad_int_cfg(par, AW_FALSE);       /* 关闭触发中断 */
        (void)aw_delayed_work_start(&mkb->work, par->debounce_ms); /* 消抖时间，过一会扫描  */
    } else {
        /* timer scanning period mode */
        __matrix_keypad_scan(mkb);
        (void)aw_delayed_work_start(&mkb->work, par->scan_interval);
    }
}

/** \brief matrix keypad call back for digitron display device */
static void __matrix_keypad_dd_cb (void *p_mkb,uint_t uparg ,uint_t interval_ms)
{
    struct matrix_keypad     *mkb = p_mkb;
    struct matrix_keypad_par *par = dev_get_matrix_keypad_par(&mkb->dev);

    if (0 == mkb->dd_scan_count_max) {      /* 该值初始化为0 */
        /* 最大扫描个数为：消抖时间 / 扫描间隔 */
        mkb->dd_scan_count_max = par->debounce_ms / interval_ms;
        if (0 == mkb->dd_scan_count_max) {
            mkb->dd_scan_count_max = 1;
        }
    }

    /*
     * 如消抖时间较长，10ms / interval_ms (2) = 5
     * 则需要数码管5扫描5次，按键才扫描一次 。确保严格的消抖
     */
    if (++mkb->dd_scan_count >= mkb->dd_scan_count_max) {
        mkb->dd_scan_count = 0;
        __matrix_keypad_init_gpio(mkb);
        __matrix_keypad_scan(mkb);
    }
}

/** \brief initialize matrix keypad mode, interrupt or scanning period */
static void __matrix_keypad_init_mode (struct matrix_keypad *mkb)
{
    struct matrix_keypad_par *par = dev_get_matrix_keypad_par(&mkb->dev);
    int i, gpio, err;

    /* scanning in digitron display device's black out call back */
    if (NULL != par->digitron) {

        return;
    }

    /* timer scanning period mode,扫描间隔不为0，采用扫描模式 */
    if (!__matrix_keypad_int_mode(par)) {
        /* 扫描模式 */
        (void)aw_delayed_work_init(&mkb->work, __matrix_keypad_irq, mkb);  
        (void)aw_delayed_work_start(&mkb->work, par->scan_interval);
        return;
    }

    /* interrupt mode */
    if (0 != par->clustered_irq) {
        (void)aw_int_connect(par->clustered_irq, __matrix_keypad_irq, mkb);
    } else {
        for (i = 0; i < par->num_row; i++) {
            gpio = par->row_gpios[i];               /* 打开相应的行中断 */
            err  = aw_gpio_trigger_connect(gpio, __matrix_keypad_irq, mkb);
            if (AW_OK != err) {
                AW_DBGF(("gpio_trigger_connect(%d) fail: %d\n", gpio, err));
            }
            (void)aw_gpio_trigger_cfg(gpio, AW_GPIO_TRIGGER_BOTH_EDGES);
        }
    }
    /* 扫描函数 */
    (void)aw_delayed_work_init(&mkb->work, __matrix_keypad_scan, mkb);  
    __matrix_keypad_int_cfg(par, AW_TRUE);
}

/******************************************************************************/

/** instance initializing stage 2 */
static void __matrix_keypad_inst_init2 (struct awbl_dev *p_dev)
{
    struct matrix_keypad     *mkb = dev_to_matrix_keypad(p_dev);
    struct matrix_keypad_par *par = dev_get_matrix_keypad_par(p_dev);

    /* 必须占用比较长的时间 ？ 等什么？等idx !=0  */

    /* must be called first to call plat_init */
    __matrix_keypad_init_gpio(mkb);

    /* get initial key status */
    memset(mkb->last_key_state, 0, sizeof(mkb->last_key_state));

    if (par->digitron == NULL) {
      /* __matrix_keypad_status_scan(par, mkb->last_key_state); */
    }

    __matrix_keypad_init_mode(mkb);
}

/******************************************************************************/

/** \brief driver functions (must defined as aw_const) */
aw_const struct awbl_drvfuncs __g_awbl_drvfuncs_matrix_keypad = {
    NULL,                       /* devInstanceInit */
    __matrix_keypad_inst_init2, /* devInstanceInit2 */
    NULL                        /* devConnect */
};

/** driver registration (must defined as aw_const) */
aw_local aw_const struct awbl_drvinfo __g_drvinfo_matrix_keypad = {
    AWBL_VER_1,                         /* awb_ver */
    AWBL_BUSID_PLB,                     /* bus_id */
    MATRIX_KEYPAD_NAME,                 /* p_drvname */
    &__g_awbl_drvfuncs_matrix_keypad,   /* p_busfuncs */
    NULL,                               /* p_methods */
    NULL                                /* pfunc_drv_probe */
};

/** \brief register matrix keyboard driver */
void awbl_matrix_keypad_drv_register (void)
{
    awbl_drv_register(&__g_drvinfo_matrix_keypad);
}

/* end of file */
