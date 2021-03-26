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
 * \file  rtk_wait_queue_const_time.c
 * \brief 实现rtk_wait_queue的操作函数，本文件中的所有实现耗费的时间都是O(1)
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
#include "os_trace_events.h"

#if CONFIG_RTK_WAIT_QUEUE_PEND_CONST_TIME

void __rtk_waitq_init(
        rtk_wait_queue_t *p_wait_queue,
        rtk_wait_queue_type_t type,
        rtk_wait_queue_option_t option)
{
    int                 i;

    for (i = 0;i<RTK_TOTAL_PRIORITY_COUNT;i++) {
        rtk_list_node_init( &p_wait_queue->pending_tasks[i] );
    }

    p_wait_queue->bitmap_group = 0;
    memset(p_wait_queue->bitmap_mask,0,sizeof(p_wait_queue->bitmap_mask) );
    p_wait_queue->highest_pending_prio = RTK_TOTAL_PRIORITY_COUNT + 1;
    p_wait_queue->type = type;
}

int __rtk_waitq_terminate( rtk_wait_queue_t *p_waitq )
{
    RTK_CRITICAL_STATUS_DECL(old);
    int                 happen = 0;
    struct rtk_task    *task;

    /* terminate因为不是在中断中调用,且现在 */
    /* 也不会发起新的对p_wait_queue的操作了，所以可以只在唤醒1个任务时 */
    /* 禁用中断，以降低中断延迟时间 */
    p_waitq->type = RTK_WAIT_QUEUE_TYPE_NULL;
    do {
        happen = 0;
        RTK_ENTER_CRITICAL(old);
        task = __rtk_waitq_pop_1_pender_ec(p_waitq);
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
    return p_wait_queue->highest_pending_prio;
}

void __rtk_waitq_add_pender_ec(
        struct rtk_task *task ,
        rtk_wait_queue_t *p_wait_queue )
{
    rtk_list_node_t        *p;
    int                     cur_prio;
    int                     cur_group;
    int                     cur_bit;

    cur_prio = task->waitq_prio;
    p = &p_wait_queue->pending_tasks[cur_prio];

    rtk_list_add_tail( &task->wait_queue_node, p );


    if (task->waitq_prio < p_wait_queue->highest_pending_prio) {
        p_wait_queue->highest_pending_prio = task->waitq_prio;
    }
    cur_group = cur_prio / (sizeof(uint32_t) *8);
    cur_bit = cur_prio % (sizeof(uint32_t) *8);

    p_wait_queue->bitmap_mask[cur_group] |= (1 << cur_bit);
    p_wait_queue->bitmap_group |= (1<< cur_group);

    task->pending_resource = p_wait_queue;
}

/*
void __rtk_wait_queue_add_pending_task_fifo_isr_locked(
        rtk_wait_queue_t *p_wait_queue,
        struct rtk_task *task )
{
    list_add_tail( &task->wait_queue_node, &p_wait_queue->pending_tasks );
    task->pending_resource = p_wait_queue;
}
*/
void __task_detach_pending_waitq_ec( struct rtk_task *task)
{
    int                     cur_prio;
    int                     cur_group;
    int                     cur_bit;
    rtk_wait_queue_t       *p_waitq;
    rtk_list_node_t        *p_next;

    if (!rtk_list_empty(&task->wait_queue_node) ) {
        p_next = RTK_LIST_FIRST(&task->wait_queue_node);
        rtk_list_del_init( &task->wait_queue_node);
        p_waitq = task->pending_resource;
        task->pending_resource = NULL;

        /* 如果当前优先级节点变空了 */
        if (rtk_list_empty (p_next) ) {
            cur_prio = p_next-p_waitq->pending_tasks;
            cur_group = cur_prio / (sizeof(uint32_t) *8);
            cur_bit = cur_prio % (sizeof(uint32_t) *8);

            p_waitq->bitmap_mask[cur_group] &= (~(1<< cur_bit));
            if ( unlikely(0 == p_waitq->bitmap_mask[ cur_group  ]) ) {
                p_waitq->bitmap_group &= ~(1 << cur_group);
            }

            if (cur_prio <= p_waitq->highest_pending_prio ) {

                /* 查找此时优先级最大的节点 */
                cur_group = rtk_cpu_ffs(p_waitq->bitmap_group) - 1;
                if ( cur_group >= 32 ) {
                    p_waitq->highest_pending_prio =
                            RTK_TOTAL_PRIORITY_COUNT + 1;
                }
                else {
                    cur_bit = rtk_cpu_ffs(p_waitq->bitmap_mask[cur_group] ) - 1;
                    cur_prio = sizeof(uint32_t) * 8 * cur_group + cur_bit;
                    p_waitq->highest_pending_prio = cur_prio;
                }
            }
        }
    }

}

int __rtk_waitq_resort_pender_ec(
        rtk_wait_queue_t *p_wait_queue,
        struct rtk_task  *task)
{
    int             prio;

    __task_detach_pending_waitq_ec( task );
    prio = __rtk_waitq_highest_pri_get_lc(p_wait_queue);
    __rtk_waitq_add_pender_ec(task,p_wait_queue);
    return prio > __rtk_waitq_highest_pri_get_lc(p_wait_queue);
}

struct rtk_task *__rtk_waitq_pop_1_pender_ec(rtk_wait_queue_t *p_waitq)
{
    register rtk_list_node_t       *p;
    register struct rtk_task       *taskwakeup;

    if ( p_waitq->highest_pending_prio >= RTK_TOTAL_PRIORITY_COUNT ) {
        /* 整个wait_queue已经没有pender了 */
        return NULL;
    }

    p = RTK_LIST_FIRST(&p_waitq->pending_tasks[p_waitq->highest_pending_prio] );
    taskwakeup = PEND_NODE_TO_PTCB( p );

    __task_detach_pending_waitq_ec( taskwakeup );

    return taskwakeup;
}

struct rtk_task *__rtk_waitq_highest_pender_get_lc(
        rtk_wait_queue_t *p_waitq)
{
    rtk_list_node_t            *p;
    struct rtk_task            *task;

    if ( p_waitq->highest_pending_prio >= RTK_TOTAL_PRIORITY_COUNT ) {
        /* 整个wait_queue已经没有pender了 */
        return NULL;
    }

    p = RTK_LIST_FIRST(&p_waitq->pending_tasks[p_waitq->highest_pending_prio]);
    task = PEND_NODE_TO_PTCB( p );
    return task;
}

int __rtk_waitq_has_penders_ec(
        rtk_wait_queue_t *p_waitq )
{
    return !(p_waitq->highest_pending_prio >= RTK_TOTAL_PRIORITY_COUNT);
}


#endif



