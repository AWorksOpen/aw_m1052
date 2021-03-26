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
 * \file
 * \brief Apollo linked-list and queue definitions
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * #include "aw_list.h"
 * \endcode
 *
 * 本文件由 linux 中的 list.h 修改而来，相关用法请参阅linux链表的相关资料。
 *
 * // 更多内容待添加
 *
 * \internal
 * \par modification history:
 * - 1.00 12-10-23  zen, added
 * \endinternal
 */

#ifndef __AW_LIST_H
#define __AW_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_list
 * \copydoc aw_list.h
 * @{
 */


#include "aw_common.h"

/**
 * \brief  定义空指针
 */
#ifndef AW_LIST_POISON1
#define AW_LIST_POISON1  NULL
#endif

#ifndef AW_LIST_POISON2
#define AW_LIST_POISON2  NULL
#endif

/**
 * \brief  双向链表结构体
 */
struct aw_list_head {
    struct aw_list_head *next, *prev;
};

/**
 * \brief hash链表结构体
 */
struct aw_hlist_head {
    struct aw_hlist_node *first;
};

/**
 * \brief hash链表节点
 */
struct aw_hlist_node {
    struct aw_hlist_node *next, **pprev;
};



#if 0

#define AW_LIST_HEAD_INIT(name) { &(name), &(name) }

#define AW_LIST_HEAD(name) \
    struct aw_list_head name = AW_LIST_HEAD_INIT(name)

#endif


/**
 * \brief 链表初始化
 *
 * \param[in] list  链表头
 */
aw_local aw_inline void AW_INIT_LIST_HEAD(struct aw_list_head *list)
{
    list->next = list;
    list->prev = list;
}
#define aw_list_head_init(list) AW_INIT_LIST_HEAD(list)

aw_local aw_inline void __aw_list_add(struct aw_list_head *elm,
                                      struct aw_list_head *prev,
                                      struct aw_list_head *next)
{
    next->prev = elm;
    elm->next  = next;
    elm->prev  = prev;
    prev->next = elm;
}

/**
 * \brief 将节点添加到链表
 *
 * \param[in] elm   节点
 * \param[in] head  链表头
 *
 * \return 无
 */
aw_local aw_inline void aw_list_add(struct aw_list_head *elm,
                                    struct aw_list_head *head)
{
    __aw_list_add(elm, head, head->next);
}


/**
 * \brief 将节点添加到链表末尾
 *
 * \param[in] elm   节点
 * \param[in] head  链表头
 *
 * \return 无
 */
aw_local aw_inline void aw_list_add_tail(struct aw_list_head *elm,
                                         struct aw_list_head *head)
{
    __aw_list_add(elm, head->prev, head);
}


aw_local aw_inline void __aw_list_del(struct aw_list_head *prev,
                                      struct aw_list_head *next)
{
    next->prev = prev;
    prev->next = next;
}

aw_local aw_inline void __aw_list_del_entry(struct aw_list_head *entry)
{
    __aw_list_del(entry->prev, entry->next);
}

/**
 * \brief 将节点从链表中删除
 *
 * \param[in] entry   要删除的节点
 *
 * \return 无
 */
aw_local aw_inline void aw_list_del(struct aw_list_head *entry)
{
    __aw_list_del(entry->prev, entry->next);

    entry->next = AW_LIST_POISON1;
    entry->prev = AW_LIST_POISON2;
}


/**
 * \brief 用新节点替换旧节点
 *
 * \param[in] old   旧节点
 * \param[in] elm   新节点
 *
 * \return 无
 */
aw_local aw_inline void aw_list_replace(struct aw_list_head *old,
                                        struct aw_list_head *elm)
{
    elm->next = old->next;
    elm->next->prev = elm;
    elm->prev = old->prev;
    elm->prev->next = elm;
}

/**
 * \brief 用新节点替换旧节点，并初始化旧节点
 *
 * \param[in] old   旧节点
 * \param[in] elm   新节点
 *
 * \return 无
 */
aw_local aw_inline void aw_list_replace_init(struct aw_list_head *old,
                                             struct aw_list_head *elm)
{
    aw_list_replace(old, elm);
    AW_INIT_LIST_HEAD(old);
}

/**
 * \brief 将节点从链表中删除，并初始化
 *
 * \param[in] entry   要删除的节点
 * 
 * \return 无
 */
aw_local aw_inline void aw_list_del_init(struct aw_list_head *entry)
{
    __aw_list_del_entry(entry);
    AW_INIT_LIST_HEAD(entry);
}


/**
 * \brief 将节点移动到另一条链表
 *
 * \param[in] list   目标节点
 * \param[in] head   目标链表
 * 
 * \return 无
 */
aw_local aw_inline void aw_list_move(struct aw_list_head *list,
                                     struct aw_list_head *head)
{
    __aw_list_del_entry(list);
    aw_list_add(list, head);
}

/**
 * \brief 将节点移动到另一条链表末尾
 *
 * \param[in] list   目标节点
 * \param[in] head   目标链表
 * 
 * \return 无
 */
aw_local aw_inline void aw_list_move_tail(struct aw_list_head *list,
                                          struct aw_list_head *head)
{
    __aw_list_del_entry(list);
    aw_list_add_tail(list, head);
}

/**
 * \brief 判断该节点是否是链表最后一个节点
 *
 * \param[in] list   目标节点
 * \param[in] head   目标链表
 *
 * \return 是则返回1，不是则返回0
 */
aw_local aw_inline int aw_list_is_last(const struct aw_list_head *list,
                                       const struct aw_list_head *head)
{
    return list->next == head;
}

/**
 * \brief 判断链表是否是空链表
 *
 * \param[in] head   目标链表
 *
 * \return 是则返回1，不是则返回0
 */
aw_local aw_inline int aw_list_empty(const struct aw_list_head *head)
{
    return head->next == head;
}


/**
 * \brief 判断节点head的next和prev是否都指向head
 *
 * \param[in] head   目标链表
 *
 * \return 是则返回1，不是则返回0
 */
aw_local aw_inline int aw_list_empty_careful(const struct aw_list_head *head)
{
    struct aw_list_head *next = head->next;
    return (next == head) && (next == head->prev);
}

/**
 * \brief 将链表的第一个节点翻转到链表末尾
 *
 * \param[in] head   目标链表
 */
aw_local aw_inline void aw_list_rotate_left(struct aw_list_head *head)
{
    struct aw_list_head *first;

    if (!aw_list_empty(head)) {
        first = head->next;
        aw_list_move_tail(first, head);
    }
}

/**
 * \brief 判断链表是否只有一个节点
 *
 * \param[in] head   目标链表
 *
 * \return 是则返回1，不是则返回0
 */
aw_local aw_inline int aw_list_is_singular(const struct aw_list_head *head)
{
    return !aw_list_empty(head) && (head->next == head->prev);
}

aw_local aw_inline void __aw_list_cut_position(struct aw_list_head *list,
                                            struct aw_list_head *head,
                                            struct aw_list_head *entry)
{
    struct aw_list_head *elm_first = entry->next;
    list->next = head->next;
    list->next->prev = list;
    list->prev = entry;
    entry->next = list;
    head->next = elm_first;
    elm_first->prev = head;
}

/**
 * \brief 将head链表从第一个节点开始到entry节点(包括entry)剪切到list链表
 *
 * \param[in] list    新链表
 * \param[in] head    原链表
 * \param[in] entry   位于原链表的目标节点
 *
 * \note 注意新链表list应该为空链表或不关心其中的节点
 *       因为剪切后新链表旧节点会全部丢失
 */
aw_local aw_inline void aw_list_cut_position(struct aw_list_head *list,
                                             struct aw_list_head *head,
                                             struct aw_list_head *entry)
{
    if (aw_list_empty(head))
        return;
    if (aw_list_is_singular(head) &&
        (head->next != entry && head != entry))
        return;
    if (entry == head)
        AW_INIT_LIST_HEAD(list);
    else
        __aw_list_cut_position(list, head, entry);
}

aw_local aw_inline void __aw_list_splice(const struct aw_list_head *list,
                                      struct aw_list_head       *prev,
                                      struct aw_list_head       *next)
{
    struct aw_list_head *first = list->next;
    struct aw_list_head *last = list->prev;

    first->prev = prev;
    prev->next = first;

    last->next = next;
    next->prev = last;
}

/**
 * \brief 将链表list剪切到链表head头部
 *
 * \param[in] list   链表
 * \param[in] head   链表
 *
 * \note 此函数不安全，此时能通过list链表被剪切的节点
 */
aw_local aw_inline void aw_list_splice(const struct aw_list_head *list,
                                       struct aw_list_head       *head)
{
    if (!aw_list_empty(list)) {
        __aw_list_splice(list, head, head->next);
    }
}

/**
 * \brief 将链表list剪切到链表head尾部
 *
 * \param[in] list   链表
 * \param[in] head   链表
 *
 * \note 此函数不安全，此时能通过list链表被剪切的节点
 */
aw_local aw_inline void aw_list_splice_tail(struct aw_list_head *list,
                                            struct aw_list_head *head)
{
    if (!aw_list_empty(list))
        __aw_list_splice(list, head->prev, head);
}

/**
 * \brief 将链表list剪切到链表head头部，并将list初始化
 *
 * \param[in] list   链表
 * \param[in] head   链表
 * 
 * \return 无
 */
aw_local aw_inline void aw_list_splice_init(struct aw_list_head *list,
                                            struct aw_list_head *head)
{
    if (!aw_list_empty(list)) {
        __aw_list_splice(list, head, head->next);
        AW_INIT_LIST_HEAD(list);
    }
}

/**
 * \brief 将链表list剪切到链表head尾部，并将list初始化
 *
 * \param[in] list   链表
 * \param[in] head   链表
 * 
 * \return 无
 */
aw_local aw_inline void aw_list_splice_tail_init(struct aw_list_head *list,
                                                 struct aw_list_head *head)
{
    if (!aw_list_empty(list)) {
        __aw_list_splice(list, head->prev, head);
        AW_INIT_LIST_HEAD(list);
    }
}

/**
 * \brief 获取type类型结构体的起始指针
 *
 * \param[in] ptr     type类型的结构体中member成员的指针
 * \param[in] type    结构体类型
 * \param[in] member  节点在这个结构体定义的名字
 *
 * \return 返回结构体地址
 */
#define aw_list_entry(ptr, type, member) \
    container_of(ptr, type, member)


/**
 * \brief  已知type类型的结构体中member成员的指针后，
 * 求得它所在的链表的下一个指针所指的member所在的type类型的结构体的起始地址
 *
 * \param[in] ptr     type类型的结构体中member成员的指针
 * \param[in] type    结构体类型
 * \param[in] member  节点在这个结构体定义的名字
 *
 * \return 返回结构体地址
 * \attention 链表默认不为空
 */
#define aw_list_first_entry(ptr, type, member) \
    aw_list_entry((ptr)->next, type, member)

/**
 * \brief  从head节点开始（不包括head节点！）遍历它的每一个节点
 *
 * \param[out] pos      存放节点地址，类型为aw_list_head *
 * \param[in]  head     链表地址
 */
#define aw_list_for_each(pos, head) \
    for (pos = (head)->next; \
         prefetch(pos->next), pos != (head); \
         pos = pos->next)

#define __aw_list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * \brief  从head节点开始（不包括head节点）向前遍历每一个节点
 *
 * \param[out] pos      存放节点地址，类型为aw_list_head *
 * \param[in]  head     链表地址
 */
#define aw_list_for_each_prev(pos, head) \
    for (pos = (head)->prev; prefetch(pos->prev), pos != (head); \
            pos = pos->prev)

/**
 * \brief  遍历一条链表，带safe是防止删除节点造成断链的发生
 *
 * \param[out] pos      存放节点地址，类型为aw_list_head *
 * \param[in]  n        临时存放节点地址的变量，类型为aw_list_head *
 * \param[in]  head     链表地址
 */
#define aw_list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
        pos = n, n = pos->next)

