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
 * \brief AWorks 中断延迟作业服务使用例程
 *
 * - 本例程将定时器中断所做处理，使用中断延迟作业执行
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - AW_DEV_GPIO_LED
 *      - AW_COM_CONSOLE
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *   3. 屏蔽其他地方对LED灯的使用
 *
 * - 实验现象：
 *   1. LED_RUN 以 1s周期闪烁。
 *
 * - 备注：
 *   1. 该组件一般用于处理中断服务函数中执行时间过长而导致系统延迟的工作，由于该组件所在的线程优
 *      先级最高，将最优先处理，该线程栈空间大小在 aw_prj_params.h 中配置：
 *      #define AW_CFG_ISR_DEFER_TASK_STACK_SIZE    (1024 * 10)
 *
 *   2. isr_defer 与 aw_sys_defer中的ISR defer是一样的
 *
 * \par 源代码
 * \snippet demo_isr_defer.c src_isr_defer
 * 
 * \internal
 * \par Modification history
 * - 1.00 17-09-07  may, first implementation.
 * \endinternal
 */
 
 /**
 * \addtogroup demo_if_isr_defer 中断延迟作业服务
 * \copydoc demo_isr_defer.c
 */

/** [src_isr_defer] */
#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_led.h"
#include "aw_isr_defer.h"
#include "aw_timer.h"
#include "aw_demo_config.h"

#define  LED_RUN   DM_LED
aw_local struct aw_isr_defer_job __g_defer_job;

/**
 * \brief job 入口函数
 */
aw_local void __my_defer_job (void *p_arg)
{
    aw_led_toggle(LED_RUN);
}

/**
 * \brief 软定时器服务入口
 */
aw_local void __timer_isr (void *p_arg)
{
    /* 添加中断延迟处理任务 */
    aw_isr_defer_job_add(&__g_defer_job);

    /*  再次定时500ms */
    aw_timer_start((aw_timer_t *)p_arg, aw_ms_to_ticks(500));
}

/**
 * \brief 中断延时作业服务 isr_defer 例程入口
 */
void demo_isr_defer_entry (void)
{
    aw_local aw_timer_t timer;

    /* 初始化 job */
    aw_isr_defer_job_init(&__g_defer_job, __my_defer_job, &__g_defer_job);

    /* 初始化一个软件定时器 */
    aw_timer_init(&timer, __timer_isr, (void *)&timer);
    aw_timer_start(&timer, aw_ms_to_ticks(500));
}

/** [src_isr_defer] */

/* end of file */
