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
 * \brief 基于gpio + hc595驱动的数码管类实现
 *
 *
 * \par 使用示例
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 17-06-25  nwt, first implementation.
 * \endinternal
 */

#include "driver/digitron/awbl_gpio_hc595_digitron.h"
#include "awbl_gpio.h"
#include "aw_gpio.h"

#define __DIGITRON_TO_HC_DIGITRON_DEV(p_dd)     \
          AW_CONTAINER_OF(p_dd, struct awbl_gpio_hc595_digitron_dev, digitron_dev)

aw_local void __gpio_hc595_digitron_com_data_send(
    struct awbl_gpio_hc595_digitron_dev *p_this, uint8_t scan_index)
{
    uint8_t i = 0;
    uint8_t com_gpio_idx = 0;

    /* 计算哪个GPIO引脚输出位码, scan_index的最大值一般为  num_cols 的数值减 1 */
    com_gpio_idx = scan_index % p_this->p_par->ddparam.num_cols;

    for (i = 0; i < p_this->p_par->gpio_com_nums; i++) {

        if (i == com_gpio_idx) {

            /* 选中该数码管 */
            aw_gpio_set(p_this->p_par->p_com_pins[i], 
                        (p_this->digitron_dev.pdd_param->seg_active_low ? 0 : 1));

        }else {

            /* 取消选中该数码管 */
            aw_gpio_set(p_this->p_par->p_com_pins[i], 
                       (p_this->digitron_dev.pdd_param->seg_active_low ? 1 : 0));
        }
    }

}

/** \brief assert row GPIO */
static aw_bool_t __row_asserted (const struct awbl_matrix_key_lite_par *par, 
                                 int                                    row)
{
    return aw_gpio_get(par->row_gpios[row]) == 1 ? !par->active_low : par->active_low;
}

static void __matrix_key_lite_status_scan (struct awbl_matrix_key_lite_par *par,
                                           uint16_t                        *state)
{
    int row = 0;

    /* assert each column and read the row status out */
    for (row = 0; row < par->num_row; row++) {

        if (__row_asserted(par, row)) {
            *state |= 1u << row;                   /* 每一列存下一个键值 */
        } else {
            *state &= ~(1u << row);                /* 更新按键状态信息 */
        }
    }

}

/** \brief scan matrix keypad */
static void __matrix_key_lite_scan (void *p_arg, uint16_t scan_index)
{
    struct awbl_gpio_hc595_digitron_dev *p_this =  \
                        (struct awbl_gpio_hc595_digitron_dev *)p_arg;

    struct awbl_matrix_key_lite_par *par = p_this->p_par->p_matrix_par;

    uint16_t                  bits_changed = 0;

    int                       row = 0, col = 0, code = 0;

    __matrix_key_lite_status_scan(par, &par->p_current_key_state[scan_index]);

    if (scan_index == p_this->p_par->p_matrix_par->num_col - 1) {

        for (col = 0; col < par->num_col; col++) {    /* 按列查看是否得到了新值 */
             bits_changed = par->p_last_key_state[col] ^ par->p_current_key_state[col];

             /* 这一列都没有按键按下 */
             if (0 == bits_changed) {
                 continue;
             }

             /* 这一列的该行没有按键按下 */
             for (row = 0; row < par->num_row; row++) {  /* 查看该列中存在的新值 */
                 if ((bits_changed & (1u << row)) == 0) {
                     continue;
                 }
                 code = MATRIX_KEY_LITE_SCAN_CODE(row, col, par->num_col);

                 awbl_input_report_key(AW_INPUT_EV_KEY, par->keymap[code],
                                       par->p_current_key_state[col] & (1u << row));
             }
        }

        /*　赋新值，更新按键状态 */
        for (col = 0; col < par->num_col; col++) {
            par->p_last_key_state[col] =  par->p_current_key_state[col];
        }
    }
}

