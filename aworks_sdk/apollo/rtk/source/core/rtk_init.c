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
 * \file  rtk_systick.c
 * \brief rtk 整体初始化相关函数
 *
 * \internal
 * \par modification history:
 * 1.00 18-04-24  sni, first implement
 * \endinternal
 */
#include "rtk.h"
#include "rtk_mem.h"
#include "rtk_arch.h"
#include "rtk_task_internal.h"
#include "rtk_priority_q_internal.h"
#include "rtk_tick_down_counter_internal.h"
#include "rtk_sem_internal.h"
#include "rtk_idle.h"
#include "rtk_mutex_internal.h"
#include "aw_errno.h"
#include "aw_common.h"
#include <string.h>
#include "os_trace_events.h"


void rtk_init( void )
{
    rtk_arch_init();
    rtk_mem_init();
    __rtk_task_global_init();
    priority_q_init();
    __rtk_semc_global_init();
    __rtk_semb_global_init();
    __rtk_mutex_global_init();

    __rtk_init_idle_task();
    __rtk_start_idle_task();
    __rtk_tick_down_counter_global_init();
    TRACE_Start();
}

void rtk_startup( void )
{
    rtk_arch_start_first_thread();
}
