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
 * \file rtk_task.c
 * \brief rtk task实现
 *
 * \par 源代码
 * \internal
 * \par Modification history
 * - 3.00       18-06-13 sni, first version.
 * - 2.02.1110  7-11-10  imp, add OS_TRACE instrumentation;remove RTK_TRACE().
 * - 2.01.1209  15-12-09 dcf, add RTK_TRACE(); fix bug: msgq_send() always return 0.
 * - 2.00.1117  15-11-17 dcf, add semc_broadcast, add members to rtk_task.
 * - 1.03.0     15-08-27 dcf, fix bug: msgq_clear();
 * - 1.02.0     15-08-26 dcf, rewrite msgq, add mbox; fix software timer bug; Support newlib reent.
 * - 1.00.14433 14-11-19 dcf, support task_errno_set/get
 * \endinternal
 */
#include "rtk.h"
#include "rtk_mem.h"
#include "aw_errno.h"
#include <string.h> /* for memset() and memcpy() */
#include "rtk_arch.h"
#include "rtk_task_internal.h"
#include "rtk_priority_q_internal.h"
#include "rtk_tick_down_counter_internal.h"
#include "rtk_idle.h"
#include "aw_common.h"
#include "rtk_waitq_internal.h"
#include "rtk_mutex_internal.h"
#include "rtk_base_mutex.h"
#include "rtk_mutex_sync_lock.h"
#include "rtk_systick.h"

#ifndef rtk_assert
#include "aw_assert.h" /* for aw_assert() */
#define rtk_assert aw_assert
#endif

#include "os_trace_events.h"


/*******************************************************************************
  globle var
*******************************************************************************/
const char * const               rtk_version = \
            "rtk_version:"__RTK_VERSION_STR__"\n";

rtk_list_head_t                 g_system_tasks_head;

static void __task_delay_timeout( void *param );
static void __rtk_task_exit( void * );
static int __rtk_task_freeze(
        struct rtk_task *task,
        int freeze_running_task);

void __rtk_task_global_init(void)
{
    rtk_list_node_init(&g_system_tasks_head);
}

static int __rtk_task_check_exist(struct rtk_task *find_task)
{
    rtk_list_node_t        *p;
    struct rtk_task        *task;
    int                     r = 0;

    rtk_list_for_each( p, &g_system_tasks_head )
    {
        task = RTK_LIST_ENTRY(p, struct rtk_task, task_list_node);
        if (task == find_task) {
            r = 1;
            break;
        }
    }

    return r;
}

static void _add_to_task_list_os_not_startup(struct rtk_task *p_task)
{
    rtk_list_add_tail( &p_task->task_list_node, &g_system_tasks_head );
}

static void _add_to_task_list(struct rtk_task *p_task)
{
    __rtk_task_mutex_sync_lock();
    rtk_list_add_tail( &p_task->task_list_node, &g_system_tasks_head );
    __rtk_task_mutex_sync_unlock();
}

static void _rtk_task_add_to_task_list(struct rtk_task *p_task)
{
    if (__rtk_is_startup()) {
        _add_to_task_list(p_task);
    }
    else {
        _add_to_task_list_os_not_startup(p_task);
    }
}

int rtk_task_check_exist(struct rtk_task *find_task)
{
    int             err;
    if (rtk_is_int_context() ) {
        return -AW_EPERM;
    }

    if (NULL == find_task) {
        return -AW_EINVAL;
    }

    __rtk_task_mutex_sync_lock();
    err = __rtk_task_check_exist(find_task);
    __rtk_task_mutex_sync_unlock();

    return err;
}

void __rtk_task_wakeup_joining_task(struct rtk_task *task,void *err)
{
    task->p_join_ret = (void *)err;
    task->p_I_join_who = NULL;
    OS_TRACE_TASK_START_READY(task);
    task->status = TASK_READY;
    READY_Q_PUT( task);
}

int rtk_task_delay(unsigned int tick )
{
    RTK_CRITICAL_STATUS_DECL(old);
    struct rtk_task    *task_current;

    if (rtk_is_int_context() ) {
        return -AW_EPERM;
    }

    if (0 == tick) {
        tick = 1;
    }
    if (AW_WAIT_FOREVER == tick) {
        tick = 0xFFFFFFFE;
    }

    task_current = rtk_task_self();
    OS_TRACE_TASK_DELAY(tick);

    /* 添加一个tick 唤醒 */
    __task_set_wakeup_after_tick(task_current,tick);

    /* 进入系统临界区 */
    RTK_ENTER_CRITICAL(old);
    /* 确保这期间任务没有被systick唤醒 */
    if (!rtk_list_empty(&task_current->tick_node.node) ) {
        task_current->status = TASK_DELAY;
        /* 将任务从就绪队列中移除 */
        READY_Q_REMOVE( task_current );
        OS_TRACE_TASK_STOP_READY(task_current, task_current->status);
    }
    RTK_EXIT_CRITICAL(old);

    rtk_schedule();

    __task_get_wake_reason(task_current);
    return 0;
}

void __task_pend_on_waitq_ec(
        struct rtk_task *task,
        rtk_wait_queue_t *p_wait_queue)
{
    __rtk_waitq_add_pender_ec(task,p_wait_queue);
}

