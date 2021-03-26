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
 * \brief ���������ӿ�
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ�
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
 * \brief ��һ�����������ϴ�����������
 *
 * \param[in] netif_name    ������������
 * \param[in] vnetif_name   ������������
 * \param[in] ip_addr       ip ��ַ
 * \param[in] netmask       ����
 * \param[in] gw_addr       ����
 *
 * \retval  AW_OK           �����ɹ�
 * \retval  -AW_EINVAL      netif_name ���ֵ�����������
 * \retval  -AW_EEXIST      vnetif_name ���ֵ������Ѵ���
 */
int aw_vnetif_add (const char *netif_name,
                   const char *vnetif_name,
                   struct in_addr *ip_addr,
                   struct in_addr *netmask,
                   struct in_addr *gw_addr);

/**
 * \brief ɾ��һ����������
 *
 * \param[in] vnetif_name   ������������
 *
 * \retval  AW_OK           �����ɹ�
 * \retval  -AW_EINVAL      vnetif_name ���ֵ�����������
 */
int aw_vnetif_remove (const char *vnetif_name);


/*******************************************************************************
 * ������������ӿڣ�lwip Э��ջ��ʹ�ã�
 ******************************************************************************/
struct netif * vnetif_iterate (struct netif *netif, struct netif *prev_vnetif);
struct netif * vnetif_find_by_src (struct netif * netif, ip_addr_t * src);
struct netif * vnetif_find_by_dest (struct netif * netif, ip_addr_t * dest);


#ifdef __cplusplus
}
#endif

#endif /* __AW_VNETIF_H */

/* end of file */
