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
 * \brief ����hc595�����ľ��󰴼��ඨ��
 *
 * \internal
 * \par modification history
 * - 1.00 14-12-08  ops, first implementation.
 * \endinternal
 */
#ifndef AWBL_HC595_MATRIX_KEY_H_
#define AWBL_HC595_MATRIX_KEY_H_

#include "apollo.h"
#include "aw_hc595.h"

#ifdef __cplusplus
extern "C" {
#endif

struct awbl_hc595_matrix_key_param {

    /**
     * \brief ���������
     */
    const uint16_t *key_code_tbl;

    /**
     * \brief ��������
     */
    uint8_t  key_nums;

    /**
     * \brief ��������
     */
    aw_bool_t   active_low;

    /**
     * \brief �Ƿ��������豸��������
     */
    aw_bool_t   multi_io;

    /**
     * \brief Key Out����
     */
    uint16_t key_out_pin;

    /**
     * \brief hc595��������
     */
    uint8_t  hc595_nums;

    /**
     * \brief λ�����ݻ�����
     */
    uint8_t  *p_com_data;

    /**
     * \brief hc595���б�
     */
    const uint8_t *p_hc595_arrange;

};

struct awbl_hc595_matrix_key_dev {

    /**
     * \brief ��������hc595�豸
     */
    struct aw_hc595_dev                 *p_hc595_dev;

    /**
     * \brief hc595�������ò���
     */
    struct awbl_hc595_matrix_key_param  *p_par;

    /**
     * \brief ������
     */
    uint8_t  detect_cnt;

    /**
     * \brief ��⿪ʼ���
     */
    uint8_t  detect_start;

    /**
     * \brief ��ǰ�����
     */
    uint32_t detect_bits_state;

    /**
     * \brief ��һ�μ��Ľ��
     */
    uint32_t last_bits_state;

    /**
     * \brief �������ų�ʼ��
     */
    void (*pfn_hc595_key_pin_gpio_init)(struct awbl_hc595_matrix_key_dev *p_this);

    /**
     * \brief �������
     */
    void (*pfn_hc595_key_detect)(struct awbl_hc595_matrix_key_dev *p_this, 
                                 uint8_t                           idx);

};


aw_local aw_inline void awbl_hc595_matrix_key_detect(
    struct awbl_hc595_matrix_key_dev *p_this, uint8_t idx)
{
    p_this->pfn_hc595_key_detect(p_this, idx);
}

void awbl_hc595_matrix_key_dev_ctor(struct awbl_hc595_matrix_key_dev    *p_this,
                                    struct aw_hc595_dev                 *p_hc,
                                    struct awbl_hc595_matrix_key_param  *p_par);

#ifdef __cplusplus
}
#endif


#endif /* AWBL_HC595_MATRIX_KEY_H_ */
