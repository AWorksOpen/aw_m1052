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
 * \file  rtk_base_mutex.c
 * \brief 定义rtk_base_mutex的内部操作函数
 *
 * rtk_base_mutex是一个rtk内部使用的锁，用于在某些情况下替换掉直接禁用中断的
 * 全局锁，它主要用来保护内核中的一些数据结构，比它更高级的是禁用中断的全局锁
 * rtk_base_mutex采用的是先进先出的策略，这种策略使得其lock和unlock操作的时间都是
 * O(1)的
 *
 * 之所以采用FIFO策略，主要是因为其操作时间为O(1)
 * 只用于内部，并不影响阻塞在其它内核结构上的任务优先级高的任务先被唤醒
 *
 * 例如用rtk_base_mutex来保护rtk_mutex，低优先级任务先来，获取rtk_base_mutex
 * 后操作rtk_mutex期间，高优先级任务会阻塞在rtk_base_mutex上(原先的方案是直接
 * 禁用中断，此时根本不会发生调度，使用rtk_base_mutex反而降低了中断延迟时间)
 *
 * 低优先级任务处理完后，将自己添加到rtk_mutex的等待队列中，然后高优先级任务
 * 被唤醒，操作rtk_mutex将自己也加入到rtk_mutex等待队列中，
 * 但是肯定会在低优先级任务之前，从而是高优先级任务先被唤醒
 *
 * 在上述过程中，如果在高优先级任务获取rtk_base_mutex之后，将自己添加到rtk_mutex
 * 等待队列之前，因为发生中断，更高优先级的任务可能执行而将rtk_mutex变得可用，
 * 此时更高优先级任务需要先获取rtk_base_mutex，它会等待直到高优先级任务操作完成,
 * 因为在rtk_mutex等待队列中高优先级任务排列更靠前，所以依然会先被唤醒
 * 这也不影响高优先级任务先执行的机制
 *
 * \internal
 * \par modification history:
 * - 2018-05-15  sni, first implement
 * \endinternal
 */

#ifndef         __RTK_BASE_MUTEX_H__
#define         __RTK_BASE_MUTEX_H__
/**
 *  @brief rtk_base_mutex
 */
typedef struct
{
    struct rtk_task    *owner;
    /** \brief 连接任务中保存持有互斥锁链表的节点 */
    rtk_list_node_t     base_mutex_hold_node;   
    /** \brief 互斥锁被递归持有的次数 */
    int                 mutex_recurse_count;    
}rtk_base_mutex_t;

/**
 * \brief 初始化一个rtk_base_mutex
 *        此函数不是用户接口函数，所以请注意不要重复初始化
 *
 * \param[in]   p_base_mutex        rtk_base_mutex对象
 *
 * \return   无
 */
void rtk_base_mutex_init(rtk_base_mutex_t *p_base_mutex);

/**
 * \brief 锁定一个rtk_base_mutex
 *        如果rtk_base_mutex被别的任务占据，当前任务会挂起等待
 * \param[in]   p_base_mutex        rtk_base_mutex对象
 * \return   无
 */
void rtk_base_mutex_lock(rtk_base_mutex_t *p_base_mutex);

/**
 * \brief 解锁一个rtk_base_mutex
 *
 * \param[in]   p_base_mutex        rtk_base_mutex对象
 * \return   无
 */
void rtk_base_mutex_unlock(rtk_base_mutex_t *p_base_mutex);

/**
 * \brief 解锁一个rtk_base_mutex,附加的功能是不发生任务调度
 *
 * \param[in]   p_base_mutex        rtk_base_mutex对象
 * \retval      0                   稍后不需要执行任务调度
 * \retval      1                   稍后需要执行任务调度
 */
int rtk_base_mutex_unlock_no_sche(rtk_base_mutex_t *p_base_mutex);

/**
 * \brief 解锁一个rtk_base_mutex,附加的功能是不发生任务调度
 * 而且此函数仅能够在拥有者任务由变为不就绪以后调用
 * 相对于rtk_base_mutex_unlock_no_sche，因为拥有者任务不再存在于
 * 就绪队列中，所以此函数省去了调整任务在就绪队列中位置的操作
 * 且仅能够在已经进入临界区的环境下操作
 *
 * \param[in]   p_base_mutex        rtk_base_mutex对象
 * \retval      0                   稍后不需要执行任务调度
 * \retval      1                   稍后需要执行任务调度
 */
void rtk_base_mutex_unlock_not_ready_ec(rtk_base_mutex_t *p_base_mutex);

#endif          /*__RTK_BASE_MUTEX_H__*/