/**
 * \brief  前向遍历一条链表，带safe是防止删除节点造成断链的发生
 *
 * \param[out] pos      存放节点地址，类型为aw_list_head *
 * \param[in]  n        临时存放节点地址的变量，类型为aw_list_head *
 * \param[in]  head     链表地址
 */
#define aw_list_for_each_prev_safe(pos, n, head) \
    for (pos = (head)->prev, n = pos->prev; \
         prefetch(pos->prev), pos != (head); \
         pos = n, n = pos->prev)

/**
 * \brief  遍历一条链表，并获取节点所在的数据结构地址
 *
 * \param[out] pos      存放节点所在的数据结构体的地址
 * \param[in]  head     链表地址
 * \param[in]  type     结构体类型定义
 * \param[in]  member   节点在这个结构体定义的名字
 */
#define aw_list_for_each_entry(pos, head, type, member) \
    for (pos = aw_list_entry((head)->next, type, member); \
         prefetch(pos->member.next), &pos->member != (head); \
         pos = aw_list_entry(pos->member.next, type, member))


/**
 * \brief  反向遍历一条链表，并获取节点所在的数据结构地址
 *
 * \param[out] pos      存放节点所在的数据结构体的地址
 * \param[in]  head     链表地址
 * \param[in]  type     结构体类型定义
 * \param[in]  member   节点在这个结构体定义的名字
 */
