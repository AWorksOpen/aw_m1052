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
 * \brief file system mount-point interface
 *
 * \internal
 * \par Modification History
 * - 1.00 17-10-23  mkr, first implementation.
 * \endinternal
 */

#ifndef __AW_TXFS_MOUNT_H
#define __AW_TXFS_MOUNT_H

#include "aw_txfs_types.h"
#include "aw_txfs_vnode.h"
#include "io/sys/aw_statvfs.h"
#include "aw_txfs_buf.h"

#ifdef __cplusplus
extern "C" {
#endif
/******************************************************************************/
/** \brief declaring */
struct aw_txfs_vfsops;
/******************************************************************************/

/** \brief aw_txfs_vnode delete list */
AW_SDLIST_HEAD(vnodelst, aw_txfs_vnode); 
AW_SDLIST_HEAD(vhashhdr, aw_txfs_vnode); /**< \brief aw_txfs_vnode hash list */

/**
 * Open-file table, per mount point.  In BSD this is in <sys/file.h>
 * and logically core I/O should do this, but here we have to do it
 * per mount point.
 *
 * At the moment, f_count is not needed... remove it?  (dup() is handled
 * inside core I/O, so that dup()ed fd's point to the same aw_txfs_vfile data
 * structure.)
 */
struct aw_txfs_vfile {
    /** \brief number of dup()s */
    u_short                        f_count;

    /** \brief AW_TXFS_FREAD and/or AW_TXFS_FWRITE */
    u_int                          f_flags;

    /** \brief aw_txfs_vnode for this file */
    struct aw_txfs_vnode          *p_f_vnode;

    /** \brief current seek pos */
    voff_t                         f_offset;

    /** \brief list of free files */
    AW_SLIST_ENTRY(aw_txfs_vfile)  f_next;
};

/** \brief mount point info */
typedef struct aw_txfs_mount {
    /** \brief connected mounted device(block device) */
    void                  *p_mnt_dev;

    /** \brief operations on fs */
    const struct aw_txfs_vfsops *p_mnt_op;

    /** \brief syncer aw_txfs_vnode */
    struct aw_txfs_vnode  *p_mnt_syncer;

    /** \brief vnode of root directory, result of VFS_ROOT() */
    struct aw_txfs_vnode  *p_mnt_rootvnode;

    /** \brief active vnodes */
    struct vnodelst        mnt_vnodelist;
#ifdef notyet
    /** \brief mount structure lock */
    struct lock            mnt_lock;
#else
    aw_mutex_id_t          mnt_lock; /* no used, remove it ? */
#endif
    /** \brief flags */
    int                    mnt_flag;

    /** \brief internal flags */
    int                    mnt_iflag;

    /** \brief file semaphore */
    AW_MUTEX_DECL(         mnt_filemutex);
    aw_mutex_id_t          mnt_filemutex_id;

    /** \brief file table */
    struct aw_txfs_vfile  *p_mnt_ftbl;

    /** \brief # files, for debug/show */
    int                    mnt_nfiles;

    /** \brief list of free files */
    AW_SLIST_HEAD(, aw_txfs_vfile) mnt_freefiles;

    /** \brief malloc() space for vnodes */
    char                  *p_mnt_vnodespace;

    /** \brief # vnodes, for debug/show */
    int                    mnt_nvnodes;

    /** \brief private data, point to aw_txfs_dev_t memory */
    void                  *p_mnt_data;

    /**
     * When doing I/O on a mounted file system, if buf_done() is
     * given an error, the error number is recoreded in the mount
     * point as well as the buffer.  This error "sticks" until
     * something clears it.  This is mostly for file systems that
     * do async I/O and want to see if some error(s) have occurred
     * since they last checked.  We save the "first" error, rather
     * than the "last", on the theory that the first one is likely
     * the most informative (subsequent errors being due to the
     * first one).
     *
     * (Essentially, the technique is: push out any delayed or
     * async I/O, then check these, then clear them for the next
     * time.)
     */
    /** \brief first read errno */
    int                   mnt_read_errno;

    /** \brief first write errno */
    int                   mnt_write_errno;

    /**
     * These items are for buffer management.  Note that there are
     * three semaphores involved: a counting semaphore for "number of
     * unowned buffers", a mutex for hash chain manipulation, and
     * one semaphore per buffer.  If you are taking a buffer semaphore
     * and the hash chain mutex, you must take the buffer semaphore
     * first (to avoid deadlocking).
     *
     * The hash chain mutex is also used to protect the free lists.
     *
     * The buffer getting routines (aw_txfs_getblk, bread) return locked
     * buffers (which are no longer on a free list) -- see buf.h.
     */
    /** \brief for free()ing */
    struct aw_txfs_buf  *p_mnt_bufspace;

    /** \brief and show()ing */
    int                  mnt_nbufs;

    /** \brief number of unowned buffers */
    AW_SEMC_DECL(        mnt_unowned_bufs);
    aw_semc_id_t         mnt_unowned_bufs_semcid;

    /** \brief hash chain mutex */
    AW_MUTEX_DECL(       mnt_bufhash_mutex);
    aw_mutex_id_t        mnt_bufhash_mutexid;

    /** \brief hash chains */
    struct bhashhdr      mnt_bufhash[AW_TXFS_BUF_BHSIZE];

    /** \brief free lists */
    struct bfreehdr      mnt_bufqueues[AW_TXFS_BUF_NQUEUES];

    /**
     * These items are for aw_txfs_vnode management, which is done in pretty
     * much the same fashion as buffer management, except that vnodes
     * do not have "priorities" to order the free list.  Also, vnodes
     * can have more than one active user, and can be "held" (by
     * buffer cache references) even when no one is using them.
     */
    /** \brief number of unowned vnodes */
    AW_SEMC_DECL(        mnt_unowned_vns_semc);
    aw_semc_id_t         mnt_unowned_vns_semc_id;

    /** \brief hash chain mutex */
    AW_MUTEX_DECL(       mnt_vhash_mutex);
    aw_mutex_id_t        mnt_vhash_mutex_id;

    /** \brief hash chains */
    struct vhashhdr      mnt_vhash[AW_TXFS_VN_VHSIZE];

    /** \brief held vnodes */
    AW_TAILQ_HEAD(, aw_txfs_vnode) mnt_vhold_list;

    /** \brief free vnodes */
    AW_TAILQ_HEAD(, aw_txfs_vnode) mnt_freevnodes;

    /**
     * These keep track of bio space allocated in mountCreate(),
     * for mountDelete() to release.
     */
    /** \brief the bios block io device request packet space */
    struct aw_blk_arp   *p_mnt_bios;

    /** \brief bio block io space for request packet */
    void                *p_mnt_biomem;
} aw_txfs_mount_t;
/******************************************************************************/
/** \brief vfs operations information structure */
struct aw_txfs_vfsops {
    const char *p_vfs_name;          /**< \brief name of this fs type */
    size_t      vfs_mount_data_size; /**< \brief size of private mount data */
    
    /** \brief size of private aw_txfs_vnode data */
    size_t      vfs_vnode_data_size;

    /** \brief start VFS */
    int  (*pfn_vfs_start)(aw_txfs_mount_t *, int);

    /** \brief unmount VFS */
    int  (*pfn_vfs_unmount)(aw_txfs_mount_t *, int);

    /** \brief create root directory in VFS */
    int  (*pfn_vfs_root)(aw_txfs_mount_t *, struct aw_txfs_vnode **);

    /** \brief getting VFS state */
    int  (*pfn_vfs_statvfs)(aw_txfs_mount_t *, struct aw_statvfs *);

    /** \brief get a aw_txfs_vnode based upon an inode number */
    int  (*pfn_vfs_vget)(aw_txfs_mount_t *, ino_t, struct aw_txfs_vnode **);

    /** \brief init VFS */
    void (*pfn_vfs_init)(void);

    /** \brief re-init VFS */
    void (*pfn_vfs_reinit)(void);

    /** \brief finalizes VFS */
    void (*pfn_vfs_done)(void);
};

/** \brief VFS start function calling */
#define AW_TXFS_VFS_START(mp, flags)    \
    ((mp)->p_mnt_op->pfn_vfs_start(mp, flags))

/** \brief VFS unmount function calling */
#define AW_TXFS_VFS_UNMOUNT(mp, force)  \
    ((mp)->p_mnt_op->pfn_vfs_unmount(mp, force))

/** \brief create root directory function calling */
#define AW_TXFS_VFS_ROOT(mp, vpp)       \
    ((mp)->p_mnt_op->pfn_vfs_root(mp, vpp))

/** \brief get VFS state function calling */
#define AW_TXFS_VFS_STATVFS(mp, sbp)    \
    ((mp)->p_mnt_op->pfn_vfs_statvfs(mp, sbp))

/** \brief get aw_txfs_vnode function calling */
#define AW_TXFS_VFS_VGET(mp, ino, vpp)  \
    ((mp)->p_mnt_op->pfn_vfs_vget(mp, ino, vpp))
/******************************************************************************/
/**
 * \brief mount point creation
 *
 * \param[in] p_vfs_ops       the VFS being instantiated
 * \param[in] p_bd            the connected block device
 * \param[in] files           how many files to allow open
 * \param[in] p_core_io_path  core I/O pathname(mount point)
 * \param[in] pp_mnt          pointer to mount-point for caller
 *
 * \return errno
 */
int aw_txfs_mount_create (const struct aw_txfs_vfsops *p_vfs_ops,
                          void                        *p_bd,
                          int                          files,
                          char                        *p_core_io_path,
                          aw_txfs_mount_t            **pp_mnt);


/**
 * \brief unmount and delete mount point.
 *
 * \param[in] p_mnt  mount point
 * \param[in] flags  Tells us to do a suspend or delete for core I/O
 *                   1 - suspend(no support now);
 *                   0 - delete;
 *
 * \note only support delete handle now.
 *
 * \return N/A
 */
void aw_txfs_mount_delete (aw_txfs_mount_t *p_mnt, int flags);


/**
 * \brief Mark a mount point as "ejecting", and unmount and delete mount point.
 *
 * \param[in] p_mnt  mount point
 * \param[in] flags  Tells us to do a suspend or delete for core I/O
 *                   1 - suspend(no support now);
 *                   0 - delete;
 *
 * \note only support delete handle now.
 *
 * \return N/A
 */
void aw_txfs_mount_eject (aw_txfs_mount_t *p_mnt, aw_bool_t flag);

/**
 * \brief Allocate buffers for a mount point.
 *
 * This happens after the mount point is created, because various file
 * systems do not know their block sizes until they read their superblocks
 * in their start functions.
 *
 * \param[in] p_mnt     mount point on which to alloc buffers
 * \param[in] buffers  the number of buffers you want
 * \param[in] buf_size   their size
 *
 * \return errno
 */
int aw_txfs_mount_buf_alloc (aw_txfs_mount_t *p_mnt,
                             int              buffers, 
                             size_t           buf_size);


/**
 * \brief Release the buffer cache associated with this mount point.
 *
 * \note you can then create a new one.
 *
 * \param[in] p_mnt  mount point
 *
 * \return N/A
 */
void aw_txfs_mount_buf_free (aw_txfs_mount_t *p_mnt);

/** \brief Mount flags. */
#define AW_TXFS_MNT_RDONLY   0x00000001  /**< \brief read only file system */

/** \brief Internal flags. */

/** \brief entry is in coreIO tables */
#define TXFS_MNTI_IN_COREIO  0x00000001  

/** \brief mount point is being ejected */
#define TXFS_MNTI_EJECTING   0x00000002  

#ifdef __cplusplus
}
#endif

#endif /* __AW_TXFS_MOUNT_H */

/* end of file */
