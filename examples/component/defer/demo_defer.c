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
 * \brief defer延迟作业组件例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能AW_COM_CONSOLE和调试串口对应的宏；
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象：
 *   1. 串口打印“job done, job data: 0x12345678”
 *
 * - 备注：
 *   1. 若用户有一些函数需要被异步调用，并且想提交到一些公共的线程去执行，那么 defer 就很适用了
 *      只需定义 defer 对象并启动，然后将工作节点 commit 到该 defer 中即可
 *   2. 当然，AWorks 本身会创建一些系统 defer 去处理，参考 "aw_sys_defer.h"
 *
 * \par 源代码
 * \snippet demo_defer.c src_defer
 *
 * \internal
 * \par Modification History
 * - 1.00 17-09-06  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_defer defer延迟作业组件例程
 * \copydoc demo_defer.c
 */

/** [src_defer] */
#include "aworks.h"
#include "aw_defer.h"
#include "aw_vdebug.h"
#include "aw_sem.h"


aw_local struct aw_defer        __g_defer;  /* 定义defer对象 */
aw_local struct aw_defer_djob   __g_djob;   /* 定义工作节点 */
AW_SEMB_DECL_STATIC(__g_sem);

aw_local void __job_call (void *p_arg)
{
    AW_INFOF(("\njob done, job data: 0x%08x\n", (uint32_t)p_arg));
    AW_SEMB_GIVE(__g_sem);
}

/**
 * \brief defer组件例程入口
 * \return 无
 */
void demo_defer_entry (void)
{
    aw_err_t  ret = AW_OK;

    /* 初始化一个延迟作业 */
    aw_defer_init(&__g_defer);

    /* 启动延迟作业（创建3个优先级为0，堆栈大小为4096的线程，并启动） */
    AW_DEFER_START(&__g_defer, 3, 0, 4096, "demo_defer");

    /* 初始化一个工作节点 */
    aw_defer_job_init(&__g_djob, __job_call, (void *)0x12345678);

    /* 初始化信号量 */
    AW_SEMB_INIT(__g_sem, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);

    /* 将一个工作节点添加至延迟作业处理 */
    ret = aw_defer_commit(&__g_defer, &__g_djob);
    if (ret != AW_OK) {
        AW_ERRF(("aw_defer_commit err: %d\n", ret));
    }

    /* 等待当前工作已完成 */
    AW_SEMB_TAKE(__g_sem, AW_SEM_WAIT_FOREVER);

    /* 判断当前工作节点是否可用 */
    if (aw_defer_job_is_usable(&__g_defer, &__g_djob)) {
        AW_INFOF(("curr job can usable\n"));
    }
}

/** [src_defer] */
 
/* end of file */
