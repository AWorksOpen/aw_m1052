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
 * \brief AWorksƽ̨����Ϣ���е�ʹ������
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_DEV_GPIO_LED
 *      - AW_COM_CONSOLE
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *   3. ���������ط�LED�Ƶ�ʹ��
 *
 * - ʵ������
 *   1. ����2ÿ��20s������1����10����Ϣ������1������Ϣ�󣬰���Ϣ��������LED��˸ʱ�䡣
 *   2. LED_RUN��˸�ɿ������رգ������յ���Ϣ�������˸��
 *
 * \par Դ����
 * \snippet demo_msgq.c src_msgq
 *
 * \internal
 * \par Modification history
 * - 1.01 17-09-07  anu, modify.
 * - 1.00 16-03-28  cod, first implementation.
 * \endinternal
 */

 /**
 * \addtogroup demo_if_msgq ��Ϣ����
 * \copydoc demo_msgq.c
 */

/** [src_msgq] */

#include "aworks.h"
#include "aw_task.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_led.h"
#include "aw_msgq.h"
#include "aw_demo_config.h"

#define LED_RUN DM_LED

#define MSG_NUM  10    /* ��Ϣ��Ŀ */

/* ����һ����Ϣ����(LED��˸ʱ����)��10����Ϣ��ÿ����Ϣ��СΪ4���ֽ� */
AW_MSGQ_DECL_STATIC(__g_msgq, MSG_NUM, sizeof(uint32_t));

/**
 * \brief ����1��ں���
 *
 * \param[in] p_arg : ע��ʱ�û�������Զ������
 *
 * \return ��
 */
static void task1_entry (void *p_arg)
{
    uint32_t light_ms;

    AW_FOREVER {

        AW_MSGQ_RECEIVE(__g_msgq,              /* ��Ϣ���� */
                        &light_ms,             /* ���ջ����� */
                        sizeof(uint32_t),      /* ����һ����Ϣ */
                        AW_MSGQ_WAIT_FOREVER); /* ���õȴ�һ����Ϣ */

        aw_led_on(LED_RUN);
        aw_mdelay(light_ms);
        aw_led_off(LED_RUN);
        aw_mdelay(light_ms);
    }
}

/**
 * \brief ����2��ں���
 *
 * \param[in] p_arg : ע��ʱ�û�������Զ������
 *
 * \return ��
 */
static void task2_entry (void *p_arg)
{
    uint32_t light_ms[MSG_NUM];
    uint8_t  i;

    for (i = 0; i < MSG_NUM; i++) {
        light_ms[i] = (i + 1) * 100;
    }

    AW_FOREVER {

        /* ����������Ϣ */
        for (i = 0; i < MSG_NUM; i++) {
            /* ������Ϣ  */
            AW_MSGQ_SEND(__g_msgq,             /* ��Ϣ���� */
                         &light_ms[i],         /* ������Ϣ */
                         sizeof(uint32_t),     /* ������Ϣ��С */
                         AW_MSGQ_WAIT_FOREVER, /* ���õȴ���Ϣ�����ͣ���Ϣ������ʱ�� */
                         AW_MSGQ_PRI_NORMAL);
        }

        aw_mdelay(20000); /* ��ʱ20s���ȴ����շ�����������Ϣ  */

    }
}

/******************************************************************************/
/**
 * \brief ��Ϣ���д������ں���
 */
void demo_msgq_entry (void)
{
    AW_TASK_DECL_STATIC(task1, 2048);
    AW_TASK_DECL_STATIC(task2, 2048);

    AW_TASK_INIT(task1,                /* ����ʵ��*/
                 "task1",              /* �������� */
                 5,                    /* �������ȼ� */
                 2048,                 /* �����ջ��С */
                 task1_entry,          /* ������ں��� */
                 NULL);                /* ������ڲ��� */

    AW_TASK_INIT(task2,                /* ����ʵ�� */
                "task2",               /* �������� */
                 6,                    /* �������ȼ� */
                 2048,                 /* �����ջ��С */
                 task2_entry,          /* ������ں��� */
                 NULL);                /* ������ڲ��� */

    AW_MSGQ_INIT(__g_msgq,
                 MSG_NUM,              /* ��Ϣ������ */
                 sizeof(uint32_t),     /* ÿ����Ϣ�Ĵ�С   */
                 AW_MSGQ_Q_PRIORITY);  /* ��ʼ��ʱ��ѡ�����ã� */
                                       /* AW_SEM_Q_PRIORITY (�����ȼ��Ŷ�)(������δʹ��) */

    AW_TASK_STARTUP(task1);
    AW_TASK_STARTUP(task2);
}

/** [src_msgq] */

/* end of file */
