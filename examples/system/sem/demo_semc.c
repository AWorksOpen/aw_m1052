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
 * \brief AWorks平台计数信号量的使用例程
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
 *   1. LED_RUN每隔10s开始闪烁，闪烁的间隔为500ms
 *   2. 串口打印调试信息
 *
 * \par 源代码
 * \snippet demo_semc.c src_semc
 *
 * \internal
 * \par Modification history
 * - 1.00 17-09-07  anu, modify.
 * - 1.00 16-03-26  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_semc 计数信号量
 * \copydoc demo_semc.c
 */

/** [src_semc] */

#include "aworks.h"
#include "aw_task.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_led.h"
#include "aw_sem.h"
#include "aw_demo_config.h"

#define LED_RUN DM_LED

/* 定义一个计数信号量 */
AW_SEMC_DECL_STATIC(__g_semc);

/**
 * \brief 信号量发送任务
 *
 * \param[in] p_arg : 注册时用户传入的自定义参数
 *
 * \return 无
 */
static void sem_task_entry (void *p_arg)
{
    AW_FOREVER {

        aw_kprintf("sem_task: give 3 sems\r\n\r\n");

        AW_SEMC_GIVE(__g_semc);   /* 计数信号量加1 */
        AW_SEMC_GIVE(__g_semc);   /* 计数信号量加1 */
        AW_SEMC_GIVE(__g_semc);   /* 计数信号量加1 */

        aw_mdelay(10000);
    }
}

/**
 * \brief 任务入口函数
 *
 * \param[in] p_arg : 注册时用户传入的自定义参数
 *
 * \return 无
 */
static void led_task_entry (void *p_arg)
{
    AW_FOREVER {

        aw_kprintf("led task: wait a semc!\r\n");

        AW_SEMC_TAKE(__g_semc, AW_SEM_WAIT_FOREVER); /* 等待信号量 */

        aw_kprintf("led task: recv a semc!\r\n");

        /* 收到二进制信号量后，翻转LED灯*/
        aw_led_toggle(LED_RUN);
        aw_mdelay(500);
    }
}

/******************************************************************************/
/**
 * \brief 计数信号量 demo 入口函数
 *
 * \return 无
 */
void demo_semc_entry (void)
{
    AW_TASK_DECL_STATIC(led_task, 1024);
    AW_TASK_DECL_STATIC(sem_task, 1024);

    AW_TASK_INIT(led_task,          /* 任务实体 */
                "led_task",         /* 任务名字 */
                 5,                 /* 任务优先级 */
                 1024,              /* 任务堆栈大小 */
                 led_task_entry,    /* 任务入口函数 */
                 (void*)0);         /* 任务入口参数 */

    AW_TASK_INIT(sem_task,          /* 任务实体 */
                "sem_task",         /* 任务名字 */
                 6,                 /* 任务优先级 */
                 1024,              /* 任务堆栈大小 */
                 sem_task_entry,    /* 任务入口函数 */
                 (void*)0);         /* 任务入口参数 */

    AW_SEMC_INIT(__g_semc,
                 AW_SEM_EMPTY,      /* 初始化为空状态 */
                 AW_SEM_Q_FIFO);    /* 初始化时，选项设置：AW_SEM_Q_FIFO(先后顺序排队) */
                                    /* AW_SEM_Q_PRIORITY (按优先级排队) */

    AW_TASK_STARTUP(led_task);
    AW_TASK_STARTUP(sem_task);
}

/** [src_semc] */

/* end of file */
