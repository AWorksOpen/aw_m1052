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
 * \brief 网络接口管理
 *
 * 使用本服务需要包含头文件
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
 * \name 网卡状态变动事件
 * @{
 */

/** \brief 网卡状态变动事件功能 */
#define AW_NETIF_LINK_EVENT_HANDLER     1

/** \brief 网卡状态变动事件最大订阅数量 */
#define AW_NETIF_LINK_EVENT_HDL_MAX     2

/** @} */

/**
 * \name 链路属性
 * @{
 */

/** \brief 硬件地址最大长度 */
#define AW_NETIF_MAX_HWADDR_LEN         6

/** @} */

/**
 * \name 网卡属性
 * @{
 */

/** @} */

/**
 * \name 网卡状态
 * @{
 */

/** \brief 网卡可用或不可用 */
#define AW_NETIF_STAT_EXIST             AW_BIT(0)

/** \brief 使能通信 */
#define AW_NETIF_STAT_DEV_UP            AW_BIT(1)

/** \brief 链路可用 */
#define AW_NETIF_STAT_LINK_UP           AW_BIT(2)

/** \brief 由DHCP管理IP地址 */
#define AW_NETIF_STAT_DHCP_ON           AW_BIT(3)

/** @} */

/**
 * \name 杂项操作
 * @{
 */

typedef enum {
    AW_NETIF_OPT_SET,
    AW_NETIF_OPT_GET,
} aw_netif_ioctl_ops_t;

/** \brief 控制抓包功能 */
#define AW_NETIF_OPT_PKTDUMP            AW_BIT(0)

#ifdef LWIP_2_X
/** \brief 组播地址 */
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

/** \brief 网卡类型 */
typedef enum {
    AW_NETIF_TYPE_RAW,      /* 暂不支持 */
    AW_NETIF_TYPE_ETHERNET,
    AW_NETIF_TYPE_WIFI,
    AW_NETIF_TYPE_PPPOS,
    AW_NETIF_TYPE_PPPOE,
} aw_netif_type_t;

/** \brief 网卡状态变动事件 */
typedef enum {
    AW_NETIF_EVENT_DEV_REMOVED,
    AW_NETIF_EVENT_DEV_UP,
    AW_NETIF_EVENT_DEV_DOWN,
    AW_NETIF_EVENT_LINK_UP,
    AW_NETIF_EVENT_LINK_DOWN,
} aw_netif_event_t;

/** \brief 以太网链路硬件地址 */
typedef struct {
    uint8_t octet[6];
} aw_eth_mac_t;

typedef struct aw_netif aw_netif_t;
typedef void (*aw_netif_iterate_cb_t) (aw_netif_t *p_netif, void *p_arg);
typedef void (*aw_netif_ev_hdl_t) (aw_netif_t *p_netif, aw_netif_event_t event, void *p_arg);

/** \brief 网卡链路变动事件订阅记录 */
typedef struct {
    aw_netif_ev_hdl_t pfn;
    void *p_arg;
} aw_netif_ev_list_elem_t;

/** \brief 网卡驱动提供的基本操作 */
typedef struct aw_netif_ops {
    aw_err_t (*init)    (aw_netif_t *p_netif);
    aw_err_t (*up)      (aw_netif_t *p_netif);
    aw_err_t (*down)    (aw_netif_t *p_netif);
    aw_err_t (*output)  (aw_netif_t *p_netif, aw_net_buf_t *p);
    aw_err_t (*io_ctl)  (aw_netif_t *p_netif, int ops, int name, void *p_arg);
    void     (*release) (aw_netif_t *p_netif);
} aw_netif_ops_t;

/** \brief 默认参数配置 */
typedef struct aw_netif_info_get_entry {
    char     *(*get_ipaddr)      (void); /**< \brief 指定网卡的IP地址 */
    char     *(*get_netmsk)      (void); /**< \brief 指定网卡的子网掩码 */
    char     *(*get_gateway)     (void); /**< \brief 指定网卡的网关地址 */
    
    /** \brief 指定网卡的MAC地址 */
    aw_err_t  (*get_hwaddr)      (uint8_t *p_hwaddr, int addr_len); 
    
    /** \brief 默认启动DHCP获取地址（需要开启DHCP客户端功能） */
    aw_bool_t    (*get_dhcp_en)  (void); 
} aw_netif_info_get_entry_t;

/** \brief 网卡，驱动提供实例 */
struct aw_netif {
    aw_netif_t              *p_next;
    const char              *p_name;
    const aw_netif_ops_t    *p_ops;
    uint8_t                  hwaddr[AW_NETIF_MAX_HWADDR_LEN];
    uint16_t                 mtu;
    uint8_t                  hwaddr_len;
    aw_netif_type_t          type;
    struct aw_refcnt         refcnt;
    uint16_t                 state;     /**< \brief 状态 */
    uint16_t                 flags;     /**< \brief 属性 */
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
    aw_pfuncvoid_t pfn_pad0; /**< \brief 避免 size 改变 */

    void          *p_phy_layer_data;
#endif
};

/**
 * \brief 初始化网卡接口管理器
 */
aw_err_t aw_netif_init (void);

/*******************************************************************************
 * 网卡接口管理（驱动使用）
 ******************************************************************************/

/**
 * \brief 注册一个网卡
 *
 * 第一个调用的作为系统默认
 *
 * \param[in] p_netif  实例的指针
 * \param[in] p_name   名字
 * \param[in] p_ops    驱动提供的基本操作
 * \param[in] p_entry  提供默认参数
 * \param[in] type     链路类型
 * \param[in] p_hwaddr 硬件地址
 * \param[in] mtu      最大传输单元
 */
aw_err_t aw_netif_add (aw_netif_t                      *p_netif,
                       const char                      *p_name,
                       const aw_netif_ops_t            *p_ops,
                       const aw_netif_info_get_entry_t *p_entry,
                       aw_netif_type_t                  type,
                       uint8_t                         *p_hwaddr,
                       uint16_t                         mtu);

/**
 * \brief 删除一个网卡
 *
 * \param[in] p_netif  实例的指针
 */
aw_err_t aw_netif_remove (aw_netif_t *p_netif);

/**
 * \brief 将收到的数据包输入TCP/IP协议栈（内部拷贝）
 *
 *  驱动自己创建任务接收数据，在该任务中调用
 *
 * \param[in] p_netif 实例句柄
 * \param[in] p_mem   数据包内容
 * \param[in] len     数据包长度
 *
 * \retval AW_OK       释放成功
 * \retval -AW_EFAULT  释放失败
 */
aw_err_t aw_netif_input (aw_netif_t *p_netif, void *p_mem, size_t len);

/**
 * \brief 将收到的数据包输入TCP/IP协议栈（驱动自行拷贝）
 *
 *  驱动自己创建任务接收数据，在该任务中调用
 *
 * \param[in] p_netif  实例句柄
 * \param[in] len      数据包总长度
 * \param[in] p_ctx    拷贝函数附加参数
 * \param[in] pfn_copy 拷贝函数
 *
 * \retval AW_OK       释放成功
 * \retval -AW_EFAULT  释放失败
 */
aw_err_t aw_netif_input_chain (aw_netif_t       *p_netif,
                               size_t            len,
                               void             *p_ctx,
                               aw_net_buf_cpy_t  pfn_copy);

/**
 * \brief 设置网卡链路为断可用状态
 *
 * \param[in] p_netif  实例句柄
 *
 * \retval AW_OK       操作成功
 * \retval -AW_ENODEV  设备不存在
 */
aw_err_t aw_netif_set_link_up (aw_netif_t *p_netif);

/**
 * \brief 设置网卡链路为断开状态
 *
 * \param[in] p_netif  实例句柄
 *
 * \retval AW_OK       操作成功
 * \retval -AW_ENODEV  设备不存在
 */
aw_err_t aw_netif_set_link_down (aw_netif_t *p_netif);

/*******************************************************************************
 * 网卡接口管理（应用层使用）
 ******************************************************************************/

/**
 * \brief 打开网卡设备
 *
 * 应用层先调用该函数取得句柄再使用，用完记得关闭
 *
 * \param[in] p_name 网卡名字
 *
 * \return 实例句柄
 */
aw_netif_t *aw_netif_dev_open (const char *p_name);

#ifdef LWIP_2_X
/**
 * \brief 打开默认网卡设备
 *
 * 用完记得关闭
 *
 * \return 实例句柄
 */
aw_netif_t *aw_netif_dev_open_default (void);
#elif defined LWIP_1_4_X
#else
#error "no lwipopts.h"
#endif

/**
 * \brief 关闭网卡设备
 *
 * 驱动移除设备后，最后一次关闭调用将最终销毁网卡实例
 *
 * \param[in] p_netif 实例句柄
 *
 * \retval AW_OK       释放成功
 * \retval -AW_ENODEV  设备不存在
 * \retval -AW_EPERM   释放失败
 */
aw_err_t aw_netif_dev_close (aw_netif_t *p_netif);

/**
 * \brief 杂项控制
 *
 *  具体功能由驱动提供
 *
 * \param[in]     p_netif  实例句柄
 * \param[in]     ops      操作
 * \param[in]     name     功能
 * \param[in,out] p_arg    参数
 *
 * \retval AW_OK       操作成功
 * \retval -AW_ENODEV  设备不存在
 * \retval -AW_ENOTSUP 设备不支持
 */
aw_err_t aw_netif_io_ctl (aw_netif_t *p_netif,
                          int         ops,
                          int         name,
                          void       *p_arg);

/**
 * \brief 订阅网卡事件
 *
 * \param[in] p_netif 实例句柄
 * \param[in] pfn     事件订户
 * \param[in] p_arg   附加参数
 *
 * \retval AW_OK      操作成功
 * \retval -AW_ENODEV 设备不存在
 * \retval -AW_EAGAIN 订户已满
 */
aw_err_t aw_netif_event_cb_register (aw_netif_t        *p_netif,
                                     aw_netif_ev_hdl_t  pfn,
                                     void              *p_arg);

/**
 * \brief 取消订阅网卡事件
 *
 * \param[in] p_netif 实例句柄
 * \param[in] pfn     事件订户
 * \param[in] p_arg   附加参数
 *
 * \retval AW_OK      操作成功
 * \retval -AW_ENODEV 设备不存在
 * \retval -AW_EAGAIN 订户已满
 */
aw_err_t aw_netif_event_cb_unregister (aw_netif_t *p_netif, aw_netif_ev_hdl_t pfn);

/*******************************************************************************
 * 网卡接口管理（驱动/应用层皆可使用）
 ******************************************************************************/

/**
 * \brief 更改默认网卡
 *
 *  第一个调用 aw_netif_add 的是系统默认
 *
 * \param[in] p_netif  实例句柄
 *
 * \retval AW_OK       操作成功
 * \retval -AW_ENODEV  设备不存在
 * \retval AW_ERROR    操作失败
 */
aw_err_t aw_netif_set_default (aw_netif_t *p_netif);

/**
 * \brief 设置网卡为可用状态
 *
 * \param[in] p_netif  实例句柄
 *
 * \retval AW_OK       操作成功
 * \retval -AW_ENODEV  设备不存在
 */
aw_err_t aw_netif_up (aw_netif_t *p_netif);

/**
 * \brief 设置网卡为不可用状态
 *
 * \param[in] p_netif  实例句柄
 *
 * \retval AW_OK       操作成功
 * \retval -AW_ENODEV  设备不存在
 */
aw_err_t aw_netif_down (aw_netif_t *p_netif);

/**
 * \brief 获取网卡可用状态
 *
 * \param[in]  p_netif 实例句柄
 * \param[out] p_up    通信状态
 *
 * \retval AW_OK       操作成功
 * \retval -AW_ENODEV  设备不存在
 */
aw_err_t aw_netif_is_up (aw_netif_t *p_netif, aw_bool_t *p_up);

/**
 * \brief 获取网卡链路状态
 *
 * \param[in]  p_netif 实例句柄
 * \param[out] p_up    链路状态
 *
 * \retval AW_OK       操作成功
 * \retval -AW_ENODEV  设备不存在
 */
aw_err_t aw_netif_is_link_up (aw_netif_t *p_netif, aw_bool_t *p_up);

/**
 * \brief 获取运行状态
 *
 * \param[in]  p_netif  实例句柄
 * \param[out] p_state  状态原始值
 *
 * \retval AW_OK       操作成功
 * \retval -AW_ENODEV  设备不存在
 */
aw_err_t aw_netif_state_get (aw_netif_t *p_netif, uint32_t *p_state);

/**
 * \brief 设备是否存在
 *
 * \param[in] p_netif 实例句柄
 *
 * \return 设备存在与否
 */
aw_static_inline aw_bool_t aw_netif_is_exist (aw_netif_t *p_netif)
{
    return (p_netif && (p_netif->state & AW_NETIF_STAT_EXIST));
}

/**
 * \brief 设备是否就绪
 *
 * \param[in] p_netif 实例句柄
 *
 * \return 设备就绪与否
 */
aw_static_inline aw_bool_t aw_netif_is_ready (aw_netif_t *p_netif)
{
    return (p_netif && 
            (p_netif->state & (AW_NETIF_STAT_EXIST | AW_NETIF_STAT_DEV_UP)));
}

/**
 * \brief 获取设备类型
 *
 * \param[in]  p_netif 实例句柄
 * \param[out] p_type  设备类型
 *
 * \retval AW_OK       操作成功
 * \retval -AW_ENODEV  设备不存在
 */
aw_err_t aw_netif_type_get (aw_netif_t *p_netif, uint8_t *p_type);

/**
 * \brief 获取设备MTU
 *
 * \param[in]  p_netif 实例句柄
 * \param[out] p_mtu   MTU
 *
 * \retval AW_OK       操作成功
 * \retval -AW_ENODEV  设备不存在
 */
aw_err_t aw_netif_mtu_get (aw_netif_t *p_netif, uint16_t *p_mtu);

/**
 * \brief 获取硬件地址
 *
 * \param[in]  p_netif 实例句柄
 * \param[out] addr    保存硬件地址的缓冲区
 * \param[in]  len     缓冲区大小
 *
 * \retval AW_OK       操作成功
 * \retval -AW_ENODEV  设备不存在
 */
aw_err_t aw_netif_hwaddr_get (aw_netif_t *p_netif, uint8_t *addr, uint8_t len);

/**
 * \brief 获取设备名称
 *
 * \param[in]  p_netif 实例句柄
 * \param[out] p_name  接收设备名称的指针
 *
 * \retval AW_OK       操作成功
 * \retval -AW_ENODEV  设备不存在
 */
aw_err_t aw_netif_name_get (aw_netif_t *p_netif, char **p_name);

/**
 * \brief 遍历所有设备
 *
 * \param[in] cb    访问网卡的函数
 * \param[in] p_arg 附加参数
 */
void aw_netif_iterate (aw_netif_iterate_cb_t cb, void *p_arg);

/*******************************************************************************
 * IP地址管理
 ******************************************************************************/

/**
 * \brief 设置IP地址
 *
 * \param[in]  p_netif 实例句柄
 * \param[in]  addr    IP地址
 *
 * \retval AW_OK       操作成功
 * \retval -AW_ENODEV  设备不存在
 */
aw_err_t aw_netif_ipv4_ip_set (aw_netif_t *p_netif, struct in_addr *addr);

/**
 * \brief 获取IP地址
 *
 * \param[in]  p_netif 实例句柄
 * \param[out] addr    IP地址
 *
 * \retval AW_OK       操作成功
 * \retval -AW_ENODEV  设备不存在
 */
aw_err_t aw_netif_ipv4_ip_get (aw_netif_t *p_netif, struct in_addr *addr);

/**
 * \brief 设置子网掩码
 *
 * \param[in]  p_netif 实例句柄
 * \param[in]  addr    子网掩码
 *
 * \retval AW_OK       操作成功
 * \retval -AW_ENODEV  设备不存在
 */
aw_err_t aw_netif_ipv4_netmask_set (aw_netif_t *p_netif, struct in_addr *addr);

/**
 * \brief 获取子网掩码
 *
 * \param[in]  p_netif 实例句柄
 * \param[out] addr    子网掩码
 *
 * \retval AW_OK       操作成功
 * \retval -AW_ENODEV  设备不存在
 */
aw_err_t aw_netif_ipv4_netmask_get (aw_netif_t *p_netif, struct in_addr *addr);

/**
 * \brief 设置网关地址
 *
 * \param[in]  p_netif 实例句柄
 * \param[in]  addr    网关地址
 *
 * \retval AW_OK       操作成功
 * \retval -AW_ENODEV  设备不存在
 */
aw_err_t aw_netif_ipv4_gw_set (aw_netif_t *p_netif, struct in_addr *addr);

/**
 * \brief 获取网关地址
 *
 * \param[in]  p_netif 实例句柄
 * \param[out] addr    网关地址
 *
 * \retval AW_OK       操作成功
 * \retval -AW_ENODEV  设备不存在
 */
aw_err_t aw_netif_ipv4_gw_get (aw_netif_t *p_netif, struct in_addr *addr);

/**
 * \brief 设置IP地址、子网掩码和网关地址
 *
 * \param[in] p_name    网卡名称
 * \param[in] p_ipaddr  IP地址
 * \param[in] p_netmask 子网掩码
 * \param[in] p_gateway 网关地址
 *
 * \retval AW_OK        操作成功
 * \retval -AW_ENODEV   设备不存在
 */
aw_err_t aw_netif_ipv4_addr_set_by_name (const char     *p_name,
                                         struct in_addr *p_ipaddr,
                                         struct in_addr *p_netmask,
                                         struct in_addr *p_gateway);

/**
 * \brief 获取IP地址、子网掩码和网关地址
 *
 * \param[in]  p_name    网卡名称
 * \param[out] p_ipaddr  IP地址
 * \param[out] p_netmask 子网掩码
 * \param[out] p_gateway 网关地址
 *
 * \retval AW_OK         操作成功
 * \retval -AW_ENODEV    设备不存在
 */
aw_err_t aw_netif_ipv4_addr_get_by_name (const char     *p_name,
                                         struct in_addr *p_ipaddr,
                                         struct in_addr *p_netmask,
                                         struct in_addr *p_gateway);

/**
 * \brief 获取硬件地址
 *
 * \param[in]  p_name  网卡名称
 * \param[out] addr    保存硬件地址的缓冲区
 * \param[in]  len     缓冲区大小
 *
 * \retval AW_OK       操作成功
 * \retval -AW_ENODEV  设备不存在
 */
aw_err_t aw_netif_hwaddr_get_by_name (const char *p_name, 
                                      uint8_t    *p_addr,
                                      uint8_t     len);

/**
 * dhcp相关
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