#define aw_list_for_each_entry_reverse(pos, head, type, member) \
    for (pos = aw_list_entry((head)->prev, type, member); \
         prefetch(pos->member.prev), &pos->member != (head); \
         pos = aw_list_entry(pos->member.prev, type, member))

/**
 * \brief  这个宏是为 aw_list_for_each_entry_continue() 函数做准备的
 *
 * \param[out] pos      结构体地址
 * \param[in]  head     链表地址
 * \param[in]  type     结构体类型定义
 * \param[in]  member   节点在这个结构体定义的名字
 *
 * \return 如果pos非空则返回pos，pos为空则从head链表中强制扩张一个type *类型的指针
 */
#define aw_list_prepare_entry(pos, head, type, member) \
    ((pos) ? : aw_list_entry(head, type, member))

/**
 * \brief  从指定的pos的节点开始遍历链表
 *
 * \param[in,out] pos      指定遍历的开始节点并存放获取到的结构体地址
 * \param[in]     head     链表地址
 * \param[in]     type     结构体类型定义
 * \param[in]     member   节点在这个结构体定义的名字
 */
#define aw_list_for_each_entry_continue(pos, head, type, member) \
    for (pos = aw_list_entry(pos->member.next, type, member); \
         prefetch(pos->member.next), &pos->member != (head); \
         pos = aw_list_entry(pos->member.next, type, member))

