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
 * \brief RAW FS configure file.
 *
 * \internal
 * \par modification history:
 * - 1.00 14-12-12 orz, first implementation.
 * \endinternal
 */

/******************************************************************************/
#include "aw_vdebug.h"
#include "fs/aw_raw_fs.h"

/******************************************************************************/
#ifndef AW_CFG_RAWFS_VOLUMES
#define AW_CFG_RAWFS_VOLUMES    1
#endif

#ifndef AW_CFG_RAWFS_FILES
#define AW_CFG_RAWFS_FILES      1
#endif

#ifndef AW_CFG_RAWFS_BLOCK_SIZE
#define AW_CFG_RAWFS_BLOCK_SIZE 512
#endif

/******************************************************************************/
void aw_rawfs_lib_init (void)
{
    static struct aw_raw_fs_volume volumes[AW_CFG_RAWFS_VOLUMES];
    static struct aw_raw_fs_file   files[AW_CFG_RAWFS_FILES];
    int                            ret;

    ret = aw_raw_fs_init(volumes,
                         sizeof(volumes),
                         files,
                         sizeof(files));
    if (ret != AW_OK) {
        AW_ERRF(("%s(): failed\n", __func__));
    }
}

/* end of file */

