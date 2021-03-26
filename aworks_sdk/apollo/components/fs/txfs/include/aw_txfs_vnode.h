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
 * \brief vnode interface
 *
 * \internal
 * \par Modification History
 * - 1.00 17-10-23  mkr, first implementation.
 * \endinternal
 */

#ifndef __AW_TXFS_VNODE_H
#define __AW_TXFS_VNODE_H

/* from $NetBSD: vnode.h,v 1.126 2004/09/21 03:10:35 thorpej Exp $  */

/*
 * Copyright (c) 1989, 1993
 *  The Regents of the University of California.  All rights reserved.
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
 *  @(#)vnode.h 8.17 (Berkeley) 5/20/95
 */

#include "aw_sem.h"
#include "io/aw_dirent.h"
#include "aw_time.h"
#include "aw_txfs_buf.h"
#include "aw_txfs_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/** \brief declaring */

#ifdef notyet
struct  namecache;
#endif
struct  aw_txfs_componentname;
struct  aw_txfs_uio;
typedef struct aw_txfs_ucred aw_txfs_ucred_t;
typedef struct aw_txfs_vattr aw_txfs_vattr_t;
struct  aw_txfs_flock;
struct  aw_txfs_lockf;

typedef int64_t voff_t;

/******************************************************************************/

/*
 * The aw_txfs_vnode is the focus of all file activity.  There is a
 * unique aw_txfs_vnode allocated for each active file (plus one for
 * file-system-metadata).
 *
 * Unlike the BSDs, vnodes are allocated per mount, so vnodes never
 * need to move from one file system type to another.
 */

/*
 * Vnode hashing, per mount-point.  This hash function probably needs
 * tuning.  Note that the current AW_TXFS_VN_HASH definition depends on
 * AW_TXFS_VN_VHSIZE being a power of two.
 */
#define AW_TXFS_VN_VHSIZE    (32) /**< \brief aw_txfs_vnode hash size */

/** \brief hash function */
#define AW_TXFS_VN_HASH(ino) ((ino) & (AW_TXFS_VN_VHSIZE - 1))

/**
 * \brief Vnode types.
 * VNON means no type.  We may not need AW_TXFS_VBAD, and
 * the block, character, link, socket, and fifo types are named
 * only for BSD compatibility.
 */
typedef enum aw_txfs_vtype {
    AW_TXFS_VNON,  /**< \brief no type */
    AW_TXFS_VREG,  /**< \brief regular file */
    AW_TXFS_VDIR,  /**< \brief directory */
    AW_TXFS_VBLK,  /**< \brief block */
    AW_TXFS_VCHR,  /**< \brief character */
    AW_TXFS_VLNK,  /**< \brief symbol link */
    AW_TXFS_VSOCK, /**< \brief socket */
    AW_TXFS_VFIFO, /**< \brief fifo */
    AW_TXFS_VMETA, /**< \brief meta */
    AW_TXFS_VBAD   /**< \brief bad aw_txfs_vnode type */
} aw_txfs_vtype_t;

AW_SDLIST_HEAD(aw_txfs_buflists, aw_txfs_buf);

/**
 * Reading or writing any of these items requires holding the appropriate lock.
 * v_freelist is locked by the mount point's aw_txfs_vnode hash chain lock
 * v_flag, v_usecount, v_holdcnt and v_writecount are locked by the v_interlock
 * simple lock.
 *
 * Each underlying file system has its own private area for v_data.
 *
 * Unlike BSD vnodes, ours have an inode number attached at all times.
 * This is the same as the number that will be used in the pfn_vfs_vget operation
 * (see mount.h).  Vnodes can cache inode data by inode number.  For a
 * aw_txfs_vnode that does not correspond to an ordinary file in the file system,
 * use AW_TXFS_VNOINODE.
 *
 * Similarly, all vnodes always have a working aw_txfs_vnode lock, locked
 * with AW_TXFS_VN_EXLOCK (exclusive lock) or AW_TXFS_VN_SHLOCK (shared lock) and
 * unlocked with AW_TXFS_VN_UNLOCK.  (Upgrade/downgrade is not yet implemented
 * and AW_TXFS_VN_SHLOCK is currently an exclusive lock.)  There is no
 * AW_TXFS_VOP_LOCK nor AW_TXFS_VOP_UNLOCK as vnodes do not stack.
 */
struct aw_txfs_vnode {
    /** \brief binary semaphore */
    AW_SEMB_DECL( v_interlock);
    aw_semb_id_t  v_interlock_id;

    /** \brief mutex semaphore during vnode allocate and release */
    AW_MUTEX_DECL(v_alloclock);
    aw_mutex_id_t v_alloclock_id;

    /** \brief flags */
    int           v_flag;

    /** \brief number of pending writes */
    atomic_t      v_numoutput;

    /** \brief semb for write data */
    AW_SEMB_DECL( v_output_active);
    aw_semb_id_t  v_output_active_id;

    /**
     * \brief reference count of users
     *
     * 引用数，指示出该文件的活跃实例数目，打开一个文件时，引用数增1，关闭文件时
     * 减1，系统调用之间是保持不变
     */
    long          v_usecount;

    /** \brief reference count of writers */
    long          v_writecount;

    /** \brief page & buffer references */
    long          v_holdcnt;

    /** \brief inode number, or 0 */
    ino_t         v_inode;

    /** \brief ptr to vfs we are in */
    aw_txfs_mount_t                *p_v_mnt;

    /** \brief aw_txfs_vnode operations */
    const struct aw_txfs_vnode_ops *p_v_ops;

    /** \brief aw_txfs_vnode freelist */
    AW_TAILQ_ENTRY(aw_txfs_vnode)   v_freelist;

    /** \brief hash chain */
    AW_SDLIST_ENTRY(aw_txfs_vnode)  v_hash;

    /** \brief clean blocklist head */
    struct aw_txfs_buflists         v_cleanblkhd;

    /** \brief dirty blocklist head,vnode 包含的数据块链表 */
    struct aw_txfs_buflists         v_dirtyblkhd;

    /** \brief vnodes with dirty buffers */
    AW_SDLIST_ENTRY(aw_txfs_vnode)  v_synclist;
#ifdef notyet

    /**< \brief namecaches for children */
    AW_SDLIST_HEAD(, namecache)     v_dnclist; 
    
