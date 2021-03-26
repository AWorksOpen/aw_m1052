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
 * \brief 自旋锁演示例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能AW_COM_CONSOLE；
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象：
 *   1.若定义了__USE_SPINLOCK，串口循环打印：
 *     "task1 data correct.
 *      task2 data correct."
 *   2.若屏蔽__USE_SPINLOCK，串口打印出现数据校验错误信息：
 *     "task1 data error.
 *      task2 data error."
 *
 * \par 源代码
 * \snippet demo_spinlock.c src_spinlock
 *
 * \internal
 * \par Modification History
 * - 1.00 17-09-05  sup, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_spinlock 自旋锁
 * \copydoc demo_spinlock.c
 */

/** [src_spinlock] */
#include "aworks.h"
#include "aw_spinlock.h"
#include "aw_task.h"
#include "aw_sem.h"
#include "aw_vdebug.h"
#include "aw_timer.h"
#include "aw_delay.h"

#define __USE_SPINLOCK

/* 计数器 */
volatile static uint32_t __g_counter = 0;

#ifdef __USE_SPINLOCK
/* 自旋锁 */
static aw_spinlock_isr_t __g_spin_lock;
#endif

/**
 * \brief 线程 task1 入口
 * \return 无
 */
static void __task1_entry (void *p_arg)
{
    int temp = 0;

    AW_FOREVER {

#ifdef __USE_SPINLOCK
        /* 利用自旋锁保护共享的全局变量
         * 在自旋锁中不易使用系统接口和一些复杂的API，且在自旋锁中不应操作时间过长
         * */
        aw_spinlock_isr_take(&__g_spin_lock);
#endif
        __g_counter = 1;

        /*用于长时间占用该锁*/
        aw_mdelay(1000);

        temp = __g_counter;

#ifdef __USE_SPINLOCK
        aw_spinlock_isr_give(&__g_spin_lock);
#endif

        if (temp == 1) {
            aw_kprintf("task1 data correct.\r\n");
        } else {
            aw_kprintf("task1 data error.\r\n");
        }
    }
}

/**
 * \brief 线程 task2 入口
 * \return 无
 */
static void __task2_entry (void *p_arg)
{
    int temp = 0;

    AW_FOREVER {
        /* 利用自旋锁保护共享的全局变量
         * 在自旋锁中不易使用系统接口和一些复杂的API，且在自旋锁中不应操作时间过长
         * */
#ifdef __USE_SPINLOCK
        aw_spinlock_isr_take(&__g_spin_lock);
#endif

        __g_counter = 2;

        /*用于长时间占用该锁*/
        aw_mdelay(1000);

        temp= __g_counter;

#ifdef __USE_SPINLOCK
        aw_spinlock_isr_give(&__g_spin_lock);
#endif

        if (temp == 2) {
            aw_kprintf("task2 data correct.\r\n");
        } else {
            aw_kprintf("task2 data error.\r\n");
        }

    }
}

/******************************************************************************/
/**
 * \brief 自旋锁演示例程入口
 *
 * \return 无
 */
void demo_spinlock_entry (void)
{
    AW_TASK_DECL_STATIC(sl_task1, 1024);
    AW_TASK_DECL_STATIC(sl_task2, 1024);

    AW_INFOF(("Spin lock task start...\n"));

#ifdef __USE_SPINLOCK
    aw_spinlock_isr_init(&__g_spin_lock, 0);
#endif

    AW_TASK_INIT(sl_task1,
                "demo_spinlock_task",
                 3,
                 1024,
                 __task1_entry,
                "task1");

    AW_TASK_STARTUP(sl_task1);

    AW_TASK_INIT(sl_task2,
                "demo_spinlock_task",
                 3,
                 1024,
                 __task2_entry,
                "task2");

    AW_TASK_STARTUP(sl_task2);

}

/** [src_spinlock] */

/* end of file */
