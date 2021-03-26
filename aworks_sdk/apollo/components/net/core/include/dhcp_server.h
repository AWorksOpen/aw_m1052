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
 * \brief DHCP Server
 *
 * \internal
 * \par Modification history
 *
 * - 15-12-02, axn, The first version.
 * - 18-04-12, phd, migrate to aw_netif.
 * \endinternal
 */
#ifndef __DHCP_SERVER_H
#define __DHCP_SERVER_H

#include "aworks.h"
#include "aw_sem.h"
#include "aw_task.h"
#include "aw_types.h"

#include "aw_netif.h"
#include "aw_sockets.h"
#include "dhcp_server_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DHCPS_CLIENT_NAME_EN

/*******************************************************************************
 *                                   Structures
 *******************************************************************************/
typedef struct dhcps_cache {
    aw_eth_mac_t mac_addr;
    struct in_addr ip_addr;
#ifdef DHCPS_CLIENT_NAME_EN
    char client_name[DHCPS_CLIENT_NAME_MAX];
#endif /* DHCPS_CLIENT_NAME_EN */
} dhcps_cache_t;

struct dhcp_server {
    AW_TASK_DECL(dhcp_server, DHCPS_TASK_STK_SIZE);
    dhcps_cache_t client_cache[DHCPS_CLIENT_CACHE_MAX];

    aw_bool_t keep_run;
    aw_bool_t thread_run;

    const char *netif;

    uint32_t ip_pool_start;
    size_t ip_pool_limit;

    char request_buf[600];
    char reply_buf[600];
};


/*******************************************************************************
 *                           Global Variables
 *******************************************************************************/

/*******************************************************************************
 *                          Function Declarations
 *******************************************************************************/

void dhcps_set_addr_pool (struct dhcp_server *p_ctx, 
                          uint32_t            start, 
                          size_t              limit);
aw_err_t dhcps_set_addr_pool_def (struct dhcp_server *p_ctx);

#ifdef DHCPS_CLIENT_NAME_EN
char *dhcps_get_host_name_by_macaddr (struct dhcp_server *p_ctx, 
                                      aw_eth_mac_t       *mac);
char *dhcps_get_host_name_by_ipaddr (struct dhcp_server *p_ctx, 
                                     struct in_addr     *ip);
#endif
aw_err_t dhcps_get_ip_by_macaddr (struct dhcp_server *p_ctx, 
                                  aw_eth_mac_t       *mac, 
                                  struct in_addr     *ip);

void dhcps_bind_netif (struct dhcp_server *p_ctx, const char *p_netif);
aw_err_t dhcps_start (struct dhcp_server *p_ctx);
aw_err_t dhcps_stop (struct dhcp_server *p_ctx);
aw_bool_t dhcps_is_on (struct dhcp_server *p_ctx);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __DHCP_SERVER_H */

/* end of file */
