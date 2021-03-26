/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief Automatic file system mount configure file.
 *
 * \internal
 * \par modification history:
 * - 20-09-30, vih, first implementation.
 * \endinternal
 */

#include "aworks.h"
#include "fs/aw_auto_mount.h"

static aw_auto_mount_cfg_t __g_cfg[] = {
    { "/sf",  "/sflash0",       "lffs", 0, 1},
    { "/nf0", "/dev/mtd0",      "lffs", 0, 1},
    { "/nf1", "/dev/mtd1",      "lffs", 0, 1},
    { "/sd",  "/dev/sd0",       "vfat", 0, 1},
    { "/u",   "/dev/h0-d1-0",   "vfat", 0, 1},
    { NULL, NULL, NULL, 0, 0}
};

aw_err_t aw_auto_mount_cfg (void)
{
    return aw_auto_mount_tbl_add(__g_cfg);
}


