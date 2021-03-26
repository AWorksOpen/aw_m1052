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
 * \file  rtk_tick_down_counter.h.h
 * \brief 定义rtk中涉及到tick counter的函数
 *
 * \internal
 * \par modification history:
 * - 0.9  18-04-23 sni first version.
 * \endinternal
 */


#ifndef             __RTK_TICK_DOWN_COUNTER_H__
#define             __RTK_TICK_DOWN_COUNTER_H__

/**
 *  @addtogroup kernel_structure
 *  @{
 */

/**
 * \brief soft timer structure definition
 *
 *  this is a link list watchdog like soft timer implementation.
 *  The list is sorted at insertion time by 'tick'.
 *  The first element's uiTick is decreased at each tick anounce in the link list.
 */
struct  rtk_tick
{
    rtk_list_node_t     node;          /**< \brief node to the link list     */
    unsigned int        tick;          /**< \brief tick count                */
    void (*timeout_callback)( void *); /**< \brief timeout callback function */
    void               *arg;
};

/**
 *  @}
 */

/**
 * \brief system tick get
 */
unsigned int rtk_tick_get( void );
unsigned long rtk_idle_tick_get( void );

void rtk_tick_down_counter_announce( void );

#if CONFIG_TICK_DOWN_COUNTER_EN>0
void rtk_tick_down_counter_init(struct rtk_tick *_this);
int rtk_tick_down_counter_set_func( struct rtk_tick *_this, void (*func)(void*), void*arg );
void rtk_tick_down_counter_start( struct rtk_tick *_this, unsigned int tick );
void rtk_tick_down_counter_stop ( struct rtk_tick *_this );
#endif


#endif          /* __RTK_TICK_DOWN_COUNTER_H__*/
