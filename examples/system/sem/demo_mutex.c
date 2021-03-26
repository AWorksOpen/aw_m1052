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
 * \brief AWorks平台中互斥量使用例程（Mutex）
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - AW_COM_CONSOLE
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象：
 *   1. 如果不使用互斥信号量保护，注释掉 USE_MUTEX 的宏定义。则打印信息中会出现“calculate error
 *      !”表明全局变量访问出错（与预期情况不符），任务2，本来是将变量加上29，发现加上29后与初始读到的
 *      值加上29不同。
 *   2. 使用互斥锁后，打印信息中不会出现“calculate error!”，并且after add 29: gdata后的值为
 *      before:gdata后的值加29，表明共享资源访问正确。
 *
 * - 说明：
 *   信号量：获取和释放在同一个任务中，任务可以多次获取，但也必须对应多次释放。
 *        不可以在一个任务中获取，另外一个任务中释放。
 *        互斥信号量主要用于设备、区域的互斥访问。访问前申请，访问后释放资源。
 *        一个任务中成对出现获取（LOCK）和释放(UNLOCK)。
 *
 *   任务1和任务2都访问了全局变量__g_data。
 *   为了确保访问的正确性，就必须使用互斥信号量对其进行保护，保证全局变量的访问是互斥的。
 *   即任务1使用时，任务2就不能使用；任务2使用时，任务1不能使用，确保了变量访问的正确性。
 *
 * \par 源代码
 * \snippet demo_mutex.c src_mutex
 *
 * \internal
 * \par Modification history
 * - 1.01 17-09-06  anu, modify.
 * - 1.00 16-03-26  cod, first implementation.
 * \endinternal
 */

 /**
 * \addtogroup demo_if_mutex 互斥量
 * \copydoc demo_mutex.c
 */

/** [src_mutex] */

#include "aworks.h"
#include "aw_task.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_sem.h"


/* 使用互斥信号量保护全局变量的访问 */
#define USE_MUTEX

static unsigned int __g_data = 0;
AW_MUTEX_DECL_STATIC(__g_data_mutex);        /* 定义互斥信号量 */


/**
 * \brief 任务2入口函数
 *
 * \param[in] p_arg : 注册时用户传入的自定义参数
 *
 * \return 无
 */
static void task2_entry (void *p_arg)
{
    unsigned int temp;
    volatile unsigned int i, j;

    /* 任务通常的形式：有一个主循环，永久循环 */
    AW_FOREVER {

#ifdef USE_MUTEX
        AW_MUTEX_LOCK(__g_data_mutex, AW_SEM_WAIT_FOREVER);
#endif
        temp = __g_data;
        aw_kprintf("before:gdata = %d\r\n", __g_data);

        /* 用于长时间占有该锁 */
        for (i = 0; i < 10000; i++) {
            for (j = 0; j < 502; j++) {
                ;
            }
        }

        __g_data += 29;

        /* 如果不使用 MUTEX 可以发现，打印出来的数据与预期不符，使用后是完全相符合的 */
        aw_kprintf("after add 29: gdata = %d\r\n\r\n", __g_data);

        if ((temp + 29) != __g_data) {
            aw_kprintf("calculate error!\r\n");
        }

#ifdef USE_MUTEX
        AW_MUTEX_UNLOCK(__g_data_mutex);
#endif
        aw_mdelay(2);
    }
}

/**
 * \brief 任务1入口函数
 *
 * \param[in] p_arg : 注册时用户传入的自定义参数
 *
 * \return    无
 */
static void task1_entry(void *p_arg)
{
    unsigned int temp;

    AW_FOREVER {

#ifdef USE_MUTEX
        AW_MUTEX_LOCK(__g_data_mutex, AW_SEM_WAIT_FOREVER);
#endif

        temp = __g_data;
        __g_data -= 11;

        if ((temp - 11) != __g_data) {
            aw_kprintf("calculate error!\r\n");
        }

#ifdef USE_MUTEX
        AW_MUTEX_UNLOCK(__g_data_mutex);
#endif

        aw_mdelay(1);
    }
}

/******************************************************************************/
/**
 * \brief 互斥量demo入口函数
 */
void demo_mutex_entry (void)
{
    AW_TASK_DECL_STATIC(task1, 2048);
    AW_TASK_DECL_STATIC(task2, 2048);

    AW_TASK_INIT(task1,       /* 任务实体 */
                 "task1name", /* 任务名字 */
                 5,           /* 任务优先级 */
                 2048,        /* 任务堆栈大小 */
                 task1_entry, /* 任务入口函数 */
                 (void*)1);   /* 任务入口参数 */

    AW_TASK_INIT(task2,       /* 任务实体 */
                 "task2name", /* 任务名字 */
                 6,           /* 任务优先级 */
                 2048,        /* 任务堆栈大小 */
                 task2_entry, /* 任务入口函数 */
                 (void*)2);   /* 任务入口参数 */

    /* 初始化时，选项设置：
     *  AW_SEM_Q_FIFO（先后顺序排队）
     *  AW_SEM_Q_PRIORITY (按优先级排队)
     */
    AW_MUTEX_INIT(__g_data_mutex, AW_SEM_Q_FIFO);

    AW_TASK_STARTUP(task1);
    AW_TASK_STARTUP(task2);
}

/** [src_mutex] */

/* end of file */
