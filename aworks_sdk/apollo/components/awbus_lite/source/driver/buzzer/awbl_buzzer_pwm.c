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
 * \brief driver for DC-Buzzer driven by PWM.
 *
 * \internal
 * \par modification history:
 * - 1.01 14-11-01  ops, redefine the pwm buzzer by using new specification.
 * - 1.00 14-07-09  ops, first implementation.
 * \endinternal
 */

#include "aw_intensity.h"
#include "driver/intensity/awbl_pwm_it.h"
#include "driver/buzzer/awbl_buzzer_pwm.h"

#define BUZZER_TO_PWM_BUZZER(p_dev)   \
             AW_CONTAINER_OF(p_dev, awbl_pwm_buzzer_t, bdev)

#define DEV_TO_PWM_BUZZER(p_dev)      \
             AW_CONTAINER_OF(p_dev, awbl_pwm_buzzer_t, dev)

#define PWM_IT_TO_IT_DEV(p_dev)           ((aw_it_dev_t *)(p_dev))
#define DEV_GET_PWM_BUZZER_PAR(p_dev)     ((void *)AWBL_DEVINFO_GET(p_dev))

static void __pwm_buzzer_beep (struct awbl_buzzer_dev *p_dev, aw_bool_t on)
{
    awbl_pwm_buzzer_t *p_pb = BUZZER_TO_PWM_BUZZER(p_dev);
    aw_it_dev_t *p_it = PWM_IT_TO_IT_DEV(&p_pb->p_dev);

    if ( on) {
        aw_it_level_set(p_it, p_pb->p_dev.idev.current_level);
    } else {
        aw_it_level_set(p_it, 0);
    }
}

static aw_err_t __pwm_buzzer_loud_set (struct awbl_buzzer_dev *p_dev, int level) 
{

    awbl_pwm_buzzer_t *p_pb = BUZZER_TO_PWM_BUZZER(p_dev);

    if (level < 0 && level >100) {
        return -AW_EINVAL;
    }

    p_pb->p_dev.idev.current_level = level;

    return AW_OK;
}

/** \brief ʵ����ʼ�� stage1 */
static void __pwm_buzzer_inst_init (struct awbl_dev *p_dev)
{
    awbl_pwm_buzzer_t        *p_pb = DEV_TO_PWM_BUZZER(p_dev);
    awbl_pwm_buzzer_param_t  *pp = DEV_GET_PWM_BUZZER_PAR(p_dev);

    p_pb->param.pwm_id    = pp->pwm_id;
    p_pb->param.period_ns = pp->period_ns;

    p_pb->bdev.beep = __pwm_buzzer_beep;
    p_pb->bdev.loud_set = __pwm_buzzer_loud_set;

    /* ����pwm_it_dev�Ĺ��캯����, ����ʹ�ø����׼�ӿڽ��в��� */
    awbl_pwm_it_ctor(&p_pb->p_dev, 100, p_pb->param.pwm_id, p_pb->param.period_ns);

    p_pb->p_dev.idev.current_level = pp->loud_init;

    /* 
     * ��ʵ����һ��pwm_buzzer����, ʵ�ֳ�����buzzer_dev���麯����, 
     * ���ݸ���ָ�빩�м��ʹ�� 
     */
    (void)awbl_buzzer_register_device(&p_pb->bdev);
}

aw_const struct awbl_drvfuncs __g_awbl_drvfuncs_pwm_buzzer = {
    __pwm_buzzer_inst_init,
    NULL,
    NULL
};

aw_local aw_const struct awbl_drvinfo __g_drvinfo_pwm_buzzer = {
    AWBL_VER_1,
    AWBL_BUSID_PLB,
    PWM_BUZZER_NAME,
    &__g_awbl_drvfuncs_pwm_buzzer,
    NULL,
    NULL
};

void awbl_pwm_buzzer_drv_register (void)
{
    awbl_drv_register(&__g_drvinfo_pwm_buzzer);
}

/* end of file*/