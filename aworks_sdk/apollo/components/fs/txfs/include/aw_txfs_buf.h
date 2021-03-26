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
 * \brief file system buffer cache interface.
 *
 * \internal
 * \par Modification History
 * - 1.00 17-10-23  mkr, first implementation.
 * \endinternal
 */

#ifndef __AW_TXFS_BUF_H
#define __AW_TXFS_BUF_H

#include "aw_txfs_list.h"
#include "aw_sem.h"
#include "fs/aw_blk_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Type-name for "logical block number".  Buffers are named by
 * <vnode, logical block number> pairs, so that a file system that
 * wants to get "block 3" of a file (vnode), you just get <vp,3>.
 * (The block size is up to the user of the buffer code.)
 *
 * This type is also used for "post translation" block numbers
 * (optional, up to the file system) so the name is a little misleading.
 * Note that for BSD compatibility, initially bp->b_pblkno == bp->b_lblkno,
 * so one can use that as a test for "has this been translated yet", but
 * it is better to use the file-system flags.
 */
typedef unsigned long long aw_txfs_lblkno_t;    /* ??? signed? */

typedef struct aw_txfs_ucred aw_txfs_ucred_t;
typedef struct aw_txfs_mount aw_txfs_mount_t;
struct aw_txfs_buf;


AW_SDLIST_HEAD(bhashhdr, aw_txfs_buf);
AW_TAILQ_HEAD(bfreehdr, aw_txfs_buf);

/*
 * Note that a buffer can be both free and on a vnode.  This occurs when
 * the buffer is cacheing valid data.
 *
 * Buffers are taken off their free list while "owned", but are always
 * on a hash chain (kept in the mount point) based on their buffer hash
 * value.
 *
 * Each buffer also has an I/O semaphore that is taken when I/O is
 * started, and given only when the I/O is finished.  Thus, if you
 * can get the I/O semaphore, I/O is done (or was never started).
 * The code to start the I/O should always be able to get the semaphore
 * without waiting (otherwise it is a repeated start w/o first finishing).
 *
 * Each buffer has an "availability semaphore.  This semaphore is taken
 * when the buffer is allocated to a task, and given when the buffer is
 * released for other tasks to use.
 *
 * The AW_TXFS_B_BUSY bit is essentially the same information as in b_avail
 * (it is documented as "I/O in progress", but this is a white lie).
 * Taking b_avail allows a task to wait for AW_TXFS_B_BUSY to clear.
 *
 * There is some subtlety with async I/O; see aw_txfs_buf_subr.c.
 *
 * The b_fsflags field is cleared whenever a buffer is first associated
 * with a vnode, at which point b_pblkno is set equal to b_lblkno.  A
 * file system typically translates b_lblkno to a different underlying
 * physical block number, and is allowed to use b_pblkno as a cache for
 * the resulting translation, with or without a flag in b_fsflags to
 * indicate that the translation has been done:
 *
 *  if ((bp->b_fsflags & FS_DEFINED_VALUE) == 0)
 *      {
 *      bp->b_pblkno = SOME_FUNCTION(bp->b_lblkno);
 *      bp->b_fsflags |= FS_DEFINED_VALUE;
 *      }
 *
 * or the traditional BSD file system method:
 *
 *  if (bp->b_pblkno == bp->b_lblkno)
 *      bp->b_pblkno = SOME_FUNCTION(bp->b_lblkno);
 *
 * The former has the advantage of handling the (unusual but possible)
 * case where pblkno==f(lblkno), e.g., logical block 10041 of some
 * vnode just happens to map to physical block 10041 in the file system.
 * The latter still works, it just does "too much" work.
 *
 * The fsflags field can be used for other purposes as well; the file
 * system can "know" that these flags will be zeroed out when the buffer
 * is dis-associated from its old vnode and re-associated with a new one.
 */
struct aw_txfs_buf {
    /** \brief mutex to lock buffer while owned */
    AW_MUTEX_DECL(b_mutex);

    /** \brief I/O semaphore, during in transfer data */
    AW_SEMB_DECL(b_iosem);

    /**
     * \brief availability semaphore,
     * indicate whether buffer available(free) or not
     */
    AW_SEMB_DECL(b_avail);

    /** \brief see below */
    int b_flags;

