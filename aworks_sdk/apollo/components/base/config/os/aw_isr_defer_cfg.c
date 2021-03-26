/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief isr deferal job library configure file
 *
 * \internal
 * \par modification history:
 * - 1.00 14-03-12  zen, first implementation
 * \endinternal
 */

#include "apollo.h"
#include "aw_defer.h"
#include "aw_isr_defer.h"

/** \brief 定义defer对象 */
struct aw_defer  g_sys_isr_defer;

#ifndef AW_CFG_ISR_DEFER_TASK_PRIO
#define AW_CFG_ISR_DEFER_TASK_PRIO  0
#endif

#ifndef AW_CFG_ISR_DEFER_TASK_CNT
#define AW_CFG_ISR_DEFER_TASK_CNT   2
#endif

#ifndef AW_CFG_ISR_DEFER_TASK_STACK_SIZE
#define AW_CFG_ISR_DEFER_TASK_STACK_SIZE  (4096)
#endif

/**
 *  \brief isr deferral 库初始化
 */
void aw_isr_defer_lib_init (void)
{
    /* 初始化一个延迟作业*/
    aw_defer_init(&g_sys_isr_defer);

    /* 启动延迟作业*/
    AW_DEFER_START(&g_sys_isr_defer,
                   AW_CFG_ISR_DEFER_TASK_CNT,
                   AW_CFG_ISR_DEFER_TASK_PRIO,
                   AW_CFG_ISR_DEFER_TASK_STACK_SIZE,
                   "isr_defer");
}

/* end of file */