    /**< \brief namecaches for parent */
    AW_SDLIST_HEAD(, namecache)     v_nclist;  
#endif
    aw_txfs_vtype_t                 v_type;    /**< \brief aw_txfs_vnode type */
#ifdef notyet /* don't have read/writer locks yet */

    /**< \brief lock for this aw_txfs_vnode */
    struct lock                     v_lock;    
#else
    AW_MUTEX_DECL(v_lock);
    aw_mutex_id_t v_lock_id;

#endif
    struct aw_txfs_lockf           *p_v_lfhead; /**< \brief lockf() list head */
   
    AW_SDLIST_HEAD(, aw_txfs_lockf) v_lfbq; /**< \brief lcokf() blocked-locks */

    /**
     * \brief Per-file-system data.
     * This is actually variable length, allocated when you
     * instantiate a file system and its "struct mount" and so forth,
     * according to the size you give for the file system module.
     *
     * in TXFS, vnode data is relate with inode.
     */
    union {
        char       v_data[1]; /**< \brief aw_txfs_vnode data */
        /**< \brief these merely force v_data to be aligned */
        long       v_un_long;
        long long  v_un_llong;
        void      *p_v_un;
    } v_data_un;
};
/******************************************************************************/
/** \brief getting vnode data */
#define AW_TXFS_VTODATA(type, vp) ((type *)((vp)->v_data_un.v_data))

/** \brief inode number for "not a file" vnodes */
#define AW_TXFS_VNOINODE ((ino_t)0)


/**
 * \brief Vnode flags.
 *
 * Many of these (commented out) are not used in vxWorks.
 * We add AW_TXFS_VRUNNING to distinguish between "aw_txfs_vnode that is being
 * initialized" and "aw_txfs_vnode that has finished initializing".
 * We add AW_TXFS_VRENAME to mark a directory that is being renamed
 * (it cannot be held locked, but only one rename should be
 * allowed to proceed at a time).
 */
/** \brief root of its file system */
#define AW_TXFS_VROOT       (0x0001)
/*  AW_TXFS_VTEXT       0x0002  -- aw_txfs_vnode is a pure text prototype */

/** \brief aw_txfs_vnode is up and running */
#define AW_TXFS_VRUNNING    (0x0002)
/* VSYSTEM only used to skip vflush()ing quota files */
/*  AW_TXFS_VSYSTEM     0x0004  -- aw_txfs_vnode being used by kernel */
/* VISTTY used when reading dead vnodes */
/*  AW_TXFS_VISTTY      0x0008  -- aw_txfs_vnode represents a tty */
/*  AW_TXFS_VEXECMAP    0x0010  -- aw_txfs_vnode has PROT_EXEC mappings */
/*  AW_TXFS_VLOCKSWORK  0x0080  -- FS supports locking discipline */
/*  AW_TXFS_VXLOCK      0x0100  -- aw_txfs_vnode is locked to change underlying type */

/** \brief aw_txfs_vnode is being rename()d */
#define AW_TXFS_VRENAME     (0x0100)
/*  AW_TXFS_VXWANT      0x0200  -- process is waiting for aw_txfs_vnode */
/*  AW_TXFS_VBWAIT      0x0400  -- waiting for output to complete */
/*  AW_TXFS_VALIASED    0x0800  -- aw_txfs_vnode has an alias */
/*  AW_TXFS_VDIROP      0x1000  -- LFS: aw_txfs_vnode is involved in a directory op */
/*  AW_TXFS_VLAYER      0x2000  -- aw_txfs_vnode is on a layer filesystem */

/** \brief On syncer work-list */
#define AW_TXFS_VONWORKLST  (0x4000)
/*  AW_TXFS_VDIRTY      0x8000  -- aw_txfs_vnode possibly has dirty pages */

/**
 * \brief Vnode operations.
 *
 * BSD computes the set of operations at boot time; we do not need this
 * and just have this fixed set.
 *
 * For each op's vnodes, we include a specification of the locking
 * protocol for that operation.  The three columns are in, out, and error
 * respectively.  The "in" column defines the lock state on input, the
 * "out" column defines the state on successful return, and the "error"
 * column defines the state on error exit.
 *
 * The locking value can take the following values:
 *  L: locked
 *  U: unlocked
 *  -: not applicable, aw_txfs_vnode does not yet (or no longer) exists
 *  =: the same on input and output, may be either L or U
 *  X: locked if not nil.
 *
 * The lookup operation defies simple description, however.  Its lock
 * state depends on the flags and operations fields in the (cnp) structure.
 * Note especially that *vpp may equal dvp and both may be locked.
 *
 * There are three types of lookups: ".", ".." (AW_TXFS_ISDOTDOT), and other.
 * On successful lookup of ".", a reference is added to dvp, and it
 *  is returned in *vpp.
 * To look up AW_TXFS_ISDOTDOT, dvp is unlocked, the ".." node is locked, and
 *  then dvp is relocked iff AW_TXFS_LOCKPARENT is set and this is the last
 *  component name (AW_TXFS_ISLASTCN set).  This preserves the protocol of
 *  always locking nodes from the root ("/") downward and prevents
 *  deadlock.
 * Other lookups find the named node (creating the aw_txfs_vnode if needed) and
 *  return it, locked, in *vpp.
 * For non-"." lookups, if AW_TXFS_LOCKPARENT is not set or this was not the
 *  last component name, dvp is returned unlocked on a successful
 *  lookup.
 * On failure, *vpp is NULL, and *dvp is left locked or unlocked (as
 *  desired by the file system).
 *
 * All of this is relaxed inside actual file systems, which merely need
 * to return successful new vnodes locked in *vpp (handling "." by increasing
 * the reference count), or setting the AW_TXFS_PDIRUNLOCK flag if dvp has been
 * unlocked.  If the file system leaves dvp locked but the lookup operation
 * does not need dvp locked, the lookup code will unlock it; if the file
 * system has unlocked dvp for AW_TXFS_ISDOTDOT purposes, it can leave it unlocked
 * and just set AW_TXFS_PDIRUNLOCK.
 *
 * In other words: if you are implementing a file system, you must handle
 * "." and ".." specially (by returning with *vpp = dvp in the first case,
 * after increasing the reference count, and by unlocking dvp before going
 * up the tree in the second case).  You must also set AW_TXFS_PDIRUNLOCK if you
 * return with dvp unlocked.  Other than that, you should just leave dvp
 * locked.
 */
