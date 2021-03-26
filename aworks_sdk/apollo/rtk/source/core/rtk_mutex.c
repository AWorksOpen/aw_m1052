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
 * \brief rtk mutexʵ��
 *
 * \par Դ����
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
 * \brief �ͷŻ�����
 * �ͷŻ�����
 *
 * \param[in] p_mutex       Ҫ�����Ļ�����
 * \retval                  �Ƿ�ɹ�������һ������
 */
static int __mutex_release( struct rtk_mutex *p_mutex);

static int _mutex_init_os_not_startup( struct rtk_mutex *p_mutex )
{
    int     err = AW_OK;

    OS_TRACE_MUTEX_INIT(p_mutex);

    /*  Ԥ�����ж��е��� */
    if ( unlikely(rtk_is_int_context()) ) {
        err = -AW_EPERM;
        goto cleanup;
    }
    if (NULL == p_mutex) {
        err = -AW_EINVAL;
        goto cleanup;
    }

    /*  �Ѿ���ʼ��? */
    if (MUTEX_MAGIC_NUM_VALID ==
            (p_mutex->magic_num + (unsigned long)p_mutex ) ) {
        __rtk_task_mutex_sync_unlock();
        err = -AW_EEXIST;
        goto cleanup;
    }

    /*  ����terminate ? */
    if (MUTEX_MAGIC_NUM_TERMINATING ==
            (p_mutex->magic_num + (unsigned long)p_mutex ) ) {
        __rtk_task_mutex_sync_unlock();
        err = -AW_EEXIST;
        goto cleanup;
    }

    /*  ִ�г�ʼ�� */
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

    /*  Ԥ�����ж��е��� */
    if ( unlikely(rtk_is_int_context()) ) {
        err = -AW_EPERM;
        goto cleanup;
    }
    if (NULL == p_mutex) {
        err = -AW_EINVAL;
        goto cleanup;
    }

    __rtk_task_mutex_sync_lock();
    /*  �Ѿ���ʼ��? */
    if (MUTEX_MAGIC_NUM_VALID ==
            (p_mutex->magic_num + (unsigned long)p_mutex ) ) {
        __rtk_task_mutex_sync_unlock();
        err = -AW_EEXIST;
        goto cleanup;
    }

    /*  ����terminate ? */
    if (MUTEX_MAGIC_NUM_TERMINATING ==
            (p_mutex->magic_num + (unsigned long)p_mutex ) ) {
        __rtk_task_mutex_sync_unlock();
        err = -AW_EEXIST;
        goto cleanup;
    }

    /*  ִ�г�ʼ�� */
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

    /*  Ԥ�����ж��е��� */
    if ( unlikely(rtk_is_int_context()) ) {
        ret = -AW_EPERM;
        goto func_exit;
    }

    if (NULL == p_mutex) {
        ret = -AW_EINVAL;
        goto func_exit;
    }
    __rtk_task_mutex_sync_lock();

    /*  �����Ч�� */
    if (MUTEX_MAGIC_NUM_VALID !=
            (p_mutex->magic_num + (unsigned long)p_mutex ) ) {
        __rtk_task_mutex_sync_unlock();
        ret = -AW_EINVAL;
        goto cleanup;
    }

    if ( p_mutex->owner == NULL ) {
        /*  �ɹ� */
        __mutex_owner_set( p_mutex, task_current );
        p_mutex->mutex_recurse_count = 1;
        __rtk_task_mutex_sync_unlock();
        goto cleanup;
    } else if ( p_mutex->owner == task_current ) {
        /*  �ݹ� */
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

    /*  �����ǰ������mutex��pend list���ȼ���ߵ����� */
    /*  ��˵��mutex pend list��������ȼ������˸ı� */
    /*  ���ʱ�����Ҫ����mutex owner�����ȼ��� */
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
    /*  ִ�е��˴���������ǰ���������Ҫpending */
    /*  ���Ǳ���Ҫ���ǵ�������Ǵ˿̳�ʱʱ������Ѿ����� */
    if ( (AW_WAIT_FOREVER == tick) ||
            (!rtk_list_empty(&task_current->tick_node.node) ) ){
        /*  ��������õȴ������߳�ʱʱ�仹û�� */
        /*  ���������ں��ʵ�״̬ */
        if (AW_WAIT_FOREVER == tick) {
            task_current->status = TASK_PENDING;
        }
        else {
            task_current->status = TASK_PENDING | TASK_DELAY;
        }
        /*  ���Լ��Ӿ����������Ƴ� */
        READY_Q_REMOVE( task_current );
        need_schedule = 1;
        __rtk_task_mutex_sync_unlock_no_sche();
        /*  �˿��Ѿ����Խ����ж��� */
        RTK_EXIT_CRITICAL(old);
    }
    else {
        /*  ��ʱʱ���Ѿ����� */
        /*  �˿��Ѿ����Խ����ж��� */
        RTK_EXIT_CRITICAL(old);
        __rtk_task_mutex_sync_unlock();
    }



    /*
     * ��ʱ��������жϣ����ȥִ���жϣ��жϴ��������Զ���ѡһ�����ȼ�
     * ��ߵ����񣬴��������һ�����㲻��ִ��
     * ֱ��mutex unlock����terminate�󣬴��������������ʼִ����һ�����
     *
     * ��������ж�û����������Ϊ�������Ѿ����پ���
     * ���Ա���Ҫ����һ���������ȣ�ִ��ʣ�����ȼ���ߵ�����
     *
     * ���Խ�������Ҫ��������һ�ε��ȣ��������һ�䵽��һ���ڼ�û���жϷ���
     * ������Ǳ���ģ�������У���ִ��һ�ε���Ҳ������Ӱ�죬
     * ��Ϊ��ǰ�������ȼ���ߵ�����ִ�кܿ�
     */
    if (need_schedule) {
        rtk_schedule();
    }

    if (AW_WAIT_FOREVER != tick) {
        __task_unset_wakeup_after_tick(task_current);
    }


    /*  ֱ�ӵ�lockû�гɹ������������پ����� */
    /*  �����ѵ�ԭ�� */
    ret = __task_get_wake_reason(task_current);

    /*  ���lock ����û�гɹ� */
    /*  ������Ҫ���Ѿ�������owener���ȼ���ԭ */
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
    /*  Ԥ�����ж��е��� */
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
     /*  �����Ч�� */
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
     *  �ָ����ȼ��̳ж��������ȼ�
     *  ��ʱ�Żָ����ȼ���ʵ����Ӱ��ʵ���ϱȵ�ǰ�������ȼ��ߵ������ʵʱ��
     *  ��Ϊ���ȼ�����������˵�ǰ�������ȼ�������Щ���񣬴Ӷ���ִ��
     *  ���ǰ�������жϺ������жϣ��������˸������ȼ������ִ��
     *  ���Ǹ����ȼ�������ִ�У�����Ӱ��ʵʱ��
     *  ����������жϣ�����û�и������ȼ�����ִ�У�����û���жϣ�
     *  ��ʱ�ָ����ȼ������ǰ�治�����жϣ�ִ�е�ʱ�䲢�޶��ٲ��
     *  ��Ҳ�ǵ�ǰ�����Ȼָ����ȼ���ʵ���ϱȵ�ǰ�������ȼ��ߵ������ִ��
     *  ������������ȴ���Խ����ж��ӳ�ʱ��
     */
    need_schedule |= __restore_task_priority(task_current);
    __rtk_task_mutex_sync_unlock();

    /*  ���ж������жϸ�����ȣ�û������������Ҳ��֤�����ȼ�������ִ�� */
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
    /*  Ԥ�����ж��е��� */
    if ( unlikely(rtk_is_int_context()) ) {
        err = -AW_EPERM;
        goto func_exit;
    }

    if (NULL == p_mutex) {
        err = -AW_EINVAL;
        goto func_exit;
    }

    /*  �����ж� */
    __rtk_task_mutex_sync_lock();
    /*  �����Ч�� */
    if (MUTEX_MAGIC_NUM_VALID !=
            (p_mutex->magic_num + (unsigned long)p_mutex ) ) {
        __rtk_task_mutex_sync_unlock();
        err = -AW_EINVAL;
        goto cleanup;
    }
    /*  mutex��Ч������Ϊ����terminate */
    /*  ����������ֹ��������Դ�mutex���в��� */
    p_mutex->magic_num = MUTEX_MAGIC_NUM_TERMINATING - (unsigned long)p_mutex;
    p_owner = p_mutex->owner;
    if (NULL != p_owner) {
        __mutex_owner_unset( p_mutex);
        need_schedule |= __restore_task_priority(p_owner);
    }
    __rtk_task_mutex_sync_unlock();

    /* ִ��������terminate���� */
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
     * �˺����ڸı��������ȼ��ĺ����е���
     * ��Ϊ�������ȼ��ı���
     * ���´�������pend�Ļ�������pend list��������ȼ�Ҳ�ı���
     * ��ʱ����Ҫ�ı��������pend��mutex��owner�����ȼ���
     * ��Ϊ��������pend�Ļ�������pend list��������ȼ��ı���
     * ������ĳ��л����������ǰ���pend�ڻ����������������ȼ�������
     * ���ԣ���ʱ�������д˻�������ӵ��������ĳ��л����������е�λ��
     * ����˱����ѡ����
    */
    while ( __resort_hold_mutex_list_and_trig(p_mutex,p_owner) &&
            p_owner->waitq_prio > new_priority ) {
        /*
         * ���p_mutex��Ϊp_owner����mutex�����ȼ���ߵ�һ��
         * �����p_owner�����ȼ�������Ҫ�ı�
         * �ټ���p_owner����pend��p_mutex������������Ч��
         * �����Ҫ�ı�p_owner�����ȼ���
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
                * ӵ��������pending����Ҫ�����ڵ���ӵ����������pending��
                * wait queue��pend list�е�λ�ã������������ִ�к�
                * ӵ������pend��wait queue�����ȼ���ߵ�����
                * ��wait queue�ǻ��������ͣ�����һ�η�����pending�ڻ�������
                * �����������ȼ��ı���������Ϳ�����Ҫ��������µĻ�����
                * ӵ���ߵ����ȼ���
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
                     * �Ѿ�ȡ�����µĻ�������ӵ���ߣ�
                     * ��Ϊ�»�������pend�����������ȼ��Ѿ������˸ı�
                     * ������ĳ��л����������ǰ���pend�ڻ����������������ȼ�������
                     * ���ԣ���ʱ�������д˻�������ӵ��������ĳ��л����������е�λ��
                     * ����˱����ѡ����
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
