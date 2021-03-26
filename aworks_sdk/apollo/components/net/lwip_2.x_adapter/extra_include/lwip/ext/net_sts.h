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
 * \brief ����״̬���
 *
 * \internal
 * \par modification history:
 * - 1.00 20-03-16  vih, first implementation
 * \endinternal
 */

#ifndef __NET_STS_H__
#define __NET_STS_H__

#include "aworks.h"
#include "aw_sem.h"
#include "aw_task.h"

#include "lwip/netif.h"


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \brief ����������Ĵ�����,�� aw_erro.h �������ô�����ʱ����
 */
typedef enum {
    AW_NET_STS_EBASE = 0xF000,
    AW_NET_STS_ECTL,           /**< \brief ��������Ч  */
    AW_NET_STS_EDEV,           /**< \brief �豸��Ч  */
    AW_NET_STS_EEVTDEV,        /**< \brief �¼������豸��Ч  */
    AW_NET_STS_EREMAN,         /**< \brief �ظ�����ǿ������   */
    AW_NET_STS_EEVTTYPE_MAN,   /**< \brief ǿ���������Ͳ�ƥ��   */
    AW_NET_STS_ENETIF,         /**< \brief ������������  */
} aw_net_sts_err_t;



#define AW_NET_STS_PRIORITY_MAX     0
#define AW_NET_STS_PRIORITY_MIN     0xFF

typedef struct aw_net_sts_evt aw_net_sts_evt_t;
typedef struct aw_net_sts_dev aw_net_sts_dev_t;
typedef void   (*aw_net_sts_log_t) (void         *p_color,
                                    const char   *func_name,
                                    int           line,
                                    const char   *fmt, ...);

typedef enum {
    AW_NET_STS_EVT_TYPE_LINK_DOWN = 0,
    AW_NET_STS_EVT_TYPE_LINK_INET,      /**< \brief �пɷ��� Ineternet ������ */
    AW_NET_STS_EVT_TYPE_LINK_SNET,      /**< \brief �пɷ���ָ����������� */

    /** \brief ����������� DNS server���ڸ�������Чʱ�����������ø� dns server */
    AW_NET_STS_EVT_TYPE_DNS,

    /** \brief �����ѯ ip, net_ctl ������ʹ�ü�¼�е� dns ������������𵽲�ѯ�ɹ�  */
    AW_NET_STS_EVT_TYPE_DNS_QUERY,

    /**
     * \brief ����ǿ�Ʒ��� Internet ��ָ���������������ʵ������״̬δ֪ ;
     *        ������豸�� netif ��δ��ȡ�������¼����ϱ����ӳ٣�ֱ���豸��ȡ�� netif �Ż��ϱ���
     *        �� 4G ģ��� ppp ��������Ҫ�ڵ�һ�� ppp ���ӳɹ���Ż��� netif������ ��Զ��ȡ���ˣ�
     */
    AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET,
    AW_NET_STS_EVT_TYPE_FORCE_LINK_SNET_SET,

    /** \brief ���ǿ�Ʒ��� Internet ��ָ�����������   */
    AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_CLR,
    AW_NET_STS_EVT_TYPE_FORCE_LINK_SNET_CLR,

    /** \brief ��һ��������ӵ��ظ�������������У�������������δ��������������е�      */
    AW_NET_STS_EVT_TYPE_NETIF_SUBNET_CHECK_ADD,
    AW_NET_STS_EVT_TYPE_NETIF_SUBNET_CHECK_DEL,

    /** \brief ��鵽�ظ��� ip ����ǰ׺������  */
    AW_NET_STS_EVT_TYPE_SUBNET_PREFIX_REPEAT,
} aw_net_sts_evt_type_t;

typedef struct aw_net_sts_evt {
    aw_net_sts_evt_type_t    type;

    union {
        struct {
            /** \brief ��Ҫʹ�� ip_addr_isany �ж�Ϊ��Ч */
            ip_addr_t                dns_server;
            struct netif            *p_netif;
        } dns;

        struct {
            const char              *host_name; /**< \brief ����ѯ����(��Ϊ ip)  */
            ip_addr_t                ip_addr;   /**< \brief ��ѯ���Ľ��   */

            /** \brief ǿ�Ʋ�ѯ��־ ����ʹ�ò���ɾ������  */
            aw_bool_t                flag_force_query;
        } dns_query;

        struct {
            /** \brief �����ĵ�ַ��ע������Ÿýṹ����λ�� */
            struct netif            *p_netif;

            /** \brief ֵԽС���ȼ�Խ�� */
            uint8_t                  prioty;
            aw_net_sts_dev_t        *p_dev;
            ip_addr_t                dns_server;
        } link;

        struct {
            /** \brief �����ĵ�ַ��ע������Ÿýṹ����λ�� */
            struct netif            *p_netif;
            aw_net_sts_dev_t        *p_dev;
            ip_addr_t                dns_server;
        } force_link;

        struct {
            struct netif            *p_netif[2];
        } subnet_prefix_repeat;

        /**
         * \brief AW_NET_STS_EVT_TYPE_NETIF_SUBNET_CHECK_ADD ��
         *        AW_NET_STS_EVT_TYPE_NETIF_SUBNET_CHECK_DEL ѡ�����
         */
        struct {
            struct netif            *p_netif;
        } netif_subnet_check;
    } u;
} aw_net_sts_evt_t;