void __task_set_wakeup_after_tick(
        struct rtk_task *task,
        unsigned int tick)
{
    __rtk_tick_down_counter_add( &task->tick_node, tick );
}

void __task_unset_wakeup_after_tick(
        struct rtk_task *task)
{
    __rtk_tick_down_counter_remove(&task->tick_node);
}

aw_err_t __task_get_wake_reason(struct rtk_task *task_current)
{
    uint32_t                    reason;
    aw_err_t                    ret;

    RTK_CRITICAL_STATUS_DECL(old);

    RTK_ENTER_CRITICAL(old);
    reason = task_current->wake_reason;
    task_current->wake_reason = 0;
    RTK_EXIT_CRITICAL(old);

    if (RTK_TASK_WAKE_BY_WAITQ & reason) {
        ret = AW_OK;
    }
    else if (RTK_TASK_WAKE_BY_TERMINATE & reason) {
        ret = -AW_ENXIO;
    }
    else if (RTK_TASK_WAKE_BY_TIMEOUT & reason) {
        ret = -AW_ETIME;
    }
    else {
        ret = AW_OK;
    }

    return ret;

}

static void __task_delay_timeout( void *param )
{
    struct rtk_task        *task;
    struct rtk_tick        *_this = (struct rtk_tick *)param;

    task = TICK_NODE_TO_PTCB( _this );

    /* 此刻已经属于系统临界区 */
    /* 将任务从pending队列移除 */
    __task_detach_pending_waitq_ec(task);
    __task_wakup_with_err_ec(task,RTK_TASK_WAKE_BY_TIMEOUT);
}

unsigned int _task_cal_suit_readyq_prio_ec(struct rtk_task *p_task)
{
    unsigned int        new_readyq_prio;

    new_readyq_prio = p_task->readq_prio;
    /* 如果没有因为base mutex提升过优先级， */
    /* 则readyq应该跟随waitq_prio*/
    if (p_task->base_mutex_need_prio >= RTK_MAX_PRIORITY) {
        new_readyq_prio = p_task->waitq_prio;
    }
    else {
        /* 如果因为base mutex提升过优先级，则新的readyq_prio不能低于waitq_prio */
        /* 且如果优先级被降低，则新的readyq_prio不能低于base_mutex_need_prio */
        if (new_readyq_prio > p_task->waitq_prio) {
            new_readyq_prio = p_task->waitq_prio;
        }
        else {
            if (p_task->base_mutex_need_prio < p_task->waitq_prio) {
                new_readyq_prio = p_task->base_mutex_need_prio;
            }
            else {
                new_readyq_prio = p_task->waitq_prio;
            }
        }
    }

    return new_readyq_prio;
}


#if CONFIG_TASK_PRIORITY_SET_EN
/**
 *  \addtogroup TASK_API    task API
 *  @{
 *
 *  @brief set task priority
 *  @fn rtk_task_priority_set
 *  @param[in]  task            task control block pointer. If NULL, current task's
 *                              priority will be change.
 *  @param[in]  new_priority    new priority.
 *  @return     0               successfully.
 *  @return     -AW_EINVAL         Invalid argument.
 *  @return     -AW_EPERM          Permission denied. The task is not startup yet.
 *  basic rules:
 *      1. if task's priority changed, we must check if we need to do something with
 *         it's pending resource (only when it's status is pending).
 *      2. if task's priority goes down, we must check mutex list's priority.
 *         Maybe it's priority cannot go down right now.
 *
 *            P0                 P1                P2
 *             |                  |                 |
 *  0(high) ==============================================>> 256(low priority)
 *                     ^                  ^
 *                     |                  |
 *              current priority    normal priority
 */
