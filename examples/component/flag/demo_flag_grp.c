/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief flag_grp �¼���־�������,������ʵ��һ�Զ��̼߳��ͬ������
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��AW_COM_CONSOLE�͵��Դ��ڶ�Ӧ�ĺꣻ
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - �����߼���
 *   1. ���¼���ʼ���¼�����λΪ��0B0001 1100��Ȼ��ȴ��¼�����Ϊ��0B1110 0011�󴥷�
 *    - ���¼�1�ȴ��¼����־��4λΪ1�󣬽���4λ��0����7λ��1��
 *    - ���¼�2�ȴ��¼����־��3λΪ1�󣬽���3λ��0����6λ��1��
 *    - ���¼�3�ȴ��¼����־��2λΪ1�󣬽���2λ��0����5λ��1��
 *
 * - ʵ������
 *   1. ���¼��ȴ����¼�1��2��3�����¼��������������������ѽ��յ�3���������ָ���¼���־λ��
 *      �򽫱�־λ��0����ӡ����ȷ��Ϣ��
 *
 *
 * \par Դ����
 * \snippet demo_flag_grp.c src_flag_grp
 *
 * \internal
 * \par Modification History
 * - 1.00 17-09-06  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_flag_grp �¼���־�������
 * \copydoc demo_flag_grp.c
 */

/** [src_flag_grp] */
#include "aw_flag_grp.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_task.h"

/* �����¼��鼰�ڵ� */
aw_local aw_flag_grp_t      __g_flg_grp;    /* �����¼���־�� */
aw_local aw_flag_node_t     __g_flg_node;   /* �����¼���־�ڵ㣬 ������ʹ�� */
aw_local aw_flag_node_t     __g_flg_node1;  /* �����¼���־�ڵ� ������1ʹ��*/
aw_local aw_flag_node_t     __g_flg_node2;  /* �����¼���־�ڵ� ������2ʹ��*/
aw_local aw_flag_node_t     __g_flg_node3;  /* �����¼���־�ڵ� ������3ʹ��*/


#define  TEST_FLAG_TASK_STACK_SIZE    1024
AW_TASK_DECL_STATIC(test1_flg_task, TEST_FLAG_TASK_STACK_SIZE);
AW_TASK_DECL_STATIC(test2_flg_task, TEST_FLAG_TASK_STACK_SIZE);
AW_TASK_DECL_STATIC(test3_flg_task, TEST_FLAG_TASK_STACK_SIZE);

#define  MASTER_INIT_FLAG   0x1C
#define  MASTER_WAIT_FLAG   0xE3

aw_local void __task1_flag_entry (void *p_arg)
{
    aw_err_t  ret = AW_OK;

    for (;;) {

        /* �ȴ��¼����־��4λΪ1 */
        ret = aw_flag_pend(&__g_flg_grp,
                           &__g_flg_node1,
                           0x10,
                           AW_FLAG_NODE_WAIT_AND | AW_FLAG_NODE_WAIT_CLEAR,
                           AW_SEM_WAIT_FOREVER,
                           NULL);

        if (ret == AW_OK) {
            aw_kprintf("task1_flag_pend successful!\r\n");

            /* ���¼����־��7λ��1����4λ��0 */
            ret = aw_flag_post(&__g_flg_grp, 0x83, AW_FLAG_OPT_SET);
            if (ret == AW_OK) {
                aw_kprintf("task1_flag_post successful!\r\n");
            } else {
                AW_ERRF(("task1_flag_post err: %d\n", ret));
            }

        } else {
            AW_ERRF(("task1_flag_pend err: %d\n", ret));
        }

        aw_mdelay(100);
    }
}

aw_local void __task2_flag_entry (void *p_arg)
{
    aw_err_t  ret = AW_OK;

    for (;;) {

        // �ȴ��¼����־��3λΪ1
        ret = aw_flag_pend(&__g_flg_grp,
                           &__g_flg_node2,
                           0x08,
                           AW_FLAG_NODE_WAIT_AND | AW_FLAG_NODE_WAIT_CLEAR,
                           AW_SEM_WAIT_FOREVER,
                           NULL);

        if (ret == AW_OK) {
            aw_kprintf("task2_flag_pend successful!\r\n");

            //���¼����־��6λ��1����3λ��0
            ret = aw_flag_post(&__g_flg_grp, 0x43, AW_FLAG_OPT_SET);
            if (ret == AW_OK) {
                aw_kprintf("task2_flag_post successful!\r\n");
            } else {
                AW_ERRF(("task2_flag_post err: %d\n", ret));
            }

        } else {
            AW_ERRF(("task2_flag_pend err: %d\n", ret));
        }

        aw_mdelay(200);
    }
}

