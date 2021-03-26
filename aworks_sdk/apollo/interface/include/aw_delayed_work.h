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
 * \brief 延迟作业服务
 *
 *  - 延迟作业的功能是把某项工作延迟一段时间在处理，和定时器的使用类似
 *  - 和定时器不同的是延迟作业的执行环境为任务而非中断
 *  - 通常需要非精确延迟（或周期）执行的任务可使用延迟作业服务
 *
 * 使用本服务需要包含头文件 aw_delayed_work.h
 *
 * \par 使用示例
 * \code
 * struct aw_delayed_work work;
 *
 * static void my_work (void *p_arg)
 * {
 *     ; // 实际工作
 *
 *     // 如果这里再次启动work，则work变成周期任务
 *     // aw_delayed_work_start((struct aw_delayed_work *)p_arg, 500);
 * }
 *
 * aw_delayed_work_init(&work, my_work, &work); // 初始化work
 * aw_delayed_work_start(&work, 500);           // 500毫秒后启动my_work
 * ......
 * aw_delayed_work_stop(&work);                 // 停止work
 * \endcode
 *
 * // 更多内容待添加。。。
 *
 * \internal
 * \par modification history
 * - 1.00 13-03-06  orz, first implementation.
 * \endinternal
 */

#ifndef __AW_DELAYED_WORK_H
#define __AW_DELAYED_WORK_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_delayed_work
 * \copydoc aw_delayed_work.h
 * @{
 */

#include "aw_types.h"
#include "aw_isr_defer.h"
#include "aw_timer.h"

/** \brief 延迟作业数据结构 */
struct aw_delayed_work {
    struct aw_isr_defer_job  job; /**< \brief the job queue */
    aw_timer_t               tmr; /**< \brief the delay timer */
};

/******************************************************************************/

/**
 * \brief 延迟作业初始化
 *
 * \param[in,out] p_work        延迟作业数据结构指针
 * \param[in]     pfunc_work    工作函数，该函数被延迟调用
 * \param[in]     p_arg         工作函数的参数
 *
 * \return 无
 */
void aw_delayed_work_init (struct aw_delayed_work *p_work,
                           void                  (*pfunc_work)(void *p_arg),
                           void                   *p_arg);

/**
 * \brief 启动延迟作业
 *
 * \param[in] p_work        延迟作业数据结构指针
 * \param[in] delay_ms      延迟毫秒数
 *
 * \return 无
 */
void aw_delayed_work_start(struct aw_delayed_work *p_work, uint_t delay_ms);

/**
 * \brief 停止延迟作业
 *
 * \note  这个接口只能停止处于延迟状态的作业，不能停止已经开始处理的作业
 *
 * \param[in] p_work        延迟作业数据结构指针
 *
 * \return 无
 */
void aw_delayed_work_stop (struct aw_delayed_work *p_work);

/**
 * \brief 销毁延迟作业
 *
 * \note  这个接口会停止处于延迟状态的作业，对于队列中等待处理的作业，
 *        会从队列中进行删除。
 *
 * \param[in] p_work        延迟作业数据结构指针
 *
 * \return 无
 */
void aw_delayed_work_destroy (struct aw_delayed_work *p_work);

/** @} grp_aw_if_delayed_work */

#ifdef __cplusplus
}
#endif

#endif /* __AW_DELAYED_WORK_H */

/* end of file */
