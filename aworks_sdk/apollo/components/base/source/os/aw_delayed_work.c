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
 * \brief delayed work queue.
 *
 * \internal
 * \par modification history
 * - 1.00 13-03-06  orz, first implementation.
 * \endinternal
 */

#include "apollo.h"
#include "aw_delayed_work.h"
#include "aw_system.h"

/**
 * \addtogroup aw_delayed_work
 * @{
 */
aw_local void __delayed_work_handler (void *p_arg)
{
    struct aw_delayed_work *p_work = p_arg;

    aw_isr_defer_job_add(&p_work->job);
}

/******************************************************************************/
void aw_delayed_work_init (struct aw_delayed_work *p_work,
                           void                  (*pfunc_work)(void *p_arg),
                           void                   *p_arg)
{
    (void)aw_timer_init(&p_work->tmr, __delayed_work_handler, p_work);
    (void)aw_isr_defer_job_init(&p_work->job, pfunc_work, p_arg);
}

/******************************************************************************/
void aw_delayed_work_start(struct aw_delayed_work *p_work, uint_t delay_ms)
{
    int ticks = aw_ms_to_ticks(delay_ms);

    (void)aw_timer_start(&p_work->tmr, ticks > 0 ? ticks : 1);
}

/******************************************************************************/
void aw_delayed_work_stop (struct aw_delayed_work *p_work)
{
    (void)aw_timer_stop(&p_work->tmr);
}

/******************************************************************************/
void aw_delayed_work_destroy (struct aw_delayed_work *p_work)
{
    (void)aw_timer_stop(&p_work->tmr);
    aw_isr_defer_job_del(&p_work->job);
}
/** @} */

/* end of file */
