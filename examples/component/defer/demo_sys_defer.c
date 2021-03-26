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
 * \brief sys_defer 组件例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能AW_COM_CONSOLE和调试串口对应的宏；
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象：
 *   1. 串口先打印“job done, job data: 0xabcdef00”再打印
 *      “job done, job data: 0x12345678”
 *
 * - 备注：
 *   1. 若用户有一些函数需要被异步调用，并且想提交到一些公共的线程去执行，那么 defer 就很适用了
 *      只需定义 defer 对象并启动，然后将工作节点 commit 到该 defer 中即可
 *   2. 当然，AWorks 本身会创建一些系统 defer 去处理，包括 nor_defer 和 isr_defer，
 *      ISR defer： 一般用于处理紧急工作
 *      Normal defer： 一般用于处理非紧工作
 *
 * \par 源代码
 * \snippet demo_sys_defer.c src_sys_defer
 *
 * \internal
 * \par Modification History
 * - 1.00 17-09-06  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_sys_defer 系统延迟作业组件例程
 * \copydoc demo_sys_defer.c
 */

/** [src_sys_defer] */
#include "aworks.h"
#include "aw_sys_defer.h"
#include "aw_vdebug.h"
#include "aw_sem.h"

aw_local struct aw_defer_djob   __g_djob_nor;
aw_local struct aw_defer_djob   __g_djob_isr;

AW_SEMC_DECL_STATIC(__g_sem);

aw_local void __job_call (void *p_arg)
{
    AW_INFOF(("job done, job data: 0x%08x\n", (uint32_t)p_arg));
    AW_SEMC_GIVE(__g_sem);
}

/**
 * \brief sys_defer组件例程入口
 * \return 无
 */
void demo_sys_defer_entry (void)
{
    aw_err_t  ret = AW_OK;

    /* 初始化信号量 */
    AW_SEMC_INIT(__g_sem, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);

    /* 初始化工作节点 */
    aw_defer_job_init(&__g_djob_nor, __job_call, (void *)0x12345678);
    aw_defer_job_init(&__g_djob_isr, __job_call, (void *)0xabcdef00);

    /* 将一个工作节点添加至系统普通延迟作业中处理 */
    ret = aw_sys_defer_commit(AW_SYS_DEFER_TYPE_NOR, &__g_djob_nor);
    if (ret != AW_OK) {
        AW_ERRF(("aw_sys_defer_commit err: %d\n", ret));
    }

    /* 判断当前工作节点是否已经使用 */
    if (aw_sys_defer_job_is_usable(AW_SYS_DEFER_TYPE_NOR, &__g_djob_nor)) {
        AW_INFOF(("nor job is used\n"));
    }

    /* 将一个工作节点添加至系统紧急延迟作业中处理 */
    ret = aw_sys_defer_commit(AW_SYS_DEFER_TYPE_ISR, &__g_djob_isr);
    if (ret != AW_OK) {
        AW_ERRF(("aw_sys_defer_commit err: %d\n", ret));
    }

    /* 判断当前工作节点是否已经使用 */
    if (aw_sys_defer_job_is_usable(AW_SYS_DEFER_TYPE_ISR, &__g_djob_isr)) {
        AW_INFOF(("isr job is used\n"));
    }

    /* 等待当前工作已完成 */
    AW_SEMC_TAKE(__g_sem, AW_SEM_WAIT_FOREVER);
    AW_SEMC_TAKE(__g_sem, AW_SEM_WAIT_FOREVER);
}

/** [src_sys_defer] */

/* end of file */
