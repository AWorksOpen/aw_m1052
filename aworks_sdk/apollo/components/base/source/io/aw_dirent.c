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
 * \brief 目录管理
 *
 * \internal
 * \par modification history:
 * - 1.02 17-12-27 deo, 使用aw_io_pwd
 * - 1.01 15-04-17 deo, 修改aw_opendir、aw_closedir，增加aw_dir_init和__g_dir_pool
 * - 1.00 14-08-14 orz, first implementation.
 * \endinternal
 */

/******************************************************************************/
#include "apollo.h"
#include "aw_pool.h"
#include "aw_common.h"
#include "io/sys/aw_stat.h"
#include "io/aw_dirent.h"
#include "io/aw_io_path.h"
#include "io/aw_unistd.h"
#include "io/aw_stropts.h"
#include "io/aw_fcntl.h"
#include "string.h"


aw_local aw_pool_t __g_dir_pool;




/******************************************************************************/
int aw_opendir_r (struct aw_dir *dirp, const char *dirname)
{
    int fd;

    if ((fd = aw_open(dirname, O_RDONLY | O_DIRECTORY, 0777)) < 0) {
        return fd;
    }
    dirp->d_fildes = fd;
    dirp->d_index  = 0;
    dirp->alloc    = AW_FALSE;
    return AW_OK;
}




/******************************************************************************/
struct aw_dir * aw_opendir (const char *dirname)
{
    struct aw_dir * dirp;
    int             fd;

    if ((fd = aw_open(dirname, O_RDONLY | O_DIRECTORY, 0777)) < 0) {
        return NULL;
    }

    dirp = (struct aw_dir *)aw_pool_item_get(&__g_dir_pool);
    if (dirp != NULL) {
        memset(dirp, 0, sizeof(struct aw_dir));
    }

    dirp->d_fildes = fd;
    dirp->d_index  = 0;
    dirp->alloc    = AW_TRUE;

    return dirp;
}




/******************************************************************************/
int aw_closedir (struct aw_dir *dirp)
{
    int ret;

    if (dirp == NULL) {
        return -AW_EINVAL;
    }

    ret = aw_close(dirp->d_fildes);

    if (dirp->alloc) {
        if (aw_pool_item_return(&__g_dir_pool, dirp)) {
            ret = -AW_EFAULT;
        }
    }

    return ret;
}




/******************************************************************************/
int aw_readdir_r (struct aw_dir     *dirp,
                  struct aw_dirent  *entry,
                  struct aw_dirent **result)
{
    int err = aw_ioctl(dirp->d_fildes, AW_FIOREADDIR, dirp);

    if (0 == err) {
        if (entry != &dirp->d_dirent) {
            (void)memcpy(entry, &dirp->d_dirent, sizeof(*entry));
        }
        *result = entry;
    } else {
        *result = NULL;
    }
    return err;
}




/******************************************************************************/
struct aw_dirent *aw_readdir (struct aw_dir *dirp)
{
    struct aw_dirent *ent;

    (void)aw_readdir_r(dirp, &dirp->d_dirent, &ent);

    return ent;
}




/******************************************************************************/
void aw_dir_init (struct aw_dir *dirp, unsigned int dir_num)
{
    aw_pool_init(&__g_dir_pool,
                 dirp,
                 sizeof(struct aw_dir) * dir_num,
                 sizeof(struct aw_dir));
}

/* end of file */
