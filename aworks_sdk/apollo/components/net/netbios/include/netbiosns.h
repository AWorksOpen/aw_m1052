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
 * \brief NetBIOS name service sample (case-insensitive)
 *
 * \internal
 * \par Modification history
 * - 1.00 16-11-13 axn, first implementation.
 * \endinternal
 */


#ifndef __NETBIOSNS_H__
#define __NETBIOSNS_H__

#include "apollo.h"
#include "aw_types.h"

/** size of a NetBIOS name */
#define NETBIOS_NAME_LEN 16

#ifdef __cplusplus
extern "C" {
#endif

aw_err_t aw_netbios_start (char *hostname);
aw_err_t aw_netbios_stop (void);
aw_bool_t   aw_netbios_is_on (void);
char    *aw_netbios_name_get (void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __NETBIOSNS_H__ */

/* end of file */

