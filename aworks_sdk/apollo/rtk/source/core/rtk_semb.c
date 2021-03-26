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
 * \file  rtk_semb.c
 * \brief rtk �Ķ������ź���
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

#if CONFIG_SEMB_EN

#define SEMB_MAGIC_NUM_VALID            0
#define SEMB_MAGIC_NUM_TERMINATING      0x45424d49U

void __rtk_semb_global_init(void)
{

}

struct rtk_semb * rtk_semb_init(
        struct rtk_semb *p_semb,
        int              InitCount )
{
    struct rtk_semb                    *p_ret = NULL;
    RTK_CRITICAL_STATUS_DECL(old);

    OS_TRACE_SEMB_INIT(p_semb, InitCount);

    /* Ԥ�����ж��е��� */
    if ( unlikely(rtk_is_int_context()) ) {

        goto cleanup;
    }
    if (NULL == p_semb) {
        goto cleanup;
    }

    /* �����ж� */
    RTK_ENTER_CRITICAL(old);
    /* �Ѿ���ʼ��? */
    if (SEMB_MAGIC_NUM_VALID ==
            (p_semb->magic_num + (unsigned long)p_semb ) ) {
        RTK_EXIT_CRITICAL(old);
        goto cleanup;
    }
    /* ����terminate ? */
    if (SEMB_MAGIC_NUM_TERMINATING ==
            (p_semb->magic_num + (unsigned long)p_semb ) ) {
        RTK_EXIT_CRITICAL(old);
        goto cleanup;
    }
    /* semb��Ч������Ϊ����terminate */
    /* ����������ֹ��������Դ�semb���в��� */
    p_semb->magic_num = SEMB_MAGIC_NUM_TERMINATING - (unsigned long)p_semb;
    RTK_EXIT_CRITICAL(old);

    /* ִ�г�ʼ�� */
    p_semb->count = !!InitCount;
    __rtk_waitq_init(
            &p_semb->wait_queue,
            RTK_WAIT_QUEUE_TYPE_SEM_BINARY,
            RTK_WAIT_QUEUE_OPTION_PRIORITY);

    RTK_ENTER_CRITICAL(old);
    p_semb->magic_num = SEMB_MAGIC_NUM_VALID - (unsigned long)p_semb;
    RTK_EXIT_CRITICAL(old);
    p_ret = p_semb;
cleanup:
    return p_ret;
}

static int rtk_semb_take_no_wait(struct rtk_semb *p_semb)
{
    RTK_CRITICAL_STATUS_DECL(old);
    int                 ret = -AW_EAGAIN;

    /* �����ж� */
    RTK_ENTER_CRITICAL(old);
    /* �����Ч�� */
    if (SEMB_MAGIC_NUM_VALID !=
            (p_semb->magic_num + (unsigned long)p_semb ) ) {
        RTK_EXIT_CRITICAL(old );
        ret = -AW_EINVAL;
        goto cleanup;
    }


    /* �˿��Ѿ�ӵ��ȫ������ */
    if ( p_semb->count ) {
        /* �ź������źţ���ȡ�ɹ� */
        --p_semb->count;
        ret = AW_OK;
        RTK_EXIT_CRITICAL(old );
    }
    RTK_EXIT_CRITICAL(old );

cleanup:
    return ret;
}



