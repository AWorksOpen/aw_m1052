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
 * \brief ����PWM��ǿ�ȵ����豸����
 *
 * \par modification history:
 * - 1.01 14-11-01  ops, redefine the intensity device by using new specification.
 * - 1.00 14-07-03  ops, first implementation
 * \endinternal
 */

#ifndef AWBL_PWM_IT_H_
#define AWBL_PWM_IT_H_

#include "aw_intensity.h"

typedef struct awbl_pwm_it_dev_param {

    /** \brief ��ʹ�õ�PWMͨ���� */
    uint8_t pwm_channel_id;

    /** \brief PWM���� */
    unsigned long period_ns;

}awbl_pwm_it_dev_param_t;

/**
 * \brief pwm intensity device
 */
typedef struct awbl_pwm_it_dev {

    /** \brief �̳��� intensity device */
    aw_it_dev_t  idev;

    /** \brief pwm_it_dev���� */
    awbl_pwm_it_dev_param_t param;

}awbl_pwm_it_dev_t;

/**
 * \brief ����PWM��ǿ�ȵ����豸����������
 *
 * \param max_level        �ȼ���Χ
 * \param pwm_channel_id   pwmͨ����
 * \param period_ns        ����
 *
 * \return ����ǿ�ȵ����豸 aw_it_dev_t
 */
aw_it_dev_t* awbl_pwm_it_ctor(awbl_pwm_it_dev_t *p_dev, 
                              int                max_level,
                              int                pwm_channel_id, 
                              unsigned long      period_ns);

#endif /* AWBL_PWM_IT_H_ */
