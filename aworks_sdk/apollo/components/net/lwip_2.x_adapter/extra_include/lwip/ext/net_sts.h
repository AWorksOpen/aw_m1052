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
 * \brief 网络状态检查
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
 * \brief 多网卡管理的错误码,当 aw_erro.h 中无适用错误码时定义
 */
typedef enum {
    AW_NET_STS_EBASE = 0xF000,
    AW_NET_STS_ECTL,           /**< \brief 控制器无效  */
    AW_NET_STS_EDEV,           /**< \brief 设备无效  */
    AW_NET_STS_EEVTDEV,        /**< \brief 事件参数设备无效  */
    AW_NET_STS_EREMAN,         /**< \brief 重复设置强制网卡   */
    AW_NET_STS_EEVTTYPE_MAN,   /**< \brief 强制网卡类型不匹配   */
    AW_NET_STS_ENETIF,         /**< \brief 网卡参数错误  */
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
    AW_NET_STS_EVT_TYPE_LINK_INET,      /**< \brief 有可访问 Ineternet 的网卡 */
    AW_NET_STS_EVT_TYPE_LINK_SNET,      /**< \brief 有可访问指定网络的网卡 */

    /** \brief 保存该网卡的 DNS server，在该网卡有效时管理器会设置该 dns server */
    AW_NET_STS_EVT_TYPE_DNS,

    /** \brief 请求查询 ip, net_ctl 会依次使用记录中的 dns 来处理该请求，起到查询成功  */
    AW_NET_STS_EVT_TYPE_DNS_QUERY,

    /**
     * \brief 设置强制访问 Internet 或指定网络的网卡，其实际网络状态未知 ;
     *        如果该设备的 netif 还未获取到，则事件的上报会延迟，直到设备获取到 netif 才会上报；
     *        如 4G 模块的 ppp 网卡，需要在第一次 ppp 连接成功后才会有 netif，否则 永远获取不了；
     */
    AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_SET,
    AW_NET_STS_EVT_TYPE_FORCE_LINK_SNET_SET,

    /** \brief 清除强制访问 Internet 或指定网络的网卡   */
    AW_NET_STS_EVT_TYPE_FORCE_LINK_INET_CLR,
    AW_NET_STS_EVT_TYPE_FORCE_LINK_SNET_CLR,

    /** \brief 将一个网卡添加到重复子网检测任务中，该网卡必须是未加入多网卡管理中的      */
    AW_NET_STS_EVT_TYPE_NETIF_SUBNET_CHECK_ADD,
    AW_NET_STS_EVT_TYPE_NETIF_SUBNET_CHECK_DEL,

    /** \brief 检查到重复的 ip 子网前缀的网卡  */
    AW_NET_STS_EVT_TYPE_SUBNET_PREFIX_REPEAT,
} aw_net_sts_evt_type_t;