aw_local void __task3_flag_entry (void *p_arg)
{
    aw_err_t  ret = AW_OK;

    for (;;) {

        // �ȴ��¼����־��2λΪ1
        ret = aw_flag_pend(&__g_flg_grp,
                           &__g_flg_node3,
                           0x04,
                           AW_FLAG_NODE_WAIT_AND | AW_FLAG_NODE_WAIT_CLEAR,
                           AW_SEM_WAIT_FOREVER,
                           NULL);

        if (ret == AW_OK) {
            aw_kprintf("task3_flag_pend successful!\r\n");

            //���¼����־��5λ��1����2λ��0
            ret = aw_flag_post(&__g_flg_grp, 0x23, AW_FLAG_OPT_SET);
            if (ret == AW_OK) {
                aw_kprintf("task3_flag_post successful!\r\n");
            } else {
                AW_ERRF(("task3_flag_post err: %d\n", ret));
            }

        } else {
            AW_ERRF(("task3_flag_pend err: %d\n", ret));
        }

        aw_mdelay(300);
    }
}

/******************************************************************************/
/**
 * \brief �¼���־���demo
 *
 * \return ��
 */
void demo_flag_grp_entry (void)
{
    aw_err_t  ret   = AW_OK;
    uint32_t  flag  = 0;
    int       i     = 0;

    /* ��ʼ���¼��� */
    ret = aw_flag_init(&__g_flg_grp,
                       0,
                       AW_FLAG_WAIT_Q_PRIORITY);
    if (ret != AW_OK) {
        AW_ERRF(("aw_flag_init err: %d\n", ret));
        return;
    }

    /* ��ʼ���¼���ڵ� */
    ret = aw_flag_node_init(&__g_flg_node);
    if (ret != AW_OK) {
        AW_ERRF(("aw_flag_node0_init err: %d\n", ret));
        return;
    }

    /* ��ʼ���¼���ڵ�1 */
    ret = aw_flag_node_init(&__g_flg_node1);
    if (ret != AW_OK) {
        AW_ERRF(("aw_flag_node1_init err: %d\n", ret));
        return;
    }

    /* ��ʼ���¼���ڵ�2 */
    ret = aw_flag_node_init(&__g_flg_node2);
    if (ret != AW_OK) {
        AW_ERRF(("aw_flag_node2_init err: %d\n", ret));
        return;
    }

    /* ��ʼ���¼���ڵ�3 */
    ret = aw_flag_node_init(&__g_flg_node3);
    if (ret != AW_OK) {
        AW_ERRF(("aw_flag_node3_init err: %d\n", ret));
        return;
    }

    /* ��ʼ������1 */
    AW_TASK_INIT(test1_flg_task,
                 "demo_flg_task1",
                 3,
                 TEST_FLAG_TASK_STACK_SIZE,
                 __task1_flag_entry,
                 NULL);

    /* ��������1 */
    AW_TASK_STARTUP(test1_flg_task);

    /* ��ʼ������2 */
    AW_TASK_INIT(test2_flg_task,
                 "demo_flg_task2",
                 4,
                 TEST_FLAG_TASK_STACK_SIZE,
                 __task2_flag_entry,
                 NULL);

    /* ��������2 */
    AW_TASK_STARTUP(test2_flg_task);

    /* ��ʼ������3 */
    AW_TASK_INIT(test3_flg_task,
                 "demo_flg_task3",
                 5,
                 TEST_FLAG_TASK_STACK_SIZE,
                 __task3_flag_entry,
                 NULL);

    /* ��������3 */
    AW_TASK_STARTUP(test3_flg_task);

    for (i = 0; i < 5; i++ ) {

        /* �����¼����־ MASTER_INIT_FLAG */
        ret = aw_flag_post(&__g_flg_grp,
                           MASTER_INIT_FLAG,
                           AW_FLAG_OPT_SET);
        if (ret != AW_OK) {
            AW_ERRF(("aw_flag_post err: %d\n", ret));
            return;
        }

        /* �ȴ��¼����־�������� MASTER_WAIT_FLAG����Ҫ�ȴ�������1��2��3���ͷű�־λ֮���������
         * ִ�У��ȴ��ɹ��󲢽��¼����־��0 */
        ret = aw_flag_pend(&__g_flg_grp,
                           &__g_flg_node,
                           MASTER_WAIT_FLAG,
                           AW_FLAG_NODE_WAIT_AND | AW_FLAG_NODE_WAIT_CLEAR,
                           AW_SEM_WAIT_FOREVER,
                           &flag);
        if (ret != AW_OK) {
            AW_ERRF(("test aw_flag_pend err: %d\n\n", ret));
        } else {
            aw_kprintf("test aw_flag_grp successful.\n\n");
        }
    }

    /* ����������1 */
    AW_TASK_TERMINATE(test1_flg_task);

    /* ����������2 */
    AW_TASK_TERMINATE(test2_flg_task);

    /* ����������3 */
    AW_TASK_TERMINATE(test3_flg_task);

    /* ���ٽڵ� */
    ret = aw_flag_node_terminate(&__g_flg_node);
    if (ret != AW_OK) {
        AW_ERRF(("aw_flag_node_terminate err: %d\n", ret));
        return;
    }
    ret = aw_flag_node_terminate(&__g_flg_node1);
    if (ret != AW_OK) {
        AW_ERRF(("aw_flag_node_terminate err: %d\n", ret));
        return;
    }

    /* �����¼������ */
    ret = aw_flag_terminate(&__g_flg_grp);
    if (ret != AW_OK) {
        AW_ERRF(("aw_flag_node_terminate err: %d\n", ret));
        return;
    }
}

/** [src_flag_grp] */

/* end of file */