int rtk_task_priority_set( struct rtk_task *task, unsigned int new_priority )
{
    RTK_CRITICAL_STATUS_DECL(old);
    int                     err = AW_OK;
    rtk_wait_queue_t       *p_wait_queue;
    int                     trig;
    int                     is_mutex;
    unsigned int            readyq_prio;
#if CONFIG_RTK_MUTEX_EN
    unsigned int            prio_mutex;
#endif

    OS_TRACE_TASK_PRIORITY_SET_ENTER(task, new_priority);
    /* 不允许在中断中调用 */
    if (rtk_is_int_context() ) {
        err = -AW_EPERM;
        goto func_exit;
    }

    if ( new_priority >= RTK_MAX_PRIORITY ) {
        err = -AW_EINVAL;
        goto func_exit;
    }

    if ( task == NULL ) {
        task = rtk_task_self();
    }

    __rtk_task_mutex_sync_lock();

    /* 不能操作不存在的任务 */
    if ( __rtk_task_check_exist(task) <=0) {
        __rtk_task_mutex_sync_unlock();
        err = -AW_EINVAL;
        goto func_exit;
     }

    /* 禁用中断，以便锁定重入 */
    RTK_ENTER_CRITICAL(old);
    /* 如果任务优先级和要设置的优先级相同，也视为设置成功 */
    /* 但是就不必进行下面的操作了 */
    if (task->waitq_prio == new_priority) {
        RTK_EXIT_CRITICAL( old );
        goto cleanup;
    }

#if CONFIG_RTK_MUTEX_EN
    task->hold_mutex_org_priority = new_priority;
    if ( new_priority < task->waitq_prio ) {
        /* 如果是提高优先级，则可以直接提高 */
        task->waitq_prio = new_priority;
    }
    else {
        /* 如果是降低优先级 */
        prio_mutex = __task_hold_mutex_priority_get( task );
        if (prio_mutex >= new_priority) {
            /* 当要降低到的优先级 高于 正在等待 */
            /* 任务中所持有互斥量的所有任务中优先级最高任务的优先级时 */
            /* 可以降低到新的优先级 */
            task->waitq_prio = new_priority;
        }
        else {
            /* 否则只能降低到任务持有互斥量的最高优先级 */
            task->waitq_prio = prio_mutex;
            new_priority = prio_mutex;
        }
    }

#else
    task->waitq_prio = new_priority;
#endif

    readyq_prio = _task_cal_suit_readyq_prio_ec(task);
    if ( task->status == TASK_READY ) {

        if (readyq_prio != task->readq_prio) {
            _task_ready_prio_change_ec(task,readyq_prio);
        }
        RTK_EXIT_CRITICAL( old );
        goto cleanup;
    }
    else {
        task->readq_prio = readyq_prio;
        if ( task->status & TASK_PENDING ) {
            p_wait_queue = task->pending_resource;
            trig = __rtk_waitq_resort_pender_ec(
                    p_wait_queue, task );
            is_mutex = (p_wait_queue->type == RTK_WAIT_QUEUE_TYPE_MUTEX);
            RTK_EXIT_CRITICAL( old );
#if CONFIG_RTK_MUTEX_EN
            if ( trig && is_mutex ) {
                __mutex_raise_owner_priority(
                        RTK_CONTAINER_OF(p_wait_queue,struct rtk_mutex,wait_queue),
                        task->waitq_prio
                        );
            }
#else
            trig = trig;
            is_mutex = is_mutex;
#endif
        }
        else {
            RTK_EXIT_CRITICAL(old);
        }
    }

cleanup:
    __rtk_task_mutex_sync_unlock();

    /* 保证高优先级任务先执行 */
    rtk_schedule();
func_exit :
    OS_TRACE_TASK_PRIORITY_SET_EXIT(err);
    return err;
}
#endif

/** @} */

struct rtk_task *__rtk_task_init_internal(
        struct rtk_task    *task,
        const char         *name,
        int                 priority,
        rtk_task_option_t   option,
        char *              stack_low,
        char *              stack_high,
        void               *pfunc,
        void               *arg1,
        void               *arg2)
{
    RTK_CRITICAL_STATUS_DECL(old);

    task->waitq_prio = priority;
    task->readq_prio = priority;
    task->status           = 0;
    task->pending_resource = (void*)0;
    task->option           = option;
    task->stack_low        = stack_low;
    task->stack_high       = stack_high;
    task->stack_deep       = stack_high;
    task->name             = name;
    task->thread_local_storage = 0;
    task->base_mutex_need_prio = RTK_MAX_PRIORITY;


#if CONFIG_RTK_MUTEX_EN
    task->hold_mutex_org_priority         = priority;
#endif

#if CONFIG_TASK_TERMINATE_EN
    task->safe_count       = 0;
#endif
    task->error_number = 0;
    memset( stack_low, 0x55, stack_high-stack_low );

    task->sp = rtk_arch_stack_init(
            pfunc,
            arg1,
            arg2,
            stack_low,
            stack_high,
            __rtk_task_exit,
            option);
    rtk_list_node_init( &task->task_prio_q_node );
    rtk_list_node_init( &task->wait_queue_node );

#if CONFIG_RTK_MUTEX_EN
    rtk_list_node_init( &task->mutex_holded_head );
#endif
    rtk_list_node_init( &task->base_mutex_holded_head );

#if CONFIG_TASK_JOIN_EN
    task -> p_who_join_me = NULL;
    task -> p_I_join_who = NULL;
    task -> p_join_ret = (void *)0;
#endif

    rtk_list_node_init( &task->task_list_node );

    __rtk_tick_down_counter_init(&task->tick_node);
    __rtk_tick_down_counter_set_func(
            &task->tick_node,
            __task_delay_timeout,
            &task->tick_node);
    task ->last_run_tick = 0;
    task->last_run_tick_valid = 0;
#if CONFIG_NEWLIB_REENT_EN
    _REENT_INIT_PTR( &task->newlib_reent );
#endif

    task->status = TASK_NOT_STARTUP;
#if CONFIG_RTK_PERFORMANCE_STATISTIC_EN
    task->total_running_time = 0;
#endif
    /* 将任务添加到所有任务列表 */
    _rtk_task_add_to_task_list(task);
    OS_TRACE_TASK_CREATE(task);

    return task;
}

struct rtk_task *rtk_task_init(struct rtk_task    *task,
                               const char         *name,
                               int                 priority,
                               rtk_task_option_t   option,
                               char *              stack_low,
                               char *              stack_high,
                               void               *pfunc,
                               void               *arg1,
                               void               *arg2)
{

    OS_TRACE_TASK_INIT(task);

    /* 不允许在中断中调用 */
    if (rtk_is_int_context() ) {
        return NULL;
    }

    if (NULL == task || NULL== name || NULL == pfunc) {
        return NULL;
    }
    /* 防止堆栈过小 */
    if ( ((uint32_t)stack_high) <= ((uint32_t)stack_low) ) {
        return NULL;
    }

    if ( ((uint32_t)stack_high) - ((uint32_t)stack_low) <= 32 ) {
        return NULL;
    }

    /* 确保正在运行的任务不会被重新初始化 */
    if (NULL != rtk_task_self() && rtk_task_check_exist(task) > 0) {
        return NULL;
    }

    memset(task,0,sizeof(*task) );
    if (priority >= RTK_MAX_PRIORITY) {
        return NULL;
    }

    return __rtk_task_init_internal(
            task,
            name,
            priority,
            option,
            stack_low,
            stack_high,
            pfunc,
            arg1,
            arg2);
}

