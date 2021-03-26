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
 * \brief 系统错误值
 *
 * 使用本服务需要包含以下头文件：
 * \code
 * #include "aw_errno.h"
 * \endcode
 *
 * - 所有的系统错误值都在这里定义。没有错误返回0，否则返回对应错误值的负值。
 *   为了兼容POSIX，这里定义了所有POSIX错误值。
 * - Apollo平台使用的错误值是在POSIX错误值前面加上AW_前缀。
 *   所有需要使用错误值的文件都应该包含本文件，Apollo平台自身代码应该使用AW_
 *   前缀的错误值，第三方软件如果使用了POSIX错误值在移植时可以直接使用本文件。
 * - 为了提高代码兼容性，不应该使用本文件之外错误值。
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
 * \name 错误编码数据类型
 * @{
 */
typedef aw_psp_error_t     error_t;    /**< \brief 兼容POSIX错误类型 */
typedef int                aw_err_t;   /**< \brief AWorks错误类型 */
/** @} */

/**
 * \name AWorks平台错误编码接口（非POSIX兼容）
 * @{
 */

/**
 * \brief 设置errno
 * \note 这个宏的作用是用于兼容支持errno和不支持errno的系统。当系统不支持errno
 *      时这个宏是个空定义。
 * \par 使用示例
 * \code
 * AW_ERRNO_SET(ENODEV); // 相当于调用：errno = ENODEV;
 * \endcode
 */
#define AW_ERRNO_SET(err)   AW_PSP_ERRNO_SET(err)

/**
 * \brief 获取errno
 * \note
 *  - 这个宏的作用是用于兼容支持errno和不支持errno的系统。当系统不支持errno
 *    时这个宏是个空定义。
 *  - 当系统不支持errno时，使用这个宏获取到错误值无意义，编写程序时应注意。
 * \par 使用示例
 * \code
 * AW_ERRNO_GET(err); // 相当于调用：err = errno;
 * if (err) {
 *     ;              // 错误处理
 * }
 * \endcode
 */
#define AW_ERRNO_GET(err)   AW_PSP_ERRNO_GET(err)
/* @} */

/* 下面的的错误码需要由平台提供 */
#if 1

/**
 * \addtogroup grp_aw_if_posix_err 系统错误编码值（POSIX兼容）
 * @{
 */

/**
 * \name POSIX错误值
 * @{
 */
#define AW_EPERM           1            /* 操作不允许 */
#define AW_ENOENT          2            /* 文件或目录不存在 */
#define AW_ESRCH           3            /* 进程不存在 */
#define AW_EINTR           4            /* 调用被中断 */
#define AW_EIO             5            /* I/O 错误 */
#define AW_ENXIO           6            /* 设备或地址不存在 */
#define AW_E2BIG           7            /* 参数列表太长 */
#define AW_ENOEXEC         8            /* 可执行文件格式错误 */
#define AW_EBADF           9            /* 文件描述符损坏 */
#define AW_ECHILD          10           /* 没有子进程 */
#define AW_EAGAIN          11           /* 资源不可用，需重试 */
#define AW_ENOMEM          12           /* 空间（内存）不足 */
#define AW_EACCES          13           /* 权限不够 */
#define AW_EFAULT          14           /* 地址错误 */
#define AW_ENOTEMPTY       15           /* 目录非空 */
#define AW_EBUSY           16           /* 设备或资源忙 */
#define AW_EEXIST          17           /* 文件已经存在 */
#define AW_EXDEV           18           /* 跨设备连接 */
#define AW_ENODEV          19           /* 设备不存在 */
#define AW_ENOTDIR         20           /* 不是目录 */
#define AW_EISDIR          21           /* 是目录 */
#define AW_EINVAL          22           /* 无效参数 */
#define AW_ENFILE          23           /* 系统打开文件太多，描述符不够用 */
#define AW_EMFILE          24           /* 打开的文件太多 */
#define AW_ENOTTY          25           /* 不合适的I/O控制操作 */
#define AW_ENAMETOOLONG    26           /* 文件名太长 */
#define AW_EFBIG           27           /* 文件太大 */
#define AW_ENOSPC          28           /* 设备剩余空间不足 */
#define AW_ESPIPE          29           /* 无效的搜索（Invalid seek） */
#define AW_EROFS           30           /* 文件系统只读 */
#define AW_EMLINK          31           /* 链接太多 */
#define AW_EPIPE           32           /* 损坏的管道 */
#define AW_EDEADLK         33           /* 资源可能死锁 */
#define AW_ENOLCK          34           /* 无可用（空闲）的锁 */
#define AW_ENOTSUP         35           /* 不支持 */
#define AW_EMSGSIZE        36           /* 消息太大 */
/** @} */

/**
 * \name ANSI错误值
 * @{
 */
#define AW_EDOM            37           /* 数学函数参数超出范围 */
#define AW_ERANGE          38           /* 数学函数计算结果太大 */
#define AW_EILSEQ          39           /* 非法的字节顺序 */
/** @} */

/**
 * \name 网络参数错误相关
 * @{
 */
#define AW_EDESTADDRREQ    40           /* 需要目标地址 */
#define AW_EPROTOTYPE      41           /* socket协议类型错误 */
#define AW_ENOPROTOOPT     42           /* 协议不可用 */
#define AW_EPROTONOSUPPORT 43           /* 协议不支持 */
#define AW_ESOCKTNOSUPPORT 44           /* Socket类型不支持 */
#define AW_EOPNOTSUPP      45           /* socket不支持该操作 */
#define AW_EPFNOSUPPORT    46           /* 协议族不支持 */
#define AW_EAFNOSUPPORT    47           /* 地址簇不支持 */
#define AW_EADDRINUSE      48           /* 地址已经被占用 */
#define AW_EADDRNOTAVAIL   49           /* 地址不可用 */
#define AW_ENOTSOCK        50           /* 被操作对象不是socket */
/** @} */

/**
 * \name 可选择实现的错误值
 * @{
 */
#define AW_ENETUNREACH     51           /* 网络不可达 */
#define AW_ENETRESET       52           /* 网络中断了连接 */
#define AW_ECONNABORTED    53           /* 连接中断 */
#define AW_ECONNRESET      54           /* 连接复位 */
#define AW_ENOBUFS         55           /* 缓冲空间不足 */
#define AW_EISCONN         56           /* Socket已经连接 */
#define AW_ENOTCONN        57           /* Socket没有连接 */
#define AW_ESHUTDOWN       58           /* Socket已经关闭，不能发送数据 */
#define AW_ETOOMANYREFS    59           /* 引用太多，无法拼接 */
#define AW_ETIMEDOUT       60           /* 连接超时 */
#define AW_ECONNREFUSED    61           /* 连接被拒绝 */
#define AW_ENETDOWN        62           /* 网络已经停止 */
#define AW_ETXTBSY         63           /* 文本文件忙 */
#define AW_ELOOP           64           /* 符号链接级数太多 */
#define AW_EHOSTUNREACH    65           /* 主机不可达 */
#define AW_ENOTBLK         66           /* 非块设备 */
#define AW_EHOSTDOWN       67           /* 主机已经关闭 */
/** @} */

/**
 * \name 非阻塞和中断I/O错误值
 * @{
 */
#define AW_EINPROGRESS     68           /* 操作正在进行中 */
#define AW_EALREADY        69           /* 连接正被使用中 */
/* 70 */
#define AW_EWOULDBLOCK     AW_EAGAIN    /* 操作会阻塞（同EAGAIN） */
#define AW_ENOSYS          71           /* 不支持的功能（功能未实现）*/
/** @} */

/**
 * \name 异步I/O错误值
 * @{
 */
#define AW_ECANCELED       72           /* 操作已经取消 */
/* 73 */
/** @} */

/**
 * \name 流相关错误值
 * @{
 */
#define AW_ENOSR           74           /* 没有流资源 */
#define AW_ENOSTR          75           /* 不是流设备 */
#define AW_EPROTO          76           /* 协议错误 */
#define AW_EBADMSG         77           /* 损坏的消息 */
#define AW_ENODATA         78           /* 流中无数据 */
#define AW_ETIME           79           /* 流ioctl()超时 */
#define AW_ENOMSG          80           /* 没有所需的消息类型 */
#define AW_EUCLEAN         81           /* Structure需要清理 */
/** @} */

#define AW_ECOMM           109          /* Communication error on send */
#define AW_EOVERFLOW       112          /* Value too large for defined data type */
#define AW_EREMOTEIO       121          /* Remote I/O error */

/** @} grp_aw_if_posix_err */

#endif  /* ... */

/** @} grp_aw_if_errno */

#endif /* __AW_ERRNO_H */

/* end of file */
