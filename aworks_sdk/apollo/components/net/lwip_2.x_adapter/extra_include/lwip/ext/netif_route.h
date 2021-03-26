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
 * \brief 简单静态路由表接口
 *
 * 使用本服务需要包含头文件
 * \code
 * #include "lwip/ext/netif_route.h"
 * \endcode
 *
 * \internal
 * \par Modification History
 * - 1.00 20-03-23  vih, first implementation
 * \endinternal
 */

#ifndef __NETIF_ROUTE_H__
#define __NETIF_ROUTE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "lwip/opt.h"

#include "lwip/def.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/inet.h"
#include "lwip/stats.h"


/**
 * \brief 添加一条路由，指定 ip 包由该网卡输出
 *
 * \param[in] netif         网卡地址
 * \param[in] p_dst_ipaddr  目的地址，不可为空，值不可为 any
 */
void netif_route_add (struct netif *netif,
                      ip_addr_t *p_dst_ipaddr);

/**
 * \brief 删除一条路由
 *
 * \param[in] netif         网卡
 * \param[in] p_dst_ipaddr  目的地址，不可为空
 */
void netif_route_del (struct netif *netif, ip_addr_t *p_dst_ipaddr);

/**
 * \brief 通过 ip 地址查询路由表中的输出网卡
 *
 * \param[in] p_src_ipaddr  源地址，不可为空，必须有值才可能匹配成功
 * \param[in] p_dst_ipaddr  目的地址，不可为空，必须有值才可能匹配成功
 *
 * \retval !NULL  查找成功
 * \retval NULL   未找到
 */
struct netif *netif_route_find (ip_addr_t *p_src_ipaddr, ip_addr_t *p_dst_ipaddr);

/**
 * \brief 修改一条路由条目，只修改目的地址，不修改网卡
 *
 * \param[in] p_old_dst_ipaddr  旧 ip，不可为空
 * \param[in] p_new_dst_ipaddr  新 ip，不可为空
 * \param[in] netif             网卡地址
 *
 * \retval  AW_OK       操作成功
 * \retval  -AW_EINVAL  参数无效
 */
int netif_route_change (ip_addr_t *p_old_dst_ipaddr,
                        ip_addr_t *p_new_dst_ipaddr,
                        struct netif *netif);



#ifdef __cplusplus
}
#endif

#endif /* __NETIF_ROUTE_H__ */

/* end of file */
