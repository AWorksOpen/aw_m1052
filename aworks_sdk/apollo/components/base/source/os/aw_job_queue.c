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
 * \brief job queue library
 *
 * 使用本服务需要包含以下头文件:
 * \code
 * #include "aw_job_queue.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 14-03-19  zen, first implementation
 * - 1.00 15-06-02  deo, add aw_job_queue_fill_job,
 * \endinternal
 */

#include "apollo.h"
#include "aw_job_queue.h"
#include "aw_task.h"
#include "aw_sem.h"
#include "aw_list.h"
#include "aw_spinlock.h"
#include "aw_mem.h"
#include "aw_cache.h"
#include "aw_vdebug.h"
#include "string.h"

/******************************************************************************/
aw_err_t aw_job_queue_init (aw_job_queue_t        *p_jobq,
                            aw_qjob_std_pool_id_t std_pool_id)
{
    int i;

    if (p_jobq == NULL) {
        return -AW_EINVAL;
    }

    memset(p_jobq, 0, sizeof (*p_jobq));

    aw_spinlock_isr_init(&p_jobq->lock, 0);

    aw_spinlock_isr_take(&p_jobq->lock);

    for (i = 0; i < AW_SRCFG_QJOB_NUM_PRI; i++)
    {
        aw_list_head_init(&p_jobq->jobs[i]);
    }

    /* activePri is already zeroed out. */


    p_jobq->task_id = NULL;

    if ((p_jobq->p_std_pool = std_pool_id) != NULL) {
            ++std_pool_id->ref_count;   /* mostly informational */
        /*
         * We need to stash this here just so we'll be able to acquire
         * the lock in jobQueueStdJobsAlloc(), since that function is
         * never given a pointer to the jobQueue structure.
         */

        std_pool_id->p_lock = &p_jobq->lock;
    }

    aw_spinlock_isr_give(&p_jobq->lock);

    /* This can't really realistically fail... */

    AW_SEMB_INIT(p_jobq->semb, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);

    return AW_OK;
}

void aw_job_queue_fill_job (aw_qjob_t       *p_job,
                            aw_pfuncvoid_t  func,
                            void            *p_arg,
                            uint16_t        pri,
                            uint16_t        flags)
{
    p_job->func = func;
    p_job->p_arg = p_arg;
    p_job->pri = pri & AW_QJOB_PRI_MASK;
    p_job->flags = flags;
    aw_list_head_init(&p_job->node);
}



/******************************************************************************/
aw_job_queue_id_t aw_job_queue_create (aw_qjob_std_pool_id_t std_pool_id)
{
    aw_job_queue_t *p_jobq;
    aw_err_t retval;

    p_jobq = aw_mem_align(sizeof(* p_jobq), AW_CACHE_LINE_SIZE);
    if (p_jobq == NULL) {
        return NULL;
    }

    retval = aw_job_queue_init (p_jobq, std_pool_id);
    if (retval == AW_OK) {
        p_jobq->flags |= AW_JOBQ_FLG_CREATED;
        return p_jobq;
    }

    aw_mem_free(p_jobq);

    return NULL;
}

/******************************************************************************/
void aw_job_queue_delete (aw_job_queue_id_t p_jobq)
{
    aw_mem_free(p_jobq);
}

/******************************************************************************/
aw_err_t aw_job_queue_post (aw_job_queue_id_t job_queue_id, aw_qjob_t *p_job)
{
    uint32_t pri;
    uint32_t old_active;

    if (job_queue_id == NULL || p_job == NULL || p_job->func == NULL) {
        return -AW_EINVAL;
    }

    aw_spinlock_isr_take(&job_queue_id->lock);

    pri = p_job->flags;
    if ((pri & (AW_QJOB_ENQUEUED | AW_QJOB_POOL_ALLOC)) == 0) {
        p_job->flags = pri | AW_QJOB_ENQUEUED;

        pri = (p_job->pri & AW_QJOB_PRI_MASK); /* extract just the priority field */

        aw_list_add_tail(&p_job->node, &job_queue_id->jobs[pri]);

        old_active = job_queue_id->active_pri;
        job_queue_id->active_pri = old_active | (1 << pri);

        aw_spinlock_isr_give(&job_queue_id->lock);

        /*
         * Give the semaphore if no (enabled) priorities previously had queued
         * jobs, and the priority of the job just posted is enabled.
         */
        if (old_active == 0) {
            AW_SEMB_GIVE(job_queue_id->semb);
        }

        return AW_OK;
    }

    aw_spinlock_isr_give(&job_queue_id->lock);

    return -AW_EBUSY;
}

