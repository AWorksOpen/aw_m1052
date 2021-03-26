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
 * \brief rtk systick相关函数
 *
 * \internal
 * \par modification history:
 * 1.00 18-04-24  sni, first implement
 * \endinternal
 */
#include "rtk.h"
#include "aw_common.h"
#include "rtk_mem.h"
#include "rtk_task_internal.h"
#include "rtk_priority_q_internal.h"
#include "aw_errno.h"
#include <string.h> /* for memset() and memcpy() */
#include "os_trace_events.h"

static struct {
    struct rtk_task     tcb;
    char                stack[CONFIG_RTK_IDLE_TASK_STACK_SIZE];
}rtk_idle;



#if CONFIG_RTK_OBJECT_CREATE_EN
static rtk_list_head_t      __g_cleanup_tasks_head;
static rtk_list_node_t * pop_a_cleanup_task_node()
{
    rtk_list_node_t        *p,*p_cleanup_tasks_head;
    RTK_CRITICAL_STATUS_DECL(old);

    RTK_ENTER_CRITICAL(old);
    p_cleanup_tasks_head = &__g_cleanup_tasks_head;
    p = p_cleanup_tasks_head ->next;
    if (p_cleanup_tasks_head != p) {
        rtk_list_del(p);
    }
    else {
        p = NULL;
    }
    RTK_EXIT_CRITICAL(old);

    return p;
}

void __rtk_add_idle_cleanup_task_ec(struct rtk_task *task)
{
    rtk_list_add_tail(&task->task_list_node,&__g_cleanup_tasks_head);
}
#endif
void pm_cpu_idle(void);

#if CONFIG_RTK_CPU_IDLE_STATISTIC
volatile uint64_t  _rtk_idle_time;

uint64_t rtk_idle_time_get( void )
{
    return _rtk_idle_time;
}
#endif

static void task_idle( void *arg )
{
#if CONFIG_RTK_OBJECT_CREATE_EN
    rtk_list_node_t        *p_cleanup_list_node;
    struct rtk_task        *p_task,*p_task_join;
    int                     err;
    RTK_CRITICAL_STATUS_DECL(old);
#endif
#if CONFIG_RTK_CPU_IDLE_STATISTIC
    RTK_INT_STATUS_DECL(old_int_status);
    rtk_timestamp_t         old,new;
#endif

#if CONFIG_TASK_TERMINATE_EN
    rtk_task_safe();
#endif
    while (1) {
#if CONFIG_RTK_OBJECT_CREATE_EN
        /* 检查是否存在需要清理的任务 */
        while (1) {
            p_cleanup_list_node = pop_a_cleanup_task_node();
            if (NULL == p_cleanup_list_node) {
                break;
            }
            p_task = RTK_LIST_ENTRY(p_cleanup_list_node,struct rtk_task,task_list_node);
            p_task_join = p_task->p_who_join_me;
            err = p_task ->error_number;

            /* 如果本任务是被动态创建，首先释放栈 */
            if (p_task->option & ENUM_RTK_TASK_OPTION_CREATE) {
                __rtk_task_release_created_task(p_task);
            }

            /* 如果有其它任务在join此任务，则唤醒它们 */
            if ( NULL  != p_task_join) {
                RTK_ENTER_CRITICAL(old);
                __rtk_task_wakeup_joining_task(p_task_join,(void *)err);
                RTK_EXIT_CRITICAL(old);
            }
        }
#endif
#if CONFIG_RTK_CPU_IDLE_STATISTIC
        RTK_INTERRUPT_DISABLE(old_int_status);
        old = rtk_timestamp_get();
        pm_cpu_idle();
        new = rtk_timestamp_get();
        _rtk_idle_time += new-old;
        RTK_INTERRUPT_ENABLE(old_int_status);

#else
        pm_cpu_idle();
#endif

    }
}


struct rtk_task *__rtk_init_idle_task(void)
{
#if CONFIG_RTK_OBJECT_CREATE_EN
    rtk_list_node_init(&__g_cleanup_tasks_head);
#endif
    __rtk_task_init_internal(
            &rtk_idle.tcb,
            "idle",
            RTK_MAX_PRIORITY,
            ENUM_RTK_TASK_OPTION_CAN_NOT_BE_FROZE,
            rtk_idle.stack,
            rtk_idle.stack + sizeof(rtk_idle.stack),
            task_idle,
            0,
            0
            );
    return &rtk_idle.tcb;
}
void __rtk_start_idle_task(void)
{
    struct rtk_task         *p_task;
    RTK_CRITICAL_STATUS_DECL(old);

    p_task = &rtk_idle.tcb;

    RTK_ENTER_CRITICAL(old);
    p_task->status = TASK_READY;
    _rtk_readyq_put_idle_task(p_task);
    RTK_EXIT_CRITICAL(old);

}

int __rtk_is_startup(void)
{
    if ( NULL == rtk_task_self()) {
        return 0;
    }
    return 1;
}
