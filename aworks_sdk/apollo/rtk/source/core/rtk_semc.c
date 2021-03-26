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
 * \file  rtk_semc.c
 * \brief rtk 的计数信号量
 *
 * \internal
 * \par modification history:
 * - 1.00 17-05-21 sni,
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

#if CONFIG_SEMC_EN

#define SEMC_MAGIC_NUM_VALID            0
#define SEMC_MAGIC_NUM_TERMINATING      0x45424d49U

void __rtk_semc_global_init(void)
{

}

struct rtk_semc * rtk_semc_init(
        struct rtk_semc *p_semc,
         unsigned int InitCount )
{
    struct rtk_semc                    *p_ret = NULL;
    RTK_CRITICAL_STATUS_DECL(old);

    OS_TRACE_SEMC_INIT(p_semc, InitCount);

    /* 预防在中断中调用 */
    if ( unlikely(rtk_is_int_context()) ) {

        goto cleanup;
    }
    if (NULL == p_semc) {
        goto cleanup;
    }

    /* 锁定中断 */
    RTK_ENTER_CRITICAL(old);
    /* 已经初始化? */
    if (SEMC_MAGIC_NUM_VALID ==
            (p_semc->magic_num + (unsigned long)p_semc ) ) {
        RTK_EXIT_CRITICAL(old);
        goto cleanup;
    }
    /* 正在terminate ? */
    if (SEMC_MAGIC_NUM_TERMINATING ==
            (p_semc->magic_num + (unsigned long)p_semc ) ) {
        RTK_EXIT_CRITICAL(old);
        goto cleanup;
    }

    /* semc有效，设置为正在terminate */
    /* 这样可以阻止其它任务对此semc进行操作 */
    p_semc->magic_num = SEMC_MAGIC_NUM_TERMINATING - (unsigned long)p_semc;
    RTK_EXIT_CRITICAL(old);

    /* 执行初始化 */
    p_semc->count = InitCount;
    __rtk_waitq_init(
            &p_semc->wait_queue,
            RTK_WAIT_QUEUE_TYPE_SEM_COUNTER,
            RTK_WAIT_QUEUE_OPTION_PRIORITY);
    RTK_ENTER_CRITICAL(old);
    p_semc->magic_num = SEMC_MAGIC_NUM_VALID - (unsigned long)p_semc;
    RTK_EXIT_CRITICAL(old);
    p_ret = p_semc;
cleanup:
    return p_ret;
}

static int rtk_semc_take_no_wait(struct rtk_semc *p_semc)
{
    RTK_CRITICAL_STATUS_DECL(old);
    int                 ret = -AW_EAGAIN;

    /* 锁定中断 */
    RTK_ENTER_CRITICAL(old);
    /* 检查有效性 */
    if (SEMC_MAGIC_NUM_VALID !=
            (p_semc->magic_num + (unsigned long)p_semc ) ) {
        RTK_EXIT_CRITICAL(old );
        ret = -AW_EINVAL;
        goto cleanup;
    }


    /* 此刻已经拥有全局锁了 */
    if ( p_semc->count ) {
        /* 信号量有信号，获取成功 */
        --p_semc->count;
        ret = AW_OK;
        RTK_EXIT_CRITICAL(old );
    }
    RTK_EXIT_CRITICAL(old );

cleanup:
    return ret;
}



