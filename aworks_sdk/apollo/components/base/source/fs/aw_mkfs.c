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
 * \brief volume formatting.
 *
 * \internal
 * \par modification history:
 * - 1.00 14-07-03 orz, first implementation.
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/
#include "apollo.h"
#include "fs/aw_mount.h"

/******************************************************************************/
int aw_make_fs (const char                    *dev_name,
                const char                    *fs_name,
                const struct aw_fs_format_arg *fmt_arg)
{
    struct aw_fs_type *fs;

    if ((fs = aw_fs_type_find(fs_name)) == NULL) {
        return -AW_EINVAL; /* file system not registered */
    }
    if (fs->fs_make_fs == NULL) {
        return -AW_ENOTSUP;
    }
    return fs->fs_make_fs(dev_name, fmt_arg);
}

/* end of file */
