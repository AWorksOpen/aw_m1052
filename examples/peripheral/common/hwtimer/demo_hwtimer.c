/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief hwtimer 测试程序
 *
 * hwtimer 通用定时器，提供如下功能:
 *
 * - 操作步骤：
 *   1. 在aw_prj_params.h中使能
 *      - 对应平台的硬件定时器宏
 *      - AW_DEV_GPIO_LED
 *   2. 在定时器的配置文件中修改 “设备信息” 为 “分配一个符合指定要求的定时器”
 *   3. 禁能其它地方对LED的操作
 *
 * - 实验现象：
 *   1. LED以定时器一半的频率闪烁；
 *   2. 串口打印相应信息。
 *
 * - 备注：
 *   1. 如果申请失败，尝试修改三个频率相关的参数。
 *
 * \par 源代码
 * \snippet demo_hwtimer.c src_hwtimer
 *
 * \internal
 * \par modification history:
 * - 1.00 2018-07-03  sdq, first implementation
 * \endinternal
 */
/**
 * \addtogroup demo_if_hwtimer 硬件定时器
 * \copydoc demo_hwtimer.c
 */

/** [src_hwtimer] */
#include "aworks.h"
#include "aw_hwtimer.h"
#include "aw_vdebug.h"
#include "aw_led.h"
#include "aw_delay.h"
#include "aw_demo_config.h"

#define  LED  DM_LED

/**
 * \brief 硬件定时器中断服务函数。
 * \param[in] p_arg : 任务参数
 */
static void mytimer_isr (void *p_arg)
{
    aw_led_toggle(LED);
}

/**
 * \brief hwtimer 测试函数
 */
aw_local void* __task_handle (void *arg)
{
    aw_hwtimer_handle_t mytimer;

    /* 通过需求申请定时器 */
    mytimer = aw_hwtimer_alloc(1000,                    /* 申请输入频率 */
                               16,                      /* 最小频率 */
                               1200000,                 /* 最大频率 */
                               AW_HWTIMER_AUTO_RELOAD,  /* 自动重装 */
                               mytimer_isr,             /* 中断服务函数 */
                               (void *) NULL);          /* 中断服务函数入口参数 */

    if (mytimer) {
        aw_hwtimer_enable(mytimer, 20);      /* 使能定时中断 20Hz */
    } else {
        aw_kprintf("Timer allocation fail!\n");
    }

    for (;;) {
        aw_mdelay(1000);
    }

    return 0;
}

/******************************************************************************/
void demo_hwtimer_entry (void)
{
    aw_task_id_t    tsk;

    tsk = aw_task_create("Hwtimer demo",
                         12,
                         1024,
                         __task_handle,
                         NULL);
    if (tsk == NULL) {
        aw_kprintf("Hwtimer demo task create failed\r\n");
        return;
    }

    aw_task_startup(tsk);
}

/** [src_hwtimer] */

/*end of file */
