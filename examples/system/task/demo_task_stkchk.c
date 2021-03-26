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
 * \brief 线程堆栈检测例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - AW_DEV_GPIO_LED
 *      - AW_COM_CONSOLE
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *   3. 屏蔽其他使用到LED灯的地方
 *
 * - 实验现象：
 *   1. LED闪烁，串口打印出栈使用情况的相关信息
 *
 * \par 源代码
 * \snippet demo_task_stkchk.c src_task_stkchk
 *
 * \internal
 * \par Modification history
 * - 1.01 17-09-05  anu, modify.
 * - 1.00 15-01-20  hbt, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_task_stkchk 线程堆栈检测
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

/** 任务定义 */
AW_TASK_DECL_STATIC(ledtask, 2048);
AW_TASK_DECL_STATIC(stkchktask, 2048);

/**
 * \brief LED任务入口函数
 *
 * \param[in] p_arg 注册时用户传入的自定义参数
 *
 * \return 无
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
 * \brief 堆栈检测任务入口函数
 *
 * \param[in] p_arg 注册时用户传入的自定义参数
 *
 * \return 无
 */
static void stkchk_entry (void *p_arg)
{
    unsigned int ledtask_total, ledtask_free;
    unsigned int stkchktask_total, stkchktask_free;

    AW_FOREVER {

        /* 检测 ledtask 堆栈使用情况 */
        AW_TASK_STACK_CHECK(ledtask, &ledtask_total, &ledtask_free);

        /* 检测本线程堆栈使用情况 */
        AW_TASK_STACK_CHECK_SELF(&stkchktask_total, &stkchktask_free);

        /* 显示两个任务的堆栈剩余情况 */
        aw_kprintf("led_task stack   : total: %4d, free: %4d\n",
                    ledtask_total, ledtask_free);
        aw_kprintf("stkchk_task stack: total: %4d, free: %4d\n",
                    stkchktask_total, stkchktask_free);

        aw_mdelay(1000);
    }
}

/******************************************************************************/
/**
 * \brief 线程堆栈检测演示入口函数
 *
 * \param[in] 无
 *
 * \return 无
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
