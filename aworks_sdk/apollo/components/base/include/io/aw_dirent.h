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
 * \brief format of directory entries.
 *
 * \internal
 * \par modification history:
 * - 1.01 15-04-17 deo, 修改aw_opendir函数
 * - 1.00 14-08-11 orz, first implementation.
 * \endinternal
 */

#ifndef __IO_DIRENT_H /* { */
#define __IO_DIRENT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_iosys_dirent
 * @{
 */

#include "aworks.h"
#include "aw_types.h"
#include "aw_pool.h"
#include "io/aw_io_path.h"


/**
 * \brief 目录入口
 */
struct aw_dirent {
    /** \brief 文件序列号 */
    ino_t   d_ino;

    /** \brief 文件名，最长不超过 NAME_MAX  */
    char    d_name[AW_IO_PATH_LEN_MAX + 1];
};

/** \brief 目录描述符  */
struct aw_dir {
    int              d_fildes; /**< \brief 这个目录的文件描述符 */
    int              d_index;  /**< \brief 目录索引 */
    struct aw_dirent d_dirent; /**< \brief 目录入口实例 */
    aw_bool_t           alloc;
};


/**
 * \brief 打开目录
 *
 * \param[in]   dirp     目录对象指针
 * \param[in]   dirname  目录名
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_opendir_r (struct aw_dir *dirp, const char *dirname);

/**
 * \brief 打开目录
 *
 * \param[in]   dirname  目录名
 *
 * \return 成功返回目录指针，否则返回NULL
 */
struct aw_dir *aw_opendir(const char *dirname);

/**
 * \brief 关闭目录
 *
 * \param[in]   dirp  目录指针
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_closedir(struct aw_dir *dirp);

/**
 * \brief 读目录入口
 *
 * \param[in]    dirp    目录指针
 * \param[in]    entry   目录入口指针
 * \param[out]   result  下一个目录入口指针
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_readdir_r (struct aw_dir     *dirp,
                  struct aw_dirent  *entry,
                  struct aw_dirent **result);

/**
 * \brief 读目录入口
 *
 * \param[in]    dirp    目录指针
 *
 * \return 成功返回目录入口指针，否则返回NULL。
 */
struct aw_dirent *aw_readdir (struct aw_dir *dirp);


/**
 * \brief 目录描述符初始化
 *
 * \param[in]    dirp      目录指针
 * \param[in]    dir_num   目录数量
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
void aw_dir_init (struct aw_dir *dirp, unsigned int dir_num);

/** @} grp_iosys_dirent */

#ifdef __cplusplus
}
#endif

#endif /* } __IO_DIRENT_H */

/* end of file */
