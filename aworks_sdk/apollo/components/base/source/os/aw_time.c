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
 * \brief system time manager library implementation
 *
 * \internal
 * \par modification history:
 * - 1.00 13-09-04  zen, first implementation
 * \endinternal
 */

#include "apollo.h"
#include "aw_spinlock.h"
#include "aw_time.h"
#include "aw_timer.h"
#include "aw_rtc.h"

#include "aw_assert.h"
#include "aw_vdebug.h"

#include <string.h>

/******************************************************************************/

/* real time clock */
struct __real_clock {
    aw_timer_t        timer;
    aw_timespec_t     time;

    unsigned long     timer_ticks;
    unsigned long     ns_add;
    int               bk_rtc_id;
};

/* real time clock instance */
struct __real_clock   __g_real_clock;

/******************************************************************************/
static void __clock_tick (void *arg)
{
    struct __real_clock *p_clock = (struct __real_clock *)arg;
    AW_INT_CPU_LOCK_DECL(lock);

    AW_INT_CPU_LOCK(lock);
    p_clock->time.tv_nsec += p_clock->ns_add;
    if (p_clock->time.tv_nsec >= 1000000000) {
        p_clock->time.tv_nsec -= 1000000000;
        p_clock->time.tv_sec  += 1;
    }
    AW_INT_CPU_UNLOCK(lock);

    aw_timer_start(&p_clock->timer, p_clock->timer_ticks);
}

/******************************************************************************/
static aw_err_t __timespec_get (aw_timespec_t *p_timespec)
{
    struct __real_clock *p_clock = &__g_real_clock;
    AW_INT_CPU_LOCK_DECL(lock);

    AW_INT_CPU_LOCK(lock);
    p_timespec->tv_sec  = p_clock->time.tv_sec;
    p_timespec->tv_nsec = p_clock->time.tv_nsec;
    AW_INT_CPU_UNLOCK(lock);

    return AW_OK;
}

