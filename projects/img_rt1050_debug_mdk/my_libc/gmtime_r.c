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
 * \brief implementation of gmtime_r().
 *
 * \internal
 * \par modification history:
 * - 1.00 14-09-26 orz, first implementation.
 * \endinternal
 */

#include "time.h"
#include "mylib.h"

#ifndef  _TMS320C6X
/******************************************************************************/
#define __is_more_than_a_year(t, leap_year) \
            ((leap_year  && (t >= __SEC_PER_LEAP_YEAR)) || \
             (!leap_year && (t >= __SEC_PER_NORMAL_YEAR)))

/******************************************************************************/
/* Days per month array, assuming this is not a leap year   */
static const struct __days_per_month {
    unsigned char dpm[12];
} __g_dpm_template = { {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31} };

/******************************************************************************/
struct tm *gmtime_r (const time_t *timer, struct tm *result)
{
    time_t                  time; /* in case *timer changing asynchronously */
    unsigned long           leap_year;
    register signed long    y, i, d;
    register unsigned long  t;
    struct __days_per_month dpm_local;


    /* Check for NULL pointers and values >= 0 */
    if ((NULL == timer) || ((time = *timer) < 0)) {
        return NULL;
    }

    t = time;
    y = __YEAR_BASE;

    /*
     * on exit:
     * y holds the current year.
     * t holds the number of seconds into the current year.
     * leap_year holds 1 if the current year is a leap year.
     */
    leap_year = __is_leap_year(y);
    for (; __is_more_than_a_year(t, leap_year); leap_year = __is_leap_year(y)) {
        t -= leap_year ? __SEC_PER_LEAP_YEAR : __SEC_PER_NORMAL_YEAR;
        y += 1; /* Increment the year  */
    }

    result->tm_year = y - 1900; /* Years since 1900 */
    result->tm_yday = d = t / (24L * 60L * 60L);

    /* Make local copy of days-per-month array, so we can adjust February */
    dpm_local = __g_dpm_template;

    /* adjust february */
    if (leap_year) {
        ++dpm_local.dpm[1];
    }

    for (i = 0; (i <= 11) && (d - dpm_local.dpm[i] >= 0); i++) {
        d -= dpm_local.dpm[i];
    }
    result->tm_mon  = i;
    result->tm_mday = d + 1;

    t              %= (24L * 60L * 60L); /* seconds into current day */
    result->tm_hour = t / (60 * 60);

    t              %= (60 * 60);         /* seconds into current hour */
    result->tm_min  = t / 60;
    result->tm_sec  = t % 60;

    i               = ((time / (24L * 60L * 60L)) % 7) + __DAY_OF_WEEK_BASE;
    result->tm_wday = (i > 6) ? i - 7 : i;

    /* Daylight savings time is not supported. */
    result->tm_isdst = 0;

    return (result);
}
#endif
/* end of file */
