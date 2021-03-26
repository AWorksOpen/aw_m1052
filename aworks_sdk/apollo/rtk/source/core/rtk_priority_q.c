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
 * \file  rtk_priority_q.c
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
#include "rtk_systick.h"

extern rtk_ready_q_t _g_rtk_readyq;
extern rtk_readyq_info_t aw_const __g_rtk_readyq_info;

#define the_readyq()                    (&_g_rtk_readyq)
#define the_readyq_info()               (&__g_rtk_readyq_info)

void priority_q_init( void )
{
    rtk_ready_q_t              *p_readyq = the_readyq();
    p_rtk_readyq_const_info_t   p_readyq_info = the_readyq_info();
    unsigned int                m = RTK_TOTAL_PRIORITY_COUNT;
    unsigned int                i;
    int                         n;

    p_readyq->p_highest_task = NULL;
    p_readyq->bitmap_group = 0;

    n = m / 32 + 1;
    for (i = 0; i<n; i++) {
        p_readyq_info->prio_bitmaps[i] = 0;
    }

    for (i = 0; i< m; i++) {
        rtk_list_node_init( &p_readyq_info->prio_tasks_lists[i] );
    }
}

void _task_ready_prio_change_ec(
        struct rtk_task *p_task,
        unsigned int new_readyq_prio)
{
    READY_Q_REMOVE(p_task);
    p_task->readq_prio = new_readyq_prio;
    READY_Q_PUT(p_task);
}

static aw_inline void priority_q_bitmap_set (
        rtk_ready_q_t *p_readyq,
        p_rtk_readyq_const_info_t p_readyq_info,
        int priority )
{
    register int grp = priority>>5;

    p_readyq_info->prio_bitmaps[ grp ] |= 1 << ( 0x1f & priority);
    p_readyq->bitmap_group |= 1 << grp;
}


static aw_inline struct rtk_task *priority_q_highest_get(
        rtk_ready_q_t *p_readyq,
        p_rtk_readyq_const_info_t p_readyq_info
        )
{
    int      index;
    int      i;

    i = rtk_cpu_ffs( p_readyq->bitmap_group ) - 1;
    index = rtk_cpu_ffs( p_readyq_info->prio_bitmaps[i] ) + (i << 5) - 1;
    return RTK_LIST_ENTRY(
            RTK_LIST_FIRST(&p_readyq_info->prio_tasks_lists[ index ] ),
            struct rtk_task,task_prio_q_node);
}

void _rtk_readyq_put_idle_task(struct rtk_task *p_task )
{
    /* set highest node */
    /* 预防p_highest_task值为NULL */
    the_readyq()->p_highest_task = p_task;
    _rtk_readyq_put(p_task);

}

void _rtk_readyq_put(struct rtk_task *p_task)
{
    register rtk_ready_q_t             *p_readyq = the_readyq();
    register p_rtk_readyq_const_info_t  p_readyq_info = the_readyq_info();
    register rtk_list_node_t           *p_prio_q_node;
    register unsigned int               readyq_prio;

    readyq_prio = p_task->readq_prio;
    p_prio_q_node = &p_task->task_prio_q_node;
    priority_q_bitmap_set( p_readyq,p_readyq_info, readyq_prio );
    rtk_list_add_tail( p_prio_q_node, &p_readyq_info->prio_tasks_lists[readyq_prio] );

    /* set highest node */
    if ( (readyq_prio < p_readyq->p_highest_task->readq_prio) ) {
        p_readyq->p_highest_task = p_task;
    }
}

int _rtk_readyq_remove( struct rtk_task *p_task )
{
    register p_rtk_readyq_const_info_t  p_readyq_info = the_readyq_info();
    register rtk_ready_q_t             *p_readyq = the_readyq();
    int                                 readyq_prio;
    int                                 group;

    p_task->last_run_tick_valid = 0;
    readyq_prio = p_task->readq_prio;
    rtk_list_del_init( &p_task->task_prio_q_node );
    if ( rtk_list_empty( &p_readyq_info->prio_tasks_lists[readyq_prio] ) ) {
        group = readyq_prio>>5;
        p_readyq_info->prio_bitmaps[ group  ] &= ~(1 << ( 0x1f & readyq_prio));
        if ( unlikely(0 == p_readyq_info->prio_bitmaps[ group  ]) ) {
            p_readyq->bitmap_group &= ~(1 << group);
        }
    }
    if ( p_readyq->p_highest_task == p_task  ) {
        p_readyq->p_highest_task = priority_q_highest_get( p_readyq,p_readyq_info );
    }
    return 0;
}

