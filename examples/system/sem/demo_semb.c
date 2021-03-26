/*******************************************************************************
*                                 AWorks
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
 * \brief AWorksƽ̨�������ź�����ʹ������
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
 *   1. LED_RUN��1s������˸
 *   2. ���ڴ�ӡ������Ϣ
 *
 * \par Դ����
 * \snippet demo_semb.c src_semb
 *
 * \internal
 * \par Modification history
 * - 1.00 17-09-06  anu, modify.
 * - 1.00 16-03-26  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_semb �������ź���
 * \copydoc demo_semb.c
 */

/** [src_semb] */

#include "aworks.h"
#include "aw_task.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_led.h"
#include "aw_sem.h"
#include "aw_demo_config.h"

#define LED_RUN DM_LED

/* ����һ���������ź��� */
AW_SEMB_DECL_STATIC(__g_semb);

/**
 * \brief �ź�����������
 *
 * \param[in] p_arg : ע��ʱ�û�������Զ������
 *
 * \return ��
 */
static void sem_task_entry (void *p_arg)
{
    AW_FOREVER {

        aw_kprintf("\r\nsem_task: give a semb!\r\n");

        /* �����ź���
         * �������ź������ֵ��Ϊ1���ͷ��ٶ࣬ʹ��һ��Ҳ��������
         */
        AW_SEMB_GIVE(__g_semb);

        aw_mdelay(500);
    }
}

/**
 * \brief LED ������ں���
 *
 * \param[in] p_arg : ע��ʱ�û�������Զ������
 *
 * \return ��
 */
static void led_task_entry (void *p_arg)
{
    AW_FOREVER {

        aw_kprintf("\r\nled task: wait a semb!\r\n");

        /* �ȴ��ź��� */
        AW_SEMB_TAKE(__g_semb, AW_SEM_WAIT_FOREVER);

        aw_kprintf("\r\nled task: recv a semb!\r\n");

        /* �յ��������ź����󣬷�תLED�� */
        aw_led_toggle(LED_RUN);
    }
}

/******************************************************************************/
/**
 * \brief �������ź����������ں���
 *
 * \return ��
 */
void demo_semb_entry (void)
{
    AW_TASK_DECL_STATIC(led_task, 1024);
    AW_TASK_DECL_STATIC(sem_task, 1024);

    AW_TASK_INIT(led_task,          /* ����ʵ�� */
                "led_task",         /* �������� */
                 5,                 /* �������ȼ� */
                 1024,              /* �����ջ��С */
                 led_task_entry,    /* ������ں��� */
                 (void*)0);         /* ������ڲ��� */

    AW_TASK_INIT(sem_task,          /* ����ʵ�� */
                "sem_task",         /* �������� */
                 6,                 /* �������ȼ�   */
                 1024,              /* �����ջ��С */
                 sem_task_entry,    /* ������ں��� */
                 (void*)0);         /* ������ڲ��� */

    AW_SEMB_INIT(__g_semb,
                 AW_SEM_EMPTY,      /* ��ʼ��Ϊ��״̬ */
                 AW_SEM_Q_FIFO);    /* ��ʼ��ʱ��ѡ�����ã�AW_SEM_Q_FIFO���Ⱥ�˳���Ŷӣ� */
                                    /* AW_SEM_Q_PRIORITY (�����ȼ��Ŷ�) */

    AW_TASK_STARTUP(led_task);
    AW_TASK_STARTUP(sem_task);
}

/** [src_semb] */

/* end of file */
