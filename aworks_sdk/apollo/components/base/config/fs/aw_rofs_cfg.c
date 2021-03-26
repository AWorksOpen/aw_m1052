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
 * \brief ROFS configure file.
 *
 * \internal
 * \par modification history:
 * - 1.00 17-03-07 sdy, first implementation.
 * \endinternal
 */


/******************************************************************************/
#include "aw_vdebug.h"
#include "aw_rofs.h"
#include "ff.h"
#include "fs/aw_blk_dev.h"
#include "rofs_data.h"

/******************************************************************************/
#ifndef AW_CFG_ROFS_VOLUMES
#define AW_CFG_ROFS_VOLUMES    1
#endif

#ifndef AW_CFG_ROFS_FILES
#define AW_CFG_ROFS_FILES      10
#endif
/******************************************************************************/
void aw_rofs_lib_init (void)
{
    static struct aw_rofs_volume volumes[AW_CFG_ROFS_VOLUMES];
    static struct aw_rofs_file   files[AW_CFG_ROFS_FILES];
    static struct aw_blk_dev     blk_dev;
    int                          ret;

    ret = aw_blk_dev_create (&blk_dev, "/dev/rofs0");
    if (ret != AW_OK) {
        AW_ERRF(("rofs: aw_blk_dev_create(): failed\n"));
    }

    ret = aw_blk_dev_inject (&blk_dev);
    if (ret != AW_OK) {
        AW_ERRF(("rofs: aw_blk_dev_inject(): failed\n"));
    }

    ret = aw_rofs_init(&volumes,
                        sizeof(volumes),
                        &files,
                        sizeof(files),
                        rofs_data_begin_get());
    if (ret != AW_OK) {
        AW_ERRF(("%s(): failed\n", __func__));
    }
}

/* end of file */

