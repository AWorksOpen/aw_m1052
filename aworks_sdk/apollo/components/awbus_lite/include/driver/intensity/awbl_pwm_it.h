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
 * \brief 基于PWM的强度调节设备驱动
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

    /** \brief 所使用的PWM通道号 */
    uint8_t pwm_channel_id;

    /** \brief PWM周期 */
    unsigned long period_ns;

}awbl_pwm_it_dev_param_t;

/**
 * \brief pwm intensity device
 */
typedef struct awbl_pwm_it_dev {

    /** \brief 继承自 intensity device */
    aw_it_dev_t  idev;

    /** \brief pwm_it_dev参数 */
    awbl_pwm_it_dev_param_t param;

}awbl_pwm_it_dev_t;

/**
 * \brief 基于PWM的强度调节设备驱动构造器
 *
 * \param max_level        等级范围
 * \param pwm_channel_id   pwm通道号
 * \param period_ns        周期
 *
 * \return 抽象强度调节设备 aw_it_dev_t
 */
aw_it_dev_t* awbl_pwm_it_ctor(awbl_pwm_it_dev_t *p_dev, 
                              int                max_level,
                              int                pwm_channel_id, 
                              unsigned long      period_ns);

#endif /* AWBL_PWM_IT_H_ */
