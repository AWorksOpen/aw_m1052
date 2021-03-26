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
 * \file  rtk_mutex_internal.h
 * \brief ����rtk_mutex���ڲ���������
 *
 * \internal
 * \par modification history:
 * - 1.00 18-05-15  sni, first implement
 * \endinternal
 */

#ifndef         __RTK_MUTEX_INTERNAl_H__
#define         __RTK_MUTEX_INTERNAl_H__

/* �ɻ������еĳ��л���������ڵ��ַ�õ��������ĵ�ַ */
#define MUTEX_HOLD_NODE_TO_MUTEX(p_node) \
    RTK_CONTAINER_OF(p_node,struct rtk_mutex,mutex_hold_node)

void __rtk_mutex_global_init(void);

/**
 * \brief ��ȡ�ȴ�������еĻ���������������ߵ����ȼ�
 *
 * \param[in] task          ������ƿ�
 * \retval   �ȴ�������еĻ���������������ߵ����ȼ�
 */
int __task_hold_mutex_priority_get ( struct rtk_task *task );

/**
 * \brief ����ӵ�д˻�������������ȼ�
 *
 * \param[in] p_mutex       Ҫ�����Ļ�����
 * \param[in] priority      Ҫ����ӵ�л��������������ȼ�������������ȼ�����
 *                              priority�����޲���
 * \retval ʵ��ִ���������ȼ�����������������0��ʾû�������������ȼ�
 */
int __mutex_raise_owner_priority(
        struct rtk_mutex *p_mutex,
        int priority );


#endif          /* __RTK_MUTEX_INTERNAl_H__ */
