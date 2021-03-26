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
 * \brief ϵͳ����ֵ
 *
 * ʹ�ñ�������Ҫ��������ͷ�ļ���
 * \code
 * #include "aw_errno.h"
 * \endcode
 *
 * - ���е�ϵͳ����ֵ�������ﶨ�塣û�д��󷵻�0�����򷵻ض�Ӧ����ֵ�ĸ�ֵ��
 *   Ϊ�˼���POSIX�����ﶨ��������POSIX����ֵ��
 * - Apolloƽ̨ʹ�õĴ���ֵ����POSIX����ֵǰ�����AW_ǰ׺��
 *   ������Ҫʹ�ô���ֵ���ļ���Ӧ�ð������ļ���Apolloƽ̨�������Ӧ��ʹ��AW_
 *   ǰ׺�Ĵ���ֵ��������������ʹ����POSIX����ֵ����ֲʱ����ֱ��ʹ�ñ��ļ���
 * - Ϊ����ߴ�������ԣ���Ӧ��ʹ�ñ��ļ�֮�����ֵ��
 *
 * \internal
 * \par modification history:
 * - 1.10 14-01-17  zen, move errno code to platform
 * - 1.00 12-09-04  orz, first implementation.
 * \endinternal
 */

#ifndef __AW_ERRNO_H
#define __AW_ERRNO_H

#include "aw_psp_errno.h"

/**
 * \addtogroup grp_aw_if_errno
 * \copydoc aw_errno.h
 * @{
 */

/**
 * \name ���������������
 * @{
 */
typedef aw_psp_error_t     error_t;    /**< \brief ����POSIX�������� */
typedef int                aw_err_t;   /**< \brief AWorks�������� */
/** @} */

/**
 * \name AWorksƽ̨�������ӿڣ���POSIX���ݣ�
 * @{
 */

/**
 * \brief ����errno
 * \note ���������������ڼ���֧��errno�Ͳ�֧��errno��ϵͳ����ϵͳ��֧��errno
 *      ʱ������Ǹ��ն��塣
 * \par ʹ��ʾ��
 * \code
 * AW_ERRNO_SET(ENODEV); // �൱�ڵ��ã�errno = ENODEV;
 * \endcode
 */
#define AW_ERRNO_SET(err)   AW_PSP_ERRNO_SET(err)

/**
 * \brief ��ȡerrno
 * \note
 *  - ���������������ڼ���֧��errno�Ͳ�֧��errno��ϵͳ����ϵͳ��֧��errno
 *    ʱ������Ǹ��ն��塣
 *  - ��ϵͳ��֧��errnoʱ��ʹ��������ȡ������ֵ�����壬��д����ʱӦע�⡣
 * \par ʹ��ʾ��
 * \code
 * AW_ERRNO_GET(err); // �൱�ڵ��ã�err = errno;
 * if (err) {
 *     ;              // ������
 * }
 * \endcode
 */
#define AW_ERRNO_GET(err)   AW_PSP_ERRNO_GET(err)
/* @} */

/* ����ĵĴ�������Ҫ��ƽ̨�ṩ */
#if 1

/**
 * \addtogroup grp_aw_if_posix_err ϵͳ�������ֵ��POSIX���ݣ�
 * @{
 */

/**
 * \name POSIX����ֵ
 * @{
 */
#define AW_EPERM           1            /* ���������� */
#define AW_ENOENT          2            /* �ļ���Ŀ¼������ */
#define AW_ESRCH           3            /* ���̲����� */
#define AW_EINTR           4            /* ���ñ��ж� */
#define AW_EIO             5            /* I/O ���� */
#define AW_ENXIO           6            /* �豸���ַ������ */
#define AW_E2BIG           7            /* �����б�̫�� */
#define AW_ENOEXEC         8            /* ��ִ���ļ���ʽ���� */
#define AW_EBADF           9            /* �ļ��������� */
#define AW_ECHILD          10           /* û���ӽ��� */
#define AW_EAGAIN          11           /* ��Դ�����ã������� */
#define AW_ENOMEM          12           /* �ռ䣨�ڴ棩���� */
#define AW_EACCES          13           /* Ȩ�޲��� */
#define AW_EFAULT          14           /* ��ַ���� */
#define AW_ENOTEMPTY       15           /* Ŀ¼�ǿ� */
#define AW_EBUSY           16           /* �豸����Դæ */
#define AW_EEXIST          17           /* �ļ��Ѿ����� */
#define AW_EXDEV           18           /* ���豸���� */
#define AW_ENODEV          19           /* �豸������ */
#define AW_ENOTDIR         20           /* ����Ŀ¼ */
#define AW_EISDIR          21           /* ��Ŀ¼ */
#define AW_EINVAL          22           /* ��Ч���� */
#define AW_ENFILE          23           /* ϵͳ���ļ�̫�࣬������������ */
#define AW_EMFILE          24           /* �򿪵��ļ�̫�� */
#define AW_ENOTTY          25           /* �����ʵ�I/O���Ʋ��� */
#define AW_ENAMETOOLONG    26           /* �ļ���̫�� */
#define AW_EFBIG           27           /* �ļ�̫�� */
#define AW_ENOSPC          28           /* �豸ʣ��ռ䲻�� */
#define AW_ESPIPE          29           /* ��Ч��������Invalid seek�� */
#define AW_EROFS           30           /* �ļ�ϵͳֻ�� */
#define AW_EMLINK          31           /* ����̫�� */
#define AW_EPIPE           32           /* �𻵵Ĺܵ� */
#define AW_EDEADLK         33           /* ��Դ�������� */
#define AW_ENOLCK          34           /* �޿��ã����У����� */
#define AW_ENOTSUP         35           /* ��֧�� */
#define AW_EMSGSIZE        36           /* ��Ϣ̫�� */
/** @} */

