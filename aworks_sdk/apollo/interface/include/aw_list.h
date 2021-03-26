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
 * ʹ�ñ�ģ����Ҫ��������ͷ�ļ���
 * \code
 * #include "aw_list.h"
 * \endcode
 *
 * ���ļ��� linux �е� list.h �޸Ķ���������÷������linux�����������ϡ�
 *
 * // �������ݴ����
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
 * \brief  �����ָ��
 */
#ifndef AW_LIST_POISON1
#define AW_LIST_POISON1  NULL
#endif

#ifndef AW_LIST_POISON2
#define AW_LIST_POISON2  NULL
#endif

/**
 * \brief  ˫������ṹ��
 */
struct aw_list_head {
    struct aw_list_head *next, *prev;
};

/**
 * \brief hash����ṹ��
 */
struct aw_hlist_head {
    struct aw_hlist_node *first;
};

/**
 * \brief hash����ڵ�
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
 * \brief �����ʼ��
 *
 * \param[in] list  ����ͷ
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
 * \brief ���ڵ���ӵ�����
 *
 * \param[in] elm   �ڵ�
 * \param[in] head  ����ͷ
 *
 * \return ��
 */
aw_local aw_inline void aw_list_add(struct aw_list_head *elm,
                                    struct aw_list_head *head)
{
    __aw_list_add(elm, head, head->next);
}


/**
 * \brief ���ڵ���ӵ�����ĩβ
 *
 * \param[in] elm   �ڵ�
 * \param[in] head  ����ͷ
 *
 * \return ��
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
 * \brief ���ڵ��������ɾ��
 *
 * \param[in] entry   Ҫɾ���Ľڵ�
 *
 * \return ��
 */
aw_local aw_inline void aw_list_del(struct aw_list_head *entry)
{
    __aw_list_del(entry->prev, entry->next);

    entry->next = AW_LIST_POISON1;
    entry->prev = AW_LIST_POISON2;
}


/**
 * \brief ���½ڵ��滻�ɽڵ�
 *
 * \param[in] old   �ɽڵ�
 * \param[in] elm   �½ڵ�
 *
 * \return ��
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
 * \brief ���½ڵ��滻�ɽڵ㣬����ʼ���ɽڵ�
 *
 * \param[in] old   �ɽڵ�
 * \param[in] elm   �½ڵ�
 *
 * \return ��
 */
aw_local aw_inline void aw_list_replace_init(struct aw_list_head *old,
                                             struct aw_list_head *elm)
{
    aw_list_replace(old, elm);
    AW_INIT_LIST_HEAD(old);
}

/**
 * \brief ���ڵ��������ɾ��������ʼ��
 *
 * \param[in] entry   Ҫɾ���Ľڵ�
 * 
 * \return ��
 */
aw_local aw_inline void aw_list_del_init(struct aw_list_head *entry)
{
    __aw_list_del_entry(entry);
    AW_INIT_LIST_HEAD(entry);
}


/**
 * \brief ���ڵ��ƶ�����һ������
 *
 * \param[in] list   Ŀ��ڵ�
 * \param[in] head   Ŀ������
 * 
 * \return ��
 */
aw_local aw_inline void aw_list_move(struct aw_list_head *list,
                                     struct aw_list_head *head)
{
    __aw_list_del_entry(list);
    aw_list_add(list, head);
}

/**
 * \brief ���ڵ��ƶ�����һ������ĩβ
 *
 * \param[in] list   Ŀ��ڵ�
 * \param[in] head   Ŀ������
 * 
 * \return ��
 */
aw_local aw_inline void aw_list_move_tail(struct aw_list_head *list,
                                          struct aw_list_head *head)
{
    __aw_list_del_entry(list);
    aw_list_add_tail(list, head);
}

/**
 * \brief �жϸýڵ��Ƿ����������һ���ڵ�
 *
 * \param[in] list   Ŀ��ڵ�
 * \param[in] head   Ŀ������
 *
 * \return ���򷵻�1�������򷵻�0
 */
