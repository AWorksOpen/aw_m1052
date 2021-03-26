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

#ifndef __FS_TYPE_H /* { */
#define __FS_TYPE_H

/**
 * \addtogroup grp_fs_type
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
#define AW_FS_FMT_FLAG_LOW_LEVEL    0x01    /**< \brief low level format */

/******************************************************************************/
/** @brief arguments for file system type to make a volume */
struct aw_fs_format_arg {
    const char *vol_name;  /**< \brief volume name */
    size_t      unit_size; /**< \brief unit size of this volume */
    uint_t      flags;     /**< \brief flags for this volume */
};

/** @brief file system type struct */
struct aw_fs_type {
    struct aw_fs_type *fs_next;
    const char        *fs_name;
    void            *(*fs_dev_attach) (const char                    *vol_path,
                                       const char                    *dev_path);
    int              (*fs_dev_detach) (void                          *volume);
    int              (*fs_make_fs)    (const char                    *dev_path,
                                       const struct aw_fs_format_arg *fmt_arg);
};

/******************************************************************************/
struct aw_fs_type *aw_fs_type_find     (const char        *name);
int                aw_fs_type_register (struct aw_fs_type *fs);


#ifdef __cplusplus
}
#endif

/** @} */

#endif /* } __FS_TYPE_H */

/* end of file */