static void task_cleanup_join_other_task(struct rtk_task *task)
{
    register struct rtk_task            *p_I_join_who;

    p_I_join_who = task->p_I_join_who;
    if (NULL != p_I_join_who) {
        task->p_I_join_who = NULL;
        p_I_join_who ->p_who_join_me = NULL;
    }
}


#if CONFIG_TASK_TERMINATE_EN
int rtk_task_safe( void )
{
    RTK_CRITICAL_STATUS_DECL(old);
    int                     err = AW_OK;
    /* 不允许在中断中调用 */
    if (rtk_is_int_context() ) {
        return -AW_EPERM;
    }

    /*  获取全局锁，和rtk_task_terminate同步 */
    RTK_ENTER_CRITICAL(old);

    /* 遇到了unsafe调用次数过多 */
    if (rtk_task_self()->safe_count < 0) {
        err = -AW_EFAULT;
        goto cleanup;
    }

    ++rtk_task_self()->safe_count;
cleanup:
    RTK_EXIT_CRITICAL(old);
    OS_TRACE_TASK_SAFE(rtk_task_self()->safe_count);

    return err;
}

int rtk_task_unsafe( void )
{
    RTK_CRITICAL_STATUS_DECL(old);
    int                 err = AW_OK;
    struct rtk_task    *p_cur_task;

    /* 不允许在中断中调用 */
    if (rtk_is_int_context() ) {
        return -AW_EPERM;
    }

    p_cur_task = rtk_task_self();
    /*  获取全局锁，和rtk_task_terminate同步 */
    RTK_ENTER_CRITICAL(old);

    /* 遇到了unsafe调用次数过多 */
    if (p_cur_task->safe_count <= 0) {
        err = -AW_EFAULT;
        goto cleanup;
    }

    --p_cur_task->safe_count;

cleanup:
    RTK_EXIT_CRITICAL(old);
    OS_TRACE_TASK_UNSAFE(p_cur_task->safe_count);

    return err;
}


int rtk_task_terminate( struct rtk_task *task )
{
    RTK_CRITICAL_STATUS_DECL(old);
    struct rtk_task        *p_who_join_me = NULL;

#if CONFIG_RTK_MUTEX_EN
    rtk_list_node_t        *save, *p;
#endif

    /* 不允许在中断中调用 */
    if (rtk_is_int_context() ) {
        return -AW_EPERM;
    }

    if (NULL == task) {
        return -AW_EINVAL;
    }

    if ( task == rtk_task_self() ) {
        return -AW_EPERM;
    }

    /* 检查要结束任务的有效性 */
    __rtk_task_mutex_sync_lock();
    if (!__rtk_task_check_exist(task) ) {
        __rtk_task_mutex_sync_unlock();
        return -AW_EINVAL;
    }

    /* 因为已经持有了__g_system_tasks_head_lock */
    /* 所以目标任务不可能在此期间自动退出了 */

    /* 检查任务是否处于safe状态 */
    RTK_ENTER_CRITICAL(old);
    if (task->safe_count) {
        /* 任务处于不允许终止状态 */
        RTK_EXIT_CRITICAL(old);
        __rtk_task_mutex_sync_unlock();
        return -AW_EPERM;
    }
    /* 启用中断，使中断可以正常响应 */
    RTK_EXIT_CRITICAL(old);

    /* 将任务从系统任务链表中移除，阻止其它任务再操作目标task */
    rtk_list_del_init(&task->task_list_node);

    /* 将目标任务冻结,如果目标任务正在执行 */
    /* 则目标任务将停留在没有持有systick锁和task_mutex锁的位置 */
    __rtk_task_freeze(task,1);

    /* 清除可能存在的delay node */
    __task_unset_wakeup_after_tick( task);

    RTK_ENTER_CRITICAL(old);
    /* 禁用中断，移除pending资源 */
#if CONFIG_SEMB_EN||CONFIG_SEMC_EN
    __task_detach_pending_waitq_ec(task);
#endif
    /* 至此，要结束的任务不会再被调度到，因为tick_node和penging资源都已经释放 */
    /* 所以也不会被其它中断或任务以延时时间到或信号量有信号等的方式唤醒 */
    /* 此时可以打开中断，以便减少长时间关闭中断带来的中断延迟 */
    RTK_EXIT_CRITICAL(old );

    /* 要结束的任务可能正在join其它任务*/
    RTK_ENTER_CRITICAL(old);
    task_cleanup_join_other_task(task);
    RTK_EXIT_CRITICAL(old);

#if CONFIG_RTK_MUTEX_EN
    /* release all mutex. */
    /* 因为task对应的任务此时对其它任务和中断来说已经是无效状态 */
    /* 其它任务和中断也不会再访问task的tcb，所以此时对task的mutex_holded */
    /* 链表的访问是安全的,task自己也已经停止运行 */
    rtk_list_for_each_safe(p, save, &task->mutex_holded_head){
        struct rtk_mutex       *p_mutex;
        p_mutex = MUTEX_HOLD_NODE_TO_MUTEX( p );
        /* 将此mutex结束掉，以便所有pender都返回-AW_EXNIO */
        /* 同时让以后的lock操作都失败，以阻止对共享资源的不良影响 */
        rtk_mutex_terminate(p_mutex);
    }
#endif

    p_who_join_me = task->p_who_join_me;
    /* 释放掉task_mutex锁，以便高优先级任务可以操作mutex和其它任务API */
    __rtk_task_mutex_sync_unlock();

#if CONFIG_RTK_OBJECT_CREATE_EN
    /* 释放要结束任务所占用的内存空间 */
    /* 如果本任务是被动态创建，首先释放栈 */
    if (task->option & ENUM_RTK_TASK_OPTION_CREATE) {
        __rtk_task_release_created_task(task);
    }
#endif

    /* 如果有其它任务在join 要结束的任务 */
    if ( NULL  != p_who_join_me ) {
        RTK_ENTER_CRITICAL(old);
        __rtk_task_wakeup_joining_task(p_who_join_me,(void *)-AW_EPIPE);
        RTK_EXIT_CRITICAL(old);
    }

    /* 执行一次调度，确保join要结束任务的任务如果优先级比当前任务高，可以被 */
    /* 正确执行 */
    rtk_schedule();

    return 0;
}
#endif

