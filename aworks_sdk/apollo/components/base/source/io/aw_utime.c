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
 * \brief access and modification times
 *
 * \internal
 * \par modification history:
 * - 1.00 15-04-20 deo, first implementation.
 * \endinternal
 */
#include "apollo.h"
#include "io/aw_fcntl.h"
#include "io/aw_stropts.h"
#include "aw_ioctl.h"
#include "io/aw_utime.h"
#include "io/aw_unistd.h"




int aw_utime(const char *path, struct aw_utimbuf *times)
{
    int err, fd;

    if ((fd = aw_open(path, O_RDONLY, 0777)) < 0) {
        return fd;
    }

    err = aw_ioctl(fd, AW_FIOUTIME, times);
    (void)aw_close(fd);
    return err;
}

/* end of file */
