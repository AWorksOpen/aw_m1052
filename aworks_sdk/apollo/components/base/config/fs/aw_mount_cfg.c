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
 * \brief FS mount configure file.
 *
 * \internal
 * \par modification history:
 * - 1.00 14-12-12 orz, first implementation.
 * \endinternal
 */

/******************************************************************************/
#include "aw_vdebug.h"
#include "fs/aw_mount.h"

/******************************************************************************/
#ifndef AW_CFG_MOUNT_POINTS
#define AW_CFG_MOUNT_POINTS 2
#endif

/******************************************************************************/
void aw_mount_lib_init (void)
{
    static struct aw_mount_info mnt_info[AW_CFG_MOUNT_POINTS];

    aw_mount_init(mnt_info, sizeof(mnt_info));
}


/* end of file */