static void __task_exit( void *ret )
{
    struct rtk_task        *task = rtk_task_self();
    RTK_CRITICAL_STATUS_DECL(old);

#if CONFIG_RTK_MUTEX_EN
    rtk_list_node_t        *save, *p;
#endif

    /* 因为是自动退出，当前任务不可能在join其它任务 */


    /* 自动退出的任务不会存在delay操作 */
    /* 所以不会有delay node */

    /* 释放已经拥有的mutex,老实说这是不想被执行的代码 */
    /* 因为这往往意味着代码里面存在着未释放的互斥锁 */
    /* 这可是严重的错误逻辑啊 */

#if CONFIG_RTK_MUTEX_EN
    rtk_list_for_each_safe(p, save, &task->mutex_holded_head) {
        struct rtk_mutex *p_mutex;
        p_mutex = MUTEX_HOLD_NODE_TO_MUTEX( p );
        /* terminate 所持有的mutex，使得其它任务再也不能访问被mutex保护的资源了 */
        /* 程序员应该要记住，退出任务之前，请unlock所有mutex */
        rtk_mutex_terminate( p_mutex );
    }
#endif
    /*获得task_mutex锁，获取锁后，其它任务暂时不能发起新的被 */
    /*task_mutex保护的操作，包括查询任务是否有效 */
    __rtk_task_mutex_sync_lock();
    RTK_ENTER_CRITICAL(old);
    /* 自动退出的任务不会pending在某个信号量上 */
    /* 现在是时候将本任务从任务列表中移除了 */
    task->status = TASK_DEAD;
    READY_Q_REMOVE( task );

    /* 把当前任务从系统任务链表上移除 */
    /* 这样，其它任务操作如terminate就会发现当前任务已经无效 */
    /* 就不会再操作了 */
    rtk_list_del_init( &task->task_list_node );

    if (task->option & ENUM_RTK_TASK_OPTION_CREATE) {
#if CONFIG_RTK_OBJECT_CREATE_EN
        /* sem,muxtex都被释放，此任务再也不会被调度到， */
        /* 还剩下tcb和栈没有被释放 */
        /* 将tcb和栈交给idle来释放，唤醒join此任务的任务也交给idle来完成 */
        task->error_number = (int)ret;
        __rtk_add_idle_cleanup_task_ec(task);
#endif
    }
    else {
        /* 这是一个静态任务，不需要释放tcb和栈,唤醒join此任务的任务 */
        if (NULL != task->p_who_join_me) {
            __rtk_task_wakeup_joining_task(task->p_who_join_me,ret);
        }
    }

    __rtk_task_mutex_sync_unlock_no_sche();
    /* 此时打开中断，可以降低中断延迟时间，并由中断驱动任务调度 */
    /* 当前函数不再执行 */
    RTK_EXIT_CRITICAL(old);
    /* 但是如果没有中断要处理，则必须手工发起调度了 */
    rtk_schedule();
}

static void __rtk_task_exit( void *ret )
{
#if CONFIG_RTK_USERMODE
    /* if current is privileged mode,we can exit directly */
    if (rtk_arch_is_privileged_mode() ) {
        __task_exit(ret);
    }
    else {
        /* 在这个时候需要执行一次syscall */
        rtk_arch_exec_task_exit_syscall(ret);
    }
#else
    __task_exit(ret);
#endif
}

int rtk_task_exit(void * status)
{
    if (rtk_is_int_context() ) {
        return -AW_EPERM;
    }

    __rtk_task_exit(status);
    return 0;
}

#if CONFIG_TASK_JOIN_EN


