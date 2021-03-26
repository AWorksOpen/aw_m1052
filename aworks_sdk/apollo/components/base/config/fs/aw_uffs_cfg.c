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
 * \brief UFFS configure file.
 *
 * \internal
 * \par modification history:
 * - 1.01 15-11-07 deo, add buf_pool
 * - 1.00 14-12-12 orz, first implementation.
 * \endinternal
 */

/******************************************************************************/
#include "aw_vdebug.h"
#include "fs/aw_uffs.h"

/******************************************************************************/
#ifndef AW_CFG_UFFS_VOLUMES
#define AW_CFG_UFFS_VOLUMES     1
#endif

#ifndef AW_CFG_UFFS_FILES
#define AW_CFG_UFFS_FILES       1
#endif

/******************************************************************************/
void aw_uffs_lib_init (void)
{
    static struct aw_uffs_volume volumes[AW_CFG_UFFS_VOLUMES];
    static struct aw_uffs_file   files[AW_CFG_UFFS_FILES];

#if ((CONFIG_USE_SYSTEM_MEMORY_ALLOCATOR == 0) || \
     (!defined(CONFIG_USE_SYSTEM_MEMORY_ALLOCATOR)))
    static uint8_t               buffer[AW_CFG_UFFS_VOLUMES * 16 * 1024];
#endif

    struct aw_uffs_init_struct   init;


    init.vol_pool      = volumes;
    init.vol_pool_size = sizeof(volumes);
    init.fil_pool      = files;
    init.fil_pool_size = sizeof(files);

#if ((CONFIG_USE_SYSTEM_MEMORY_ALLOCATOR == 0) || \
     (!defined(CONFIG_USE_SYSTEM_MEMORY_ALLOCATOR)))

    init.buf_pool      = (void*)buffer;
    init.buf_pool_size = sizeof(buffer);
    init.buf_item_size = sizeof(buffer) / AW_CFG_UFFS_VOLUMES;
#endif

    (void)aw_uffs_init(&init);
}


/* end of file */

