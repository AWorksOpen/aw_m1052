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
 * \brief I/O library configure file.
 *
 * \internal
 * \par modification history:
 * - 15-04-17 deo, Ôö¼Óaw_dir_init
 * - 1.00 14-12-12 orz, first implementation.
 * \endinternal
 */

#include "apollo.h"
#include "io/aw_io_device.h"
#include "io/aw_io_entry.h"
#include "io/aw_dirent.h"

/******************************************************************************/
#ifndef AW_CFG_IO_OPEN_FILES_MAX
#define AW_CFG_IO_OPEN_FILES_MAX    4
#endif

/******************************************************************************/
void aw_io_lib_init (void)
{
    /* reserved 3 file descriptors for stdin, stdout and stderr */
    static struct aw_io_entry fdent[3 + AW_CFG_IO_OPEN_FILES_MAX];
    static struct aw_dir dirents[AW_CFG_IO_OPEN_FILES_MAX];

    aw_iodev_lib_init();
    aw_io_entry_init(fdent, sizeof(fdent) / sizeof(fdent[0]));

    aw_dir_init(dirents, sizeof(dirents) / sizeof(dirents[0]));
}

/* end of file */

