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
 * \brief 软件定时器平台相关标准定义头文件
 *
 * \internal
 * \par modification history:
 * - 1.11 13-03-21  liangyaozhan, adjust API.
 * - 1.10 13-03-21  zen, refine the description
 * - 1.00 12-11-13  liangyaozhan, first implementation
 * \endinternal
 */

#ifndef __AW_PSP_TIMER_H
#define __AW_PSP_TIMER_H

#include "rtk.h"

/******************************************************************************/
typedef struct rtk_tick aw_psp_timer_t;

/******************************************************************************/
static aw_inline void aw_timer_start (aw_psp_timer_t *p_timer,
                                      unsigned int ticks)
{
    rtk_tick_down_counter_start(p_timer, ticks);
}

/******************************************************************************/
static aw_inline void aw_timer_stop (aw_psp_timer_t *p_timer)
{
    rtk_tick_down_counter_stop(p_timer);
}

/******************************************************************************/
static aw_inline void aw_timer_init (aw_psp_timer_t *p_timer,
                                     aw_pfuncvoid_t   p_func,
                                     void            *p_arg)
{
    rtk_tick_down_counter_init(p_timer);
    rtk_tick_down_counter_set_func(p_timer, p_func, p_arg);
}

#endif  /* __AW_PSP_TIMER_H */

/* end of file */
