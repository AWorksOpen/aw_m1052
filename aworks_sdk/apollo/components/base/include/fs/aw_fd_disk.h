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
 * \brief file descriptor for block device
 *
 * \internal
 * \par modification history:
 * - 1.00 15-09-21  deo, first implementation
 * \endinternal
 */


#ifndef __AW_FD_DISK_H
#define __AW_FD_DISK_H

#ifdef __cplusplus
extern "C" {
#endif


#include "fs/aw_block_dev.h"

/******************************************************************************/

/** \brief file descriptor disk data */
struct aw_fd_disk {
    struct aw_block_dev  bd_dev;    /**< \brief block device instance */
    int                  fd;
};
typedef struct aw_fd_disk* pfdisk;

/******************************************************************************/
pfdisk aw_fd_disk_create (const char         *path,
                          uint_t             disk_size);

int aw_fd_disk_delete (pfdisk disk);

int aw_fd_disk_attach (struct aw_fd_disk *disk,
                       uint_t             disk_size,
                       const char         *path);

int aw_fd_disk_detach (struct aw_fd_disk *disk);

#ifdef __cplusplus
}
#endif

#endif /* __AW_FD_DISK_H */
