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
 * \brief 虚拟网卡接口
 *
 * 使用本服务需要包含头文件
 * \code
 * #include "aw_vnetif.h"
 * \endcode
 *
 * \internal
 * \par Modification History
 * - 1.00 20-03-23  vih, first implementation
 * \endinternal
 */

#ifndef __AW_VNETIF_H
#define __AW_VNETIF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_netif.h"
#include "lwip/netif.h"
#include "lwip/ip.h"


/**
 * \brief 在一个物理网卡上创建虚拟网卡
 *
 * \param[in] netif_name    物理网卡名字
 * \param[in] vnetif_name   虚拟网卡名字
 * \param[in] ip_addr       ip 地址
 * \param[in] netmask       掩码
 * \param[in] gw_addr       网关
 *
 * \retval  AW_OK           操作成功
 * \retval  -AW_EINVAL      netif_name 名字的网卡不存在
 * \retval  -AW_EEXIST      vnetif_name 名字的网卡已存在
 */
int aw_vnetif_add (const char *netif_name,
                   const char *vnetif_name,
                   struct in_addr *ip_addr,
                   struct in_addr *netmask,
                   struct in_addr *gw_addr);

/**
 * \brief 删除一个虚拟网卡
 *
 * \param[in] vnetif_name   虚拟网卡名字
 *
 * \retval  AW_OK           操作成功
 * \retval  -AW_EINVAL      vnetif_name 名字的网卡不存在
 */
int aw_vnetif_remove (const char *vnetif_name);


/*******************************************************************************
 * 虚拟网卡管理接口（lwip 协议栈中使用）
 ******************************************************************************/
struct netif * vnetif_iterate (struct netif *netif, struct netif *prev_vnetif);
struct netif * vnetif_find_by_src (struct netif * netif, ip_addr_t * src);
struct netif * vnetif_find_by_dest (struct netif * netif, ip_addr_t * dest);


#ifdef __cplusplus
}
#endif

#endif /* __AW_VNETIF_H */

/* end of file */
