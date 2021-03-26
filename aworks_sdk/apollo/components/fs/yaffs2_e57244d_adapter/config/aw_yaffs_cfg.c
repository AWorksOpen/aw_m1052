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
 * \brief YAFFS configure file
 *
 * \internal
 * \par modification history:
 * - 1.00 16-06-28 cyl, first implementation.
 * \endinternal
 */

#include "aw_vdebug.h"
#include "aw_yaffs.h"

#ifndef AW_CFG_YAFFS_VOLUMES
#define AW_CFG_YAFFS_VOLUMES     2
#endif

#ifndef AW_CFG_YAFFS_FILES
#define AW_CFG_YAFFS_FILES       4
#endif


void aw_yaffs_lib_init (void)
{
    static struct aw_yaffs_volume volumes[AW_CFG_YAFFS_VOLUMES];
    static struct aw_yaffs_file   files[AW_CFG_YAFFS_FILES];

    struct aw_yaffs_init_param init;


    init.vol_pool      = volumes;
    init.vol_pool_size = sizeof(volumes);
    init.fil_pool      = files;
    init.fil_pool_size = sizeof(files);

    (void)aw_yaffs_init(&init);
}
