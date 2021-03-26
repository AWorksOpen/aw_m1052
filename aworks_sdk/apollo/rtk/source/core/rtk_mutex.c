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
#include "rtk_priority_q_internal.h"
#include "aw_errno.h"
#include "aw_common.h"
#include <string.h> /* for memset() and memcpy() */
#include "rtk_waitq_internal.h"
#include "rtk_mutex_internal.h"
#include "rtk_base_mutex.h"
#include "rtk_mutex_sync_lock.h"
#include "rtk_task_internal.h"
#include "os_trace_events.h"

#if CONFIG_RTK_MUTEX_EN

#define MUTEX_MAGIC_NUM_VALID           0
#define MUTEX_MAGIC_NUM_TERMINATING     0x45424d49U

static int  __mutex_owner_set(
        struct rtk_mutex *p_mutex,
        struct rtk_task *task );
static void  __mutex_owner_unset(struct rtk_mutex *p_mutex );
static int __restore_task_priority (struct rtk_task *p_task);

/**
 * \brief 释放互斥锁
 * 释放互斥锁
 *
 * \param[in] p_mutex       要操作的互斥量
 * \retval                  是否成功唤醒了一个任务
 */
static int __mutex_release( struct rtk_mutex *p_mutex);

static int _mutex_init_os_not_startup( struct rtk_mutex *p_mutex )
{
    int     err = AW_OK;

    OS_TRACE_MUTEX_INIT(p_mutex);

    /*  预防在中断中调用 */
    if ( unlikely(rtk_is_int_context()) ) {
        err = -AW_EPERM;
        goto cleanup;
    }
    if (NULL == p_mutex) {
        err = -AW_EINVAL;
        goto cleanup;
    }

    /*  已经初始化? */
    if (MUTEX_MAGIC_NUM_VALID ==
            (p_mutex->magic_num + (unsigned long)p_mutex ) ) {
        __rtk_task_mutex_sync_unlock();
        err = -AW_EEXIST;
        goto cleanup;
    }

    /*  正在terminate ? */
    if (MUTEX_MAGIC_NUM_TERMINATING ==
            (p_mutex->magic_num + (unsigned long)p_mutex ) ) {
        __rtk_task_mutex_sync_unlock();
        err = -AW_EEXIST;
        goto cleanup;
    }

    /*  执行初始化 */
    __rtk_waitq_init(
            &p_mutex->wait_queue,
            RTK_WAIT_QUEUE_TYPE_MUTEX,
            RTK_WAIT_QUEUE_OPTION_PRIORITY);
    rtk_list_node_init( &p_mutex->mutex_hold_node );
    p_mutex->mutex_recurse_count  = 0;
    p_mutex->owner = NULL;
    p_mutex->magic_num = MUTEX_MAGIC_NUM_VALID - (unsigned long)p_mutex;

cleanup:
    return err;
}

int _mutex_init_os_startup( struct rtk_mutex *p_mutex )
{
    int   err = AW_OK;

    OS_TRACE_MUTEX_INIT(p_mutex);

    /*  预防在中断中调用 */
    if ( unlikely(rtk_is_int_context()) ) {
        err = -AW_EPERM;
        goto cleanup;
    }
    if (NULL == p_mutex) {
        err = -AW_EINVAL;
        goto cleanup;
    }

    __rtk_task_mutex_sync_lock();
    /*  已经初始化? */
    if (MUTEX_MAGIC_NUM_VALID ==
            (p_mutex->magic_num + (unsigned long)p_mutex ) ) {
        __rtk_task_mutex_sync_unlock();
        err = -AW_EEXIST;
        goto cleanup;
    }

    /*  正在terminate ? */
    if (MUTEX_MAGIC_NUM_TERMINATING ==
            (p_mutex->magic_num + (unsigned long)p_mutex ) ) {
        __rtk_task_mutex_sync_unlock();
        err = -AW_EEXIST;
        goto cleanup;
    }

    /*  执行初始化 */
    __rtk_waitq_init(
            &p_mutex->wait_queue,
            RTK_WAIT_QUEUE_TYPE_MUTEX,
            RTK_WAIT_QUEUE_OPTION_PRIORITY);
    rtk_list_node_init( &p_mutex->mutex_hold_node );
    p_mutex->mutex_recurse_count  = 0;
    p_mutex->owner = NULL;
    p_mutex->magic_num = MUTEX_MAGIC_NUM_VALID - (unsigned long)p_mutex;

    __rtk_task_mutex_sync_unlock();
cleanup:
    return err;
}

