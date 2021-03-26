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

#ifndef __POSIX_ADAPTER_INTERNAL_H
#define __POSIX_ADAPTER_INTERNAL_H

#include "aw_errno.h"

/******************************************************************************/
/**
 * \addtogroup grp_aw_if_posix_err ϵͳ�������ֵ��POSIX���ݣ�
 * @{
 */

/**
 * \name POSIX����ֵ
 * @{
 */
#define EPERM         AW_EPERM              /**< \brief ���������� */
#define ENOENT        AW_ENOENT             /**< \brief �ļ���Ŀ¼������ */
#define ESRCH         AW_ESRCH              /**< \brief ���̲����� */
#define EINTR         AW_EINTR              /**< \brief ���ñ��ж� */
#define EIO           AW_EIO                /**< \brief I/O ���� */
#define ENXIO         AW_ENXIO              /**< \brief �豸���ַ������ */
#define E2BIG         AW_E2BIG              /**< \brief �����б�̫�� */
#define ENOEXEC       AW_ENOEXEC            /**< \brief ��ִ���ļ���ʽ���� */
#define EBADF         AW_EBADF              /**< \brief �ļ��������� */
#define ECHILD        AW_ECHILD             /**< \brief û���ӽ��� */
#define EAGAIN        AW_EAGAIN             /**< \brief ��Դ�����ã������� */
#define ENOMEM        AW_ENOMEM             /**< \brief �ռ䣨�ڴ棩���� */
#define EACCES        AW_EACCES             /**< \brief Ȩ�޲��� */
#define EFAULT        AW_EFAULT             /**< \brief ��ַ���� */
#define ENOTEMPTY     AW_ENOTEMPTY          /**< \brief Ŀ¼�ǿ� */
#define EBUSY         AW_EBUSY              /**< \brief �豸����Դæ */
#define EEXIST        AW_EEXIST             /**< \brief �ļ��Ѿ����� */
#define EXDEV         AW_EXDEV              /**< \brief ���豸���� */
#define ENODEV        AW_ENODEV             /**< \brief �豸������ */
#define ENOTDIR       AW_ENOTDIR            /**< \brief ����Ŀ¼ */
#define EISDIR        AW_EISDIR             /**< \brief ��Ŀ¼ */
#define EINVAL        AW_EINVAL             /**< \brief ��Ч���� */
#define ENFILE        AW_ENFILE             /**< \brief ϵͳ���ļ�̫�࣬������������ */
#define EMFILE        AW_EMFILE             /**< \brief �򿪵��ļ�̫�� */
#define ENOTTY        AW_ENOTTY             /**< \brief �����ʵ�I/O���Ʋ��� */
#define ENAMETOOLONG  AW_ENAMETOOLONG       /**< \brief �ļ���̫�� */
#define EFBIG         AW_EFBIG              /**< \brief �ļ�̫�� */
#define ENOSPC        AW_ENOSPC             /**< \brief �豸ʣ��ռ䲻�� */
#define ESPIPE        AW_ESPIPE             /**< \brief ��Ч��������Invalid seek�� */
#define EROFS         AW_EROFS              /**< \brief �ļ�ϵͳֻ�� */
#define EMLINK        AW_EMLINK             /**< \brief ����̫�� */
#define EPIPE         AW_EPIPE              /**< \brief �𻵵Ĺܵ� */
#define EDEADLK       AW_EDEADLK            /**< \brief ��Դ�������� */
#define ENOLCK        AW_ENOLCK             /**< \brief �޿��ã����У����� */
#define ENOTSUP       AW_ENOTSUP            /**< \brief ��֧�� */
#define EMSGSIZE      AW_EMSGSIZE           /**< \brief ��Ϣ̫�� */
/** @} */

/**
 * \name ANSI����ֵ
 * @{
 */
#define EDOM          AW_EDOM               /**< \brief ��ѧ��������������Χ */
#define ERANGE        AW_ERANGE             /**< \brief ��ѧ����������̫�� */
#define EILSEQ        AW_EILSEQ             /**< \brief �Ƿ����ֽ�˳�� */
/** @} */

/**
 * \name ��������������
 * @{
 */
