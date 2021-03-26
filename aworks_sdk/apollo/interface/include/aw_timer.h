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
 * \brief  软件定时器接口
 *
 * 使用本服务需要包含头文件 aw_timer.h
 *
 * \par 定时器提供周期性服务
 * \code
 *
 * #define TIMER_MS    500
 * static  aw_timer_t  my_timer;
 *
 *  static void my_callback (void *p_arg)
 *  {
 *      // 执行相关操作
 *
 *      // 重启定时器
 *      aw_timer_start(&my_timer, aw_ms_to_tick(TIMER_MS));
 *  }
 *
 *  static void timer_init(void) {
 *   
 *      // 初始化定时器
 *      aw_timer_init(&my_timer, my_callback, 0);
 *
 *      // 启动定时器
 *      aw_timer_start(&my_timer, aw_ms_to_tick(TIMER_MS));
 *
 *      // 等待定时器周期性执行回调函数
 *  }
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.10 13-03-21  zen, refine the description
 * - 1.01 13-02-22  orz, fix the defines and code style.
 * - 1.00 12-10-23  liangyaozhan, first implementation.
 * \endinternal
 */

#ifndef __AW_TIMER_H
#define __AW_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus	*/

#include "aworks.h"
#include "aw_psp_timer.h"

/**
 * \brief 软件定时器类型
 */
typedef aw_psp_timer_t   aw_timer_t;

/**
 * \addtogroup grp_aw_if_timer
 * \copydoc aw_timer.h
 * @{
 */

/**
 * \brief 初始化软件定时器
 *
 * 试图初始化一个正在工作的定时器会破坏内核数据结构。
 * 用户可以先使用 aw_timer_stop() 函数停止定时器后再初始化。
 *
 * \param[in] p_timer   软件定时器
 * \param[in] p_func    定时器回调函数，定时结束时将调用此回调函数并传入其参数
 * \param[in] p_arg     回调函数的参数
 *
 * \return  成功返回软件定时器的ID，失败则返回NULL
 *
 * \sa aw_timer_start(), aw_timer_destroy()
 */
void aw_timer_init (aw_timer_t *p_timer, aw_pfuncvoid_t p_func, void *p_arg);

/**
 * \brief 启动软件定时器
 *
 * 当定时结束时，软件定时器将会自动停止，若需要周期性的定时，则需要在回调函数中
 * 再次调用 aw_timer_start() 。
 *
 * \note 可以被多次启动，以最后一次启动为准。
 *
 * \param[in] p_timer   软件定时器
 * \param[in] ticks     定时长度，单位：系统节拍数。可使用 aw_ms_to_ticks()
 *                      将毫秒数转换为节拍数
 * \sa aw_timer_init()
 */
void aw_timer_start (aw_timer_t *p_timer, unsigned int ticks);

/**
 * \brief 停止软件定时器
 *
 * \param[in] p_timer  软件定时器
 *
 * \sa aw_timer_init(), aw_timer_start()
 */
void aw_timer_stop (aw_timer_t *p_timer);


/**
 * \brief 销毁软件定时器
 *
 * 释放软件定时器占用的资源
 *
 * \param[in] p_timer   软件定时器
 */
aw_inline static void aw_timer_destroy (aw_timer_t *p_timer)
{
    aw_timer_stop(p_timer);     /* FIXME: 在PSP中实现这个函数 */
}

/** @} grp_aw_if_timer */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AW_TIMER_H */

/* end of file */
