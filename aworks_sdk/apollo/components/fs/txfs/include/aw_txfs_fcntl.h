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
 * \brief basic control types header.
 *
 * \internal
 * \par Modification History
 * - 1.00 17-10-23  mkr, first implementation.
 * \endinternal
 */

#ifndef __AW_TXFS_FCNTL_H
#define __AW_TXFS_FCNTL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief file flags
 */
 
 /** \brief from sys/file.h, kernel use only */
#define AW_TXFS_FOPEN      (-1)    
#define AW_TXFS_FREAD      0x0001  /**< \brief read enabled */
#define AW_TXFS_FWRITE     0x0002  /**< \brief write enabled */
#define AW_TXFS_FNDELAY    0x0004  /**< \brief non blocking I/O (4.2 style) */

/** \brief append (writes guaranteed at the end) */
#define AW_TXFS_FAPPEND    0x0008  
#define AW_TXFS_FMARK      0x0010  /**< \brief internal; mark during gc() */

/** \brief internal; defer for next gc pass */
#define AW_TXFS_FDEFER     0x0020  
#define AW_TXFS_FASYNC     0x0040  /**< \brief signal pgrp when data ready */

/** \brief BSD flock() shared lock present */
#define AW_TXFS_FSHLOCK    0x0080  

/** \brief BSD flock() exclusive lock present */
#define AW_TXFS_FEXLOCK    0x0100  
#define AW_TXFS_FCREAT     0x0200  /**< \brief open with file create */
#define AW_TXFS_FTRUNC     0x0400  /**< \brief open with truncation */
#define AW_TXFS_FEXCL      0x0800  /**< \brief error on open if file exists */
#define AW_TXFS_FNBIO      0x1000  /**< \brief non blocking I/O (sys5 style) */
#define AW_TXFS_FSYNC      0x2000  /**< \brief do all writes synchronously */
#define AW_TXFS_FNONBLOCK  0x4000  /**< \brief non blocking I/O (POSIX style) */

/** \brief don't assign a ctty on this open */
#define AW_TXFS_FNOCTTY    0x8000  

/** \brief file data only integrity while writing */
#define AW_TXFS_FDSYNC     0x10000 

/** \brief sync read operations at same level by */
#define AW_TXFS_FRSYNC     0x20000 
                          /**< \brief AW_TXFS_FSYNC and AW_TXFS_FDSYNC flags */
                                   
/** \brief special open mode for vnode ioctl's */
#define AW_TXFS_FCNTRL     0x40000 

/** \brief special open mode to not update access */
#define AW_TXFS_FNOATIME   0x80000 
                                   /**< \brief time on close */
                                   
/** \brief special flag for FD_CLOEXEC */
#define AW_TXFS_FFD_CLOEXEC   0x100000 
#define AW_TXFS_FTEXT         0x200000 /**< \brief text translation mode */

#if !defined(_POSIX_C_SOURCE) && !defined(_XOPEN_SOURCE) && \
    !defined(_POSIX_AEP_RT_CONTROLLER_C_SOURCE)

#define DEFFILEMODE     0666

/** \brief Flags that work for fcntl(fd, F_SETFL, FXXXX) */
#define FAPPEND     AW_TXFS_FAPPEND
#define FSYNC       AW_TXFS_FSYNC
#define FDSYNC      AW_TXFS_FDSYNC
#define FRSYNC      AW_TXFS_FRSYNC
#define FASYNC      AW_TXFS_FASYNC
#define FNBIO       AW_TXFS_FNBIO
#define FNONBIO     AW_TXFS_FNONBLOCK
#define FNDELAY     AW_TXFS_FNDELAY

/*
 * \brief Flags that are disallowed for fcntl's (FCNTLCANT);
 * used for opens, internal state, or locking.
 */
#define FREAD       AW_TXFS_FREAD
#define FWRITE      AW_TXFS_FWRITE
#define FMARK       AW_TXFS_FMARK
#define FDEFER      AW_TXFS_FDEFER
#define FSHLOCK     AW_TXFS_FSHLOCK
#define FEXLOCK     AW_TXFS_FEXLOCK

