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
 * - 1.01 15-04-29 deo, ÐÞ¸Äaw_fstat
 * - 1.00 14-08-12 orz, first implementation.
 * \endinternal
 */

/******************************************************************************/
#include "apollo.h"
#include "io/sys/aw_stat.h"
#include "io/aw_fcntl.h"
#include "io/aw_stropts.h"
#include "io/aw_unistd.h"
#include "aw_ioctl.h"


/******************************************************************************/
int aw_fstat (int fildes, struct aw_stat *buf)
{
    return aw_ioctl(fildes, AW_FIOSTATGET, buf);
}

/******************************************************************************/
int aw_stat (const char *path, struct aw_stat *buf)
{
    int  fd, err;

    /* try to open file */
    if ((fd = aw_open(path, O_RDONLY, 0777)) < 0) {
        AW_ERRNO_SET(AW_ENOENT);
        return -1;
    }

    err = aw_ioctl(fd, AW_FIOSTATGET, buf);
    if (err < 0) {
        AW_ERRNO_SET(-err);
        err = -1;
    } else {
        AW_ERRNO_SET(0);
        err = 0;
    }

    (void)aw_close(fd);
    return err;
}

/******************************************************************************/
int aw_mkdir (const char *path, mode_t mode)
{
    int fd = aw_open(path, O_RDWR | O_CREAT | O_DIRECTORY, mode);

    return fd < 0 ? fd : aw_close(fd);
}

/******************************************************************************/
int aw_chmod(const char *path, mode_t mode)
{
    int fd, err;

    if ((fd = aw_open(path, O_RDONLY, 0777)) < 0) {
        return fd;
    }

    err = aw_ioctl(fd, AW_FIOCHMOD, (void*)&mode);

    (void)aw_close(fd);

    return err;
}
/* end of file */
