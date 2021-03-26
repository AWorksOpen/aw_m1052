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
 * \brief 主机名称
 *
 * 使用本服务需要包含头文件
 * \code
 * #include "aw_hostname.h"
 * \endcode
 *
 * \internal
 * \par Modification History
 * - 1.00 18-03-28  phd, first implementation
 * \endinternal
 */

#ifndef __AW_HOSTNAME_H
#define __AW_HOSTNAME_H

#ifdef __cplusplus
extern "C" {
#endif

#define AW_NET_HOSTNAME_LEN_MAX     32

/**
 * @brief Network hostname configuration library
 * @defgroup net_hostname Network Hostname Library
 * @ingroup networking
 * @{
 */

/**
 * @brief Get the device hostname
 *
 * @details Return pointer to device hostname.
 *
 * @return Pointer to hostname.
 */
const char *aw_net_hostname_get (void);

/**
 * @brief Set the device hostname
 *
 * @details Return pointer to device hostname.
 *
 * @return Pointer to hostname or NULL if not set.
 */
const char *aw_net_hostname_set (const char *p_name);

/**
 * @brief Initialize and set the device hostname.
 *
 */
void aw_net_hostname_init (void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif

/* end of file */

