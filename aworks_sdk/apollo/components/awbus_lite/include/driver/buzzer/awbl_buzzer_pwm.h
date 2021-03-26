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
 * \brief PWM-Buzzer driver.
 *
 * �������������κ�ʹ�ñ�׼PWM�ӿ�������ֱ����������
 *
 * \par 1.������Ϣ
 *
 *  - ������:   "pwm-buzzer"
 *  - ��������: AWBL_BUSID_PLB
 *  - �豸��Ϣ: struct pwm_buzzer_param
 *
 * \par 2.�����豸
 *
 *  - �κο���ʹ�ñ�׼PWM�ӿڿ��Ƶ�ֱ��������
 *
 * \par 3.�豸����/����
 *
 *  - \ref grp_awbl_buzzer_pwm_hwconf
 *
 * \internal
 * \par modification history
 * - 1.01 14-11-01  ops  redefine the pwm buzzer by using new specification.
 * - 1.00 14-07-09  ops, first implementation.
 * \endinternal
 */

#ifndef AWBL_BUZZER_PWM_H_
#define AWBL_BUZZER_PWM_H_

#include "apollo.h"
#include "awbus_lite.h"
#include "awbl_buzzer.h"
#include "driver/intensity/awbl_pwm_it.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PWM_BUZZER_NAME  "pwm-buzzer"

typedef struct awbl_pwm_buzzer_param {

    /* PWMͨ��ID */
    int pwm_id;

    /* PWM�����������(��λ: ����) */
    unsigned long  period_ns;

    /* ��ʼ��� (%)*/
    int loud_init;

}awbl_pwm_buzzer_param_t;

typedef struct awbl_pwm_buzzer {

    struct awbl_buzzer_dev  bdev;
    struct awbl_dev         dev;
    awbl_pwm_it_dev_t       p_dev;

    awbl_pwm_buzzer_param_t param;

}awbl_pwm_buzzer_t;

void awbl_pwm_buzzer_drv_register (void);

#ifdef __cplusplus
}
#endif

#endif /* AWBL_BUZZER_PWM_H_ */