static int rtk_semc_take_timeout( struct rtk_semc *p_semc, unsigned int tick )
{
    RTK_CRITICAL_STATUS_DECL(old);
    int                 ret = AW_OK;
    struct rtk_task    *task_current;
    int                 need_schedule = 0;
    int                 take_sucess = 0;
    rtk_task_status_t   task_status;


    task_current = rtk_task_self();
    /* 设置唤醒时间 */
    if (AW_WAIT_FOREVER != tick) {
        __task_set_wakeup_after_tick(task_current,tick);
        task_status = TASK_PENDING | TASK_DELAY;
    }
    else {
        task_status = TASK_PENDING;
    }

    /* 锁定中断 */
    RTK_ENTER_CRITICAL(old);
    /* 检查有效性 */
    if (SEMC_MAGIC_NUM_VALID !=
            (p_semc->magic_num + (unsigned long)p_semc ) ) {
        RTK_EXIT_CRITICAL(old );
        ret = -AW_EINVAL;
        goto cleanup;
    }

    /* 此刻已经拥有全局锁了 */
    if ( p_semc->count ) {
        /* 信号量有信号，获取成功 */
        --p_semc->count;
        RTK_EXIT_CRITICAL(old );
        take_sucess = 1;
        goto cleanup;
    }

    if ( tick == 0 ) {
        /* 信号量无信号，但是不允许等待 */
        RTK_EXIT_CRITICAL(old );
        ret = -AW_EAGAIN;
        goto cleanup;
    }

    /* 执行到此处，表明当前任务可能需要pending */
    /* 但是必须要考虑到的情况是此刻超时时间可能已经到了 */
    if ((AW_WAIT_FOREVER == tick) ||
            (!rtk_list_empty(&task_current->tick_node.node)) ) {

        /* 如果是永久等待，或者超时时间还没到，设置进入pending状态 */
        __task_pend_on_waitq_ec(
            task_current,
            &p_semc->wait_queue);
        task_current->status = task_status;
        /* 将自己从就绪队列上移除 */
        READY_Q_REMOVE( task_current );
        need_schedule = 1;
    }
    else {
        /* 超时时间已经到了 */
        /* 不再进入pending状态 */
        // 此刻task_current->wakereason timeout标志有效
    }

    /* 此刻已经可以解锁中断了 */
    RTK_EXIT_CRITICAL(old);

    /* 此时如果发生中断，则会去执行中断，中断处理完后会自动挑选一个优先级 */
    /* 最高的任务，此任务的下一句代码便不会执行 */
    /* 直到semc give或是terminate后，此任务便会就绪，开始执行下一句代码 */

    /* 但是如果中断没发生，则因为此任务已经不再就绪 */
    /* 所以必须要发起一次主动调度，执行剩下优先级最高的任务 */

    /* 所以接下来需要主动发起一次调度，如果从上一句到这一句期间没有中断发生 */
    /* 则调度是必须的，而如果有，再执行一次调度也不会有影响， */
    /* 因为当前任务优先级最高调度器执行很快 */

    if (need_schedule) {
        rtk_schedule();
        /* 执行到这里 任务可能是因为超时被唤醒 */
        /* 也可能是因为被semc唤醒 */
        /* 如果是超时唤醒，清除pending状态的操作已经在超时唤醒时做过了 */
        /* 如果是semc唤醒，则稍后会清除延时唤醒 */
    }
cleanup:
    /* 清除延时唤醒 */
    if (AW_WAIT_FOREVER != tick) {
        __task_unset_wakeup_after_tick(task_current);
    }
    ret = __task_get_wake_reason(task_current);
    if (take_sucess) {
        ret = AW_OK;
    }
    return ret;
}

int rtk_semc_take( struct rtk_semc *p_semc, unsigned int tick )
{
    if (NULL == p_semc) {
        return -AW_EINVAL;
    }

    /* 在中断中调用 */
    if ( unlikely(rtk_is_int_context() || 0 == tick) ) {
        return rtk_semc_take_no_wait(p_semc);
    }
    return rtk_semc_take_timeout(p_semc,tick);
}


int rtk_semc_reset(
        struct rtk_semc  *p_semc,
        unsigned int      reset_value )
{
    RTK_CRITICAL_STATUS_DECL(old);
    int                 err = AW_OK;
    int                 tmp_ret;


    OS_TRACE_SEMC_RESET_ENTER(p_semc, reset_value);
    if (NULL == p_semc) {
        err = -AW_EINVAL;
        goto func_exit;
    }
    /* 预防在中断中调用 */
    if ( unlikely(rtk_is_int_context()) ) {
        err = -AW_EPERM;
        goto func_exit;
    }
    /* 锁定中断 */
    RTK_ENTER_CRITICAL(old);

    /* 检查有效性 */
    if (SEMC_MAGIC_NUM_VALID !=
            (p_semc->magic_num + (unsigned long)p_semc ) ) {
        RTK_EXIT_CRITICAL(old );
        err = -AW_EINVAL;
        goto func_exit;
    }

    if ((0 != reset_value) && __rtk_waitq_has_penders_ec(
            &p_semc->wait_queue) ) {
        /* 如果有任务pending在此semc上 */
        /* 且要重新设置的值不为0 */
        /* 则需要唤醒一些任务 */
        RTK_EXIT_CRITICAL(old );
        do {
            tmp_ret = rtk_semc_give(p_semc);
            reset_value --;
        } while( (0 != reset_value) && (0 ==tmp_ret) );

        goto func_exit;
    }
    p_semc->count = reset_value;
    RTK_EXIT_CRITICAL(old );

func_exit:
    OS_TRACE_SEMC_RESET_EXIT(err);
    return err;

}

