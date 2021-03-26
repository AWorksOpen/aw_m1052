/*******************************************************************************
*                                 AWorks
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
 * \brief �������з������̣���̬���䣩
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��AW_COM_CONSOLE�͵��Դ��ڶ�Ӧ�ĺꣻ
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������
 *   1. ���ڴ�ӡ��ǰִ�еĹ������С�
 *      ÿ��5s�������й������С����������ȼ�����ÿ�δ�ӡ�ĵ�һ�������ž�ΪQJOB_NUM-1
 *
 * \par Դ����
 * \snippet demo_jobq_alloc.c src_jobq_alloc
 *
 * \internal
 * \par Modification history
 * - 1.00 16-04-05  cod, first implementation.
 * \endinternal
 */

 /**
 * \addtogroup demo_if_jobq_alloc ��������(��̬����)
 * \copydoc demo_jobq_alloc.c
 */

/** [src_jobq_alloc] */
#include "aworks.h"
#include "aw_task.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_job_queue.h"

#define QJOB_NUM  10  /*������  */

/**
 * \brief �������д����������
 */
static void __process_entry (aw_job_queue_id_t p_jobq_id)
{
    AW_FOREVER {

        /* �������д��� */
        aw_job_queue_process(p_jobq_id);

        aw_mdelay(20);
    }
}

/**
 * \brief �������
 */
static void __job_queue_func (void *p_arg)
{
    aw_kprintf("Current Job Number: %d\r\n", (int)p_arg);
}

/******************************************************************************/
/**
 * \brief �������У���̬���䣩�������ں���
 */
void demo_jobq_alloc_entry (void)
{
    AW_TASK_DECL_STATIC(jobq_process_task, 2048);
    static aw_job_queue_id_t   jobq_id;
    static aw_qjob_std_pool_t  qjob_pool;
    int                        i    = 0;
    aw_bool_t                  flag = AW_FALSE;

    /* ��ʼ���������г�  */
    aw_job_queue_std_pool_init(&qjob_pool);

    /* ����һ��jodq ID */
    jobq_id = aw_job_queue_create(&qjob_pool);
    if (jobq_id == NULL) {
        return;
    }

    /* ��̬����QJOB_NUM������ */
    aw_job_queue_std_jobs_alloc(&qjob_pool, QJOB_NUM);

    AW_TASK_INIT(jobq_process_task,     /* ����ʵ��     */
                 "jobq_process_task",   /* ��������     */
                 7,                     /* �������ȼ�   */
                 2048,                  /* �����ջ��С */
                 __process_entry,       /* ������ں��� */
                 (void*)jobq_id);       /* ������ڲ��� */

    AW_TASK_STARTUP(jobq_process_task); /* �����������д������� */

    AW_FOREVER {

        /* ���ܴӵ͵��ߵ����ȼ����ͻ��ߴӸߵ��͵����ȼ����ͣ�
         * �������д�����������ȼ���ʼִ�й��� */
        if (flag == AW_TRUE) {

            /* ��������ȼ���ʼ�������й���  */
            for (i = QJOB_NUM - 1; i >= 0; i--) {
                aw_job_queue_std_post(jobq_id,
                                      i,                /* �������ȼ�    */
                                      __job_queue_func, /* ������ں���  */
                                      (void *)i,        /* ������ڲ���1 */
                                      NULL,             /* ������ڲ���2 */
                                      NULL,             /* ������ڲ���3 */
                                      NULL);            /* ������ڲ���4 */
            }
        } else {

            /* ��������ȼ���ʼ�������й��� */
            for (i = 0; i < QJOB_NUM; i++) {
                aw_job_queue_std_post(jobq_id,
                                      i,                /* �������ȼ�    */
                                      __job_queue_func, /* ������ں���  */
                                      (void *)i,        /* ������ڲ���1 */
                                      NULL,             /* ������ڲ���2 */
                                      NULL,             /* ������ڲ���3 */
                                      NULL);            /* ������ڲ���4 */
            }
        }

        flag = !flag;

        aw_mdelay(5000);
    }
}

/** [src_jobq_alloc] */

/* end of file */
