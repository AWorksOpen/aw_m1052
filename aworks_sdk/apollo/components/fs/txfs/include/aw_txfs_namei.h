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
 * \brief aw_txfs_vnode/AFF lookup interface
 *
 * \internal
 * \par Modification History
 * - 1.00 17-10-23  mkr, first implementation.
 * \endinternal
 */

/*  $NetBSD: namei.h,v 1.37 2004/06/27 08:50:44 yamt Exp $  */
/*  BSDI $Id: namei.h,v 2.5 2001/11/13 22:43:19 fmayhar Exp $   */

/*
 * Copyright (c) 1985, 1989, 1991, 1993
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
 *  @(#)namei.h 8.5 (Berkeley) 8/20/94
 */

#ifndef __AW_TXFS_NAMEI_H
#define __AW_TXFS_NAMEI_H

#include "aw_txfs_vnode.h"

#ifdef __cplusplus
extern "C" {
#endif

/* XXX randomly using -201 as a error code */
#define EJUSTRETURN (-201)

/**
 * \brief Lookup parameters.
 *
 * this structure describes the subset of information from the
 * aw_txfs_nameidata structure that is passed through the VOP interface.
 */
struct aw_txfs_componentname {
    /**
     * \brief Arguments to lookup.
     * operation, namei operations, for example AW_TXFS_CREATE
     */
    int              cn_nameiop;

    /** \brief flags, namei operational modifiers */
    unsigned long    cn_flags;

    /** \brief credentials */
    aw_txfs_ucred_t *p_cn_cred;

    /**
     * \brief Shared between lookup and commit routines.
     * pathname buffer, totally pathname
     */
    char            *p_cn_pnbuf;

    /** \brief pointer to looked up name */
    char            *p_cn_nameptr;

    /**
     * \brief length of looked up component.
     * (one level path length,no included '/')
     */
    long             cn_namelen;

    /** \brief hash value of looked up name */
    unsigned long    cn_hash;

    /** \brief chars to consume in lookup() */
    long             cn_consume;
};

/** \brief Encapsulation of namei parameters. */
struct aw_txfs_nameidata {
    /**
     * \brief Arguments to namei/lookup.
     *
     * pathname pointer, totally pathname
     */
    char            *p_ni_path;

    /** \brief mount point (from core I/O) */
    aw_txfs_mount_t *p_ni_mnt;

    /**
     * \brief Arguments to lookup.
     *
     * Note that unlike BSD, we always have "absolute" path names
     * and the "rootdir" is based on the mount point.
     */
    struct  aw_txfs_vnode *p_ni_startdir; /**< \brief starting directory */
    struct  aw_txfs_vnode *p_ni_rootdir;  /**< \brief logical root directory */

    /** \brief Results: returned from/manipulated by lookup */
    
    /** \brief lookup destination file aw_txfs_vnode */
    struct  aw_txfs_vnode *p_ni_vnode;  
    
    /** \brief lookup destination file upper dir aw_txfs_vnode */
    struct  aw_txfs_vnode *p_ni_dvnode; 

    /** \brief Shared between namei and lookup/commit routines. */
    size_t         ni_pathlen;  /**< \brief remaining chars(length) in path */
    char          *p_ni_next;   /**< \brief next location in path */

    /** \brief Lookup parameters. */
    struct aw_txfs_componentname ni_cnd;
};
/******************************************************************************/
/** \brief namei operations */
#define AW_TXFS_LOOKUP      (0)   /**< \brief perform name lookup only */
#define AW_TXFS_CREATE      (1)   /**< \brief setup for file creation */
#define AW_TXFS_DELETE      (2)   /**< \brief setup for file deletion */
#define AW_TXFS_RENAME      (3)   /**< \brief setup for file renaming */

/** \brief namei operational modifiers, stored in ni_cnd.cn_flags */
#define AW_TXFS_LOCKLEAF    (0x0004) /**< \brief lock inode on return */

/** \brief want parent aw_txfs_vnode returned locked */
#define AW_TXFS_LOCKPARENT  (0x0008) 

/** \brief want parent aw_txfs_vnode returned unlocked */
#define AW_TXFS_WANTPARENT  (0x0010) 

/** \brief name must not be left in cache */
#define AW_TXFS_NOCACHE     (0x0020) 
#define AW_TXFS_FOLLOW      (0x0040) /**< \brief follow symbolic links */
#define AW_TXFS_NOSYMLINK   (0x0080) /**< \brief symbolic links are an error */

/** \brief do not follow symbolic links (pseudo) */
#define AW_TXFS_NOFOLLOW    (0x0000) 

/** \brief mask of operational modifiers */
#define AW_TXFS_MODMASK     (0x00fc) 

/**
 * \brief Namei parameter descriptors.
 *
 * AW_TXFS_SAVENAME may be set by AW_TXFS_VOP_LOOKUP().
 * In vxWorks, we never copy the name to a malloc()ed buffer, so
 * the flag has no real meaning -- when we are called from Core I/O we
 * already have the name in a buffer, which we just use.
 *
 * AW_TXFS_SAVESTART is set only by the callers of namei.
 * It implies AW_TXFS_SAVENAME plus the addition of saving the parent
 * directory that contains the name in p_ni_startdir.  This allows
 * repeated calls to lookup for the name being sought.
 * The caller is responsible for vrele'ing p_ni_startdir.
 *
 * AW_TXFS_NOCROSSMOUNT is for NFS and is not used in AWorks anyway, as we
 * never cross mount points in namei().
 */

/** \brief do not cross mount points */
#define AW_TXFS_NOCROSSMOUNT    (0x0000100)

/** \brief lookup with read-only semantics */
#define AW_TXFS_RDONLY          (0x0000200)
/*  AW_TXFS_HASBUF      (0x0000400)   -- we malloc'ed a pathname buffer */

/** \brief save pathname buffer [N/A] */
#define AW_TXFS_SAVENAME        (0x0000800)

/** \brief save starting directory */
#define AW_TXFS_SAVESTART       (0x0001000)

/** \brief current component name is .. */
#define AW_TXFS_ISDOTDOT        (0x0002000)

/** \brief entry is to be added to name cache */
#define AW_TXFS_MAKEENTRY       (0x0004000)

/** \brief this is last component of pathname */
#define AW_TXFS_ISLASTCN        (0x0008000)

/** \brief symlink needs interpretation */
#define AW_TXFS_ISSYMLINK       (0x0010000)
/*  AW_TXFS_ISWHITEOUT  (0x0020000)   -- found whiteout */
/*  AW_TXFS_DOWHITEOUT  (0x0040000)   -- do whiteouts */
/*  AW_TXFS_REQUIREDIR  (0x0080000)   -- must be a directory */

/** \brief pfn_vop_lookup unlocked parent dir */
#define AW_TXFS_PDIRUNLOCK      (0x0100000)
/*  AW_TXFS_CREATEDIR   (0x0200000)   -- creating entry is a directory */

/** \brief Initializtion of a aw_txfs_nameidata structure. */
#define AW_TXFS_NDINIT(ndp, op, flags, mp, namep, cred) \
do { \
    (ndp)->ni_cnd.cn_nameiop = op;    \
    (ndp)->ni_cnd.cn_flags   = flags; \
    (ndp)->p_ni_mnt          = mp;    \
    (ndp)->p_ni_path         = namep; \
    (ndp)->ni_cnd.p_cn_cred  = cred;  \
} while (0)

#ifdef notyet
/*
 * \brief This structure describes the elements in the cache of recent
 * names looked up by namei.  Names longer than NCHNAMLEN are not
 * stored in the cache.
 */
#define NCHNAMLEN   31  /* maximum name segment length we bother with */

struct  namecache {
    AW_SDLIST_ENTRY(namecache) nc_hash;   /**< \brief hash chain */
    AW_TAILQ_ENTRY(namecache)  nc_lru;    /**< \brief LRU chain */
    AW_SDLIST_ENTRY(namecache) nc_vhash;  /**< \brief directory hash chain */
    AW_SDLIST_ENTRY(namecache) nc_dvlist;
    
    /** \brief aw_txfs_vnode of parent of name */
    struct aw_txfs_vnode              *nc_dvp;    
    AW_SDLIST_ENTRY(namecache) nc_vlist;
    
    /** \brief aw_txfs_vnode the name refers to */
    struct aw_txfs_vnode *nc_vp;          
    uchar         nc_len;                 /**< \brief length of name */
    char          nc_name[NCHNAMLEN];     /**< \brief segment name */
};

/*
 * Stats on usefulness of namei caches.
 */
struct  nchstats {
    unsigned long ncs_goodhits;  /**< \brief hits that we can really use */
    unsigned long ncs_neghits;   /**< \brief negative hits that we can use */
    unsigned long ncs_badhits;   /**< \brief hits we must drop */
    unsigned long ncs_falsehits; /**< \brief hits with id mismatch */
    unsigned long ncs_miss;      /**< \brief misses */
    unsigned long ncs_long;      /**< \brief long names that ignore cache */
    unsigned long ncs_pass2;     /**< \brief names found with passes == 2 */
    unsigned long ncs_2passes;   /**< \brief number of times we attempt it */
};
#endif /* notyet */

/******************************************************************************/
/**
 * \brief Pathname-related functions.
 *
 * Currently, namei() is not exported at all (is local to vnodeAff.c);
 * if we export it we should declare it here, but perhaps also change
 * its name.
 */

/**
 * aw_txfs_vn_check_path() is a reimplementation of the BSD ufs_checkpath()
 * modified to be generic across file systems.  A file system can
 * use it to make sure that a rename() operation on a directory is
 * OK, i.e., will not orphan the directory.
 *
 * \param[in]     fvp   dir being renamed
 * \param[in out] tdvp  new target directory
 * \param[in]     cnp   new name in target dir (for aw_txfs_ucred_t)
 *
 * 当为一个目录更名时， newname 不能包含 oldname 作为其路径前缀。例如，不能将
 * /usr/foo 更名为 /usr/foo/testdir ，因为老名字(/usr/foo)是新名字的路径前缀，
 * 因而不能将其删除。
 *
 * Note that fvp is not locked at entry, while tdvp is.
 */
int aw_txfs_vn_check_path (struct aw_txfs_vnode         *p_fv,
                           struct aw_txfs_vnode         *p_tdv,
                           struct aw_txfs_componentname *p_cn);

/**
 * \brief (re)lookup final path name component.
 * This is a highly simplified version of lookup that never crosses
 * file systems or mount points.
 * Used by any aw_txfs_vnode op that needs it, which currently is just rename.
 *
 * \param[in] dvp       directory aw_txfs_vnode pointer
 * \param[in out] vpp   the lookup result of aw_txfs_vnode
 * \param[in] cnp       the lookup component name
 * \param[in] newflags  lookup flags
 *
 * This is the same as the BSD relookup() except for the flags argument.
 * We clear out (AW_TXFS_MODMASK|AW_TXFS_SAVESTART) and set whatever
 * flags are passed in (which makes the interface slightly cleaner).
 */
int aw_txfs_vn_relookup (struct aw_txfs_vnode          *p_dvnode,
                         struct aw_txfs_vnode         **pp_vnode,
                         struct aw_txfs_componentname  *p_cn,
                         int                            newflags);

#ifdef __cplusplus
}
#endif

#endif /* __AW_TXFS_NAMEI_H */

/* end of file */