aw_local aw_inline int aw_list_is_last(const struct aw_list_head *list,
                                       const struct aw_list_head *head)
{
    return list->next == head;
}

/**
 * \brief �ж������Ƿ��ǿ�����
 *
 * \param[in] head   Ŀ������
 *
 * \return ���򷵻�1�������򷵻�0
 */
aw_local aw_inline int aw_list_empty(const struct aw_list_head *head)
{
    return head->next == head;
}


/**
 * \brief �жϽڵ�head��next��prev�Ƿ�ָ��head
 *
 * \param[in] head   Ŀ������
 *
 * \return ���򷵻�1�������򷵻�0
 */
aw_local aw_inline int aw_list_empty_careful(const struct aw_list_head *head)
{
    struct aw_list_head *next = head->next;
    return (next == head) && (next == head->prev);
}

/**
 * \brief ������ĵ�һ���ڵ㷭ת������ĩβ
 *
 * \param[in] head   Ŀ������
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
 * \brief �ж������Ƿ�ֻ��һ���ڵ�
 *
 * \param[in] head   Ŀ������
 *
 * \return ���򷵻�1�������򷵻�0
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
 * \brief ��head����ӵ�һ���ڵ㿪ʼ��entry�ڵ�(����entry)���е�list����
 *
 * \param[in] list    ������
 * \param[in] head    ԭ����
 * \param[in] entry   λ��ԭ�����Ŀ��ڵ�
 *
 * \note ע��������listӦ��Ϊ������򲻹������еĽڵ�
 *       ��Ϊ���к�������ɽڵ��ȫ����ʧ
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
 * \brief ������list���е�����headͷ��
 *
 * \param[in] list   ����
 * \param[in] head   ����
 *
 * \note �˺�������ȫ����ʱ��ͨ��list�������еĽڵ�
 */
aw_local aw_inline void aw_list_splice(const struct aw_list_head *list,
                                       struct aw_list_head       *head)
{
    if (!aw_list_empty(list)) {
        __aw_list_splice(list, head, head->next);
    }
}

/**
 * \brief ������list���е�����headβ��
 *
 * \param[in] list   ����
 * \param[in] head   ����
 *
 * \note �˺�������ȫ����ʱ��ͨ��list�������еĽڵ�
 */
aw_local aw_inline void aw_list_splice_tail(struct aw_list_head *list,
                                            struct aw_list_head *head)
{
    if (!aw_list_empty(list))
        __aw_list_splice(list, head->prev, head);
}

/**
 * \brief ������list���е�����headͷ��������list��ʼ��
 *
 * \param[in] list   ����
 * \param[in] head   ����
 * 
 * \return ��
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
 * \brief ������list���е�����headβ��������list��ʼ��
 *
 * \param[in] list   ����
 * \param[in] head   ����
 * 
 * \return ��
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
 * \brief ��ȡtype���ͽṹ�����ʼָ��
 *
 * \param[in] ptr     type���͵Ľṹ����member��Ա��ָ��
 * \param[in] type    �ṹ������
 * \param[in] member  �ڵ�������ṹ�嶨�������
 *
 * \return ���ؽṹ���ַ
 */
#define aw_list_entry(ptr, type, member) \
    container_of(ptr, type, member)


/**
 * \brief  ��֪type���͵Ľṹ����member��Ա��ָ���
 * ��������ڵ��������һ��ָ����ָ��member���ڵ�type���͵Ľṹ�����ʼ��ַ
 *
 * \param[in] ptr     type���͵Ľṹ����member��Ա��ָ��
 * \param[in] type    �ṹ������
 * \param[in] member  �ڵ�������ṹ�嶨�������
 *
 * \return ���ؽṹ���ַ
 * \attention ����Ĭ�ϲ�Ϊ��
 */
#define aw_list_first_entry(ptr, type, member) \
    aw_list_entry((ptr)->next, type, member)

/**
 * \brief  ��head�ڵ㿪ʼ��������head�ڵ㣡����������ÿһ���ڵ�
 *
 * \param[out] pos      ��Žڵ��ַ������Ϊaw_list_head *
 * \param[in]  head     �����ַ
 */
