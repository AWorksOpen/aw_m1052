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
 * \brief AWorksƽ̨�������ʹ������
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��AW_COM_CONSOLE��
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������
 *   1. ����1���յ�����2�ķ�������Ϣ�󣬴�ӡ��Ϣ���ݡ�
 *
 * \par Դ����
 * \snippet demo_mailbox.c src_mailbox
 *
 * \internal
 * \par Modification history
 * - 1.00 16-03-28  cod, first implementation.
 * \endinternal
 */

 /**
 * \addtogroup demo_if_mailbox ��������
 * \copydoc demo_mailbox.c
 */

/** [src_mailbox] */

#include "aworks.h"
#include "aw_task.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_mailbox.h"

#define MAILBOX_NUM  2    /* ������Ŀ */

/* ����һ������ʵ��, 2����Ϣ */
AW_MAILBOX_DECL_STATIC(__g_mailbox, MAILBOX_NUM);

/**
 * \brief ����1��ں���
 *
 * \param[in] p_arg : ע��ʱ�û�������Զ������
 *
 * \return ��
 */
aw_local void task1_entry (void *p_arg)
{
    char *p_str;

    AW_FOREVER {

        if (AW_MAILBOX_RECV(__g_mailbox,
                            (uint32_t *)&p_str,
                            AW_WAIT_FOREVER) == AW_OK) {

            aw_kprintf("task1 get mail from mailbox, the content is %s\n",
                       p_str);

        } else {
            aw_kprintf("task1 get mail failed!\n");
        }

        aw_mdelay(100);
    }
}

/**
 * \brief ����2��ں���
 *
 * \param[in] p_arg : ע��ʱ�û�������Զ������
 *
 * \return ��
 */
aw_local void task2_entry (void *p_arg)
{
    aw_local char mailbox_str1[] = "I'm a mailbox";
    aw_local char mailbox_str2[] = "I'm a anthor mailbox";
    uint32_t    cnt            = 0;

    AW_FOREVER {

        cnt++;

        if (cnt & 1) {
            /* ������Ϣ 1 */
            AW_MAILBOX_SEND(__g_mailbox,
                            (uint32_t)&mailbox_str1,
                            AW_WAIT_FOREVER,
                            AW_MAILBOX_PRI_NORMAL);
        } else {
            /* ������Ϣ 2 */
            AW_MAILBOX_SEND(__g_mailbox,
                            (uint32_t)&mailbox_str2,
                            AW_WAIT_FOREVER,
                            AW_MAILBOX_PRI_NORMAL);
        }

        aw_mdelay(1000);
    }
}

/******************************************************************************/
/**
 * \brief ���� demo ��ں���
 */
void demo_mailbox_entry (void)
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

    /* ��ʼ����Ϣ */
    AW_MAILBOX_INIT(__g_mailbox,
                    MAILBOX_NUM,
                    AW_MAILBOX_Q_FIFO);

    AW_TASK_STARTUP(task1);
    AW_TASK_STARTUP(task2);
}

/** [src_mailbox] */

/* end of file */
