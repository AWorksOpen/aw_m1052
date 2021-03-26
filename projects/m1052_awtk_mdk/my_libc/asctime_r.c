/**
 * \file
 * \brief implementation of asctime_r().
 *
 * \internal
 * \par modification history:
 * - 140925 orz, first implementation.
 * \endinternal
 */

#include <time.h>
#include <stdio.h>

/******************************************************************************/

/**
 * The function converts the broken-down time in the structure
 * pointed to by timeptr into a string in the form:
 * Sun Sep 16 01:03:52 1973\n\0. This routine is re-entrant.
 *
 * \param timeptr Pointer to the tm structure.
 * \param buf     User supplied buffer.
 *
 * \return A pointer to a string if successful, a NULL otherwise.
 */
char *asctime_r (const struct tm *timeptr, char *buf)
{
    static const char *wday_name[7] = {
        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
    };
    static const char *mon_name[12] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    (void)sprintf(buf,
#if 0 /* fixme: this format currently not support */
                  "%.3s %.3s%3d %.2d:%.2d:%.2d %d\n",
#else
                  "%3s %3s%3d %02d:%02d:%02d %d\n",
#endif
                  wday_name[timeptr->tm_wday],
                  mon_name[timeptr->tm_mon],
                  timeptr->tm_mday,
                  timeptr->tm_hour,
                  timeptr->tm_min,
                  timeptr->tm_sec,
                  1900 + timeptr->tm_year);
    return (buf);
}

/* end of file */