    /** \brief reserved to file system */
    int b_fsflags;

    /**
     * \brief don't need b_size yet -
     *        TXFS buffers are all one size per mount
     */
    int b_bsize;    /** \brief size of data */

    /** \brief this buffer's data (shared w/bio) */
    void *b_data;

    /** \brief ### need more, eventually? */
    struct aw_blk_arp *b_bio;

    /** \brief error for buf_wait */
    int b_error;

    /** \brief this buffer's vnode */
    struct aw_txfs_vnode *b_vp;

    /** \brief and its logical block number */
    aw_txfs_lblkno_t b_lblkno;

    /** \brief physical (translated) blkno */
    aw_txfs_lblkno_t b_pblkno;

    /** \brief cache priority */
    int b_priority;

    /** \brief hash chain */
    AW_SDLIST_ENTRY(aw_txfs_buf) b_hash;

    /** \brief chain of buffers on this vnode */
    AW_SDLIST_ENTRY(aw_txfs_buf) b_vnbufs;

    /** \brief chain if free */
    AW_TAILQ_ENTRY(aw_txfs_buf) b_freelist;

    /** \brief private data of buffer */
    void *b_priv;
};

/**
 * \brief Flags for b_flags.
 *
 * The names are from NetBSD; the bit-value numbers do not match though.
 */
#define AW_TXFS_B_ASYNC   0x00000001 /**< \brief Start I/O, do not wait */
#define AW_TXFS_B_BUSY    0x00000002 /**< \brief I/O in progress */

#define AW_TXFS_B_DELWRI  0x00000008 /**< \brief buffer has delayed-write data */
#define AW_TXFS_B_DONE    0x00000010 /**< \brief I/O completed */

#define AW_TXFS_B_EINTR   0x00000020 /**< \brief I/O was interrupted (for NFS) */
#define AW_TXFS_B_ERROR   0x00000040 /**< \brief I/O error occurred */
#define AW_TXFS_B_INVAL   0x00000100 /**< \brief does not contain valid info */
#define AW_TXFS_B_READ    0x00000200 /**< \brief read buffer */
#define AW_TXFS_B_WRITE   0x00000400 /**< \brief write buffer */

#define AW_TXFS_B_PRIVATE 0x00000800 /**< \brief this buffer has allocate private data buffer */
#define AW_TXFS_B_INTRANS 0x00001000 /**< \brief already in the transaction */

/**
 * \brief Buffer caching "levels" (priorities).
 *
 * A buffer that contains some valid data goes on one of the "higher-level"
 * queues, in LRU fashion, by putting released buffers at the end of the queue
 * and taking buffers off the front of the queue.
 *
 * The ones in the "trash" queue by definition have no valid data
 * (the code itself does not really need this distinction at the
 * moment, but it seems reasonable to keep it).
 */
#define AW_TXFS_BUF_TRASH       0  /**< \brief "trash" queue: no valid data */
#define AW_TXFS_BUF_USED_ONCE   1  /**< \brief blocks that are used just once */
#define AW_TXFS_BUF_ORDINARY    2  /**< \brief ordinary data/transactions */
#define AW_TXFS_BUF_META_LOW    3  /**< \brief low priority metadata */
#define AW_TXFS_BUF_META_HIGH   4  /**< \brief high priority metadata */
#define AW_TXFS_BUF_TRANS       5  /**< \brief high priority metadata */
#define AW_TXFS_BUF_NQUEUES     6  /**< \brief number of free-list queues */

/** \brief setting buffer's priority */
#define aw_txfs_buf_setpriority(bp, pri) ((bp)->b_priority = (pri))

/*
 * Buffer hashing, per mount-point.  This hash function probably needs
 * tuning.  Note that the current AW_TXFS_BUFHASH definition depends on BHSIZE
 * being a power of two.
 */
#define AW_TXFS_BUF_BHSIZE 32
#define AW_TXFS_BUFHASH(vp, lbn) \
    (((((long)(vp) >> 8) + (int)(lbn))) & (AW_TXFS_BUF_BHSIZE - 1))

/**
 * \brief initialize buffer-related fields for a mount point
 * \param[in] mp  pointer to mount
 * \return N/A
 */
void aw_txfs_buf_initmountpoint(aw_txfs_mount_t *p_mnt);

