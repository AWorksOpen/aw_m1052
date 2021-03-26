/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief PWM����������������Ϣ.
 *
 * \internal
 * \par modification history:
 * - 1.00 16-04-08  hgo, first implementation.
 * \endinternal
 */

#ifndef __AWBL_HWCONF_BUZZER_PWM_H
#define __AWBL_HWCONF_BUZZER_PWM_H

#include "driver/buzzer/awbl_buzzer_pwm.h"

#ifdef AW_DEV_PWM_BUZZER


aw_local aw_const awbl_pwm_buzzer_param_t __g_pwm_buzzer_param = {
    PWM10,        /* PWMͨ��ID */
    366166,      /* PWM�����������(��λ: ����) */
    50           /* ��ʼ���(ռ�ձ�) 50% ����Χ0-100 */
};

aw_local awbl_pwm_buzzer_t __g_pwm_buzzer;

#define AWBL_HWCONF_BUZZER_PWM  \
    {                           \
        PWM_BUZZER_NAME,        \
        0,                      \
        AWBL_BUSID_PLB,         \
        0,                      \
        &(__g_pwm_buzzer.dev),  \
        &(__g_pwm_buzzer_param) \
    },                          \

#else

#define AWBL_HWCONF_BUZZER_PWM

#endif

#endif /* __AWBL_HWCONF_BUZZER_PWM_H */
