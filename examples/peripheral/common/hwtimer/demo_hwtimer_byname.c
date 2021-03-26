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
 *   2. 禁能其它地方对LED的操作
 *
 * - 实验现象：
 *   1. LED以定时器一半的频率闪烁；
 *   2. 串口打印相应信息。
 *
 * - 备注：
 *   1. 使用该例程时需要在定时器的配置文件中修改 “设备信息” 为 “通过名称分配定时器”。
 *
 * \par 源代码
 * \snippet demo_hwtimer_byname.c src_hwtimer_byname
 *
 * \internal
 * \par modification history:
 * - 1.00 2018-07-03  sdq, first implementation
 * \endinternal
 */
/**
 * \addtogroup demo_if_hwtimer_byname 硬件定时器(byname)
 * \copydoc demo_hwtimer_byname.c
 */

/** [src_hwtimer_byname] */
#include "aworks.h"
#include "aw_int.h"
#include "aw_hwtimer.h"
#include "aw_vdebug.h"
#include "aw_led.h"
#include "aw_delay.h"
#include "aw_demo_config.h"

#define  __TEST_HWTIMER_DRV_NAME  DE_HWTIMER_TIMER_NAME     /* 定时器名字 */
#define  __TEST_HWTIMER_BUSID     DE_HWTIMER_TIMER_ID       /* 定时器所在的总线ID */

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

    /* 分配名为 __TEST_HWTIMER_DRV_NAME 的定时器 */
    mytimer = aw_hwtimer_alloc_byname(__TEST_HWTIMER_DRV_NAME,  /* 定时器名字 */
                                      __TEST_HWTIMER_BUSID,     /* 定时器所在总线ID */
                                      0,                        /* 定时器编号 */
                                      mytimer_isr,              /* 中断回调函数 */
                                      NULL);                    /* 中断回调函数的参数 */

    if (mytimer) {
        aw_hwtimer_enable(mytimer, 20);     /* 使能定时中断 20Hz */
    } else {
        aw_kprintf("Timer allocation fail!\n");
    }

    for (;;) {
        aw_mdelay(1000);
    }

    return 0;
}

/******************************************************************************/
void demo_hwtimer_byname_entry (void)
{
    aw_task_id_t    tsk;

    tsk = aw_task_create("Hwtimer byname demo",
                         12,
                         1024,
                         __task_handle,
                         NULL);
    if (tsk == NULL) {
        aw_kprintf("Hwtimer byname demo task create failed\r\n");
        return;
    }

    aw_task_startup(tsk);

}

/** [src_hwtimer_byname] */

/*end of file */
