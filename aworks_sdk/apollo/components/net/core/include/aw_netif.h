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
 * \brief ����ӿڹ���
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ�
 * \code
 * #include "aw_netif.h"
 * \endcode
 *
 * \internal
 * \par Modification History
 * - 1.00 18-03-25  xdn, first implementation
 * - 1.00 18-04-11  xdn, change interface to netif
 * \endinternal
 */

#ifndef __AW_NETIF_H
#define __AW_NETIF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_netif
 * \copydoc aw_netif.h
 * @{
 */

#include "aworks.h"
#include "aw_refcnt.h"
#include "aw_spinlock.h"

#include "lwip/opt.h"
#include "aw_netif_adapter.h"
#include "aw_net_buf.h"

#ifdef AW_NETIF_EN_PCAP
#include "aw_pcap.h"
#endif


/**
 * \name ����״̬�䶯�¼�
 * @{
 */

/** \brief ����״̬�䶯�¼����� */
#define AW_NETIF_LINK_EVENT_HANDLER     1

/** \brief ����״̬�䶯�¼���������� */
#define AW_NETIF_LINK_EVENT_HDL_MAX     2

/** @} */

/**
 * \name ��·����
 * @{
 */

/** \brief Ӳ����ַ��󳤶� */
#define AW_NETIF_MAX_HWADDR_LEN         6

/** @} */

/**
 * \name ��������
 * @{
 */

/** @} */

/**
 * \name ����״̬
 * @{
 */

/** \brief �������û򲻿��� */
#define AW_NETIF_STAT_EXIST             AW_BIT(0)

/** \brief ʹ��ͨ�� */
#define AW_NETIF_STAT_DEV_UP            AW_BIT(1)

/** \brief ��·���� */
#define AW_NETIF_STAT_LINK_UP           AW_BIT(2)

/** \brief ��DHCP����IP��ַ */
#define AW_NETIF_STAT_DHCP_ON           AW_BIT(3)

/** @} */

/**
 * \name �������
 * @{
 */

typedef enum {
    AW_NETIF_OPT_SET,
    AW_NETIF_OPT_GET,
} aw_netif_ioctl_ops_t;

/** \brief ����ץ������ */
#define AW_NETIF_OPT_PKTDUMP            AW_BIT(0)

#ifdef LWIP_2_X
/** \brief �鲥��ַ */
#define AW_NETIF_OPT_MCAST_JOIN         AW_BIT(1)
#define AW_NETIF_OPT_MCAST_LEAVE        AW_BIT(2)
#define AW_NETIF_OPT_ETHADDR_FILTER     AW_BIT(3)
#elif defined LWIP_1_4_X
#else
#error "no lwipopts.h"
#endif

/** @} */

/******************************************************************************/

#define DBG_PRINT_ETHHWADDR_FMT \
    "%02X:%02X:%02X:%02X:%02X:%02X"

#define DBG_PRINT_IP4ADDR_FMT       "%d.%d.%d.%d"

#define DBG_PRINT_ETHHWADDR_VAL(addr) \
    (addr)[0], (addr)[1], (addr)[2], (addr)[3], (addr)[4], (addr)[5]

#define DBG_PRINT_IP4ADDR_VAL(addr) \
    (addr) & 0x000000ff, \
   ((addr) & 0x0000ff00) >> 8, \
   ((addr) & 0x00ff0000) >> 16, \
    (addr) >> 24

/******************************************************************************/

/** \brief �������� */
typedef enum {
    AW_NETIF_TYPE_RAW,      /* �ݲ�֧�� */
    AW_NETIF_TYPE_ETHERNET,
    AW_NETIF_TYPE_WIFI,
    AW_NETIF_TYPE_PPPOS,
    AW_NETIF_TYPE_PPPOE,
} aw_netif_type_t;

/** \brief ����״̬�䶯�¼� */
typedef enum {
    AW_NETIF_EVENT_DEV_REMOVED,
    AW_NETIF_EVENT_DEV_UP,
    AW_NETIF_EVENT_DEV_DOWN,
    AW_NETIF_EVENT_LINK_UP,
    AW_NETIF_EVENT_LINK_DOWN,
} aw_netif_event_t;

/** \brief ��̫����·Ӳ����ַ */
typedef struct {
    uint8_t octet[6];
} aw_eth_mac_t;

typedef struct aw_netif aw_netif_t;
typedef void (*aw_netif_iterate_cb_t) (aw_netif_t *p_netif, void *p_arg);
typedef void (*aw_netif_ev_hdl_t) (aw_netif_t *p_netif, aw_netif_event_t event, void *p_arg);

/** \brief ������·�䶯�¼����ļ�¼ */
typedef struct {
    aw_netif_ev_hdl_t pfn;
    void *p_arg;
} aw_netif_ev_list_elem_t;

/** \brief ���������ṩ�Ļ������� */
typedef struct aw_netif_ops {
    aw_err_t (*init)    (aw_netif_t *p_netif);
    aw_err_t (*up)      (aw_netif_t *p_netif);
    aw_err_t (*down)    (aw_netif_t *p_netif);
    aw_err_t (*output)  (aw_netif_t *p_netif, aw_net_buf_t *p);
    aw_err_t (*io_ctl)  (aw_netif_t *p_netif, int ops, int name, void *p_arg);
    void     (*release) (aw_netif_t *p_netif);
} aw_netif_ops_t;

/** \brief Ĭ�ϲ������� */
typedef struct aw_netif_info_get_entry {
    char     *(*get_ipaddr)      (void); /**< \brief ָ��������IP��ַ */
    char     *(*get_netmsk)      (void); /**< \brief ָ���������������� */
    char     *(*get_gateway)     (void); /**< \brief ָ�����������ص�ַ */
    
    /** \brief ָ��������MAC��ַ */
    aw_err_t  (*get_hwaddr)      (uint8_t *p_hwaddr, int addr_len); 
    
    /** \brief Ĭ������DHCP��ȡ��ַ����Ҫ����DHCP�ͻ��˹��ܣ� */
    aw_bool_t    (*get_dhcp_en)  (void); 
} aw_netif_info_get_entry_t;

/** \brief �����������ṩʵ�� */
struct aw_netif {
    aw_netif_t              *p_next;
    const char              *p_name;
    const aw_netif_ops_t    *p_ops;
    uint8_t                  hwaddr[AW_NETIF_MAX_HWADDR_LEN];
    uint16_t                 mtu;
    uint8_t                  hwaddr_len;
    aw_netif_type_t          type;
    struct aw_refcnt         refcnt;
    uint16_t                 state;     /**< \brief ״̬ */
    uint16_t                 flags;     /**< \brief ���� */
    struct netif             netif;
    struct dhcp              dhcpc;
#if AW_NETIF_LINK_EVENT_HANDLER
    aw_netif_ev_list_elem_t  event_list[AW_NETIF_LINK_EVENT_HDL_MAX];
#endif

#ifdef LWIP_2_X
#if LWIP_IPV6 && LWIP_IPV6_DHCP6
    struct dhcp6 dhcp6;
#endif
#elif defined LWIP_1_4_X
#else
#error "no lwipopts.h"
#endif

#ifdef AW_NETIF_EN_PCAP
    aw_pcap_out_fn pcap_out;
#else
    aw_pfuncvoid_t pfn_pad0; /**< \brief ���� size �ı� */

    void          *p_phy_layer_data;
#endif
};

/**
 * \brief ��ʼ�������ӿڹ�����
 */
aw_err_t aw_netif_init (void);

/*******************************************************************************
 * �����ӿڹ�������ʹ�ã�
 ******************************************************************************/

/**
 * \brief ע��һ������
 *
 * ��һ�����õ���ΪϵͳĬ��
 *
 * \param[in] p_netif  ʵ����ָ��
 * \param[in] p_name   ����
 * \param[in] p_ops    �����ṩ�Ļ�������
 * \param[in] p_entry  �ṩĬ�ϲ���
 * \param[in] type     ��·����
 * \param[in] p_hwaddr Ӳ����ַ
 * \param[in] mtu      ����䵥Ԫ
 */
aw_err_t aw_netif_add (aw_netif_t                      *p_netif,
                       const char                      *p_name,
                       const aw_netif_ops_t            *p_ops,
                       const aw_netif_info_get_entry_t *p_entry,
                       aw_netif_type_t                  type,
                       uint8_t                         *p_hwaddr,
                       uint16_t                         mtu);

/**
 * \brief ɾ��һ������
 *
 * \param[in] p_netif  ʵ����ָ��
 */
aw_err_t aw_netif_remove (aw_netif_t *p_netif);

/**
 * \brief ���յ������ݰ�����TCP/IPЭ��ջ���ڲ�������
 *
 *  �����Լ���������������ݣ��ڸ������е���
 *
 * \param[in] p_netif ʵ�����
 * \param[in] p_mem   ���ݰ�����
 * \param[in] len     ���ݰ�����
 *
 * \retval AW_OK       �ͷųɹ�
 * \retval -AW_EFAULT  �ͷ�ʧ��
 */
aw_err_t aw_netif_input (aw_netif_t *p_netif, void *p_mem, size_t len);

/**
 * \brief ���յ������ݰ�����TCP/IPЭ��ջ���������п�����
 *
 *  �����Լ���������������ݣ��ڸ������е���
 *
 * \param[in] p_netif  ʵ�����
 * \param[in] len      ���ݰ��ܳ���
 * \param[in] p_ctx    �����������Ӳ���
 * \param[in] pfn_copy ��������
 *
 * \retval AW_OK       �ͷųɹ�
 * \retval -AW_EFAULT  �ͷ�ʧ��
 */
aw_err_t aw_netif_input_chain (aw_netif_t       *p_netif,
                               size_t            len,
                               void             *p_ctx,
                               aw_net_buf_cpy_t  pfn_copy);

/**
 * \brief ����������·Ϊ�Ͽ���״̬
 *
 * \param[in] p_netif  ʵ�����
 *
 * \retval AW_OK       �����ɹ�
 * \retval -AW_ENODEV  �豸������
 */
aw_err_t aw_netif_set_link_up (aw_netif_t *p_netif);

/**
 * \brief ����������·Ϊ�Ͽ�״̬
 *
 * \param[in] p_netif  ʵ�����
 *
 * \retval AW_OK       �����ɹ�
 * \retval -AW_ENODEV  �豸������
 */
aw_err_t aw_netif_set_link_down (aw_netif_t *p_netif);

/*******************************************************************************
 * �����ӿڹ���Ӧ�ò�ʹ�ã�
 ******************************************************************************/

/**
 * \brief �������豸
 *
 * Ӧ�ò��ȵ��øú���ȡ�þ����ʹ�ã�����ǵùر�
 *
 * \param[in] p_name ��������
 *
 * \return ʵ�����
 */
aw_netif_t *aw_netif_dev_open (const char *p_name);

#ifdef LWIP_2_X
/**
 * \brief ��Ĭ�������豸
 *
 * ����ǵùر�
 *
 * \return ʵ�����
 */
aw_netif_t *aw_netif_dev_open_default (void);
#elif defined LWIP_1_4_X
#else
#error "no lwipopts.h"
#endif

/**
 * \brief �ر������豸
 *
 * �����Ƴ��豸�����һ�ιرյ��ý�������������ʵ��
 *
 * \param[in] p_netif ʵ�����
 *
 * \retval AW_OK       �ͷųɹ�
 * \retval -AW_ENODEV  �豸������
 * \retval -AW_EPERM   �ͷ�ʧ��
 */
aw_err_t aw_netif_dev_close (aw_netif_t *p_netif);

/**
 * \brief �������
 *
 *  ���幦���������ṩ
 *
 * \param[in]     p_netif  ʵ�����
 * \param[in]     ops      ����
 * \param[in]     name     ����
 * \param[in,out] p_arg    ����
 *
 * \retval AW_OK       �����ɹ�
 * \retval -AW_ENODEV  �豸������
 * \retval -AW_ENOTSUP �豸��֧��
 */
aw_err_t aw_netif_io_ctl (aw_netif_t *p_netif,
                          int         ops,
                          int         name,
                          void       *p_arg);

/**
 * \brief ���������¼�
 *
 * \param[in] p_netif ʵ�����
 * \param[in] pfn     �¼�����
 * \param[in] p_arg   ���Ӳ���
 *
 * \retval AW_OK      �����ɹ�
 * \retval -AW_ENODEV �豸������
 * \retval -AW_EAGAIN ��������
 */
aw_err_t aw_netif_event_cb_register (aw_netif_t        *p_netif,
                                     aw_netif_ev_hdl_t  pfn,
                                     void              *p_arg);

/**
 * \brief ȡ�����������¼�
 *
 * \param[in] p_netif ʵ�����
 * \param[in] pfn     �¼�����
 * \param[in] p_arg   ���Ӳ���
 *
 * \retval AW_OK      �����ɹ�
 * \retval -AW_ENODEV �豸������
 * \retval -AW_EAGAIN ��������
 */
aw_err_t aw_netif_event_cb_unregister (aw_netif_t *p_netif, aw_netif_ev_hdl_t pfn);

/*******************************************************************************
 * �����ӿڹ�������/Ӧ�ò�Կ�ʹ�ã�
 ******************************************************************************/

/**
 * \brief ����Ĭ������
 *
 *  ��һ������ aw_netif_add ����ϵͳĬ��
 *
 * \param[in] p_netif  ʵ�����
 *
 * \retval AW_OK       �����ɹ�
 * \retval -AW_ENODEV  �豸������
 * \retval AW_ERROR    ����ʧ��
 */
aw_err_t aw_netif_set_default (aw_netif_t *p_netif);

/**
 * \brief ��������Ϊ����״̬
 *
 * \param[in] p_netif  ʵ�����
 *
 * \retval AW_OK       �����ɹ�
 * \retval -AW_ENODEV  �豸������
 */
aw_err_t aw_netif_up (aw_netif_t *p_netif);

/**
 * \brief ��������Ϊ������״̬
 *
 * \param[in] p_netif  ʵ�����
 *
 * \retval AW_OK       �����ɹ�
 * \retval -AW_ENODEV  �豸������
 */
aw_err_t aw_netif_down (aw_netif_t *p_netif);

/**
 * \brief ��ȡ��������״̬
 *
 * \param[in]  p_netif ʵ�����
 * \param[out] p_up    ͨ��״̬
 *
 * \retval AW_OK       �����ɹ�
 * \retval -AW_ENODEV  �豸������
 */
aw_err_t aw_netif_is_up (aw_netif_t *p_netif, aw_bool_t *p_up);

/**
 * \brief ��ȡ������·״̬
 *
 * \param[in]  p_netif ʵ�����
 * \param[out] p_up    ��·״̬
 *
 * \retval AW_OK       �����ɹ�
 * \retval -AW_ENODEV  �豸������
 */
aw_err_t aw_netif_is_link_up (aw_netif_t *p_netif, aw_bool_t *p_up);

/**
 * \brief ��ȡ����״̬
 *
 * \param[in]  p_netif  ʵ�����
 * \param[out] p_state  ״̬ԭʼֵ
 *
 * \retval AW_OK       �����ɹ�
 * \retval -AW_ENODEV  �豸������
 */
aw_err_t aw_netif_state_get (aw_netif_t *p_netif, uint32_t *p_state);

/**
 * \brief �豸�Ƿ����
 *
 * \param[in] p_netif ʵ�����
 *
 * \return �豸�������
 */
aw_static_inline aw_bool_t aw_netif_is_exist (aw_netif_t *p_netif)
{
    return (p_netif && (p_netif->state & AW_NETIF_STAT_EXIST));
}

/**
 * \brief �豸�Ƿ����
 *
 * \param[in] p_netif ʵ�����
 *
 * \return �豸�������
 */
aw_static_inline aw_bool_t aw_netif_is_ready (aw_netif_t *p_netif)
{
    return (p_netif && 
            (p_netif->state & (AW_NETIF_STAT_EXIST | AW_NETIF_STAT_DEV_UP)));
}

/**
 * \brief ��ȡ�豸����
 *
 * \param[in]  p_netif ʵ�����
 * \param[out] p_type  �豸����
 *
 * \retval AW_OK       �����ɹ�
 * \retval -AW_ENODEV  �豸������
 */
aw_err_t aw_netif_type_get (aw_netif_t *p_netif, uint8_t *p_type);

/**
 * \brief ��ȡ�豸MTU
 *
 * \param[in]  p_netif ʵ�����
 * \param[out] p_mtu   MTU
 *
 * \retval AW_OK       �����ɹ�
 * \retval -AW_ENODEV  �豸������
 */
aw_err_t aw_netif_mtu_get (aw_netif_t *p_netif, uint16_t *p_mtu);

/**
 * \brief ��ȡӲ����ַ
 *
 * \param[in]  p_netif ʵ�����
 * \param[out] addr    ����Ӳ����ַ�Ļ�����
 * \param[in]  len     ��������С
 *
 * \retval AW_OK       �����ɹ�
 * \retval -AW_ENODEV  �豸������
 */
aw_err_t aw_netif_hwaddr_get (aw_netif_t *p_netif, uint8_t *addr, uint8_t len);

/**
 * \brief ��ȡ�豸����
 *
 * \param[in]  p_netif ʵ�����
 * \param[out] p_name  �����豸���Ƶ�ָ��
 *
 * \retval AW_OK       �����ɹ�
 * \retval -AW_ENODEV  �豸������
 */
aw_err_t aw_netif_name_get (aw_netif_t *p_netif, char **p_name);

/**
 * \brief ���������豸
 *
 * \param[in] cb    ���������ĺ���
 * \param[in] p_arg ���Ӳ���
 */
void aw_netif_iterate (aw_netif_iterate_cb_t cb, void *p_arg);

/*******************************************************************************
 * IP��ַ����
 ******************************************************************************/

/**
 * \brief ����IP��ַ
 *
 * \param[in]  p_netif ʵ�����
 * \param[in]  addr    IP��ַ
 *
 * \retval AW_OK       �����ɹ�
 * \retval -AW_ENODEV  �豸������
 */
aw_err_t aw_netif_ipv4_ip_set (aw_netif_t *p_netif, struct in_addr *addr);

/**
 * \brief ��ȡIP��ַ
 *
 * \param[in]  p_netif ʵ�����
 * \param[out] addr    IP��ַ
 *
 * \retval AW_OK       �����ɹ�
 * \retval -AW_ENODEV  �豸������
 */
aw_err_t aw_netif_ipv4_ip_get (aw_netif_t *p_netif, struct in_addr *addr);

/**
 * \brief ������������
 *
 * \param[in]  p_netif ʵ�����
 * \param[in]  addr    ��������
 *
 * \retval AW_OK       �����ɹ�
 * \retval -AW_ENODEV  �豸������
 */
aw_err_t aw_netif_ipv4_netmask_set (aw_netif_t *p_netif, struct in_addr *addr);

/**
 * \brief ��ȡ��������
 *
 * \param[in]  p_netif ʵ�����
 * \param[out] addr    ��������
 *
 * \retval AW_OK       �����ɹ�
 * \retval -AW_ENODEV  �豸������
 */
aw_err_t aw_netif_ipv4_netmask_get (aw_netif_t *p_netif, struct in_addr *addr);

/**
 * \brief �������ص�ַ
 *
 * \param[in]  p_netif ʵ�����
 * \param[in]  addr    ���ص�ַ
 *
 * \retval AW_OK       �����ɹ�
 * \retval -AW_ENODEV  �豸������
 */
aw_err_t aw_netif_ipv4_gw_set (aw_netif_t *p_netif, struct in_addr *addr);

/**
 * \brief ��ȡ���ص�ַ
 *
 * \param[in]  p_netif ʵ�����
 * \param[out] addr    ���ص�ַ
 *
 * \retval AW_OK       �����ɹ�
 * \retval -AW_ENODEV  �豸������
 */
aw_err_t aw_netif_ipv4_gw_get (aw_netif_t *p_netif, struct in_addr *addr);

/**
 * \brief ����IP��ַ��������������ص�ַ
 *
 * \param[in] p_name    ��������
 * \param[in] p_ipaddr  IP��ַ
 * \param[in] p_netmask ��������
 * \param[in] p_gateway ���ص�ַ
 *
 * \retval AW_OK        �����ɹ�
 * \retval -AW_ENODEV   �豸������
 */
aw_err_t aw_netif_ipv4_addr_set_by_name (const char     *p_name,
                                         struct in_addr *p_ipaddr,
                                         struct in_addr *p_netmask,
                                         struct in_addr *p_gateway);

/**
 * \brief ��ȡIP��ַ��������������ص�ַ
 *
 * \param[in]  p_name    ��������
 * \param[out] p_ipaddr  IP��ַ
 * \param[out] p_netmask ��������
 * \param[out] p_gateway ���ص�ַ
 *
 * \retval AW_OK         �����ɹ�
 * \retval -AW_ENODEV    �豸������
 */
aw_err_t aw_netif_ipv4_addr_get_by_name (const char     *p_name,
                                         struct in_addr *p_ipaddr,
                                         struct in_addr *p_netmask,
                                         struct in_addr *p_gateway);

/**
 * \brief ��ȡӲ����ַ
 *
 * \param[in]  p_name  ��������
 * \param[out] addr    ����Ӳ����ַ�Ļ�����
 * \param[in]  len     ��������С
 *
 * \retval AW_OK       �����ɹ�
 * \retval -AW_ENODEV  �豸������
 */
aw_err_t aw_netif_hwaddr_get_by_name (const char *p_name, 
                                      uint8_t    *p_addr,
                                      uint8_t     len);

/**
 * dhcp���
 */
aw_err_t aw_netif_dhcpc_start (aw_netif_t *p_netif);

aw_err_t aw_netif_dhcpc_stop (aw_netif_t *p_netif);

aw_err_t aw_netif_dhcpc_is_on (aw_netif_t *p_netif, aw_bool_t *on);

aw_err_t aw_netif_dhcpc_is_bound (aw_netif_t *p_netif, aw_bool_t *bound);

aw_err_t aw_netif_dhcpc_start_by_name (const char *p_name);

aw_err_t aw_netif_dhcpc_stop_by_name (const char *p_name);

/** @} grp_aw_if_netif */

#ifdef __cplusplus
}
#endif

#endif /* __AW_NETIF_H */

/* end of file */