static int __semc_wakeup_a_pender_isr_locked(
        struct rtk_semc *p_semc)
{
    struct rtk_task         *task;

    task = __rtk_waitq_pop_1_pender_ec(&p_semc->wait_queue);
    if (NULL == task) {
        return 0;
    }
    p_semc->count --;
    __task_wakup_with_err_ec(task,RTK_TASK_WAKE_BY_WAITQ);
    return 1;
}

int rtk_semc_give( struct rtk_semc *p_semc )
{
    RTK_CRITICAL_STATUS_DECL(old);
    int                 err = AW_OK;
    int                 wakeup_count;

    OS_TRACE_SEMC_GIVE_ENTER(p_semc);
    if (NULL == p_semc) {
        err = -AW_EINVAL;
        goto error_exit;
    }

    /* 锁定中断 */
    RTK_ENTER_CRITICAL(old);
    /* 检查有效性 */
    if (SEMC_MAGIC_NUM_VALID !=
            (p_semc->magic_num + (unsigned long)p_semc ) ) {
        RTK_EXIT_CRITICAL(old );
        err = -AW_EINVAL;
        goto cleanup;
    }

    if ( (unsigned int)(~0) == p_semc->count  ) {
        RTK_EXIT_CRITICAL(old );
        err = -AW_ENOSPC;
        goto cleanup;
    }

    p_semc->count ++;

    /* 唤醒pending的优先级最高的任务 */
    wakeup_count = __semc_wakeup_a_pender_isr_locked(p_semc);
    /* 此时释放中断，如果有中断CPU，可以去处理中断 */
    RTK_EXIT_CRITICAL(old );
    /* 此时允许中断，已经不再影响到semc的值了，且唤醒的任务已经在就绪队列中 */
    /* 启用中断，可以降低中断延迟时间，如果中断导致了更高优先级任务的执行 */
    /* 也符合实时系统的原则，当然中断也可能唤醒由give唤醒的任务 */
    /* 这也符合高优先级任务先执行的原则 */
    /* 如果没有终点到来，接下来需要主动作一次切换，以实现 */
    /* 高优先级任务先执行 */


    /* 继续执行，如果唤醒了任务，且不是在中断环境中 */
    /* 则执行一次调度 */
    if (wakeup_count ) {
        if ( unlikely(rtk_is_int_context()) ) {
            rtk_schedule_isr();
        }
        else {
            rtk_schedule();
        }
    }

cleanup:
error_exit:
    OS_TRACE_SEMC_TAKE_EXIT(err);
    return err;
}

int rtk_semc_terminate( struct rtk_semc *p_semc )
{
    int                 err = AW_OK;
    RTK_CRITICAL_STATUS_DECL(old);

    OS_TRACE_SEMC_TERMINATE_ENTER(p_semc);
    /* 预防在中断中调用 */
    if ( unlikely(rtk_is_int_context()) ) {
        err = -AW_EPERM;
        goto func_exit;
    }

    if (NULL == p_semc) {
        err = -AW_EINVAL;
        goto func_exit;
    }

    /* 锁定中断 */
    RTK_ENTER_CRITICAL(old);
     /* 检查有效性 */
    if (SEMC_MAGIC_NUM_VALID !=
            (p_semc->magic_num + (unsigned long)p_semc ) ) {
        RTK_EXIT_CRITICAL(old);
        err = -AW_EINVAL;
        goto cleanup;
    }
    /* semc有效，设置为正在terminate */
    /* 这样可以阻止其它任务对此semc进行操作 */
    p_semc->magic_num = SEMC_MAGIC_NUM_TERMINATING - (unsigned long)p_semc;
    RTK_EXIT_CRITICAL(old);
    /* 执行真正的terminate操作 */
    err = __rtk_waitq_terminate(&p_semc->wait_queue);
    memset(&p_semc->wait_queue,0,sizeof(p_semc->wait_queue));
    p_semc->count = 0;

    RTK_ENTER_CRITICAL(old);
    p_semc->magic_num = 0;
    RTK_EXIT_CRITICAL(old);

cleanup:
func_exit:
    OS_TRACE_SEMC_TERMINATE_EXIT(err);
    return err;

}

#endif
