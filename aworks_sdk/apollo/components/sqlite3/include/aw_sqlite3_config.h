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
 * \brief Sqlite3's vfs implementation.(AWorks platform)
 *
 * \internal
 * \par modification history:
 * - 1.00 16-01-18  cyl, first implementation.
 * \endinternal
 */

#ifndef __AW_SQLITE3_CONFIG_H
#define __AW_SQLITE3_CONFIG_H

/** \brief 使用非windows平台、unix-like平台时需要将该宏定义为1 */
#define SQLITE_OS_OTHER         1

/** \brief 将aw_sqlite3_io.c文件中的代码加入编译需要定义该宏 */
#define SQLITE_OS_AWORKS

/** \brief 将aw_sqlite3_os.c文件中的锁管理接口相关代码加入编译需要定义该宏 */
#define SQLITE_MUTEX_AWORKS

/** \brief 将aw_sqlite3_os.c文件中的内存管理接口相关代码加入编译需要定义该宏 */
#define SQLITE_AWORKS_MALLOC

/**
 * \brief 若需要对AWorks的OS接口层函数的调用进行调试，
 *        则需要使能SQLITE_AWORKS_DEBUG宏定义
 */
#if 0
#  define SQLITE_AWORKS_DEBUG
#endif

/* #define SQLITE_DEBUG 1*/

#endif /* __AW_SQLITE3_CONFIG_H */

/* end of file */