#define aw_list_for_each(pos, head) \
    for (pos = (head)->next; \
         prefetch(pos->next), pos != (head); \
         pos = pos->next)

#define __aw_list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * \brief  ��head�ڵ㿪ʼ��������head�ڵ㣩��ǰ����ÿһ���ڵ�
 *
 * \param[out] pos      ��Žڵ��ַ������Ϊaw_list_head *
 * \param[in]  head     �����ַ
 */
#define aw_list_for_each_prev(pos, head) \
    for (pos = (head)->prev; prefetch(pos->prev), pos != (head); \
            pos = pos->prev)

/**
 * \brief  ����һ��������safe�Ƿ�ֹɾ���ڵ���ɶ����ķ���
 *
 * \param[out] pos      ��Žڵ��ַ������Ϊaw_list_head *
 * \param[in]  n        ��ʱ��Žڵ��ַ�ı���������Ϊaw_list_head *
 * \param[in]  head     �����ַ
 */
#define aw_list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
        pos = n, n = pos->next)

/**
 * \brief  ǰ�����һ��������safe�Ƿ�ֹɾ���ڵ���ɶ����ķ���
 *
 * \param[out] pos      ��Žڵ��ַ������Ϊaw_list_head *
 * \param[in]  n        ��ʱ��Žڵ��ַ�ı���������Ϊaw_list_head *
 * \param[in]  head     �����ַ
 */
#define aw_list_for_each_prev_safe(pos, n, head) \
    for (pos = (head)->prev, n = pos->prev; \
         prefetch(pos->prev), pos != (head); \
         pos = n, n = pos->prev)

/**
 * \brief  ����һ����������ȡ�ڵ����ڵ����ݽṹ��ַ
 *
 * \param[out] pos      ��Žڵ����ڵ����ݽṹ��ĵ�ַ
 * \param[in]  head     �����ַ
 * \param[in]  type     �ṹ�����Ͷ���
 * \param[in]  member   �ڵ�������ṹ�嶨�������
 */
#define aw_list_for_each_entry(pos, head, type, member) \
    for (pos = aw_list_entry((head)->next, type, member); \
         prefetch(pos->member.next), &pos->member != (head); \
         pos = aw_list_entry(pos->member.next, type, member))


/**
 * \brief  �������һ����������ȡ�ڵ����ڵ����ݽṹ��ַ
 *
 * \param[out] pos      ��Žڵ����ڵ����ݽṹ��ĵ�ַ
 * \param[in]  head     �����ַ
 * \param[in]  type     �ṹ�����Ͷ���
 * \param[in]  member   �ڵ�������ṹ�嶨�������
 */
#define aw_list_for_each_entry_reverse(pos, head, type, member) \
    for (pos = aw_list_entry((head)->prev, type, member); \
         prefetch(pos->member.prev), &pos->member != (head); \
         pos = aw_list_entry(pos->member.prev, type, member))

/**
 * \brief  �������Ϊ aw_list_for_each_entry_continue() ������׼����
 *
 * \param[out] pos      �ṹ���ַ
 * \param[in]  head     �����ַ
 * \param[in]  type     �ṹ�����Ͷ���
 * \param[in]  member   �ڵ�������ṹ�嶨�������
 *
 * \return ���pos�ǿ��򷵻�pos��posΪ�����head������ǿ������һ��type *���͵�ָ��
 */
#define aw_list_prepare_entry(pos, head, type, member) \
    ((pos) ? : aw_list_entry(head, type, member))

/**
 * \brief  ��ָ����pos�Ľڵ㿪ʼ��������
 *
 * \param[in,out] pos      ָ�������Ŀ�ʼ�ڵ㲢��Ż�ȡ���Ľṹ���ַ
 * \param[in]     head     �����ַ
 * \param[in]     type     �ṹ�����Ͷ���
 * \param[in]     member   �ڵ�������ṹ�嶨�������
 */
