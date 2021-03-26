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
 * \brief ����GPIO������������ඨ��
 *
 *
 * \par ʹ��ʾ��
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 17-06-19  nwt, first implementation.
 * \endinternal
 */

#ifndef AWBL_GPIO_DIGITRON_H_
#define AWBL_GPIO_DIGITRON_H_

#include "awbl_digitron_if.h"
#include "awbl_gpio.h"


#ifdef __cplusplus
extern "C" {
#endif

struct awbl_gpio_key_digitron_param {

    /**
     * \brief ͨ����������ò���
     */
    struct awbl_digitron_disp_param  ddparam;

    /** \brief ���� GPIO���� ���� */
    const uint16_t                   *p_seg_pins;  
    /** \brief λ�� GPIO���� ���� */    
    const uint16_t                   *p_com_pins;   


    /**
     * \brief ��������ܶ����gpio����
     */
    uint8_t                          gpio_seg_nums;

    /**
     * \brief ���������λ���gpio����
     */
    uint8_t                          gpio_com_nums;


    /**
     * \brief һ��ɨ������ܵĸ���
     */
    uint8_t                          scan_num_digitron;


    /**
     * \brief ɨ�����ݻ���
     */
    void                            *p_scan_data;


};

struct awbl_gpio_key_digitron_dev {

    /**
     * \brief �̳���ͨ�������awbl_digitron_dev
     */
    struct awbl_digitron_dev          digitron_dev;


    /**
     * \brief gpio��������ò���
     */
    struct awbl_gpio_key_digitron_param  *p_par;

    /**
     * \brief �������ݷ��ͽӿ�
     */
    void (*pfn_data_send)(struct awbl_gpio_key_digitron_dev *p_this, uint8_t idx);

};

aw_local aw_inline void awbl_gpio_key_digitron_data_send(
    struct awbl_gpio_key_digitron_dev *p_this, uint8_t idx)
{
    p_this->pfn_data_send(p_this, idx);
}

void awbl_gpio_key_digitron_dev_ctor(
    struct awbl_gpio_key_digitron_dev    *p_this,
    struct awbl_gpio_key_digitron_param  *p_par,
    uint8_t                               disp_dev_id);

#ifdef __cplusplus
}
#endif


#endif /* AWBL_GPIO_DIGITRON_H_ */
