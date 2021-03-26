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
 * \brief ����hc595�������������ʵ��
 *
 *
 * \par ʹ��ʾ��
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 14-12-05  ops, first implementation.
 * \endinternal
 */

#include "driver/digitron/awbl_hc595_digitron.h"

#define __DIGITRON_TO_HC_DIGITRON_DEV(p_dd)     \
          AW_CONTAINER_OF(p_dd, struct awbl_hc595_digitron_dev, digitron_dev)

aw_local void __hc595_digitron_data_send(
    struct awbl_hc595_digitron_dev *p_this, uint8_t scan_index)
{

    uint8_t i = 0, tmp = 0;
    uint32_t map = 0;
    uint32_t buf_idx = 0, disp_idx = 0;

    uint8_t valid_com = 0, invalid_com = 0;

    uint8_t com_hc595_idx = 0, seg_hc595_idx = 0, com_hc595_pin_idx = 0;

    for (i = 0; i < p_this->p_par->ddparam.num_rows; i++) {

        disp_idx = scan_index + p_this->p_par->ddparam.num_cols * i;

        if ((!(p_this->digitron_dev.blink_flags & \
               AW_BIT(p_this->p_par->ddparam.p_disp_idx_tbl[disp_idx]))) | \
               p_this->digitron_dev.blinking) {

            /* ���������������, ���Ҷ�Ӧ�Դ� */
            buf_idx = p_this->p_par->ddparam.p_disp_idx_tbl[disp_idx];

            p_this->p_par->p_seg_data[i] = \
                           p_this->p_par->ddparam.p_disp_buf[buf_idx];
        }
        else {

            map = p_this->digitron_dev.pdd_param->seg_active_low ? 0xFF : 0x00;

            p_this->p_par->p_seg_data[i] = map;

        }
    }

    /* ������Ƭhc595���λ�� */
    com_hc595_idx = scan_index / HC595_OUTPUT_PIN_NUMS;

    /* �����hc595���ĸ�������� */
    com_hc595_pin_idx = scan_index - (com_hc595_idx) * HC595_OUTPUT_PIN_NUMS;

    /* ������Чλ�� */
    valid_com |= AW_SBF(1, (7 - com_hc595_pin_idx));
    valid_com = p_this->digitron_dev.pdd_param->com_active_low ? (~valid_com) : valid_com;

    /* �����Чλ�� */
    invalid_com = p_this->digitron_dev.pdd_param->com_active_low ? 0xFF : 0x00;

    for (i = 0; i < p_this->p_par->hc595_com_nums; i++) {

        if (i == com_hc595_idx) {

            p_this->p_par->p_com_data[i] = valid_com;

        }else {

            p_this->p_par->p_com_data[i] = invalid_com;
        }
    }

    /* ����HC595��PCB���б�, ��������, HC595����λ��������������� */
    for (i = 0; i < (p_this->p_par->hc595_com_nums + p_this->p_par->hc595_seg_nums); i++) {

        /* ���λ��1, �����ѡ, ���Ͷ������� */
        if ((p_this->p_par->p_hc595_arrange[i]) & (1 << 7)) {

            /* ��ȡ��ѡ�������������*/
            tmp = p_this->p_par->p_hc595_arrange[i];
            seg_hc595_idx = AW_BIT_CLR(tmp, 7);

            aw_hc595_shift(p_this->p_hc595_dev,
                           HC595_LSBFISRT,
                           p_this->p_par->p_seg_data[seg_hc595_idx]);

        }else {

            aw_hc595_shift(p_this->p_hc595_dev,
                           HC595_LSBFISRT,
                           p_this->p_par->p_com_data[p_this->p_par->p_hc595_arrange[i]]);

        }
    }
}

void awbl_hc595_digitron_dev_ctor(struct awbl_hc595_digitron_dev   *p_this,
                                  struct aw_hc595_dev              *p_hc,
                                  struct awbl_hc595_digitron_param *p_par,
                                  uint8_t                           disp_dev_id)
{

    p_this->p_hc595_dev = p_hc;
    p_this->p_par       = p_par;

    p_this->digitron_dev.disp_dev_id = disp_dev_id;

    p_this->pfn_data_send = __hc595_digitron_data_send;

}

/* end of file*/
