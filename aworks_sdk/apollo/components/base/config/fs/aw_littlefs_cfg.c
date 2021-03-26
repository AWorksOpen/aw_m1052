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
 * \brief LITTLEFS configure file
 *
 * \internal
 * \par modification history:
 * - 1.00 16-06-28 cyl, first implementation.
 * \endinternal
 */

#include "aw_vdebug.h"
#include "aw_littlefs.h"

#ifndef AW_CFG_LITTLEFS_VOLUMES
#define AW_CFG_LITTLEFS_VOLUMES     2
#endif

#ifndef AW_CFG_LITTLEFS_FILES
#define AW_CFG_LITTLEFS_FILES       10
#endif


void aw_littlefs_lib_init (void)
{
    static struct aw_littlefs_volume volumes[AW_CFG_LITTLEFS_VOLUMES];
    static struct aw_littlefs_file   files[AW_CFG_LITTLEFS_FILES];

    (void)aw_littlefs_init(&volumes,
                           sizeof(volumes),
                           &files,
                           sizeof(files));
}
