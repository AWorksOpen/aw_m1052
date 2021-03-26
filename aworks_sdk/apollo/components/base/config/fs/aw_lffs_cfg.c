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
 * \brief LFFS configure file.
 *
 * \internal
 * \par modification history:
 * - 170923, vih, first implementation.
 * \endinternal
 */

/******************************************************************************/
#include "aw_lffs.h"
/******************************************************************************/
#ifndef AW_CFG_LFFS_VOLUMES
#define AW_CFG_LFFS_VOLUMES     2
#endif

#ifndef AW_CFG_LFFS_FILES
#define AW_CFG_LFFS_FILES       4
#endif

/******************************************************************************/
void aw_lffs_lib_init (void)
{
   static struct aw_lffs_volume volumes[AW_CFG_LFFS_VOLUMES];
   static struct aw_lffs_file_desc   files[AW_CFG_LFFS_FILES];

#if ((AW_LFFS_CONFIG_USE_SYSTEM_MEMORY_ALLOCATOR == 0) || \
(!defined(AW_LFFS_CONFIG_USE_SYSTEM_MEMORY_ALLOCATOR)))
   static uint8_t               buffer[AW_CFG_LFFS_VOLUMES * 16 * 1024];
#endif

   struct aw_lffs_init_struct   init;


   init.vol_pool      = volumes;
   init.vol_pool_size = sizeof(volumes);
   init.fil_pool      = files;
   init.fil_pool_size = sizeof(files);

#if ((AW_LFFS_CONFIG_USE_SYSTEM_MEMORY_ALLOCATOR == 0) || \
(!defined(AW_LFFS_CONFIG_USE_SYSTEM_MEMORY_ALLOCATOR)))

   init.buf_pool      = (void*)buffer;
   init.buf_pool_size = sizeof(buffer);
   init.buf_item_size = sizeof(buffer) / AW_CFG_LFFS_VOLUMES;
#endif

   (void)aw_lffs_init(&init);
}


/* end of file */

