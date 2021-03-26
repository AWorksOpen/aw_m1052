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
 * \file  rtk_mutex.h
 * \brief rtk关于mutex的结构和函数
 *
 * \internal
 * \par modification history:
 * - 1.00 18-05-15  sni, first implement
 * \endinternal
 */

#ifndef         __RTK_MUTEX_H__
#define         __RTK_MUTEX_H__

struct rtk_mutex
{
    rtk_wait_queue_t    wait_queue;
    struct rtk_task    *owner;
    /** \brief 连接任务中保存持有互斥锁链表的节点 */
    rtk_list_node_t     mutex_hold_node;     
    /** \brief 互斥锁被递归持有的次数 */    
    unsigned long       mutex_recurse_count;    
    unsigned long       magic_num;
};

/**
 *  @brief mutex declaration macro.
 */
#define RTK_MUTEX_DECL(var) struct rtk_mutex var

/**
 * \brief 初始化一个互斥锁
 *
 * 不能在中断中调用
 *
 * \param[in] p_mutex       struct rtk_mutex结构
 *
 * \retval    0             成功，否则返回负的错误码
 */
int rtk_mutex_init( struct rtk_mutex *p_mutex );

/**
 *  \brief aquire a mutex lock.
 *
 *  \param[in] p_mutex    mutex pointer
 *  \param[in] tick     max waiting time in systerm tick.
 *                      if tick == 0, it will return immedately without block.
 *                      if tick == -1, it will wait forever.
 *
 *  \return     0       successfully.
 *  \return     -EPERM  permission denied.
 *  \return     -EINVAL Invalid argument
 *  \return     -ETIME  time out.
 *  \return     -EDEADLK Deadlock condition detected.
 *  \return     -ENXIO  mutex is terminated by other task or interrupt service routine.
 *  \return     -EAGAIN Try again. Only when tick==0 and mutex is not available.
 *
 *  \attention          cannot be used in interrupt service.
 */
/**
 * \brief 获取一个互斥锁
 *
 * 不可在中断中调用
 *
 * \param[in] p_mutex       struct rtk_mutex结构
 * \param[in] tick          用system tick计数的最大等待时间
 *                          0表示无论是否获取成功，都会立即返回
 *                          AW_WAIT_FOREVER，表示会永久等待
 *
 * \retval  0               成功返回0，否则返回负的错误码
 * \retval  -AW_EINVAL      参数无效
 * \retval  -AW_ETIME       获取超时
 * \retval  -AW_ENXIO       互斥锁被其它任务终止了
 * \retval  -AW_EAGAIN      需要重试，只有tick==0并且不能锁定互斥锁才会返回
 * \retval  -AW_EPERM       在中断中调用
 */
int rtk_mutex_lock( struct rtk_mutex *p_mutex, unsigned int tick );

/**
 * \brief 释放一个互斥锁
 *
 * 不可在中断中调用
 *
 * \param[in] p_mutex       struct rtk_mutex结构
 *
 * \retval  0               成功返回0，否则返回负的错误码
 * \retval  -AW_EINVAL      参数无效
 * \retval  -AW_EPERM       在中断中调用，或者当前任务不是mutex的owener
 */
int rtk_mutex_unlock( struct rtk_mutex *p_mutex );

/**
 * \brief 反初始化一个互斥锁
 *
 * 不能在中断中调用，任何一个成功初始化的互斥锁都可以调用rtk_mutex_terminate
 * 来反初始化，反初始化会唤醒所以等待在此信号量上的任务，那些任务的lock操作会
 * 得到返回值-AW_ENXIO
 *
 * \param[in] p_mutex       struct rtk_mutex结构
 * \retval    0             成功返回0，否则返回负的错误码
 * \retval    -AW_EPERM     没有权限进行此操作，例如在中断中操作
 * \retval    -AW_EINVAL    无效的参数
 */
int rtk_mutex_terminate( struct rtk_mutex *p_mutex );


#endif          //__RTK_MUTEX_H__
