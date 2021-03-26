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
 * \brief 简单静态路由表
 *
 * \internal
 * \par modification history:
 * - 1.00 19-09-16  vih, first implementation
 * \endinternal
 */


#include "aworks.h"
#include "aw_assert.h"
#include "string.h"
#include "lwip/ext/netif_route.h"

struct netif_route_table {
    struct netif_route_table    *next;
    struct netif                *netif;
    ip_addr_t                    dst_ip_addr;
};

struct netif_route_table *g_netif_rtbl_list = NULL;
static struct netif_route_table *__netif_rtbl_last = NULL;


struct netif_route_table *__route_tbl_find (ip_addr_t *p_src_ipaddr, ip_addr_t *p_dst_ipaddr)
{
    struct netif_route_table *p_nr = NULL;

    aw_assert(p_dst_ipaddr != NULL);

    /* 任一地址为 any 都不进行查找，直接返回空   */
    if (ip_addr_isany(p_src_ipaddr) || ip_addr_isany(p_dst_ipaddr)) {
        return NULL;
    }

    /* 检查上一次使用的条目是否匹配  */
    do {
        if (__netif_rtbl_last == NULL) {
            break;
        }
        if (!netif_is_link_up(__netif_rtbl_last->netif)) {
            break;
        }
        if (!ip_addr_cmp(&__netif_rtbl_last->dst_ip_addr, p_dst_ipaddr)) {
            break;
        }
        if (!ip_addr_cmp(&__netif_rtbl_last->netif->ip_addr, p_src_ipaddr)) {
            break;
        }
        return __netif_rtbl_last;
    } while (0);

    for (p_nr = g_netif_rtbl_list; p_nr != NULL; p_nr = p_nr->next) {
        if (!netif_is_link_up(p_nr->netif)) {
            continue;
        }

        if (ip_addr_cmp(&p_nr->netif->ip_addr, p_src_ipaddr) &&
                ip_addr_cmp(&p_nr->dst_ip_addr, p_dst_ipaddr) ) {
            __netif_rtbl_last = p_nr;
            return p_nr;
        }
    }

    return NULL;
}

void netif_route_add (struct netif *netif, ip_addr_t *p_dst_ipaddr)
{
    struct netif_route_table *p_nr;

    /* dst 地址必须有效  */
    aw_assert(p_dst_ipaddr);
    aw_assert(!ip_addr_isany(p_dst_ipaddr));
    aw_assert(netif);

    /* 检查是否已添加 */
    for (p_nr = g_netif_rtbl_list; p_nr != NULL; p_nr = p_nr->next) {
        if (p_nr->netif == netif &&
                ip_addr_cmp(&p_nr->dst_ip_addr, p_dst_ipaddr) ) {
            /* 已添加则退出  */
            return;
        }
    }

    p_nr = (struct netif_route_table *)mem_malloc(sizeof(struct netif_route_table));

    if (p_nr != NULL) {
        p_nr->netif = netif;
        ip_addr_set(&p_nr->dst_ip_addr, p_dst_ipaddr);
        p_nr->next = g_netif_rtbl_list;
        g_netif_rtbl_list = p_nr;
    }
}

void netif_route_del (struct netif *netif, ip_addr_t *p_dst_ipaddr)
{
    struct netif_route_table *p_nr = NULL;
    struct netif_route_table *p_prev = NULL;

    aw_assert(netif);
    aw_assert(p_dst_ipaddr);

    for (p_nr = g_netif_rtbl_list; p_nr != NULL; p_prev = p_nr, p_nr = p_nr->next) {
        if (p_nr->netif == netif &&
                ip_addr_cmp(&p_nr->dst_ip_addr, p_dst_ipaddr) ) {
            if (p_prev == NULL) {
                g_netif_rtbl_list = p_nr->next;
            } else {
                p_prev->next = p_nr->next;
            }
            if (__netif_rtbl_last == p_nr) {
                __netif_rtbl_last = NULL;
            }
            mem_free(p_nr);
        }
    }
}

struct netif *netif_route_find (ip_addr_t *p_src_ipaddr, ip_addr_t *p_dst_ipaddr)
{
    struct netif_route_table *p_nr = NULL;

    aw_assert(p_src_ipaddr);
    aw_assert(p_dst_ipaddr);
    p_nr = __route_tbl_find(p_src_ipaddr, p_dst_ipaddr);
    if (p_nr) {
        aw_assert(p_nr->netif);
        if (netif_is_link_up(p_nr->netif)) {
            return p_nr->netif;
        } else {
            return NULL;
        }
    }

    return NULL;
}


int netif_route_change (ip_addr_t *p_old_dst_ipaddr,
                        ip_addr_t *p_new_dst_ipaddr,
                        struct netif *netif)
{
    struct netif_route_table *p_nr;

    if (netif == NULL) {
        return -AW_EINVAL;
    }
    aw_assert(p_old_dst_ipaddr);
    aw_assert(p_new_dst_ipaddr);

    for (p_nr = g_netif_rtbl_list; p_nr != NULL; p_nr = p_nr->next) {
        if (p_nr->netif == netif &&
                ip_addr_cmp(&p_nr->dst_ip_addr, p_old_dst_ipaddr) ) {
            break;
        }
    }
    if (!p_nr) {
        /* 不存在旧的项 */
        return -AW_ENOMEM;
    }

    /* 只修改目的地址，网卡不变  */
    p_nr->dst_ip_addr   = *p_new_dst_ipaddr;
    return 0;
}



