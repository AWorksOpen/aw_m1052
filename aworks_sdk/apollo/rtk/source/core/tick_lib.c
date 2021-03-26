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
 * \brief architecture-independent system tick library
 *
 * \internal
 * \par modification history:
 * - 1.00 13-02-27  zen, first implementation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/

#include "apollo.h"
#include "aw_system.h"

#include "rtk.h"

/*******************************************************************************
  imports
*******************************************************************************/

/*******************************************************************************
  implementation
*******************************************************************************/

/******************************************************************************/
aw_tick_t aw_ms_to_ticks (unsigned int ms)
{
    uint64_t                t = ms;
    aw_tick_t               tick;

    t *= aw_sys_clkrate_get();
    t = (t + 1000 -1)/1000;
    tick = (aw_tick_t)t;

    return tick;
}

/******************************************************************************/
unsigned int aw_ticks_to_ms (aw_tick_t ticks)
{
    uint64_t                t;

    t = ticks;
    t *= 1000;
    t /= aw_sys_clkrate_get();
    return (unsigned int)t;
}

/******************************************************************************/
aw_tick_t aw_sys_tick_get(void)
{
    return rtk_tick_get();
}

/* end of file */
