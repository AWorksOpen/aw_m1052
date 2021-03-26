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
 * \file  rtk_priority_config.c
 * \brief 
 *
 * \internal
 * \par modification history:
 * - 1.00 18-05-15  sni, first implement
 * \endinternal
 */
 
#include "rtk.h"
#include "rtk_mem.h"
#include "aw_errno.h"
#include "aw_common.h"
#include <string.h>
#include "rtk_arch.h"
#include "rtk_task_internal.h"
#include "rtk_priority_q_internal.h"
#include "os_trace_events.h"

unsigned int rtk_max_priority( void )
{
    return CONFIG_RTK_REALTIME_PRIORITY_COUNT +
            CONFIG_RTK_SYS_PRIORITY_COUNT +
            CONFIG_RTK_USR_PRIORITY_COUNT +
            CONFIG_RTK_SYS_LOW_PRIORITY_COUNT ;
}


#define __RTK_TOTAL_PRIO_COUNT \
            (CONFIG_RTK_REALTIME_PRIORITY_COUNT + \
                    CONFIG_RTK_SYS_PRIORITY_COUNT + \
                    CONFIG_RTK_USR_PRIORITY_COUNT + \
                    CONFIG_RTK_SYS_LOW_PRIORITY_COUNT + 1 )



#define RTK_MAX_PRIORITY_DEF(_max_priority)                                 \
    static uint32_t         __rtk_prio_bitmaps[((_max_priority)/32) + 1]; \
    static rtk_list_node_t  __rtk_prio_task_lists[_max_priority];    \
    rtk_ready_q_t _g_rtk_readyq = {0};          \


RTK_MAX_PRIORITY_DEF(RTK_TOTAL_PRIORITY_COUNT);

rtk_readyq_info_t aw_const __g_rtk_readyq_info = {
        __rtk_prio_task_lists,
        __rtk_prio_bitmaps};

