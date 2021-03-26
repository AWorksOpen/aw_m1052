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
 * \brief implementation of asctime().
 *
 * \internal
 * \par modification history:
 * - 1.00 14-09-25 orz, first implementation.
 * \endinternal
 */

#include <time.h>
#include <stdio.h>

/******************************************************************************/

/**
 * the asctime function converts the broken-down time value that brokentime
 * points to into a string in a standard format:
 *
 *      "Tue May 21 13:46:22 1991\n"
 *
 * The abbreviations for the days of week are:
 *      Sun, Mon, Tue, Wed, Thu, Fri, and Sat.
 *
 * The abbreviations for the months are:
 *      Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, and Dec.

 * \param timeptr Pointer to the tm structure.
 * \param buf     User supplied buffer.
 *
 * \return The return value points to a statically allocated string,
 * which might be overwritten by subsequent calls to asctime or ctime.
 * (But no other library function overwrites the contents of this string.)
 */
#ifndef  _TMS320C6X
char *asctime (const struct tm *timeptr)
{
    static char buf[sizeof("Tue May 21 13:46:22 1991\n")];

    return asctime_r(timeptr, buf);
}
#endif

/* end of file */
