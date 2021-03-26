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
 * \brief VFS 文件系统信息.
 *
 * \internal
 * \par modification history:
 * - 1.01 15-04-17 deo, 修改struct statvfs为struct aw_statvfs
 * - 1.00 14-08-12 orz, first implementation.
 * \endinternal
 */

#ifndef __IO_SYS_STATVFS_H /* { */
#define __IO_SYS_STATVFS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_ios_statvfs
 * @{
 */

#include <aw_types.h> 

#include "aw_compiler.h"


#define ST_RDONLY   0x01    /**< \brief 只读 */

/**
 * \brief Does not support the semantics of
 * the ST_ISUID and ST_SSGID file mode bits
 */
#define ST_NOSUID   0x02

#ifndef _FSBLKCNT_T_DECLARED
typedef unsigned int fsblkcnt_t;
typedef unsigned int fsfilcnt_t;
#define _FSBLKCNT_T_DECLARED
#endif


/**
 * \brief VFS 结构体
 */
struct aw_statvfs {
    unsigned long f_bsize;  /**< \brief 块大小 */
    unsigned long f_frsize; /**< \brief 文件系统基础块大小 */

    fsblkcnt_t    f_blocks; /**< \brief 块数量 */
    fsblkcnt_t    f_bfree;  /**< \brief 空闲块数量 */
    fsblkcnt_t    f_bavail; /**< \brief 可用块数量 */

    fsfilcnt_t    f_files;  /**< \brief 文件序列号数量 */
    fsfilcnt_t    f_ffree;  /**< \brief 空闲的文件序列号数量 */
    fsfilcnt_t    f_favail; /**< \brief 可用的文件序列号数量 */

    unsigned long f_fsid;   /**< \brief 文件系统ID */
    unsigned long f_flag;   /**< \brief 位掩码 */
    unsigned long f_namemax;/**< \brief 最大文件名长度 */
};

/**
 * \brief 获取文件状态
 *
 * \param[in]   fildes  文件描述符
 * \param[out]  buf     缓存
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_fstatvfs (int fildes, struct aw_statvfs *buf);

/**
 * \brief 获取文件状态
 *
 * \param[in]   fildes  文件描述符
 * \param[out]  buf     缓存
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_statvfs (const char *path, struct aw_statvfs *buf);

/** @} grp_ios_statvfs */

#ifdef __cplusplus
}
#endif

#endif /* } __IO_SYS_STATVFS_H */

/* end of file */