int rtk_task_join(struct rtk_task *task,void **p_status)
{
    RTK_CRITICAL_STATUS_DECL(old);
    int                 ret = 0;
    struct rtk_task    *task_current;
    int                 need_schedule = 0;

    task_current = rtk_task_self();
    OS_TRACE_TASK_JOIN_ENTER(task_current,task);

    /* 不允许join自己，也不允许在中断中进行此操作 */
    if (rtk_is_int_context() ) {
        ret = -AW_EPERM;
        goto func_exit;
    }

    if (task == NULL) {
        ret = -AW_EINVAL;
        goto func_exit;
    }

    if (task == task_current) {
        ret = -AW_EDEADLK;
        goto func_exit;
    }

    __rtk_task_mutex_sync_lock();
    /* 不允许join不在系统中的任务 */
    if (!__rtk_task_check_exist(task) ) {
        __rtk_task_mutex_sync_unlock();
        ret = -AW_EINVAL;
        goto func_exit;
    }

    /* 阻止其它任务再操作本task */

    RTK_ENTER_CRITICAL(old);
    /* 已经被join的任务不能再被join */
    if (NULL !=task -> p_who_join_me) {
        RTK_EXIT_CRITICAL(old);
        __rtk_task_mutex_sync_unlock();
        ret = -AW_EBUSY;
        goto func_exit;
    }
    /* 设置目标任务正在被join */
    task->p_who_join_me = task_current;
    task_current->p_I_join_who = task;
    RTK_EXIT_CRITICAL(old);
    __rtk_task_mutex_sync_unlock();

    /* 设置当前任务为非就绪状态 */
    RTK_ENTER_CRITICAL(old);
    /* 如果上次中断启用到禁用期间，目标任务已经结束 */
    /* 则不用将当前任务移出就绪任务队列 */
    if (NULL != task_current->p_I_join_who) {
        /* 将自己从就绪队列上移除 */
        task_current->status = TASK_JOINING;
        READY_Q_REMOVE( task_current );
        need_schedule = 1;
    }
    /* 此刻已经可以解锁中断了 */
    RTK_EXIT_CRITICAL(old);
    /* 此时如果发生中断，则会去执行中断，中断处理完后会自动挑选一个优先级 */
    /* 最高的任务，此任务的下一句代码便不会执行 */
    /* 直到join的任务结束后，此任务便会就绪，开始执行下一句代码 */

    /* 但是如果中断没发生，则因为此任务已经不再就绪 */
    /* 所以必须要发起一次主动调度，执行剩下优先级最高的任务 */

    /* 所以接下来需要主动发起一次调度，如果从上一句到这一句期间没有中断发生 */
    /* 则调度是必须的，而如果有，再执行一次调度也不会有影响， */
    /* 因为当前任务优先级最高调度器执行很快 */

    if (need_schedule) {
        rtk_schedule();
    }

    /* 到这里，已经可以确保当前任务因为join的任务而唤醒了 */
    if (NULL != p_status) {
        *p_status = task_current->p_join_ret;
    }

func_exit:
    OS_TRACE_TASK_JOIN_EXIT(ret);
    return ret;
}
#endif

static int __rtk_task_freeze(
        struct rtk_task  *task,
        int               freeze_running_task)
{
    int                     err = AW_OK;
    RTK_CRITICAL_STATUS_DECL(old);
    RTK_SYSTICK_LOCK_DECL(systick_lock);

    RTK_SYSTICK_LOCK(systick_lock);
    RTK_ENTER_CRITICAL(old);
    if ( task->status == TASK_READY ) {
        /* 此任务处于就绪状态，需要看看是否确定冻结正在执行的任务 */
        /* 因为冻结正在执行的任务可能会产生不想要的结果 */
        /* 一般仅在关机时才冻结正在执行的任务，挂起时不能冻结正在执行的任务 */
        if (freeze_running_task) {
            READY_Q_REMOVE( task );
            task->status = TASK_FROZE;
            OS_TRACE_TASK_STOP_READY(task, task->status);
            RTK_EXIT_CRITICAL(old);
            RTK_SYSTICK_UNLOCK(systick_lock);
            /* 这里不需要执行调度，因为当前任务优先级可能因为yield而不是最高 */
            /* 但是这里也不需要切换会那个更高的优先级 */
            /* 等待下次systick到来是自然会切换 */
            goto cleanup;
        }
        else {
            RTK_EXIT_CRITICAL(old);
            RTK_SYSTICK_UNLOCK(systick_lock);
            err = -AW_EPERM;
            goto cleanup;
        }
    }
    else {
        /* 此线程处于pending或是delay状态 */
        /* 设置线程正处于冻结中标志，此线程试图变为ready状态的时候会自动froze */
        task->status |= TASK_FREEZING;
    }
    RTK_EXIT_CRITICAL(old);

    RTK_SYSTICK_UNLOCK(systick_lock);
cleanup:
    return err;
}

int rtk_task_freeze( struct rtk_task *task,int freeze_running_task)
{
    int            err = AW_OK;

   /* 不允许在中断中调用，也不允许冻结自己 */
   if (rtk_is_int_context() || task == rtk_task_self()) {
       err = -AW_EPERM;
       goto func_exit;
   }

    if ( NULL == task ) {
        err = -AW_EINVAL;
        goto func_exit;
    }

    /* 检查任务是否允许冻结 */
    if ((ENUM_RTK_TASK_OPTION_CAN_NOT_BE_FROZE & task ->option )) {
        err = -AW_EPERM;
        goto func_exit;
    }

    __rtk_task_mutex_sync_lock();
    /* 检查任务是否存在于系统中 */
    if (!__rtk_task_check_exist(task) ) {
        __rtk_task_mutex_sync_unlock();
        err = -AW_EINVAL;
        goto func_exit;
    }

    err = __rtk_task_freeze(task,freeze_running_task);
    __rtk_task_mutex_sync_unlock();
func_exit:
    return err;
}