struct aw_txfs_vnode_ops {
    int (*pfn_vop_lookup)(
        struct aw_txfs_vnode          *dvp, /**< \brief L ? ? */
        struct aw_txfs_vnode         **vpp, /**< \brief - L - */
        struct aw_txfs_componentname  *cnp
    );

    int (*pfn_vop_create)(
        struct aw_txfs_vnode          *dvp, /**< \brief L L L */
        struct aw_txfs_vnode         **vpp, /**< \brief - L - */
        
        /**< \brief cnp flags: AW_TXFS_CREATE, AW_TXFS_LOCKPARENT */
        struct aw_txfs_componentname  *cnp, 
        aw_txfs_vattr_t *vap
    );

    int (*pfn_vop_open)(
        struct aw_txfs_vnode *vp,   /**< \brief L L L */
        int                   mode,
        aw_txfs_ucred_t      *cred
    );

    int (*pfn_vop_close)(
        struct aw_txfs_vnode *vp,   /**< \brief L L L */
        int                   fflag,
        aw_txfs_ucred_t      *cred
    );

    int (*pfn_vop_access)(
        struct aw_txfs_vnode *vp,   /**< \brief L L L */
        int                   mode,
        aw_txfs_ucred_t      *cred
    );

    int (*pfn_vop_getattr)(
        struct aw_txfs_vnode *vp,   /**< \brief = = = */
        aw_txfs_vattr_t         *vap,
        aw_txfs_ucred_t      *cred
    );

    int (*pfn_vop_setattr)(
        struct aw_txfs_vnode *vp,   /**< \brief L L L */
        aw_txfs_vattr_t         *vap,
        aw_txfs_ucred_t      *cred
    );

    int (*pfn_vop_read)(
        struct aw_txfs_vnode *vp,   /**< \brief L L L */
        struct aw_txfs_uio   *uio,
        int                   ioflag,
        aw_txfs_ucred_t      *cred
    );

    int (*pfn_vop_write)(
        struct aw_txfs_vnode *vp,   /**< \brief L L L */
        struct aw_txfs_uio   *uio,
        int                   ioflag,
        aw_txfs_ucred_t      *cred
    );

    int (*pfn_vop_ioctl)(
        struct aw_txfs_vnode *vp,   /**< \brief L L L */
        u_long                command,
        void                 *data,
        int                   fflag,
        aw_txfs_ucred_t      *cred
    );

    int (*pfn_vop_fcntl)(
        struct aw_txfs_vnode *vp,   /**< \brief L L L */
        u_int                 command,
        void                 *data,
        int                   fflag,
        aw_txfs_ucred_t      *cred
    );

    int (*pfn_vop_fsync)(
        struct aw_txfs_vnode *vp,   /**< \brief L L L */
        aw_txfs_ucred_t      *cred,
        int                   flags
    );

    int (*pfn_vop_seek)(
        struct aw_txfs_vnode *vp,   /**< \brief ? ? ? */
        voff_t                oldoff,
        voff_t                newoff,
        aw_txfs_ucred_t      *cred
    );

    int (*pfn_vop_remove)(
        struct aw_txfs_vnode         *dvp, /**< \brief L L L */
        struct aw_txfs_vnode         *vp,  /**< \brief L L L */
        
        /** \brief AW_TXFS_DELETE, AW_TXFS_LOCKPARENT | AW_TXFS_LOCKLEAF */
        struct aw_txfs_componentname *cnp  
    );

    int (*pfn_vop_link)(
        struct aw_txfs_vnode         *dvp, /**< \brief L L L */
        struct aw_txfs_vnode         *vp,  /**< \brief U U U */
        
        /** \brief AW_TXFS_CREATE, AW_TXFS_LOCKPARENT */
        struct aw_txfs_componentname *cnp  
    );

    int (*pfn_vop_rename)(
        struct aw_txfs_vnode         *fdvp, /**< \brief U U U */
        struct aw_txfs_vnode         *fvp,  /**< \brief U U U */
        
        /** \brief AW_TXFS_DELETE, AW_TXFS_WANTPARENT | AW_TXFS_SAVESTART */
        struct aw_txfs_componentname *fcnp, 
        struct aw_txfs_vnode         *tdvp, /**< \brief L U U */
        struct aw_txfs_vnode         *tvp,  /**< \brief X U U */
        
        /**
         * \brief AW_TXFS_RENAME, AW_TXFS_LOCKPARENT | AW_TXFS_LOCKLEAF | 
         * AW_TXFS_NOCACHE | AW_TXFS_SAVESTART 
         */
        struct aw_txfs_componentname *tcnp 
        /**
         * \brief XXX the pfn_vop_rename routines should REALLY NOT be depending 
         * on AW_TXFS_SAVESTART!
         */
    );

    int (*pfn_vop_mkdir)(
        struct aw_txfs_vnode          *dvp, /**< \brief L L L */
        struct aw_txfs_vnode         **vpp, /**< \brief - L - */
        
        /** \brief AW_TXFS_CREATE, AW_TXFS_LOCKPARENT */
        struct aw_txfs_componentname  *cnp, 
        aw_txfs_vattr_t               *vap
    );

    int (*pfn_vop_rmdir)(
        struct aw_txfs_vnode         *dvp, /**< \brief L L L */
        struct aw_txfs_vnode         *vp,  /**< \brief L L L */
        
        /** \brief AW_TXFS_DELETE, AW_TXFS_LOCKPARENT | AW_TXFS_LOCKLEAF */
        struct aw_txfs_componentname *cnp  
    );

    int (*pfn_vop_symlink)(
        struct aw_txfs_vnode          *dvp, /**< \brief L L L */
        struct aw_txfs_vnode         **vpp, /**< \brief - L - */
        
        /** \brief AW_TXFS_CREATE, AW_TXFS_LOCKPARENT */
        struct aw_txfs_componentname  *cnp, 
        aw_txfs_vattr_t               *vap,
        char                          *target
    );

    int (*pfn_vop_readdir)(
        struct aw_txfs_vnode *vp,     /**< \brief L L L */
        struct aw_dirent     *direntp,
        aw_txfs_ucred_t      *cred,
        int                  *eofflag,
        
        /** \brief really, FS-level seek offset */
        int                  *cookie  
    );