int rtk_mutex_init( struct rtk_mutex *p_mutex )
{
    if (__rtk_is_startup()) {
        return _mutex_init_os_startup(p_mutex);
    }
    else {
        return _mutex_init_os_not_startup(p_mutex);
    }
}

int rtk_mutex_lock( struct rtk_mutex *p_mutex, unsigned int tick )
{
    RTK_CRITICAL_STATUS_DECL(old);
    int                     ret = AW_OK;
    struct rtk_task        *task_current = rtk_task_self();
    int                     need_schedule = 0;

    OS_TRACE_MUTEX_LOCK_ENTER(p_mutex, tick);

    /*  预防在中断中调用 */
    if ( unlikely(rtk_is_int_context()) ) {
        ret = -AW_EPERM;
        goto func_exit;
    }

    if (NULL == p_mutex) {
        ret = -AW_EINVAL;
        goto func_exit;
    }
    __rtk_task_mutex_sync_lock();

    /*  检查有效性 */
    if (MUTEX_MAGIC_NUM_VALID !=
            (p_mutex->magic_num + (unsigned long)p_mutex ) ) {
        __rtk_task_mutex_sync_unlock();
        ret = -AW_EINVAL;
        goto cleanup;
    }

    if ( p_mutex->owner == NULL ) {
        /*  成功 */
        __mutex_owner_set( p_mutex, task_current );
        p_mutex->mutex_recurse_count = 1;
        __rtk_task_mutex_sync_unlock();
        goto cleanup;
    } else if ( p_mutex->owner == task_current ) {
        /*  递归 */
        if (((unsigned long) (~0) ) != p_mutex->mutex_recurse_count ) {
            p_mutex->mutex_recurse_count++;
            __rtk_task_mutex_sync_unlock();
        }
        else {
            __rtk_task_mutex_sync_unlock();
            ret = -AW_ENOSPC;
        }

        goto cleanup;
    }

#if CONFIG_DEAD_LOCK_DETECT_EN
    else if ( __mutex_dead_lock_detected( p_mutex ) ) {
#ifdef DEAD_LOCK_HOOK
        DEAD_LOCK_HOOK(p_mutex, p_mutex->s.u.owner );
#endif
#if CONFIG_DEAD_LOCK_SHOW_EN
        __mutex_dead_lock_show( p_mutex );
#endif

        OS_TRACE_MUTEX_LOCK_EXIT(-AW_EDEADLK);

        return -AW_EDEADLK;/* Deadlock condition */
    }
#endif

    if ( tick == 0 ) {
        __rtk_task_mutex_sync_unlock();
        ret = -AW_EAGAIN;
        goto cleanup;
    }

    RTK_ENTER_CRITICAL(old);
    __task_pend_on_waitq_ec(
            task_current,
            &p_mutex->wait_queue);
    RTK_EXIT_CRITICAL(old);

    /*  如果当前任务是mutex的pend list优先级最高的任务 */
    /*  则说明mutex pend list的最高优先级发生了改变 */
    /*  这个时候就需要提升mutex owner的优先级了 */
    if (task_current ==
            __rtk_waitq_highest_pender_get_lc(&p_mutex->wait_queue) ) {

            __mutex_raise_owner_priority(
                    p_mutex,
                    task_current->waitq_prio );
            need_schedule = 1;

    }

    if (AW_WAIT_FOREVER != tick) {
        __task_set_wakeup_after_tick(task_current,tick);
    }

    RTK_ENTER_CRITICAL(old);
    /*  执行到此处，表明当前任务可能需要pending */
    /*  但是必须要考虑到的情况是此刻超时时间可能已经到了 */
    if ( (AW_WAIT_FOREVER == tick) ||
            (!rtk_list_empty(&task_current->tick_node.node) ) ){
        /*  如果是永久等待，或者超时时间还没到 */
        /*  设置任务处于合适的状态 */
        if (AW_WAIT_FOREVER == tick) {
            task_current->status = TASK_PENDING;
        }
        else {
            task_current->status = TASK_PENDING | TASK_DELAY;
        }
        /*  将自己从就绪队列上移除 */
        READY_Q_REMOVE( task_current );
        need_schedule = 1;
        __rtk_task_mutex_sync_unlock_no_sche();
        /*  此刻已经可以解锁中断了 */
        RTK_EXIT_CRITICAL(old);
    }
    else {
        /*  超时时间已经到了 */
        /*  此刻已经可以解锁中断了 */
        RTK_EXIT_CRITICAL(old);
        __rtk_task_mutex_sync_unlock();
    }



    /*
     * 此时如果发生中断，则会去执行中断，中断处理完后会自动挑选一个优先级
     * 最高的任务，此任务的下一句代码便不会执行
     * 直到mutex unlock或是terminate后，此任务便会就绪，开始执行下一句代码
     *
     * 但是如果中断没发生，则因为此任务已经不再就绪
     * 所以必须要发起一次主动调度，执行剩下优先级最高的任务
     *
     * 所以接下来需要主动发起一次调度，如果从上一句到这一句期间没有中断发生
     * 则调度是必须的，而如果有，再执行一次调度也不会有影响，
     * 因为当前任务优先级最高调度器执行很快
     */
    if (need_schedule) {
        rtk_schedule();
    }

    if (AW_WAIT_FOREVER != tick) {
        __task_unset_wakeup_after_tick(task_current);
    }


    /*  直接的lock没有成功，则考虑任务不再就绪后 */
    /*  被唤醒的原因 */
    ret = __task_get_wake_reason(task_current);

    /*  如果lock 操作没有成功 */
    /*  可能需要将已经提升的owener优先级还原 */
cleanup:
func_exit:
    OS_TRACE_MUTEX_LOCK_EXIT(err);
    return ret;
}

