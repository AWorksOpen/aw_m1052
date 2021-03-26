/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief Reset handler for arm, initialize c run time
 *
 * This is the reset handler for gcc. The handler will initialize c run time.
 *
 * \internal
 * \par modification history:
 * - 1.01 13-04-15  or2, move sys_tick interrupt connect to the function systick_init()
 * - 1.00 12-11-28  or2, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "rtk.h"
#include "aw_task.h"
#include "aw_exc.h"


extern void SystemInit (void);
extern void aw_bsp_system_init(void);
extern void aw_bsp_system_heap_init(void);
extern void aw_bsp_dma_safe_heap_init(void);
extern void aw_bsp_system_no_cache_heap_init(void);
extern void aw_bsp_clock_init(void);

void aw_bsp_early_init(void)
{
    arch_interrupt_disable();
    aw_bsp_clock_init();
    aw_bsp_system_init();
}

void aw_bsp_task_level_init(void)
{
    aw_bsp_system_heap_init();
    aw_bsp_dma_safe_heap_init();
    
#if defined(__CC_ARM)
    extern int __rt_lib_init(void);
    __rt_lib_init();
#endif
}
/* end of file */
