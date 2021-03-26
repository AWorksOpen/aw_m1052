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
 * \brief AWorks �ж��ӳ���ҵ����ʹ������
 *
 * - �����̽���ʱ���ж���������ʹ���ж��ӳ���ҵִ��
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_DEV_GPIO_LED
 *      - AW_COM_CONSOLE
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *   3. ���������ط���LED�Ƶ�ʹ��
 *
 * - ʵ������
 *   1. LED_RUN �� 1s������˸��
 *
 * - ��ע��
 *   1. �����һ�����ڴ����жϷ�������ִ��ʱ�����������ϵͳ�ӳٵĹ��������ڸ�������ڵ��߳���
 *      �ȼ���ߣ��������ȴ������߳�ջ�ռ��С�� aw_prj_params.h �����ã�
 *      #define AW_CFG_ISR_DEFER_TASK_STACK_SIZE    (1024 * 10)
 *
 *   2. isr_defer �� aw_sys_defer�е�ISR defer��һ����
 *
 * \par Դ����
 * \snippet demo_isr_defer.c src_isr_defer
 * 
 * \internal
 * \par Modification history
 * - 1.00 17-09-07  may, first implementation.
 * \endinternal
 */
 
 /**
 * \addtogroup demo_if_isr_defer �ж��ӳ���ҵ����
 * \copydoc demo_isr_defer.c
 */

/** [src_isr_defer] */
#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_led.h"
#include "aw_isr_defer.h"
#include "aw_timer.h"
#include "aw_demo_config.h"

#define  LED_RUN   DM_LED
aw_local struct aw_isr_defer_job __g_defer_job;

/**
 * \brief job ��ں���
 */
aw_local void __my_defer_job (void *p_arg)
{
    aw_led_toggle(LED_RUN);
}

/**
 * \brief ��ʱ���������
 */
aw_local void __timer_isr (void *p_arg)
{
    /* ����ж��ӳٴ������� */
    aw_isr_defer_job_add(&__g_defer_job);

    /*  �ٴζ�ʱ500ms */
    aw_timer_start((aw_timer_t *)p_arg, aw_ms_to_ticks(500));
}

/**
 * \brief �ж���ʱ��ҵ���� isr_defer �������
 */
void demo_isr_defer_entry (void)
{
    aw_local aw_timer_t timer;

    /* ��ʼ�� job */
    aw_isr_defer_job_init(&__g_defer_job, __my_defer_job, &__g_defer_job);

    /* ��ʼ��һ�������ʱ�� */
    aw_timer_init(&timer, __timer_isr, (void *)&timer);
    aw_timer_start(&timer, aw_ms_to_ticks(500));
}

/** [src_isr_defer] */

/* end of file */
