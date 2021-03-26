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
 * \brief format of directory entries.
 *
 * \internal
 * \par modification history:
 * - 150417 deo, ÐÞ¸Äopendir_rÎªopendir
 * - 140822 orz, first implementation.
 * \endinternal
 */

#ifndef __DIRENT_H /* { */
#define __DIRENT_H

#include "io/aw_dirent.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
typedef struct aw_dir DIR;

struct dirent {
    ino_t  d_ino;
    char   d_name[AW_IO_PATH_LEN_MAX + 1];
};

#if 0
/******************************************************************************/
#define opendir     aw_opendir
#define closedir    aw_closedir
#define readdir_r   aw_readdir_r
#define readdir     aw_readdir
#endif

/******************************************************************************/
aw_local aw_inline DIR *opendir (const char *dirname)
{
    return (DIR*)aw_opendir(dirname);
}




/******************************************************************************/
aw_local aw_inline int closedir (DIR *dirp)
{
    return aw_closedir((struct aw_dir*)dirp);
}




/******************************************************************************/
aw_local aw_inline int readdir_r (DIR            *dirp,
                                  struct dirent  *entry,
                                  struct dirent **result)
{
    return aw_readdir_r((struct aw_dir*)dirp,
                        (struct aw_dirent*)entry,
                        (struct aw_dirent**)result);
}




/******************************************************************************/
aw_local  aw_inline struct dirent *readdir (DIR *dirp)
{
    return (struct dirent*)aw_readdir((struct aw_dir*)dirp);
}





#ifdef __cplusplus
}
#endif

#endif /* } __DIRENT_H */

/* end of file */
