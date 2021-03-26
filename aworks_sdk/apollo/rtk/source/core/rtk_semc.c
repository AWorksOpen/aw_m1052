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
 * \brief rtk �ļ����ź���
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

    /* Ԥ�����ж��е��� */
    if ( unlikely(rtk_is_int_context()) ) {

        goto cleanup;
    }
    if (NULL == p_semc) {
        goto cleanup;
    }

    /* �����ж� */
    RTK_ENTER_CRITICAL(old);
    /* �Ѿ���ʼ��? */
    if (SEMC_MAGIC_NUM_VALID ==
            (p_semc->magic_num + (unsigned long)p_semc ) ) {
        RTK_EXIT_CRITICAL(old);
        goto cleanup;
    }
    /* ����terminate ? */
    if (SEMC_MAGIC_NUM_TERMINATING ==
            (p_semc->magic_num + (unsigned long)p_semc ) ) {
        RTK_EXIT_CRITICAL(old);
        goto cleanup;
    }

    /* semc��Ч������Ϊ����terminate */
    /* ����������ֹ��������Դ�semc���в��� */
    p_semc->magic_num = SEMC_MAGIC_NUM_TERMINATING - (unsigned long)p_semc;
    RTK_EXIT_CRITICAL(old);

    /* ִ�г�ʼ�� */
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

    /* �����ж� */
    RTK_ENTER_CRITICAL(old);
    /* �����Ч�� */
    if (SEMC_MAGIC_NUM_VALID !=
            (p_semc->magic_num + (unsigned long)p_semc ) ) {
        RTK_EXIT_CRITICAL(old );
        ret = -AW_EINVAL;
        goto cleanup;
    }


    /* �˿��Ѿ�ӵ��ȫ������ */
    if ( p_semc->count ) {
        /* �ź������źţ���ȡ�ɹ� */
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
    /* ���û���ʱ�� */
    if (AW_WAIT_FOREVER != tick) {
        __task_set_wakeup_after_tick(task_current,tick);
        task_status = TASK_PENDING | TASK_DELAY;
    }
    else {
        task_status = TASK_PENDING;
    }

    /* �����ж� */
    RTK_ENTER_CRITICAL(old);
    /* �����Ч�� */
    if (SEMC_MAGIC_NUM_VALID !=
            (p_semc->magic_num + (unsigned long)p_semc ) ) {
        RTK_EXIT_CRITICAL(old );
        ret = -AW_EINVAL;
        goto cleanup;
    }

    /* �˿��Ѿ�ӵ��ȫ������ */
    if ( p_semc->count ) {
        /* �ź������źţ���ȡ�ɹ� */
        --p_semc->count;
        RTK_EXIT_CRITICAL(old );
        take_sucess = 1;
        goto cleanup;
    }

    if ( tick == 0 ) {
        /* �ź������źţ����ǲ�����ȴ� */
        RTK_EXIT_CRITICAL(old );
        ret = -AW_EAGAIN;
        goto cleanup;
    }

    /* ִ�е��˴���������ǰ���������Ҫpending */
    /* ���Ǳ���Ҫ���ǵ�������Ǵ˿̳�ʱʱ������Ѿ����� */
    if ((AW_WAIT_FOREVER == tick) ||
            (!rtk_list_empty(&task_current->tick_node.node)) ) {

        /* ��������õȴ������߳�ʱʱ�仹û�������ý���pending״̬ */
        __task_pend_on_waitq_ec(
            task_current,
            &p_semc->wait_queue);
        task_current->status = task_status;
        /* ���Լ��Ӿ����������Ƴ� */
        READY_Q_REMOVE( task_current );
        need_schedule = 1;
    }
    else {
        /* ��ʱʱ���Ѿ����� */
        /* ���ٽ���pending״̬ */
        // �˿�task_current->wakereason timeout��־��Ч
    }

    /* �˿��Ѿ����Խ����ж��� */
    RTK_EXIT_CRITICAL(old);

    /* ��ʱ��������жϣ����ȥִ���жϣ��жϴ��������Զ���ѡһ�����ȼ� */
    /* ��ߵ����񣬴��������һ�����㲻��ִ�� */
    /* ֱ��semc give����terminate�󣬴��������������ʼִ����һ����� */

    /* ��������ж�û����������Ϊ�������Ѿ����پ��� */
    /* ���Ա���Ҫ����һ���������ȣ�ִ��ʣ�����ȼ���ߵ����� */

    /* ���Խ�������Ҫ��������һ�ε��ȣ��������һ�䵽��һ���ڼ�û���жϷ��� */
    /* ������Ǳ���ģ�������У���ִ��һ�ε���Ҳ������Ӱ�죬 */
    /* ��Ϊ��ǰ�������ȼ���ߵ�����ִ�кܿ� */

    if (need_schedule) {
        rtk_schedule();
        /* ִ�е����� �����������Ϊ��ʱ������ */
        /* Ҳ��������Ϊ��semc���� */
        /* ����ǳ�ʱ���ѣ����pending״̬�Ĳ����Ѿ��ڳ�ʱ����ʱ������ */
        /* �����semc���ѣ����Ժ�������ʱ���� */
    }
cleanup:
    /* �����ʱ���� */
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

    /* ���ж��е��� */
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
    /* Ԥ�����ж��е��� */
    if ( unlikely(rtk_is_int_context()) ) {
        err = -AW_EPERM;
        goto func_exit;
    }
    /* �����ж� */
    RTK_ENTER_CRITICAL(old);

    /* �����Ч�� */
    if (SEMC_MAGIC_NUM_VALID !=
            (p_semc->magic_num + (unsigned long)p_semc ) ) {
        RTK_EXIT_CRITICAL(old );
        err = -AW_EINVAL;
        goto func_exit;
    }

    if ((0 != reset_value) && __rtk_waitq_has_penders_ec(
            &p_semc->wait_queue) ) {
        /* ���������pending�ڴ�semc�� */
        /* ��Ҫ�������õ�ֵ��Ϊ0 */
        /* ����Ҫ����һЩ���� */
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

    /* �����ж� */
    RTK_ENTER_CRITICAL(old);
    /* �����Ч�� */
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

    /* ����pending�����ȼ���ߵ����� */
    wakeup_count = __semc_wakeup_a_pender_isr_locked(p_semc);
    /* ��ʱ�ͷ��жϣ�������ж�CPU������ȥ�����ж� */
    RTK_EXIT_CRITICAL(old );
    /* ��ʱ�����жϣ��Ѿ�����Ӱ�쵽semc��ֵ�ˣ��һ��ѵ������Ѿ��ھ��������� */
    /* �����жϣ����Խ����ж��ӳ�ʱ�䣬����жϵ����˸������ȼ������ִ�� */
    /* Ҳ����ʵʱϵͳ��ԭ�򣬵�Ȼ�ж�Ҳ���ܻ�����give���ѵ����� */
    /* ��Ҳ���ϸ����ȼ�������ִ�е�ԭ�� */
    /* ���û���յ㵽������������Ҫ������һ���л�����ʵ�� */
    /* �����ȼ�������ִ�� */


    /* ����ִ�У���������������Ҳ������жϻ����� */
    /* ��ִ��һ�ε��� */
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
    /* Ԥ�����ж��е��� */
    if ( unlikely(rtk_is_int_context()) ) {
        err = -AW_EPERM;
        goto func_exit;
    }

    if (NULL == p_semc) {
        err = -AW_EINVAL;
        goto func_exit;
    }

    /* �����ж� */
    RTK_ENTER_CRITICAL(old);
     /* �����Ч�� */
    if (SEMC_MAGIC_NUM_VALID !=
            (p_semc->magic_num + (unsigned long)p_semc ) ) {
        RTK_EXIT_CRITICAL(old);
        err = -AW_EINVAL;
        goto cleanup;
    }
    /* semc��Ч������Ϊ����terminate */
    /* ����������ֹ��������Դ�semc���в��� */
    p_semc->magic_num = SEMC_MAGIC_NUM_TERMINATING - (unsigned long)p_semc;
    RTK_EXIT_CRITICAL(old);
    /* ִ��������terminate���� */
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
