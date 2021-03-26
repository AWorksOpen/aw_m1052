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
 * \brief DHCP Server config header
 *
 * \internal
 * \par modification history:
 * - 1.00 19-08-14  mex, first implementation
 * \endinternal
 */

#ifndef __DHCP_SERVER_CONFIG_H
#define __DHCP_SERVER_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif


/**
 *  DHCP server task stack size
 */
#define DHCPS_TASK_STK_SIZE              4096


/**
 *  The maximum number of clients that the DHCP server can accept
 */
#ifndef DHCPS_CLIENT_CACHE_MAX
#define DHCPS_CLIENT_CACHE_MAX           32
#endif /* DHCPS_CLIENT_CACHE_MAX */


/**
 *  The maximum length of client name that the DHCP server can accept
 */
#ifndef DHCPS_CLIENT_NAME_MAX
#define DHCPS_CLIENT_NAME_MAX            24
#endif /* DHCPS_CLIENT_NAME_MAX */


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __DHCP_SERVER_CONFIG_H */

/* end of file */
