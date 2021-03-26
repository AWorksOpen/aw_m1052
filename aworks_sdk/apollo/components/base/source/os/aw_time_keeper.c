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
#include "aw_rtc.h"
#include "aw_delayed_work.h"

#include "aw_assert.h"
#include "aw_vdebug.h"

#include <string.h>

/******************************************************************************/

/* real time clock */
struct __time_keeper {
    struct aw_delayed_work work;
    unsigned int           period_s;
    int                    bk_rtc_id;
};

/* time keeper */
struct __time_keeper   __g_time_keeper;

/******************************************************************************/
extern aw_err_t aw_xtimespec_set (aw_timespec_t *p_timespec);
static void __time_keeping_work (void *arg)
{
    struct __time_keeper *p_tk = (struct __time_keeper *)arg;
    aw_tm_t               tm;
    aw_timespec_t         timespec;
    aw_time_t             time;
    aw_err_t              ret;

    ret = aw_rtc_time_get(p_tk->bk_rtc_id, &tm);
    if ((ret == AW_OK) && (aw_tm_to_time(&tm, &time) == AW_OK)) {
        timespec.tv_sec  = time;
        timespec.tv_nsec = 0;
        aw_xtimespec_set(&timespec);
    }

    aw_delayed_work_start(&p_tk->work, p_tk->period_s * 1000);
}


/******************************************************************************/
aw_err_t aw_time_keeper_init (int bk_rtc_id,  unsigned int period_s)
{
    struct __time_keeper *p_tk = & __g_time_keeper;

    p_tk->bk_rtc_id = bk_rtc_id;
    p_tk->period_s  = period_s;

    aw_delayed_work_init(&p_tk->work, __time_keeping_work, p_tk);
    aw_delayed_work_start(&p_tk->work, p_tk->period_s * 1000);
    return AW_OK;
}

/* end of file */

