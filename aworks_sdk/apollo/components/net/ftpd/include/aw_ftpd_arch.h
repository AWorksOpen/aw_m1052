/*******************************************************************************
*                                 AnyWhere
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      anywhere.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief FTP server x86 arch for MINGW
 *
 * \internal
 * \par modification history:
 * - 1.01 15-07-06  axn, 修复lwip-1.4.1下的兼容性问题
 * - 1.00 13-07-08  orz, first implementation.
 * \endinternal
 */

#ifndef __FTPD_ARCH_H
#define __FTPD_ARCH_H

/**
 * \addtogroup grp_aw_if_ftpd
 * \copydoc ftpd_cfg.h
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_task.h"
#include "aw_delay.h"
#include "aw_errno.h"
#include "ftpd/aw_ftpd.h"

#include "io/sys/aw_stat.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "io/aw_dirent.h"

#define FTPD_ARCH_NO_FSTAT
#define FTPD_ARCH_TCP_TCP_KEEPIDLE

/******************************************************************************/
#define ftpd_isspace(a)             isspace((int)(a))
#define ftpd_toupper(a)             toupper((int)(a))
#define sleep(s)                    aw_mdelay(s * 1000)
#define ftpd_arch_task_dead()		do { sleep(10); } while (1)

/******************************************************************************/
typedef int socket_t;

#define socket_set_invalid(sock)	((sock) = -1)
#define socket_valid(sock)			((sock) >= 0)

#define FTPD_SO_SNDRCVTIMEO_GET_MS(optval) \
((((struct timeval *)(optval))->tv_sec * 1000U) + (((struct timeval *)(optval))->tv_usec / 1000U))

#ifdef __cplusplus
}
#endif

/** @} grp_aw_if_ftpd */

#endif /* } __FTPD_ARCH_H */

/* end of file */

