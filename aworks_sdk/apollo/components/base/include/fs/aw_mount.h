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
 * \brief device mount management.
 *
 * \internal
 * \par modification history:
 * - 1.01 14-12-23 orz, remove block device pointer from mount info.
 * - 1.00 14-08-15 orz, first implementation.
 * \endinternal
 */

#ifndef __FS_MOUNT_H /* { */
#define __FS_MOUNT_H

#include "fs/aw_fs_type.h"

/**
 * \addtogroup grp_fs_mount
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/

/**
 * \note if AW_CFG_MOUNT_POINT_NAME_MAX or AW_CFG_MOUNT_DEV_NAME_MAX is > 0,
 * the name is copied into mount info, otherwise mount info just hold the name
 * string pointer.
 *
 * if the name of mount point or device is volatile, e.g. the name is
 * generated in a char array that allocated in stack, we must make this two
 * configure macros > 0.
 */

#ifndef AW_CFG_MOUNT_POINT_NAME_MAX
#define AW_CFG_MOUNT_POINT_NAME_MAX 32 /**< \brief 0 for infinite name length */
#endif

#ifndef AW_CFG_MOUNT_DEV_NAME_MAX
#define AW_CFG_MOUNT_DEV_NAME_MAX   32 /**< \brief 0 for infinite name length */
#endif

/******************************************************************************/

/** \brief mount pointer information */
struct aw_mount_info {
    struct aw_mount_info *next;

    /** \brief mount path */
#if (AW_CFG_MOUNT_POINT_NAME_MAX > 0)
    char                  mnt_point[AW_CFG_MOUNT_POINT_NAME_MAX];
#else
    const char           *mnt_point;
#endif

    /** \brief device to be mounted */
#if (AW_CFG_MOUNT_DEV_NAME_MAX > 0)
    char                  dev_name[AW_CFG_MOUNT_DEV_NAME_MAX];
#else
    const char           *dev_name;
#endif

    struct aw_fs_type    *fs;        /**< \brief FS type the device mounted as*/
    void                 *fs_data;   /**< \brief data for FS device */
    unsigned              flags;     /**< \brief mount flags */
};

/******************************************************************************/
void aw_mount_init (struct aw_mount_info *p_info, unsigned int info_nbytes);

int aw_mount (const char *mnt, const char *dev, const char *fs, unsigned flags);

int aw_umount (const char *path, unsigned flags);

/******************************************************************************/
int aw_make_fs (const char                     *dev_name,
                const char                     *fs_name,
                const struct aw_fs_format_arg  *fmt_arg);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* } __FS_MOUNT_H */

/* end of file */
