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
 * \brief 工作队列服务例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能AW_COM_CONSOLE和调试串口对应的宏；
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象：
 *   1. 串口打印当前执行的工作队列。
 *      每隔5s发送所有工作队列。按工作优先级排序，每次打印的第一个工作号均为QJOB_NUM-1
 *
 * \par 源代码
 * \snippet demo_job_queue.c src_job_queue
 *
 * \internal
 * \par Modification history
 * - 1.00 16-04-05  cod, first implementation.
 * \endinternal
 */

 /**
 * \addtogroup demo_if_job_queue 工作队列
 * \copydoc demo_job_queue.c
 */

/** [src_job_queue] */
#include "aworks.h"
#include "aw_task.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_job_queue.h"

#define QJOB_NUM  10  /*工作数 */

/**
 * \brief 工作队列处理任务入口
 */
static void __process_entry (aw_job_queue_id_t p_jobq_id)
{
    AW_FOREVER {

        /* 工作队列处理 */
        aw_job_queue_process(p_jobq_id);

        aw_mdelay(20);
    }
}

/**
 * \brief 工作入口
 */
static void __job_queue_func (void *p_arg)
{
    aw_kprintf("Current Job Number: %d\r\n", (int)p_arg);
}

/******************************************************************************/
/**
 * \brief 工作队列服务 demo
 */
void demo_job_queue_entry (void)
{
    AW_TASK_DECL_STATIC(jobq_process_task, 2048);
    static aw_job_queue_t   jobq;
    static aw_qjob_t        my_qjob[QJOB_NUM];
    int                     i    = 0;
    aw_bool_t               flag = AW_FALSE;

    /* 初始化工作队列  */
    aw_job_queue_init(&jobq, NULL);

    AW_TASK_INIT(jobq_process_task,     /* 任务实体       */
                 "jobq_process_task",   /* 任务名字       */
                 7,                     /* 任务优先级     */
                 2048,                  /* 任务堆栈大小   */
                 __process_entry,       /* 任务入口函数   */
                 (void*)&jobq);         /* 任务入口参数   */

    AW_TASK_STARTUP(jobq_process_task); /* 启动工作队列处理任务 */

    /* 初始化所有工作， my_qjob[0]优先级最低， my_qjob[QJOB_NUM - 1]优先级最高
     * 每个工作函数传入参数为my_qjob数组下标。
     */
    for (i = 0; i < QJOB_NUM; i++) {
        aw_job_queue_fill_job(&my_qjob[i], __job_queue_func, (void *)i, i, 0);
    }

    AW_FOREVER {

        /* 不管从低到高的优先级发送或者从高到低的优先级发送，
         * 工作队列处理都从最高优先级开始执行工作 */
        if (flag == AW_TRUE) {

            /* 从最高优先级开始发送所有工作 */
            for (i = QJOB_NUM - 1; i >= 0; i--) {
                aw_job_queue_post(&jobq, &my_qjob[i]);
            }
        } else {

            /* 从最低优先级开始发送所有工作 */
            for (i = 0; i < QJOB_NUM; i++) {
                aw_job_queue_post(&jobq, &my_qjob[i]);
            }
        }

        flag = !flag;

        aw_mdelay(5000);
    }
}

/** [src_job_queue] */

/* end of file */