int rtk_mutex_unlock( struct rtk_mutex *p_mutex )
{
    struct rtk_task    *task_current;
    int                 err = AW_OK;
    int                 need_schedule = 0;

    OS_TRACE_MUTEX_UNLOCK_ENTER(p_mutex);
    /*  预防在中断中调用 */
    if ( unlikely(rtk_is_int_context()) ) {
        err = -AW_EPERM;
        goto func_exit;
    }

    if (NULL == p_mutex) {
        err = -AW_EINVAL;
        goto func_exit;
    }

     task_current = rtk_task_self();

     __rtk_task_mutex_sync_lock();
     /*  检查有效性 */
    if (MUTEX_MAGIC_NUM_VALID !=
            (p_mutex->magic_num + (unsigned long)p_mutex ) ) {
        __rtk_task_mutex_sync_unlock();
        err = -AW_EINVAL;
        goto func_exit;
    }

    if ( p_mutex-> owner != task_current ) {
        __rtk_task_mutex_sync_unlock();
        err = -AW_EPERM;
        goto func_exit;
    }

    if ( --p_mutex->mutex_recurse_count ) {
        __rtk_task_mutex_sync_unlock();
        goto func_exit;
    }

    need_schedule = __mutex_release( p_mutex);

    /*
     *  恢复优先级继承而来的优先级
     *  这时才恢复优先级其实并不影响实际上比当前任务优先级高的任务的实时性
     *  因为优先级提升，造成了当前任务优先级高于那些任务，从而先执行
     *  如果前次启用中断后处理了中断，因此造成了更高优先级任务的执行
     *  这是高优先级任务先执行，并不影响实时性
     *  如果处理了中断，但是没有更高优先级任务执行，或是没有中断，
     *  这时恢复优先级相对于前面不启用中断，执行的时间并无多少差别
     *  且也是当前任务先恢复优先级，实际上比当前任务优先级高的任务后执行
     *  但是这样处理，却可以降低中断延迟时间
     */
    need_schedule |= __restore_task_priority(task_current);
    __rtk_task_mutex_sync_unlock();

    /*  有中断则由中断负责调度，没有则主动调度也保证高优先级任务先执行 */
    if (need_schedule) {
        rtk_schedule();
    }
func_exit:
     OS_TRACE_MUTEX_UNLOCK_EXIT(err);
    return err;
}