#define aw_list_for_each_entry_continue(pos, head, type, member) \
    for (pos = aw_list_entry(pos->member.next, type, member); \
         prefetch(pos->member.next), &pos->member != (head); \
         pos = aw_list_entry(pos->member.next, type, member))

/**
 * \brief  ��ָ����pos�Ľڵ㿪ʼ�����������
 *
 * \param[in,out] pos      ָ�������Ŀ�ʼ�ڵ㲢��Ż�ȡ���Ľṹ���ַ
 * \param[in]     head     �����ַ
 * \param[in]     type     �ṹ�����Ͷ���
 * \param[in]     member   �ڵ�������ṹ�嶨�������
 */
#define aw_list_for_each_entry_continue_reverse(pos, head, type, member) \
    for (pos = aw_list_entry(pos->member.prev, type, member); \
         prefetch(pos->member.prev), &pos->member != (head); \
         pos = aw_list_entry(pos->member.prev, type, member))

/**
 * \brief  �ӵ�ǰ�ĵ�pos��ʼ��������
 *
 * \param[in]     pos      ָ�������Ŀ�ʼ�ڵ㲢��Ż�ȡ���Ľṹ���ַ
 * \param[in]     head     �����ַ
 * \param[in]     type     �ṹ�����Ͷ���
 * \param[in]     member   �ڵ�������ṹ�嶨�������
 */
#define aw_list_for_each_entry_from(pos, head, type, member) \
    for (; prefetch(pos->member.next), &pos->member != (head); \
         pos = aw_list_entry(pos->member.next, type, member))

/**
 * \brief  �ӵ�ǰ�ĵ�pos��ʼ����������safe�Ƿ�ֹɾ���ڵ���ɶ����ķ���
 *
 * \param[in]     pos      ָ�������Ŀ�ʼ�ڵ㲢��Ż�ȡ���Ľṹ���ַ
 * \param[in]     n        �����ʱ�ڵ�ṹ���ַ�ı���
 * \param[in]     head     �����ַ
 * \param[in]     type     �ṹ�����Ͷ���
 * \param[in]     member   �ڵ�������ṹ�嶨�������
 */
#define aw_list_for_each_entry_safe(pos, n, head, type, member) \
    for (pos = aw_list_entry((head)->next, type, member), \
        n = aw_list_entry(pos->member.next, type, member); \
         &pos->member != (head); \
         pos = n, n = aw_list_entry(n->member.next, type, member))

/**
 * \brief  ��ָ����pos�Ľڵ㿪ʼ����������safe�Ƿ�ֹɾ���ڵ���ɶ����ķ���
 *
 * \param[in,out] pos      ָ�������Ŀ�ʼ�ڵ㲢��Ż�ȡ���Ľṹ���ַ
 * \param[in]     n        �����ʱ�ڵ�ṹ���ַ�ı���
 * \param[in]     head     �����ַ
 * \param[in]     type     �ṹ�����Ͷ���
 * \param[in]     member   �ڵ�������ṹ�嶨�������
 */
#define aw_list_for_each_entry_safe_continue(pos, n, head, type, member) \
    for (pos = aw_list_entry(pos->member.next, type, member), \
        n = aw_list_entry(pos->member.next, type, member); \
         &pos->member != (head); \
         pos = n, n = aw_list_entry(n->member.next, type, member))

/**
 * \brief  �ӵ�ǰ�ĵ�pos��ʼ����������safe�Ƿ�ֹɾ���ڵ���ɶ����ķ���
 *
 * \param[in]     pos      ָ�������Ŀ�ʼ�ڵ㲢��Ż�ȡ���Ľṹ���ַ
 * \param[in]     n        �����ʱ�ڵ�ṹ���ַ�ı���
 * \param[in]     head     �����ַ
 * \param[in]     type     �ṹ�����Ͷ���
 * \param[in]     member   �ڵ�������ṹ�嶨�������
 */
#define aw_list_for_each_entry_safe_from(pos, n, head, type, member) \
    for (n = aw_list_entry(pos->member.next, type, member); \
         &pos->member != (head); \
         pos = n, n = aw_list_entry(n->member.next, type, member))

