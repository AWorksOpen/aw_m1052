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
 * \brief systick function for armv8m arch.
 *
 * \internal
 * \par History
 * - 1.10 2019-08-29, A.J, armv8-m systick.
 * - 1.00 17-07-11  sni, first implementation.
 * \endinternal
 */

#ifndef __ARMV8M_SYSTICK_H /* { */
#define __ARMV8M_SYSTICK_H

#include <aw_types.h>

/** \brief  Structure type to access the System Timer (SysTick).
 */
typedef struct
{
    /** \brief Offset: 0x000 (R/W)  SysTick Control and Status Register */
    volatile uint32_t CTRL;
    /** \brief Offset: 0x004 (R/W)  SysTick Reload Value Register       */
    volatile uint32_t LOAD;
    /** \brief Offset: 0x008 (R/W)  SysTick Current Value Register      */
    volatile uint32_t VAL;
    /** \brief Offset: 0x00C (R/ )  SysTick Calibration Register        */
    volatile uint32_t CALIB;
} SysTick_Type;

#define SYS_TICK        ((SysTick_Type *)0xE000E010UL)

uint32_t armv8m_systick_init(
            unsigned int systick_freq,
            unsigned int cpu_freq,
            int tick_per_second,
            int use_cpu_core_clk);

uint32_t armv8m_systick_counter_get(void);

#endif /* } __ARMV8M_SYSTICK_H */

/* end of file */
