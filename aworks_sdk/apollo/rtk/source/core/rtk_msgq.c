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
 * \file  rtk_msgq.c
 * \brief RTK MSGQ���
 *
 * \internal
 * \par modification history:
 * 1.00 18-08-20  sni, first version
 * \endinternal
 */
#include "rtk.h"
#include "aw_errno.h"
#include <string.h> /* for memset() and memcpy() */
#include "aw_common.h"
#include "os_trace_events.h"


#if CONFIG_RTK_MSGQ_EN

#define MSGQ_MAGIC_NUM_VALID            0
#define MSGQ_MAGIC_NUM_TERMINATING      0x45424d49U
#define MSGQ_MAGIC_NUM_INITTING         0x292e2945U

struct rtk_msgq *rtk_msgq_init(
        struct rtk_msgq *p_msgq,
        struct rtk_msgq_node *p_msgq_node_array,
        uint8_t *buff,
        uint32_t buffer_size,
        uint32_t unit_size )
{
    int                     count;
    int                     i;
    RTK_CRITICAL_STATUS_DECL(old);
    struct rtk_msgq        *p_ret = NULL;

    OS_TRACE_MSGQ_INIT(p_msgq, buff, buffer_size, unit_size);

    /* Ԥ�����ж��е��� */
    if ( unlikely(rtk_is_int_context()) ) {
        goto cleanup;
    }

    if (NULL == p_msgq) {
        goto cleanup;
    }

    /* �����ж� */
    RTK_ENTER_CRITICAL(old);
    /* �Ѿ���ʼ��? */
    if (MSGQ_MAGIC_NUM_VALID ==
            (p_msgq->magic_num + (unsigned long)p_msgq ) ) {
        RTK_EXIT_CRITICAL(old);
        goto cleanup;
    }

    /* ����terminate ? */
    if (MSGQ_MAGIC_NUM_TERMINATING ==
            (p_msgq->magic_num + (unsigned long)p_msgq ) ) {
        RTK_EXIT_CRITICAL(old);
        goto cleanup;
    }

    /* msgq��Ч������Ϊ����terminate */
    /* ����������ֹ��������Դ�msgq���в��� */
    p_msgq->magic_num = MSGQ_MAGIC_NUM_TERMINATING - (unsigned long)p_msgq;
    RTK_EXIT_CRITICAL(old);

    count = buffer_size / unit_size;

    if ( buffer_size == 0 || count == 0 ) {
        return NULL;
    }

    for (i=1; i<count; i++ ) {
        p_msgq_node_array[i-1].next = &p_msgq_node_array[i];
    }

    p_msgq_node_array[count-1].next = NULL;

    rtk_semc_init(&p_msgq->semc_write,count);
    rtk_semc_init(&p_msgq->semc_read,0);

    rtk_smp_wmb();

    p_msgq->free_list_head = p_msgq_node_array;
    p_msgq->p_msgq_nodes = p_msgq_node_array;
    p_msgq->full_list_head = NULL;
    p_msgq->full_list_tail = NULL;
    p_msgq->buff = buff;
    p_msgq->buff_size = buffer_size;
    p_msgq->unit_size = unit_size;
    p_msgq->count = count;
    p_msgq->ref_count = 0;

    RTK_ENTER_CRITICAL(old);
    p_msgq->magic_num = MSGQ_MAGIC_NUM_VALID - (unsigned long)p_msgq;
    RTK_EXIT_CRITICAL(old);

    p_ret = p_msgq;
cleanup:
    return p_ret;
}