/******************************************************************************/
static aw_err_t __timespec_set (aw_timespec_t *p_timespec)
{
    struct __real_clock *p_clock = &__g_real_clock;

    AW_INT_CPU_LOCK_DECL(lock);

    AW_INT_CPU_LOCK(lock);
    p_clock->time.tv_nsec = p_timespec->tv_nsec;
    p_clock->time.tv_sec  = p_timespec->tv_sec;
    AW_INT_CPU_UNLOCK(lock);

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_time_lib_init (unsigned int clkrate,
                           int bk_rtc_id)
{
    struct __real_clock *p_clock = &__g_real_clock;
    aw_tm_t                 tm;

    if ((clkrate == 0) || (clkrate > aw_sys_clkrate_get())) {
        return -AW_EINVAL;
    }

    memset(p_clock, 0, sizeof(*p_clock));

    /* caclulate the timer ticks */
    p_clock->timer_ticks = aw_sys_clkrate_get() / clkrate;
    if (p_clock->timer_ticks == 0) {
        p_clock->timer_ticks = 1;
    }

    /* calculate the ns increase every tick */
    p_clock->ns_add = p_clock->timer_ticks * 1000000000 / aw_sys_clkrate_get();

    /* restore time from RTC */
    p_clock->bk_rtc_id = bk_rtc_id;
    if (aw_rtc_time_get(p_clock->bk_rtc_id, &tm) == AW_OK) {
        aw_timespec_t timespec;
        aw_tm_to_time(&tm, &timespec.tv_sec);
        timespec.tv_nsec = 0;

        AW_INFOF(("current time: %d-%02d-%02d %02d:%02d:%02d\n",
                tm.tm_year+1900,
                tm.tm_mon+1,
                tm.tm_mday,
                tm.tm_hour,
                tm.tm_min,
                tm.tm_sec));

        /* set time */
        __timespec_set(&timespec);
    } else {
        AW_ERRF(("[aw_time]:Restore time from RTC failed!\n"));
    }

    /* TODO: set C library's time */

    /* start up the timer */
    aw_timer_init(&p_clock->timer, __clock_tick, p_clock);
    aw_timer_start(&p_clock->timer, p_clock->timer_ticks);

    return AW_OK;
}

/******************************************************************************/
aw_time_t aw_time (aw_time_t *p_time)
{
    aw_timespec_t timespec;

    /* get time from real time clock */
    __timespec_get(&timespec);

    if (p_time) {
        *p_time = timespec.tv_sec;
    }

    return timespec.tv_sec;
}

/******************************************************************************/
aw_err_t aw_timespec_get (aw_timespec_t *p_timespec)
{
    if (p_timespec == NULL) {
        return -AW_EINVAL;
    }

    /* get time from real time clock */
    return __timespec_get(p_timespec);
}

/******************************************************************************/
aw_err_t aw_timespec_set (aw_timespec_t *p_timespec)
{
    struct __real_clock *p_clock = &__g_real_clock;
    aw_tm_t tm;

    if (p_timespec == NULL) {
        return -AW_EINVAL;
    }

    /* set time to real time clock */
    __timespec_set(p_timespec);

    /* set time to RTC */
    aw_time_to_tm(&p_timespec->tv_sec, &tm);
    aw_rtc_time_set(p_clock->bk_rtc_id, &tm);

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_tm_set (aw_tm_t *p_tm)
{
    struct __real_clock *p_clock = &__g_real_clock;
    aw_timespec_t time;

    if (p_tm == NULL) {
        return -AW_EINVAL;
    }

    /* make time */
    time.tv_sec  = mktime((struct tm *)p_tm);
    time.tv_nsec = 0;
    if (time.tv_sec == (aw_time_t)-1) {
        return -AW_EBADF;
    }

    /* set time to real time clock */
    (void)__timespec_set(&time);

    /* set time to RTC */
    if (aw_rtc_time_set(p_clock->bk_rtc_id, p_tm) != AW_OK) {
        AW_ERRF(("[aw_time]:Save time to RTC failed!\n"));
    }

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_xtimespec_set (aw_timespec_t *p_timespec)
{
    if (p_timespec == NULL) {
        return -AW_EINVAL;
    }

    /* make time */
    if (p_timespec->tv_sec == (aw_time_t)-1) {
        return -AW_EBADF;
    }
    if (p_timespec->tv_nsec > 1000000000) {
        p_timespec->tv_sec  += p_timespec->tv_nsec / 1000000000;
        p_timespec->tv_nsec  = p_timespec->tv_nsec % 1000000000;
    }

    /* set time to real time clock */
    return __timespec_set(p_timespec);
}

/******************************************************************************/
aw_err_t aw_tm_get (aw_tm_t *p_tm)
{
    aw_timespec_t time;

    if (p_tm == NULL) {
        return -AW_EINVAL;
    }

    /* get time from real time clock */
    (void)__timespec_get(&time);

    /* transfer to pm type */
    return aw_time_to_tm(&time.tv_sec, p_tm);
}

/******************************************************************************/
aw_err_t aw_tm_to_time (aw_tm_t *p_tm, aw_time_t *p_time)
{
    if ((p_tm == NULL) ||
        (p_time == NULL)) {

        return -AW_EINVAL;
    }

    if ((*p_time = mktime((struct tm *)p_tm)) == (aw_time_t)-1) {
        return -AW_EBADF;
    }
    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_time_to_tm (aw_time_t *p_time, aw_tm_t *p_tm)
{
    struct tm *p_tm_tmp = NULL;

    if ((p_tm == NULL) ||
        (p_time == NULL)) {

        return -AW_EINVAL;
    }

    p_tm_tmp = localtime((const time_t *)p_time);
    if (p_tm_tmp) {
        memcpy(p_tm, p_tm_tmp, sizeof(*p_tm));
    }

    return AW_OK;
}

/* end of file */

