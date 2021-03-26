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
 * \brief �̶߳�ջ�������
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_DEV_GPIO_LED
 *      - AW_COM_CONSOLE
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *   3. ��������ʹ�õ�LED�Ƶĵط�
 *
 * - ʵ������
 *   1. LED��˸�����ڴ�ӡ��ջʹ������������Ϣ
 *
 * \par Դ����
 * \snippet demo_task_stkchk.c src_task_stkchk
 *
 * \internal
 * \par Modification history
 * - 1.01 17-09-05  anu, modify.
 * - 1.00 15-01-20  hbt, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_task_stkchk �̶߳�ջ���
 * \copydoc demo_task_stkchk.c
 */

/** [src_task_stkchk] */

#include "aworks.h"
#include "aw_task.h"
#include "aw_delay.h"
#include "aw_led.h"
#include "aw_vdebug.h"
#include "aw_demo_config.h"

#define LED_RUN DM_LED

/** ������ */
AW_TASK_DECL_STATIC(ledtask, 2048);
AW_TASK_DECL_STATIC(stkchktask, 2048);

/**
 * \brief LED������ں���
 *
 * \param[in] p_arg ע��ʱ�û�������Զ������
 *
 * \return ��
 */
static void led_entry(void *p_arg)
{
    AW_FOREVER{
        aw_led_on(LED_RUN);
        aw_mdelay(250);
        aw_led_off(LED_RUN);
        aw_mdelay(250);
    }
}

/**
 * \brief ��ջ���������ں���
 *
 * \param[in] p_arg ע��ʱ�û�������Զ������
 *
 * \return ��
 */
static void stkchk_entry (void *p_arg)
{
    unsigned int ledtask_total, ledtask_free;
    unsigned int stkchktask_total, stkchktask_free;

    AW_FOREVER {

        /* ��� ledtask ��ջʹ����� */
        AW_TASK_STACK_CHECK(ledtask, &ledtask_total, &ledtask_free);

        /* ��Ȿ�̶߳�ջʹ����� */
        AW_TASK_STACK_CHECK_SELF(&stkchktask_total, &stkchktask_free);

        /* ��ʾ��������Ķ�ջʣ����� */
        aw_kprintf("led_task stack   : total: %4d, free: %4d\n",
                    ledtask_total, ledtask_free);
        aw_kprintf("stkchk_task stack: total: %4d, free: %4d\n",
                    stkchktask_total, stkchktask_free);

        aw_mdelay(1000);
    }
}

/******************************************************************************/
/**
 * \brief �̶߳�ջ�����ʾ��ں���
 *
 * \param[in] ��
 *
 * \return ��
 */
void demo_task_stkchk_entry (void)
{
    AW_TASK_INIT(ledtask,    "led_task",    3, 2048, led_entry,    0);
    AW_TASK_INIT(stkchktask, "stkchk_task", 3, 2048, stkchk_entry, 0);

    AW_TASK_STARTUP(ledtask);
    AW_TASK_STARTUP(stkchktask);
}

/** [src_task_stkchk] */

/* end of file */
