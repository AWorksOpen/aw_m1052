/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

#ifndef __SK_BUFF_H__
#define __SK_BUFF_H__
#include "apollo.h"
#include "host/class/cdc/awbl_usbh_usbnet.h"


#define skb_queue_walk(queue, skb) \
        for (skb = (queue)->next;                   \
             skb != (struct aw_sk_buff *)(queue);              \
             skb = skb->next)

static inline void __skb_queue_head_init(struct aw_sk_buff_head *list)
{
    list->prev = list->next = (struct aw_sk_buff *)list;
    list->qlen = 0;
}

/* ��ʼ���׽��ֻ���ͷ
 * list��Ҫ��ʼ�����׽��ֻ���ͷ*/
static inline void skb_queue_head_init(struct aw_sk_buff_head *list)
{
    /* ��ʼ��������*/
    aw_spinlock_isr_init(&list->lock, 0);
    /* ��ʼ���׽��ֻ���ͷָ��*/
    __skb_queue_head_init(list);
}

/* �����׽��ֻ�������
 * newsk��Ҫ������׽��ֻ���
 * prev ���׽��ֻ���ͷ��ǰһ��Ԫ��
 * next ���׽��ֻ���ͷ
 * list ���׽��ֻ���ͷ*/
static inline void __skb_insert(struct aw_sk_buff *newsk,
                struct aw_sk_buff *prev, struct aw_sk_buff *next,
                struct aw_sk_buff_head *list)
{
    newsk->next = next;
    newsk->prev = prev;
    next->prev  = prev->next = newsk;
    list->qlen++;
}

static inline void __skb_queue_before(struct aw_sk_buff_head *list,
                      struct aw_sk_buff *next,
                      struct aw_sk_buff *newsk)
{
    __skb_insert(newsk, next->prev, next, list);
}


static inline void __skb_queue_tail(struct aw_sk_buff_head *list,
                   struct aw_sk_buff *newsk)
{
    __skb_queue_before(list, (struct aw_sk_buff *)list, newsk);
}

/* ����ǲ���һ���ն���
 * list���׽��ֻ���ͷ
 * ����    �����Ϊ�շ���1�����򷵻�0*/
static inline int skb_queue_empty(const struct aw_sk_buff_head *list)
{
    return list->next == (const struct aw_sk_buff *) list;
}

/* ȡ���׽��ֻ�������
 * skb ��Ҫȡ�����ӵ��׽��ֻ���
 * list���׽��ֻ���ͷ*/
static inline void __skb_unlink(struct aw_sk_buff *skb, struct aw_sk_buff_head *list)
{
    struct aw_sk_buff *next, *prev;

    list->qlen--;
    next       = skb->next;
    prev       = skb->prev;
    skb->next  = skb->prev = NULL;
    next->prev = prev;
    prev->next = next;
}


/* ��ȡ�׽��ֻ�������ͷ�ĵ�һ���׽��ֻ���Ԫ��
 * list_���׽��ֻ�������ͷ
 * ����       ��������򷵻��׽��ֻ��棬û���򷵻�NULL*/
static inline struct aw_sk_buff *skb_peek(const struct aw_sk_buff_head *list_)
{
    struct aw_sk_buff *skb = list_->next;

    if (skb == (struct aw_sk_buff *)list_)
        skb = NULL;
    return skb;
}

/* �Ƴ��׽��ֻ���
 * list���׽��ֻ�������ͷ
 * ����    ���������׽��ֻ���*/
static inline struct aw_sk_buff *__skb_dequeue(struct aw_sk_buff_head *list)
{
    struct aw_sk_buff *skb = skb_peek(list);
    if (skb)
        __skb_unlink(skb, list);
    return skb;
}
/* �Ƴ��׽��ֻ���
 * list���׽��ֻ�������ͷ
 * ����    ���������׽��ֻ���*/
static inline struct aw_sk_buff *skb_dequeue(struct aw_sk_buff_head *list)
{
    struct aw_sk_buff *result;

    aw_spinlock_isr_take(&list->lock);
    result = __skb_dequeue(list);
    aw_spinlock_isr_give(&list->lock);
    return result;
}

static inline void skb_queue_tail(struct aw_sk_buff_head *list, struct aw_sk_buff *newsk)
{
    aw_spinlock_isr_take(&list->lock);
    __skb_queue_tail(list, newsk);
    aw_spinlock_isr_give(&list->lock);
}

#endif