    int (*pfn_vop_readlink)(
        struct aw_txfs_vnode *vp,   /**< \brief L L L */
        struct aw_txfs_uio   *uio,
        aw_txfs_ucred_t      *cred
    );

    int (*pfn_vop_abortop)(
        struct aw_txfs_vnode         *dvp, /**< \brief = = = */
        
        /** \brief flags per op being aborted */
        struct aw_txfs_componentname *cnp 
    );

    int (*pfn_vop_activate)(
        struct aw_txfs_vnode *vp    /**< \brief L L L */
    );

    int (*pfn_vop_inactive)(
        struct aw_txfs_vnode *vp    /**< \brief L L L */
    );

#if 0
    int (*pfn_vop_lock)(
        struct aw_txfs_vnode *vp    /**< \brief U L U */
    );

    int (*pfn_vop_unlock)(
        struct aw_txfs_vnode *vp    /**< \brief L U L */
    );
#endif

    void (*pfn_vop_strategy)(
        struct aw_txfs_vnode *vp,   /**< \brief = = = */
        struct aw_txfs_buf   *bp
    );

    void (*pfn_vop_print)(
        struct aw_txfs_vnode *vp    /**< \brief = = = */
    );

    int (*pfn_vop_pathconf)(
        struct aw_txfs_vnode *vp,   /**< \brief L L L */
        int                   name,
        long                 *retval
    );

    int (*pfn_vop_advlock)(
        struct aw_txfs_vnode *vp,   /**< \brief U U U */
        void                 *id,
        int                   op,
        struct aw_txfs_flock *fl,
        int                   flags
    );

    int (*pfn_vop_truncate)(
        struct aw_txfs_vnode *vp,   /**< \brief L L L */
        voff_t                length,
        int                   flags,
        aw_txfs_ucred_t      *ucred
    );
};

/**
 * \brief Vnode attributes.
 * A field value of AW_TXFS_VNOVAL represents a field whose value
 * is unavailable (getattr) or which is not to be changed (setattr).
 */
typedef struct aw_txfs_vattr {
    aw_txfs_vtype_t va_type;      /**< \brief aw_txfs_vnode type (for create) */
    mode_t          va_mode;      /**< \brief file access mode and type */
    nlink_t         va_nlink;     /**< \brief number of references to file */
    uid_t           va_uid;       /**< \brief owner user id */
    gid_t           va_gid;       /**< \brief owner group id */
    long            va_fsid;      /**< \brief file system id (dev for now) */
    uint64_t        va_fileid;    /**< \brief file id */
    uint64_t        va_size;      /**< \brief file size in bytes */
    long            va_blocksize; /**< \brief blocksize preferred for i/o */
    aw_timespec_t   va_atime;     /**< \brief time of last access */
    aw_timespec_t   va_mtime;     /**< \brief time of last modification */
    aw_timespec_t   va_ctime;     /**< \brief time file changed */
    aw_timespec_t   va_birthtime; /**< \brief time file created */
    u_long          va_gen;       /**< \brief generation number of file */
    u_long          va_flags;     /**< \brief flags defined for file */
    
    /** \brief device the special file represents */
    dev_t           va_rdev;      
    uint64_t        va_bytes;    /**< \brief bytes of disk space held by file */
    uint64_t        va_filerev;  /**< \brief file modification number */
    u_int           va_vaflags;  /**< \brief operations flags, see below */
    long            va_spare;    /**< \brief remain quad aligned */
} aw_txfs_vattr_t;

/** \brief Flags for va_vaflags. */
#define AW_TXFS_VA_UTIMES_NULL    (0x01) /**< \brief utimes argument was NULL */
#define AW_TXFS_VA_EXCLUSIVE      (0x02) /**< \brief exclusive create request */

/** \brief Flags for ioflag (read/write aw_txfs_vnode ops). */
#define AW_TXFS_VNIO_UNIT         (0x01) /**< \brief do I/O as atomic unit */
#define AW_TXFS_VNIO_APPEND       (0x02) /**< \brief append write to end */
/** \brief sync I/O file integrity completion */
#define AW_TXFS_VNIO_SYNC         (0x04 | AW_TXFS_VNIO_DSYNC)

/** \brief underlying node already locked */
#define AW_TXFS_VNIO_NODELOCKED   (0x08) 

/** \brief FNDELAY flag set in file table */
#define AW_TXFS_VNIO_NDELAY       (0x10) 

/** \brief sync I/O data integrity completion */
#define AW_TXFS_VNIO_DSYNC        (0x20) 

/** \brief sync read operations at same level */
#define AW_TXFS_VNIO_RSYNC        (0x40) 

/** \brief use alternate i/o semantics */
#define AW_TXFS_VNIO_ALTSEMANTICS (0x80) 

/** \brief Modes for vaccess() (fifth argument). */
#define AW_TXFS_VREAD   0004 /**< \brief read permissions */
#define AW_TXFS_VWRITE  0002 /**< \brief write permissions */
#define AW_TXFS_VEXEC   0001 /**< \brief execute permissions */

/** \brief Token indicating no attribute value yet assigned. */
#define AW_TXFS_VNOVAL  (-1)

/**
 * \brief Flags to various aw_txfs_vnode functions.
 *  XXX DON'T KNOW IF WE NEED MOST OF THESE
 */
/*#define AW_TXFS_SKIPSYSTEM  0x0001      -- vflush: skip vnodes marked VSYSTEM */

/** \brief vflush: force file closeure */
#define AW_TXFS_FORCECLOSE     (0x0002) 

/** \brief vflush: only close writeable file */
#define AW_TXFS_WRITECLOSE     (0x0004) 

/** \brief vclean: close active files */
#define AW_TXFS_DOCLOSE        (0x0004) 

/** \brief aw_txfs_vinvalbuf: sync file first */
#define AW_TXFS_V_SAVE         (0x0001)

/** \brief Flags to various aw_txfs_vnode operations. */

/** \brief fsync: wait for completion */
#define AW_TXFS_FSYNC_WAIT     (0x0001) 

/** \brief fsync: hint: sync file data only */
#define AW_TXFS_FSYNC_DATAONLY (0x0002) 

/** \brief fsync: hint: aw_txfs_vnode is being reclaimed */
#define AW_TXFS_FSYNC_RECLAIM  (0x0004) 

