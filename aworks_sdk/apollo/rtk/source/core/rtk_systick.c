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
 * \file  rtk_systick.c
 * \brief rtk systickÏà¹Øº¯Êý
 *
 * \internal
 * \par modification history:
 * 1.00 18-04-24  sni, first implement
 * \endinternal
 */
#include "rtk.h"
#include "rtk_mem.h"
#include "rtk_arch.h"
#include "rtk_task_internal.h"
#include "rtk_priority_q_internal.h"
#include "rtk_tick_down_counter_internal.h"
#include "rtk_systick.h"
#include "aw_errno.h"
#include "aw_common.h"
#include <string.h> /* for memset() and memcpy() */

#if CONFIG_RTK_SYSTICK_TASK_EN

static struct {
    struct rtk_task     tcb;
    char                stack[CONFIG_RTK_SYSTICK_TASK_STACK_SIZE];
}rtk_systick;

static struct rtk_semb                  __systick_semb;

static rtk_base_mutex_t                 __systick_lock;

void * __rtk_systick_lock(void)
{
    rtk_base_mutex_lock(&__systick_lock);
    return &__systick_lock;
}

void __rtk_systick_unlock(void *p_lock)
{
    rtk_base_mutex_unlock(&__systick_lock);
}

static void __task_fn_systick(void)
{
    while(1) {
        rtk_semb_take(&__systick_semb,AW_WAIT_FOREVER);
        __rtk_tick_down_counter_1_tick();
    }
}

void __rtk_systick_init(void)
{
    rtk_semb_init(&__systick_semb,0);
    rtk_base_mutex_init(&__systick_lock);

    __rtk_task_init_internal(
            &rtk_systick.tcb,
            "systick",
            CONFIG_RTK_SYSTICK_TASK_PRIO,
            ENUM_RTK_TASK_OPTION_CAN_NOT_BE_FROZE,
            rtk_systick.stack,
            rtk_systick.stack + sizeof(rtk_systick.stack),
            __task_fn_systick,
            0,
            0
            );
    rtk_task_startup(&rtk_systick.tcb);

}

void rtk_tick_down_counter_announce( void )
{
    rtk_semb_give(&__systick_semb);
}






#else

void __rtk_systick_init(void)
{

}

/**
 *  \brief soft timer announce.
 *
 *  systerm tick is provided by calling this function.
 *
 *  \sa ENTER_INT_CONTEXT(), EXIT_INT_CONTEXT().
 */
void rtk_tick_down_counter_announce( void )
{
    __rtk_tick_down_counter_1_tick();
    rtk_schedule_systick_isr();
}

#endif
