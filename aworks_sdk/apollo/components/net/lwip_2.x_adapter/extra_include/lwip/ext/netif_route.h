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
 * \brief �򵥾�̬·�ɱ�ӿ�
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ�
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
 * \brief ���һ��·�ɣ�ָ�� ip ���ɸ��������
 *
 * \param[in] netif         ������ַ
 * \param[in] p_dst_ipaddr  Ŀ�ĵ�ַ������Ϊ�գ�ֵ����Ϊ any
 */
void netif_route_add (struct netif *netif,
                      ip_addr_t *p_dst_ipaddr);

/**
 * \brief ɾ��һ��·��
 *
 * \param[in] netif         ����
 * \param[in] p_dst_ipaddr  Ŀ�ĵ�ַ������Ϊ��
 */
void netif_route_del (struct netif *netif, ip_addr_t *p_dst_ipaddr);

/**
 * \brief ͨ�� ip ��ַ��ѯ·�ɱ��е��������
 *
 * \param[in] p_src_ipaddr  Դ��ַ������Ϊ�գ�������ֵ�ſ���ƥ��ɹ�
 * \param[in] p_dst_ipaddr  Ŀ�ĵ�ַ������Ϊ�գ�������ֵ�ſ���ƥ��ɹ�
 *
 * \retval !NULL  ���ҳɹ�
 * \retval NULL   δ�ҵ�
 */
struct netif *netif_route_find (ip_addr_t *p_src_ipaddr, ip_addr_t *p_dst_ipaddr);

/**
 * \brief �޸�һ��·����Ŀ��ֻ�޸�Ŀ�ĵ�ַ�����޸�����
 *
 * \param[in] p_old_dst_ipaddr  �� ip������Ϊ��
 * \param[in] p_new_dst_ipaddr  �� ip������Ϊ��
 * \param[in] netif             ������ַ
 *
 * \retval  AW_OK       �����ɹ�
 * \retval  -AW_EINVAL  ������Ч
 */
int netif_route_change (ip_addr_t *p_old_dst_ipaddr,
                        ip_addr_t *p_new_dst_ipaddr,
                        struct netif *netif);



#ifdef __cplusplus
}
#endif

#endif /* __NETIF_ROUTE_H__ */

/* end of file */
