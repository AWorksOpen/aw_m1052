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
 * \brief 软件定时器库演示例程
 *
 * 本例程使用系统定时器提供软件定时器的“时钟”。
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能AW_COM_CONSOLE和调试串口使能宏；
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *   3. 屏蔽其他地方对LED等的使用
 *
 * - 实验现象:
 *   1. LED0 以 2s 为周期闪烁
 *
 * \par 源代码
 * \snippet demo_sftimer_lib.c src_sftimer_lib
 *
 * \internal
 * \par History
 * - 1.00 16-03-25  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_sftimer_lib 软件定时器
 * \copydoc demo_sftimer_lib.c
 */

/** [src_sftimer_lib] */
#include "aworks.h"
#include "aw_sftimer_lib.h"
#include "aw_vdebug.h"
#include "aw_timer.h"
#include "aw_led.h"

#define BUCKET_SIZE   6     /* 定时器散列数 */
#define HW_CLKRATE    10    /* 系统定时器定时频率 10ms */
#define SF_TIME_MS    1000  /* 软定时器定时周期 */
#define LED_ID        0     /* LED0*/

/**
 * \brief 软定时器定时回调函数
 */
static void __sftimer_handler(void *arg)
{
    aw_led_toggle(LED_ID);
    aw_sftimer_restart((struct aw_sftimer *)arg);
}

/* 系统定时器对象 */
static  aw_timer_t  my_timer;

static void __my_callback (void *p_arg)
{
    /* 执行软件定时器定时回调函数，需要由外部定时调用来驱动 */
    aw_sftimer_group_tick((struct aw_sftimer_group *)p_arg);

    /* 重启定时器 */
    aw_timer_start(&my_timer, aw_ms_to_ticks(HW_CLKRATE));
}

/**
 * \brief 软件定时器库演示例程入口
 *
 */
void demo_sftimer_lib_entry (void)
{
    aw_err_t  ret = AW_OK;

    static struct aw_sftimer        my_sftimer;
    static struct aw_sftimer_group  my_sftimer_group;
    static struct aw_sftimer_bucket my_bucket[BUCKET_SIZE];

    /* 初始化软定时器组 */
    ret = aw_sftimer_group_init(&my_sftimer_group,  /* 软定时器组 */
                                &my_bucket[0],      /* 散列数组 */
                                BUCKET_SIZE,        /* 散列数 */
                                1000/HW_CLKRATE     /* 外部定时频率(Hz) */
                                );
    if (ret != AW_OK) {
        AW_ERRF(("aw_sftimer_group_init err: %d\n"));
        return;
    }

    /* 初始化软定时器 */
    aw_sftimer_init(&my_sftimer,
                    &my_sftimer_group,
                    __sftimer_handler,
                    &my_sftimer);

    /* 设置定时周期 */
    aw_sftimer_start_ms(&my_sftimer, SF_TIME_MS);

    /* 使用软件定时器库，需要外部提供周期运行的服务例程，来充当软件定时器库的
     * “时钟”
     *
     * 这里，我们使用系统软件定时器来定时回调
     */
    aw_timer_init(&my_timer, __my_callback, &my_sftimer_group);

    /* 启动定时器 */
    aw_timer_start(&my_timer, aw_ms_to_ticks(HW_CLKRATE));
}
/** [src_sftimer_lib] */

/* end of file */
