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
 * \brief implementation of _errno().
 *
 * \internal
 * \par modification history:
 * - 1.00 14-10-29  may, first implementation.
 * \endinternal
 */

#include "aw_common.h"
#include "aw_mem.h"
#include "aw_psp_errno.h"
#include "rtk.h"
#include "aw_task.h"


/******************************************************************************/
aw_task_id_t aw_task_create (const char     *name,
                             int             priority,
                             size_t          stack_size,
                             aw_task_func_t  func,
                             void           *arg)
{
    aw_psp_task_id_t        id;
    id = rtk_task_create(name,(priority + __HIGH_THAN_USR_PRIORITY_COUNT), 0,stack_size,func,(arg),0);

     return id;
}

aw_err_t aw_task_delete (aw_task_id_t task_id)
{
    rtk_task_terminate( task_id );
    return 0;
}

aw_err_t aw_task_suspend(aw_task_id_t task_id)
{
    return rtk_task_freeze(task_id,1);
}

aw_err_t aw_task_resume(aw_task_id_t task_id)
{
    return rtk_task_unfreeze(task_id);
}

aw_err_t aw_task_safe (void)
{
    return rtk_task_safe();
}

aw_err_t aw_task_unsafe (void)
{
    return rtk_task_unsafe();
}

aw_err_t aw_task_exit(void *status)
{
    return rtk_task_exit(status);
}

/**
 * \brief 获取系统优先级数目
 * \return  优先级数目
 */
unsigned int aw_task_sys_priority_count (void)
{
    return rtk_get_sys_priority_count();
}

/**
 * \brief 获取实时优先级数目
 * \return  优先级数目
 */
unsigned int aw_task_realtime_priority_count (void)
{
    return rtk_get_realtime_priority_count();
}

/**
 * \brief 获取系统低优先级数目
 * \return  优先级数目
 */
unsigned int aw_task_sys_low_priority_count (void)
{
    return rtk_get_sys_low_priority_count();
}

/* end of file */
