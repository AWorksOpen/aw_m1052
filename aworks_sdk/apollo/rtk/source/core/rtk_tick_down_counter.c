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
 * \file  rtk_tick_down_counter.h
 * \brief 定义rtk中涉及到tick down的内部函数
 *
 * \internal
 * \par modification history:
 * - 0.9 18-04-23 sni first version.
 * \endinternal
 */

#include "rtk.h"
#include "aw_common.h"
#include "rtk_mem.h"
#include "aw_errno.h"
#include <string.h> /* for memset() and memcpy() */
#include "rtk_arch.h"
#include "rtk_task_internal.h"
#include "rtk_priority_q_internal.h"
#include "rtk_tick_down_counter_internal.h"
#include "rtk_systick.h"
#include "os_trace_events.h"


static rtk_list_node_t              g_softtime_head;
volatile unsigned long              g_systick;

void __rtk_tick_down_counter_global_init(void)
{
    rtk_list_node_init(&g_softtime_head);
    g_systick = 0;
    __rtk_systick_init();
}

void __rtk_tick_down_counter_set_func(
        struct rtk_tick *_this,
        void (*func)(void*),
        void *arg )
{
    _this->timeout_callback = func;
    _this->arg              = arg;
}

void __rtk_tick_down_counter_init(struct rtk_tick *_this)
{
    rtk_list_node_init( &_this->node );
}

void __rtk_tick_down_counter_add( struct rtk_tick *_this, unsigned int tick )
{
    rtk_list_node_t    *p;
    struct rtk_tick    *p_tick = 0;
    RTK_SYSTICK_LOCK_DECL(systick_lock);
    RTK_CRITICAL_STATUS_DECL(old);

    RTK_SYSTICK_LOCK(systick_lock);

    rtk_list_for_each( p, &g_softtime_head ) {
        p_tick = RTK_LIST_ENTRY(p, struct rtk_tick, node);
        if (tick >= p_tick->tick ) {
            tick -= p_tick->tick;
        } else {
            break;
        }
    }
    _this->tick = tick;

    RTK_ENTER_CRITICAL(old);
    rtk_list_add_tail( &_this->node, p);
    RTK_EXIT_CRITICAL(old);

    if (p != &g_softtime_head ) {
        p_tick->tick -= tick;
    }
    RTK_SYSTICK_UNLOCK(systick_lock);
}

void __rtk_tick_down_counter_remove ( struct rtk_tick *_this )
{
    struct rtk_tick    *p_tick;
    RTK_SYSTICK_LOCK_DECL(systick_lock);
    RTK_CRITICAL_STATUS_DECL(old);

    RTK_SYSTICK_LOCK(systick_lock);

    if ( rtk_list_empty(&_this->node) ) {
        RTK_SYSTICK_UNLOCK(systick_lock);
        return ;
    }


    if ( RTK_LIST_FIRST( &_this->node ) != &g_softtime_head ) {
        p_tick = RTK_LIST_ENTRY( RTK_LIST_FIRST( &_this->node ), struct rtk_tick, node);
        p_tick->tick += _this->tick;
    }
    RTK_ENTER_CRITICAL(old);
    rtk_list_del_init( &_this->node );
    RTK_EXIT_CRITICAL(old);

    RTK_SYSTICK_UNLOCK(systick_lock);
}

unsigned long rtk_tick_down_get_expect_idle_tick()
{
    unsigned long       idle_time = (unsigned long) -1;
    RTK_SYSTICK_LOCK_DECL(systick_lock);
    rtk_list_node_t    *current_node;
    struct rtk_tick    *_this;

    RTK_SYSTICK_LOCK(systick_lock);

    if ( !rtk_list_empty( &g_softtime_head ) ) {
        current_node = RTK_LIST_FIRST( &g_softtime_head );
        _this = RTK_LIST_ENTRY(current_node, struct rtk_tick, node);
        idle_time = _this->tick;
    }

    RTK_SYSTICK_UNLOCK(systick_lock);
    return idle_time;
}

