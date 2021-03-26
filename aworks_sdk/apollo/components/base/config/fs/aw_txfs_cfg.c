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
 * \brief TXFS configure file.
 *
 * \internal
 * \par modification history:
 * - 1.00 14-12-12 orz, first implementation.
 * \endinternal
 */

/******************************************************************************/
#include "aw_vdebug.h"
#include "aw_txfs.h"

/******************************************************************************/
#ifndef AW_CFG_TXFS_VOLUMES
#define AW_CFG_TXFS_VOLUMES    1
#endif

#ifndef AW_CFG_TXFS_FILES
#define AW_CFG_TXFS_FILES      1
#endif

#ifndef AW_CFG_TXFS_BUF_CNT
#define AW_CFG_TXFS_BUF_CNT    50
#endif

/******************************************************************************/
void aw_txfs_lib_init (void)
{
    static struct aw_txfs_vol   volumes[AW_CFG_TXFS_VOLUMES];
    static struct aw_txfs_file  files[AW_CFG_TXFS_FILES];

    if (aw_txfs_init(&volumes[0], sizeof(volumes),
                     &files[0],   sizeof(files)) != AW_OK) {
        AW_ERRF(("%s(): failed\n", __func__));
    }

    if (aw_txfs_buffer_config(AW_CFG_TXFS_BUF_CNT) != AW_OK) {
        AW_ERRF(("aw_txfs_buffer_config(): failed\n"));
    }
}

