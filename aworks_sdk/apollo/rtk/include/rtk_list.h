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
 * \file  rtk_list.h
 * \brief rtk中的双向链表相关结构及函数
 *
 * \internal
 * \par modification history:
 * - 1.00.00 18-07-19 sni, first version
 * \endinternal
 */

#ifndef __RTK_LIST_H__
#define __RTK_LIST_H__

#ifdef __cplusplus
extern "C" {
#endif


/**
 * \brief 定义rtk内部使用的双向链表
 */
typedef struct _rtk_list_node {
    struct _rtk_list_node *next;
    struct _rtk_list_node *prev;
}rtk_list_node_t;

typedef rtk_list_node_t rtk_list_head_t;


/**
 * \brief 初始化链表节点

 * 所有的链表节点都必须被初始化之后才能加入到链表中
 * \param[in] elm       要添加的节点
 * \retval    无
 */
void rtk_list_node_init(rtk_list_node_t *list_node);



/**
 * \brief 将链表节点elm添加到head之后

 * \param[in] elm       要添加的节点
 * \param[in] head      被添加的节点
 * \retval    无
 */
void rtk_list_add(rtk_list_node_t *elm, rtk_list_node_t *head);


/**
 * \brief 将链表节点elm添加到head之前

 * \param[in] elm       要添加的节点
 * \param[in] head      被添加的节点
 * \retval    无
 */
void rtk_list_add_tail(rtk_list_node_t *elm, rtk_list_node_t *head);



/**
 * \brief 将链表节点从链表中删除

 * \param[in] entry     要删除的节点
 * \retval    无
 */
void rtk_list_del(rtk_list_node_t *entry);


/**
 * \brief 将链表节点从链表中删除，并重新初始化该节点

 * \param[in] entry     要删除的节点
 * \retval    无
 */
void rtk_list_del_init(rtk_list_node_t *entry);

/**
 * \brief 从链表头节点判断链表是否为空

 * \param[in] head      链表的头节点
 * \retval    0         链表不为空
 * \retval    非0       链表为空
 */
int rtk_list_empty(const rtk_list_node_t *head);


/**
 * \brief 判断节点是否是链表的最后一个节点

 * \param[in] list      要判断的节点
 * \param[in] head      链表的头节点
 * \retval    0         不是最后一个节点
 *            非0       是最户一个节点
 */
int rtk_list_is_last(const rtk_list_node_t *list,const rtk_list_node_t *head);


/** \brief 由链表头节点，获取链表第一个有效节点 */
#define RTK_LIST_FIRST( head )      ((head)->next)
/** \brief 由链表头节点，获取链表最后有效节点 */
#define RTK_LIST_LAST( head )       ((head)->prev)

/** \brief 遍历链表，如果遍历过程不需要删除节点 */
#define rtk_list_for_each(itervar, list)                                    \
    for (itervar = (list)->next; itervar != (list); itervar = itervar->next)
/** \brief 遍历链表，如果遍历过程需要删除节点 */
#define rtk_list_for_each_safe(itervar, save_var, list)                     \
    for (itervar = (list)->next, save_var = (list)->next->next;         \
         itervar != (list); itervar = save_var, save_var = save_var->next)

/** \brief 由成员指针，获取整个结构体的地址 */
#define RTK_CONTAINER_OF(ptr, type, member) \
    ((type *)((unsigned long)ptr - (unsigned long)(&(((type *)0)->member))))

#define RTK_OFFSET_OF(type,member) ((unsigned long)(&(((type *)0)->member)))

/** \brief 由结构体的链表节点成员指针，获取整个结构体的地址 */
#define RTK_LIST_ENTRY(ptr, type, member)   RTK_CONTAINER_OF(ptr, type, member)

#ifdef __cplusplus
}
#endif

#endif

