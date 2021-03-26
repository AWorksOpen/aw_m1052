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
 * \brief 基于hc595驱动的矩阵按键类实现
 *
 * \internal
 * \par modification history
 * - 1.00 14-12-08  ops, first implementation.
 * \endinternal
 */

#include "driver/input/key/awbl_hc595_matrix_key.h"
#include "aw_gpio.h"
#include "aw_vdebug.h"
#include "aw_input.h"

aw_local aw_bool_t __key_asserted(struct awbl_hc595_matrix_key_dev *p_this)
{
    return aw_gpio_get(p_this->p_par->key_out_pin) == 1 ?
                            (!p_this->p_par->active_low) :
                            (p_this->p_par->active_low);
}

aw_local void __hc595_data_send (struct awbl_hc595_matrix_key_dev *p_this, 
                                 uint32_t                          data)
{
    uint8_t i = 0, buf = 0;

    uint32_t com = 1;

    com <<= data;

    com = p_this->p_par->active_low ? (~com) : (com);

    for (i = 0; i < p_this->p_par->hc595_nums; i++) {

        buf = AW_BITS_GET(com,
                          p_this->p_par->p_hc595_arrange[i] * HC595_OUTPUT_PIN_NUMS,
                          HC595_OUTPUT_PIN_NUMS);

        aw_hc595_shift(p_this->p_hc595_dev, HC595_LSBFISRT, buf);
    }

    com = 1;
}


aw_local uint32_t __hc595_key_status_scan (
    struct awbl_hc595_matrix_key_dev *p_this, uint8_t index)
{
    uint32_t state = 0;

    if (p_this->p_par->multi_io) {

        if (index < (p_this->p_par->key_nums)) {

            if (__key_asserted(p_this)) {

                state |= 1u << index;
            }
        }

    }else {

        __hc595_data_send(p_this, index);

        if (__key_asserted(p_this)) {

            state |= 1 << index;
        }

    }

    return state;
}

aw_local void __hc595_key_report(struct awbl_hc595_matrix_key_dev *p_this, 
                                 uint32_t                          bits_changed)
{
    uint8_t  i = 0;

    for (i = 0; i < (p_this->p_par->key_nums); i++) {

        if ((bits_changed & (1u << i)) == 0) {

            continue;
        }

        aw_input_report_key(AW_INPUT_EV_KEY,
                            p_this->p_par->key_code_tbl[i],
                            p_this->detect_bits_state & (1u << i) ? AW_TRUE : AW_FALSE);
    }
}

aw_local void __hc595_key_scan(struct awbl_hc595_matrix_key_dev *p_this, 
                               uint8_t                           index)
{
    uint32_t bit_state = 0, bits_changed = 0;

    if (p_this->detect_cnt == p_this->p_par->key_nums) {

        p_this->detect_cnt = 0;
    }

    bit_state  = __hc595_key_status_scan(p_this, p_this->detect_cnt++);

    if (p_this->detect_start == 0) {

        if (index == 0) {

            p_this->detect_start       = 1;
            p_this->detect_bits_state |= bit_state;
        }
    }
    else {

        if (index == (p_this->p_par->key_nums) - 1) {

            /*
             * 扫描到最后一个按键, 上报到输入子系统.
             */
            p_this->detect_bits_state |= bit_state;


            /* 记录按键变化 */
            bits_changed = p_this->last_bits_state ^ p_this->detect_bits_state;

            if (bits_changed) {

                __hc595_key_report(p_this, bits_changed);

                /* 记录当前按键状态 */
                p_this->last_bits_state = p_this->detect_bits_state;
            }

            p_this->detect_bits_state = 0;
            p_this->detect_start      = 0;

        }
        else {

            p_this->detect_bits_state |= bit_state;
        }
    }
}

aw_local void __hc595_key_pin_gpio_init (struct awbl_hc595_matrix_key_dev *p_this)
{

    aw_gpio_pin_cfg(p_this->p_par->key_out_pin, AW_GPIO_INPUT);
}


void awbl_hc595_matrix_key_dev_ctor(struct awbl_hc595_matrix_key_dev    *p_this,
                                    struct aw_hc595_dev                 *p_hc,
                                    struct awbl_hc595_matrix_key_param  *p_par)
{
    p_this->p_hc595_dev       = p_hc;
    p_this->p_par             = p_par;
    p_this->last_bits_state   = 0;
    p_this->detect_bits_state = 0xFFFFFFFF;

    p_this->detect_start = 0;
    p_this->detect_cnt   = 0;

    p_this->pfn_hc595_key_pin_gpio_init = __hc595_key_pin_gpio_init;
    p_this->pfn_hc595_key_detect        = __hc595_key_scan;


    p_this->pfn_hc595_key_pin_gpio_init(p_this);

}


/* end of file*/