/**
 * \name ANSI����ֵ
 * @{
 */
#define AW_EDOM            37           /* ��ѧ��������������Χ */
#define AW_ERANGE          38           /* ��ѧ����������̫�� */
#define AW_EILSEQ          39           /* �Ƿ����ֽ�˳�� */
/** @} */

/**
 * \name ��������������
 * @{
 */
#define AW_EDESTADDRREQ    40           /* ��ҪĿ���ַ */
#define AW_EPROTOTYPE      41           /* socketЭ�����ʹ��� */
#define AW_ENOPROTOOPT     42           /* Э�鲻���� */
#define AW_EPROTONOSUPPORT 43           /* Э�鲻֧�� */
#define AW_ESOCKTNOSUPPORT 44           /* Socket���Ͳ�֧�� */
#define AW_EOPNOTSUPP      45           /* socket��֧�ָò��� */
#define AW_EPFNOSUPPORT    46           /* Э���岻֧�� */
#define AW_EAFNOSUPPORT    47           /* ��ַ�ز�֧�� */
#define AW_EADDRINUSE      48           /* ��ַ�Ѿ���ռ�� */
#define AW_EADDRNOTAVAIL   49           /* ��ַ������ */
#define AW_ENOTSOCK        50           /* ������������socket */
/** @} */

/**
 * \name ��ѡ��ʵ�ֵĴ���ֵ
 * @{
 */
#define AW_ENETUNREACH     51           /* ���粻�ɴ� */
#define AW_ENETRESET       52           /* �����ж������� */
#define AW_ECONNABORTED    53           /* �����ж� */
#define AW_ECONNRESET      54           /* ���Ӹ�λ */
#define AW_ENOBUFS         55           /* ����ռ䲻�� */
#define AW_EISCONN         56           /* Socket�Ѿ����� */
#define AW_ENOTCONN        57           /* Socketû������ */
#define AW_ESHUTDOWN       58           /* Socket�Ѿ��رգ����ܷ������� */
#define AW_ETOOMANYREFS    59           /* ����̫�࣬�޷�ƴ�� */
#define AW_ETIMEDOUT       60           /* ���ӳ�ʱ */
#define AW_ECONNREFUSED    61           /* ���ӱ��ܾ� */
#define AW_ENETDOWN        62           /* �����Ѿ�ֹͣ */
#define AW_ETXTBSY         63           /* �ı��ļ�æ */
#define AW_ELOOP           64           /* �������Ӽ���̫�� */
#define AW_EHOSTUNREACH    65           /* �������ɴ� */
#define AW_ENOTBLK         66           /* �ǿ��豸 */
#define AW_EHOSTDOWN       67           /* �����Ѿ��ر� */
/** @} */

/**
 * \name ���������ж�I/O����ֵ
 * @{
 */
#define AW_EINPROGRESS     68           /* �������ڽ����� */
#define AW_EALREADY        69           /* ��������ʹ���� */
/* 70 */
#define AW_EWOULDBLOCK     AW_EAGAIN    /* ������������ͬEAGAIN�� */
#define AW_ENOSYS          71           /* ��֧�ֵĹ��ܣ�����δʵ�֣�*/
/** @} */

/**
 * \name �첽I/O����ֵ
 * @{
 */
#define AW_ECANCELED       72           /* �����Ѿ�ȡ�� */
/* 73 */
/** @} */

/**
 * \name ����ش���ֵ
 * @{
 */
#define AW_ENOSR           74           /* û������Դ */
#define AW_ENOSTR          75           /* �������豸 */
#define AW_EPROTO          76           /* Э����� */
#define AW_EBADMSG         77           /* �𻵵���Ϣ */
#define AW_ENODATA         78           /* ���������� */
#define AW_ETIME           79           /* ��ioctl()��ʱ */
#define AW_ENOMSG          80           /* û���������Ϣ���� */
#define AW_EUCLEAN         81           /* Structure��Ҫ���� */
/** @} */

#define AW_ECOMM           109          /* Communication error on send */
#define AW_EOVERFLOW       112          /* Value too large for defined data type */
#define AW_EREMOTEIO       121          /* Remote I/O error */

/** @} grp_aw_if_posix_err */

#endif  /* ... */

/** @} grp_aw_if_errno */

#endif /* __AW_ERRNO_H */

/* end of file */