int rtk_msgq_terminate( struct rtk_msgq *p_msgq )
{
    RTK_CRITICAL_STATUS_DECL(old);
    int                     err = AW_OK;

    OS_TRACE_MSGQ_TERMINATE_ENTER(p_msgq);
    /* Ԥ�����ж��е��� */
    if ( unlikely(rtk_is_int_context()) ) {
        err = -AW_EPERM;
        goto func_exit;
    }

    if (NULL == p_msgq) {
        err = -AW_EINVAL;
        goto func_exit;
    }

    /* �����ж� */
    RTK_ENTER_CRITICAL(old);
     /* �����Ч�� */
    if (MSGQ_MAGIC_NUM_VALID !=
            (p_msgq->magic_num + (unsigned long)p_msgq ) ) {
        RTK_EXIT_CRITICAL(old);
        err = -AW_EINVAL;
        goto func_exit;
    }
    /* msgq��Ч������Ϊ����terminate */
    /* ����������ֹ��������Դ�msgq���в��� */
    p_msgq->magic_num = MSGQ_MAGIC_NUM_TERMINATING - (unsigned long)p_msgq;
    RTK_EXIT_CRITICAL(old);

    /* ���ڻ������еĵȴ� */
    rtk_semc_terminate(&p_msgq->semc_read);
    rtk_semc_terminate(&p_msgq->semc_write);

    /* ȷ������msgqû���������������� */
    RTK_ENTER_CRITICAL(old);
    while (0 != p_msgq->ref_count) {
        RTK_EXIT_CRITICAL(old);
        rtk_task_delay(1);
        RTK_ENTER_CRITICAL(old);
    }
    RTK_EXIT_CRITICAL(old);

    /* ���ڿ��԰�ȫ�ز������ݳ�Ա�� */
    p_msgq->buff_size = 0;
    p_msgq->count = 0;
    p_msgq->unit_size = 0;
    p_msgq->full_list_head = NULL;
    p_msgq->full_list_tail = NULL;
    p_msgq->free_list_head = NULL;
    p_msgq->buff = NULL;
    p_msgq->p_msgq_nodes = NULL;
    p_msgq->ref_count = 0;
    RTK_ENTER_CRITICAL(old);
    p_msgq->magic_num = 0;
    RTK_EXIT_CRITICAL(old);
func_exit:
    return err;
}

static int msgq_receive_begin(
        struct rtk_msgq *p_msgq,
        void           **pp_buffer,
        aw_tick_t        tick )
{
    int                         err = AW_OK;
    RTK_CRITICAL_STATUS_DECL(old);
    struct rtk_msgq_node       *node = NULL;
    struct rtk_task            *p_current_task;

    p_current_task = rtk_task_self();
    RTK_ENTER_CRITICAL(old);
    /* �Ƿ�Ϊ��Ч��msgq */
    if (MSGQ_MAGIC_NUM_VALID !=
            (p_msgq->magic_num + (unsigned long)p_msgq ) ) {
        RTK_EXIT_CRITICAL(old);
        err = -AW_EINVAL;
        goto func_exit;
    }
    /* ���ü�����1 */
    p_msgq->ref_count ++;
    p_current_task->safe_count ++;
    RTK_EXIT_CRITICAL(old);

    err = rtk_semc_take(&p_msgq->semc_read,tick);
    if (AW_OK == err) {
        /* �пɶ����� */
        RTK_ENTER_CRITICAL(old);
        node = p_msgq->full_list_head;
        p_msgq->full_list_head = p_msgq->full_list_head->next;
        if (NULL == p_msgq->full_list_head) {
            p_msgq->full_list_tail = NULL;
        }
        RTK_EXIT_CRITICAL(old);

        *pp_buffer = p_msgq->buff +
                (node - p_msgq->p_msgq_nodes) * p_msgq->unit_size;
    }
    else {
        /* �ȴ������б���ֹ��ʱ */
        /* ��msgq���ٱ����� */
        RTK_ENTER_CRITICAL(old);
        p_msgq->ref_count --;
        p_current_task->safe_count --;
        RTK_EXIT_CRITICAL(old);
    }

func_exit:
    return err;
}

static int msgq_receive_end( struct rtk_msgq *p_msgq, void *buffer )
{
    int                     index;
    RTK_CRITICAL_STATUS_DECL(old);
    struct rtk_msgq_node   *node;
    struct rtk_task        *p_current_task;

    index = ((uint8_t *)buffer - p_msgq->buff) / p_msgq->unit_size;
    node = &p_msgq->p_msgq_nodes[index];

    RTK_ENTER_CRITICAL(old);
    /* put msgq_node to free list head */
    node->next = p_msgq->free_list_head;
    p_msgq->free_list_head = node;
    RTK_EXIT_CRITICAL(old);

    /* ����һ��pending��write�ϵ����� */
    rtk_semc_give(&p_msgq->semc_write);

    /* ����ref count */
    p_current_task = rtk_task_self();
    RTK_ENTER_CRITICAL(old);
    p_msgq->ref_count --;
    p_current_task->safe_count --;
    RTK_EXIT_CRITICAL(old);

    return 0;
}