int rtk_mutex_terminate( struct rtk_mutex *p_mutex )
{
    int                 err = AW_OK;
    int                 need_schedule = 0;
    struct rtk_task    *p_owner;

    OS_TRACE_MUTEX_TERMINATE_ENTER(p_mutex);
    /*  预防在中断中调用 */
    if ( unlikely(rtk_is_int_context()) ) {
        err = -AW_EPERM;
        goto func_exit;
    }

    if (NULL == p_mutex) {
        err = -AW_EINVAL;
        goto func_exit;
    }

    /*  锁定中断 */
    __rtk_task_mutex_sync_lock();
    /*  检查有效性 */
    if (MUTEX_MAGIC_NUM_VALID !=
            (p_mutex->magic_num + (unsigned long)p_mutex ) ) {
        __rtk_task_mutex_sync_unlock();
        err = -AW_EINVAL;
        goto cleanup;
    }
    /*  mutex有效，设置为正在terminate */
    /*  这样可以阻止其它任务对此mutex进行操作 */
    p_mutex->magic_num = MUTEX_MAGIC_NUM_TERMINATING - (unsigned long)p_mutex;
    p_owner = p_mutex->owner;
    if (NULL != p_owner) {
        __mutex_owner_unset( p_mutex);
        need_schedule |= __restore_task_priority(p_owner);
    }
    __rtk_task_mutex_sync_unlock();

    /* 执行真正的terminate操作 */
    err = __rtk_waitq_terminate(&p_mutex->wait_queue);
    __rtk_task_mutex_sync_lock();
    memset(&p_mutex->wait_queue,0,sizeof(p_mutex->wait_queue));
    memset(&p_mutex->mutex_hold_node,0,sizeof(p_mutex->mutex_hold_node));
    p_mutex->owner = NULL;
    p_mutex->mutex_recurse_count = 0;
    p_mutex->magic_num = 0;
    __rtk_task_mutex_sync_unlock();
cleanup:
func_exit:
    if (need_schedule) {
        rtk_schedule();
    }
    OS_TRACE_MUTEX_TERMINATE_EXIT(err);
    return err;
}


#if CONFIG_DEAD_LOCK_DETECT_EN
static int __mutex_dead_lock_detected( struct rtk_mutex *p_mutex )
{
    struct rtk_task *powner;
    struct rtk_mutex *s = p_mutex;
    struct rtk_task *task_current = rtk_task_self();

    powner = s->s.u.owner;
again:
    if ( powner->status & TASK_PENDING ) {
        s = (struct rtk_mutex *)PLIST_PTR_TO_SEMID( powner->pending_resource );
        if ( s->s.type == SEM_TYPE_MUTEX ) {
            powner = s->s.u.owner;
            if ( powner == task_current  )
                return 1;
            goto again;
        }
    }
    return 0;
}


#if CONFIG_DEAD_LOCK_SHOW_EN
void __mutex_dead_lock_show( struct rtk_mutex *mutex )
{
    struct rtk_task *powner;
    struct rtk_mutex *s = mutex;
    struct rtk_task *task_current = rtk_task_self();
    powner = s->s.u.owner;

    kprintf("Dead lock path:\n task %s pending on 0x%08X", task_current->name, mutex);
again:
    kprintf(", taken by %s", powner->name );
    if ( powner->status & TASK_PENDING ) {
        s = (struct rtk_mutex *)PLIST_PTR_TO_SEMID( powner->pending_resource );
        kprintf(", and pending on 0x%08X ", s );
        if ( s->s.type == SEM_TYPE_MUTEX ) {
            powner = s->s.u.owner;
            if ( powner == task_current ) {
                kprintf(", taken by %s\n", powner->name );
                return;
            }
            goto again;
        }
    }
    kprintf(".\n");
}
#endif
#endif


