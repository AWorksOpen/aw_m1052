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
 * \brief POSIX lockf() internals
 *
 * \internal
 * \par Modification History
 * - 1.00 17-10-23  mkr, first implementation.
 * \endinternal
 */

/*
 *  NetBSD: lockf.h,v 1.15 2004/04/25 16:42:43 simonb Exp
 *
 * Copyright (c) 1991, 1993
 *  The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Scooter Morris at Genentech Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *  @(#)lockf.h 8.2 (Berkeley) 10/26/94
 */

#ifndef __INClockfPh
#define __INClockfPh

#include "aw_txfs_vnode.h"
#include "aw_sem.h"
#include "aw_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The aw_txfs_lockf structure is a kernel structure which contains the 
 * information associated with a byte range lock.  There is one per lock per 
 * task.
 * These are linked into the vnode (in several ways, vp->v_lfhead is all
 * locks on the vnode and vp->v_lfbq is all "locks that are currently
 * blocking another lock" on that vnode).
 *
 * Locks are sorted by the starting byte of the lock for efficiency.
 *
 * p_lf_next is used for two purposes, depending on whether the lock is
 * being held, or is in conflict with an existing lock.  If this lock
 * is held, it indicates the next lock on the same vnode.
 * For pending locks, if lock->p_lf_next is non-NULL, then lock->lf_block
 * must be queued on the lf_blkhd TAILQ of lock->p_lf_next.
 */

AW_TAILQ_HEAD(aw_txfs_locklist, aw_txfs_lockf);

struct aw_txfs_lockf {
    /** \brief semaphore on which to wait for wake */
    aw_semb_id_t  lf_blocksem;

    short   lf_flags;   /**< \brief see below */
    short   lf_type;    /**< \brief Lock type: F_RDLCK, F_WRLCK */
    voff_t  lf_start;   /**< \brief The byte # of the start of the lock */
    voff_t  lf_end;     /**< \brief The byte # of the end of the lock (-1=EOF)*/
    void   *p_lf_task;  /**< \brief task that created this lock */
    void   *p_lf_id;    /**< \brief lock identifier (from VOP_ADVLOCK) */

    /** \brief Back pointer to the head of aw_txfs_lockf list */
    struct  aw_txfs_lockf **pp_lf_head;

    /** \brief Next lock on this vnode, or blocking lock */
    struct  aw_txfs_lockf  *p_lf_next;

    /** \brief List of requests blocked on this lock */
    struct  aw_txfs_locklist lf_blkhd;

    /** \brief A request waiting for a lock */
    AW_TAILQ_ENTRY(aw_txfs_lockf) lf_block;

    /** \brief list of all blocked locks on a vnode */
    AW_SDLIST_ENTRY(aw_txfs_lockf) lf_bq;
};

/**
 * \brief Flags for lf_flags.
 * Note that the first several are actually in sys/fcntlcom.h.
 * AW_TXFS_F_RDLCK, AW_TXFS_F_WRLCK, and AW_TXFS_F_UNLCK are arguments to lockf().
 */
/*  AW_TXFS_F_RDLCK     1   -- shared or read lock */
/*  AW_TXFS_F_WRLCK     2   -- write lock */
/*  AW_TXFS_F_UNLCK     3   -- remove lock */
/*  AW_TXFS_F_UNLKSYS   4   -- remove remote locks for a given system */
#define AW_TXFS_F_WAIT      0x10    /* wait until lock is granted */
#define AW_TXFS_F_FLOCK     0x20    /* BSD semantics for lock (not used yet) */
#define AW_TXFS_F_POSIX     0x40    /* POSIX semantics for lock */

extern int lf_advlock (struct aw_txfs_vnode *, void *, int,
                       struct aw_txfs_flock *, voff_t, int);

extern int (*_func_lf_advlock) (struct aw_txfs_vnode *, void *, int,
                                struct aw_txfs_flock *, voff_t, int);

#ifdef __cplusplus
}
#endif

#endif /* __INClockfPh */

/* end of file */

