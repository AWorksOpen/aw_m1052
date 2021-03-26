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


/** \brief ����дִ��Ȩ�� */
#define O_ACCMODE   (O_RDONLY | O_WRONLY | O_RDWR)

#define O_RDONLY    0x0000  /**< \brief ֻ�� */
#define O_WRONLY    0x0001  /**< \brief ֻд */
#define O_RDWR      0x0002  /**< \brief �ɶ�д */
#define O_APPEND    0x0008  /**< \brief ׷�� */

#define O_EXCL      0x0100  /**< \brief ��ִ�� */
#define O_CREAT     0x0200  /**< \brief ���� */
#define O_TRUNC     0x0400  /**< \brief �ض� */
#define O_DIRECTORY 0x0800  /**< \brief Ŀ¼  */

/**
 * \brief �����ļ�
 *
 * \param[in]    path      �ļ���
 * \param[in]    mode      �ļ�Ȩ��
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_create (const char *path, mode_t mode);

/**
 * \brief ���ļ�
 *
 * \param[in]    path      �ļ���
 * \param[in]    oflag     �򿪱�־
 * \param[in]    mode      ��ģʽ
 *
 * \return �ɹ������ļ���������򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_open (const char *path, int oflag, mode_t mode);

/**
 * \brief �ļ����ƺ���
 *
 * \param[in]        fildes      �ļ�������
 * \param[in]        cmd         ����
 * \param[in,out]    arg         ����
 *
 * \return �ɹ�����AW_OK�����򷵻�ֵΪ������룬�� aw_errno.h ��
 */
int aw_fcntl  (int fildes, int cmd, void *arg);


/** @} grp_iosys_fcntl */

#ifdef __cplusplus
}
#endif


#endif /* } __IO_FCNTL_H */

/* end of file */
