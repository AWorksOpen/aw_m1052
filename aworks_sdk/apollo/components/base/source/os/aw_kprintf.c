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
 * \brief kprintf for kernel and driver debugging
 *
 * to make kprintf safe, define AW_KPRINTF_LOCK_ENABLE in config.h
 *
 * \internal
 * \par modification history:
 * - 1.00 12-11-22  orz, first implementation from RTK
 * \endinternal
 */

#include "aw_vdebug.h"
#include "aw_koutput.h"
#include "aw_int.h"
#include "aw_sem.h"
#include <stdarg.h>

AW_MUTEX_DECL(_g_kprintf_mutex);

/**
 * \brief kernel format print function
 *
 * This function output the formatted string into the current console device.
 * The console output functions is set by aw_console_set().
 *
 * \param fmt the format string
 * \param ... the arguments list for the format string
 *
 * \return number of chars output into console
 */
int aw_kprintf (const char *fmt, ...)
{
    int     len;
    va_list args;


    if ((NULL == gpfunc_kputc) || (NULL == gpfunc_kputs)) {
        return 0;
    }

    va_start(args, fmt);

    if ( !(AW_INT_CONTEXT() || AW_FAULT_CONTEXT() )) {
        AW_MUTEX_LOCK(_g_kprintf_mutex,AW_WAIT_FOREVER);
    }


    len = aw_vfprintf_do(gp_kout_file, gpfunc_kputc, gpfunc_kputs, fmt, args);

    if ( !(AW_INT_CONTEXT() || AW_FAULT_CONTEXT() )) {
        AW_MUTEX_UNLOCK(_g_kprintf_mutex);
    }
    va_end(args);

    return len;
}

/* end of file */