/**
 * \brief  从指定的pos的节点开始反向遍历链表
 *
 * \param[in,out] pos      指定遍历的开始节点并存放获取到的结构体地址
 * \param[in]     head     链表地址
 * \param[in]     type     结构体类型定义
 * \param[in]     member   节点在这个结构体定义的名字
 */
#define aw_list_for_each_entry_continue_reverse(pos, head, type, member) \
    for (pos = aw_list_entry(pos->member.prev, type, member); \
         prefetch(pos->member.prev), &pos->member != (head); \
         pos = aw_list_entry(pos->member.prev, type, member))

/**
 * \brief  从当前的点pos开始遍历链表
 *
 * \param[in]     pos      指定遍历的开始节点并存放获取到的结构体地址
 * \param[in]     head     链表地址
 * \param[in]     type     结构体类型定义
 * \param[in]     member   节点在这个结构体定义的名字
 */
#define aw_list_for_each_entry_from(pos, head, type, member) \
    for (; prefetch(pos->member.next), &pos->member != (head); \
         pos = aw_list_entry(pos->member.next, type, member))

/**
 * \brief  从当前的点pos开始遍历链表，带safe是防止删除节点造成断链的发生
 *
 * \param[in]     pos      指定遍历的开始节点并存放获取到的结构体地址
 * \param[in]     n        存放临时节点结构体地址的变量
 * \param[in]     head     链表地址
 * \param[in]     type     结构体类型定义
 * \param[in]     member   节点在这个结构体定义的名字
 */
