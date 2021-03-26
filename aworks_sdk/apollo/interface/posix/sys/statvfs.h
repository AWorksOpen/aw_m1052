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
 * \brief VFS file system information.
 *
 * \internal
 * \par modification history:
 * - 140822 orz, first implementation.
 * \endinternal
 */

#ifndef __SYS_STATVFS_H /* { */
#define __SYS_STATVFS_H

#include "io/sys/aw_statvfs.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
#define fstatvfs    aw_fstatvfs
#define statvfs     aw_statvfs


#ifdef __cplusplus
}
#endif

#endif /* } __SYS_STATVFS_H */

/* end of file */
