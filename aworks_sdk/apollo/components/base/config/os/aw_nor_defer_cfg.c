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
 * \brief Normal deferal job library configure file
 *
 * \internal
 * \par modification history:
 * - 1.00 17-03-31  may, first implementation
 * \endinternal
 */
#include "aw_defer.h"
#include "aw_task.h"

/** \brief 定义defer对象 */
struct aw_defer g_sys_nor_defer;

#ifndef AW_CFG_NOR_DEFER_TASK_PRIO
#define AW_CFG_NOR_DEFER_TASK_PRIO  7
#endif

#ifndef AW_CFG_NOR_DEFER_TASK_CNT
#define AW_CFG_NOR_DEFER_TASK_CNT   2
#endif

#ifndef AW_CFG_NOR_DEFER_TASK_STACK_SIZE
#define AW_CFG_NOR_DEFER_TASK_STACK_SIZE  (4096)
#endif

/**
 *  \brief normal deferral 库初始化
 */
void aw_nor_defer_lib_init (void)
{
    /* 初始化一个延迟作业*/
    aw_defer_init(&g_sys_nor_defer);

    /* 启动延迟作业*/
    AW_DEFER_START(&g_sys_nor_defer,
                   AW_CFG_NOR_DEFER_TASK_CNT,
                   AW_CFG_NOR_DEFER_TASK_PRIO,
                   AW_CFG_NOR_DEFER_TASK_STACK_SIZE,
                   "nor_defer");
}
