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
 * \brief File system types management.
 *
 * \internal
 * \par modification history:
 * - 1.00 14-08-05 orz, first implementation.
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/
#include "apollo.h"
#include "fs/aw_fs_type.h"
#include "aw_int.h"
#include "string.h"

/*******************************************************************************
  local variables
*******************************************************************************/
static struct aw_fs_type *__gp_fs_type_list = NULL;

/******************************************************************************/
/** @brief find a file system type on the FS list */
struct aw_fs_type *aw_fs_type_find (const char *name)
{
    struct aw_fs_type   *fs_find = NULL;
    struct aw_fs_type   *fs;
    AW_INT_CPU_LOCK_DECL(flags);

    AW_INT_CPU_LOCK(flags);
    for (fs = __gp_fs_type_list; fs != NULL; fs = fs->fs_next) {
        if (strcmp(name, fs->fs_name) == 0) {
            fs_find = fs;
            break;
        }
    }
    AW_INT_CPU_UNLOCK(flags);

    return fs_find;
}

/** \brief register a file system type */
int aw_fs_type_register (struct aw_fs_type *fs)
{
    AW_INT_CPU_LOCK_DECL(flags);

    if (aw_fs_type_find(fs->fs_name) != NULL) {
        return -AW_EINVAL; /* file system type already exist */
    }
    /* put FS type on the FS list */
    AW_INT_CPU_LOCK(flags);
    fs->fs_next       = __gp_fs_type_list;
    __gp_fs_type_list = fs;
    AW_INT_CPU_UNLOCK(flags);

    return 0;
}

/* end of file */
