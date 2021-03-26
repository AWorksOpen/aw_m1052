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
 * \brief file control options.
 *
 * \internal
 * \par modification history:
 * - 140703 orz, first implementation.
 * \endinternal
 */

#ifndef __IO_FCNTL_H /* { */
#define __IO_FCNTL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_iosys_fcntl
 * @{
 */

#include "aworks.h"
#include "aw_types.h"


/** \brief 读、写执行权限 */
#define O_ACCMODE   (O_RDONLY | O_WRONLY | O_RDWR)

#define O_RDONLY    0x0000  /**< \brief 只读 */
#define O_WRONLY    0x0001  /**< \brief 只写 */
#define O_RDWR      0x0002  /**< \brief 可读写 */
#define O_APPEND    0x0008  /**< \brief 追加 */

#define O_EXCL      0x0100  /**< \brief 可执行 */
#define O_CREAT     0x0200  /**< \brief 创建 */
#define O_TRUNC     0x0400  /**< \brief 截断 */
#define O_DIRECTORY 0x0800  /**< \brief 目录  */

/**
 * \brief 创建文件
 *
 * \param[in]    path      文件名
 * \param[in]    mode      文件权限
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_create (const char *path, mode_t mode);

/**
 * \brief 打开文件
 *
 * \param[in]    path      文件名
 * \param[in]    oflag     打开标志
 * \param[in]    mode      打开模式
 *
 * \return 成功返回文件句柄，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_open (const char *path, int oflag, mode_t mode);

/**
 * \brief 文件控制函数
 *
 * \param[in]        fildes      文件描述符
 * \param[in]        cmd         命令
 * \param[in,out]    arg         参数
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_fcntl  (int fildes, int cmd, void *arg);


/** @} grp_iosys_fcntl */

#ifdef __cplusplus
}
#endif


#endif /* } __IO_FCNTL_H */

/* end of file */