#define aw_list_for_each_entry_safe(pos, n, head, type, member) \
    for (pos = aw_list_entry((head)->next, type, member), \
        n = aw_list_entry(pos->member.next, type, member); \
         &pos->member != (head); \
         pos = n, n = aw_list_entry(n->member.next, type, member))

/**
 * \brief  从指定的pos的节点开始遍历链表，带safe是防止删除节点造成断链的发生
 *
 * \param[in,out] pos      指定遍历的开始节点并存放获取到的结构体地址
 * \param[in]     n        存放临时节点结构体地址的变量
 * \param[in]     head     链表地址
 * \param[in]     type     结构体类型定义
 * \param[in]     member   节点在这个结构体定义的名字
 */
#define aw_list_for_each_entry_safe_continue(pos, n, head, type, member) \
    for (pos = aw_list_entry(pos->member.next, type, member), \
        n = aw_list_entry(pos->member.next, type, member); \
         &pos->member != (head); \
         pos = n, n = aw_list_entry(n->member.next, type, member))

/**
 * \brief  从当前的点pos开始遍历链表，带safe是防止删除节点造成断链的发生
 *
 * \param[in]     pos      指定遍历的开始节点并存放获取到的结构体地址
 * \param[in]     n        存放临时节点结构体地址的变量
 * \param[in]     head     链表地址
 * \param[in]     type     结构体类型定义
 * \param[in]     member   节点在这个结构体定义的名字
 */
#define aw_list_for_each_entry_safe_from(pos, n, head, type, member) \
    for (n = aw_list_entry(pos->member.next, type, member); \
         &pos->member != (head); \
         pos = n, n = aw_list_entry(n->member.next, type, member))

/**
 * \brief  从当前的点pos开始反向遍历链表，带safe是防止删除节点造成断链的发生
 *
 * \param[in]     pos      指定遍历的开始节点并存放获取到的结构体地址
 * \param[in]     n        存放临时节点结构体地址的变量
 * \param[in]     head     链表地址
 * \param[in]     type     结构体类型定义
 * \param[in]     member   节点在这个结构体定义的名字
 */
#define aw_list_for_each_entry_safe_reverse(pos, n, head, type, member) \
    for (pos = aw_list_entry((head)->prev, type, member), \
        n = aw_list_entry(pos->member.prev, type, member); \
         &pos->member != (head); \
         pos = n, n = aw_list_entry(n->member.prev, type, member))


/**
 * \brief  获取n结构体指针
 *
 * \param[in]     pos      指定遍历的开始节点并存放获取到的结构体地址
 * \param[in]     n        存放临时节点结构体地址的变量
 * \param[in]     type     结构体类型定义
 * \param[in]     member   节点在这个结构体定义的名字
 */
#define list_safe_reset_next(pos, n, type, member) \
    n = aw_list_entry(pos->member.next, type, member)


