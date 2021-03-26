
#define __YEAR_BASE             1970 /**< \brief year base */
#define __DAY_OF_WEEK_BASE      4    /**< \brief day of week base */

/** \brief index to time zone name for strftime */
#define __TIME_ZONE             3    

/** \brief seconds per normal year */
#define __SEC_PER_NORMAL_YEAR   (365L * 24L * 60L * 60L)

/** \brief seconds per leap year */
#define __SEC_PER_LEAP_YEAR     (366L * 24L * 60L * 60L)

/** \brief test if 'a' is a leap year */
#define __is_leap_year(a)       (((a % 4) == 0) && \
                                 !(((a % 100) == 0) && ((a % 400) != 0)))
extern long  timezone;

#ifndef TZNAME_MAX
#define TZNAME_MAX  6
#endif