/**
 * \brief Create the buffer cache for the given mount point, with the given
 *        number of buffers.
 *
 * This is also the number of bios (at least at the moment), and you must
 * provide a pointer to the bios (which in turn point to their data space),
 * and the (fixed) size of each buffer.
 *
 * \param[in] mp        mount point for these buffers
 * \param[in] nbuffers  number of buffers desired
 * \param[in] bufsize   data size for each buffer
 * \param[in] biobase   first of the nbuffers bio structs
 *
 *\return errno
 */
int aw_txfs_buf_makebuffers (aw_txfs_mount_t   *p_mnt,
                             int                nbufs,
                             int                buf_size,
                             struct aw_blk_arp *p_biobase);

/**
 * \brief Release the buffer cache for the given mount point.
 *
 * \param[in] mp  pointer to mount
 *
 * We assume that all I/O is finished, including any delayed writes,
 * so that all buffers are on the free lists and no I/O will be required.
 * We do still brelvp() each vnode though.
 *
 * Note that this does not give up the bio's and bio-space.
 *
 * \return N/A
 */
void aw_txfs_buf_deletebuffers (aw_txfs_mount_t *p_mnt);

/**
 * aw_txfs_getblk() is BSD-compatible, except that we ignore the two "unused"s
 * (so they had better always be 0!  in BSD they control sleep() behavior).
 *
 * bread() does a aw_txfs_getblk() and then reads the underlying data if the
 * block was not already in cache.  bwrite() writes the data, and there
 * are several variations on the theme.  In general, once you aw_txfs_getblk()
 * or bread(), you own that <vnode,lbn> pair until you brelse() or
 * bwrite() it to give it back.
 *
 * For file systems that move underlying data, we provide buf_moveblk(),
 * to move a buffer (which you must own) from one <vp,lbn> pair to
 * another.  The new vnode may be the same as the old one.  It is OK to
 * move the lbn to a cached LBN (this just invalidates the old cached
 * copy) -- it has to be, since the caller can't tell which LBNs are
 * cached -- but it is an error to move it to a <vp,lbn> pair that is
 * currently both cached and owned.  (This means you have a bug in your
 * file system, since you think you are working with that <vp,lbn> pair
 * on behalf of this task or some other task.)  In this case you will
 * get EBUSY as a return value.  (??? maybe we should just panic?)
 *
 * Both vnodes must be on the same mount point (if not, you get an
 * error, again).
 *
 * We also provide buf_swapdata, which simply swaps the underlying
 * data of two buffers (you must own both) without even looking at
 * anything else.
 */
struct aw_txfs_buf *aw_txfs_buf_getblk (struct aw_txfs_vnode *p_vnode,
                                        aw_txfs_lblkno_t      lblkno,
                                        int                   size);

/**
 * \brief Re-Busy a buffer in vnode
 *
 * \param[in] bp  a buffer in vnode
 * \param[in] vp  vnode pointer
 *
 * \return errno
 *         AW_OK      successful
 *         -AW_EINVAL the buffer(bp) does not belong to the vnode - vp
 *         -AW_EBUSY  the buffer has busy
 */
int aw_txfs_buf_rebusy (struct aw_txfs_buf   *p_buf,
                        struct aw_txfs_vnode *p_vnode);

/**
 * \brief Move a busy buffer to a new <newvn, newlbn> pair
 *
 * \details Waiting all the buffers in new vnode were completely committed,
 *          and insert the buffer to new vnode
 *
 * \param[in] bp      buffer
 * \param[in] newvn   new vnode
 * \param[in] newlbn  new logical block number
 *
 * \return errno
 *         AW_OK      successful
 *         -AW_EXDEV  cross different mount point
 *         -AW_EINVAL buffer no busy
 *         -AW_EBUSY  the buffer in new vnode was using
 */
int aw_txfs_buf_moveblk (struct aw_txfs_buf   *p_buf,
                         struct aw_txfs_vnode *p_newvnode,
                         aw_txfs_lblkno_t      newlbn);

/**
 * \brief Swap data in two buffers
 *
 * \param[in] b1  one buffer
 * \param[in] b2  another buffer
 *
 * \return N/A
 */
void aw_txfs_buf_swapdata(struct aw_txfs_buf *p_buf1,
                          struct aw_txfs_buf *p_buf2);

