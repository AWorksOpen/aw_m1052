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
 * \file  rtk_wait_queue.h
 * \brief 定义rtk_wait_queue的操作函数
 *
 * \internal
 * \par modification history:
 * - 1.00 18-05-15  sni, first implement
 * \endinternal
 */

#ifndef         __RTK_WAITQ_INTERNAL_H__
#define         __RTK_WAITQ_INTERNAL_H__

void __rtk_waitq_init(
        rtk_wait_queue_t *p_wait_queue,
        rtk_wait_queue_type_t type,
        rtk_wait_queue_option_t option);

/**
 * \brief 重排序等待在此等待队列上的任务
 *
 * 当task的优先级改变后调用此函数
 * 作用是按新的优先级重新排列task在等待p_wait_queue任务的位置
 * 返回值表明了重新排列后task在所有等待p_wait_queue任务中是否优先级最高
 *
 * \param[in]   p_wait_queue    要执行操作的等待队列
 * \param[in]   task            要重新排序的任务
 * \retval      0   重新排列后task在所有等待p_wait_queue任务中优先级不是最高
 * \retval      非0 重新排列后task在所有等待p_wait_queue任务中优先级最高
 */
int __rtk_waitq_resort_pender_ec(
        rtk_wait_queue_t *p_wait_queue,
        struct rtk_task *task);

/**
 * \brief 弹出一个等待在此等待队列上的任务
 *          这个任务将在接下来被唤醒
 *          等待队列上再无此pender
 *
 * \param[in] p_wait_queue  要执行操作的等待队列
 * \retval    弹出的任务
 */
struct rtk_task *__rtk_waitq_pop_1_pender_ec(rtk_wait_queue_t *p_wait_queue);

/**
 * \brief 获取此等待队列上优先级最高的pender
 *
 * \param[in] p_wait_queue  要执行操作的等待队列
 * \retval   此等待队列上优先级最高的pender，无pender返回NULL
 */
struct rtk_task *__rtk_waitq_highest_pender_get_lc(
        rtk_wait_queue_t *p_wait_queue);

/**
 * \brief 判断等待队列上是否有任务在等待
 *
 * \param[in] p_wait_queue  要执行操作的等待队列
 * \retval      0       没有任务在等待
 *              其它值  有任务再等待
 */
int __rtk_waitq_has_penders_ec(
        rtk_wait_queue_t *p_wait_queue );


/**
 * \brief 获取等待在此等待队列上的任务的最高优先级
 *
 * \param[in] p_wait_queue  要执行操作的等待队列
 * \retval   等待在此等待队列上的任务的最高优先级
 */
int __rtk_waitq_highest_pri_get_lc (
        rtk_wait_queue_t *p_wait_queue );

/**
 * \brief 添加任务到等待队列，使得任务开始等待此等待队列
 *
 * \param[in]   task            要添加的任务
 * \param[in]   p_wait_queue    要执行操作的等待队列
 * \retval   无
 */
void __rtk_waitq_add_pender_ec(
        struct rtk_task *task ,
        rtk_wait_queue_t *p_wait_queue );



/**
 * \brief 将任务从等待队列上移除
 * \param[in]   task            要移除的任务
 * \retval      无
 */
void __task_detach_pending_waitq_ec( struct rtk_task *task );

int __rtk_waitq_terminate( rtk_wait_queue_t *p_wait_queue );


#endif          //__RTK_WAIT_QUEUE_H__
