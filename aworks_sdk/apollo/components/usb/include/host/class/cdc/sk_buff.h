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

/* 初始化套接字缓存头
 * list：要初始化的套接字缓存头*/
static inline void skb_queue_head_init(struct aw_sk_buff_head *list)
{
    /* 初始化自旋锁*/
    aw_spinlock_isr_init(&list->lock, 0);
    /* 初始化套接字缓存头指针*/
    __skb_queue_head_init(list);
}

/* 插入套接字缓存链表
 * newsk：要插入的套接字缓存
 * prev ：套接字缓存头的前一个元素
 * next ：套接字缓存头
 * list ：套接字缓存头*/
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

/* 检查是不是一个空队列
 * list：套接字缓存头
 * 返回    ：如果为空返回1，否则返回0*/
static inline int skb_queue_empty(const struct aw_sk_buff_head *list)
{
    return list->next == (const struct aw_sk_buff *) list;
}

/* 取消套接字缓存链接
 * skb ：要取消链接的套接字缓存
 * list：套接字缓存头*/
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


/* 获取套接字缓存链表头的第一个套接字缓存元素
 * list_：套接字缓存链表头
 * 返回       ：如果有则返回套接字缓存，没有则返回NULL*/
static inline struct aw_sk_buff *skb_peek(const struct aw_sk_buff_head *list_)
{
    struct aw_sk_buff *skb = list_->next;

    if (skb == (struct aw_sk_buff *)list_)
        skb = NULL;
    return skb;
}

/* 移除套接字缓存
 * list：套接字缓存链表头
 * 返回    ：弹出的套接字缓存*/
static inline struct aw_sk_buff *__skb_dequeue(struct aw_sk_buff_head *list)
{
    struct aw_sk_buff *skb = skb_peek(list);
    if (skb)
        __skb_unlink(skb, list);
    return skb;
}
/* 移除套接字缓存
 * list：套接字缓存链表头
 * 返回    ：弹出的套接字缓存*/
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

