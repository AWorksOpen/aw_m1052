/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/
/**
 * \file
 * \brief 虚拟网卡
 *
 * \internal
 * \par modification history:
 * - 1.00 20-03-23  vih, first implementation
 * \endinternal
 */


#include "aworks.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stdint.h"
#include "assert.h"

#include "aw_netif.h"
#include "aw_mem.h"
#include "aw_shell.h"

#include "lwip/ip_addr.h"
#include "lwip/ext/vnetif.h"
/*----------------------------------------------------------------------------*/
#define __LOG_BASE(mask, color, fmt, ...)   \
    if ((mask)) {aw_shell_printf(AW_DBG_SHELL_IO, fmt, ##__VA_ARGS__);}
#define __LOG(mask, fmt, ...)  __LOG_BASE(mask, NULL, fmt, ##__VA_ARGS__)
#define __LOG_PFX(mask, fmt, ...) \
    if ((mask)) {__LOG(1, "[%s:%d] " fmt "\n", \
                 __func__, __LINE__, ##__VA_ARGS__);}
#define __LOG_ERR(fmt, ...)  \
        __LOG_BASE(__MASK_ERR, AW_SHELL_FCOLOR_RED, "[err][%s:%d] " fmt "\n", \
                   __func__, __LINE__, ##__VA_ARGS__)
#define __LOG_ERR_IF(cond)  \
    if ((cond)) {__LOG_BASE(__MASK_ERR, AW_SHELL_FCOLOR_RED, "[err][%s:%d] " \
                            #cond "\n", __func__, __LINE__); }
#define __GOTO_EXIT_IF(cond, ret_val)  \
        if ((cond)) {__LOG_BASE(__MASK_ERR, AW_SHELL_FCOLOR_RED, \
                        "[err][%s:%d] " #cond "\n", __func__, __LINE__); \
                        ret = (ret_val); goto exit; }
#define __RETURN_IF(cond, ret_exp)  \
        if ((cond)) {__LOG_BASE(__MASK_ERR, AW_SHELL_FCOLOR_RED,\
            "[err][%s:%d] " #cond "\n", __func__, __LINE__); return (ret_exp); }
#define __EXIT_IF(cond)  \
        if ((cond)) {__LOG_BASE(__MASK_ERR, AW_SHELL_FCOLOR_RED,\
                "[err][%s:%d] " #cond "\n", __func__, __LINE__); return; }
/*----------------------------------------------------------------------------*/
#define __MASK_ERR               1

/*----------------------------------------------------------------------------*/
/* 1：虚拟网卡总是 link up； 0：虚拟网卡link up 状态随实际网卡变化 */
#define __VNETIF_ALWAYS_LINK_UP  1

/*----------------------------------------------------------------------------*/
aw_local char *__strdup (const char * src)
{
    char *dst;
    int   len;

    if (src == NULL) {
        return NULL;
    }
    dst = aw_mem_alloc(strlen(src) + 1);
    if (dst) {
        strcpy(dst, src);
    }
    return dst;
}
/*----------------------------------------------------------------------------*/

aw_local void __netif_ev_hdl (aw_netif_t *p_netif, aw_netif_event_t event, void *p_arg)
{
    aw_netif_t      *vnetif = p_arg;

    switch (event) {
        case AW_NETIF_EVENT_DEV_UP:
            __LOG_PFX(1, "virtual netif up [%s]", vnetif->p_name);
            aw_netif_up(vnetif);
            break;

        case AW_NETIF_EVENT_DEV_DOWN:
            __LOG_PFX(1, "virtual netif down [%s]", vnetif->p_name);
            aw_netif_down(vnetif);
            break;

#if __VNETIF_ALWAYS_LINK_UP
            /*
             * todo
             */
        case AW_NETIF_EVENT_LINK_UP:
        case AW_NETIF_EVENT_LINK_DOWN:
            break;

#else
        case AW_NETIF_EVENT_LINK_UP:
            __LOG_PFX(1, "virtual netif link up [%s]", vnetif->p_name);
            aw_netif_set_link_up(vnetif);
            break;

        case AW_NETIF_EVENT_LINK_DOWN:
            __LOG_PFX(1, "virtual netif link down [%s]", vnetif->p_name);
            aw_netif_set_link_down(vnetif);
            break;
#endif

        case AW_NETIF_EVENT_DEV_REMOVED:
            __LOG_PFX(1, "virtual netif remove ? [%s]", vnetif->p_name);
            /* todo 这里不允许直接 remove 虚拟网卡，应该通知应用层，由应用层来 remove */
            break;
    }
}

/*
 * 范例：
    struct in_addr ip_addr;
    struct in_addr netmask;
    struct in_addr gw_addr;

    inet_aton("192.168.13.1", &ip_addr);
    inet_aton("255.255.255.0", &netmask);
    inet_aton("192.168.13.254", &gw_addr);

    ret = aw_vnetif_add("eth0", "veth0", &ip_addr, &netmask, &gw_addr);

 */
int aw_vnetif_add (const char *netif_name,
                   const char *vnetif_name,
                   struct in_addr *ip_addr,
                   struct in_addr *netmask,
                   struct in_addr *gw_addr)
{
    aw_netif_t      *netif;
    aw_netif_t      *vnetif = NULL;
    aw_netif_ops_t  *p_ops  = NULL;
    int             ret;
    aw_bool_t       is_up;
    char            *p_vn_name;

    (void)is_up;

    netif = aw_netif_dev_open(vnetif_name);
    if (netif != NULL) {
        aw_netif_dev_close(netif);
        return -AW_EEXIST;
    }

    netif = aw_netif_dev_open(netif_name);
    if (netif == NULL) {
        return -AW_EINVAL;
    } else {
        aw_netif_dev_close(netif);
    }

    vnetif = aw_mem_alloc(sizeof(*vnetif));
    if (vnetif == NULL) {
        goto cleanup;
    }
    p_ops = aw_mem_alloc(sizeof(*p_ops));
    if (p_ops == NULL) {
        goto cleanup;
    }
    p_vn_name = __strdup(vnetif_name);
    if (p_vn_name == NULL) {
        goto cleanup;
    }

    memset(vnetif, 0, sizeof(*vnetif));
    memset(p_ops, 0, sizeof(*p_ops));
    p_ops->output = netif->p_ops->output;

    ret = aw_netif_event_cb_register(netif, __netif_ev_hdl, vnetif);
    if (ret != AW_OK) {
        __LOG_ERR("event cb register failed. [%s]", vnetif_name);
        goto cleanup;
    }

    ret = aw_netif_add(  vnetif,
                         p_vn_name,
                         p_ops,
                         NULL,
                         netif->type,
                         netif->hwaddr,
                         netif->mtu);

    if (ret != AW_OK) {
        __LOG_ERR("already exist");
        goto cleanup;
    }

    vnetif->p_phy_layer_data = netif->p_phy_layer_data;

    aw_netif_ipv4_ip_set(vnetif, ip_addr);
    aw_netif_ipv4_gw_set(vnetif, gw_addr);
    aw_netif_ipv4_netmask_set(vnetif, netmask);

#if __VNETIF_ALWAYS_LINK_UP
    /*
     * 初始化时自动设置了 AW_NETIF_STAT_DEV_UP，能触发 aw_netif_up
     */
    aw_netif_set_link_up(vnetif);
#else
    aw_netif_is_link_up(netif, &is_up);
    if (is_up) {
        aw_netif_set_link_up(vnetif);
    }
#endif

    return AW_OK;
cleanup:
    if (vnetif) {
        aw_mem_free(vnetif);
    }
    if (p_ops) {
        aw_mem_free(p_ops);
    }
    if (p_vn_name) {
        free(p_vn_name);
    }

    return -AW_EPERM;
}

int aw_vnetif_remove (const char *vnetif_name)
{
    aw_netif_t *vnetif;

    vnetif = aw_netif_dev_open(vnetif_name);
    if (vnetif == NULL) {
        return -AW_EINVAL;
    } else {
        aw_netif_dev_close(vnetif);
    }

    aw_netif_remove(vnetif);

    aw_mem_free((void *)vnetif->p_ops);
    free((const void*)vnetif->p_name);
    aw_mem_free(vnetif);

    return AW_OK;
}


/*
 * 通过 src ip 查找该网段的网卡，必须和物理网卡相同的 hwaddr
 */
struct netif * vnetif_find_by_src (struct netif * netif, ip_addr_t * src)
{
    struct netif    *target_netif;

    if (ip_addr_isbroadcast(src, netif)) {
        return NULL;
    }

    for (target_netif = netif_list;
            target_netif != NULL;
            target_netif = target_netif->next) {

        assert(IP_IS_V4(src));
        assert(IP_IS_V4(&target_netif->ip_addr));

        /* 查找网段相同的网卡 */
        if (ip4_addr_netcmp(ip_2_ip4(src),
                            ip_2_ip4(&target_netif->ip_addr),
                            ip_2_ip4(&target_netif->netmask))) {

            /* 最后 硬件地址必须相同 */
            if (memcmp(netif->hwaddr,
                       target_netif->hwaddr,
                       netif->hwaddr_len) == 0) {

                LWIP_DEBUGF(LWIP_DBG_OFF, ("use netif [%c%c%d].\n",
                        target_netif->name[0] ,
                        target_netif->name[1],
                        target_netif->num));
                return target_netif;
            }
        }
    }
    return NULL;
}

/*
 * 通过 dest ip 查找该 ip 的网卡，必须和物理网卡相同的 hwaddr
 */
struct netif * vnetif_find_by_dest (struct netif * netif, ip_addr_t * dest)
{
    struct netif    *target_netif;

    if (ip_addr_isbroadcast(dest, netif)) {
        return NULL;
    }

    /*
     * 处理请求虚拟网卡的 arp:
     * 遍历 netif，比较 dest ip 和 target_netif->ipaddr，再 比较 进来的 netif 和
     * target_netif 的 mac 要相等，这样就能保证访问该虚拟网卡ip的包是从虚拟网卡绑定
     * 的网口中进来的
     */
    for (target_netif = netif_list;
            target_netif != NULL;
            target_netif = target_netif->next) {

        /* 查找 ip 相同的网卡 */
        if (ip4_addr_cmp(ip_2_ip4(dest), ip_2_ip4(&target_netif->ip_addr))) {
            /* 最后 硬件地址必须相同 */
            if (memcmp(netif->hwaddr,
                       target_netif->hwaddr,
                       netif->hwaddr_len) == 0) {
                LWIP_DEBUGF(0, ("use netif [%c%c%d].\n", target_netif->name[0]
                        , target_netif->name[1], target_netif->num));
                return target_netif;
            }
        }
    }
    return NULL;
}



/*
 * 获取该物理网卡的下一个虚拟网卡
 *
 * netif:       物理网卡
 * prev_vnetif: 前一个虚拟网卡
 *
 * 返回：下一个虚拟网卡
 */
struct netif * vnetif_iterate (struct netif *netif, struct netif *prev_vnetif)
{
    struct netif    *target_netif;
    char            start = 0;

    for (target_netif = netif_list;
            target_netif != NULL;
            target_netif = target_netif->next) {
        if (target_netif == netif) {
            continue;
        }

        if (start == 0) {
            if (prev_vnetif == NULL) {
                start = 1;
            } else {
                if (target_netif != prev_vnetif) {
                    continue;
                } else {
                    start = 1;
                    continue;
                }
            }
        }

        if (memcmp(netif->hwaddr, target_netif->hwaddr, netif->hwaddr_len) == 0) {
            return target_netif;
        }
    }

    return NULL;
}




