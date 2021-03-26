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
 * \brief implementation of mktime().
 *
 * \internal
 * \par modification history:
 * - 1.00 14-09-26 orz, first implementation.
 * \endinternal
 */

#include <time.h>
#include "mylib.h"

#ifndef  _TMS320C6X
/******************************************************************************/
/* Days per month array, assuming this is not a leap year   */
static const struct __days_per_month {
    unsigned char dpm[12];
} __g_dpm_template = { {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31} };

/******************************************************************************/

/**
 * The mktime function is used to convert a broken-down time structure to a
 * simple time representation. It also "normalizes" the contents of the
 * broken-down time structure, by filling in the day of week and day of year
 * based on the other date and time components.
 *
 * The mktime function ignores the specified contents of the tm_wday and
 * tm_yday members of the broken-down time structure. It uses the values of the
 * other components to determine the calendar time; it's permissible for these
 * components to have unnormalized values outside their normal ranges. The last
 * thing that mktime does is adjust the components of the brokentime structure
 * (including the tm_wday and tm_yday).
 *
 * If the specified broken-down time cannot be represented as a simple time,
 * mktime returns a value of (time_t)(-1) and does not modify the contents of
 * brokentime.
 *
 * Calling mktime also sets the variable tzname with information about the
 * current time zone.
 */
time_t mktime (struct tm *timeptr)
{

    time_t                  time;
    unsigned long           leap_year;
    register unsigned long  t, y;
    register long           m;
    struct __days_per_month dpm_local;
    struct tm               tm_buf, *tmptr;
    if (timeptr == NULL) {
        return ((time_t)-1);
    }

    t = (unsigned long)(timeptr->tm_sec
      + (unsigned long)(timeptr->tm_min * 60)
      + (unsigned long)(timeptr->tm_hour * 60ul * 60ul)
      + (unsigned long)(timeptr->tm_mday - 1) * 24ul * 60ul * 60ul);
    y = (unsigned long)(timeptr->tm_year + (timeptr->tm_mon / 12) + 1900);


    /* Make local copy of days-per-month array, so we can adjust February */
    dpm_local = __g_dpm_template;

    if ((leap_year = __is_leap_year(y)) != 0) {
        ++dpm_local.dpm[1];
    }
    for (m = (timeptr->tm_mon % 12) - 1; m >= 0; --m) {
        t += dpm_local.dpm[m] * 24ul * 60ul * 60ul;
    }
    if (y < __YEAR_BASE) {
        if (leap_year && (t < __SEC_PER_LEAP_YEAR)) {
            return ((time_t)-1);
        } else if (t < __SEC_PER_NORMAL_YEAR) {
            return ((time_t)-1);
        }
    }
    y        -= 1;
		
    leap_year = __is_leap_year(y);
    for (; y >= __YEAR_BASE; leap_year = __is_leap_year(y)) {
        t += leap_year ? __SEC_PER_LEAP_YEAR : __SEC_PER_NORMAL_YEAR;
        y -= 1;
    }
    t   -= timezone; /*  Compensate the effect of the time zone */
    time = (time_t)t;

		if ((tmptr = localtime_r(&time, &tm_buf)) != NULL) {
			timeptr->tm_year = tmptr->tm_year;
					timeptr->tm_mon = tmptr->tm_mon;
					timeptr->tm_mday = tmptr->tm_mday;
					timeptr->tm_hour = tmptr->tm_hour;
					timeptr->tm_min = tmptr->tm_min;
					timeptr->tm_sec = tmptr->tm_sec;
					timeptr->tm_wday = tmptr->tm_wday;
					timeptr->tm_yday = tmptr->tm_yday;
					timeptr->tm_isdst = tmptr->tm_isdst;
    } else {
        return ((time_t)-1);
    }

    /* t now holds the local time in seconds. */
    return ((time_t)t);
}
#endif
/* end of file */
