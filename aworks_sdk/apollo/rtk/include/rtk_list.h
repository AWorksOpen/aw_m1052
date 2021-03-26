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
 * \brief rtk�е�˫��������ؽṹ������
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
 * \brief ����rtk�ڲ�ʹ�õ�˫������
 */
typedef struct _rtk_list_node {
    struct _rtk_list_node *next;
    struct _rtk_list_node *prev;
}rtk_list_node_t;

typedef rtk_list_node_t rtk_list_head_t;


/**
 * \brief ��ʼ������ڵ�

 * ���е�����ڵ㶼���뱻��ʼ��֮����ܼ��뵽������
 * \param[in] elm       Ҫ��ӵĽڵ�
 * \retval    ��
 */
void rtk_list_node_init(rtk_list_node_t *list_node);



/**
 * \brief ������ڵ�elm��ӵ�head֮��

 * \param[in] elm       Ҫ��ӵĽڵ�
 * \param[in] head      ����ӵĽڵ�
 * \retval    ��
 */
void rtk_list_add(rtk_list_node_t *elm, rtk_list_node_t *head);


/**
 * \brief ������ڵ�elm��ӵ�head֮ǰ

 * \param[in] elm       Ҫ��ӵĽڵ�
 * \param[in] head      ����ӵĽڵ�
 * \retval    ��
 */
void rtk_list_add_tail(rtk_list_node_t *elm, rtk_list_node_t *head);



/**
 * \brief ������ڵ��������ɾ��

 * \param[in] entry     Ҫɾ���Ľڵ�
 * \retval    ��
 */
void rtk_list_del(rtk_list_node_t *entry);


/**
 * \brief ������ڵ��������ɾ���������³�ʼ���ýڵ�

 * \param[in] entry     Ҫɾ���Ľڵ�
 * \retval    ��
 */
void rtk_list_del_init(rtk_list_node_t *entry);

/**
 * \brief ������ͷ�ڵ��ж������Ƿ�Ϊ��

 * \param[in] head      �����ͷ�ڵ�
 * \retval    0         ����Ϊ��
 * \retval    ��0       ����Ϊ��
 */
int rtk_list_empty(const rtk_list_node_t *head);


/**
 * \brief �жϽڵ��Ƿ�����������һ���ڵ�

 * \param[in] list      Ҫ�жϵĽڵ�
 * \param[in] head      �����ͷ�ڵ�
 * \retval    0         �������һ���ڵ�
 *            ��0       ���һ���ڵ�
 */
int rtk_list_is_last(const rtk_list_node_t *list,const rtk_list_node_t *head);


/** \brief ������ͷ�ڵ㣬��ȡ�����һ����Ч�ڵ� */
#define RTK_LIST_FIRST( head )      ((head)->next)
/** \brief ������ͷ�ڵ㣬��ȡ���������Ч�ڵ� */
#define RTK_LIST_LAST( head )       ((head)->prev)

/** \brief ������������������̲���Ҫɾ���ڵ� */
#define rtk_list_for_each(itervar, list)                                    \
    for (itervar = (list)->next; itervar != (list); itervar = itervar->next)
/** \brief ���������������������Ҫɾ���ڵ� */
#define rtk_list_for_each_safe(itervar, save_var, list)                     \
    for (itervar = (list)->next, save_var = (list)->next->next;         \
         itervar != (list); itervar = save_var, save_var = save_var->next)

/** \brief �ɳ�Աָ�룬��ȡ�����ṹ��ĵ�ַ */
#define RTK_CONTAINER_OF(ptr, type, member) \
    ((type *)((unsigned long)ptr - (unsigned long)(&(((type *)0)->member))))

#define RTK_OFFSET_OF(type,member) ((unsigned long)(&(((type *)0)->member)))

/** \brief �ɽṹ�������ڵ��Աָ�룬��ȡ�����ṹ��ĵ�ַ */
#define RTK_LIST_ENTRY(ptr, type, member)   RTK_CONTAINER_OF(ptr, type, member)

#ifdef __cplusplus
}
#endif

#endif

