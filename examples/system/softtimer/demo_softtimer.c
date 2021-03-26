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
 * \brief 软件定时器例程
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
 *   1. LED_RUN以1s为周期闪烁。
 *
 * - 备注：
 *   1. 软定时器的回调一般在中断服务中执行，所以应该尽可能执行耗时短的工作，并且不能在
 *      回调中执行阻塞接口，如等待信号量、aw_mdelay等
 *
 * \par 源代码
 * \snippet demo_softtimer.c src_softtimer
 * 
 * \internal
 * \par Modification history
 * - 1.00 16-03-28  cod, first implementation.
 * \endinternal
 */
 
 /**
 * \addtogroup demo_if_softtimer 软件定时器
 * \copydoc demo_softtimer.c
 */

/** [src_softtimer] */
#include "aworks.h"
#include "aw_led.h"
#include "aw_timer.h"
#include "aw_demo_config.h"

#define LED_RUN DM_LED

/**
 * \brief 软件定时器中断入口函数
 */
aw_local void __timer_isr (void *p_arg)
{
    aw_led_toggle(LED_RUN);                     /* LED_RUN状态翻转 */

    aw_timer_start(p_arg, aw_ms_to_ticks(500)); /*  再次定时500ms */
}

/**
 * \brief 软件定时器例程入口函数
 */
void demo_softtimer_entry (void)
{
    static aw_timer_t timer;

    /* 初始化一个软件定时器 */
    aw_timer_init(&timer, __timer_isr, (void *)&timer);

    /* 定时500ms， 启动软件定时器 */
    aw_timer_start(&timer, aw_ms_to_ticks(500));
}

/** [src_softtimer] */

/* end of file */
