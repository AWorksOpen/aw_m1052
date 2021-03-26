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
 * \brief deferal job library configure file
 *
 * \internal
 * \par modification history:
 * - 1.00 17-03-24  may, first implementation
 * \endinternal
 */

#include "apollo.h"
#include "aw_int.h"
#include "aw_defer.h"
#include "aw_sem.h"
#include "aw_assert.h"

/*******************************************************************************
  macros
*******************************************************************************/
#define __IRQ_SAVE_DISABLE(olk) AW_INT_CPU_LOCK(old)
#define __IRQ_SAVE_RESTORE(old) AW_INT_CPU_UNLOCK(old)

/******************************************************************************/
void aw_defer_init (struct aw_defer        *p_this)
{
    AW_SEMC_INIT(p_this->sem, 0, AW_SEM_Q_PRIORITY);
    AW_INIT_LIST_HEAD(&p_this->head);

    aw_assert(p_this != NULL);
}

/******************************************************************************/
void aw_defer_task_entry(struct aw_defer *p_this)
{
    struct aw_defer_djob *pjob;
    void (*func)(void *);
    void *param;
    AW_INT_CPU_LOCK_DECL(old);
    struct aw_list_head *p;

    while (1) {
        if (AW_OK == AW_SEMC_TAKE(p_this->sem, AW_SEM_WAIT_FOREVER)) {
            __IRQ_SAVE_DISABLE(old);
            if (aw_list_empty(&p_this->head)) {
                __IRQ_SAVE_RESTORE(old);
                continue;
            }
            p = p_this->head.next;
            aw_list_del_init(p);
            pjob = aw_list_entry(p, struct aw_defer_djob, node);
            func = pjob->func;
            param = pjob->param;
            __IRQ_SAVE_RESTORE(old);
            if (func) {
                func(param);
            }
        }
    }
}

/******************************************************************************/
void aw_defer_job_init (struct aw_defer_djob   *p_this,
                        void                  (*pfunc) (void *),
                        void                   *p_arg)
{
    AW_INIT_LIST_HEAD(&p_this->node);

    p_this->func  = pfunc;
    p_this->param = p_arg;
}

/******************************************************************************/
aw_err_t aw_defer_commit( struct aw_defer *p_this, struct aw_defer_djob *p_job)
{
    AW_INT_CPU_LOCK_DECL(old);

    if ((NULL == p_this) || (NULL == p_job)) {
        return  -AW_EINVAL;
    }

    __IRQ_SAVE_DISABLE(old);
    if (p_job->node.next == NULL && p_job->node.prev == NULL) {
        AW_INIT_LIST_HEAD(&p_job->node);
    }
    if (!aw_list_empty(&p_job->node)) {
        __IRQ_SAVE_RESTORE(old);
        return -AW_EEXIST;
    }
    aw_list_add_tail(&p_job->node, &p_this->head);
    __IRQ_SAVE_RESTORE(old);

    /**
     * aw_semc_give will always return 0 (OK).
     * If not, everything will go wrong.
     */
    return  AW_SEMC_GIVE(p_this->sem);
}

/******************************************************************************/
aw_err_t aw_defer_abort( struct aw_defer *p_this, struct aw_defer_djob  *p_job)
{
    AW_INT_CPU_LOCK_DECL(old);

    if ((NULL == p_this) || (NULL == p_job)) {
        return  -AW_EINVAL;
    }

    __IRQ_SAVE_DISABLE(old);
    aw_list_del_init(&p_job->node);
    __IRQ_SAVE_RESTORE(old);

    return  AW_OK;
}

/******************************************************************************/
aw_bool_t aw_defer_job_is_usable (struct aw_defer *p_this, struct aw_defer_djob *p_job)
{
    aw_bool_t ret;
    AW_INT_CPU_LOCK_DECL(old);

    if ((NULL == p_this) || (NULL == p_job)) {
        return  AW_FALSE;
    }

    __IRQ_SAVE_DISABLE(old);
    ret = aw_list_empty(&p_job->node);
    __IRQ_SAVE_RESTORE(old);

    return ret ? AW_TRUE : AW_FALSE;
}