#define EDESTADDRREQ    AW_EDESTADDRREQ       /**< \brief ��ҪĿ���ַ */
#define EPROTOTYPE      AW_EPROTOTYPE         /**< \brief socketЭ�����ʹ��� */
#define ENOPROTOOPT     AW_ENOPROTOOPT        /**< \brief Э�鲻���� */
#define EPROTONOSUPPORT AW_EPROTONOSUPPORT    /**< \brief Э�鲻֧�� */
#define ESOCKTNOSUPPORT AW_ESOCKTNOSUPPORT    /**< \brief Socket���Ͳ�֧�� */
#define EOPNOTSUPP      AW_EOPNOTSUPP         /**< \brief socket��֧�ָò��� */
#define EPFNOSUPPORT    AW_EPFNOSUPPORT       /**< \brief Э���岻֧�� */
#define EAFNOSUPPORT    AW_EAFNOSUPPORT       /**< \brief ��ַ�ز�֧�� */
#define EADDRINUSE      AW_EADDRINUSE         /**< \brief ��ַ�Ѿ���ռ�� */
#define EADDRNOTAVAIL   AW_EADDRNOTAVAIL      /**< \brief ��ַ������ */
#define ENOTSOCK        AW_ENOTSOCK           /**< \brief ������������socket */
/** @} */

/**
 * \name ��ѡ��ʵ�ֵĴ���ֵ
 * @{
 */
#define ENETUNREACH    AW_ENETUNREACH         /**< \brief ���粻�ɴ� */
#define ENETRESET      AW_ENETRESET           /**< \brief �����ж������� */
#define ECONNABORTED   AW_ECONNABORTED        /**< \brief �����ж� */
#define ECONNRESET     AW_ECONNRESET          /**< \brief ���Ӹ�λ */
#define ENOBUFS        AW_ENOBUFS             /**< \brief ����ռ䲻�� */
#define EISCONN        AW_EISCONN             /**< \brief Socket�Ѿ����� */
#define ENOTCONN       AW_ENOTCONN            /**< \brief Socketû������ */
#define ESHUTDOWN      AW_ESHUTDOWN           /**< \brief Socket�Ѿ��رգ����ܷ������� */
#define ETOOMANYREFS   AW_ETOOMANYREFS        /**< \brief ����̫�࣬�޷�ƴ�� */
#define ETIMEDOUT      AW_ETIMEDOUT           /**< \brief ���ӳ�ʱ */
#define ECONNREFUSED   AW_ECONNREFUSED        /**< \brief ���ӱ��ܾ� */
#define ENETDOWN       AW_ENETDOWN            /**< \brief �����Ѿ�ֹͣ */
#define ETXTBSY        AW_ETXTBSY             /**< \brief �ı��ļ�æ */
#define ELOOP          AW_ELOOP               /**< \brief �������Ӽ���̫�� */
#define EHOSTUNREACH   AW_EHOSTUNREACH        /**< \brief �������ɴ� */
#define ENOTBLK        AW_ENOTBLK             /**< \brief �ǿ��豸 */
#define EHOSTDOWN      AW_EHOSTDOWN           /**< \brief �����Ѿ��ر� */
/** @} */

/**
 * \name ���������ж�I/O����ֵ
 * @{
 */
#define EINPROGRESS   AW_EINPROGRESS         /**< \brief �������ڽ����� */
#define EALREADY      AW_EALREADY            /**< \brief ��������ʹ���� */
/* 70 */
#define EWOULDBLOCK   AW_EWOULDBLOCK         /**< \brief ������������ͬEAGAIN�� */
#define ENOSYS        AW_ENOSYS              /**< \brief��֧�ֵĹ��ܣ�����δʵ�֣�*/
/** @} */

/**
 * \name �첽I/O����ֵ
 * @{
 */
#define ECANCELED     AW_ECANCELED           /**< \brief �����Ѿ�ȡ�� */
/* 73 */
/** @} */

/**
 * \name ����ش���ֵ
 * @{
 */
#define ENOSR       AW_ENOSR               /**< \brief û������Դ */
#define ENOSTR      AW_ENOSTR              /**< \brief �������豸 */
#define EPROTO      AW_EPROTO              /**< \brief Э����� */
#define EBADMSG     AW_EBADMSG             /**< \brief �𻵵���Ϣ */
#define ENODATA     AW_ENODATA             /**< \brief ���������� */
#define ETIME       AW_ETIME               /**< \brief ��ioctl()��ʱ */
#define ENOMSG      AW_ENOMSG              /**< \brief û���������Ϣ���� */
#define EUCLEAN     AW_EUCLEAN             /**< \brief Structure��Ҫ���� */
/** @} */

#define ECOMM       AW_ECOMM               /**< \brief Communication error on send */
#define EOVERFLOW   AW_EOVERFLOW           /**< \brief Value too large for defined data type */
#define EREMOTEIO   AW_EREMOTEIO           /**< \brief Remote I/O error */

extern int *_aw_get_errno_addr(void);

#define errno (*_aw_get_errno_addr())

#endif  /* __POSIX_ADAPTER_INTERNAL_H */

/* end of file */
