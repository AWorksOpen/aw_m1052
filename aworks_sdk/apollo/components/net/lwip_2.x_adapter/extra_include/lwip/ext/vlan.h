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
 * \brief ���������
 *
 * \internal
 * \par modification history:
 * - 1.00 20-03-16  vih, first implementation
 * \endinternal
 */

#ifndef __VLAN_H__
#define __VLAN_H__

#include "aworks.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


typedef enum {
    VLAN_ACCESS_LINK_MAC,
    VLAN_ACCESS_LINK_PORT,
    VLAN_TRUNK_LINK,
} vlan_link_t;

typedef struct {
    vlan_link_t      mode;
    uint16_t         vid;
    struct eth_addr  eth;
} vlan_entry_t;

aw_err_t vlan_init (struct netif *bridgeif,
                    vlan_entry_t *p_entry,
                    uint16_t entry_num);

/**
 * \brief
 * \note �����ظ�����
 *
 * \param   p_entyr         vlan ����,���ڴ治�����ͷ�
 * \param   entry_num       ���鳤��
 *
 * \retval  AW_OK                   �����ɹ�
 * \retval  -AW_EINVAL              ��������
 */
aw_err_t vlan_port_add (struct netif *portif, vlan_entry_t *p_entry);

aw_err_t vlan_port_del (struct netif *portif);
vlan_entry_t * vlan_port_entry_get (struct netif *portif);

/**
 * \brief
 * \note �����ظ�����
 *
 * \param   portif          Ϊת���˿�
 * \param   p               ��ת��eth֡
 *
 * \retval  AW_OK                   �����ɹ�
 * \retval  -AW_EINVAL              ��������
 */
aw_bool_t vlan_port_forward_allow (struct netif *portif, struct pbuf *p);

#ifdef __cplusplus
}
#endif /* __cplusplus   */


#endif __VLAN_H__

/* end of file */
