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
 * \file  rtk_semc.h
 * \brief 定义rtk计数信号量相关结构和接口函数
 *
 * \internal
 * \par modification history:
 * - 1.00 18-05-15  sni, first implement
 * \endinternal
 */

#ifndef         __RTK_SEMC_H__
#define         __RTK_SEMC_H__

/**
 *  @defgroup SEMAPHORE_COUNTER_API semaphore counter API
 *  @{
 */


struct rtk_semc
{
    rtk_wait_queue_t        wait_queue;
    unsigned int            count;
    unsigned long           magic_num;
};


/**
 * \brief semaphore counter declaration macro.
 */
#define RTK_SEMC_DECL(name) struct rtk_semc name

/**
 * \brief 初始化一个计数信号量
 *
 * 不能在中断中调用
 *
 * \param[in] p_semc        struct rtk_semaphore_counter结构
 * \param[in] InitCount     计数信号量初始计数值
 * \retval    失败返回NULL，成功则返回执行rtk_semaphore_counter结构的指针
 */
struct rtk_semc * rtk_semc_init(
        struct rtk_semc *p_semc,
        unsigned int InitCount );

/**
 * \brief 反初始化一个计数信号量
 *
 * 不能在中断中调用，任何一个成功初始化的信号量都可以调用rtk_semc_terminate
 * 来反初始化，反初始化会唤醒所以等待在此信号量上的任务，那些任务的take操作会
 * 得到返回值-AW_ENXIO
 *
 * \param[in] p_semc        struct rtk_semaphore_counter结构
 * \retval    0             成功返回0，否则返回负的错误码
 */
int rtk_semc_terminate( struct rtk_semc *p_semc );

/**
 * \brief 重新设置一个计数信号量的值
 *
 * 不能在中断中调用，要想成功重置，则此p_semc上不能有任何阻塞着的take操作
 *
 * \param[in] p_semc        struct rtk_semaphore_counter结构
 * \param[in] reset_value   新的计数信号量的值
 * \retval    0             成功返回0，否则返回负的错误码
 */
int rtk_semc_reset( struct rtk_semc *p_semc, unsigned int reset_value );

/**
 * \brief 获取一个计数信号量
 *
 * 可在中断中调用，行为相当于tick == 0
 *
 * \param[in] p_semc        struct rtk_semaphore_counter结构
 * \param[in] tick          用system tick计数的最大等待时间
 *                          0表示无论是否获取成功，都会立即返回
 *                          AW_WAIT_FOREVER，表示会永久等待
 * \retval  0               成功返回0，否则返回负的错误码
 * \retval  -AW_EINVAL      参数无效
 * \retval  -AW_ETIME       获取超时
 * \retval  -AW_ENXIO       信号量被其它任务终止了
 * \retval  -AW_EAGAIN      需要重试，只有tick==0并且计数信号量不可用才会返回
 */
int rtk_semc_take( struct rtk_semc *p_semc, unsigned int tick );

/**
 * \brief 释放一个计数信号量
 *
 * 可在中断中调用
 *
 * \param[in] p_semc        struct rtk_semaphore_counter结构
 * \retval  0               成功返回0，否则返回负的错误码
 * \retval  -AW_EINVAL      参数无效
 * \retval  -AW_ENOSPC      semc没有空间了，这意味着semc的计数值已经达到最大
 */
int rtk_semc_give( struct rtk_semc *p_semc );

/**
 *  @}
 */

#endif          //__RTK_SEMC_H__
