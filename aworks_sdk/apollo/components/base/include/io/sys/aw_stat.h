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
 * \brief stat function.
 *
 * \internal
 * \par modification history:
 * - 1.01 15-04-17 deo, 增加 aw_chmod
 * - 1.00 14-08-11 orz, first implementation.
 * \endinternal
 */

#ifndef __IO_SYS_STAT_H /* { */
#define __IO_SYS_STAT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_ios_stat
 * @{
 */

#include <aw_types.h>
#include "time.h"

/**
 * \brief 文件模式(st_mode)位掩码
 */
#define S_IFMT   0xf000 /**< \brief 文件类型的字段   */
#define S_IFIFO  0x1000 /**< \brief 先进先出(fifo) */
#define S_IFCHR  0x2000 /**< \brief 字符设备 */
#define S_IFDIR  0x4000 /**< \brief 目录 */
#define S_IFBLK  0x6000 /**< \brief 块设备 */
#define S_IFREG  0x8000 /**< \brief 一般文件 */
#define S_IFLNK  0xa000 /**< \brief 符号链接  */
#define S_IFSHM  0xb000 /**< \brief 共享内存 */
#define S_IFSOCK 0xc000 /**< \brief socket */

#define S_ISUID  04000  /**< \brief 用户id位 */
#define S_ISGID  02000  /**< \brief 用户组id位 */

#define S_IRWXU  00700  /**< \brief 文件所有者具有读、写、可执行权限 */
#define S_IRUSR  00400  /**< \brief 文件所有者具有读权限 */
#define S_IWUSR  00200  /**< \brief 文件所有者具有写权限 */
#define S_IXUSR  00100  /**< \brief 文件所有者具有可执行权限 */

#define S_IRWXG  00070  /**< \brief 用户组具有读、写、可执行权限 */
#define S_IRGRP  00040  /**< \brief 用户组具有读权限 */
#define S_IWGRP  00020  /**< \brief 用户组具有写权限 */
#define S_IXGRP  00010  /**< \brief 用户组具有可执行权限 */

#define S_IRWXO  00007  /**< \brief 其他用户具有读、写、可执行权限 */
#define S_IROTH  00004  /**< \brief 其他用户具有读权限 */
#define S_IWOTH  00002  /**< \brief 其他用户具有写权限 */
#define S_IXOTH  00001  /**< \brief 其他用户具有可执行权限 */

/******************************************************************************/
/**
 * \brief 文件类型的判断宏
 */
#define S_ISBLK(m)  ((m & S_IFMT) == S_IFBLK)   /**< \brief 是否为块设备 */
#define S_ISCHR(m)  ((m & S_IFMT) == S_IFCHR)   /**< \brief 是否为字符设备 */
#define S_ISDIR(m)  ((m & S_IFMT) == S_IFDIR)   /**< \brief 是否是目录 */
#define S_ISFIFO(m) ((m & S_IFMT) == S_IFIFO)   /**< \brief 是否是先进先出  */
#define S_ISREG(m)  ((m & S_IFMT) == S_IFREG)   /**< \brief 是否是一般文件 */
#define S_ISLNK(m)  ((m & S_IFMT) == S_IFLNK)   /**< \brief 是否是符号链接 */
#define S_ISSOCK(m) ((m & S_IFMT) == S_IFSOCK)  /**< \brief 是否为socket */

/**
 * \brief POSIX.1b objects.
 * 这些宏用于对参数进行语法检查
 */
#define S_TYPEISMQ(buf)     ((buf)->st_mode - (buf)->st_mode)
#define S_TYPEISSEM(buf)    ((buf)->st_mode - (buf)->st_mode)
#define S_TYPEISSHM(buf)    (((buf)->st_mode & S_IFMT) == S_IFSHM)

/******************************************************************************/

/**
 * \brief stat 结构体
 */
struct aw_stat {
    dev_t   st_dev;     /**< \brief 文件系统设备 */
    ino_t   st_ino;     /**< \brief 文件序列号 */
    mode_t  st_mode;    /**< \brief 文件类型和权限信息 */
    nlink_t st_nlink;   /**< \brief 文件的符号链接数量 */
    uid_t   st_uid;     /**< \brief 用户id */
    gid_t   st_gid;     /**< \brief 组id */
    dev_t   st_rdev;    /**< \brief 设备id */
    off_t   st_size;    /**< \brief 文件长度，单位byte */

    struct timespec st_atim; /**< \brief 最后访问文件的时间 */
    struct timespec st_mtim; /**< \brief 最后修改文件内容的时间 */
    struct timespec st_ctim; /**< \brief 最后修改文件状态的时间 */

    /** \brief 推荐的 I/O 块大小 */
    blksize_t st_blksize;
    /** \brief 这个对象拥有的块数量 */
    blkcnt_t  st_blocks;
};

/**
 * \brief 获取文件状态
 *
 * \param[in]   fildes  文件描述符
 * \param[out]  buf     缓存
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_fstat (int  fildes, struct aw_stat *buf);

/**
 * \brief 获取文件状态
 *
 * \param[in]   path    文件绝对路径
 * \param[out]  buf     缓存
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_stat  (const char *path, struct aw_stat *buf);

/**
 * \brief 创建目录
 *
 * \param[in]   path  目录绝对路径
 * \param[out]  mode  模式
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_mkdir (const char *path, mode_t mode);

/**
 * \brief 修改文件的模式
 *
 * \param[in]   path  文件绝对路径
 * \param[out]  mode  模式
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_chmod(const char *path, mode_t mode);

/** @} grp_ios_stat */

#ifdef __cplusplus
}
#endif


#endif /* } __IO_SYS_STAT_H */

/* end of file */