void __rtk_tick_down_counter_1_tick( void )
{
    RTK_SYSTICK_LOCK_DECL(systick_lock);
    RTK_CRITICAL_STATUS_DECL(old);
    rtk_list_node_t    *current_node;
    struct rtk_tick    *p_cur_tick;

    RTK_SYSTICK_LOCK(systick_lock);
    g_systick ++;
    if ( !rtk_list_empty( &g_softtime_head ) ) {
        current_node = RTK_LIST_FIRST( &g_softtime_head );
        p_cur_tick = RTK_LIST_ENTRY(current_node, struct rtk_tick, node);

        p_cur_tick->tick --;
        while (0 == p_cur_tick->tick) {
            RTK_ENTER_CRITICAL(old);
            rtk_list_del_init( &p_cur_tick->node );
            if ( p_cur_tick->timeout_callback )
            {
                OS_TRACE_TIMER_ENTER(p_cur_tick);
                (*p_cur_tick->timeout_callback )( p_cur_tick->arg );
                OS_TRACE_TIMER_EXIT();
            }
            RTK_EXIT_CRITICAL(old);

            if (rtk_list_empty(&g_softtime_head)) {
                break;
            }
            current_node = RTK_LIST_FIRST( &g_softtime_head );
            p_cur_tick = RTK_LIST_ENTRY(current_node, struct rtk_tick, node);
        }
    }

    RTK_SYSTICK_UNLOCK(systick_lock)
}

#if 0

void __rtk_tick_down_counter_announce_with_ticks( uint32_t tick_count )
{
    void               *p_lock;
    RTK_CRITICAL_STATUS_DECL(old);
    rtk_list_node_t    *current_node;
    struct rtk_tick    *_this;

    p_lock = __rtk_systick_lock();

    if ( !rtk_list_empty( &g_softtime_head ) ) {
        do {
            current_node = RTK_LIST_FIRST( &g_softtime_head );
            _this = RTK_LIST_ENTRY(current_node, struct rtk_tick, node);

            /*
             * in case of 'task_delay(0)'
             */
            if ( _this->tick >= tick_count ) {
                _this->tick -= tick_count;
                g_systick += tick_count;
                tick_count = 0;
            }
            else {
                g_systick += _this->tick;
                tick_count -= _this->tick;
                _this->tick = 0;

            }

            for (; !rtk_list_empty(&g_softtime_head); ) {
                current_node = RTK_LIST_FIRST( &g_softtime_head );
                _this = RTK_LIST_ENTRY(current_node, struct rtk_tick, node);
                if ( _this->tick == 0) {
                    RTK_ENTER_CRITICAL(old);
                    rtk_list_del_init( &_this->node );
                    if ( _this->timeout_callback )
                    {
                        OS_TRACE_TIMER_ENTER(_this);

                        (*_this->timeout_callback )( _this->arg );

                        OS_TRACE_TIMER_EXIT();
                    }
                    RTK_EXIT_CRITICAL(old);
                } else {
                    /*goto DoneOK; */
                    goto cleanup;
                }
            }

            if (0 == tick_count) {
                break;
            }
        } while ( !rtk_list_empty( &g_softtime_head ) );
    }
    else {
        g_systick += tick_count;
    }
cleanup:
    __rtk_systick_unlock(p_lock);
}
#endif

#if CONFIG_TICK_DOWN_COUNTER_EN>0

void rtk_tick_down_counter_init(struct rtk_tick *_this)
{
    __rtk_tick_down_counter_init(_this);
}

int rtk_tick_down_counter_set_func( struct rtk_tick *_this,
                                    void (*func)(void*),
                                    void            *arg )
{
    __rtk_tick_down_counter_set_func(_this, func, arg );
    return 0;
}

void rtk_tick_down_counter_start( struct rtk_tick *_this, unsigned int tick )
{
    if (0 == tick) {
        tick = 1;
    }
    __rtk_tick_down_counter_remove( _this );
    __rtk_tick_down_counter_add( _this, tick );
}

void rtk_tick_down_counter_stop ( struct rtk_tick *_this )
{
    __rtk_tick_down_counter_remove( _this );
}


#endif


unsigned int rtk_tick_get( void )
{
    unsigned int temp;

    temp = g_systick;
    OS_TRACE_TICK_GET(temp);
    return temp;
}