/** \brief fsync: lazy sync (trickle) */
#define AW_TXFS_FSYNC_LAZY     (0x0008) 

/** \brief update: wait for completion */
#define AW_TXFS_UPDATE_WAIT    (0x0001) 

/** \brief update: hint for fs to wait or not */
#define AW_TXFS_UPDATE_DIROP   (0x0002) 
#define AW_TXFS_UPDATE_CLOSE   (0x0004) /**< \brief update: clean up on close */

/** \brief aw_txfs_vpurgebufs function flag parameter */
#define AW_TXFS_VPURGE_CLEAN   (1)      /**< \brief purge clean buffers */
#define AW_TXFS_VPURGE_DIRTY   (2)      /**< \brief purge dirty buffers */

/** \brief hold/holdrele should not appear outside vnodeSubr.c */
/* #define  HOLDRELE(vp)    holdrele(vp) */
/* #define  VHOLD(vp)   vhold(vp) */

/** \brief 只要目录的被赋值一次，引用计数值就增加一 */
#define AW_TXFS_VREF(vp)       aw_txfs_vref(vp)

/**
 * \brief Get shared or exclusive lock on aw_txfs_vnode.
 * Currently shared locks are actually exclusive.
 */
#define AW_TXFS_VN_SHLOCK(vp)  AW_MUTEX_LOCK((vp)->v_lock, AW_SEM_WAIT_FOREVER)
#define AW_TXFS_VN_EXLOCK(vp)  AW_MUTEX_LOCK((vp)->v_lock, AW_SEM_WAIT_FOREVER)
#define AW_TXFS_VN_UNLOCK(vp)  AW_MUTEX_UNLOCK((vp)->v_lock)

/**
 * These macros upgrade from a shared lock to an exclusive locks and downgrade
 * from an exclusive lock to a shared lock. Currently, as there's no difference
 * in lock types they do nothing.
 */
#define AW_TXFS_VN_UPLOCK(vp)
#define AW_TXFS_VN_DOWNLOCK(vp)

/**
 * \brief File system operations.
 */
/** \brief lookup */
#define AW_TXFS_VOP_LOOKUP(dvp, vpp, cnp) \
        ((dvp)->p_v_ops->pfn_vop_lookup(dvp, vpp, cnp))

/** \brief create */
#define AW_TXFS_VOP_CREATE(dvp, vpp, cnp, vap) \
        ((dvp)->p_v_ops->pfn_vop_create(dvp, vpp, cnp, vap))

/** \brief open */
#define AW_TXFS_VOP_OPEN(vp, mode, cred) \
        ((vp)->p_v_ops->pfn_vop_open(vp, mode, cred))

/** \brief close */
#define AW_TXFS_VOP_CLOSE(vp, fflag, cred) \
        ((vp)->p_v_ops->pfn_vop_close(vp, fflag, cred))

/** \brief access permissions check */
#define AW_TXFS_VOP_ACCESS(vp, mode, cred) \
        ((vp)->p_v_ops->pfn_vop_access(vp, mode, cred))

/** \brief get attribute */
#define AW_TXFS_VOP_GETATTR(vp, vap, cred) \
        ((vp)->p_v_ops->pfn_vop_getattr(vp, vap, cred))

/** \brief set attribute */
#define AW_TXFS_VOP_SETATTR(vp, vap, cred) \
        ((vp)->p_v_ops->pfn_vop_setattr(vp, vap, cred))

/** \brief read */
#define AW_TXFS_VOP_READ(vp, uio, ioflag, cred) \
        ((vp)->p_v_ops->pfn_vop_read(vp, uio, ioflag, cred))

/** \brief write */
#define AW_TXFS_VOP_WRITE(vp, uio, ioflag, cred) \
        ((vp)->p_v_ops->pfn_vop_write(vp, uio, ioflag, cred))

/** \brief ioctl */
#define AW_TXFS_VOP_IOCTL(vp, cmd, data, fflag, cred) \
        ((vp)->p_v_ops->pfn_vop_ioctl(vp, cmd, data, fflag, cred))

/** \brief fcntl */
#define AW_TXFS_VOP_FCNTL(vp, cmd, data, fflag, cred) \
        ((vp)->p_v_ops->pfn_vop_fcntl(vp, cmd, data, fflag, cred))

/** \brief fync */
#define AW_TXFS_VOP_FSYNC(vp, cred, flags) \
        ((vp)->p_v_ops->pfn_vop_fsync(vp, cred, flags))

/** \brief seek */
#define AW_TXFS_VOP_SEEK(vp, oldoff, newoff, cred) \
        ((vp)->p_v_ops->pfn_vop_seek(vp, oldoff, newoff, cred))

/** \brief remove */
#define AW_TXFS_VOP_REMOVE(dvp, vp, cnp) \
        ((dvp)->p_v_ops->pfn_vop_remove(dvp, vp, cnp))

/** \brief link */
#define AW_TXFS_VOP_LINK(dvp, vp, cnp) \
        ((dvp)->p_v_ops->pfn_vop_link(dvp, vp, cnp))

/** \brief rename */
#define AW_TXFS_VOP_RENAME(fdvp, fvp, fcnp, tdvp, tvp, tcnp) \
        ((fdvp)->p_v_ops->pfn_vop_rename(fdvp, fvp, fcnp, tdvp, tvp, tcnp))

/** \brief mkdir */
#define AW_TXFS_VOP_MKDIR(dvp, vpp, cnp, vap) \
        ((dvp)->p_v_ops->pfn_vop_mkdir(dvp, vpp, cnp, vap))

/** \brief rmdir */
#define AW_TXFS_VOP_RMDIR(dvp, vp, cnp) \
        ((dvp)->p_v_ops->pfn_vop_rmdir(dvp, vp, cnp))

/** \brief symlink */
#define AW_TXFS_VOP_SYMLINK(dvp, vpp, cnp, vap, target) \
        ((dvp)->p_v_ops->pfn_vop_symlink(dvp, vpp, cnp, vap, target))

/** \brief read dir */
#define AW_TXFS_VOP_READDIR(vp, direntp, cred, eofflag, cookie) \
        ((vp)->p_v_ops->pfn_vop_readdir(vp, direntp, cred, eofflag, cookie))

