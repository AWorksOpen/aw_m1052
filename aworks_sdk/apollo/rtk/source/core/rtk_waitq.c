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
 * \file  rtk_wait_queue.c
 * \brief 实现rtk_wait_queue的操作函数
 *
 * \internal
 * \par modification history:
 * - 1.00 18-05-24  sni, first implement
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
#include "rtk_waitq_internal.h"
#include "rtk_tick_down_counter_internal.h"
#include "rtk_task_internal.h"


#if CONFIG_RTK_WAIT_QUEUE_PEND_CONST_TIME
#else

void __rtk_waitq_init(
        rtk_wait_queue_t *p_wait_queue,
        rtk_wait_queue_type_t type,
        rtk_wait_queue_option_t option)
{
    rtk_list_node_init( &p_wait_queue->pending_tasks );
    p_wait_queue->type = type;
}

int __rtk_waitq_terminate( rtk_wait_queue_t *p_wait_queue )
{
    RTK_CRITICAL_STATUS_DECL(old);
    int                 happen = 0;
    struct rtk_task    *task;

    /* terminate因为不是在中断中调用,且现在 */
    /* 也不会发起新的对p_wait_queue的操作了，所以可以只在唤醒1个任务时 */
    /* 禁用中断，以降低中断延迟时间 */
    p_wait_queue->type = RTK_WAIT_QUEUE_TYPE_NULL;
    do {
        happen = 0;
        RTK_ENTER_CRITICAL(old);
        task = __rtk_waitq_pop_1_pender_ec(p_wait_queue);
        if (NULL != task) {
            __task_wakup_with_err_ec(task,RTK_TASK_WAKE_BY_TERMINATE);
            happen = 1;
        }
        RTK_EXIT_CRITICAL(old);
        if ( happen ) {
            rtk_schedule();
        }
    }while(happen);

    return 0;
}

int __rtk_waitq_highest_pri_get_lc (
        rtk_wait_queue_t *p_wait_queue )
{
    struct rtk_task *task;

    if ( !rtk_list_empty(&p_wait_queue->pending_tasks) ) {
        task = PEND_NODE_TO_PTCB( RTK_LIST_FIRST(&p_wait_queue->pending_tasks) );
        return task->waitq_prio;
    }
    return RTK_MAX_PRIORITY+1;
}

void __rtk_waitq_add_pender_ec(
        struct rtk_task  *task,
        rtk_wait_queue_t *p_wait_queue)
{
    struct rtk_task    *task_i;
    rtk_list_node_t    *p;

    rtk_list_for_each( p, &p_wait_queue->pending_tasks) {
        task_i = PEND_NODE_TO_PTCB( p );
        if ( task_i->waitq_prio > task->waitq_prio ) {
            break;
        }
    }

    rtk_list_add_tail( &task->wait_queue_node, p );
    task->pending_resource = p_wait_queue;
}


void __task_detach_pending_waitq_ec( struct rtk_task *task )
{
    rtk_list_del_init( &task->wait_queue_node );
    task->pending_resource = NULL;
}

int __rtk_waitq_resort_pender_ec(
        rtk_wait_queue_t *p_wait_queue,
        struct rtk_task  *task)
{
    int pri;

    __task_detach_pending_waitq_ec( task );
    pri = __rtk_waitq_highest_pri_get_lc(p_wait_queue);
    __rtk_waitq_add_pender_ec( task,p_wait_queue );
    return pri > __rtk_waitq_highest_pri_get_lc(p_wait_queue);
}

struct rtk_task *__rtk_waitq_pop_1_pender_ec(rtk_wait_queue_t *p_wait_queue)
{
    register rtk_list_node_t       *p;
    register struct rtk_task       *taskwakeup;

    if (rtk_list_empty(&p_wait_queue->pending_tasks) ) {
        /* 整个wait_queue已经没有pender了 */
        return NULL;
    }

    p = RTK_LIST_FIRST(&p_wait_queue->pending_tasks);
    taskwakeup = PEND_NODE_TO_PTCB( p );

    __task_detach_pending_waitq_ec(taskwakeup);

    return taskwakeup;
}

struct rtk_task *__rtk_waitq_highest_pender_get_lc(
        rtk_wait_queue_t *p_wait_queue)
{
    rtk_list_node_t            *p;
    struct rtk_task            *task;

    if (rtk_list_empty(&p_wait_queue->pending_tasks) ) {
        /* 整个wait_queue已经没有pender了 */
        return NULL;
    }

    p = RTK_LIST_FIRST(&p_wait_queue->pending_tasks);
    task = PEND_NODE_TO_PTCB( p );
    return task;
}

int __rtk_waitq_has_penders_ec(
        rtk_wait_queue_t *p_wait_queue )
{
    return !rtk_list_empty(&p_wait_queue->pending_tasks);
}

#endif



