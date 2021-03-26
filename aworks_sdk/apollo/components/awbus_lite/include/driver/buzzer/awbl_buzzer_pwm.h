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
 * 本驱动适用于任何使用标准PWM接口驱动的直流蜂鸣器。
 *
 * \par 1.驱动信息
 *
 *  - 驱动名:   "pwm-buzzer"
 *  - 总线类型: AWBL_BUSID_PLB
 *  - 设备信息: struct pwm_buzzer_param
 *
 * \par 2.兼容设备
 *
 *  - 任何可以使用标准PWM接口控制的直流蜂鸣器
 *
 * \par 3.设备定义/配置
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

    /* PWM通道ID */
    int pwm_id;

    /* PWM波形输出周期(单位: 纳秒) */
    unsigned long  period_ns;

    /* 初始响度 (%)*/
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
