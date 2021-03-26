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
 * \brief stat function.
 *
 * \internal
 * \par modification history:
 * - 150417 deo, Ôö¼Ó chmod
 * - 140822 orz, first implementation.
 * \endinternal
 */

#ifndef __SYS_STAT_H /* { */
#define __SYS_STAT_H

#include "io/sys/aw_stat.h"

#ifdef __cplusplus
extern "C" {
#endif

struct stat {
    dev_t           st_dev;     /**< \brief device ID of device containing file */
    ino_t           st_ino;     /**< \brief File serial number */
    mode_t          st_mode;    /**< \brief mode of file */
    nlink_t         st_nlink;   /**< \brief number of hard links to the file */
    uid_t           st_uid;     /**< \brief user ID of file */
    gid_t           st_gid;     /**< \brief group ID of file */
    dev_t           st_rdev;    /**< \brief device ID (if file is character or block) */
    off_t           st_size;

    struct timespec st_atim;    /**< \brief last data access time stamp */
    struct timespec st_mtim;    /**< \brief last data modification time stamp */
    struct timespec st_ctim;    /**< \brief last file status change time stamp */
    blksize_t       st_blksize;
    blkcnt_t        st_blocks;
};

/******************************************************************************/
#define fstat(fildes, buf)  aw_fstat(fildes, buf)
#define stat(path, buf)     aw_stat(path, (struct aw_stat*)(buf))
#define lstat(path, buf)    aw_stat(path, buf)
#define mkdir(path, mode)   aw_mkdir(path, mode)
#define chmod(path, mode)	aw_chmod(path, mode)

#ifdef __cplusplus
}
#endif

#endif /* } __SYS_STAT_H */

/* end of file */