/**
 * \brief  �ӵ�ǰ�ĵ�pos��ʼ�������������safe�Ƿ�ֹɾ���ڵ���ɶ����ķ���
 *
 * \param[in]     pos      ָ�������Ŀ�ʼ�ڵ㲢��Ż�ȡ���Ľṹ���ַ
 * \param[in]     n        �����ʱ�ڵ�ṹ���ַ�ı���
 * \param[in]     head     �����ַ
 * \param[in]     type     �ṹ�����Ͷ���
 * \param[in]     member   �ڵ�������ṹ�嶨�������
 */
#define aw_list_for_each_entry_safe_reverse(pos, n, head, type, member) \
    for (pos = aw_list_entry((head)->prev, type, member), \
        n = aw_list_entry(pos->member.prev, type, member); \
         &pos->member != (head); \
         pos = n, n = aw_list_entry(n->member.prev, type, member))


/**
 * \brief  ��ȡn�ṹ��ָ��
 *
 * \param[in]     pos      ָ�������Ŀ�ʼ�ڵ㲢��Ż�ȡ���Ľṹ���ַ
 * \param[in]     n        �����ʱ�ڵ�ṹ���ַ�ı���
 * \param[in]     type     �ṹ�����Ͷ���
 * \param[in]     member   �ڵ�������ṹ�嶨�������
 */
#define list_safe_reset_next(pos, n, type, member) \
    n = aw_list_entry(pos->member.next, type, member)


#if 0
#define AW_HLIST_HEAD_INIT { .first = NULL }
#define AW_HLIST_HEAD(name) struct aw_hlist_head name = {  .first = NULL }
#endif


/** \brief ��ʼ��hash���� */
#define AW_INIT_HLIST_HEAD(ptr) ((ptr)->first = NULL)

/**
 * \brief  ��ʼ��hash����ڵ�
 *
 * \param[in] h hash����ڵ�ָ��
 * 
 * \return ��
 */
aw_local aw_inline void AW_INIT_HLIST_NODE(struct aw_hlist_node *h)
{
    h->next = NULL;
    h->pprev = NULL;
}

/**
 * \brief  �жϽڵ��Ƿ���hash������
 *
 * \param[in] h   hash�ڵ�ָ��
 *
 * \return ���򷵻�1�����򷵻�0
 */
aw_local aw_inline int aw_hlist_unhashed(const struct aw_hlist_node *h)
{
    return !h->pprev;
}

/**
 * \brief  �ж�hash�����Ƿ�Ϊ��
 *
 * \param[in] h   hash����
 *
 * \return ���򷵻�1�����򷵻�0
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
 * \brief  ɾ���ڵ�
 *
 * \param[in] n   hash�ڵ�ָ��
 * 
 * \return ��
 */
aw_local aw_inline void aw_hlist_del(struct aw_hlist_node *n)
{
    __aw_hlist_del(n);
    n->next = AW_LIST_POISON1;
    n->pprev = AW_LIST_POISON2;
}

/**
 * \brief  ɾ���ڵ㣬����ʼ��
 *
 * \param[in] n   hash�ڵ�ָ��
 * 
 * \return ��
 */
aw_local aw_inline void aw_hlist_del_init(struct aw_hlist_node *n)
{
    if (!aw_hlist_unhashed(n)) {
        __aw_hlist_del(n);
        AW_INIT_HLIST_NODE(n);
    }
}

/**
 * \brief  ��ӽڵ㵽hash����ͷ
 *
 * \param[in] n   hash�ڵ�ָ��
 * \param[in] h   hash����ָ��
 * 
 * \return ��
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
 * \brief  ���ڵ�n��ӵ��ڵ�next֮ǰ
 *
 * \param[in] n      hash�ڵ�ָ��
 * \param[in] next   hash�ڵ�ָ��
 *
 * \return ��
 * \note  next���벻ΪNULL
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
 * \brief  ���ڵ�n��ӵ��ڵ�next֮��
 *
 * \param[in] n      hash�ڵ�ָ��
 * \param[in] next   hash�ڵ�ָ��
 *
 * 
 * \return ��
 * \note  next���벻ΪNULL
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
 * \brief  ����n->pprev = &n->next
 *
 * \param[in] n      hash�ڵ�ָ��
 * 
 * \return ��
 */