typedef struct {
    void                   (*pfn_evt_handler) (aw_net_sts_evt_t *p_evt);
    aw_net_sts_log_t         pfn_log_out;
} aw_net_sts_ctl_init_t;

typedef struct aw_net_sts_ctl {
    struct aw_net_sts_ctl  *p_next;

    AW_MUTEX_DECL(          mutex);
    aw_net_sts_ctl_init_t   attr;

#define __NET_STS_DEV_ARRAY_MAX  8
    aw_net_sts_dev_t       *p_dev_arr[__NET_STS_DEV_ARRAY_MAX];

#define __NET_STS_EVT_ARRAY_MAX  8
    aw_net_sts_evt_t        sts_arr[__NET_STS_EVT_ARRAY_MAX];

    /**< \brief ��ǰ�ϱ� NET_OK ����Ϣ  */
    aw_net_sts_evt_t       *p_cur_sts;

    /** \brief �ȴ�ǿ��ָ�����豸��ȡ netif���иñ�־���ʾǿ��ָ��ʹ�õ������豸�������豸��δ��ȡ�� netif */
    aw_bool_t               flag_wait_force_netif;
    aw_net_sts_evt_t        wait_force_evt;

    /** \brief ��ʾ�������ڴ���������ѯ  */
    aw_bool_t               flag_dns_processing;
} aw_net_sts_ctl_t;


typedef enum {
    AW_NET_STS_4G_EVT_TYPE_POWER_OFF_PRE = 0,   /**< \brief 4G ģ�鼴������ */
    AW_NET_STS_4G_EVT_TYPE_POWER_OFF,           /**< \brief 4G ģ����ɵ��� */
    AW_NET_STS_4G_EVT_TYPE_POWER_ON,
    AW_NET_STS_4G_EVT_TYPE_NET_LINK_UP,         /**< \brief 4G ģ�齨����������  */
    AW_NET_STS_4G_EVT_TYPE_SOFT_RESET_PRE,      /**< \brief 4G ģ�鼴����λ */
    AW_NET_STS_4G_EVT_TYPE_SOFT_RESET,          /**< \brief 4G ģ�������λ */
    AW_NET_STS_4G_EVT_TYPE_NET_OK_FIRST,        /**< \brief 4G ģ���һ�μ����������� */
    AW_NET_STS_4G_EVT_TYPE_NET_OK,              /**< \brief 4G ģ���������� */
    AW_NET_STS_4G_EVT_TYPE_NET_ERR,
} aw_net_sts_4g_evt_type_t;

typedef struct aw_net_sts_4g_evt {
    aw_net_sts_4g_evt_type_t    type;
    int                         devid;
} aw_net_sts_4g_evt_t;

typedef enum {
    AW_NET_STS_DEV_TYPE_4G = 0,
    AW_NET_STS_DEV_TYPE_ETH ,
    AW_NET_STS_DEV_TYPE_WIFI,
} aw_net_sts_dev_type_t;

typedef struct {
    aw_net_sts_dev_type_t    type;
    char                     netif_name[8];
    uint8_t                  prioty;
    char                     task_name[16];

    /**
     * \brief
     * ���ڼ������״̬�ĵ�ַ������ ip ����������p_chk_inet_ip ���ȼ����� p_chk_snet_ip ,
     * p_chk_snet_ip ��ΪNULL ʱ�� ��� p_chk_inet_ip Ϊ NULL����ֻ���Ϊ
     * AW_NET_STS_EVT_TYPE_LINK_SNET
     */
    const char              *p_chk_snet_addr;
    const char              *p_chk_inet_addr;

    uint32_t                 chk_ms;        /**< ����� */
    aw_net_sts_log_t         pfn_log_out;

    union {
        struct {
            /**
             * \brief ����֪ͨӦ�� 4g ģ���״̬
             */
            void (*pfn_event_notify) (aw_net_sts_4g_evt_t *p_evt);
        } _4g;
    } u;
} aw_net_sts_dev_init_t;

#define AW_NET_STS_TASK_STACK_SIZE   (4*1024)

typedef struct aw_net_sts_dev {
    aw_net_sts_dev_init_t    attr;
    struct netif            *p_netif;
    aw_net_sts_ctl_t        *p_ctl;

    aw_bool_t                flag_busy;

    AW_SEMB_DECL(            task_sem);
    AW_TASK_DECL(            task, AW_NET_STS_TASK_STACK_SIZE);
    aw_err_t               (*pfn_startup) (aw_net_sts_dev_t *p_dev);
    aw_err_t               (*pfn_suspend) (aw_net_sts_dev_t *p_dev);
    aw_err_t               (*pfn_reinit) (aw_net_sts_dev_t      *p_dev,
                                          aw_net_sts_dev_init_t *p_init);
} aw_net_sts_dev_t;

