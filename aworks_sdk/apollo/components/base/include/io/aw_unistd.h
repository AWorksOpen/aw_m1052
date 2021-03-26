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
 * \brief standard symbolic constants and types.
 *
 * \internal
 * \par modification history:
 * - 1.02 15-04-30 deo, 增加aw_ftruncate,aw_fsync
 * - 1.01 15-04-17 deo, 增加aw_truncate
 * - 1.00 14-08-22 orz, first implementation.
 * \endinternal
 */

#ifndef __IO_UNISTD_H /* { */
#define __IO_UNISTD_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_iosys_unistd
 * @{
 */
#include "aworks.h"
#include "aw_types.h"
#include "stdio.h"          /**< \brief for SEEK_SET, SEEK_CUR and SEEK_END */



#ifndef SEEK_SET
#define    SEEK_SET    0    /**< \brief set file offset to offset */
#endif
#ifndef SEEK_CUR
#define    SEEK_CUR    1    /**< \brief set file offset to current plus offset */
#endif
#ifndef SEEK_END
#define    SEEK_END    2    /**< \brief set file offset to EOF plus offset */
#endif


/**
 * \brief seek结构体
 */
struct aw_seek_struct {
    int   whence;          /**< \brief seek的模式 */
    off_t offset;          /**< \brief seek的偏移 */
};


/**
 * \brief 关闭文件
 *
 * \param[in]         filedes     文件描述符
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_close (int filedes);

/**
 * \brief 读文件
 *
 * \param[in]         filedes     文件描述符
 * \param[out]        buf         缓存
 * \param[in]         nbyte       字节数
 *
 * \return 成功返回读取到的字节数，否则返回值为错误编码，见 aw_errno.h 。
 */
ssize_t aw_read  (int filedes, void *buf, size_t nbyte);

/**
 * \brief 写文件
 *
 * \param[in]         filedes     文件描述符
 * \param[in]         buf         缓存
 * \param[in]         nbyte       字节数
 *
 * \return 成功返回写入的字节数，否则返回值为错误编码，见 aw_errno.h 。
 */
ssize_t aw_write (int filedes, const void *buf, size_t nbyte);


/**
 * \brief 设置文件读写的位置
 *
 * \param[in]         filedes     文件描述符
 * \param[in]         offset      偏移
 * \param[in]         whence      seek的模式
 *
 * \return 成功返回偏移量，否则返回值为错误编码，见 aw_errno.h 。
 */
off_t aw_lseek (int fildes, off_t offset, int whence);

/**
 * \brief 删除一个目录
 *
 * \param[in]         path     目录路径
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_rmdir  (const char *path);

/**
 * \brief 删除一个链接
 *
 * \param[in]         path     路径
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_unlink (const char *path);

/**
 * \brief 重命名
 *
 * \param[in]         oldpath     旧路径
 * \param[in]         newpath     新路径
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_rename (const char *oldpath, const char *newpath);

/**
 * \brief 文件截断
 *
 * \param[in]         path        路径
 * \param[in]         length      截断长度
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_truncate (const char *path, off_t length);

/**
 * \brief 文件截断
 *
 * \param[in]         fildes      文件描述符
 * \param[in]         length      截断长度
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_ftruncate(int fildes, off_t length);

/**
 * \brief 获取当前路径
 *
 * \param[out]         buf      缓存
 * \param[in]          size     缓存大小
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
char *aw_getcwd (char *buf, size_t size);

/**
 * \brief 改变当前路径
 *
 * \param[in]          path     路径名
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_chdir (const char *path);

/**
 * \brief 文件同步
 *
 * \param[in]          fildes     文件描述符
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_fsync(int fildes);


/** @} grp_iosys_unistd */

#ifdef __cplusplus
}
#endif


#endif /* } __IO_UNISTD_H */

/* end of file */
