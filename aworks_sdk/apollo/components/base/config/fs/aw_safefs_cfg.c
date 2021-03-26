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
 * \brief Safe flash file system configure file.
 *
 * \internal
 * \par modification history:
 * - 1.00 15-07-08 cod, first implementation.
 * \endinternal
 */

/******************************************************************************/
#include "aw_vdebug.h"
#include "fs/aw_safefs.h"

/******************************************************************************/
#ifndef AW_CFG_SAFEFS_VOLUMES
#define AW_CFG_SAFEFS_VOLUMES    1
#endif

#ifndef AW_CFG_SAFEFS_FILES
#define AW_CFG_SAFEFS_FILES      1
#endif

/******************************************************************************/
void aw_safefs_lib_init (void)
{
    static struct aw_safe_fs_volume  volumes[AW_CFG_SAFEFS_VOLUMES];
    static struct aw_safe_fs_file    files[AW_CFG_SAFEFS_FILES];
    struct aw_safe_fs_init_struct    init;

    init.vol_pool      = volumes;
    init.vol_pool_size = sizeof(volumes);
    init.fil_pool      = files;
    init.fil_pool_size = sizeof(files);

    if (aw_safefs_init(&init) != 0) {
        AW_ERRF(("%s(): failed\n", __func__));
    }
}

/* end of file */

