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
 * \file rtk_mutex.c
 * \brief rtk mutex实现
 *
 * \par 源代码
 * \internal
 * \par Modification history
 * - 1.00 18-06-13 sni, first version.
 * \endinternal
 */
#include "rtk.h"
#include "rtk_mem.h"
#include "rtk_arch.h"
#include "rtk_task_internal.h"
#include "aw_errno.h"
#include "aw_common.h"
#include <string.h> /* for memset() and memcpy() */
#include "rtk_mutex_internal.h"
#include "rtk_base_mutex.h"
#include "rtk_mutex_sync_lock.h"
#include "os_trace_events.h"


static rtk_base_mutex_t     __task_mutex_sync_lock;

void __rtk_task_mutex_sync_lock_init(void)
{
    rtk_base_mutex_init(&__task_mutex_sync_lock);
}

void __rtk_task_mutex_sync_lock(void)
{
    rtk_base_mutex_lock(&__task_mutex_sync_lock);
}

void __rtk_task_mutex_sync_unlock(void)
{
    rtk_base_mutex_unlock(&__task_mutex_sync_lock);
}

void __rtk_task_mutex_sync_unlock_no_sche(void)
{
    rtk_base_mutex_unlock_not_ready_ec(&__task_mutex_sync_lock);
}

