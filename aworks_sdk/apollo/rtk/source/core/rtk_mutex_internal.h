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
 * \brief 定义rtk_mutex的内部操作函数
 *
 * \internal
 * \par modification history:
 * - 1.00 18-05-15  sni, first implement
 * \endinternal
 */

#ifndef         __RTK_MUTEX_INTERNAl_H__
#define         __RTK_MUTEX_INTERNAl_H__

/* 由互斥量中的持有互斥锁链表节点地址得到互斥量的地址 */
#define MUTEX_HOLD_NODE_TO_MUTEX(p_node) \
    RTK_CONTAINER_OF(p_node,struct rtk_mutex,mutex_hold_node)

void __rtk_mutex_global_init(void);

/**
 * \brief 获取等待任务持有的互斥量的任务中最高的优先级
 *
 * \param[in] task          任务控制块
 * \retval   等待任务持有的互斥量的任务中最高的优先级
 */
int __task_hold_mutex_priority_get ( struct rtk_task *task );

/**
 * \brief 提升拥有此互斥量任务的优先级
 *
 * \param[in] p_mutex       要操作的互斥量
 * \param[in] priority      要提升拥有互斥量任务至优先级，如果任务优先级高于
 *                              priority，则无操作
 * \retval 实际执行提升优先级操作的任务数量，0表示没有任务被提升优先级
 */
int __mutex_raise_owner_priority(
        struct rtk_mutex *p_mutex,
        int priority );


#endif          /* __RTK_MUTEX_INTERNAl_H__ */