uint32_t * __rtk_highest_tcb_sp_addr_ec(void)
{
    struct rtk_task    *task;
    task =  the_readyq()->p_highest_task;
    return  (uint32_t *)&task->sp;
}

struct rtk_task *_rtk_highest_task_get_ec(void)
{
    return the_readyq()->p_highest_task;
}

struct rtk_task *_rtk_highest_task_loop_get_ec(void)
{
    int                                 cur_readyq_prio;
    p_rtk_readyq_const_info_t           p_readyq_info;
    rtk_ready_q_t                      *p_readyq;
    register rtk_list_node_t           *p_first_prio_node;
    rtk_list_node_t                    *p_next_prio_node;
    rtk_list_node_t                    *p_prio_node_head;
    rtk_task_prio_q_node_t             *next_task_prio_q_node;
    struct rtk_task                    *cur_task,*next_task;
    unsigned long                       cur_tick;

    p_readyq_info = the_readyq_info();
    p_readyq = the_readyq();

    cur_tick = g_systick;
    cur_task = p_readyq->p_highest_task;

    cur_readyq_prio = p_readyq->p_highest_task->readq_prio;
    p_prio_node_head = &p_readyq_info->prio_tasks_lists[cur_readyq_prio];
    p_first_prio_node = p_prio_node_head->next;

    p_next_prio_node = p_first_prio_node->next;
    if (p_next_prio_node != p_prio_node_head) {
        /* 当前最高优先级任务，存在同优先级任务 */
        next_task_prio_q_node = p_next_prio_node;
        next_task = PRIO_NODE_TO_PTCB(next_task_prio_q_node);

        if (cur_task->last_run_tick_valid && cur_task->last_run_tick != cur_tick) {
            /* 当前最高优先级任务最近运行时刻不是当前时刻 */
            /* 表明可能需要执行时间片轮转 */
            if ( (!next_task->last_run_tick_valid ) ||
                    ((cur_tick - cur_task->last_run_tick )
                < (cur_tick - next_task->last_run_tick)) ) {
                /* 当前最有优先级任务运行时刻新于下一个任务的运行时刻，才执行 */
                /* 时间片轮转 */
                rtk_list_del_init(p_first_prio_node);
                rtk_list_add_tail(p_first_prio_node,p_prio_node_head);
                p_readyq->p_highest_task = next_task;
                cur_task = next_task;
            }
        }
        else {
            /* 当前最高优先级任务最近运行时刻是当前时刻，表明当前 */
            /* 最高优先级任务是被打断了，其时间片还没用完 */
        }
    }

    return cur_task;
}


void __rtk_prio_loop_in_systick( void )
{
    register int                                cur_readyq_prio;
    register p_rtk_readyq_const_info_t          p_readyq_info;
    register rtk_ready_q_t                     *p_readyq;
    register rtk_list_node_t                   *p_first_prio_node;
    register rtk_list_node_t                   *p_prio_node_head;
    RTK_CRITICAL_STATUS_DECL(old);

    p_readyq_info = the_readyq_info();
    p_readyq = the_readyq();

    RTK_ENTER_CRITICAL(old);

    /* 从当前最高优先级任务开始 */
    /* 查找第一个具有两个及以上任务的优先级 */
    cur_readyq_prio = p_readyq->p_highest_task->readq_prio;
    while (cur_readyq_prio < RTK_MAX_PRIORITY) {
        p_prio_node_head = &p_readyq_info->prio_tasks_lists[cur_readyq_prio];
        /* 如果此优先级上存在任务 */
        if (unlikely(!rtk_list_empty(p_prio_node_head)) ) {
            p_first_prio_node = p_prio_node_head->next;
            /* 查看此优先级上是否还有其他任务 */
            /* 如果有，满足优先级轮转条件 */
            if ( unlikely(p_first_prio_node->next != p_prio_node_head) ) {
                /* 这个优先级有两个及以上任务，符合优先级轮转条件 */
                /* 完成优先级轮转 */
                rtk_list_del_init(p_first_prio_node);
                rtk_list_add_tail(p_first_prio_node,p_prio_node_head);
                /* 查看是否需要更新最高优先级节点 */
                if (cur_readyq_prio == p_readyq->p_highest_task->readq_prio) {
                    p_readyq->p_highest_task =
                            RTK_LIST_ENTRY(
                                    RTK_LIST_FIRST( p_prio_node_head),
                                    struct rtk_task, task_prio_q_node);
                }

                /* 优先级轮转已经完成 */
                break;
            }
        }
        cur_readyq_prio ++;
    }

    RTK_EXIT_CRITICAL(old);
}