int rtk_msgq_receive(
        struct rtk_msgq *p_msgq,
        void            *buff,
        size_t           buff_size,
        aw_tick_t        tick )
{
    void                   *rx_buffer = NULL;
    int                     err = AW_OK;

    /* ���Ƚ��г����Ĳ������ */

    OS_TRACE_MSGQ_RECEIVE_ENTER(p_msgq, buff, buff_size, tick);
    /* �������������ж��������� */
    if ( unlikely(rtk_is_int_context()) ) {
        err = -AW_EPERM;
        goto func_exit;
    }

    if (NULL == p_msgq || ((NULL!= buff && 0== buff_size) )
            || ((NULL == buff) && (0 != buff_size) )) {
        err = -AW_EINVAL;
        goto func_exit;
    }

    err = msgq_receive_begin( p_msgq, &rx_buffer,  tick );
    if ( !err ) {
        if (NULL != buff ) {
            buff_size = buff_size > p_msgq->unit_size? p_msgq->unit_size:buff_size;
            memcpy( buff, rx_buffer, buff_size );
        }
        msgq_receive_end( p_msgq, rx_buffer );
    }

func_exit:
    OS_TRACE_MSGQ_RECEIVE_EXIT(err);
    return err;
}

static int msgq_send_begin(
        struct rtk_msgq *p_msgq,
        void           **pp_buff,
        int              tick )
{
    int                         err = AW_OK;
    RTK_CRITICAL_STATUS_DECL(old);
    struct rtk_msgq_node       *node = NULL;
    int                         index;
    struct rtk_task            *p_current_task;

    p_current_task = rtk_task_self();
    RTK_ENTER_CRITICAL(old);
    /* �Ƿ�Ϊ��Ч��msgq */
    if (MSGQ_MAGIC_NUM_VALID !=
            (p_msgq->magic_num + (unsigned long)p_msgq ) ) {
        RTK_EXIT_CRITICAL(old);
        err = -AW_EINVAL;
        goto func_exit;
    }

    /* ���ü�����1 */
    p_msgq->ref_count ++;
    p_current_task->safe_count ++;
    RTK_EXIT_CRITICAL(old);

    err = rtk_semc_take(&p_msgq->semc_write,tick);
    if (AW_OK == err) {
        /* �п�д�ռ� */
        RTK_ENTER_CRITICAL(old);
        node = p_msgq->free_list_head;
        if (NULL != p_msgq->free_list_head) {
            p_msgq->free_list_head = p_msgq->free_list_head->next;
        }
        RTK_EXIT_CRITICAL(old);

        index = node - p_msgq->p_msgq_nodes;
        *pp_buff = p_msgq->buff + index * p_msgq->unit_size;
    }
    else {
        /* �ȴ������б���ֹ��ʱ */
        /* ��msgq���ٱ����� */
        RTK_ENTER_CRITICAL(old);
        p_msgq->ref_count --;
        p_current_task->safe_count --;
        RTK_EXIT_CRITICAL(old);
    }
func_exit:
    return err;
}

static int msgq_send_end( struct rtk_msgq *p_msgq, void *buff )
{
    int                     index;
    RTK_CRITICAL_STATUS_DECL(old);
    struct rtk_msgq_node   *node;
    struct rtk_task        *p_current_task;

    index = ((uint8_t *)buff - p_msgq->buff)/p_msgq->unit_size;
    node = &p_msgq->p_msgq_nodes[index];

    RTK_ENTER_CRITICAL(old);
    node->next = NULL;
    if ( NULL != p_msgq->full_list_tail ) {
        p_msgq->full_list_tail->next = node;
        p_msgq->full_list_tail = node;
    }
    else {
        p_msgq->full_list_head = node;
        p_msgq->full_list_tail = node;
    }
    RTK_EXIT_CRITICAL(old);

    /* ����һ�������� */
    rtk_semc_give(&p_msgq->semc_read);

    /* ����ref count */
    p_current_task = rtk_task_self();
    RTK_ENTER_CRITICAL(old);
    p_msgq->ref_count --;
    p_current_task->safe_count --;
    RTK_EXIT_CRITICAL(old);

    return 0;
}

