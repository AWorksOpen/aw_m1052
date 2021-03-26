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
 * \brief ����hc595��led����
 *
 * \internal
 * \par modification history:
 * - 1.01 15-04-13  ops, add LED set implementation by using digitron if.
 * - 1.00 14-12-17  ops, first implementation.
 * \endinternal
 */

#include "driver/led/awbl_hc595_led.h"
#include "aw_vdebug.h"

#define __DEV_TO_HC595_LED_DEV(p_dev)   AW_CONTAINER_OF(p_dev, \
                                                        struct awbl_hc595_led_dev, \
                                                        p_dev)

#define __DEV_GET_HC595_LED_PAR(p_dev)  ((void *)AWBL_DEVINFO_GET(p_dev))
#define __THIS_TO_HC595_LED_DEV(p_this) ((struct awbl_hc595_led_dev *)(p_this))

aw_local aw_err_t __hc595_led_set(struct awbl_hc595_led_dev *p_this, 
                                  int                        id, 
                                  aw_bool_t                  on)
{
    aw_const struct awbl_hc595_led_tbl *p_cur = p_this->p_par->p_led_tbl;
    uint8_t code = 0;

    while(p_cur != '\0') {

        if (p_cur->led_id == id) {

            code = (on) << p_cur->grp_led_id;

            code = p_this->p_par->led_active_low ? (~code) : (code);

            p_this->p_par->p_led_disp_buf[p_cur->grp_id] = code;

            return AW_OK;
        }

        p_cur++;
    }

    return -AW_ENODEV;
}

aw_local aw_err_t __hc595_led_toggle(struct awbl_hc595_led_dev *p_this, int id)
{
    AW_INFOF(("__hc595_led_toggle. \n"));

    return AW_OK;
}

aw_local void __hc595_led_data_send (struct awbl_hc595_led_dev *p_this,
                                     uint8_t                    scan_index,
                                     uint8_t                    led_rank_idx)
{

    uint8_t i = 0, led_grp_idx = 0;
    uint8_t com_hc595_idx = 0, com_hc595_pin_idx = 0;
    uint8_t valid_com = 0, invalid_com = 0, tmp = 0, led_hc595_idx = 0;

    /* ÿ��ɨ��һ������ (led_rank_idx)*/

    for (i = 0; i < p_this->p_par->leds_grp_per_rank_nums; i++) {

        /* �����Ӧ�������� */
        led_grp_idx = i + led_rank_idx * p_this->p_par->leds_grp_per_rank_nums;

        /* ����������, ���Ҷ�Ӧ�Դ� , ����ֵ�����ݷ��ͻ�����*/
        p_this->p_par->p_led_data[i] = p_this->p_par->p_led_disp_buf[led_grp_idx];
    }

    /* ������Ƭhc595���λ�� */
    com_hc595_idx = scan_index / HC595_OUTPUT_PIN_NUMS;

    /* �����hc595���ĸ�������� */
    com_hc595_pin_idx = scan_index - (com_hc595_idx) * HC595_OUTPUT_PIN_NUMS;

    /* ������Чλ�� */
    valid_com |= AW_SBF(1, (7 - com_hc595_pin_idx));
    valid_com = p_this->p_par->com_active_low ? (~valid_com) : valid_com;

    /* �����Чλ�� */
    invalid_com = p_this->p_par->com_active_low ? 0xFF : 0x00;

    for (i = 0; i < p_this->p_par->hc595_com_nums; i++) {

        if (i == com_hc595_idx) {

            p_this->p_par->p_com_data[i] = valid_com;

        }else {

            p_this->p_par->p_com_data[i] = invalid_com;
        }
    }

    /* ����HC595��PCB���б�, ��������, HC595����λ��������������� */
    for (i = 0; i < (p_this->p_par->hc595_com_nums + \
                        p_this->p_par->leds_grp_per_rank_nums); i++) {

        /* ���λ��1, �����ѡ, ���Ͷ������� */
        if ((p_this->p_par->p_hc595_arrange[i]) & (1 << 7)) {

            /* ��ȡ��ѡ�������������*/
            tmp = p_this->p_par->p_hc595_arrange[i];
            led_hc595_idx = AW_BIT_CLR(tmp, 7);

            aw_hc595_shift(p_this->p_hc595_dev,
                           HC595_LSBFISRT,
                           p_this->p_par->p_led_data[led_hc595_idx]);

        }else {

            aw_hc595_shift(p_this->p_hc595_dev,
                           HC595_LSBFISRT,
                           p_this->p_par->p_com_data[p_this->p_par->p_hc595_arrange[i]]);

        }
    }
}

void awbl_hc595_led_dev_ctor(struct awbl_hc595_led_dev    *p_this,
                             struct aw_hc595_dev          *p_hc,
                             struct awbl_hc595_led_param  *p_par)
{

    p_this->p_hc595_dev = p_hc;
    p_this->p_par       = p_par;

    p_this->pfn_hc595_led_set       = __hc595_led_set;
    p_this->pfn_hc595_led_toggle    = __hc595_led_toggle;
    p_this->pfn_hc595_led_data_send = __hc595_led_data_send;
}

/* end of file*/

