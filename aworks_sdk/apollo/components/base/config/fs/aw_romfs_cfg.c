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
 * \brief ROMFS configure file.
 *
 * \internal
 * \par modification history:
 * - 190326, vih, first implementation.
 * \endinternal
 */

/******************************************************************************/
#include "aw_vdebug.h"
#include "aw_romfs.h"
#include "aw_romfs_dev.h"

/******************************************************************************/
#ifndef AW_CFG_ROMFS_VOLUMES
#define AW_CFG_ROMFS_VOLUMES     1
#endif

#ifndef AW_CFG_ROMFS_FILES
#define AW_CFG_ROMFS_FILES       4
#endif

/******************************************************************************/
void aw_romfs_lib_init (void)
{
    static struct aw_romfs_volume volumes[AW_CFG_ROMFS_VOLUMES];
    static struct aw_romfs_file   files[AW_CFG_ROMFS_FILES];
    int                           ret;

    aw_romfs_dev_lib_init();

    ret = aw_romfs_init(&volumes,
                        sizeof(volumes),
                        &files,
                        sizeof(files));
    if (ret != AW_OK) {
        AW_ERRF(("%s(): failed\n", __func__));
    }
}

/* end of file */

