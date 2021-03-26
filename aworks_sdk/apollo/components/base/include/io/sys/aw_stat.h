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
 * - 1.01 15-04-17 deo, ���� aw_chmod
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
 * \brief �ļ�ģʽ(st_mode)λ����
 */
#define S_IFMT   0xf000 /**< \brief �ļ����͵��ֶ�   */
#define S_IFIFO  0x1000 /**< \brief �Ƚ��ȳ�(fifo) */
#define S_IFCHR  0x2000 /**< \brief �ַ��豸 */
#define S_IFDIR  0x4000 /**< \brief Ŀ¼ */
#define S_IFBLK  0x6000 /**< \brief ���豸 */
#define S_IFREG  0x8000 /**< \brief һ���ļ� */
#define S_IFLNK  0xa000 /**< \brief ��������  */
#define S_IFSHM  0xb000 /**< \brief �����ڴ� */
#define S_IFSOCK 0xc000 /**< \brief socket */

#define S_ISUID  04000  /**< \brief �û�idλ */
#define S_ISGID  02000  /**< \brief �û���idλ */

#define S_IRWXU  00700  /**< \brief �ļ������߾��ж���д����ִ��Ȩ�� */
#define S_IRUSR  00400  /**< \brief �ļ������߾��ж�Ȩ�� */
#define S_IWUSR  00200  /**< \brief �ļ������߾���дȨ�� */
#define S_IXUSR  00100  /**< \brief �ļ������߾��п�ִ��Ȩ�� */

#define S_IRWXG  00070  /**< \brief �û�����ж���д����ִ��Ȩ�� */
#define S_IRGRP  00040  /**< \brief �û�����ж�Ȩ�� */
#define S_IWGRP  00020  /**< \brief �û������дȨ�� */
#define S_IXGRP  00010  /**< \brief �û�����п�ִ��Ȩ�� */

#define S_IRWXO  00007  /**< \brief �����û����ж���д����ִ��Ȩ�� */
#define S_IROTH  00004  /**< \brief �����û����ж�Ȩ�� */
#define S_IWOTH  00002  /**< \brief �����û�����дȨ�� */
#define S_IXOTH  00001  /**< \brief �����û����п�ִ��Ȩ�� */

/******************************************************************************/
/**
 * \brief �ļ����͵��жϺ�
 */
#define S_ISBLK(m)  ((m & S_IFMT) == S_IFBLK)   /**< \brief �Ƿ�Ϊ���豸 */
#define S_ISCHR(m)  ((m & S_IFMT) == S_IFCHR)   /**< \brief �Ƿ�Ϊ�ַ��豸 */
#define S_ISDIR(m)  ((m & S_IFMT) == S_IFDIR)   /**< \brief �Ƿ���Ŀ¼ */
#define S_ISFIFO(m) ((m & S_IFMT) == S_IFIFO)   /**< \brief �Ƿ����Ƚ��ȳ�  */
#define S_ISREG(m)  ((m & S_IFMT) == S_IFREG)   /**< \brief �Ƿ���һ���ļ� */
#define S_ISLNK(m)  ((m & S_IFMT) == S_IFLNK)   /**< \brief �Ƿ��Ƿ������� */
#define S_ISSOCK(m) ((m & S_IFMT) == S_IFSOCK)  /**< \brief �Ƿ�Ϊsocket */

/**
 * \brief POSIX.1b objects.
 * ��Щ�����ڶԲ��������﷨���
 */
#define S_TYPEISMQ(buf)     ((buf)->st_mode - (buf)->st_mode)
#define S_TYPEISSEM(buf)    ((buf)->st_mode - (buf)->st_mode)
#define S_TYPEISSHM(buf)    (((buf)->st_mode & S_IFMT) == S_IFSHM)

/******************************************************************************/

/**
 * \brief stat �ṹ��
 */
struct aw_stat {
    dev_t   st_dev;     /**< \brief �ļ�ϵͳ�豸 */
    ino_t   st_ino;     /**< \brief �ļ����к� */
    mode_t  st_mode;    /**< \brief �ļ����ͺ�Ȩ����Ϣ */
    nlink_t st_nlink;   /**< \brief �ļ��ķ����������� */
    uid_t   st_uid;     /**< \brief �û�id */
    gid_t   st_gid;     /**< \brief ��id */
    dev_t   st_rdev;    /**< \brief �豸id */
    off_t   st_size;    /**< \brief �ļ����ȣ���λbyte */

    struct timespec st_atim; /**< \brief �������ļ���ʱ�� */
    struct timespec st_mtim; /**< \brief ����޸��ļ����ݵ�ʱ�� */
    struct timespec st_ctim; /**< \brief ����޸��ļ�״̬��ʱ�� */

    /** \brief �Ƽ��� I/O ���С */
    blksize_t st_blksize;
    /** \brief �������ӵ�еĿ����� */
    blkcnt_t  st_blocks;
};

/**
 * \brief ��ȡ�ļ�״̬
 *
 * \param[in]   fildes  �ļ�������
 * \param[out]  buf     ����
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_fstat (int  fildes, struct aw_stat *buf);

/**
 * \brief ��ȡ�ļ�״̬
 *
 * \param[in]   path    �ļ�����·��
 * \param[out]  buf     ����
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_stat  (const char *path, struct aw_stat *buf);

/**
 * \brief ����Ŀ¼
 *
 * \param[in]   path  Ŀ¼����·��
 * \param[out]  mode  ģʽ
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_mkdir (const char *path, mode_t mode);

/**
 * \brief �޸��ļ���ģʽ
 *
 * \param[in]   path  �ļ�����·��
 * \param[out]  mode  ģʽ
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_chmod(const char *path, mode_t mode);

/** @} grp_ios_stat */

#ifdef __cplusplus
}
#endif


#endif /* } __IO_SYS_STAT_H */

/* end of file */