aw_local aw_inline void aw_hlist_add_fake(struct aw_hlist_node *n)
{
    n->pprev = &n->next;
}

/**
 * \brief  �ڵ�elm�ӹܽڵ�old�����нڵ㣬����ʼ��old
 *
 * \param[in] old     ��hash�ڵ�ָ��
 * \param[in] elm     ��hash�ڵ�ָ��
 * 
 * \return ��
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
 * \brief  ��ȡtype���͵Ľṹ�����ʼ��ַ
 *
 * \param[in] ptr     hash�ڵ�ָ��
 * \param[in] type    �ṹ������
 * \param[in] member  �ڵ��ڽṹ���ж��������
 */
#define aw_hlist_entry(ptr, type, member) container_of(ptr,type,member)

/**
 * \brief  ����hash����
 *
 * \param[in] pos     hash�ڵ�ָ��
 * \param[in] head    ����ͷ
 */
#define aw_hlist_for_each(pos, head) \
    for (pos = (head)->first; pos && ({ prefetch(pos->next); 1; }); \
         pos = pos->next)

/**
 * \brief  ����hash����
 *
 * \param[in] pos     hash�ڵ�ָ��
 * \param[in] n       hash�ڵ�ָ�뻺��
 * \param[in] head    ����ͷ
 */
#define aw_hlist_for_each_safe(pos, n, head) \
    for (pos = (head)->first; pos && ({ n = pos->next; 1; }); \
         pos = n)

/**
 * \brief  ����hash������ȡtype���͵Ľṹ��ָ��
 *
 * \param[in] tpos    ���type���͵Ľṹ��ָ��
 * \param[in] pos     ���hash�ڵ�ָ��
 * \param[in] head    ����ͷ
 * \param[in] type    �ṹ������
 * \param[in] member  �ڵ��ڽṹ���ж��������
 */
#define aw_hlist_for_each_entry(tpos, pos, head, type, member) \
    for (pos = (head)->first; \
         pos && ({ prefetch(pos->next); 1;}) && \
        ({ tpos = aw_hlist_entry(pos, type, member); 1;}); \
         pos = pos->next)

/**
 * \brief  �ӽ��pos��һ��������type���͵Ľṹ���ַ
 *
 * \param[in] tpos    ���type���͵Ľṹ��ָ��
 * \param[in] pos     ���hash�ڵ�ָ��
 * \param[in] type    �ṹ������
 * \param[in] member  �ڵ��ڽṹ���ж��������
 */
#define aw_hlist_for_each_entry_continue(tpos, pos, type, member) \
    for (pos = (pos)->next; \
         pos && ({ prefetch(pos->next); 1;}) && \
        ({ tpos = aw_hlist_entry(pos, type, member); 1;}); \
         pos = pos->next)

/**
 * \brief �ӽ��pos��ʼ������type���͵Ľṹ���ַ
 *
 * \param[in] tpos    ���type���͵Ľṹ��ָ��
 * \param[in] pos     ���hash�ڵ�ָ��
 * \param[in] type    �ṹ������
 * \param[in] member  �ڵ��ڽṹ���ж��������
 */
#define aw_hlist_for_each_entry_from(tpos, pos, type, member) \
    for (; pos && ({ prefetch(pos->next); 1;}) && \
        ({ tpos = aw_hlist_entry(pos, type, member); 1;}); \
         pos = pos->next)

/**
 * \brief  �ӽ��pos��ʼ������type���͵Ľṹ���ַ
 *
 * \param[in] tpos    ���type���͵Ľṹ��ָ��
 * \param[in] pos     �ڵ㻺��
 * \param[in] n       �ڵ㻺��
 * \param[in] head    ��ʼ�ڵ�
 * \param[in] type    �ṹ������
 * \param[in] member  �ڵ��ڽṹ���ж��������
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
