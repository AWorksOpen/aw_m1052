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
 * \file  rtk_schedule.h
 * \brief 
 *
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
#include "rtk_systick.h"
#include "aw_errno.h"
#include "aw_common.h"
#include <string.h> /* for memset() and memcpy() */

#include "os_trace_events.h"

struct rtk_task  *volatile      rtk_task_current = NULL;
#if CONFIG_RTK_PERFORMANCE_STATISTIC_EN
static rtk_timestamp_t          _g_tasK_start_exec_moment = 0;
#endif

/**
 * \brief RTK 当前任务停止执行
 *
 * \attention 这是一个内部函数，在汇编语言中调用
                当要切换任务的时候，先调用此函数表明当前任务停止执行
 * 若操作系统还未启动，则最好不要调用此函数
 * \return  此函数返回当前任务tcb结构中sp成员的地址，方便汇编语言中操作
 */
uint32_t * __rtk_cur_task_stop_exec_ec(void)
{
#if CONFIG_RTK_PERFORMANCE_STATISTIC_EN
    aw_timestamp_t      t;
    t = rtk_timestamp_get();
    rtk_task_current->total_running_time += t - _g_tasK_start_exec_moment;
#endif
    return (uint32_t *)((unsigned long)rtk_task_current
            + RTK_OFFSET_OF(struct rtk_task,sp) );
}

struct rtk_task *rtk_task_self(void)
{
    return rtk_task_current;
}

static void rtk_set_self_ec(struct rtk_task *p)
{
    rtk_task_current = p;
#if CONFIG_NEWLIB_REENT_EN
    _impure_ptr = &p->newlib_reent;
#endif
}

void _rtk_task_begin_exec(void **sp)
{
    struct rtk_task        *p_task;
#if CONFIG_RTK_PERFORMANCE_STATISTIC_EN
    _g_tasK_start_exec_moment = rtk_timestamp_get();
#endif
    p_task = RTK_LIST_ENTRY(sp,struct rtk_task,sp);
    p_task ->last_run_tick = g_systick;
    p_task->last_run_tick_valid = 1;
    rtk_set_self_ec(p_task);
}

#if CONFIG_RTK_SCHEDULE_HOOK

static volatile pfn_rtk_hook_t      __fn_sched_begin_hook = NULL;
static volatile pfn_rtk_hook_t      __fn_sched_end_hook = NULL;

void rtk_sched_begin_hook_set(pfn_rtk_hook_t hook_fn)
{
    __fn_sched_begin_hook = hook_fn;
}

void _rtk_sched_begin_hook_exec(void)
{
    pfn_rtk_hook_t      fn = __fn_sched_begin_hook;
    if (NULL != fn) {
        fn();
    }
}

void rtk_sched_end_hook_set(pfn_rtk_hook_t hook_fn)
{
    __fn_sched_end_hook = hook_fn;
}

void _rtk_sched_end_hook_exec(void)
{
    pfn_rtk_hook_t      fn = __fn_sched_end_hook;
    if (NULL != fn) {
        fn();
    }
}
#endif

uint32_t * volatile     _rtk_switch_to_tcb_sp_addr;

int rtk_task_yield( void )
{
    RTK_CRITICAL_STATUS_DECL(old);
    struct rtk_task    *task_current;

    if ( rtk_is_int_context() ) {
        return -AW_EPERM;
    }

    task_current = rtk_task_self();
    task_current->stack_deep = (task_current->stack_deep > task_current->sp) ? \
                                task_current->sp : task_current->stack_deep;

    RTK_ENTER_CRITICAL(old);

    READY_Q_REMOVE( task_current );
    _rtk_switch_to_tcb_sp_addr = __rtk_highest_tcb_sp_addr_ec();
    READY_Q_PUT( task_current );
    RTK_EXIT_CRITICAL(old);

    rtk_arch_context_switch( );

    return 0;
}

void rtk_schedule_isr(void)
{
    RTK_CRITICAL_STATUS_DECL(old);
    struct rtk_task        *p;
    struct rtk_task        *self = rtk_task_self();

    self->stack_deep = (self->stack_deep > self->sp) ? self->sp : self->stack_deep;
    RTK_ENTER_CRITICAL(old);
    p = _rtk_highest_task_get_ec();
    _rtk_switch_to_tcb_sp_addr = (uint32_t *)&p->sp;
    RTK_EXIT_CRITICAL(old);
    if (p != self) {
        rtk_arch_context_switch_interrupt();
    }
}

void rtk_schedule_systick_isr(void)
{
    RTK_CRITICAL_STATUS_DECL(old);
    struct rtk_task        *p;
    struct rtk_task        *self = rtk_task_self();

    self->stack_deep = (self->stack_deep > self->sp) ? self->sp : self->stack_deep;
    RTK_ENTER_CRITICAL(old);
    p = _rtk_highest_task_loop_get_ec();
    _rtk_switch_to_tcb_sp_addr = (uint32_t *)&p->sp;
    RTK_EXIT_CRITICAL(old);
    if (p != self) {
        rtk_arch_context_switch_interrupt( );
    }
}

void rtk_schedule(void)
{
    RTK_CRITICAL_STATUS_DECL(old);
    struct rtk_task        *p;
    struct rtk_task        *self = rtk_task_self();

    self->stack_deep = (self->stack_deep > self->sp) ? self->sp : self->stack_deep;
    RTK_ENTER_CRITICAL(old);
    p = _rtk_highest_task_loop_get_ec();
    _rtk_switch_to_tcb_sp_addr = (uint32_t *)&p->sp;
    RTK_EXIT_CRITICAL(old);
    if (p != self) {
        rtk_arch_context_switch( );
    }
}
/* 判断当前是否需要任务调度，此函数需要在中断保护区域内运行*/
long rtk_is_task_scheduler_needed(void)
{

#if 0
    struct rtk_task *p;
    struct rtk_task *self = rtk_task_self();

    p = _rtk_highest_task_loop_get_ec();
    if ( p != self ) {
        return 1;
    }
#endif
    return 0;
}
