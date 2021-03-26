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
 * \brief Transaction File System types interfaces
 *
 * \internal
 * \par modification history:
 * - 1.00 17-07-06  mkr, first implementation
 * \endinternal
 */

#ifndef __AW_TXFS_TYPES_H
#define __AW_TXFS_TYPES_H

#include "aw_vdebug.h"
#include "io/aw_fcntl.h"
#include "io/aw_dirent.h"
#include "aw_compiler.h"
#include "aw_common.h"
#include "aw_sem.h"
#include "aw_task.h"
#include "aw_refcnt.h"


#ifdef __cplusplus
extern "C" {
#endif

#define FAST      register
#define LOCAL     aw_local

/** \brief typedef */
/** \brief atomic type define */
typedef struct aw_refcnt atomic_t;

/** \brief sector_t type define */
typedef uint_t sector_t;

#if 0 
typedef STATUS (*hrfs_fmt_rtn) (char      *path,       
                                uint64_t  disk_size,
                                uint32_t  block_size, 
                                uint32_t  files,
                                uint32_t  major_ver, 
                                uint32_t  minor_ver,
                                uint32_t  option);
#endif 

#ifdef __cplusplus
}
#endif

#endif /* __AW_TXFS_TYPES_H */

/* end of file */
