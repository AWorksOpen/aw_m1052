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
 * \brief statvfs function.
 *
 * \internal
 * \par modification history:
 * - 1.01 15-05-11 deo, ÐÞ¸Äaw_fstat
 * \endinternal
 */

#include "apollo.h"
#include "io/sys/aw_statvfs.h"
#include "io/aw_stropts.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "aw_ioctl.h"


int aw_fstatvfs (int fildes, struct aw_statvfs *buf)
{
    return aw_ioctl(fildes, AW_FIOFSTATFSGET, buf);
}

int aw_statvfs (const char *path, struct aw_statvfs *buf)
{
    int fd, err;
    const char *subpath;

    if ((fd = aw_open(path, O_RDONLY, 0777)) < 0) {
        return fd;
    }

    err = aw_ioctl(fd, AW_FIOFSTATFSGET, buf);

    (void)aw_close(fd);
    return err;
}