#if 0
#define AW_HLIST_HEAD_INIT { .first = NULL }
#define AW_HLIST_HEAD(name) struct aw_hlist_head name = {  .first = NULL }
#endif


/** \brief 初始化hash链表 */
#define AW_INIT_HLIST_HEAD(ptr) ((ptr)->first = NULL)

/**
 * \brief  初始化hash链表节点
 *
 * \param[in] h hash链表节点指针
 * 
 * \return 无
 */
aw_local aw_inline void AW_INIT_HLIST_NODE(struct aw_hlist_node *h)
{
    h->next = NULL;
    h->pprev = NULL;
}

/**
 * \brief  判断节点是否在hash链表中
 *
 * \param[in] h   hash节点指针
 *
 * \return 是则返回1，否则返回0
 */
aw_local aw_inline int aw_hlist_unhashed(const struct aw_hlist_node *h)
{
    return !h->pprev;
}

/**
 * \brief  判断hash链表是否为空
 *
 * \param[in] h   hash链表
 *
 * \return 是则返回1，否则返回0
 */
aw_local aw_inline int aw_hlist_empty(const struct aw_hlist_head *h)
{
    return !h->first;
}

aw_local aw_inline void __aw_hlist_del(struct aw_hlist_node *n)
{
    struct aw_hlist_node *next = n->next;
    struct aw_hlist_node **pprev = n->pprev;
    *pprev = next;
    if (next)
        next->pprev = pprev;
}

/**
 * \brief  删除节点
 *
 * \param[in] n   hash节点指针
 * 
 * \return 无
 */
aw_local aw_inline void aw_hlist_del(struct aw_hlist_node *n)
{
    __aw_hlist_del(n);
    n->next = AW_LIST_POISON1;
    n->pprev = AW_LIST_POISON2;
}

/**
 * \brief  删除节点，并初始化
 *
 * \param[in] n   hash节点指针
 * 
 * \return 无
 */
aw_local aw_inline void aw_hlist_del_init(struct aw_hlist_node *n)
{
    if (!aw_hlist_unhashed(n)) {
        __aw_hlist_del(n);
        AW_INIT_HLIST_NODE(n);
    }
}

/**
 * \brief  添加节点到hash链表头
 *
 * \param[in] n   hash节点指针
 * \param[in] h   hash链表指针
 * 
 * \return 无
 */
aw_local aw_inline void aw_hlist_add_head(struct aw_hlist_node *n,
                                          struct aw_hlist_head *h)
{
    struct aw_hlist_node *first = h->first;
    n->next = first;
    if (first)
        first->pprev = &n->next;
    h->first = n;
    n->pprev = &h->first;
}

/**
 * \brief  将节点n添加到节点next之前
 *
 * \param[in] n      hash节点指针
 * \param[in] next   hash节点指针
 *
 * \return 无
 * \note  next必须不为NULL
 */
aw_local aw_inline void aw_hlist_add_before(struct aw_hlist_node *n,
                                            struct aw_hlist_node *next)
{
    n->pprev = next->pprev;
    n->next = next;
    next->pprev = &n->next;
    *(n->pprev) = n;
}

/**
 * \brief  将节点n添加到节点next之后
 *
 * \param[in] n      hash节点指针
 * \param[in] next   hash节点指针
 *
 * 
 * \return 无
 * \note  next必须不为NULL
 */
aw_local aw_inline void aw_hlist_add_after(struct aw_hlist_node *n,
                                           struct aw_hlist_node *next)
{
    next->next = n->next;
    n->next = next;
    next->pprev = &n->next;

    if(next->next)
        next->next->pprev  = &next->next;
}

/**
 * \brief  设置n->pprev = &n->next
 *
 * \param[in] n      hash节点指针
 * 
 * \return 无
 */
aw_local aw_inline void aw_hlist_add_fake(struct aw_hlist_node *n)
{
    n->pprev = &n->next;
}

