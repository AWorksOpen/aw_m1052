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
 * \brief 基于hc595数码管的led驱动
 *
 * \internal
 * \par modification history:
 * - 1.00 15-04-14  ops, first implementation.
 * \endinternal
 */
#include "driver/led/awbl_hc595_digi_led.h"
#include "aw_digitron_disp.h"
#include "aw_bitops.h"

aw_local aw_err_t __led_seg8_decode (struct awbl_hc595_digi_led_dev *p_this,
                                     uint16_t                        id,
                                     uint16_t                       *p_digi_idx,
                                     uint16_t                       *p_seg_data,
                                     aw_bool_t                       on)
{

    aw_const struct awbl_led_seg8_tbl *p_cur = p_this->p_par->p_decode_tbl;

    while (p_cur != '\0') {

        if (p_cur->led_id == id) {

            *p_digi_idx = p_cur->digi_id;

            if (on) {

                AW_BIT_SET (p_this->p_par->p_led_disp_buf[p_cur->grp_id], 
                            p_cur->grp_led_id);

            }else {

                AW_BIT_CLR (p_this->p_par->p_led_disp_buf[p_cur->grp_id], 
                            p_cur->grp_led_id);
            }

            *p_seg_data = p_this->p_par->p_led_disp_buf[p_cur->grp_id];

            return AW_OK;
        }

        p_cur++;
    }

    return AW_ERROR;
}


aw_local aw_err_t __hc595_led_set_by_digitron (
    struct awbl_hc595_digi_led_dev *p_this,
    int                             id,
    aw_bool_t                       on) 
{

    uint16_t digi_idx = 0, seg_data = 0;

    if (on) {

        __led_seg8_decode (p_this, id, &digi_idx, &seg_data, AW_TRUE);

    }else {

        __led_seg8_decode (p_this, id, &digi_idx, &seg_data, AW_FALSE);

        if (p_this->p_par->active_low) {

            seg_data = ~seg_data;
        }

    }

    return aw_digitron_disp_at(p_this->p_par->digitron_dev_id, 
                               digi_idx, 
                               seg_data);
}

aw_local aw_err_t __hc595_led_toggle_by_digitron_dummy (
    struct awbl_hc595_digi_led_dev *p_this,
    int                             id) 
{

    return AW_ERROR;
}

void awbl_hc595_digi_led_dev_ctor (struct awbl_hc595_digi_led_dev    *p_this,
                                   struct awbl_hc595_digi_led_param  *p_par)
{

    p_this->p_par = p_par;

    p_this->pfn_hc595_led_set_by_digitron    = __hc595_led_set_by_digitron;
    p_this->pfn_hc595_led_toggle_by_digitron = __hc595_led_toggle_by_digitron_dummy;
}

/* end of file*/