typedef struct aw_net_sts_evt {
    aw_net_sts_evt_type_t    type;

    union {
        struct {
            /** \brief 需要使用 ip_addr_isany 判断为有效 */
            ip_addr_t                dns_server;
            struct netif            *p_netif;
        } dns;

        struct {
            const char              *host_name; /**< \brief 待查询域名(可为 ip)  */
            ip_addr_t                ip_addr;   /**< \brief 查询到的结果   */

            /** \brief 强制查询标志 ，不使用并会删除缓存  */
            aw_bool_t                flag_force_query;
        } dns_query;

        struct {
            /** \brief 网卡的地址；注：必须放该结构体首位置 */
            struct netif            *p_netif;

            /** \brief 值越小优先级越高 */
            uint8_t                  prioty;
            aw_net_sts_dev_t        *p_dev;
            ip_addr_t                dns_server;
        } link;

        struct {
            /** \brief 网卡的地址；注：必须放该结构体首位置 */
            struct netif            *p_netif;
            aw_net_sts_dev_t        *p_dev;
            ip_addr_t                dns_server;
        } force_link;

        struct {
            struct netif            *p_netif[2];
        } subnet_prefix_repeat;

        /**
         * \brief AW_NET_STS_EVT_TYPE_NETIF_SUBNET_CHECK_ADD 和
         *        AW_NET_STS_EVT_TYPE_NETIF_SUBNET_CHECK_DEL 选项参数
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

    /**< \brief 当前上报 NET_OK 的消息  */
    aw_net_sts_evt_t       *p_cur_sts;

    /** \brief 等待强制指定的设备获取 netif；有该标志则表示强制指定使用的网络设备，但该设备还未获取到 netif */
    aw_bool_t               flag_wait_force_netif;
    aw_net_sts_evt_t        wait_force_evt;

    /** \brief 表示任务正在处理域名查询  */
    aw_bool_t               flag_dns_processing;
} aw_net_sts_ctl_t;


typedef enum {
    AW_NET_STS_4G_EVT_TYPE_POWER_OFF_PRE = 0,   /**< \brief 4G 模块即将掉电 */
    AW_NET_STS_4G_EVT_TYPE_POWER_OFF,           /**< \brief 4G 模块完成掉电 */
    AW_NET_STS_4G_EVT_TYPE_POWER_ON,
    AW_NET_STS_4G_EVT_TYPE_NET_LINK_UP,         /**< \brief 4G 模块建立拨号连接  */
    AW_NET_STS_4G_EVT_TYPE_SOFT_RESET_PRE,      /**< \brief 4G 模块即将软复位 */
    AW_NET_STS_4G_EVT_TYPE_SOFT_RESET,          /**< \brief 4G 模块完成软复位 */
    AW_NET_STS_4G_EVT_TYPE_NET_OK_FIRST,        /**< \brief 4G 模块第一次检测出网络正常 */
    AW_NET_STS_4G_EVT_TYPE_NET_OK,              /**< \brief 4G 模块网络正常 */
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
     * 用于检查网络状态的地址（可用 ip 和域名），p_chk_inet_ip 优先级高于 p_chk_snet_ip ,
     * p_chk_snet_ip 不为NULL 时， 如果 p_chk_inet_ip 为 NULL，则只检测为
     * AW_NET_STS_EVT_TYPE_LINK_SNET
     */
    const char              *p_chk_snet_addr;
    const char              *p_chk_inet_addr;

    uint32_t                 chk_ms;        /**< 检查间隔 */
    aw_net_sts_log_t         pfn_log_out;

    union {
        struct {
            /**
             * \brief 用于通知应用 4g 模块的状态
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
 * \brief 遍历网卡管理控制器
 *
 * \param   pfn_iterate_cb  应用回调
 * \param   p_cb_arg        回调参数，可为 NULL
 *
 * \retval  AW_OK                   操作成功
 * \retval  -AW_EINVAL              参数错误
 * \retval  -AW_NET_STS_ECTL        无 ctl
 */
aw_err_t aw_net_sts_ctl_iterate (
                aw_net_sts_ctl_iterate_cb_t  pfn_iterate_cb,
                void                        *p_cb_arg);

/**
 * \brief 重新设置初始化参数
 *
 * \param   p_ctl   网卡状态管理设备
 * \param   p_dev   网卡状态检查设备
 * \param   p_init  新参数
 *
 * \retval  AW_OK                   操作成功
 * \retval  -AW_EBUSY               设备繁忙，稍后重试
 * \retval  -AW_EINVAL              p_init->type 类型不支持修改
 * \retval  -AW_ENOTSUP             该设备不支持 reinit 操作
 * \retval  -AW_ENOSR               该设备未添加到 ctl 中
 */
aw_err_t aw_net_sts_dev_reinit (aw_net_sts_ctl_t      *p_ctl,
                                aw_net_sts_dev_t      *p_dev,
                                aw_net_sts_dev_init_t *p_init);

/**
 * \brief 添加并启动网卡状态检查设备
 *
 * \param   p_ctl   网卡状态管理设备
 * \param   p_dev   网卡状态检查设备
 *
 * \retval  AW_OK                   操作成功
 * \retval  -AW_EEXIST              设备已被添加该 ctl
 * \retval  -AW_EFAULT              设备已被添加到另外的 ctl
 * \retval  -AW_EBUSY               设备繁忙，稍后重试
 */
aw_err_t aw_net_sts_add (aw_net_sts_ctl_t *p_ctl, aw_net_sts_dev_t *p_dev);

/**
 * \brief 从网卡管理中删除该设备
 *
 * \param   p_ctl   网卡状态管理设备
 * \param   p_dev   网卡状态检查设备
 *
 * \retval  AW_OK                   操作成功
 * \retval  -AW_EEXIST              设备未添加到任何 ctl
 * \retval  -AW_EFAULT              设备已被添加到另外的 ctl
 * \retval  -AW_EBUSY               设备繁忙，稍后重试
 */
aw_err_t aw_net_sts_del (aw_net_sts_ctl_t *p_ctl, aw_net_sts_dev_t *p_dev);

aw_bool_t aw_net_sts_dev_is_free (aw_net_sts_dev_t *p_dev);
aw_bool_t aw_net_sts_dev_is_busy (aw_net_sts_dev_t *p_dev);
/**
 * \brief 获取最新的状态事件
 *
 * \detail 获取到的 p_out 中事件类型值为  AW_NET_STS_EVT_TYPE_LINK_UP 则表示有可访问外网网卡，
 *         网卡为 p_out->p_netif；
 *         如果事件类型值为 AW_NET_STS_EVT_TYPE_LINK_DOWN，则表示当前无可访问外网网卡；
 *
 * \param[in]   p_ctl 管理器
 * \param[out]  p_out 保存状态事件的地址
 *
 * \retval  AW_OK   操作成功
 */
aw_err_t aw_net_sts_get (aw_net_sts_ctl_t *p_ctl, aw_net_sts_evt_t *p_out);

/**
 * \brief 事件处理
 *
 * \param[in]   p_ctl 管理器
 * \param[in]   p_evt 事件
 *
 * \retval  AW_OK               操作成功
 * \retval  else                查看 aw_net_sts_err_t 错误类型
 */
aw_err_t aw_net_sts_evt_proc (aw_net_sts_ctl_t *p_ctl, aw_net_sts_evt_t *p_evt);


#ifdef __cplusplus
}
#endif /* __cplusplus   */

#endif /* __NET_STS_H__ */

/* end of file */