/**
 * \brief get a buffer according to vnode and logical block number
 *
 * \param[in] vp      the vnode you want
 * \param[in] lblkno  the logical block you want
 * \param[in] size    the size of that block
 *
 * \note the return buffer size is equal to blksize when it format now,
 *       so the actually size maybe no equal to the parameter 'size'
 *
 * \return the buffer pointer
 */
#define aw_txfs_getblk(vp, lbn, size, unused1, unused2) \
        aw_txfs_buf_getblk(vp, lbn, size)

/**
 * \brief Reading a block data according to vnode and logical block number
 *
 * \param[in] p_vnode     vnode
 * \param[in] lblkno      logical block number
 * \param[in] size        the size of the block
 * \param[in] p_cred      creditence
 * \param[in,out] pp_buf  the return buffer contain data if success, else NULL
 *
 * \return errno
 */
int aw_txfs_bread (struct aw_txfs_vnode  *p_vnode,
                   aw_txfs_lblkno_t       lblkno,
                   int                    size,
                   aw_txfs_ucred_t       *p_cred,
                   struct aw_txfs_buf   **pp_buf);

#ifdef notyet
int breada(struct aw_txfs_vnode *,
           aw_txfs_lblkno_t,
           int,
           aw_txfs_lblkno_t,
           int,
           aw_txfs_ucred_t *,
           struct aw_txfs_buf **);
#endif

/**
 * \brief  Read a block data(sync) and start read-ahead
 *         on N more blocks data(async).
 *
 * \param[in] p_vnode     vnode
 * \param[in] lblkno      logical block number
 * \param[in] size        logical blcok size
 *
 * \param[in] p_rablks    read-ahead block numbers
 * \param[in] p_rasizes   read-ahead size
 * \param[in] nrablks     read-ahead block accounts
 * \param[in] p_cred      credential
 * \param[in,out] pp_buf  the return buffer
 *
 * \return errno
 */
int aw_txfs_breadn (struct aw_txfs_vnode  *p_vnode,
                    aw_txfs_lblkno_t       lblkno,
                    int                    size,
                    aw_txfs_lblkno_t      *p_rablks,
                    int                   *p_rasizes,
                    int                    nrablks,
                    aw_txfs_ucred_t       *p_cred,
                    struct aw_txfs_buf   **pp_buf);

/**
 * \brief Write buffer data to disk
 *
 * \param[in] p_buf  buffer to be written
 *
 * \return errno
 */
int aw_txfs_bwrite(struct aw_txfs_buf *p_buf);

/**
 * \brief Start a device-level write on a buffer.
 *        caller still owns(still has lock) buf
 *
 * \param[in] p_buf  buffer to be written
 *
 * \return N/A
 */
void aw_txfs_buf_startwrite(struct aw_txfs_buf *p_buf);

/**
 * \brief Delayed write: write after timeout
 *
 * \note The buffer is marked dirty, but is not queued for I/O.
 *       This routine should be used when the buffer is expected
 *       to be modified again soon, typically a small write that
 *       partially fills a buffer.
 *
 * \param[in] p_buf  buffer to be wrote
 *
 * \return N/A
 */
void aw_txfs_bdwrite(struct aw_txfs_buf *p_buf);

/**
 * \brief Async write, start write but don't wait
 *
 * \param[in] p_buf  buffer to be wrote
 *
 * \return N/A
 */
void aw_txfs_bawrite(struct aw_txfs_buf *bp);

/**
 * \brief Release buffer to free list, no I/O
 *
 * \param[in] p_buf  buffer to be released
 *
 * \return N/A
 */
void aw_txfs_brelse(struct aw_txfs_buf *p_buf);

/**
 * \brief Wait for I/O finished
 *
 * \param[in] p_buf  buffer to be transfer
 *
 * \return errno
 */
int aw_txfs_buf_wait(struct aw_txfs_buf *p_buf);

/**
 * \brief Indicate I/O completion on a buffer
 *
 * \param[in] p_buf    buffer has been complete transfered
 * \param[in] error error number
 *
 * \return N/A
 */
void aw_txfs_buf_done(struct aw_txfs_buf *p_buf, int error);

#ifdef __cplusplus
}
#endif

#endif /* __AW_TXFS_BUF_H */

/* end of file */
