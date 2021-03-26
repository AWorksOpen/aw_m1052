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
 * \brief implementation of strftime().
 *
 * \internal
 * \par modification history:
 * - 1.00 14-09-22 orz, first implementation.
 * \endinternal
 */

#include <time.h>
#include <stdio.h>
#include "mylib.h"

#ifndef  _TMS320C6X
/******************************************************************************/

static const struct {
    const char *awn[7];
    const char *fwn[7];
    const char *amn[12];
    const char *fmn[12];
    const char *hd[2];
    const char *tzn[8];
} __g_time_str = {
    { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" },
    { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
      "Saturday" },
    { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct",
      "Nov", "Dec" },
    { "January", "February", "March", "April", "May", "June", "July",
      "August", "September", "October", "November", "December" },
    { "A.M.", "P.M." },
    { "EST", "CST", "MST", "PST", "EDT", "CDT", "MDT", "PDT" }
};

/**
 * \brief Copies up to max bytes from the stream pointed to by src to that
 *		  pointed to by dst.
 *
 * \param dst Pointer to destination buffer.
 * \param src Pointer to source buffer.
 * \param max Maximum bytes to be copied.
 *
 * \return Pointer to last byte written.
 */
static char *__strfcpy (char *dst, const char *src, const char *max)
{
    while (dst <= max && *src != '\0') {
        *dst++ = *src++;
    }
    return(dst);
}

/**
 * \brief The function shall place bytes into the array pointed to by s as
 *        controlled by the string pointed to by format.
 *
 * \param s       Pointer to the string.
 * \param maxsize maximum size of the no of bytes
 *                to be placed in a character array.
 * \param format  Given Format.
 * \param timeptr Pointer to the time structure.
 *
 * \return resulting bytes If resulting bytes <= maxsize, 0 Otherwise.
 */
size_t strftime (char            *s,
                 size_t           maxsize,
                 const char      *format,
                 const struct tm *timeptr)
{
    char  buf[16];
    long long  tmp, tmp1, tmp2;
    char *smax, *sbgn;

    if (maxsize < 1) {
        return ((size_t)0);
    }
    sbgn = s;
    smax = s + (maxsize - 1);
    while ((s <= smax) && ('\0' != *format)) {
        /* Search for the conversion specifier */
        if (*format != '%') {
            *s++ = *format++;
            continue;
        }
        /* Take actions according to the given format */
        switch (*(++format)) {
        case 'a':
            /* Abbreviated weekday name. */
            s = __strfcpy(s, __g_time_str.awn[timeptr->tm_wday], smax);
            break;
        case 'A':
            /* Full weekday name. */
            s = __strfcpy(s, __g_time_str.fwn[timeptr->tm_wday], smax);
            break;
        case 'b':
            /* Abbreviated month name. */
            s = __strfcpy(s, __g_time_str.amn[timeptr->tm_mon], smax);
            break;
        case 'B':
            /* Full month name. */
            s = __strfcpy(s, __g_time_str.fmn[timeptr->tm_mon], smax);
            break;
        case 'c':
            /* Date and time representation. */
            s += strftime(s, ((size_t)(smax - s)), "%a %b %d %H:%M:%S %Z %Y",
                          timeptr);
            break;
        case 'C':
            /* Year divided by 100 and truncated to an integer, as a decimal
             * number. */
            (void)sprintf(buf, "%02d", ((timeptr->tm_year + 1900) / 100));
            s = __strfcpy(s, &buf[0], smax);
            break;
        case 'd':
            /* Day of the month as a decimal number. */
            (void)sprintf(buf, "%02d", timeptr->tm_mday);
            s = __strfcpy(s, &buf[0], smax);
            break;
        case 'D':
            /* Equivalent to %m/%d/%y. */
            s += strftime(s, ((size_t)(smax - s)), "%m/ %d/ %y", timeptr);
            break;
        case 'e':
            /* Day of the month as a decimal number. */
            (void)sprintf(buf, "%2d", timeptr->tm_mday);
            s = __strfcpy(s, &buf[0], smax);
            break;
        case 'F':
            /* Equivalent to %Y-%m-%d. */
            s += strftime(s, ((size_t)(smax - s)), "%Y-%m-%d", timeptr);
            break;
        case 'g':
            /* Last 2 digits of the week-based year (see below)
             * as a decimal number. */
            tmp  = timeptr->tm_year % 100;
            tmp1 = timeptr->tm_yday + 1;
            tmp2 = timeptr->tm_yday - 365
                 - (__is_leap_year(timeptr->tm_year) ? 1 : 0);
            if (!((tmp1 > 3) && (tmp2 < -3))) {
                switch (tmp1) {
                case 1:
                    if ((timeptr->tm_wday == 0) || (timeptr->tm_wday == 6) ||
                        (timeptr->tm_wday == 5)) {
                        tmp -= 1;
                    }
                    break;
                case 2:
                    if ((timeptr->tm_wday == 0) || (timeptr->tm_wday == 6)) {
                        tmp -= 1;
                    }
                    break;
                case 3:
                    if ((timeptr->tm_wday == 0)) {
                        tmp -= 1;
                    }
                    break;
                }
                switch (tmp2) {
                case -1:
                    if ((timeptr->tm_wday == 1) || (timeptr->tm_wday == 2) ||
                        (timeptr->tm_wday == 3)) {
                        tmp += 1;
                    }
                    break;
                case -2:
                    if ((timeptr->tm_wday == 1) || (timeptr->tm_wday == 2)) {
                        tmp += 1;
                    }
                    break;
                case -3:
                    if ((timeptr->tm_wday == 1)) {
                        tmp += 1;
                    }
                    break;
                }
            }
            (void)sprintf(buf, "%02d", (int)tmp);
            s = __strfcpy(s, &buf[0], smax);
            break;
        case 'G':
            /* Week-based year (see below) as a decimal number. */
            tmp  = timeptr->tm_year % 100;
            tmp1 = timeptr->tm_yday + 1;
            tmp2 = timeptr->tm_yday - 365
                 - (__is_leap_year(timeptr->tm_year) ? 1 : 0);
            if (!((tmp1 > 3) && (tmp2 < -3))) {
                switch (tmp1) {
                case 1:
                    if ((timeptr->tm_wday == 0) || (timeptr->tm_wday == 6) ||
                        (timeptr->tm_wday == 5)) {
                        tmp -= 1;
                    }
                    break;
                case 2:
                    if ((timeptr->tm_wday == 0) || (timeptr->tm_wday == 6)) {
                        tmp -= 1;
                    }
                    break;
                case 3:
                    if ((timeptr->tm_wday == 0)) {
                        tmp -= 1;
                    }
                    break;
                }
                switch (tmp2) {
                case -1:
                    if ((timeptr->tm_wday == 1) || (timeptr->tm_wday == 2) ||
                        (timeptr->tm_wday == 3)) {
                        tmp += 1;
                    }
                    break;
                case -2:
                    if ((timeptr->tm_wday == 1) || (timeptr->tm_wday == 2)) {
                        tmp += 1;
                    }
                    break;
                case -3:
                    if ((timeptr->tm_wday == 1)) {
                        tmp += 1;
                    }
                    break;
                }
            }
            tmp += 1900;
            (void)sprintf(buf, "%d", (int)tmp);
            s = __strfcpy(s, &buf[0], smax);
            break;
        case 'H':
            /* Hour (24-hour clock) as a decimal number. */
            (void)sprintf(buf, "%02d", timeptr->tm_hour);
            s = __strfcpy(s, &buf[0], smax);
            break;
        case 'h':
            /* Equivalent to %b. */
            s += strftime(s, ((size_t)(smax - s)), "%b", timeptr);
            break;
        case 'I':
            /* Hour (12-hour clock) as a decimal number. */
            tmp = timeptr->tm_hour > 11
                ? timeptr->tm_hour - 12 : timeptr->tm_hour;
            if (timeptr->tm_hour == 0 || timeptr->tm_hour == 12) {
                tmp = 12;
            }
            (void)sprintf(buf, "%02d", (int)tmp);
            s = __strfcpy(s, &buf[0], smax);
            break;
        case 'j':
            /* Day of the year as a decimal number. */
            (void)sprintf(buf, "%03d", timeptr->tm_yday + 1);
            s = __strfcpy(s, &buf[0], smax);
            break;
        case 'm':
            /* Month as a decimal number. */
            (void)sprintf(buf, "%02d", timeptr->tm_mon + 1);
            s = __strfcpy(s, &buf[0], smax);
            break;
        case 'M':
            /* Minute as a decimal number. */
            (void)sprintf(buf, "%02d", timeptr->tm_min);
            s = __strfcpy(s, &buf[0], smax);
            break;
        case 'n':
            /* Replaced by a <newline>. */
            (void)sprintf(buf, "\n");
            s = __strfcpy(s, &buf[0], smax);
            break;
        case 'p':
            /* Equivalent of either a.m. or p.m. */
            if (timeptr->tm_hour < 12) {
                s = __strfcpy(s, __g_time_str.hd[0], smax);
            } else {
                s = __strfcpy(s, __g_time_str.hd[1], smax);
            }
            break;
        case 'r':
            /* Time in a.m. and p.m. notation. */
            s += strftime(s, ((size_t)(smax - s)), "%I:%M:%S %p", timeptr);
            break;
        case 'R':
            /* Time in 24-hour notation. */
            s += strftime(s, ((size_t)(smax - s)), "%H:%M", timeptr);
            break;
        case 'S':
            /* Second as a decimal number. */
            (void)sprintf(buf, "%02d", timeptr->tm_sec);
            s = __strfcpy(s, &buf[0], smax);
            break;
        case 't':
            /* Replaced by a <tab>. */
            (void)sprintf(buf, "\t");
            s = __strfcpy(s, &buf[0], smax);
            break;
        case 'T':
            /* Time (%H:%M:%S). */
            s += strftime(s, ((size_t)(smax - s)), "%H:%M:%S", timeptr);
            break;
        case 'u':
            /* Weekday as a decimal number. */
            tmp = (timeptr->tm_wday == 0) ? 7 : timeptr->tm_wday;
            (void)sprintf(buf, "%1d", (int)tmp);
            s = __strfcpy(s, &buf[0], smax);
            break;
        case 'U':
            /* Week number of the year as a decimal number. */
            tmp = (timeptr->tm_yday - timeptr->tm_wday < 0) ? 0 :
                    (timeptr->tm_yday - timeptr->tm_wday) / 7;
            (void)sprintf(buf, "%02d", (int)tmp);
            s = __strfcpy(s, &buf[0], smax);
            break;
        case 'V':
            /* Week number of the year (Monday as the first day of
             the week) as a decimal number. */
            tmp  = (timeptr->tm_yday / 365) + 1;
            tmp1 = timeptr->tm_yday + 1;
            tmp2 = timeptr->tm_yday - 365
                 - (__is_leap_year(timeptr->tm_year) ? 1 : 0);
            if (!((tmp1 > 3) && (tmp2 < -3))) {
                switch (tmp1) {
                case 1:
                    if ((timeptr->tm_wday == 0) || (timeptr->tm_wday == 6) ||
                        (timeptr->tm_wday == 5)) {
                        tmp = 53;
                    }
                    break;
                case 2:
                    if ((timeptr->tm_wday == 0) || (timeptr->tm_wday == 6)) {
                        tmp = 53;
                    }
                    break;
                case 3:
                    if ((timeptr->tm_wday == 0)) {
                        tmp = 53;
                    }
                    break;
                }
                switch (tmp2) {
                case -1:
                    if ((timeptr->tm_wday == 1) || (timeptr->tm_wday == 2) ||
                        (timeptr->tm_wday == 3)) {
                        tmp = 1;
                    }
                    break;
                case -2:
                    if ((timeptr->tm_wday == 1) || (timeptr->tm_wday == 2)) {
                        tmp = 1;
                    }
                    break;
                case -3:
                    if ((timeptr->tm_wday == 1)) {
                        tmp = 1;
                    }
                    break;
                }
            }
            (void)sprintf(buf, "%d", (int)tmp);
            s = __strfcpy(s, &buf[0], smax);
            break;
        case 'w':
            /* Weekday as a decimal number [0,6], with 0 representing Sunday. */
            (void)sprintf(buf, "%02d", timeptr->tm_wday);
            s = __strfcpy(s, &buf[0], smax);
            break;
        case 'W':
            /* Week number of the year as a decimal number. */
            tmp1 = (timeptr->tm_wday == 0) ? 6 : timeptr->tm_wday - 1;
            tmp  = (timeptr->tm_yday - tmp1 < 0)
                 ? 0 : (timeptr->tm_yday - tmp1) / 7;
            (void)sprintf(buf, "%02d", (int)tmp);
            s = __strfcpy(s, &buf[0], smax);
            break;
        case 'x':
            /* Appropriate date representation. */
            s += strftime(s, ((size_t)(smax - s)), "%a %b %d %Y", timeptr);
            break;
        case 'X':
            /* Appropriate time representation. */
            s += strftime(s, ((size_t)(smax - s)), "%H:%M:%S %Z", timeptr);
            break;
        case 'y':
            /* Last two digits of the year as a decimal number. */
            (void)sprintf(buf, "%02d", (timeptr->tm_year + 1900) % 100);
            s = __strfcpy(s, &buf[0], smax);
            break;
        case 'Y':
            /* Year as a decimal number. */
            (void)sprintf(buf, "%d", timeptr->tm_year + 1900);
            s = __strfcpy(s, &buf[0], smax);
            break;
        case 'z':
            /* Offset from UTC in the ISO 8601: 2000 standard format. */
            /* No Conversion is specified as no timezone is determinable.
             * Will be provided later */
            break;
        case 'Z':
            /* Timezone name or abbreviation. */
            s = __strfcpy(s, __g_time_str.tzn[__TIME_ZONE], smax);
            break;
        case '%':
            *s++ = '%';
            break;
        }
        ++format;
    }
    if (s > smax) {
        return ((size_t)0);
    } else {
        *s = '\0';
        return ((size_t)(s - sbgn));
    }
}
#endif
/* end of file */