/*
 * \brief The rest of the flags, used only for opens
 */
#define FOPEN       AW_TXFS_FOPEN
#define FCREAT      AW_TXFS_FCREAT
#define FTRUNC      AW_TXFS_FTRUNC
#define FEXCL       AW_TXFS_FEXCL
#define FNOCTTY     AW_TXFS_FNOCTTY

#endif  /* _POSIX_xxx_SOURCE */

/** \brief XXX close on exec request; must match UF_EXCLOSE in user.h */
#define FD_CLOEXEC  1   /**< \brief posix */

/** \brief fcntl(2) requests */
#define F_DUPFD     0   /**< \brief Duplicate fildes */
#define F_GETFD     1   /**< \brief Get fildes flags (close on exec) */
#define F_SETFD     2   /**< \brief Set fildes flags (close on exec) */
#define F_GETFL     3   /**< \brief Get file flags */
#define F_SETFL     4   /**< \brief Set file flags */
#define F_GETOWN    5   /**< \brief Get owner - for ASYNC */
#define F_SETOWN    6   /**< \brief Set owner - for ASYNC */
#define F_GETLK     7   /**< \brief Get record-locking information */
#define F_SETLK     8   /**< \brief Set or Clear a record-lock (Non-Blocking) */
#define F_SETLKW    9   /**< \brief Set or Clear a record-lock (Blocking) */
#if !defined(_POSIX_C_SOURCE) && !defined(_XOPEN_SOURCE) && \
    !defined(_POSIX_AEP_RT_CONTROLLER_C_SOURCE)
#define F_RGETLK   10  /**< \brief Test a remote lock to see if it is blocked */
#define F_RSETLK    11  /**< \brief Set or unlock a remote lock */
#define F_CNVT      12  /**< \brief Convert a fhandle to an open fd */
#define F_RSETLKW   13  /**< \brief Set or Clear remote record-lock(Blocking) */
#endif  /* _POSIX_xxx_SOURCE */

/** \brief fcntl(2) flags (l_type field of flock structure) */
#define F_RDLCK     1   /**< \brief read lock */
#define F_WRLCK     2   /**< \brief write lock */
#define F_UNLCK     3   /**< \brief remove lock(s) */
#if !defined(_POSIX_C_SOURCE) && !defined(_XOPEN_SOURCE) && \
    !defined(_POSIX_AEP_RT_CONTROLLER_C_SOURCE)
#define F_UNLKSYS   4   /**< \brief remove remote locks for a given system */
#endif  /* _POSIX_xxx_SOURCE */

#ifndef _ASMLANGUAGE

/* 
 * \brief file segment locking set data type - information passed to system 
 *        by user
 * NB: l_start and l_len are off_t, but we do not know if off_t is defined 
 */
struct aw_txfs_flock {
    short     l_type;   /**< \brief F_RDLCK, F_WRLCK, or F_UNLCK */
    short     l_whence; /**< \brief flag to choose starting offset */
    long long l_start;  /**< \brief relative offset, in bytes */
    long long l_len;    /**< \brief length, in bytes; 0 means lock to EOF */
    int       l_pid;    /**< \brief returned with F_GETLK */
};

#if !defined(_POSIX_C_SOURCE) && !defined(_XOPEN_SOURCE) && \
    !defined(_POSIX_AEP_RT_CONTROLLER_C_SOURCE)
/** \brief extended file segment locking set data type */
struct eflock {
    short     l_type;   /**< \brief F_RDLCK, F_WRLCK, or F_UNLCK */
    short     l_whence; /**< \brief flag to choose starting offset */
    long long l_start;  /**< \brief relative offset, in bytes */
    long long l_len;    /**< \brief length, in bytes; 0 means lock to EOF */
    int       l_pid;    /**< \brief returned with F_GETLK */
    long      l_rpid;   /**< \brief Remote process id wanting this lock */
    long      l_rsys;   /**< \brief Remote system id wanting this lock */
};
#endif  /* _POSIX_xxx_SOURCE */

#endif /* #ifndef _ASMLANGUAGE */


#ifdef __cplusplus
}
#endif

#endif /* __AW_TXFS_FCNTL_H */

/* end of file */
