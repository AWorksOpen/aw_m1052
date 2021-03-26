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
 * \brief deferal job library configure file
 *
 * \internal
 * \par modification history:
 * - 1.01 12-07-12  zyr   implement aw_mdelay in C
 * - 1.00 12-07-09  zyr   first implementation
 * \endinternal
 */


#include "apollo.h"
#include "aw_task.h"
#include "aw_delay.h"

/**
 * \brief 延时服务函数
 */
void aw_mdelay(uint32_t ms)
{
    unsigned int            ticks;
    uint64_t                t = ms;

    t = aw_sys_clkrate_get() * t / 1000;

    ticks = (aw_tick_t)t;
    if (ticks == 0) {
        ticks = 1;   /* at least one tick */
    }

    aw_task_delay(ticks);
}

/* end of file */