int rtk_msgq_send(
        struct rtk_msgq *p_msgq,
        const  void     *buff,
        size_t           size,
        aw_tick_t        tick )
{
    void               *tx = NULL;
    int                 err = AW_OK;
    int                 copy_size;

    OS_TRACE_MSGQ_SEND_ENTER(p_msgq, buff, size, tick);

    /* ������� */
    if (NULL == p_msgq || NULL == buff || 0 == size) {
        err = -AW_EINVAL;
        goto func_exit;
    }

    /* �������ж������У������ж��в�����ȴ� */
    if ( unlikely(rtk_is_int_context()) ) {
       tick = 0;
    }

    err = msgq_send_begin( p_msgq, &tx, tick );
    if ( !err ) {
        if (size < p_msgq->unit_size ) {
            copy_size = size;
        }
        else {
            copy_size = p_msgq->unit_size;
        }
        memcpy( tx, buff, copy_size );
        msgq_send_end( p_msgq, tx );
    }

func_exit:
    OS_TRACE_MSGQ_SEND_EXIT(err);
    return err;
}

int rtk_msgq_clear( struct rtk_msgq *p_msgq)
{
    int                     err ;

    do {
        err = rtk_msgq_receive(p_msgq,NULL,0,0);
    } while(AW_OK == err);

    return 0;
}

int rtk_msgq_is_empty( struct rtk_msgq *p_msgq,int *p_is_empty)
{
    RTK_CRITICAL_STATUS_DECL(old);
    int                     err = AW_OK;
    int                     is_empty = 0;

    /* ������ */
    if (NULL == p_msgq || NULL == p_is_empty ) {
        err = -AW_EINVAL;
        goto func_exit;
    }

    /* �������������ж��������� */
    if ( unlikely(rtk_is_int_context()) ) {
        err = -AW_EPERM;
        goto func_exit;
    }

    RTK_ENTER_CRITICAL(old);
    /* �Ƿ�Ϊ��Ч��msgq */
    if (MSGQ_MAGIC_NUM_VALID !=
            (p_msgq->magic_num + (unsigned long)p_msgq ) ) {
        RTK_EXIT_CRITICAL(old);
        err = -AW_EINVAL;
        goto func_exit;
    }

    if (NULL == p_msgq->full_list_head) {
        is_empty = 1;
    }

    RTK_EXIT_CRITICAL(old);

    *p_is_empty = is_empty;
func_exit:
    return err;
}

int rtk_msgq_is_full( struct rtk_msgq *p_msgq, int *p_is_full)
{
    RTK_CRITICAL_STATUS_DECL(old);
    int                     err = AW_OK;
    int                     is_full = 0;

    /* ������ */
    if (NULL == p_msgq || NULL == p_is_full ) {
        err = -AW_EINVAL;
        goto func_exit;
    }

    /* �������������ж��������� */
    if ( unlikely(rtk_is_int_context()) ) {
        err = -AW_EPERM;
        goto func_exit;
    }

    RTK_ENTER_CRITICAL(old);
    /* �Ƿ�Ϊ��Ч��msgq */
    if (MSGQ_MAGIC_NUM_VALID !=
            (p_msgq->magic_num + (unsigned long)p_msgq ) ) {
        RTK_EXIT_CRITICAL(old);
        err = -AW_EINVAL;
        goto func_exit;
    }

    if (NULL == p_msgq->free_list_head) {
        is_full = 1;
    }

    RTK_EXIT_CRITICAL(old);

    *p_is_full = is_full;
func_exit:
    return err;
}


aw_bool_t rtk_msgq_is_valid( struct rtk_msgq *p_msgq )
{
    RTK_CRITICAL_STATUS_DECL(old);
    int                     err = AW_OK;
    int                     is_full = 0;

    /* ������ */
    if (NULL == p_msgq ) {
        return AW_FALSE;
    }

    RTK_ENTER_CRITICAL(old);
    /* �Ƿ�Ϊ��Ч��msgq */
    if (MSGQ_MAGIC_NUM_VALID !=
            (p_msgq->magic_num + (unsigned long)p_msgq ) ) {
        RTK_EXIT_CRITICAL(old);
        return AW_FALSE;
    }

    RTK_EXIT_CRITICAL(old);
    return AW_TRUE;
}
#endif

/* end of file*/
