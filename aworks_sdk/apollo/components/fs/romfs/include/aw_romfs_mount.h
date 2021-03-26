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
 * \brief AWorks RomFs Mount lib
 *
 * \internal
 * \par modification history:
 * - 1.00 19-04-30  sdy, first implementation
 * \endinternal
 */

#ifndef __AW_ROMFS_MOUNT_H
#define __AW_ROMFS_MOUNT_H

#ifdef __cplusplus
extern "C" {
#endif

aw_err_t aw_romfs_mount (struct aw_romfs_volume *p_vol);
aw_err_t aw_romfs_unmount (struct aw_romfs_volume *p_vol);

#endif /* __AW_ROMFS_MOUNT_H */

/* end of file */