typedef void (*aw_net_sts_ctl_iterate_cb_t) (aw_net_sts_ctl_t *p_ctl,
                                             void             *p_cb_arg);



aw_net_sts_ctl_t * aw_net_sts_ctl_create (aw_net_sts_ctl_init_t *p_init);
aw_net_sts_dev_t * aw_net_sts_dev_create (aw_net_sts_dev_init_t *p_init);
aw_err_t aw_net_sts_dev_destroy (aw_net_sts_dev_t  *p_dev);

/**
 * \brief �����������������
 *
 * \param   pfn_iterate_cb  Ӧ�ûص�
 * \param   p_cb_arg        �ص���������Ϊ NULL
 *
 * \retval  AW_OK                   �����ɹ�
 * \retval  -AW_EINVAL              ��������
 * \retval  -AW_NET_STS_ECTL        �� ctl
 */
aw_err_t aw_net_sts_ctl_iterate (
                aw_net_sts_ctl_iterate_cb_t  pfn_iterate_cb,
                void                        *p_cb_arg);

/**
 * \brief �������ó�ʼ������
 *
 * \param   p_ctl   ����״̬�����豸
 * \param   p_dev   ����״̬����豸
 * \param   p_init  �²���
 *
 * \retval  AW_OK                   �����ɹ�
 * \retval  -AW_EBUSY               �豸��æ���Ժ�����
 * \retval  -AW_EINVAL              p_init->type ���Ͳ�֧���޸�
 * \retval  -AW_ENOTSUP             ���豸��֧�� reinit ����
 * \retval  -AW_ENOSR               ���豸δ��ӵ� ctl ��
 */
aw_err_t aw_net_sts_dev_reinit (aw_net_sts_ctl_t      *p_ctl,
                                aw_net_sts_dev_t      *p_dev,
                                aw_net_sts_dev_init_t *p_init);

/**
 * \brief ��Ӳ���������״̬����豸
 *
 * \param   p_ctl   ����״̬�����豸
 * \param   p_dev   ����״̬����豸
 *
 * \retval  AW_OK                   �����ɹ�
 * \retval  -AW_EEXIST              �豸�ѱ���Ӹ� ctl
 * \retval  -AW_EFAULT              �豸�ѱ���ӵ������ ctl
 * \retval  -AW_EBUSY               �豸��æ���Ժ�����
 */
aw_err_t aw_net_sts_add (aw_net_sts_ctl_t *p_ctl, aw_net_sts_dev_t *p_dev);

/**
 * \brief ������������ɾ�����豸
 *
 * \param   p_ctl   ����״̬�����豸
 * \param   p_dev   ����״̬����豸
 *
 * \retval  AW_OK                   �����ɹ�
 * \retval  -AW_EEXIST              �豸δ��ӵ��κ� ctl
 * \retval  -AW_EFAULT              �豸�ѱ���ӵ������ ctl
 * \retval  -AW_EBUSY               �豸��æ���Ժ�����
 */
aw_err_t aw_net_sts_del (aw_net_sts_ctl_t *p_ctl, aw_net_sts_dev_t *p_dev);

aw_bool_t aw_net_sts_dev_is_free (aw_net_sts_dev_t *p_dev);
aw_bool_t aw_net_sts_dev_is_busy (aw_net_sts_dev_t *p_dev);
/**
 * \brief ��ȡ���µ�״̬�¼�
 *
 * \detail ��ȡ���� p_out ���¼�����ֵΪ  AW_NET_STS_EVT_TYPE_LINK_UP ���ʾ�пɷ�������������
 *         ����Ϊ p_out->p_netif��
 *         ����¼�����ֵΪ AW_NET_STS_EVT_TYPE_LINK_DOWN�����ʾ��ǰ�޿ɷ�������������
 *
 * \param[in]   p_ctl ������
 * \param[out]  p_out ����״̬�¼��ĵ�ַ
 *
 * \retval  AW_OK   �����ɹ�
 */
aw_err_t aw_net_sts_get (aw_net_sts_ctl_t *p_ctl, aw_net_sts_evt_t *p_out);

/**
 * \brief �¼�����
 *
 * \param[in]   p_ctl ������
 * \param[in]   p_evt �¼�
 *
 * \retval  AW_OK               �����ɹ�
 * \retval  else                �鿴 aw_net_sts_err_t ��������
 */
aw_err_t aw_net_sts_evt_proc (aw_net_sts_ctl_t *p_ctl, aw_net_sts_evt_t *p_evt);


#ifdef __cplusplus
}
#endif /* __cplusplus   */

#endif /* __NET_STS_H__ */

/* end of file */

