/*******************************************************************************
 *                                  AWorks
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
 * \brief 引用计数演示例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能AW_COM_CONSOLE和调试串口使能宏；
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象：
 *      串口打印操作信息。
 *
 * - 备注：
 *     1.如果在多个任务中使用同一个内存资源，在某个函数使用完成后，不确定是否还有其他的任务使用这块资源，故不能直接释放资源；
 *     而对资源对象进行引用计数，释放前判断是否有其他任务调用资源，当计数值为0时，资源无效再将其释放。
 *     2.任务一初始化引用计数对象，任务二也用到了这个对象，每个任务在使用完公用资源后，都调用aw_refcnt_put()接口释放
 *     引用计数，只有当引用计数为0时才会调用回调函数，真正的释放掉内存资源。
 *
 * \par 源代码
 * \snippet demo_refcnt.c src_refcnt
 *
 * \internal
 * \par Modification History
 * - 1.00 17-09-06  sup, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_refcnt 引用计数
 * \copydoc demo_refcnt.c
 */

/** [src_refcnt] */
#include "aworks.h"
#include "aw_refcnt.h"
#include "aw_vdebug.h"
#include "aw_mem.h"
#include "aw_task.h"
#include <string.h>
#include "aw_delay.h"

#define MEM_SIZE   10   /* 动态内存大小 */

struct ref_mem {
    struct aw_refcnt    ref;
    uint8_t            *p_mem;
};

AW_MUTEX_DECL_STATIC(__g_data_mutex);   /* 定义互斥信号量，用于保护全局变量 */

aw_local char           __g_str[24];    /* 存放字符串 */
aw_local struct ref_mem test_ref;       /* 定义一个全局的引用计数对象 */


/**
 * \brief 引用计数释放回调接口
 * \return 无
 */
static void __release_callback (struct aw_refcnt *p_ref)
{
    struct ref_mem  *p = AW_CONTAINER_OF(p_ref, struct ref_mem, ref);

    /* 释放动态内存 */
    aw_mem_free(p->p_mem);

    aw_kprintf("resource is released by %s\n", __g_str);
}

static void* __task1_func (void *parg)
{
    int i = 0;

    /* 初始化一个引用计数对象,初始值为1 */
    aw_refcnt_init(&test_ref.ref);

    test_ref.p_mem = aw_mem_alloc(MEM_SIZE);
    if (NULL == test_ref.p_mem) {
        AW_ERRF(("aw_mem_alloc err: %d\n", -AW_ENOMEM));
        return 0;
    }

    AW_MUTEX_LOCK(__g_data_mutex, AW_SEM_WAIT_FOREVER);

    for (i = 0; i < MEM_SIZE; i++) {
        test_ref.p_mem[i] = (char)i;
    }

    AW_MUTEX_UNLOCK(__g_data_mutex);

    /* 延时，等待任务2启动 */
    aw_mdelay(3000);

    memcpy(__g_str, "task1", 5);

    /* 引用计数减一，若计数值无效(0)，则调用回调函数 */
    aw_refcnt_put(&test_ref.ref, __release_callback);

    /* 查询引用计数状态，此时状态为有效 */
    AW_INFOF(("__task1_func end, count is %s\n",
             aw_refcnt_valid(&test_ref.ref) ? "valid" : "invalid"));

    while (1) {
        aw_mdelay(500);
    }

    return 0;
}

static void* __task2_func (void *parg)
{
    int i = 0;

    /* 引用计数加一 */
    aw_refcnt_get(&test_ref.ref);

    AW_MUTEX_LOCK(__g_data_mutex, AW_SEM_WAIT_FOREVER);

    /* 使用动态内存 */
    for (i = 0; i < MEM_SIZE; i++) {
        aw_kprintf("test_ref.p_mem[%d] = %d\r\n", i, test_ref.p_mem[i]);
    }

    AW_MUTEX_UNLOCK(__g_data_mutex);

    memcpy(__g_str, "task2", 5);

    /* 引用计数减一，若计数值无效(0)，则调用回调函数 */
    aw_refcnt_put(&test_ref.ref, __release_callback);

    /* 查询引用计数状态，此时状态为有效 */
    AW_INFOF(("__task2_func end, count is %s\n",
             aw_refcnt_valid(&test_ref.ref) ? "valid" : "invalid"));

    while (1) {
        aw_mdelay(500);
    }

    return 0;

}

/**
 * \brief 引用计数演示例程入口
 * \return 无
 */
void demo_refcnt_entry (void)
{
    /* 定义任务实体，分配栈空间大小为1024 */
    AW_TASK_DECL_STATIC(task1, 1024);

    /* 定义任务实体，分配栈空间大小为1024 */
    AW_TASK_DECL_STATIC(task2, 1024);

    AW_TASK_INIT(task1,                 /* 定义的任务实体 */
                 "task1_refcnt",        /* 任务名 */
                 5,                     /* 任务优先级 */
                 1024,                  /* 任务栈大小 */
                 __task1_func,          /* 任务的入口函数 */
                 (void *)NULL);         /* 传递给任务的参数 */

    AW_TASK_INIT(task2,                 /* 定义的任务实体 */
                 "task2_refcnt",        /* 任务名 */
                 8,                     /* 任务优先级 */
                 1024,                  /* 任务栈大小 */
                 __task2_func,          /* 任务的入口函数 */
                 (void *)NULL);         /* 传递给任务的参数 */

    AW_TASK_STARTUP(task1);     /* 启动任务1 */

    AW_TASK_STARTUP(task2);     /* 启动任务2 */

    return ;
}

/** [src_refcnt] */

/* end of file */
