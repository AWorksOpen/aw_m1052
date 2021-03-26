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

#include "awbl_nic_manage.h"
#include "aw_netif.h"

aw_err_t awbl_nic_ioctl (const char *p_nic_name, int opt, void *p_arg)
{
    aw_err_t result;
    aw_netif_t *p_netif;

    p_netif = aw_netif_dev_open(p_nic_name);

    switch (opt) {

    case AWBL_NIC_OPT_GET_IPADDR:
        result = aw_netif_ipv4_ip_get(p_netif, (struct in_addr *) p_arg);
        break;

    case AWBL_NIC_OPT_SET_IPADDR:
        result = aw_netif_ipv4_ip_set(p_netif, (struct in_addr *) p_arg);
        break;

    case AWBL_NIC_OPT_GET_GATEWAY:
        result = aw_netif_ipv4_gw_get(p_netif, (struct in_addr *) p_arg);
        break;

    case AWBL_NIC_OPT_SET_GATEWAY:
        result = aw_netif_ipv4_gw_set(p_netif, (struct in_addr *) p_arg);
        break;

    case AWBL_NIC_OPT_GET_NETMASK:
        result = aw_netif_ipv4_netmask_get(p_netif, (struct in_addr *) p_arg);
        break;

    case AWBL_NIC_OPT_SET_NETMASK:
        result = aw_netif_ipv4_netmask_get(p_netif, (struct in_addr *) p_arg);
        break;

    case AWBL_NIC_OPT_GET_HWADDR:
        result = aw_netif_hwaddr_get(p_netif, (uint8_t *) p_arg, 6);
        break;

    case AWBL_NIC_OPT_SET_DEFAULT:
        result = aw_netif_set_default(p_netif);
        break;

    case AWBL_NIC_OPT_SET_DHCPC:
        if ((aw_bool_t) p_arg) {
            result = aw_netif_dhcpc_start(p_netif);
        } else {
            result = aw_netif_dhcpc_stop(p_netif);
        }
        break;

    case AWBL_NIC_OPT_GET_DHCPC: {
        aw_bool_t up;
        result = aw_netif_dhcpc_is_on(p_netif, &up);
        *(aw_bool_t *) p_arg = up;
    }
        break;

    case AWBL_NIC_OPT_DHCPC_STATE: {
        aw_bool_t bound;
        result = aw_netif_dhcpc_is_bound(p_netif, &bound);
        *(aw_bool_t *) p_arg = bound;
    }
        break;

    default:
        result = -AW_ENOTSUP;
        break;
    }

    aw_netif_dev_close(p_netif);
    return result;
}

/**
 * \brief start DHCP client
 */
aw_err_t awbl_nic_dhcpc_start (const char *p_nic_name)
{
    return awbl_nic_ioctl(p_nic_name, AWBL_NIC_OPT_SET_DHCPC, (void *) AW_TRUE);
}

/**
 * \brief stop DHCP client
 */
aw_err_t awbl_nic_dhcpc_stop (const char *p_nic_name)
{
	return awbl_nic_ioctl(p_nic_name, AWBL_NIC_OPT_SET_DHCPC, (void *) AW_FALSE);
}

/**
 * \brief DHCP client is on or not?
 */
aw_bool_t awbl_nic_dhcpc_is_on (const char *p_nic_name)
{
    aw_bool_t on = AW_FALSE;
    awbl_nic_ioctl(p_nic_name, AWBL_NIC_OPT_GET_DHCPC, (void *) &on);
    return on;
}

/**
 * \brief get DHCP client state
 */
aw_bool_t awbl_nic_dhcpc_state (const char *p_nic_name)
{
    aw_bool_t state = AW_FALSE;
    awbl_nic_ioctl(p_nic_name, AWBL_NIC_OPT_DHCPC_STATE, (void *) &state);
    return state;
}

/* end of file */