static int __mutex_wakeup_a_pender(
        struct rtk_mutex *p_mutex)
{
    struct rtk_task        *task;
    RTK_CRITICAL_STATUS_DECL(old);

    RTK_ENTER_CRITICAL(old);
    task = __rtk_waitq_pop_1_pender_ec(&p_mutex->wait_queue);
    RTK_EXIT_CRITICAL(old);

    if (NULL == task) {
        return 0;
    }

    __mutex_owner_set( p_mutex, task );
    p_mutex->mutex_recurse_count = 1;
    RTK_ENTER_CRITICAL(old);
    __task_wakup_with_err_ec(task,RTK_TASK_WAKE_BY_WAITQ);
    RTK_EXIT_CRITICAL(old);
    return 1;
}

int __mutex_release( struct rtk_mutex *p_mutex )
{
    __mutex_owner_unset( p_mutex );
    return __mutex_wakeup_a_pender(p_mutex);
}


int __task_hold_mutex_priority_get ( struct rtk_task *task )
{
    struct rtk_mutex *p_mutex;

    if ( !rtk_list_empty(&task->mutex_holded_head) ) {
        p_mutex = MUTEX_HOLD_NODE_TO_MUTEX( RTK_LIST_FIRST(&task->mutex_holded_head) );
        return __rtk_waitq_highest_pri_get_lc(
                    &p_mutex->wait_queue);
    }
    return RTK_MAX_PRIORITY+1;
}

static int  __mutex_owner_set(
        struct rtk_mutex *p_mutex,
        struct rtk_task *task )
{
    rtk_list_node_t        *p;
    int                     pri;
    struct rtk_mutex       *p_tmp_mutex;

    p_mutex->owner = task;

    pri = __rtk_waitq_highest_pri_get_lc(
                &p_mutex->wait_queue);
    rtk_list_for_each( p, &task->mutex_holded_head) {
        p_tmp_mutex = MUTEX_HOLD_NODE_TO_MUTEX( p );
        if ( (__rtk_waitq_highest_pri_get_lc
                ( &p_tmp_mutex->wait_queue)  )
                >= pri )  {
            break;
        }
    }

    rtk_list_add_tail( &p_mutex->mutex_hold_node, p );
    return 0;
}

static void  __mutex_owner_unset(
        struct rtk_mutex *p_mutex )
{
    p_mutex->owner = NULL;
    rtk_list_del_init( &p_mutex->mutex_hold_node );
}


static int __resort_hold_mutex_list_and_trig(
        struct rtk_mutex *p_mutex,
        struct rtk_task *task )
{
    int pri;
    __mutex_owner_unset(p_mutex);
    pri = __task_hold_mutex_priority_get(task);
    __mutex_owner_set(p_mutex, task);
    return pri > __task_hold_mutex_priority_get(task);
}

