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
 * \brief AWorks 任务的相关服务例程（动态任务创建、删除）
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
 *   1. LED_RUN以500ms的时间间隔亮灭
 *   2. 串口打印当前执行任务名
 *   3. 任务2串口打印20次后，任务被删除，不再打印
 *
 * \par 源代码
 * \snippet demo_task_dynamic.c src_task_dynamic
 *
 * \internal
 * \par Modification history
 * - 1.01 17-09-05  anu, first implementation.
 * \endinternal
 */

 /**
 * \addtogroup demo_if_task_dynamic 动态任务
 * \copydoc demo_task_dynamic.c
 */

/** [src_task_dynamic] */
#include "aworks.h"
#include "aw_task.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_led.h"
#include "aw_demo_config.h"

#define LED_RUN DM_LED

/**
 * \brief 任务2入口函数
 *
 * \param[in] p_arg 注册时用户传入的自定义参数
 *
 * \return 无
 */
static void task2_entry (void *p_arg)
{
    aw_task_id_t task2_id = aw_task_id_self();  /* 获取当前任务的ID */
    uint8_t      cnt      = 0;
    int          ret;

    /* 任务通常的形式：有一个主循环，永久循环 */
    AW_FOREVER {
        aw_kprintf("Current Task's name is %s, Current Count is %d\r\n",
                    aw_task_name_get(task2_id), ++cnt);

        aw_mdelay(500);

        /* cnt计数到20后， 终止当前任务  */
        if (cnt == 20) {
            aw_kprintf("current task will be terminated\r\n");
            aw_task_exit(&ret);
        }
    }
}

/**
 * \brief 任务1入口函数
 *
 * \param[in] p_arg 注册时用户传入的自定义参数
 *
 * \return 无
 */
static void task1_entry(void *p_arg)
{
    aw_task_id_t task2;     /* 定义即将创建的任务2的ID，利用ID完成任务创建 */

    /*
     * 可以在任务中创建另外一个任务，如下所示，创建任务2
     */
    task2 = aw_task_create("task2",       /* 任务名字 */
                            4,            /* 任务优先级 */
                            2048,         /* 任务堆栈大小 */
                            task2_entry,  /* 任务入口函数 */
                            (void*)2);    /* 任务入口参数 */

    aw_task_startup(task2);    /* 可以使用任务ID启动一个任务 */


    /*
     * 任务启动时，会进行一次任务调用，由于任务2优先级比任务1优先级高（优先级数值越小，优先级越高）
     * 因此，启动后，会首先去执行任务2，遇到任务2的延时函数时，才会释放CPU，回到任务1继续执行
     *
     */

    /* 任务通常的形式：有一个主循环，永久循环 */
    AW_FOREVER {

        aw_kprintf("Current Task's name is %s\r\n",
                    aw_task_name_get(aw_task_id_self()));

        aw_led_toggle(LED_RUN);
        /* 任务延时 500ms，与aw_mdelay(500)相同 */
        aw_task_delay(aw_sys_clkrate_get() * 500 / 1000);
    }
}

/******************************************************************************/
/**
 * \brief 动态任务操作的入口函数
 *
 * \return 无
 */
void demo_task_dynamic_entry (void)
{
    aw_task_id_t task1;

    /*
     * 常用的创建任务的方式是利用宏创建。
     * 使用宏操作创建任务两步曲: 
     *      1、动态创建任务 aw_task_create
     *      2、启动任务     aw_task_startup
     */
    task1 = aw_task_create("task1",       /* 任务名字 */
                            5,            /* 任务优先级 */
                            2048,         /* 任务堆栈大小 */
                            task1_entry,  /* 任务入口函数 */
                            (void*)1);    /* 任务入口参数 */

    aw_task_startup(task1);    /* 启动task1,由于当前其优先级最高，进入运行函数task1_entry */
}

/** [src_task_dynamic] */

/* end of file */
