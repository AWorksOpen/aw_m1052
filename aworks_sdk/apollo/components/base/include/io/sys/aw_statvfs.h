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
 * \brief VFS �ļ�ϵͳ��Ϣ.
 *
 * \internal
 * \par modification history:
 * - 1.01 15-04-17 deo, �޸�struct statvfsΪstruct aw_statvfs
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


#define ST_RDONLY   0x01    /**< \brief ֻ�� */

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
 * \brief VFS �ṹ��
 */
struct aw_statvfs {
    unsigned long f_bsize;  /**< \brief ���С */
    unsigned long f_frsize; /**< \brief �ļ�ϵͳ�������С */

    fsblkcnt_t    f_blocks; /**< \brief ������ */
    fsblkcnt_t    f_bfree;  /**< \brief ���п����� */
    fsblkcnt_t    f_bavail; /**< \brief ���ÿ����� */

    fsfilcnt_t    f_files;  /**< \brief �ļ����к����� */
    fsfilcnt_t    f_ffree;  /**< \brief ���е��ļ����к����� */
    fsfilcnt_t    f_favail; /**< \brief ���õ��ļ����к����� */

    unsigned long f_fsid;   /**< \brief �ļ�ϵͳID */
    unsigned long f_flag;   /**< \brief λ���� */
    unsigned long f_namemax;/**< \brief ����ļ������� */
};

/**
 * \brief ��ȡ�ļ�״̬
 *
 * \param[in]   fildes  �ļ�������
 * \param[out]  buf     ����
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_fstatvfs (int fildes, struct aw_statvfs *buf);

/**
 * \brief ��ȡ�ļ�״̬
 *
 * \param[in]   fildes  �ļ�������
 * \param[out]  buf     ����
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_statvfs (const char *path, struct aw_statvfs *buf);

/** @} grp_ios_statvfs */

#ifdef __cplusplus
}
#endif

#endif /* } __IO_SYS_STATVFS_H */

/* end of file */