static int _startup_os_not_startup(struct rtk_task *task)
{
    int            err = AW_OK;

    /* 检查任务是否存在于系统中 */
    if ( __rtk_task_check_exist(task) <=0) {
        err = -AW_EINVAL;
        goto cleanup;
    }

    task->status = TASK_READY;
    READY_Q_PUT( task );
    OS_TRACE_TASK_START_READY(task);
cleanup:
    return err;
}

static int _startup_os_has_startup(struct rtk_task *task)
{
    int                 err = AW_OK;
    RTK_CRITICAL_STATUS_DECL(old);
    int                 need_schedule = 0;

    __rtk_task_mutex_sync_lock();

    /* 不能启动不存在的任务 */
    if ( __rtk_task_check_exist(task) <=0) {
        __rtk_task_mutex_sync_unlock();
        err = -AW_EINVAL;
        goto func_exit;
     }


    /* 禁用中断，以便锁定重入 */
    RTK_ENTER_CRITICAL(old);
    /* 如果任务没有启动，则启动 */
    if ( TASK_NOT_STARTUP == task->status ) {
        task->status = TASK_READY;
        READY_Q_PUT( task );
        RTK_EXIT_CRITICAL(old);
        need_schedule = 1;
        goto cleanup;
    }
    RTK_EXIT_CRITICAL(old);

cleanup:
    __rtk_task_mutex_sync_unlock();
    if (need_schedule ) {
        rtk_schedule();
    }
func_exit:
     return err;
}

int rtk_task_startup(struct rtk_task *task)
{
    int            os_started;

    /* 不允许在中断中调用 */
    if (rtk_is_int_context() ) {
        return -AW_EPERM;
    }

    if ( NULL == task ) {
        return -AW_EINVAL;
    }
    /* 不能自己操作自己 */
    os_started = __rtk_is_startup();
    if (os_started) {
        return _startup_os_has_startup(task);
    }
    else {
        return _startup_os_not_startup(task);
    }

}

int rtk_task_unfreeze( struct rtk_task *task )
{
    int                 err = AW_OK;
    RTK_CRITICAL_STATUS_DECL(old);
    int                 need_schedule = 0;

    /* 不允许在中断中调用 */
    if (rtk_is_int_context() ) {
        return -AW_EPERM;
    }

    if ( NULL == task ) {
        return -AW_EINVAL;
    }
    /* 不能自己操作自己 */
    if (rtk_task_self() == task) {
        return -AW_EPERM;
    }

    __rtk_task_mutex_sync_lock();

    /* 不能解冻不存在的任务 */
    if ( __rtk_task_check_exist(task) <=0 ) {
        __rtk_task_mutex_sync_unlock();
        err = -AW_EINVAL;
        goto func_exit;
    }

    /* 禁用中断，以便锁定重入 */
    RTK_ENTER_CRITICAL(old);
    /* 如果任务已经被冻结，则恢复 */
    if ( TASK_FROZE & task->status ) {
        task->status = TASK_READY;
        READY_Q_PUT( task );
        RTK_EXIT_CRITICAL(old);
        need_schedule = 1;
        goto cleanup;
    }
    else if ( TASK_FREEZING & task->status ) {
        /* 如果任务正在被冻结，则放弃冻结行为 */
        task->status &= (~TASK_FREEZING);
    }
    RTK_EXIT_CRITICAL(old);

cleanup:
    __rtk_task_mutex_sync_unlock();
    if (need_schedule ) {
        rtk_schedule();
    }
func_exit:
     return err;
}

void __task_wakup_with_err_ec(
        struct rtk_task *task,
        int err)
{
    task ->wake_reason |= err;
    if (TASK_FREEZING &task ->status) {
        task ->status = TASK_FROZE;
    }
    else if (TASK_READY != task->status) {
        task ->status = TASK_READY;
        READY_Q_PUT( task );
    }
}


void OS_SetTraceAPI(const OS_TRACE_API *p_trace)
{
    TRACEAPI_SET(p_trace);
}

#if CONFIG_RTK_OBJECT_CREATE_EN

#if CONFIG_RTK_USERMODE

#else
struct rtk_task *rtk_task_create(const char    *name,
                          int                   priority,
                          rtk_task_option_t     option,
                          unsigned int          stack_size,
                          void                 *pfunc,
                          void                 *arg1,
                          void                 *arg2)
{
    struct rtk_task            *p_task = NULL;
    struct rtk_task            *p_ret = NULL;
    char                       *p_stack = NULL;
    aw_err_t                    err = -AW_ENOMEM;

    /* 检查参数 */
    if (NULL == name || priority > RTK_MAX_PRIORITY || 0 == stack_size || NULL == pfunc) {
        return NULL;
    }

    /* alloc the tcb and task*/
    p_task = (struct rtk_task *)rtk_sys_mem_alloc(sizeof(*p_task) + stack_size );
    if (NULL == p_task) {
        goto cleanup;
    }
    p_stack = (((char *)p_task) + sizeof(*p_task));
    option |= ENUM_RTK_TASK_OPTION_CREATE;
    p_ret = rtk_task_init(p_task,
                          name,
                          priority,
                          option,
                          p_stack,
                          p_stack + stack_size,
                          pfunc,
                          arg1,
                          arg2);

cleanup:
    if (NULL == p_ret) {
        if (NULL != p_task) {
            rtk_sys_mem_free(p_task);
            p_task = NULL;
        }
    }
    return p_ret;
}
#endif