/**
 * \brief  节点elm接管节点old的所有节点，并初始化old
 *
 * \param[in] old     旧hash节点指针
 * \param[in] elm     新hash节点指针
 * 
 * \return 无
 */
aw_local aw_inline void aw_hlist_move_list(struct aw_hlist_head *old,
                                           struct aw_hlist_head *elm)
{
    elm->first = old->first;
    if (elm->first)
        elm->first->pprev = &elm->first;
    old->first = NULL;
}

/**
 * \brief  获取type类型的结构体的起始地址
 *
 * \param[in] ptr     hash节点指针
 * \param[in] type    结构体类型
 * \param[in] member  节点在结构体中定义的名字
 */
#define aw_hlist_entry(ptr, type, member) container_of(ptr,type,member)

/**
 * \brief  遍历hash链表
 *
 * \param[in] pos     hash节点指针
 * \param[in] head    链表头
 */
#define aw_hlist_for_each(pos, head) \
    for (pos = (head)->first; pos && ({ prefetch(pos->next); 1; }); \
         pos = pos->next)

/**
 * \brief  遍历hash链表
 *
 * \param[in] pos     hash节点指针
 * \param[in] n       hash节点指针缓存
 * \param[in] head    链表头
 */
#define aw_hlist_for_each_safe(pos, n, head) \
    for (pos = (head)->first; pos && ({ n = pos->next; 1; }); \
         pos = n)

/**
 * \brief  遍历hash链表，获取type类型的结构体指针
 *
 * \param[in] tpos    存放type类型的结构体指针
 * \param[in] pos     存放hash节点指针
 * \param[in] head    链表头
 * \param[in] type    结构体类型
 * \param[in] member  节点在结构体中定义的名字
 */
#define aw_hlist_for_each_entry(tpos, pos, head, type, member) \
    for (pos = (head)->first; \
         pos && ({ prefetch(pos->next); 1;}) && \
        ({ tpos = aw_hlist_entry(pos, type, member); 1;}); \
         pos = pos->next)

/**
 * \brief  从结点pos下一个遍历找type类型的结构体地址
 *
 * \param[in] tpos    存放type类型的结构体指针
 * \param[in] pos     存放hash节点指针
 * \param[in] type    结构体类型
 * \param[in] member  节点在结构体中定义的名字
 */
#define aw_hlist_for_each_entry_continue(tpos, pos, type, member) \
    for (pos = (pos)->next; \
         pos && ({ prefetch(pos->next); 1;}) && \
        ({ tpos = aw_hlist_entry(pos, type, member); 1;}); \
         pos = pos->next)

/**
 * \brief 从结点pos开始遍历找type类型的结构体地址
 *
 * \param[in] tpos    存放type类型的结构体指针
 * \param[in] pos     存放hash节点指针
 * \param[in] type    结构体类型
 * \param[in] member  节点在结构体中定义的名字
 */
#define aw_hlist_for_each_entry_from(tpos, pos, type, member) \
    for (; pos && ({ prefetch(pos->next); 1;}) && \
        ({ tpos = aw_hlist_entry(pos, type, member); 1;}); \
         pos = pos->next)

/**
 * \brief  从结点pos开始遍历找type类型的结构体地址
 *
 * \param[in] tpos    存放type类型的结构体指针
 * \param[in] pos     节点缓存
 * \param[in] n       节点缓存
 * \param[in] head    起始节点
 * \param[in] type    结构体类型
 * \param[in] member  节点在结构体中定义的名字
 */
#define aw_hlist_for_each_entry_safe(tpos, pos, n, head, type, member) \
    for (pos = (head)->first; \
         pos && ({ n = pos->next; 1; }) && \
        ({ tpos = aw_hlist_entry(pos, type, member); 1;}); \
         pos = n)

/** @}  grp_aw_if_list */

#ifdef __cplusplus
}
#endif

#endif /* __AW_LIST_H */

/* end of file */