/** \brief read link */
#define AW_TXFS_VOP_READLINK(vp, uio, cred) \
        ((vp)->p_v_ops->pfn_vop_readlink(vp, uio, cred))

/** \brief path conf */
#define AW_TXFS_VOP_PATHCONF(vp, name, retval) \
        ((vp)->p_v_ops->pfn_vop_pathconf(vp, name, retval))

/** \brief adv lock */
#define AW_TXFS_VOP_ADVLOCK(vp, id, op, fl, flags) \
        ((vp)->p_v_ops->pfn_vop_advlock(vp, id, op, fl, flags))

/** \brief truncate */
#define AW_TXFS_VOP_TRUNCATE(vp, length, flags, ucred) \
        ((vp)->p_v_ops->pfn_vop_truncate(vp, length, flags, ucred))

/**
 * \brief Housekeeping and other internals.
 * \note: AW_TXFS_VOP_ACTIVATE is not in BSD.
 */
#define AW_TXFS_VOP_ABORTOP(vp, cnp)  ((vp)->p_v_ops->pfn_vop_abortop(vp, cnp))
#define AW_TXFS_VOP_ACTIVATE(vp)      ((vp)->p_v_ops->pfn_vop_activate(vp))
#define AW_TXFS_VOP_INACTIVE(vp)      ((vp)->p_v_ops->pfn_vop_inactive(vp))
#if 0
#define AW_TXFS_VOP_LOCK(vp)          ((vp)->p_v_ops->pfn_vop_lock(vp))
#define AW_TXFS_VOP_UNLOCK(vp)        ((vp)->p_v_ops->pfn_vop_unlock(vp))
#endif
#define AW_TXFS_VOP_STRATEGY(vp, bp)  ((vp)->p_v_ops->pfn_vop_strategy(vp, bp))
#define AW_TXFS_VOP_PRINT(vp)         ((vp)->p_v_ops->pfn_vop_print(vp))
/******************************************************************************/
/**
 * \brief Vnode ops that return ENOSYS are redirected here.
 */
int aw_txfs_enosys(void);

/**
 * \brief Vnode ops that return ENOTDIR (rmdir(file)) or
 * EISDIR (unlink(dir)) can be directed to these.
 */
int aw_txfs_vop_err_enotdir(void);

/**
 * \brief Vnode ops that return AW_EISDIR are redirected here.
 */
int aw_txfs_vop_err_eisdir(void);

int aw_txfs_vop_error_rename(struct aw_txfs_vnode *,
                             struct aw_txfs_vnode *,
                             struct aw_txfs_componentname *,
                             struct aw_txfs_vnode *,
                             struct aw_txfs_vnode *,
                             struct aw_txfs_componentname *);

#define aw_txfs_vop_error_lookup       \
    ((int (*)(struct aw_txfs_vnode *,  \
              struct aw_txfs_vnode **, \
              struct aw_txfs_componentname *))aw_txfs_enosys)

#define aw_txfs_vop_error_lookup_enotdir \
    ((int (*)(struct aw_txfs_vnode *,    \
              struct aw_txfs_vnode **,   \
              struct aw_txfs_componentname *))aw_txfs_vop_err_enotdir)

#define aw_txfs_vop_error_create              \
    ((int (*)(struct aw_txfs_vnode *,         \
              struct aw_txfs_vnode **,        \
              struct aw_txfs_componentname *, \
              aw_txfs_vattr_t *))aw_txfs_enosys)

#define aw_txfs_vop_error_open        \
    ((int (*)(struct aw_txfs_vnode *, \
              int,                    \
              aw_txfs_ucred_t *))aw_txfs_enosys)

#define aw_txfs_vop_error_close       \
    ((int (*)(struct aw_txfs_vnode *, \
              int, aw_txfs_ucred_t *))aw_txfs_enosys)

#define aw_txfs_vop_error_access      \
    ((int (*)(struct aw_txfs_vnode *, \
              int,                    \
              aw_txfs_ucred_t *))aw_txfs_enosys)

#define aw_txfs_vop_error_getattr     \
    ((int (*)(struct aw_txfs_vnode *, \
              aw_txfs_vattr_t *,      \
              aw_txfs_ucred_t *))aw_txfs_enosys)

#define aw_txfs_vop_error_setattr     \
    ((int (*)(struct aw_txfs_vnode *, \
              aw_txfs_vattr_t *,      \
              aw_txfs_ucred_t *))aw_txfs_enosys)

#define aw_txfs_vop_error_read        \
    ((int (*)(struct aw_txfs_vnode *, \
              struct aw_txfs_uio *,   \
              int,                    \
              aw_txfs_ucred_t *))aw_txfs_enosys)

#define aw_txfs_vop_error_write       \
    ((int (*)(struct aw_txfs_vnode *, \
              struct aw_txfs_uio *,   \
              int,                    \
              aw_txfs_ucred_t *))aw_txfs_enosys)

#define aw_txfs_vop_error_ioctl       \
    ((int (*)(struct aw_txfs_vnode *, \
              u_long,                 \
              void *,                 \
              int,                    \
              aw_txfs_ucred_t *))aw_txfs_enosys)

#define aw_txfs_vop_error_fcntl       \
    ((int (*)(struct aw_txfs_vnode *, \
              u_int,                  \
              void *,                 \
              int,                    \
              aw_txfs_ucred_t *))aw_txfs_enosys)

#define aw_txfs_vop_error_fsync       \
    ((int (*)(struct aw_txfs_vnode *, \
              aw_txfs_ucred_t *,      \
              int))aw_txfs_enosys)

#define aw_txfs_vop_error_seek        \
    ((int (*)(struct aw_txfs_vnode *, \
              voff_t,                 \
              voff_t,                 \
              aw_txfs_ucred_t *))aw_txfs_enosys)

#define aw_txfs_vop_error_remove      \
    ((int (*)(struct aw_txfs_vnode *, \
              struct aw_txfs_vnode *, \
              struct aw_txfs_componentname *))aw_txfs_enosys)

#define aw_txfs_vop_error_link        \
    ((int (*)(struct aw_txfs_vnode *, \
              struct aw_txfs_vnode *, \
              struct aw_txfs_componentname *))aw_txfs_enosys)

