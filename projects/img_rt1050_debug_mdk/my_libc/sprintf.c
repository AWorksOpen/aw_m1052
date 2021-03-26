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
 * \brief sprintf() implementation.
 *
 * \internal
 * \par modification history:
 * - 1.00 14-09-25 orz, first implementation.
 * \endinternal
 */

#include "aw_vdebug.h"
#include <stdarg.h>
#include <stdio.h>
#include <stddef.h> /* for ptrdiff_t */

#ifndef  _TMS320C6X
/**
 * \brief format a string into a string buffer.
 *
 * \param s      the string buffer to hold the formatted string
 * \param format the format string
 * \param ...    the arguments list for the format string
 *
 * \return number of char put into buffer
 */
int sprintf (char *s, const char *format, ...)
{
    va_list args;
    int     len;

    va_start(args, format);
    len = aw_vsnprintf(s, ((char *)(~((ptrdiff_t)0u)) - s), format, args);
    va_end(args);

    return len;
}
#endif
/* end of file */
