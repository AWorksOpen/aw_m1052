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
 * \brief 事件组演示例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能AW_COM_CONSOLE和调试串口对应的宏；
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象：
 *   1. 串口打印相关调试信息。
 *
 * \par 源代码
 * \snippet demo_event.c src_event
 *
 * \internal
 * \par Modification History
 * - 1.00 17-09-05  anu, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_event 事件组演示例程
 * \copydoc demo_event.c
 */

/** [src_event] */

#include "aworks.h"
#include "aw_event.h"
#include "aw_vdebug.h"
#include "aw_delay.h"

static struct event_category   __g_category;        /* 事件种类 */
static struct event_type       __g_event_1;         /* 事件类型1 */
static struct event_type       __g_event_2;         /* 事件类型2 */
static struct event_handler    __g_handler_cat;     /* 事件种类句柄 */
static struct event_handler    __g_handler_evt_1_1; /* 事件类型1-1句柄 */
static struct event_handler    __g_handler_evt_2_1; /* 事件类型2-1句柄 */
static struct event_handler    __g_handler_evt_2_2; /* 事件类型2-2句柄 */
static struct event_handler    __g_handler_evt_2_3; /* 事件类型2-3句柄 */
static struct event_handler    __g_handler_evt_2_4; /* 事件类型2-4句柄 */

/**
 * \brief 事件触发后的回调函数
 *
 * \note 该函数由事件触发后进行回调，该函数会打印事件数据（触发时传递）以及用户数据（注册时记录）
 *
 * \param[in] p_evt_type 事件类型
 * \param[in] p_evt_data 事件的数据
 * \param[in] p_usr_data 用户数据
 *
 * \return 无
 */
static void __event_function_test (struct event_type *p_evt_type,
                                   void              *p_evt_data,
                                   void              *p_usr_data)
{
    static int count = 0;

    aw_kprintf("#%d: %s raised, handler_%s called\n",
               count++, (const char *)p_evt_data, (const char *)p_usr_data);
}

/**
 * \brief 测试事件组件相关接口
 *
 * \return 无
 */
void demo_event_entry (void)
{
    aw_event_category_init(&__g_category); /* 初始化事件种类 */

    aw_event_init(&__g_event_1);           /* 初始化事件类型1 */
    aw_event_init(&__g_event_2);           /* 初始化事件类型2 */

    /* 初始化事件句柄 */
    aw_event_handler_init(&__g_handler_cat, __event_function_test,
                          "category", EVENT_HANDLER_FLAG_NONE);
    aw_event_handler_init(&__g_handler_evt_1_1, __event_function_test,
                          "event_1_1", EVENT_HANDLER_FLAG_NONE);
    aw_event_handler_init(&__g_handler_evt_2_1, __event_function_test,
                          "event_2_1", EVENT_HANDLER_FLAG_NONE);
    aw_event_handler_init(&__g_handler_evt_2_2, __event_function_test,
                          "event_2_2", EVENT_HANDLER_FLAG_NONE);
    aw_event_handler_init(&__g_handler_evt_2_3, __event_function_test,
                          "event_2_3", EVENT_HANDLER_FLAG_NONE);
    /* 该事件被触发后会被移除 */
    aw_event_handler_init(&__g_handler_evt_2_4, __event_function_test,
                          "event_2_4", EVENT_HANDLER_FLAG_AUTO_UNREG);

    /* 将事件种类句柄注册到事件种类中 */
    aw_event_category_handler_register(&__g_category, &__g_handler_cat);

    /* 将事件句柄注册到对应的事件类型中 */
    aw_event_handler_register(&__g_event_1, &__g_handler_evt_1_1);

    /* 将事件句柄注册到对应的事件类型中 */
    aw_event_handler_register(&__g_event_2, &__g_handler_evt_2_1);
    aw_event_handler_register(&__g_event_2, &__g_handler_evt_2_2);
    aw_event_handler_register(&__g_event_2, &__g_handler_evt_2_3);
    aw_event_handler_register(&__g_event_2, &__g_handler_evt_2_4);

    /* 将事件类型注册到事件种类中 */
    aw_event_category_event_register(&__g_category, &__g_event_1);
    aw_event_category_event_register(&__g_category, &__g_event_2);

    aw_kprintf("event SYNC test start...\n");
    aw_event_raise(&__g_event_1, "event_1_sync", EVENT_PROC_FLAG_SYNC);         /* 同步触发事件 */
    aw_event_raise(&__g_event_1, "event_1_async", EVENT_PROC_FLAG_ASYNC);       /* 异步触发事件 */
    aw_event_raise(&__g_event_1, "event_1_cat_only", EVENT_PROC_FLAG_CAT_ONLY); /* 只触发事件种类（异步） */

    aw_mdelay(100);

    aw_kprintf("\n---------------\n");
    aw_event_raise(&__g_event_2, "event_2", EVENT_PROC_FLAG_SYNC);

    aw_kprintf("\n--------------- now handler_event_2_4 should be deleted\n");
    aw_event_raise(&__g_event_2, "event_2", EVENT_PROC_FLAG_SYNC);

    aw_kprintf("\n--------------- now handler_event_2_2 should be deleted\n");
    aw_event_handler_unregister(&__g_event_2, &__g_handler_evt_2_2);
    aw_event_raise(&__g_event_2, "event_2", EVENT_PROC_FLAG_SYNC);
}

/** [src_event] */

/* end of file */