/******************************************************************************/
aw_err_t aw_job_queue_std_post (aw_job_queue_id_t job_queue_id,
                                int               priority,
                                aw_pfuncvoid_t    func,
                                void             *arg1,
                                void             *arg2,
                                void             *arg3,
                                void             *arg4)
{
    uint32_t               old_active;
    aw_qjob_std_t         *p_job;
    aw_qjob_std_pool_id_t  p_std_pool;

    if ((job_queue_id == NULL) || (func == NULL)) {
        return -AW_EINVAL;
    }

    p_std_pool = job_queue_id->p_std_pool;

    /*
     * It's an error to try to allocate a standard job if the job queue
     * isn't attached to a standard job pool.
     * The pStdPool pointer should be stable, so we don't need
     * to do this test locked.
     */
    if (p_std_pool == NULL) {
        return -AW_ENOMEM;
    }

    priority &= AW_QJOB_PRI_MASK;

    aw_spinlock_isr_take(&job_queue_id->lock);

    if (aw_list_empty_careful(&p_std_pool->free)) {
        aw_spinlock_isr_give(&job_queue_id->lock);
        AW_WARNF(("[job queue] Std_job_pool(%x) out of items \n", p_std_pool));
        return -AW_ENOENT;
    }

    /* pop job object from free list */
    p_job = (aw_list_entry(p_std_pool->free.next, aw_qjob_std_t, base.node));
    aw_list_del_init(p_std_pool->free.next);

    p_job->base.flags = AW_QJOB_ENQUEUED | AW_QJOB_POOL_ALLOC;
    p_job->base.pri   = priority;
    p_job->base.func  = func;
    p_job->arg1       = arg1;
    p_job->arg2       = arg2;
    p_job->arg3       = arg3;
    p_job->arg4       = arg4;

    /* push job to jobq list */
    aw_list_add_tail(&p_job->base.node, &job_queue_id->jobs[priority]);

    old_active = job_queue_id->active_pri;
    job_queue_id->active_pri = old_active | (1 << priority);

    aw_spinlock_isr_give(&job_queue_id->lock);

    if (old_active == 0) {
        AW_SEMB_GIVE(job_queue_id->semb);
    }

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_job_queue_process (aw_job_queue_id_t job_queue_id)
{
    uint32_t        active_pri;
    uint16_t        flags;
    aw_pfuncvoid_t  func;

    struct aw_list_head *p_q;
    aw_qjob_t           *p_job;

    aw_qjob_std_pool_t  *p_std_pool;
    aw_qjob_std_t       *p_stdjob;


    if ((job_queue_id == NULL) ||
        ((job_queue_id->flags & AW_JOBQ_FLG_RUNNING) != 0)) {
        return -AW_EINVAL;
    }

    p_std_pool = job_queue_id->p_std_pool;

    aw_spinlock_isr_take(&job_queue_id->lock);

    job_queue_id->flags |= AW_JOBQ_FLG_RUNNING;
    job_queue_id->task_id = aw_task_id_self();

again:
    aw_spinlock_isr_give (&job_queue_id->lock);

    AW_SEMB_TAKE(job_queue_id->semb, AW_SEM_WAIT_FOREVER);

    AW_FOREVER {

        int            pri;

        aw_spinlock_isr_take(&job_queue_id->lock);

check:
        active_pri = job_queue_id->active_pri;

        if (active_pri == 0) {
            goto again;
        }

        /*
         * Higher priority values are actually higher priority!
         * We need ffsMsb() to be optimized as much as possible for
         * each architecture.
         */
#if 0
        pri = AW_FFS(active_pri) - 1;
#else
        {
            int i;
            pri = 0;
            for (i =0; i < 32; i++) {
                if (AW_BIT_ISSET(active_pri, i)) {
                    pri = i;
                }
            }
        }
#endif
        /* Remove the job from the appropriate queue */
        p_q = &job_queue_id->jobs[pri];

        p_job = aw_list_entry(p_q->next, aw_qjob_t, node);
        aw_list_del_init(p_q->next);
        if (aw_list_empty_careful(p_q)) {
            AW_BIT_CLR(job_queue_id->active_pri, pri);
        }

        p_job->flags &= ~AW_QJOB_ENQUEUED;

        func  = p_job->func;
        flags = p_job->flags;

        aw_spinlock_isr_give (&job_queue_id->lock);

        if ((flags & AW_QJOB_POOL_ALLOC) == 0) {
            if (func) {
                func(p_job->p_arg);
            }
            continue;
        }

        /*
         * legacy job allocated from this pool's QJOB_STD_POOL.
         *
         * We don't check here whether pStdPool is NULL for this
         * job queue, since a check is done in jobQueueStdPost().
         */
        p_stdjob = (aw_qjob_std_t *)p_job;

        ((void (*) (void *, void *, void *, void *) )func)(p_stdjob->arg1,
                                                           p_stdjob->arg2,
                                                           p_stdjob->arg3,
                                                           p_stdjob->arg4);

        aw_spinlock_isr_take(&job_queue_id->lock);

        /* return jsp to the free list */
        aw_list_add_tail(&p_stdjob->base.node, &p_std_pool->free);


        goto check; /* avoid an unlock, lock sequence */
    }
    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_job_queue_std_pool_init (aw_qjob_std_pool_id_t std_pool_id)
{
    memset(std_pool_id, 0, sizeof (*std_pool_id));
    aw_list_head_init(&std_pool_id->free);
    aw_list_head_init(&std_pool_id->blocks);

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_job_queue_std_jobs_alloc (aw_qjob_std_pool_id_t std_pool_id,
                                      size_t                njobs)
{
    aw_qjob_std_t *p_block;
    int i;

    if ((njobs <= 0) || (std_pool_id == NULL)) {
        return -AW_EINVAL;
    }

    /*
     * Allocate first block of standard jobs. One extra job is allocated
     * to use to link the list of blocks.
     */
    p_block = aw_mem_alloc((njobs + 1) * sizeof(*p_block));
    if (p_block == NULL) {
        return -AW_ENOMEM;
    }
    memset (p_block, 0, (njobs + 1) * sizeof (*p_block));

    /* Link up the remaining njobs jobs */
    aw_list_head_init(&p_block->base.node);
    for (i = 1; i < (njobs + 1); i++) {
        aw_list_add_tail(&(p_block + i)->base.node, &p_block->base.node);
    }

    /*
     * pJs now points to the last job in the block
     * Link the block and the list of new free standard
     * jobs into the job queue.
     */
    aw_spinlock_isr_take(std_pool_id->p_lock);
    aw_list_splice_tail(&p_block->base.node, &std_pool_id->free);
    aw_spinlock_isr_give(std_pool_id->p_lock);

    return AW_OK;
}

/* end of file */