#define aw_txfs_vop_error_mkdir               \
    ((int (*)(struct aw_txfs_vnode *,         \
              struct aw_txfs_vnode **,        \
              struct aw_txfs_componentname *, \
              aw_txfs_vattr_t *))aw_txfs_enosys)

#define aw_txfs_vop_error_rmdir       \
    ((int (*)(struct aw_txfs_vnode *, \
              struct aw_txfs_vnode *, \
              struct aw_txfs_componentname *))aw_txfs_enosys)

#define aw_txfs_vop_error_rmdir_enotdir \
    ((int (*)(struct aw_txfs_vnode *,   \
              struct aw_txfs_vnode *,   \
              struct aw_txfs_componentname *))aw_txfs_vop_err_enotdir)

#define aw_txfs_vop_error_symlink             \
    ((int (*)(struct aw_txfs_vnode *,         \
              struct aw_txfs_vnode **,        \
              struct aw_txfs_componentname *, \
              aw_txfs_vattr_t *,              \
              char *))aw_txfs_enosys)

#define aw_txfs_vop_error_readdir     \
    ((int (*)(struct aw_txfs_vnode *, \
              struct aw_dirent *,     \
              aw_txfs_ucred_t *,      \
              int *,                  \
              int *))aw_txfs_enosys)

#define aw_txfs_vop_error_readdir_enotdir \
    ((int (*)(struct aw_txfs_vnode *,     \
              struct aw_dirent *,         \
              aw_txfs_ucred_t *,          \
              int *,                      \
              int *))aw_txfs_vop_err_enotdir)

#define aw_txfs_vop_error_readlink    \
    ((int (*)(struct aw_txfs_vnode *, \
              struct aw_txfs_uio *,   \
              aw_txfs_ucred_t *))aw_txfs_enosys)

#define aw_txfs_vop_error_abortop     \
    ((int (*)(struct aw_txfs_vnode *, \
              struct aw_txfs_componentname *))aw_txfs_enosys)

#define aw_txfs_vop_error_pathconf    \
    ((int (*)(struct aw_txfs_vnode *, \
              int,                    \
              long *))aw_txfs_enosys)

#define aw_txfs_vop_error_advlock     \
    ((int (*)(struct aw_txfs_vnode *, \
              void *,                 \
              int,                    \
              struct aw_txfs_flock *, \
              int))aw_txfs_enosys)

#define aw_txfs_vop_error_truncate    \
    ((int (*)(struct aw_txfs_vnode *, \
              voff_t,                 \
              int,                    \
              aw_txfs_ucred_t *))aw_txfs_enosys)

/**
 * \brief Create vnodes for given mount point given the number of vnodes
 *        desired and the information about the file system (such as the
 *        size of each aw_txfs_vnode data field).
 *
 * \param[in] p_mnt    mount point
 * \param[in] nvnodes  the number of vnodes
 *
 * \return  an errno, AW_OK for success.
 */
int aw_txfs_vn_makevnodes(aw_txfs_mount_t *p_mnt, int nvnodes);

/**
 * \brief Release all the resources held by the aw_txfs_vnode list
 *        for a given mount point.
 *
 * \param[in] p_mnt  mount point
 *
 * IMPORTANT: We assume that all the vnodes are on the free list!
 * (i.e. all have a zero ref and hold count, and have been vput())
 *
 * \return N/A
 */
void aw_txfs_vn_deletevnodes(aw_txfs_mount_t *p_mnt);

/**
 * \brief Set aw_txfs_vnode attributes to AW_TXFS_VNOVAL.
 *
 * \param[in] p_va  aw_txfs_vnode attribute pointer
 *
 * \return N/A
 */
void aw_txfs_vattr_null(aw_txfs_vattr_t *p_va);

/**
 * \brief Associate a buffer with a aw_txfs_vnode.
 *
 * \param[in] p_vnode  new aw_txfs_vnode for buffer
 * \param[in] p_buf    the buffer being added
 *
 * Called by buffer code when a buffer that was previously holding
 * nothing at all, or was just brelvp()'d from its previous cache duty,
 * is being put on a new aw_txfs_vnode.
 *
 * The buffer code is holding its hash-chain lock.
 */
void aw_txfs_bgetvp(struct aw_txfs_vnode *p_vnode, struct aw_txfs_buf *p_buf);

/*
 * \brief Dissociate a buffer from a aw_txfs_vnode.
 *
 * \param[in] p_buf  the buffer being released
 *
 * The buffer code is holding its hash-chain lock.
 */
void aw_txfs_brelvp(struct aw_txfs_buf *p_buf);

/*
 * \brief Assign a buffer to the clean or dirty block list on its aw_txfs_vnode,
 *        based on the buffer's state. (Note that I have removed the "newvp"
 *        argument from NetBSD -- it has to already be in bp->b_vp!)
 *
 * \param[in] p_buf  the buffer being moved
 */
void aw_txfs_vreassign_buf(struct aw_txfs_buf *p_buf);

/**
 * \brief Push out delayed I/O for a file, and optionally wait for it to complete.
 * Should be called with the underlying object locked (see internal comments).
 *
 * \param[in] p_vnode  vnode pointer
 * \param[in] sync     flush flag,
 *                     1 - sync
 *                     0 - async
 *
 * If asked to write synchronously, returns the error from
 * the last failing bwrite() call, or OK if they all worked.
 */
int aw_txfs_vflushbuf (struct aw_txfs_vnode *p_vnode, aw_bool_t sync);

/**
 * \brief Flush out and invalidate all buffers associated with a aw_txfs_vnode.
 * Called with the underlying object locked.
 *
 * \param[in] p_vnode  vnode pointer
 * \param[in] flags    using AW_TXFS_V_SAVE flag or no using
 * \param[in] p_cred   credient
 *
 */
int aw_txfs_vinvalbuf (struct aw_txfs_vnode *p_vnode,
                       int                   flags,
                       aw_txfs_ucred_t      *p_cred);

