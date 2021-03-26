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
 * \brief RTT I/O for kernel output
 *
 * \internal
 * \par modification history:
 * - 1.00 18-08-29  phd, first implementation
 * \endinternal
 */

#include "aw_koutput.h"

#include "utils/SEGGER_RTT/SEGGER_RTT.h"

#include <stdio.h>

/** \brief out a char */
static int __rtt_putc(const char c, void *fil)
{
    return SEGGER_RTT_Write((unsigned) fil, &c, 1);
}

/** \brief out a string */
static int __rtt_puts(const char *s, void *fil)
{
    return SEGGER_RTT_WriteString((unsigned) fil, s);
}

/**
 * \brief Initialize RTT I/O as kernel output
 *
 * \param buffer_index  Index of the buffer to configure
 */
void aw_koutput_segger_rtt_init(unsigned buffer_index)
{
    SEGGER_RTT_ConfigUpBuffer(buffer_index, 
                              "koutput", 
                              NULL, 
                              0,
                              SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    aw_koutput_set((void *) buffer_index, __rtt_putc, __rtt_puts);
}
