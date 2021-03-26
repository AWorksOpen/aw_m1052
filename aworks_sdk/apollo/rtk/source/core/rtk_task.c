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
 * \brief rtk taskʵ��
 *
 * \par Դ����
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

    /* ���һ��tick ���� */
    __task_set_wakeup_after_tick(task_current,tick);

    /* ����ϵͳ�ٽ��� */
    RTK_ENTER_CRITICAL(old);
    /* ȷ�����ڼ�����û�б�systick���� */
    if (!rtk_list_empty(&task_current->tick_node.node) ) {
        task_current->status = TASK_DELAY;
        /* ������Ӿ����������Ƴ� */
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

    /* �˿��Ѿ�����ϵͳ�ٽ��� */
    /* �������pending�����Ƴ� */
    __task_detach_pending_waitq_ec(task);
    __task_wakup_with_err_ec(task,RTK_TASK_WAKE_BY_TIMEOUT);
}

unsigned int _task_cal_suit_readyq_prio_ec(struct rtk_task *p_task)
{
    unsigned int        new_readyq_prio;

    new_readyq_prio = p_task->readq_prio;
    /* ���û����Ϊbase mutex���������ȼ��� */
    /* ��readyqӦ�ø���waitq_prio*/
    if (p_task->base_mutex_need_prio >= RTK_MAX_PRIORITY) {
        new_readyq_prio = p_task->waitq_prio;
    }
    else {
        /* �����Ϊbase mutex���������ȼ������µ�readyq_prio���ܵ���waitq_prio */
        /* ��������ȼ������ͣ����µ�readyq_prio���ܵ���base_mutex_need_prio */
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
    /* ���������ж��е��� */
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

    /* ���ܲ��������ڵ����� */
    if ( __rtk_task_check_exist(task) <=0) {
        __rtk_task_mutex_sync_unlock();
        err = -AW_EINVAL;
        goto func_exit;
     }

    /* �����жϣ��Ա��������� */
    RTK_ENTER_CRITICAL(old);
    /* ����������ȼ���Ҫ���õ����ȼ���ͬ��Ҳ��Ϊ���óɹ� */
    /* ���ǾͲ��ؽ�������Ĳ����� */
    if (task->waitq_prio == new_priority) {
        RTK_EXIT_CRITICAL( old );
        goto cleanup;
    }

#if CONFIG_RTK_MUTEX_EN
    task->hold_mutex_org_priority = new_priority;
    if ( new_priority < task->waitq_prio ) {
        /* �����������ȼ��������ֱ����� */
        task->waitq_prio = new_priority;
    }
    else {
        /* ����ǽ������ȼ� */
        prio_mutex = __task_hold_mutex_priority_get( task );
        if (prio_mutex >= new_priority) {
            /* ��Ҫ���͵������ȼ� ���� ���ڵȴ� */
            /* �����������л��������������������ȼ������������ȼ�ʱ */
            /* ���Խ��͵��µ����ȼ� */
            task->waitq_prio = new_priority;
        }
        else {
            /* ����ֻ�ܽ��͵�������л�������������ȼ� */
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

    /* ��֤�����ȼ�������ִ�� */
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
    /* ��������ӵ����������б� */
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

    /* ���������ж��е��� */
    if (rtk_is_int_context() ) {
        return NULL;
    }

    if (NULL == task || NULL== name || NULL == pfunc) {
        return NULL;
    }
    /* ��ֹ��ջ��С */
    if ( ((uint32_t)stack_high) <= ((uint32_t)stack_low) ) {
        return NULL;
    }

    if ( ((uint32_t)stack_high) - ((uint32_t)stack_low) <= 32 ) {
        return NULL;
    }

    /* ȷ���������е����񲻻ᱻ���³�ʼ�� */
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
    /* ���������ж��е��� */
    if (rtk_is_int_context() ) {
        return -AW_EPERM;
    }

    /*  ��ȡȫ��������rtk_task_terminateͬ�� */
    RTK_ENTER_CRITICAL(old);

    /* ������unsafe���ô������� */
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

    /* ���������ж��е��� */
    if (rtk_is_int_context() ) {
        return -AW_EPERM;
    }

    p_cur_task = rtk_task_self();
    /*  ��ȡȫ��������rtk_task_terminateͬ�� */
    RTK_ENTER_CRITICAL(old);

    /* ������unsafe���ô������� */
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

    /* ���������ж��е��� */
    if (rtk_is_int_context() ) {
        return -AW_EPERM;
    }

    if (NULL == task) {
        return -AW_EINVAL;
    }

    if ( task == rtk_task_self() ) {
        return -AW_EPERM;
    }

    /* ���Ҫ�����������Ч�� */
    __rtk_task_mutex_sync_lock();
    if (!__rtk_task_check_exist(task) ) {
        __rtk_task_mutex_sync_unlock();
        return -AW_EINVAL;
    }

    /* ��Ϊ�Ѿ�������__g_system_tasks_head_lock */
    /* ����Ŀ�����񲻿����ڴ��ڼ��Զ��˳��� */

    /* ��������Ƿ���safe״̬ */
    RTK_ENTER_CRITICAL(old);
    if (task->safe_count) {
        /* �����ڲ�������ֹ״̬ */
        RTK_EXIT_CRITICAL(old);
        __rtk_task_mutex_sync_unlock();
        return -AW_EPERM;
    }
    /* �����жϣ�ʹ�жϿ���������Ӧ */
    RTK_EXIT_CRITICAL(old);

    /* �������ϵͳ�����������Ƴ�����ֹ���������ٲ���Ŀ��task */
    rtk_list_del_init(&task->task_list_node);

    /* ��Ŀ�����񶳽�,���Ŀ����������ִ�� */
    /* ��Ŀ������ͣ����û�г���systick����task_mutex����λ�� */
    __rtk_task_freeze(task,1);

    /* ������ܴ��ڵ�delay node */
    __task_unset_wakeup_after_tick( task);

    RTK_ENTER_CRITICAL(old);
    /* �����жϣ��Ƴ�pending��Դ */
#if CONFIG_SEMB_EN||CONFIG_SEMC_EN
    __task_detach_pending_waitq_ec(task);
#endif
    /* ���ˣ�Ҫ���������񲻻��ٱ����ȵ�����Ϊtick_node��penging��Դ���Ѿ��ͷ� */
    /* ����Ҳ���ᱻ�����жϻ���������ʱʱ�䵽���ź������źŵȵķ�ʽ���� */
    /* ��ʱ���Դ��жϣ��Ա���ٳ�ʱ��ر��жϴ������ж��ӳ� */
    RTK_EXIT_CRITICAL(old );

    /* Ҫ�����������������join��������*/
    RTK_ENTER_CRITICAL(old);
    task_cleanup_join_other_task(task);
    RTK_EXIT_CRITICAL(old);

#if CONFIG_RTK_MUTEX_EN
    /* release all mutex. */
    /* ��Ϊtask��Ӧ�������ʱ������������ж���˵�Ѿ�����Ч״̬ */
    /* ����������ж�Ҳ�����ٷ���task��tcb�����Դ�ʱ��task��mutex_holded */
    /* ����ķ����ǰ�ȫ��,task�Լ�Ҳ�Ѿ�ֹͣ���� */
    rtk_list_for_each_safe(p, save, &task->mutex_holded_head){
        struct rtk_mutex       *p_mutex;
        p_mutex = MUTEX_HOLD_NODE_TO_MUTEX( p );
        /* ����mutex���������Ա�����pender������-AW_EXNIO */
        /* ͬʱ���Ժ��lock������ʧ�ܣ�����ֹ�Թ�����Դ�Ĳ���Ӱ�� */
        rtk_mutex_terminate(p_mutex);
    }
#endif

    p_who_join_me = task->p_who_join_me;
    /* �ͷŵ�task_mutex�����Ա�����ȼ�������Բ���mutex����������API */
    __rtk_task_mutex_sync_unlock();

#if CONFIG_RTK_OBJECT_CREATE_EN
    /* �ͷ�Ҫ����������ռ�õ��ڴ�ռ� */
    /* ����������Ǳ���̬�����������ͷ�ջ */
    if (task->option & ENUM_RTK_TASK_OPTION_CREATE) {
        __rtk_task_release_created_task(task);
    }
#endif

    /* ���������������join Ҫ���������� */
    if ( NULL  != p_who_join_me ) {
        RTK_ENTER_CRITICAL(old);
        __rtk_task_wakeup_joining_task(p_who_join_me,(void *)-AW_EPIPE);
        RTK_EXIT_CRITICAL(old);
    }

    /* ִ��һ�ε��ȣ�ȷ��joinҪ�������������������ȼ��ȵ�ǰ����ߣ����Ա� */
    /* ��ȷִ�� */
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

    /* ��Ϊ���Զ��˳�����ǰ���񲻿�����join�������� */


    /* �Զ��˳������񲻻����delay���� */
    /* ���Բ�����delay node */

    /* �ͷ��Ѿ�ӵ�е�mutex,��ʵ˵���ǲ��뱻ִ�еĴ��� */
    /* ��Ϊ��������ζ�Ŵ������������δ�ͷŵĻ����� */
    /* ��������صĴ����߼��� */

#if CONFIG_RTK_MUTEX_EN
    rtk_list_for_each_safe(p, save, &task->mutex_holded_head) {
        struct rtk_mutex *p_mutex;
        p_mutex = MUTEX_HOLD_NODE_TO_MUTEX( p );
        /* terminate �����е�mutex��ʹ������������Ҳ���ܷ��ʱ�mutex��������Դ�� */
        /* ����ԱӦ��Ҫ��ס���˳�����֮ǰ����unlock����mutex */
        rtk_mutex_terminate( p_mutex );
    }
#endif
    /*���task_mutex������ȡ��������������ʱ���ܷ����µı� */
    /*task_mutex�����Ĳ�����������ѯ�����Ƿ���Ч */
    __rtk_task_mutex_sync_lock();
    RTK_ENTER_CRITICAL(old);
    /* �Զ��˳������񲻻�pending��ĳ���ź����� */
    /* ������ʱ�򽫱�����������б����Ƴ��� */
    task->status = TASK_DEAD;
    READY_Q_REMOVE( task );

    /* �ѵ�ǰ�����ϵͳ�����������Ƴ� */
    /* �������������������terminate�ͻᷢ�ֵ�ǰ�����Ѿ���Ч */
    /* �Ͳ����ٲ����� */
    rtk_list_del_init( &task->task_list_node );

    if (task->option & ENUM_RTK_TASK_OPTION_CREATE) {
#if CONFIG_RTK_OBJECT_CREATE_EN
        /* sem,muxtex�����ͷţ���������Ҳ���ᱻ���ȵ��� */
        /* ��ʣ��tcb��ջû�б��ͷ� */
        /* ��tcb��ջ����idle���ͷţ�����join�����������Ҳ����idle����� */
        task->error_number = (int)ret;
        __rtk_add_idle_cleanup_task_ec(task);
#endif
    }
    else {
        /* ����һ����̬���񣬲���Ҫ�ͷ�tcb��ջ,����join����������� */
        if (NULL != task->p_who_join_me) {
            __rtk_task_wakeup_joining_task(task->p_who_join_me,ret);
        }
    }

    __rtk_task_mutex_sync_unlock_no_sche();
    /* ��ʱ���жϣ����Խ����ж��ӳ�ʱ�䣬�����ж������������ */
    /* ��ǰ��������ִ�� */
    RTK_EXIT_CRITICAL(old);
    /* �������û���ж�Ҫ����������ֹ���������� */
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
        /* �����ʱ����Ҫִ��һ��syscall */
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

    /* ������join�Լ���Ҳ���������ж��н��д˲��� */
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
    /* ������join����ϵͳ�е����� */
    if (!__rtk_task_check_exist(task) ) {
        __rtk_task_mutex_sync_unlock();
        ret = -AW_EINVAL;
        goto func_exit;
    }

    /* ��ֹ���������ٲ�����task */

    RTK_ENTER_CRITICAL(old);
    /* �Ѿ���join���������ٱ�join */
    if (NULL !=task -> p_who_join_me) {
        RTK_EXIT_CRITICAL(old);
        __rtk_task_mutex_sync_unlock();
        ret = -AW_EBUSY;
        goto func_exit;
    }
    /* ����Ŀ���������ڱ�join */
    task->p_who_join_me = task_current;
    task_current->p_I_join_who = task;
    RTK_EXIT_CRITICAL(old);
    __rtk_task_mutex_sync_unlock();

    /* ���õ�ǰ����Ϊ�Ǿ���״̬ */
    RTK_ENTER_CRITICAL(old);
    /* ����ϴ��ж����õ������ڼ䣬Ŀ�������Ѿ����� */
    /* ���ý���ǰ�����Ƴ������������ */
    if (NULL != task_current->p_I_join_who) {
        /* ���Լ��Ӿ����������Ƴ� */
        task_current->status = TASK_JOINING;
        READY_Q_REMOVE( task_current );
        need_schedule = 1;
    }
    /* �˿��Ѿ����Խ����ж��� */
    RTK_EXIT_CRITICAL(old);
    /* ��ʱ��������жϣ����ȥִ���жϣ��жϴ��������Զ���ѡһ�����ȼ� */
    /* ��ߵ����񣬴��������һ�����㲻��ִ�� */
    /* ֱ��join����������󣬴��������������ʼִ����һ����� */

    /* ��������ж�û����������Ϊ�������Ѿ����پ��� */
    /* ���Ա���Ҫ����һ���������ȣ�ִ��ʣ�����ȼ���ߵ����� */

    /* ���Խ�������Ҫ��������һ�ε��ȣ��������һ�䵽��һ���ڼ�û���жϷ��� */
    /* ������Ǳ���ģ�������У���ִ��һ�ε���Ҳ������Ӱ�죬 */
    /* ��Ϊ��ǰ�������ȼ���ߵ�����ִ�кܿ� */

    if (need_schedule) {
        rtk_schedule();
    }

    /* ������Ѿ�����ȷ����ǰ������Ϊjoin������������� */
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
        /* �������ھ���״̬����Ҫ�����Ƿ�ȷ����������ִ�е����� */
        /* ��Ϊ��������ִ�е�������ܻ��������Ҫ�Ľ�� */
        /* һ����ڹػ�ʱ�Ŷ�������ִ�е����񣬹���ʱ���ܶ�������ִ�е����� */
        if (freeze_running_task) {
            READY_Q_REMOVE( task );
            task->status = TASK_FROZE;
            OS_TRACE_TASK_STOP_READY(task, task->status);
            RTK_EXIT_CRITICAL(old);
            RTK_SYSTICK_UNLOCK(systick_lock);
            /* ���ﲻ��Ҫִ�е��ȣ���Ϊ��ǰ�������ȼ�������Ϊyield��������� */
            /* ��������Ҳ����Ҫ�л����Ǹ����ߵ����ȼ� */
            /* �ȴ��´�systick��������Ȼ���л� */
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
        /* ���̴߳���pending����delay״̬ */
        /* �����߳������ڶ����б�־�����߳���ͼ��Ϊready״̬��ʱ����Զ�froze */
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

   /* ���������ж��е��ã�Ҳ���������Լ� */
   if (rtk_is_int_context() || task == rtk_task_self()) {
       err = -AW_EPERM;
       goto func_exit;
   }

    if ( NULL == task ) {
        err = -AW_EINVAL;
        goto func_exit;
    }

    /* ��������Ƿ������� */
    if ((ENUM_RTK_TASK_OPTION_CAN_NOT_BE_FROZE & task ->option )) {
        err = -AW_EPERM;
        goto func_exit;
    }

    __rtk_task_mutex_sync_lock();
    /* ��������Ƿ������ϵͳ�� */
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

    /* ��������Ƿ������ϵͳ�� */
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

    /* �������������ڵ����� */
    if ( __rtk_task_check_exist(task) <=0) {
        __rtk_task_mutex_sync_unlock();
        err = -AW_EINVAL;
        goto func_exit;
     }


    /* �����жϣ��Ա��������� */
    RTK_ENTER_CRITICAL(old);
    /* �������û�������������� */
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

    /* ���������ж��е��� */
    if (rtk_is_int_context() ) {
        return -AW_EPERM;
    }

    if ( NULL == task ) {
        return -AW_EINVAL;
    }
    /* �����Լ������Լ� */
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

    /* ���������ж��е��� */
    if (rtk_is_int_context() ) {
        return -AW_EPERM;
    }

    if ( NULL == task ) {
        return -AW_EINVAL;
    }
    /* �����Լ������Լ� */
    if (rtk_task_self() == task) {
        return -AW_EPERM;
    }

    __rtk_task_mutex_sync_lock();

    /* ���ܽⶳ�����ڵ����� */
    if ( __rtk_task_check_exist(task) <=0 ) {
        __rtk_task_mutex_sync_unlock();
        err = -AW_EINVAL;
        goto func_exit;
    }

    /* �����жϣ��Ա��������� */
    RTK_ENTER_CRITICAL(old);
    /* ��������Ѿ������ᣬ��ָ� */
    if ( TASK_FROZE & task->status ) {
        task->status = TASK_READY;
        READY_Q_PUT( task );
        RTK_EXIT_CRITICAL(old);
        need_schedule = 1;
        goto cleanup;
    }
    else if ( TASK_FREEZING & task->status ) {
        /* ����������ڱ����ᣬ�����������Ϊ */
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

    /* ������ */
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
    /* �ͷŴ������TCB�Ͷ�ջ */
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
