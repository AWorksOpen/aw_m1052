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
 * \brief NIC(net interface card) MANAGE
 *
 * \internal
 * \par modification history
 * - 1.20 18-06-05  phd, rework for compatibility
 * - 1.10 17-12-22  phd, add link state & event
 * - 1.00 15-07-28  axn, first implementation
 * \endinternal
 */

#ifndef __AWBL_NIC_MANAGE_H
#define __AWBL_NIC_MANAGE_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aworks.h"

#warning "deprecated, remove this file."

/**
 * \brief awbl_nic_ioctl opts
 */
typedef enum __awbl_nic_opt {
    AWBL_NIC_OPT_DUMMY = 0,
    AWBL_NIC_OPT_GET_IPADDR,
    AWBL_NIC_OPT_SET_IPADDR,
    AWBL_NIC_OPT_GET_GATEWAY,
    AWBL_NIC_OPT_SET_GATEWAY,
    AWBL_NIC_OPT_GET_NETMASK,
    AWBL_NIC_OPT_SET_NETMASK,
    AWBL_NIC_OPT_GET_HWADDR ,
    AWBL_NIC_OPT_SET_DEFAULT,
    AWBL_NIC_OPT_SET_DHCPC,
    AWBL_NIC_OPT_GET_DHCPC,
    AWBL_NIC_OPT_DHCPC_STATE,
} awbl_nic_opt_t;

/**
 * \brief NIC IOCTRL
 *
 * [opt] = AWBL_NIC_OPT_GET_XXXX or AWBL_NIC_OPT_SET_XXXX
 *
 * \see at the beginning of this document
 */
aw_err_t awbl_nic_ioctl (const char *p_nic_name, int opt, void *p_arg);

/**
 * \brief start DHCP client
 */
aw_err_t awbl_nic_dhcpc_start (const char *p_nic_name);

/**
 * \brief stop DHCP client
 */
aw_err_t awbl_nic_dhcpc_stop (const char *p_nic_name);

/**
 * \brief DHCP client is on or not
 */
aw_bool_t awbl_nic_dhcpc_is_on (const char *p_nic_name);

/**
 * \brief get DHCP client state
 */
aw_bool_t awbl_nic_dhcpc_state (const char *p_nic_name);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_NIC_MANAGE_H */

/* end of file */
