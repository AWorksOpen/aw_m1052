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
 * \brief ��Դ����������
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_DEV_PWM_BUZZER
 *      - ��Ӧƽ̨��PWM��
 *      - ��Ӧƽ̨�Ĵ��ں�
 *      - AW_COM_CONSOLE
 * - ʵ������
 *   1. ������ÿ��500ms����һ�Ρ�
 *
 * - ��ע��
 *   1. �����޸���Դ����������Ƶ�ʣ�����awbl_hwconf_buzzer_pwm.h���޸�������Ϣ��
 *
 * \par Դ����
 * \snippet demo_pwm_buzzer.c src_pwm_buzzer
 *
 * \internal
 * \par Modification History
 * - 1.00 17-09-04  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_pwm_buzzer ��Դ������
 * \copydoc demo_pwm_buzzer.c
 */

/** [src_pwm_buzzer] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_buzzer.h"

/**
 * \brief �����������������
 * \return ��
 */
aw_local void* __task_handle (void *arg)
{
    while(1) {

        aw_buzzer_loud_set(80); /* ���÷���������ǿ�� */
        aw_buzzer_beep(500);    /* ������������ʱ500ms */

        aw_buzzer_loud_set(50); /* ���÷���������ǿ�� */
        aw_buzzer_on();         /* ���������� */
        aw_mdelay(500);         /* ��ʱ500ms */
        aw_buzzer_off();        /* �رշ����� */
        aw_mdelay(500);         /* ��ʱ500ms */
    }

    return 0;
}

/******************************************************************************/
void demo_buzzer_entry (void)
{
    aw_task_id_t    tsk;

    tsk = aw_task_create("Buzzer demo",
                         12,
                         1024,
                         __task_handle,
                         (void *)NULL);
    if (tsk == NULL) {
        aw_kprintf("Buzzer demo task create failed\r\n");
        return;
    }

    if (aw_task_startup(tsk) != AW_OK) {
        aw_task_delete(tsk);
    }
}

/** [src_pwm_buzzer] */

/* end of file */
