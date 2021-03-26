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
 * \brief AWorks平台中消息队列的使用例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - AW_DEV_GPIO_LED
 *      - AW_COM_CONSOLE
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *   3. 屏蔽其他地方LED灯的使用
 *
 * - 实验现象：
 *   1. 任务2每隔20s给任务1发送10条消息，任务1接收消息后，按消息数据设置LED闪烁时间。
 *   2. LED_RUN闪烁由快变慢后关闭，待接收到消息后继续闪烁。
 *
 * \par 源代码
 * \snippet demo_msgq.c src_msgq
 *
 * \internal
 * \par Modification history
 * - 1.01 17-09-07  anu, modify.
 * - 1.00 16-03-28  cod, first implementation.
 * \endinternal
 */

 /**
 * \addtogroup demo_if_msgq 消息队列
 * \copydoc demo_msgq.c
 */

/** [src_msgq] */

#include "aworks.h"
#include "aw_task.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_led.h"
#include "aw_msgq.h"
#include "aw_demo_config.h"

#define LED_RUN DM_LED

#define MSG_NUM  10    /* 消息数目 */

/* 定义一个消息队列(LED闪烁时间间隔)，10条消息，每条消息大小为4个字节 */
AW_MSGQ_DECL_STATIC(__g_msgq, MSG_NUM, sizeof(uint32_t));

/**
 * \brief 任务1入口函数
 *
 * \param[in] p_arg : 注册时用户传入的自定义参数
 *
 * \return 无
 */
static void task1_entry (void *p_arg)
{
    uint32_t light_ms;

    AW_FOREVER {

        AW_MSGQ_RECEIVE(__g_msgq,              /* 消息队列 */
                        &light_ms,             /* 接收缓冲区 */
                        sizeof(uint32_t),      /* 接收一条消息 */
                        AW_MSGQ_WAIT_FOREVER); /* 永久等待一个消息 */

        aw_led_on(LED_RUN);
        aw_mdelay(light_ms);
        aw_led_off(LED_RUN);
        aw_mdelay(light_ms);
    }
}

/**
 * \brief 任务2入口函数
 *
 * \param[in] p_arg : 注册时用户传入的自定义参数
 *
 * \return 无
 */
static void task2_entry (void *p_arg)
{
    uint32_t light_ms[MSG_NUM];
    uint8_t  i;

    for (i = 0; i < MSG_NUM; i++) {
        light_ms[i] = (i + 1) * 100;
    }

    AW_FOREVER {

        /* 发送所有消息 */
        for (i = 0; i < MSG_NUM; i++) {
            /* 发送消息  */
            AW_MSGQ_SEND(__g_msgq,             /* 消息队列 */
                         &light_ms[i],         /* 发送消息 */
                         sizeof(uint32_t),     /* 发送消息大小 */
                         AW_MSGQ_WAIT_FOREVER, /* 永久等待消息被发送（消息队列满时） */
                         AW_MSGQ_PRI_NORMAL);
        }

        aw_mdelay(20000); /* 延时20s，等待接收方接收所有消息  */

    }
}

/******************************************************************************/
/**
 * \brief 消息队列处理的入口函数
 */
void demo_msgq_entry (void)
{
    AW_TASK_DECL_STATIC(task1, 2048);
    AW_TASK_DECL_STATIC(task2, 2048);

    AW_TASK_INIT(task1,                /* 任务实体*/
                 "task1",              /* 任务名字 */
                 5,                    /* 任务优先级 */
                 2048,                 /* 任务堆栈大小 */
                 task1_entry,          /* 任务入口函数 */
                 NULL);                /* 任务入口参数 */

    AW_TASK_INIT(task2,                /* 任务实体 */
                "task2",               /* 任务名字 */
                 6,                    /* 任务优先级 */
                 2048,                 /* 任务堆栈大小 */
                 task2_entry,          /* 任务入口函数 */
                 NULL);                /* 任务入口参数 */

    AW_MSGQ_INIT(__g_msgq,
                 MSG_NUM,              /* 消息的条数 */
                 sizeof(uint32_t),     /* 每条消息的大小   */
                 AW_MSGQ_Q_PRIORITY);  /* 初始化时，选项设置： */
                                       /* AW_SEM_Q_PRIORITY (按优先级排队)(参数暂未使用) */

    AW_TASK_STARTUP(task1);
    AW_TASK_STARTUP(task2);
}

/** [src_msgq] */

/* end of file */
