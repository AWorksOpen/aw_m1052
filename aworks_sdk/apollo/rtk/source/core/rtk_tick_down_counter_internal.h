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
 * \file  rtk_tick_down_counter_internal.h
 * \brief 定义rtk中涉及到tick counter的内部函数
 *
 * \internal
 * \par modification history:
 * - 0.9 18-04-23 sni first version.
 * \endinternal
 */


#ifndef             __RTK_TICK_DOWN_COUNTER_INTERNAL_H__
#define             __RTK_TICK_DOWN_COUNTER_INTERNAL_H__

void __rtk_tick_down_counter_global_init(void);

void __rtk_tick_down_counter_init(struct rtk_tick *_this);
void __rtk_tick_down_counter_set_func(
        struct rtk_tick *_this,
        void (*func)(void*),
        void *arg );

void __rtk_tick_down_counter_add(
        struct rtk_tick *_this,
        unsigned int tick);

void __rtk_tick_down_counter_remove ( struct rtk_tick *_this );


void __rtk_tick_down_counter_announce_with_ticks( uint32_t tick_count );
void __rtk_tick_down_counter_1_tick( void );

#endif          /* __RTK_TICK_DOWN_COUNTER_INTERNAL_H__*/
