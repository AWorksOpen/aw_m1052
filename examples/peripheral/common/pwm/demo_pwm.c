/*******************************************************************************
 *                                  AWorks
 *                       ----------------------------
 *                       innovating embedded platform
 *
 * Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
 * All rights reserved.
 *
 * Contact information:
 * web site:    http://www.zlg.cn
 * e-mail:      support@zlg.cn
 *******************************************************************************/

/**
 * \file
 * \brief PWM��ʾ����(PWM������Դ������)
 *
 * - �������裺
 *   1. ��Ҫ��aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_DEV_PWM_BUZZER�����Ӧ��PWM�꣨�磺AW_DEV_xxx_EFLEX_PWMx��AW_DEV_xxx_QTIMERx_PWM��
 *      - ��Ӧƽ̨�Ĵ��ں�
 *      - AW_COM_CONSOLE
 *
 * - ʵ������
 *   1. ��������500msΪ�������С�
 *
 * \par Դ����
 * \snippet demo_pwm.c src_pwm
 *
 * \internal
 * \par Modification History
 * - 1.00 16-03-25  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_std_pwm PWM
 * \copydoc demo_pwm.c
 */

/** [src_pwm] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_pwm.h"
#include "aw_demo_config.h"
#include "aw_vdebug.h"

#define AW_PWM_ID   DE_BUZZER_PWMID

/**
 * \brief PWM��ʾ�������
 * \return ��
 */
aw_local void* __task_handle (void *arg)
{
    uint32_t    period1 = 2000000;      /* (ns) */
    uint32_t    period2 = 1000000;      /* (ns) */

    aw_kprintf("\nPWM demo testing...\n");

    while(1) {

        /* ���� PWM ����Чʱ�䣨�ߵ�ƽʱ�䣩50% ,���� period1*/
        aw_pwm_config(AW_PWM_ID, period1 / 2, period1);
        aw_pwm_enable(AW_PWM_ID);      /* ʹ��ͨ�� */
        aw_mdelay(250);
        aw_pwm_disable(AW_PWM_ID);     /* ����ͨ�� */
        aw_mdelay(250);


        /* ���� PWM ����Чʱ�䣨�ߵ�ƽʱ�䣩2% ,���� period1*/
        aw_pwm_config(AW_PWM_ID, period1 / 50, period1);
        aw_pwm_enable(AW_PWM_ID);      /* ʹ��ͨ�� */
        aw_mdelay(250);
        aw_pwm_disable(AW_PWM_ID);     /* ����ͨ�� */
        aw_mdelay(250);


        /* ���� PWM ����Чʱ�䣨�ߵ�ƽʱ�䣩50% ,���� period2*/
        aw_pwm_config(AW_PWM_ID, period2 / 2, period2);
        aw_pwm_enable(AW_PWM_ID);      /* ʹ��ͨ�� */
        aw_mdelay(250);
        aw_pwm_disable(AW_PWM_ID);     /* ����ͨ�� */
        aw_mdelay(250);


        /* ���� PWM ����Чʱ�䣨�ߵ�ƽʱ�䣩2% ,���� period2*/
        aw_pwm_config(AW_PWM_ID, period2 / 50, period2);
        aw_pwm_enable(AW_PWM_ID);      /* ʹ��ͨ�� */
        aw_mdelay(250);
        aw_pwm_disable(AW_PWM_ID);     /* ����ͨ�� */
        aw_mdelay(250);
    }

    return 0;
}

/******************************************************************************/
void demo_pwm_entry (void)
{
    aw_task_id_t    tsk;

    tsk = aw_task_create("PWM demo",
                         12,
                         1024,
                         __task_handle,
                         (void *)NULL);
    if (tsk == NULL) {
        aw_kprintf("PWM demo task create failed\r\n");
        return;
    }

    if (aw_task_startup(tsk) != AW_OK) {
        aw_task_delete(tsk);
    }
}

/** [src_pwm] */

/* end of file */
