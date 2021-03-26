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
 * \brief FAT FS configure file.
 *
 * \internal
 * \par modification history:
 * - 1.00 14-12-12 orz, first implementation.
 * \endinternal
 */
#if 0
/******************************************************************************/
#include "aw_vdebug.h"
#include "aw_fat_fs.h"

/******************************************************************************/
#ifndef AW_CFG_FATFS_VOLUMES
#define AW_CFG_FATFS_VOLUMES    1
#endif

#ifndef AW_CFG_FATFS_FILES
#define AW_CFG_FATFS_FILES      1
#endif

#ifndef AW_CFG_FATFS_BLOCK_SIZE
#define AW_CFG_FATFS_BLOCK_SIZE 512
#endif

/******************************************************************************/
void aw_fatfs_lib_init (void)
{
    static struct fat_fs_volume     volumes[AW_CFG_FATFS_VOLUMES];
    static struct fat_fs_file       files[AW_CFG_FATFS_FILES];

    struct fat_fs_init_struct       init;

    init.vol_pool      = volumes;
    init.vol_pool_size = sizeof(volumes);
    init.fil_pool      = files;
    init.fil_pool_size = sizeof(files);

    if (fat_fs_init(&init) != 0) {
        AW_ERRF(("%s(): failed\n", __func__));
    }
}
#else

/******************************************************************************/
#include "aw_vdebug.h"
#include "aw_fatfs.h"
#include "ff.h"

/******************************************************************************/
#ifndef AW_CFG_FATFS_BLOCK_SIZE
#define AW_CFG_FATFS_BLOCK_SIZE CONFIG_AW_CFG_FATFS_BLOCK_SIZE
#endif


/******************************************************************************/
void aw_fatfs_lib_init (void)
{
    static struct aw_fatfs_volume volumes[CONFIG_AW_CFG_FATFS_VOLUMES];
    static struct aw_fatfs_file   files[CONFIG_AW_CFG_FATFS_FILES];
    int                           ret;

    aw_fatfs_disk_lib_init();

    ret = aw_fatfs_init(&volumes,
                        sizeof(volumes),
                        &files,
                        sizeof(files));
    if (ret != AW_OK) {
        AW_ERRF(("%s(): failed\n", __func__));
    }
}


#endif

/* end of file */

