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
 * \file  rtk_base_mutex.c
 * \brief 实现rtk_base_mutex的内部操作函数
 * \internal
 * \par modification history:
 * - 1.00 18-05-15  sni, first implement
 * \endinternal
 */

#include "rtk.h"
#include "rtk_mem.h"
#include "rtk_arch.h"
#include "rtk_task_internal.h"
#include "rtk_priority_q_internal.h"
#include "aw_errno.h"
#include "aw_common.h"
#include <string.h> /* for memset() and memcpy() */
#include "rtk_base_mutex.h"


void rtk_base_mutex_init(rtk_base_mutex_t *p_base_mutex)
{
    /* 预防在中断中调用 */
    if (rtk_is_int_context() ) {
        while(1);
    }

    rtk_list_node_init( &p_base_mutex->base_mutex_hold_node );
    p_base_mutex->mutex_recurse_count  = 0;
    p_base_mutex->owner = NULL;
}

static void  __base_mutex_owner_set_ec(rtk_base_mutex_t *p_base_mutex, 
                                       struct rtk_task  *task )
{
    p_base_mutex->owner = task;
    rtk_list_add_tail(
            &p_base_mutex->base_mutex_hold_node,
            &task->base_mutex_holded_head );
}

static void  __base_mutex_owner_unset_ec(
        rtk_base_mutex_t *p_base_mutex )
{
    p_base_mutex->owner = NULL;
    rtk_list_del_init( &p_base_mutex->base_mutex_hold_node );
}




void rtk_base_mutex_lock(rtk_base_mutex_t *p_base_mutex)
{
    RTK_CRITICAL_STATUS_DECL(old);
    struct rtk_task    *task_current = rtk_task_self();
    struct rtk_task    *p_owner;

#if KERNEL_ARG_CHECK_EN
    if ( unlikely(rtk_is_int_context()) ) {
        while(1);
    }
#endif
retry:
    RTK_ENTER_CRITICAL(old);
    if ( NULL == p_base_mutex->owner ) {
        __base_mutex_owner_set_ec(p_base_mutex,task_current);
        p_base_mutex ->mutex_recurse_count = 1;

        RTK_EXIT_CRITICAL(old );
        return ;
    } else if ( p_base_mutex->owner == task_current ) {
        p_base_mutex->mutex_recurse_count++;
        RTK_EXIT_CRITICAL(old );
        return ;
    }
    /* 如果不能获取，则需要提升owner优先级*/
    p_owner = p_base_mutex->owner;
#if KERNEL_ARG_CHECK_EN
    while(TASK_READY != p_owner->status);
#endif
    if (task_current->readq_prio < p_owner->readq_prio) {
        p_owner->base_mutex_need_prio = task_current->readq_prio;
        _task_ready_prio_change_ec(p_owner,task_current->readq_prio);
    }

    RTK_EXIT_CRITICAL(old);
    rtk_task_yield();
    goto retry;
}


int rtk_base_mutex_unlock_no_sche(rtk_base_mutex_t *p_base_mutex)
{
    RTK_CRITICAL_STATUS_DECL(old);
    struct rtk_task    *task_current = rtk_task_self();
    int                 need_schedule = 0;


#ifndef KERNEL_NO_ARG_CHECK
    if ( unlikely(rtk_is_int_context()) ) {
        while(1);
    }
#endif
    RTK_ENTER_CRITICAL(old);
    if ( p_base_mutex-> owner != task_current ) {
        while(1);
    }

    /* 如果锁还在当前任务 */
    if ( --p_base_mutex->mutex_recurse_count ) {
        RTK_EXIT_CRITICAL(old );
        return need_schedule;
    }
    __base_mutex_owner_unset_ec( p_base_mutex);
    if (task_current->base_mutex_need_prio < RTK_MAX_PRIORITY ) {
        /* 进行过优先级提升 */
        /* 如果已经释放了所有base mutex */
        if (likely(rtk_list_empty(&task_current->base_mutex_holded_head ) ) ) {
            task_current->base_mutex_need_prio = RTK_MAX_PRIORITY;
            if (task_current->readq_prio != task_current->waitq_prio) {
                _task_ready_prio_change_ec(task_current,task_current->waitq_prio);
            }
            need_schedule = 1;
        }

    }
    RTK_EXIT_CRITICAL(old );

    return need_schedule;
}

void rtk_base_mutex_unlock_not_ready_ec(rtk_base_mutex_t *p_base_mutex)
{

    struct rtk_task    *task_current = rtk_task_self();


#ifndef KERNEL_NO_ARG_CHECK
    if ( unlikely(rtk_is_int_context()) ) {
        while(1);
    }
#endif

    if ( p_base_mutex-> owner != task_current ) {
        while(1);
    }
    /* 如果锁还在当前任务 */
    if ( --p_base_mutex->mutex_recurse_count ) {
        return ;
    }
    __base_mutex_owner_unset_ec( p_base_mutex);
    if (task_current->base_mutex_need_prio < RTK_MAX_PRIORITY ) {
        /* 进行过优先级提升 */
        /* 如果已经释放了所有base mutex */
        if (likely(rtk_list_empty(&task_current->base_mutex_holded_head ) ) ) {
            task_current->base_mutex_need_prio = RTK_MAX_PRIORITY;
            task_current->readq_prio = task_current->waitq_prio;
        }

    }

}

void rtk_base_mutex_unlock(rtk_base_mutex_t *p_base_mutex)
{
    int                     need_schedule = 0;

    need_schedule = rtk_base_mutex_unlock_no_sche(p_base_mutex);
    if (need_schedule ) {
        rtk_schedule();
    }
}