/**
 * \brief Remove specified buffers from the given aw_txfs_vnode.
 * Note that they are removed even if they are dirty. If you want pending
 * writes performed, you must do this first (or in the called
 * function, but that is probably a bad strategy).
 *
 * We decide whether the buffer is to be purged based on the result
 * of the called function.
 *
 * If the function pointer is given as NULL, all buffers are removed.
 *
 * Note that if you do new I/O on the aw_txfs_vnode while this function is
 * running, new buffers could be added to the aw_txfs_vnode, and not seen
 * by the validator.  Hence it is best to lock the underlying object
 * before calling vpurgebufs, preventing other tasks from starting I/O,
 * and then not do any I/O on it yourself.
 *
 * \param[in] p_vnode      vnode pointer
 * \param[in] flags        \see \a AW_TXFS_VPURGE_CLEAN or
 *                         \a AW_TXFS_VPURGE_DIRTY
 * \param[in] pfn_keepbuf  a callback function pointer
 * \param[in] p_keeparg    callback function parameter
 *
 * \return N/A
 */
void aw_txfs_vpurgebufs (struct aw_txfs_vnode *p_vnode,
                         int                   flags,
                         aw_bool_t (*pfn_keepbuf)(struct aw_txfs_vnode *,
                                                  struct aw_txfs_buf   *,
                                                  void                 *),
                         void                 *p_keeparg);

/**
 * \brief Called by the buffer code when a dirty block is queued to
 * the underlying device.
 * We must track the number of such blocks, and for vflushbuf(),
 * make sure there is a semaphore it can use to wait for numoutput to go to zero.
 *
 * \param[in] p_vnode  vnode pointer
 *
 * \return N/A
 */
void aw_txfs_vn_incr_numoutput(struct aw_txfs_vnode *p_vnode);

/**
 * \brief Called (typically from driver tasks, but occasionally from the
 * original task by synchronous drivers like the XBD RAMdisk) by the buffer
 * code when a write has finished (whether or not it worked).  If the output
 * count goes to zero, release the output-active semaphore.
 *
 * \param[in] p_vnode  vnode pointer
 *
 * \return N/A
 */
void aw_txfs_vn_decr_numoutput(struct aw_txfs_vnode *p_vnode);

/**
 * \brief get a new aw_txfs_vnode from the free list.
 * Note that this aw_txfs_vnode is NOT associated with an inode!
 * To get a "brand new" inode (e.g., when creating a new file),
 * you must still use vgetino().
 *
 * \param[in] p_mnt         pointer to mount
 * \param[in] p_vops        vnode operations
 * \param[in out] pp_vnode  the result vnode
 *
 * \return errno or AW_OK if success
 */
int aw_txfs_getnewvnode (aw_txfs_mount_t           *p_mnt,
                         struct aw_txfs_vnode_ops  *p_vops,
                         struct aw_txfs_vnode     **pp_vnode);

/**
 * \brief Get aw_txfs_vnode from or into mount-point cache, by inode number.
 * Note that only vnodes that have inode numbers are in this table.  The
 * syncer aw_txfs_vnode (and any other "special" vnodes, if the file system
 * has them) have vp->v_inode == AW_TXFS_VNOINODE.  Completely free vnodes
 * (those not caching inodes) are likewise not in the table.
 *
 * Because the aw_txfs_vnode may be in the cache and yet not free (may have a
 * nonzero use and/or hold count), there must be a "spare" aw_txfs_vnode.  We
 * need it ready to go in case we need to take a free aw_txfs_vnode and put
 * it into the hash table to make sure we do not create two vnodes for
 * the same inode.
 *
 * \param[in] p_mnt         pointer to mount
 * \param[in] inode         inode number
 * \param[in] p_ops         vnode operations
 * \param[in out] pp_vnode  the result vnode
 *
 * \return AW_OK or errno
 */
int aw_txfs_vgetino (aw_txfs_mount_t          *p_mnt,
                     ino_t                     inode,
               const struct aw_txfs_vnode_ops *p_ops,
                     struct aw_txfs_vnode    **pp_vnode);

/**
 * \brief Vnode reference (increase use count).
 *
 * \param[in] p_vnode  pointer to aw_txfs_vnode
 *
 * \return N/A
 */
void aw_txfs_vref(struct aw_txfs_vnode *p_vnode);

/**
 * \brief vput(): release locked aw_txfs_vnode.
 *
 * \note the aw_txfs_vnode was locked
 *
 * \return N/A
 */
void aw_txfs_vput(struct aw_txfs_vnode *p_vnode);

/**
 * \brief Vnode release
 *
 * \note with aw_txfs_vnode not locked.
 *
 * \param[in] vp  pointer to aw_txfs_vnode
 *
 * \return N/A
 */
void aw_txfs_vrele(struct aw_txfs_vnode *p_vnode);

/**
 * \brief Check permissions on file access.
 *
 * \param[in] type            vnode type,
 *                            (normally just AW_TXFS_VREG or AW_TXFS_VDIR)
 * \param[in] file_mode       file's mode (permission bits) expressed in terms
 *                            of S_I{R,W,X}{USR,GRP,OTH} bits as found
 *                            in sys/stat.h.
 * \param[in] uid             user id
 * \param[in] gid             group id
 * \param[in] access_request  one of VREAD, AW_TXFS_VWRITE, or AW_TXFS_VEXEC
 * \param[in] p_cred          the user making the request
 *
 * \return AW_OK or -AW_EACCES
 */
int aw_txfs_vaccess (aw_txfs_vtype_t  type,
                     mode_t           file_mode,
                     uid_t            uid,
                     gid_t            gid,
                     int              access_request,
                     aw_txfs_ucred_t *p_cred);

void aw_txfs_vprint (const char *label, struct aw_txfs_vnode *vp);


/**
 * aw_txfs_vn_start_write and aw_txfs_vn_finished_write are intended to allow
 * mount points to be "downgraded" from r/w to r/o: if the mount
 * point is waiting for a downgrade, the "start" calls will pause
 * or return an error (depending on supplied flags).
 *
 * We do not implement this yet so we just define this away to 0
 * for now.
 */
#define aw_txfs_vn_start_write(vp, mp, flags) (0)
#define aw_txfs_vn_finished_write(mp, flags)

/**
 * This perhaps belongs elsewhere (BSD has a separate ucred.h) but
 * will serve for now.  Note that we only have one gid, rather than
 * a group-array and number-of-groups - this may eventually change
 * (for NFS)...
 */
typedef struct aw_txfs_ucred {
    uid_t   cr_uid; /* uid */
    gid_t   cr_gid; /* gid */
} aw_txfs_ucred_t;

#ifdef __cplusplus
}
#endif

#endif /* __AW_TXFS_VNODE_H */

/* end of file */