#if CONFIG_RTK_USERMODE

#else

void __rtk_task_release_created_task(struct rtk_task *task)
{
    /* 释放此任务的TCB和堆栈 */
    rtk_sys_mem_free(task);
}

#endif

#endif


/**
 *  \brief get current task_errno addr
 *
 *  \param errno's address
 */
int *rtk_task_errno_addr( void )
{
    struct rtk_task *task;

    if ( rtk_is_int_context() )
    {
        return NULL;
    }
    task = rtk_task_self();
#if CONFIG_NEWLIB_REENT_EN
    return &task->newlib_reent._errno;
#else
    {
        return &task->error_number;
    }
#endif
}


void *rtk_task_set_tls(struct rtk_task *p_task, void *data )
{
    OS_TRACE_TASK_SET_TLS_ENTER(p_task, data);

    if ( p_task )
    {
        void *old = p_task->thread_local_storage;
        p_task->thread_local_storage = data;

        OS_TRACE_TASK_SET_TLS_EXIT(old);
        return old;
    }

    OS_TRACE_TASK_SET_TLS_EXIT(0);
    return 0;
}

void *rtk_task_get_tls(struct rtk_task *p_task )
{
    OS_TRACE_TASK_GET_TLS_ENTER(p_task);

    if ( p_task )
    {
        OS_TRACE_TASK_GET_TLS_EXIT(p_task->thread_local_storage);
        return p_task->thread_local_storage;
    }

    OS_TRACE_TASK_GET_TLS_EXIT(NULL);
    return NULL;
}


/******************************************************************************/
#define RTK_STK_MAGIC32         0x55555555
int rtk_task_stack_check (
        struct rtk_task *p_task,
        unsigned int    *p_total,
        unsigned int    *p_free)
{
    uint32_t        *stk;

    if ( rtk_is_int_context() ) {
        return -AW_EPERM;
    }

    if (NULL == p_task || NULL == p_free) {
        return -AW_EINVAL;
    }

    /* get total stack size in bytes */
    if ((unsigned int *)0 != p_total) {
        *p_total = (char *)p_task->stack_high - (char *)p_task->stack_low;
    }

    /* get free stack size in bytes */
    if ((unsigned int *)0 != p_free) {
#ifdef RTK_ARCH_STK_GROW_UP
        for (stk = (void *)p_task->stack_high; RTK_STK_MAGIC == *stk; stk--) {
        }
        *p_free = (char *)p_task->stack_high - (char *)stk;
#else
        stk = (uint32_t *)p_task->stack_low;
        while(1) {
            if ( (RTK_STK_MAGIC32 != (*stk))
                    || ( ((uint32_t)stk ) >= ((uint32_t)p_task->stack_high ) ) ) {
                break;
            }
            stk ++;
        }

        *p_free = (char *)stk - (char *)p_task->stack_low;
#endif
    }

    return 0;
}



int rtk_enum_all_task_info(
        struct rtk_task_info *p_task_infos,
        int task_info_count,
        int *p_total_task_count)
{
    rtk_list_node_t        *p;
    struct rtk_task        *p_task;
    int                     total_task_count = 0;
    int                     output_task_count = 0;
    struct rtk_task_info   *p_task_info;

    if ( rtk_is_int_context() ) {
        return -AW_EPERM;
    }

    if (NULL == p_task_infos || 0 == task_info_count) {
        return -AW_EINVAL;
    }

    __rtk_task_mutex_sync_lock();

    rtk_list_for_each( p, &g_system_tasks_head )
    {
        p_task = RTK_LIST_ENTRY(p, struct rtk_task, task_list_node);
        if (total_task_count < task_info_count) {
            p_task_info = &p_task_infos[total_task_count];
            p_task_info->task_id = (uint32_t)p_task;
            strncpy(p_task_info->name,p_task->name,sizeof(p_task_info->name));
            p_task_info->stack_low = p_task->stack_low;
            p_task_info->stack_high = p_task->stack_high;
            p_task_info->sp   = p_task->sp;
            p_task_info->prio = p_task->waitq_prio;
#if CONFIG_RTK_PERFORMANCE_STATISTIC_EN
            p_task_info->total_running_time = p_task->total_running_time;
#else
            p_task_info->total_running_time = 0;
#endif
            p_task->stack_deep = (p_task->stack_deep > p_task->sp) ? \
                                  p_task->sp : p_task->stack_deep;
            p_task_info->stack_deep = p_task->stack_deep;
            
            output_task_count ++;
        }
        total_task_count ++;
    }

    __rtk_task_mutex_sync_unlock();

    if (NULL != p_total_task_count) {
        *p_total_task_count = total_task_count;
    }

    return output_task_count;
}
