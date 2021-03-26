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
 * - 1.01 15-04-17 deo, �޸�aw_opendir����
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
 * \brief Ŀ¼���
 */
struct aw_dirent {
    /** \brief �ļ����к� */
    ino_t   d_ino;

    /** \brief �ļ������������ NAME_MAX  */
    char    d_name[AW_IO_PATH_LEN_MAX + 1];
};

/** \brief Ŀ¼������  */
struct aw_dir {
    int              d_fildes; /**< \brief ���Ŀ¼���ļ������� */
    int              d_index;  /**< \brief Ŀ¼���� */
    struct aw_dirent d_dirent; /**< \brief Ŀ¼���ʵ�� */
    aw_bool_t           alloc;
};


/**
 * \brief ��Ŀ¼
 *
 * \param[in]   dirp     Ŀ¼����ָ��
 * \param[in]   dirname  Ŀ¼��
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_opendir_r (struct aw_dir *dirp, const char *dirname);

/**
 * \brief ��Ŀ¼
 *
 * \param[in]   dirname  Ŀ¼��
 *
 * \return �ɹ�����Ŀ¼ָ�룬���򷵻�NULL
 */
struct aw_dir *aw_opendir(const char *dirname);

/**
 * \brief �ر�Ŀ¼
 *
 * \param[in]   dirp  Ŀ¼ָ��
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_closedir(struct aw_dir *dirp);

/**
 * \brief ��Ŀ¼���
 *
 * \param[in]    dirp    Ŀ¼ָ��
 * \param[in]    entry   Ŀ¼���ָ��
 * \param[out]   result  ��һ��Ŀ¼���ָ��
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_readdir_r (struct aw_dir     *dirp,
                  struct aw_dirent  *entry,
                  struct aw_dirent **result);

/**
 * \brief ��Ŀ¼���
 *
 * \param[in]    dirp    Ŀ¼ָ��
 *
 * \return �ɹ�����Ŀ¼���ָ�룬���򷵻�NULL��
 */
struct aw_dirent *aw_readdir (struct aw_dir *dirp);


/**
 * \brief Ŀ¼��������ʼ��
 *
 * \param[in]    dirp      Ŀ¼ָ��
 * \param[in]    dir_num   Ŀ¼����
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
void aw_dir_init (struct aw_dir *dirp, unsigned int dir_num);

/** @} grp_iosys_dirent */

#ifdef __cplusplus
}
#endif

#endif /* } __IO_DIRENT_H */

/* end of file */
