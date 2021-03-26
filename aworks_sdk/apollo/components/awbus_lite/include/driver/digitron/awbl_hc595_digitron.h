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
 * \brief ����hc595������������ඨ��
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

#ifndef AWBL_HC595_DIGITRON_H_
#define AWBL_HC595_DIGITRON_H_

#include "awbl_digitron_if.h"
#include "awbl_gpio_hc595.h"

#ifdef __cplusplus
extern "C" {
#endif

struct awbl_hc595_digitron_param {

    /**
     * \brief ͨ����������ò���
     */
    struct awbl_digitron_disp_param  ddparam;

    /**
     * \brief ��������ܶ����hc595����
     */
    uint8_t                          hc595_seg_nums;

    /**
     * \brief ���������λ���hc595����
     */
    uint8_t                          hc595_com_nums;

    /**
     * \brief �������ݻ�����
     */
    uint8_t                          *p_seg_data;

    /**
     * \brief λ�����ݻ�����
     */
    uint8_t                          *p_com_data;

    /**
     * \brief hc595����, 1: ����, 0: λ��
     */
    aw_const uint8_t                 *p_hc595_arrange;

};

struct awbl_hc595_digitron_dev {

    /**
     * \brief �̳���ͨ�������awbl_digitron_dev
     */
    struct awbl_digitron_dev          digitron_dev;

    /**
     * \brief ����ͨ��hc595�豸
     */
    struct aw_hc595_dev               *p_hc595_dev;

    /**
     * \brief hc595��������ò���
     */
    struct awbl_hc595_digitron_param  *p_par;

    /**
     * \brief �������ݷ��ͽӿ�
     */
    void (*pfn_data_send)(struct awbl_hc595_digitron_dev *p_this, uint8_t idx);

};

aw_local aw_inline void awbl_hc595_digitron_data_send(
    struct awbl_hc595_digitron_dev *p_this, uint8_t idx)
{
    p_this->pfn_data_send(p_this, idx);
}

void awbl_hc595_digitron_dev_ctor(struct awbl_hc595_digitron_dev   *p_this,
                                  struct aw_hc595_dev              *p_hc,
                                  struct awbl_hc595_digitron_param *p_par,
                                  uint8_t                           disp_dev_id);

#ifdef __cplusplus
}
#endif


#endif /* AWBL_HC595_DIGITRON_H_ */