int __mutex_raise_owner_priority(
        struct rtk_mutex *p_mutex,
        int new_priority )
{
    int                 ret = 0;
    struct rtk_task    *p_owner;
    rtk_wait_queue_t   *p_wait_queue;
    RTK_CRITICAL_STATUS_DECL(old);
    int                 highest,is_mutex;
    unsigned int        readyq_prio;

    p_owner   = p_mutex->owner;

    /*
     * 此函数在改变任务优先级的函数中调用
     * 因为任务优先级改变了
     * 导致此任务所pend的互斥锁的pend list的最高优先级也改变了
     * 这时就需要改变此任务锁pend的mutex的owner的优先级了
     * 因为此任务所pend的互斥锁的pend list的最高优先级改变了
     * 而任务的持有互斥锁链表是按照pend在互斥锁任务的最高优先级来排序
     * 所以，此时重新排列此互斥锁在拥有者任务的持有互斥锁链表中的位置
     * 便成了必须的选项了
    */
    while ( __resort_hold_mutex_list_and_trig(p_mutex,p_owner) &&
            p_owner->waitq_prio > new_priority ) {
        /*
         * 如果p_mutex成为p_owner持有mutex中优先级最高的一个
         * 则表明p_owner的优先级可能需要改变
         * 再加上p_owner低于pend在p_mutex上任务的最高有效级
         * 则必须要改变p_owner的优先级了
         */
        ret++;
        RTK_ENTER_CRITICAL(old);
        p_owner->waitq_prio = new_priority;
        readyq_prio = _task_cal_suit_readyq_prio_ec(p_owner);
        if ( p_owner->status == TASK_READY ) {
            if (readyq_prio !=p_owner->readq_prio ) {
                _task_ready_prio_change_ec(p_owner,readyq_prio);
            }
            RTK_EXIT_CRITICAL(old);
            break;
        }
        else {
            p_owner->readq_prio = readyq_prio;
            if ( p_owner->status & TASK_PENDING ) {
                p_wait_queue =  p_owner->pending_resource;
               /*
                * 拥有者正在pending，需要首先在调整拥有者任务在pending的
                * wait queue的pend list中的位置，如果调整操作执行后
                * 拥有者是pend在wait queue上优先级最高的任务
                * 且wait queue是互斥锁类型，则又一次发生了pending在互斥锁上
                * 任务的最高优先级改变的情况，这就可能需要提升这个新的互斥锁
                * 拥有者的优先级了
                */
                highest = __rtk_waitq_resort_pender_ec(
                        p_wait_queue, p_owner );
                is_mutex = (RTK_WAIT_QUEUE_TYPE_MUTEX == p_wait_queue->type);
                RTK_EXIT_CRITICAL(old);
                if (highest && is_mutex) {
                    p_mutex = RTK_CONTAINER_OF(
                        p_wait_queue,
                        struct rtk_mutex,
                        wait_queue);
                    p_owner = p_mutex->owner;
                    /*
                     * 已经取得了新的互斥锁和拥有者，
                     * 因为新互斥锁上pend任务的最高优先级已经发生了改变
                     * 而任务的持有互斥锁链表是按照pend在互斥锁任务的最高优先级来排序
                     * 所以，此时重新排列此互斥锁在拥有者任务的持有互斥锁链表中的位置
                     * 便成了必须的选项了
                     */
                }
                else {
                    break;
                }
            }
            else {
                RTK_EXIT_CRITICAL(old);
                break;
            }
        }
    }


    return ret;
}

static int __restore_task_priority (struct rtk_task *p_task)
{
    int                 priority;
    RTK_CRITICAL_STATUS_DECL(old);
    int                 need_schedule = 0;

    /*
     *  find the highest priority needed to setup,
     *  which is from the mutex of current task holded.
     *  it will be always the first one of MutexHeadList.
     */
    priority = __task_hold_mutex_priority_get(p_task);

    RTK_ENTER_CRITICAL(old);
    if ( priority > p_task->hold_mutex_org_priority ) {
        priority = p_task->hold_mutex_org_priority;
    }

    if ( unlikely(priority != p_task->waitq_prio )) {
        p_task->waitq_prio = priority;

        if (p_task->waitq_prio > p_task->base_mutex_need_prio) {
            priority = p_task->base_mutex_need_prio;
        }
        else {
            priority = p_task->waitq_prio;
        }

        if (p_task->readq_prio != priority) {
            _task_ready_prio_change_ec(p_task,priority);
            need_schedule = 1;
        }
    }
    RTK_EXIT_CRITICAL(old);

    return need_schedule;
}

#endif /* CONFIG_RTK_MUTEX_EN */


void __rtk_mutex_global_init(void)
{
    __rtk_task_mutex_sync_lock_init();
}
