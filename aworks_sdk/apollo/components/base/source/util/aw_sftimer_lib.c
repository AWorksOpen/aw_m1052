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
 * \brief Soft timer library
 *
 * This library provides soft timer support. Before using soft timer, you should
 * initialize a soft timer group, then the soft timers are attached to this
 * group. There more, you should call the timer group's clock handler function
 * periodicaly, and the frequency must equal with the value that you initialized
 * the timer group.
 *
 * \internal
 * \par modification history:
 * - 1.00 13-08-30  zen, first implementation, the algorithm is inherited
 *                  from orz.
 * \endinternal
 */

#include "apollo.h"
#include "aw_int.h"
#include "aw_sftimer_lib.h"

/*******************************************************************************
  implementation
*******************************************************************************/

/**
 * \brief modify timer exipre time and add it to timer buckets
 * \param tmr the timer to be started
 * \param expires ticks to exipred
 * \note before call this function, the caller should take the timer lock,
 *       and ensure thart the arguments are correct
 */
static void __sftimer_add (struct aw_sftimer *p_timer, uint32_t expires)
{
    struct aw_sftimer_group *p_timer_grp = p_timer->p_timer_grp;

    p_timer->expires = expires;

    /* calculate bucket and round */
    p_timer->bucket = (expires + p_timer_grp->cur_bucket) %
                       p_timer_grp->bucket_cnt;

    p_timer->round  = expires / p_timer_grp->bucket_cnt;

    /* current bucket is not the bucket we handler in timer_tick(), so ... */
    if (p_timer->bucket == p_timer_grp->cur_bucket) {
        p_timer->round--;
    }

    /* detach if pending */
    if (!aw_list_empty(&p_timer->listnode)) {
        aw_list_del_init(&p_timer->listnode);
    }

    /* insert into timer buckets */
    aw_list_add(&p_timer->listnode,
                &p_timer_grp->p_buckets[p_timer->bucket].list_head);
}

/******************************************************************************/
void aw_sftimer_group_tick (struct aw_sftimer_group *p_timer_grp)
{
    struct aw_list_head *tmr_list, *tmr_list_head, *tmp;
    struct aw_sftimer   *tmr;

    AW_INT_CPU_LOCK_DECL(lock);

    if ((p_timer_grp             == NULL) ||
        (p_timer_grp->p_buckets  == NULL) ||
        (p_timer_grp->bucket_cnt == 0) ||
        (p_timer_grp->clkrate    == 0)) {
        return;
    }

    /* move to next timer bucket and check If reach the end of buckets */
    if (++p_timer_grp->cur_bucket >= p_timer_grp->bucket_cnt) {
        p_timer_grp->cur_bucket = 0;
    }

    AW_INT_CPU_LOCK(lock);

    /* get timer list head of current bucket */
    tmr_list_head = &p_timer_grp->p_buckets[p_timer_grp->cur_bucket].list_head;

    /* now go through the timer list of current bucket */
    if (!aw_list_empty(tmr_list_head)) {
        aw_list_for_each_safe(tmr_list, tmp, tmr_list_head) {
            tmr = aw_list_entry(tmr_list, struct aw_sftimer, listnode);
            if (tmr->round > 0) {
                /* timer not expired */
                tmr->round--;
            } else {
                /* timer is expired, remove the timer and call the handler */
                aw_list_del_init(&tmr->listnode);
                AW_INT_CPU_UNLOCK(lock);
                tmr->func(tmr->arg);
                AW_INT_CPU_LOCK(lock);
            }
        }
    }
    AW_INT_CPU_UNLOCK(lock);
}

/******************************************************************************/
aw_err_t aw_sftimer_group_init (struct aw_sftimer_group  *p_timer_grp,
                                struct aw_sftimer_bucket *p_buckets,
                                size_t                    bucket_cnt,
                                unsigned int              clkrate)
{
    int i;

    if ((p_timer_grp == NULL) ||
        (p_buckets   == NULL) ||
        (bucket_cnt  == 0)    ||
        (clkrate     == 0)) {

        return -AW_EINVAL;
    }

    p_timer_grp->p_buckets  = p_buckets;
    p_timer_grp->bucket_cnt = bucket_cnt;
    p_timer_grp->clkrate    = clkrate;

    p_timer_grp->cur_bucket = 0;

    for (i = 0; i < p_timer_grp->bucket_cnt; i++) {
        aw_list_head_init(&p_timer_grp->p_buckets[i].list_head);
    }

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_sftimer_init (struct aw_sftimer       *p_timer,
                          struct aw_sftimer_group *p_timer_grp,
                          void                   (*func) (void *arg),
                          void                    *arg)
{
    if ((p_timer     == NULL) ||
        (p_timer_grp == NULL) ||
        (func        == NULL)) {

        return -AW_EINVAL;
    }

    aw_list_head_init(&p_timer->listnode);

    p_timer->p_timer_grp = p_timer_grp;
    p_timer->func        = func;
    p_timer->arg         = arg;
    p_timer->expires     = 0;

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_sftimer_start (struct aw_sftimer *p_timer, unsigned int ticks)
{
    AW_INT_CPU_LOCK_DECL(lock);

    if (p_timer == NULL) {
        return -AW_EINVAL;
    }

    if ((p_timer->p_timer_grp == NULL) ||
        (p_timer->func        == NULL)) {
        return -AW_EBADF;
    }

    AW_INT_CPU_LOCK(lock);
    __sftimer_add(p_timer, ticks > 0 ? ticks : 1);
    AW_INT_CPU_UNLOCK(lock);

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_sftimer_start_ms (struct aw_sftimer *p_timer, unsigned int ms)
{
    unsigned int ticks;

    AW_INT_CPU_LOCK_DECL(lock);

    if (p_timer == NULL) {
        return -AW_EINVAL;
    }

    if ((p_timer->p_timer_grp == NULL) ||
        (p_timer->func        == NULL)) {
        return -AW_EBADF;
    }

    ticks = AW_DIV_ROUND_UP(ms * p_timer->p_timer_grp->clkrate, 1000u);

    AW_INT_CPU_LOCK(lock);
    __sftimer_add(p_timer, ticks > 0 ? ticks : 1);
    AW_INT_CPU_UNLOCK(lock);

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_sftimer_restart (struct aw_sftimer *p_timer)
{
    return aw_sftimer_start(p_timer, p_timer->expires);
}

/******************************************************************************/
aw_err_t aw_sftimer_stop (struct aw_sftimer *p_timer)
{
    AW_INT_CPU_LOCK_DECL(lock);

    if (p_timer == NULL) {
        return -AW_EINVAL;
    }

    if ((p_timer->p_timer_grp == NULL) ||
        (p_timer->func        == NULL)) {
        return -AW_EBADF;
    }

    AW_INT_CPU_LOCK(lock);
    aw_list_del_init(&p_timer->listnode);
    AW_INT_CPU_UNLOCK(lock);

    return AW_OK;
}

/* end of file */

