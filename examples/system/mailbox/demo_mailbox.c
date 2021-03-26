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
 * \brief AWorks平台中邮箱的使用例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能AW_COM_CONSOLE；
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象：
 *   1. 任务1接收到任务2的发出的消息后，打印消息内容。
 *
 * \par 源代码
 * \snippet demo_mailbox.c src_mailbox
 *
 * \internal
 * \par Modification history
 * - 1.00 16-03-28  cod, first implementation.
 * \endinternal
 */

 /**
 * \addtogroup demo_if_mailbox 邮箱例程
 * \copydoc demo_mailbox.c
 */

/** [src_mailbox] */

#include "aworks.h"
#include "aw_task.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_mailbox.h"

#define MAILBOX_NUM  2    /* 邮箱数目 */

/* 定义一个邮箱实体, 2条消息 */
AW_MAILBOX_DECL_STATIC(__g_mailbox, MAILBOX_NUM);

/**
 * \brief 任务1入口函数
 *
 * \param[in] p_arg : 注册时用户传入的自定义参数
 *
 * \return 无
 */
aw_local void task1_entry (void *p_arg)
{
    char *p_str;

    AW_FOREVER {

        if (AW_MAILBOX_RECV(__g_mailbox,
                            (uint32_t *)&p_str,
                            AW_WAIT_FOREVER) == AW_OK) {

            aw_kprintf("task1 get mail from mailbox, the content is %s\n",
                       p_str);

        } else {
            aw_kprintf("task1 get mail failed!\n");
        }

        aw_mdelay(100);
    }
}

/**
 * \brief 任务2入口函数
 *
 * \param[in] p_arg : 注册时用户传入的自定义参数
 *
 * \return 无
 */
aw_local void task2_entry (void *p_arg)
{
    aw_local char mailbox_str1[] = "I'm a mailbox";
    aw_local char mailbox_str2[] = "I'm a anthor mailbox";
    uint32_t    cnt            = 0;

    AW_FOREVER {

        cnt++;

        if (cnt & 1) {
            /* 发送消息 1 */
            AW_MAILBOX_SEND(__g_mailbox,
                            (uint32_t)&mailbox_str1,
                            AW_WAIT_FOREVER,
                            AW_MAILBOX_PRI_NORMAL);
        } else {
            /* 发送消息 2 */
            AW_MAILBOX_SEND(__g_mailbox,
                            (uint32_t)&mailbox_str2,
                            AW_WAIT_FOREVER,
                            AW_MAILBOX_PRI_NORMAL);
        }

        aw_mdelay(1000);
    }
}

/******************************************************************************/
/**
 * \brief 邮箱 demo 入口函数
 */
void demo_mailbox_entry (void)
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

    /* 初始化消息 */
    AW_MAILBOX_INIT(__g_mailbox,
                    MAILBOX_NUM,
                    AW_MAILBOX_Q_FIFO);

    AW_TASK_STARTUP(task1);
    AW_TASK_STARTUP(task2);
}

/** [src_mailbox] */

/* end of file */
