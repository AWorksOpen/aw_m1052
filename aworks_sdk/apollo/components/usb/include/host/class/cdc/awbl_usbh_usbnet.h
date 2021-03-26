/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

#ifndef __AWBL_USBH_USBNET_H
#define __AWBL_USBH_USBNET_H
#include "apollo.h"
#include "aw_timer.h"
#include "host/awbl_usbh.h"
#include "aw_netif.h"
#include "aw_pool.h"
#include "string.h"
#include "host/class/cdc/awbl_usbh_cdc.h"

/* USB网络打印函数*/
#define __USB_USBNET_TRACE(info, ...) \
            do { \
                AW_INFOF(("USBH-NET:"info, ##__VA_ARGS__)); \
            } while (0)

#ifdef USB_NET_DEBUG
    #define __USB_NET_TRACE_DEBUG  __USB_NET_TRACE
    #define usb_net_printf  aw_kprintf
#else
    #define __USB_NET_TRACE_DEBUG(info, ...) \
            do { \
            } while (0)
#endif

#define AW_USBNET_OPEN 0

enum net_device_flags {
    AW_IFF_UP              = 1<<0,
    AW_IFF_BROADCAST       = 1<<1,
    AW_IFF_DEBUG           = 1<<2,
    AW_IFF_LOOPBACK        = 1<<3,
    AW_IFF_POINTOPOINT     = 1<<4,
    AW_IFF_NOTRAILERS      = 1<<5,
    AW_IFF_RUNNING         = 1<<6,
    AW_IFF_NOARP           = 1<<7,
    AW_IFF_PROMISC         = 1<<8,
    AW_IFF_ALLMULTI        = 1<<9,
    AW_IFF_MASTER          = 1<<10,
    AW_IFF_SLAVE           = 1<<11,
    AW_IFF_MULTICAST       = 1<<12,
    AW_IFF_PORTSEL         = 1<<13,
    AW_IFF_AUTOMEDIA       = 1<<14,
    AW_IFF_DYNAMIC         = 1<<15,
    AW_IFF_LOWER_UP        = 1<<16,
    AW_IFF_DORMANT         = 1<<17,
    AW_IFF_ECHO            = 1<<18,
};

/* 硬件地址分配类型*/
#define AW_NET_ADDR_PERM       0   /* 地址是固定的(默认的)*/
#define AW_NET_ADDR_RANDOM     1   /* 地址随机生成*/
#define AW_NET_ADDR_STOLEN     2   /* 地址是用别的设备的地址*/
#define AW_NET_ADDR_SET        3   /* 地址的设置使用的*/

/* 套接字缓存状态*/
enum aw_skb_state {
    illegal = 0,
    tx_start, tx_done,
    rx_start, rx_done, rx_cleanup,
    unlink_start
};

/* 套接字缓存链表头状态*/
enum aw_skb_head_state {
    __QUEUE_STATE_DRV_XON  = 0,
    __QUEUE_STATE_DRV_XOFF = 1,
};

struct awbl_usbh_usbnet;
/* 套接字缓存私有数据结构体*/
struct aw_skb_data {
    struct awbl_usbh_trp *trp;    /* USB传输请求包*/
    struct awbl_usbh_usbnet *dev; /* USB网络设备*/
    enum aw_skb_state state;      /* 套接字缓存状态*/
    long length;                  /* 数据长度*/
    unsigned long packets;        /* 包数量*/
};


/* 套接字缓存结构体(暂定)*/
struct aw_sk_buff {
    struct aw_sk_buff *next;
    struct aw_sk_buff *prev;

    struct aw_skb_data p_data;       /* 数据缓存*/
    unsigned int act_len, data_len;  /* 实际数据传输长度，和数据总长度*/
    unsigned char *data;             /* 数据*/
};

/* 套接字缓存头结构体*/
struct aw_sk_buff_head {
    /* 这两个成员必须放在第一个*/
    struct aw_sk_buff  *next;       /* 下一个套接字缓存结构体指针*/
    struct aw_sk_buff  *prev;       /* 上一个套接字缓存结构体指针*/

    uint32_t       qlen;            /* 队列长度*/
    uint8_t        state;           /* 队列状态*/
    aw_spinlock_isr_t lock;         /* 自旋锁*/
};

static inline void
awbl_usbnet_set_skb_tx_stats(struct aw_sk_buff *skb,
            unsigned long packets, long bytes_delta)
{
    struct aw_skb_data *entry = &skb->p_data;

    entry->packets = packets;
    entry->length = bytes_delta;
}

/* USB ECM端点结构体*/
struct awbl_usbh_usbnet_endpoint{
    struct aw_usb_interface_desc    *p_desc;
    struct awbl_usbh_endpoint       *eps;
    int extralen;                            /* 额外的描述符的长度*/
    unsigned char *extra;                    /* 额外的描述符(例如，特定类描述符或特定产商描述符) */
    struct aw_list_head node;                /* 当前USB ECM接口结构体节点*/
};

/* 重用USB CDC 基础设施的一些标准的驱动程序(特别是根据CDC联合描述符
 * 使用多个接口)会得到一些帮助程序代码**/
struct awbl_usbh_cdc_state {
    struct usb_cdc_header_desc  *header;        /* CDC头描述符*/
    struct usb_cdc_union_desc   *u;             /* CDC联合描述符*/
    struct usb_cdc_ether_desc   *ether;         /* CDC以太网描述符*/
    struct awbl_usbh_interface  *control;       /* 控制接口*/
    struct awbl_usbh_interface  *data;          /* 数据接口*/
};

struct awbl_usbh_usbnet;
/* 驱动信息，从设备/帧级“迷你驱动”到核心的接口 */
struct usbnet_driver_info {
    char *description;        /* 描述信息*/
    int flags;

#define FLAG_FRAMING_NC 0x0001      /* guard against device dropouts */
#define FLAG_FRAMING_GL 0x0002      /* genelink batches packets */
#define FLAG_FRAMING_Z  0x0004      /* zaurus adds a trailer */
#define FLAG_FRAMING_RN 0x0008      /* RNDIS batches, plus huge header */

#define FLAG_NO_SETINT  0x0010             /* 设备不能设置接口*/
#define FLAG_ETHER  0x0020                 /* 以太网设备 */

#define FLAG_FRAMING_AX 0x0040             /* AX88772/178 包 */
#define FLAG_WLAN   0x0080                 /* 无线局域网设备*/
#define FLAG_AVOID_UNLINK_URBS 0x0100      /* don't unlink urbs at usbnet_stop() */
#define FLAG_SEND_ZLP   0x0200             /* 硬件需要发送0长度包*/
#define FLAG_WWAN   0x0400                 /* 无线广域网设备*/

#define FLAG_LINK_INTR  0x0800             /* updates link (carrier) status */

#define FLAG_POINTTOPOINT 0x1000           /* 点对点设备*/

#define FLAG_MULTI_PACKET   0x2000         /* 指明USB驱动程序积累多个IP包*/
#define FLAG_RX_ASSEMBLE    0x4000  /* rx packets may span >1 frames */
#define FLAG_NOARP      0x8000  /* device can't do ARP */

    /* 初始化设备*/
    aw_err_t (*bind)(struct awbl_usbh_usbnet *, struct awbl_usbh_interface *);
    /* 清除设备 */
    void (*unbind)(struct awbl_usbh_usbnet *, struct awbl_usbh_interface *);
    /* 状态轮询 */
    void (*status)(struct awbl_usbh_usbnet *, struct awbl_usbh_trp *);
    /* 接收模式改变(设备改变地址列表滤波器) */
    void (*set_rx_mode)(struct awbl_usbh_usbnet *dev);
    /* 复位设备 ...*/
    int (*reset)(struct awbl_usbh_usbnet *);
    /* 查看对等方是否已连接*/
    int (*check_connect)(struct awbl_usbh_usbnet *);
    /* 激活/禁用运行时电源管理*/
    int (*manage_power)(struct awbl_usbh_usbnet *, int);
    /* 链接复位处理*/
    int (*link_reset)(struct awbl_usbh_usbnet *);
    /* 修复接收包*/
    int (*rx_fixup)(struct awbl_usbh_usbnet *dev, struct aw_sk_buff *skb);
    /* 关闭设备*/
    aw_err_t (*stop)(struct awbl_usbh_usbnet *dev);

    /* 对于新设备，使用描述符读取代码*/
    struct awbl_usbh_endpoint *in;              /* 输入端点*/
    struct awbl_usbh_endpoint *out;             /* 输出端点*/
};

/* 网络设备状态*/
struct awbl_usbh_usbnet_device_stats {
    unsigned long   tx_packets;        /* 发送包数量*/
    unsigned long   tx_bytes;          /* 发送字节数*/
    unsigned long   tx_errors;         /* 发送错误次数*/
    unsigned long   rx_packets;        /* 接收的包数量*/
    unsigned long   rx_bytes;          /* 接收的字节数*/
    unsigned long   rx_errors;         /* 接收错误次数*/
    unsigned long   rx_length_errors;  /* 接收长度错误*/
};

/* 管理网络USB传输请求包*/
struct awbl_usbh_usbnet_trp_head{
    struct aw_list_head trp_done;
    struct aw_list_head trp_start;
    uint8_t trp_len;
    aw_spinlock_isr_t done_lock;         /* 自旋锁*/
    aw_spinlock_isr_t start_lock;         /* 自旋锁*/
};

/* USB网络内存池管理结构体*/
struct awbl_usbh_usbnet_pool_mem{
    void *p_pool_mem;
    aw_bool_t is_used;
};


/* USB网络设备结构体*/
struct awbl_usbh_usbnet {
    struct awbl_usbh_function *p_fun;           /* USB功能结构体*/
    struct usbnet_driver_info  *driver_info;    /* ECM驱动信息*/
    struct awbl_usbh_interface *act_itf;        /* 当前使用的USB设备当前接口*/
    struct awbl_usbh_config *act_cfg;           /* 当前使用的USB设备的当前配置*/

    AW_MUTEX_DECL(interrupt_mutex);             /* 中断互斥锁*/
    AW_MUTEX_DECL(phy_mutex);
    uint32_t interrupt_count;                   /* 中断请求计数*/
    aw_timer_t delay;                           /* 延时定时器*/
    unsigned long data[5];
    uint32_t hard_mtu;                          /* 硬件最大传输单元，计数任何额外的帧*/
    struct awbl_usbh_endpoint *status;          /* 状态端点*/
    struct awbl_usbh_endpoint *in;              /* 输入端点*/
    struct awbl_usbh_endpoint *out;             /* 输出端点*/
    uint32_t rx_trp_size;                       /* 接收传输请求包大小*/
    struct awbl_usbh_trp *int_trp;              /* 中断传输请求包*/
    uint32_t maxpacket;                         /* 最大包大小*/

    struct awbl_usbh_usbnet_trp_head rxq;       /* 接收队列*/
    uint16_t  rx_qlen, tx_qlen;                 /* 发送/接收队列长度*/

    aw_netif_t ethif;                           /* 以太网设备*/
    uint8_t pkt_cnt, pkt_err;                   /* 包次数，包错误*/
    aw_usb_task_handle_t bh;                    /* 处理接收包任务*/
    aw_usb_task_handle_t kevent;                /* 事件任务*/
    aw_usb_sem_handle_t kevent_sem;             /* 事件信号量*/
    aw_usb_sem_handle_t bh_sem;                 /* 信号量*/
    uint32_t dev_flags;                         /* 设备标志*/
    uint32_t net_flags;                         /* 网络相关的标志位*/
    uint16_t hard_header_len;
#define AW_EVENT_TX_HALT            0
#define AW_EVENT_RX_HALT            1
#define AW_EVENT_RX_MEMORY          2
#define AW_EVENT_STS_SPLIT          3
#define AW_EVENT_LINK_RESET         4
#define AW_EVENT_RX_PAUSED          5
#define AW_EVENT_DEV_ASLEEP         6
#define AW_EVENT_DEV_OPEN           7
#define AW_EVENT_DEVICE_REPORT_IDLE 8
#define AW_EVENT_NO_RUNTIME_PM      9
#define AW_EVENT_RX_KILL            10
#define AW_EVENT_LINK_CHANGE        11
#define AW_EVENT_SET_RX_MODE        12
    struct aw_pool              trp_pool;       /* USB传输请求包内存池*/


    char ethif_name[32];                        /* 以太网设备名字*/
    uint32_t trans_start;                       /* 记录传输启动时间*/
    aw_bool_t is_removed;                       /* 设备是否移除*/

    /* 以下成员只是暂时定义*/
    ////////////////////////////////////////////////////////////////////////////////
    struct awbl_usbh_usbnet_device_stats stats; /* 设备状态*/
    ///////////////////////////////////////////////////////////////////////////////
};

/* USB网络获取端点函数*/
int awbl_usbnet_get_endpoints(struct awbl_usbh_usbnet *dev,
                              struct awbl_usbh_config *cfg,
                              struct awbl_usbh_interface *intf);
/* 获取以太网地址*/
int awbl_usbnet_get_ethernet_addr(struct awbl_usbh_usbnet *dev, int iMACAddress);
/* 初始化USB网络设备中断状态端点*/
aw_err_t awbl_usbnet_init_status (struct awbl_usbh_usbnet *dev);
/* 比较两个以太网地址是否相等*/
aw_bool_t awbl_usbnet_ether_addr_equal(const uint8_t *addr1, const uint8_t *addr2);
/* 更新网络设备最大队列长度*/
void awbl_usbnet_update_max_qlen(struct awbl_usbh_usbnet *dev);
/* 初始化随机以太网地址*/
void awbl_usbnet_init_random_addr(uint8_t *addr);
/* 通过PID和VID寻找对应的USB网络设备*/
struct awbl_usbh_usbnet *awbl_usbnet_dev_find(int pid, int vid);

aw_err_t awbl_usbnet_link_init (aw_netif_t *p_ethif);
aw_err_t  awbl_usbnet_up (struct aw_netif *p_ethif);
aw_err_t  awbl_usbnet_down (struct aw_netif *p_ethif);
aw_err_t awbl_usbnet_output (aw_netif_t *p_ethif, aw_net_buf_t *p_buf);
aw_err_t awbl_usbnet_ioctl (aw_netif_t *p_netif, int opt, int name, void *p_arg);
void awbl_usbnet_release (aw_netif_t *p_ethif);
aw_err_t awbl_usbnet_close(struct awbl_usbh_usbnet *dev);
#endif
