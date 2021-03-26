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
 * \addtogroup grp_aw_if_posix_err 系统错误编码值（POSIX兼容）
 * @{
 */

/**
 * \name POSIX错误值
 * @{
 */
#define EPERM         AW_EPERM              /**< \brief 操作不允许 */
#define ENOENT        AW_ENOENT             /**< \brief 文件或目录不存在 */
#define ESRCH         AW_ESRCH              /**< \brief 进程不存在 */
#define EINTR         AW_EINTR              /**< \brief 调用被中断 */
#define EIO           AW_EIO                /**< \brief I/O 错误 */
#define ENXIO         AW_ENXIO              /**< \brief 设备或地址不存在 */
#define E2BIG         AW_E2BIG              /**< \brief 参数列表太长 */
#define ENOEXEC       AW_ENOEXEC            /**< \brief 可执行文件格式错误 */
#define EBADF         AW_EBADF              /**< \brief 文件描述符损坏 */
#define ECHILD        AW_ECHILD             /**< \brief 没有子进程 */
#define EAGAIN        AW_EAGAIN             /**< \brief 资源不可用，需重试 */
#define ENOMEM        AW_ENOMEM             /**< \brief 空间（内存）不足 */
#define EACCES        AW_EACCES             /**< \brief 权限不够 */
#define EFAULT        AW_EFAULT             /**< \brief 地址错误 */
#define ENOTEMPTY     AW_ENOTEMPTY          /**< \brief 目录非空 */
#define EBUSY         AW_EBUSY              /**< \brief 设备或资源忙 */
#define EEXIST        AW_EEXIST             /**< \brief 文件已经存在 */
#define EXDEV         AW_EXDEV              /**< \brief 跨设备连接 */
#define ENODEV        AW_ENODEV             /**< \brief 设备不存在 */
#define ENOTDIR       AW_ENOTDIR            /**< \brief 不是目录 */
#define EISDIR        AW_EISDIR             /**< \brief 是目录 */
#define EINVAL        AW_EINVAL             /**< \brief 无效参数 */
#define ENFILE        AW_ENFILE             /**< \brief 系统打开文件太多，描述符不够用 */
#define EMFILE        AW_EMFILE             /**< \brief 打开的文件太多 */
#define ENOTTY        AW_ENOTTY             /**< \brief 不合适的I/O控制操作 */
#define ENAMETOOLONG  AW_ENAMETOOLONG       /**< \brief 文件名太长 */
#define EFBIG         AW_EFBIG              /**< \brief 文件太大 */
#define ENOSPC        AW_ENOSPC             /**< \brief 设备剩余空间不足 */
#define ESPIPE        AW_ESPIPE             /**< \brief 无效的搜索（Invalid seek） */
#define EROFS         AW_EROFS              /**< \brief 文件系统只读 */
#define EMLINK        AW_EMLINK             /**< \brief 链接太多 */
#define EPIPE         AW_EPIPE              /**< \brief 损坏的管道 */
#define EDEADLK       AW_EDEADLK            /**< \brief 资源可能死锁 */
#define ENOLCK        AW_ENOLCK             /**< \brief 无可用（空闲）的锁 */
#define ENOTSUP       AW_ENOTSUP            /**< \brief 不支持 */
#define EMSGSIZE      AW_EMSGSIZE           /**< \brief 消息太大 */
/** @} */

/**
 * \name ANSI错误值
 * @{
 */
#define EDOM          AW_EDOM               /**< \brief 数学函数参数超出范围 */
#define ERANGE        AW_ERANGE             /**< \brief 数学函数计算结果太大 */
#define EILSEQ        AW_EILSEQ             /**< \brief 非法的字节顺序 */
/** @} */

/**
 * \name 网络参数错误相关
 * @{
 */
#define EDESTADDRREQ    AW_EDESTADDRREQ       /**< \brief 需要目标地址 */
#define EPROTOTYPE      AW_EPROTOTYPE         /**< \brief socket协议类型错误 */
#define ENOPROTOOPT     AW_ENOPROTOOPT        /**< \brief 协议不可用 */
#define EPROTONOSUPPORT AW_EPROTONOSUPPORT    /**< \brief 协议不支持 */
#define ESOCKTNOSUPPORT AW_ESOCKTNOSUPPORT    /**< \brief Socket类型不支持 */
#define EOPNOTSUPP      AW_EOPNOTSUPP         /**< \brief socket不支持该操作 */
#define EPFNOSUPPORT    AW_EPFNOSUPPORT       /**< \brief 协议族不支持 */
#define EAFNOSUPPORT    AW_EAFNOSUPPORT       /**< \brief 地址簇不支持 */
#define EADDRINUSE      AW_EADDRINUSE         /**< \brief 地址已经被占用 */
#define EADDRNOTAVAIL   AW_EADDRNOTAVAIL      /**< \brief 地址不可用 */
#define ENOTSOCK        AW_ENOTSOCK           /**< \brief 被操作对象不是socket */
/** @} */

/**
 * \name 可选择实现的错误值
 * @{
 */
#define ENETUNREACH    AW_ENETUNREACH         /**< \brief 网络不可达 */
#define ENETRESET      AW_ENETRESET           /**< \brief 网络中断了连接 */
#define ECONNABORTED   AW_ECONNABORTED        /**< \brief 连接中断 */
#define ECONNRESET     AW_ECONNRESET          /**< \brief 连接复位 */
#define ENOBUFS        AW_ENOBUFS             /**< \brief 缓冲空间不足 */
#define EISCONN        AW_EISCONN             /**< \brief Socket已经连接 */
#define ENOTCONN       AW_ENOTCONN            /**< \brief Socket没有连接 */
#define ESHUTDOWN      AW_ESHUTDOWN           /**< \brief Socket已经关闭，不能发送数据 */
#define ETOOMANYREFS   AW_ETOOMANYREFS        /**< \brief 引用太多，无法拼接 */
#define ETIMEDOUT      AW_ETIMEDOUT           /**< \brief 连接超时 */
#define ECONNREFUSED   AW_ECONNREFUSED        /**< \brief 连接被拒绝 */
#define ENETDOWN       AW_ENETDOWN            /**< \brief 网络已经停止 */
#define ETXTBSY        AW_ETXTBSY             /**< \brief 文本文件忙 */
#define ELOOP          AW_ELOOP               /**< \brief 符号链接级数太多 */
#define EHOSTUNREACH   AW_EHOSTUNREACH        /**< \brief 主机不可达 */
#define ENOTBLK        AW_ENOTBLK             /**< \brief 非块设备 */
#define EHOSTDOWN      AW_EHOSTDOWN           /**< \brief 主机已经关闭 */
/** @} */

/**
 * \name 非阻塞和中断I/O错误值
 * @{
 */
#define EINPROGRESS   AW_EINPROGRESS         /**< \brief 操作正在进行中 */
#define EALREADY      AW_EALREADY            /**< \brief 连接正被使用中 */
/* 70 */
#define EWOULDBLOCK   AW_EWOULDBLOCK         /**< \brief 操作会阻塞（同EAGAIN） */
#define ENOSYS        AW_ENOSYS              /**< \brief不支持的功能（功能未实现）*/
/** @} */

/**
 * \name 异步I/O错误值
 * @{
 */
#define ECANCELED     AW_ECANCELED           /**< \brief 操作已经取消 */
/* 73 */
/** @} */

/**
 * \name 流相关错误值
 * @{
 */
#define ENOSR       AW_ENOSR               /**< \brief 没有流资源 */
#define ENOSTR      AW_ENOSTR              /**< \brief 不是流设备 */
#define EPROTO      AW_EPROTO              /**< \brief 协议错误 */
#define EBADMSG     AW_EBADMSG             /**< \brief 损坏的消息 */
#define ENODATA     AW_ENODATA             /**< \brief 流中无数据 */
#define ETIME       AW_ETIME               /**< \brief 流ioctl()超时 */
#define ENOMSG      AW_ENOMSG              /**< \brief 没有所需的消息类型 */
#define EUCLEAN     AW_EUCLEAN             /**< \brief Structure需要清理 */
/** @} */

#define ECOMM       AW_ECOMM               /**< \brief Communication error on send */
#define EOVERFLOW   AW_EOVERFLOW           /**< \brief Value too large for defined data type */
#define EREMOTEIO   AW_EREMOTEIO           /**< \brief Remote I/O error */

extern int *_aw_get_errno_addr(void);

#define errno (*_aw_get_errno_addr())

#endif  /* __POSIX_ADAPTER_INTERNAL_H */

/* end of file */