aw_local void __gpio_hc595_digitron_black_cb(
    struct awbl_gpio_hc595_digitron_dev *p_this, uint8_t scan_index)
{

    __matrix_key_lite_scan((void *)p_this, scan_index);
}

aw_local void __gpio_hc595_digitron_seg_data_send(
    struct awbl_gpio_hc595_digitron_dev *p_this, uint8_t scan_index)
{
    uint8_t i = 0;
    uint32_t map = 0;

    uint32_t buf_idx = 0, disp_idx = 0;


    for (i = 0; i < p_this->p_par->ddparam.num_rows; i++) {

        /* 有多少行就有多少个扫描缓存 */
        disp_idx = scan_index + p_this->p_par->ddparam.num_cols * i;

        /*　if条件判断语句用于确定扫描到那个数码管时闪烁 */
        if ((!(p_this->digitron_dev.blink_flags & AW_BIT( \
                   p_this->p_par->ddparam.p_disp_idx_tbl[disp_idx]))) | \
               p_this->digitron_dev.blinking) {

            /* 根据数码管索引号, 查找对应显存 */
            buf_idx = p_this->p_par->ddparam.p_disp_idx_tbl[disp_idx];

            /* 这样写可以同时扫描多个数码管 */
            p_this->p_par->p_seg_data[i] = p_this->p_par->ddparam.p_disp_buf[buf_idx];

        } else {

            map = p_this->digitron_dev.pdd_param->seg_active_low ? 0xFF : 0x00;

            p_this->p_par->p_seg_data[i] = map;

        }
    }

    /* 
     * 根据HC595的PCB排列表, 发送数据, HC595的移位次数由其个数决定,
     * 根据多个595可以同时扫描多个数码管 
     */
    for (i = 0; i <  p_this->p_par->hc595_seg_nums; i++) {

        aw_hc595_shift(p_this->p_hc595_dev,
                       HC595_LSBFISRT,
                       p_this->p_par->p_seg_data[i]);


    }
}

void awbl_gpio_hc595_digitron_dev_ctor(
    struct awbl_gpio_hc595_digitron_dev   *p_this,
    struct aw_hc595_dev                   *p_hc,
    struct awbl_gpio_hc595_digitron_param *p_par,
    uint8_t                                disp_dev_id)
{
    int i = 0;

    p_this->p_hc595_dev = p_hc;
    p_this->p_par       = p_par;

    /* 通用数码管设备的其它参数在awbl_digitron_disp_register的时候构建 */
    p_this->digitron_dev.disp_dev_id = disp_dev_id;

    p_this->pfn_com_data_send = __gpio_hc595_digitron_com_data_send;

    p_this->pfn_seg_data_send = __gpio_hc595_digitron_seg_data_send;

    if (p_this->p_par->multi_io ) {
        p_this->pfn_black_cb = __gpio_hc595_digitron_black_cb;
    } else {
        p_this->pfn_black_cb = NULL;
    }


    /* 初始化GPIO COM管脚  */
    if (p_par->ddparam.com_active_low) {
        for (i = 0; i < p_par->gpio_com_nums; i++) {
            aw_gpio_pin_cfg(p_par->p_com_pins[i], AW_GPIO_OUTPUT);
            aw_gpio_set(p_par->p_com_pins[i], 1);
        }
    } else {
        for (i = 0; i < p_par->gpio_com_nums; i++) {
            aw_gpio_pin_cfg(p_par->p_com_pins[i], AW_GPIO_OUTPUT);
            aw_gpio_set(p_par->p_com_pins[i], 0);
        }
    }

    if (p_this->p_par->multi_io ) {

        /* 复用时矩阵键盘的行作为输入配置引脚 */
        for (i = 0; i < p_par->p_matrix_par->num_row; i++) {
            aw_gpio_pin_cfg(p_par->p_matrix_par->row_gpios[i], 
                            AW_GPIO_INPUT | AW_GPIO_PULL_DOWN);
        }
    }
}

/* end of file*/

