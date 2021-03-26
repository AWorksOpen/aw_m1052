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
 * - 1.02 15-04-30 deo, ����aw_ftruncate,aw_fsync
 * - 1.01 15-04-17 deo, ����aw_truncate
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
 * \brief seek�ṹ��
 */
struct aw_seek_struct {
    int   whence;          /**< \brief seek��ģʽ */
    off_t offset;          /**< \brief seek��ƫ�� */
};


/**
 * \brief �ر��ļ�
 *
 * \param[in]         filedes     �ļ�������
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_close (int filedes);

/**
 * \brief ���ļ�
 *
 * \param[in]         filedes     �ļ�������
 * \param[out]        buf         ����
 * \param[in]         nbyte       �ֽ���
 *
 * \return �ɹ����ض�ȡ�����ֽ��������򷵻�ֵΪ������룬�� aw_errno.h ��
 */
ssize_t aw_read  (int filedes, void *buf, size_t nbyte);

/**
 * \brief д�ļ�
 *
 * \param[in]         filedes     �ļ�������
 * \param[in]         buf         ����
 * \param[in]         nbyte       �ֽ���
 *
 * \return �ɹ�����д����ֽ��������򷵻�ֵΪ������룬�� aw_errno.h ��
 */
ssize_t aw_write (int filedes, const void *buf, size_t nbyte);


/**
 * \brief �����ļ���д��λ��
 *
 * \param[in]         filedes     �ļ�������
 * \param[in]         offset      ƫ��
 * \param[in]         whence      seek��ģʽ
 *
 * \return �ɹ�����ƫ���������򷵻�ֵΪ������룬�� aw_errno.h ��
 */
off_t aw_lseek (int fildes, off_t offset, int whence);

/**
 * \brief ɾ��һ��Ŀ¼
 *
 * \param[in]         path     Ŀ¼·��
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_rmdir  (const char *path);

/**
 * \brief ɾ��һ������
 *
 * \param[in]         path     ·��
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_unlink (const char *path);

/**
 * \brief ������
 *
 * \param[in]         oldpath     ��·��
 * \param[in]         newpath     ��·��
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_rename (const char *oldpath, const char *newpath);

/**
 * \brief �ļ��ض�
 *
 * \param[in]         path        ·��
 * \param[in]         length      �ضϳ���
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_truncate (const char *path, off_t length);

/**
 * \brief �ļ��ض�
 *
 * \param[in]         fildes      �ļ�������
 * \param[in]         length      �ضϳ���
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_ftruncate(int fildes, off_t length);

/**
 * \brief ��ȡ��ǰ·��
 *
 * \param[out]         buf      ����
 * \param[in]          size     �����С
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
char *aw_getcwd (char *buf, size_t size);

/**
 * \brief �ı䵱ǰ·��
 *
 * \param[in]          path     ·����
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_chdir (const char *path);

/**
 * \brief �ļ�ͬ��
 *
 * \param[in]          fildes     �ļ�������
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_fsync(int fildes);


/** @} grp_iosys_unistd */

#ifdef __cplusplus
}
#endif


#endif /* } __IO_UNISTD_H */

/* end of file */