static int rtk_semb_take_timeout( struct rtk_semb *p_semb, unsigned int tick )
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
    if (SEMB_MAGIC_NUM_VALID !=
            (p_semb->magic_num + (unsigned long)p_semb ) ) {
        RTK_EXIT_CRITICAL(old );
        ret = -AW_EINVAL;
        goto cleanup;
    }

    /* �˿��Ѿ�ӵ��ȫ������ */
    if ( p_semb->count ) {
        /* �ź������źţ���ȡ�ɹ� */
        --p_semb->count;
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
            &p_semb->wait_queue);
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
    /* ֱ��semb give����terminate�󣬴��������������ʼִ����һ����� */

    /* ��������ж�û����������Ϊ�������Ѿ����پ��� */
    /* ���Ա���Ҫ����һ���������ȣ�ִ��ʣ�����ȼ���ߵ����� */

    /* ���Խ�������Ҫ��������һ�ε��ȣ��������һ�䵽��һ���ڼ�û���жϷ��� */
    /* ������Ǳ���ģ�������У���ִ��һ�ε���Ҳ������Ӱ�죬 */
    /* ��Ϊ��ǰ�������ȼ���ߵ�����ִ�кܿ� */

    if (need_schedule) {
        rtk_schedule();
        /* ִ�е����� �����������Ϊ��ʱ������ */
        /* Ҳ��������Ϊ��semb���� */
        /* ����ǳ�ʱ���ѣ����pending״̬�Ĳ����Ѿ��ڳ�ʱ����ʱ������ */
        /* �����semb���ѣ����Ժ�������ʱ���� */
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

int rtk_semb_take( struct rtk_semb *p_semb, unsigned int tick )
{
    if (NULL == p_semb) {
        return -AW_EINVAL;
    }

    /* ���ж��е��� */
    if ( unlikely(rtk_is_int_context() || 0 == tick) ) {
        return rtk_semb_take_no_wait(p_semb);
    }
    return rtk_semb_take_timeout(p_semb,tick);
}

int rtk_semb_reset( struct rtk_semb*p_semb, uint32_t reset_value )
{
    RTK_CRITICAL_STATUS_DECL(old);
    int                     err = AW_OK;
    int                     tmp_ret ;

    reset_value = !!reset_value;
    OS_TRACE_SEMB_RESET_ENTER(p_semb, reset_value);
    if (NULL == p_semb) {
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
    if (SEMB_MAGIC_NUM_VALID !=
            (p_semb->magic_num + (unsigned long)p_semb ) ) {
        RTK_EXIT_CRITICAL(old );
        err = -AW_EINVAL;
        goto func_exit;
    }

    if ((0 != reset_value) && __rtk_waitq_has_penders_ec(
            &p_semb->wait_queue) ) {
        /* ���������pending�ڴ�semb�� */
        /* ��Ҫ�������õ�ֵ��Ϊ0 */
        /* ����Ҫ����һЩ���� */
        RTK_EXIT_CRITICAL(old );
        do {
            tmp_ret = rtk_semb_give(p_semb);
            reset_value --;
        } while( (0 != reset_value) && (0 ==tmp_ret) );

        goto func_exit;
    }
    p_semb->count = reset_value;
    RTK_EXIT_CRITICAL(old );

func_exit:
    OS_TRACE_SEMB_RESET_EXIT(err);
    return err;

}

static int __semb_wakeup_a_pender_ec(
        struct rtk_semb *p_semb)
{
    struct rtk_task         *task;

    task = __rtk_waitq_pop_1_pender_ec(&p_semb->wait_queue);
    if (NULL == task) {
        return 0;
    }
    p_semb->count = 0;
    __task_wakup_with_err_ec(task,RTK_TASK_WAKE_BY_WAITQ);
    return 1;
}

int rtk_semb_give( struct rtk_semb *p_semb )
{
    RTK_CRITICAL_STATUS_DECL(old);
    int                     err = AW_OK;
    int                     wakeup_count;

    OS_TRACE_SEMB_GIVE_ENTER(p_semb);
    if (NULL == p_semb) {
        err = -AW_EINVAL;
        goto error_exit;
    }

    /* �����ж� */
    RTK_ENTER_CRITICAL(old);
    /* �����Ч�� */
    if (SEMB_MAGIC_NUM_VALID !=
            (p_semb->magic_num + (unsigned long)p_semb ) ) {
        RTK_EXIT_CRITICAL(old );
        err = -AW_EINVAL;
        goto cleanup;
    }

    p_semb->count = 1;

    /* ����pending�����ȼ���ߵ����� */
    wakeup_count = __semb_wakeup_a_pender_ec(p_semb);
    /* ��ʱ�ͷ��жϣ�������ж�CPU������ȥ�����ж� */
    RTK_EXIT_CRITICAL(old );
    /* ��ʱ�����жϣ��Ѿ�����Ӱ�쵽semb��ֵ�ˣ��һ��ѵ������Ѿ��ھ��������� */
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
    OS_TRACE_SEMB_TAKE_EXIT(err);
    return err;
}

int rtk_semb_terminate( struct rtk_semb *p_semb )
{
    int                 err = AW_OK;
    RTK_CRITICAL_STATUS_DECL(old);

    OS_TRACE_SEMB_TERMINATE_ENTER(p_semb);
    /* Ԥ�����ж��е��� */
    if ( unlikely(rtk_is_int_context()) ) {
        err = -AW_EPERM;
        goto func_exit;
    }

    if (NULL == p_semb) {
        err = -AW_EINVAL;
        goto func_exit;
    }

    /* �����ж� */
    RTK_ENTER_CRITICAL(old);
    /* �����Ч�� */
    if (SEMB_MAGIC_NUM_VALID !=
            (p_semb->magic_num + (unsigned long)p_semb ) ) {
        RTK_EXIT_CRITICAL(old);
        err = -AW_EINVAL;
        goto cleanup;
    }

    /* semb��Ч������Ϊ����terminate */
    /* ����������ֹ��������Դ�semb���в��� */
    p_semb->magic_num = SEMB_MAGIC_NUM_TERMINATING - (unsigned long)p_semb;
    RTK_EXIT_CRITICAL(old);

    /* ִ��������terminate���� */
    err = __rtk_waitq_terminate(&p_semb->wait_queue);

    memset(&p_semb->wait_queue,0,sizeof(p_semb->wait_queue));
    p_semb->count = 0;
    RTK_ENTER_CRITICAL(old);
    p_semb->magic_num = 0;
    RTK_EXIT_CRITICAL(old);
cleanup:
func_exit:
    OS_TRACE_SEMB_TERMINATE_EXIT(err);
    return err;

}

#endif
