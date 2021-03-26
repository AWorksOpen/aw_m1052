/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief Automatic file system mount management.
 *
 * \internal
 * \par modification history:
 * - 1.00 20-09-30 vih, first implementation.
 * \endinternal
 */
#ifndef __AW_AUTO_MOUNT_H_
#define __AW_AUTO_MOUNT_H_

/**
 * \addtogroup grp_fs_mount
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char *mnt;
    const char *dev;
    const char *fs;
    unsigned    mount_flags;
    aw_bool_t   auto_format;        /* 请求自动格式化未格式化的磁盘   */
} aw_auto_mount_cfg_t;

aw_err_t aw_auto_mount_tbl_add (aw_auto_mount_cfg_t *p_arr);
void aw_auto_mount_tbl_clear (void);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* __AW_AUTO_MOUNT_H_ */

/* end of file */
