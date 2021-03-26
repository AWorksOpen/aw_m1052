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

/* USB�����ӡ����*/
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

/* Ӳ����ַ��������*/
#define AW_NET_ADDR_PERM       0   /* ��ַ�ǹ̶���(Ĭ�ϵ�)*/
#define AW_NET_ADDR_RANDOM     1   /* ��ַ�������*/
#define AW_NET_ADDR_STOLEN     2   /* ��ַ���ñ���豸�ĵ�ַ*/
#define AW_NET_ADDR_SET        3   /* ��ַ������ʹ�õ�*/

/* �׽��ֻ���״̬*/
enum aw_skb_state {
    illegal = 0,
    tx_start, tx_done,
    rx_start, rx_done, rx_cleanup,
    unlink_start
};

/* �׽��ֻ�������ͷ״̬*/
enum aw_skb_head_state {
    __QUEUE_STATE_DRV_XON  = 0,
    __QUEUE_STATE_DRV_XOFF = 1,
};

struct awbl_usbh_usbnet;
/* �׽��ֻ���˽�����ݽṹ��*/
struct aw_skb_data {
    struct awbl_usbh_trp *trp;    /* USB���������*/
    struct awbl_usbh_usbnet *dev; /* USB�����豸*/
    enum aw_skb_state state;      /* �׽��ֻ���״̬*/
    long length;                  /* ���ݳ���*/
    unsigned long packets;        /* ������*/
};


/* �׽��ֻ���ṹ��(�ݶ�)*/
struct aw_sk_buff {
    struct aw_sk_buff *next;
    struct aw_sk_buff *prev;

    struct aw_skb_data p_data;       /* ���ݻ���*/
    unsigned int act_len, data_len;  /* ʵ�����ݴ��䳤�ȣ��������ܳ���*/
    unsigned char *data;             /* ����*/
};

/* �׽��ֻ���ͷ�ṹ��*/
struct aw_sk_buff_head {
    /* ��������Ա������ڵ�һ��*/
    struct aw_sk_buff  *next;       /* ��һ���׽��ֻ���ṹ��ָ��*/
    struct aw_sk_buff  *prev;       /* ��һ���׽��ֻ���ṹ��ָ��*/

    uint32_t       qlen;            /* ���г���*/
    uint8_t        state;           /* ����״̬*/
    aw_spinlock_isr_t lock;         /* ������*/
};

static inline void
awbl_usbnet_set_skb_tx_stats(struct aw_sk_buff *skb,
            unsigned long packets, long bytes_delta)
{
    struct aw_skb_data *entry = &skb->p_data;

    entry->packets = packets;
    entry->length = bytes_delta;
}

/* USB ECM�˵�ṹ��*/
struct awbl_usbh_usbnet_endpoint{
    struct aw_usb_interface_desc    *p_desc;
    struct awbl_usbh_endpoint       *eps;
    int extralen;                            /* ������������ĳ���*/
    unsigned char *extra;                    /* �����������(���磬�ض������������ض�����������) */
    struct aw_list_head node;                /* ��ǰUSB ECM�ӿڽṹ��ڵ�*/
};

/* ����USB CDC ������ʩ��һЩ��׼����������(�ر��Ǹ���CDC����������
 * ʹ�ö���ӿ�)��õ�һЩ�����������**/
struct awbl_usbh_cdc_state {
    struct usb_cdc_header_desc  *header;        /* CDCͷ������*/
    struct usb_cdc_union_desc   *u;             /* CDC����������*/
    struct usb_cdc_ether_desc   *ether;         /* CDC��̫��������*/
    struct awbl_usbh_interface  *control;       /* ���ƽӿ�*/
    struct awbl_usbh_interface  *data;          /* ���ݽӿ�*/
};

struct awbl_usbh_usbnet;
/* ������Ϣ�����豸/֡�������������������ĵĽӿ� */
struct usbnet_driver_info {
    char *description;        /* ������Ϣ*/
    int flags;

#define FLAG_FRAMING_NC 0x0001      /* guard against device dropouts */
#define FLAG_FRAMING_GL 0x0002      /* genelink batches packets */
#define FLAG_FRAMING_Z  0x0004      /* zaurus adds a trailer */
#define FLAG_FRAMING_RN 0x0008      /* RNDIS batches, plus huge header */

#define FLAG_NO_SETINT  0x0010             /* �豸�������ýӿ�*/
#define FLAG_ETHER  0x0020                 /* ��̫���豸 */

#define FLAG_FRAMING_AX 0x0040             /* AX88772/178 �� */
#define FLAG_WLAN   0x0080                 /* ���߾������豸*/
#define FLAG_AVOID_UNLINK_URBS 0x0100      /* don't unlink urbs at usbnet_stop() */
#define FLAG_SEND_ZLP   0x0200             /* Ӳ����Ҫ����0���Ȱ�*/
#define FLAG_WWAN   0x0400                 /* ���߹������豸*/

#define FLAG_LINK_INTR  0x0800             /* updates link (carrier) status */

#define FLAG_POINTTOPOINT 0x1000           /* ��Ե��豸*/

#define FLAG_MULTI_PACKET   0x2000         /* ָ��USB����������۶��IP��*/
#define FLAG_RX_ASSEMBLE    0x4000  /* rx packets may span >1 frames */
#define FLAG_NOARP      0x8000  /* device can't do ARP */

    /* ��ʼ���豸*/
    aw_err_t (*bind)(struct awbl_usbh_usbnet *, struct awbl_usbh_interface *);
    /* ����豸 */
    void (*unbind)(struct awbl_usbh_usbnet *, struct awbl_usbh_interface *);
    /* ״̬��ѯ */
    void (*status)(struct awbl_usbh_usbnet *, struct awbl_usbh_trp *);
    /* ����ģʽ�ı�(�豸�ı��ַ�б��˲���) */
    void (*set_rx_mode)(struct awbl_usbh_usbnet *dev);
    /* ��λ�豸 ...*/
    int (*reset)(struct awbl_usbh_usbnet *);
    /* �鿴�Եȷ��Ƿ�������*/
    int (*check_connect)(struct awbl_usbh_usbnet *);
    /* ����/��������ʱ��Դ����*/
    int (*manage_power)(struct awbl_usbh_usbnet *, int);
    /* ���Ӹ�λ����*/
    int (*link_reset)(struct awbl_usbh_usbnet *);
    /* �޸����հ�*/
    int (*rx_fixup)(struct awbl_usbh_usbnet *dev, struct aw_sk_buff *skb);
    /* �ر��豸*/
    aw_err_t (*stop)(struct awbl_usbh_usbnet *dev);

    /* �������豸��ʹ����������ȡ����*/
    struct awbl_usbh_endpoint *in;              /* ����˵�*/
    struct awbl_usbh_endpoint *out;             /* ����˵�*/
};

/* �����豸״̬*/
struct awbl_usbh_usbnet_device_stats {
    unsigned long   tx_packets;        /* ���Ͱ�����*/
    unsigned long   tx_bytes;          /* �����ֽ���*/
    unsigned long   tx_errors;         /* ���ʹ������*/
    unsigned long   rx_packets;        /* ���յİ�����*/
    unsigned long   rx_bytes;          /* ���յ��ֽ���*/
    unsigned long   rx_errors;         /* ���մ������*/
    unsigned long   rx_length_errors;  /* ���ճ��ȴ���*/
};

/* ��������USB���������*/
struct awbl_usbh_usbnet_trp_head{
    struct aw_list_head trp_done;
    struct aw_list_head trp_start;
    uint8_t trp_len;
    aw_spinlock_isr_t done_lock;         /* ������*/
    aw_spinlock_isr_t start_lock;         /* ������*/
};

/* USB�����ڴ�ع���ṹ��*/
struct awbl_usbh_usbnet_pool_mem{
    void *p_pool_mem;
    aw_bool_t is_used;
};


/* USB�����豸�ṹ��*/
struct awbl_usbh_usbnet {
    struct awbl_usbh_function *p_fun;           /* USB���ܽṹ��*/
    struct usbnet_driver_info  *driver_info;    /* ECM������Ϣ*/
    struct awbl_usbh_interface *act_itf;        /* ��ǰʹ�õ�USB�豸��ǰ�ӿ�*/
    struct awbl_usbh_config *act_cfg;           /* ��ǰʹ�õ�USB�豸�ĵ�ǰ����*/

    AW_MUTEX_DECL(interrupt_mutex);             /* �жϻ�����*/
    AW_MUTEX_DECL(phy_mutex);
    uint32_t interrupt_count;                   /* �ж��������*/
    aw_timer_t delay;                           /* ��ʱ��ʱ��*/
    unsigned long data[5];
    uint32_t hard_mtu;                          /* Ӳ������䵥Ԫ�������κζ����֡*/
    struct awbl_usbh_endpoint *status;          /* ״̬�˵�*/
    struct awbl_usbh_endpoint *in;              /* ����˵�*/
    struct awbl_usbh_endpoint *out;             /* ����˵�*/
    uint32_t rx_trp_size;                       /* ���մ����������С*/
    struct awbl_usbh_trp *int_trp;              /* �жϴ��������*/
    uint32_t maxpacket;                         /* ������С*/

    struct awbl_usbh_usbnet_trp_head rxq;       /* ���ն���*/
    uint16_t  rx_qlen, tx_qlen;                 /* ����/���ն��г���*/

    aw_netif_t ethif;                           /* ��̫���豸*/
    uint8_t pkt_cnt, pkt_err;                   /* ��������������*/
    aw_usb_task_handle_t bh;                    /* ������հ�����*/
    aw_usb_task_handle_t kevent;                /* �¼�����*/
    aw_usb_sem_handle_t kevent_sem;             /* �¼��ź���*/
    aw_usb_sem_handle_t bh_sem;                 /* �ź���*/
    uint32_t dev_flags;                         /* �豸��־*/
    uint32_t net_flags;                         /* ������صı�־λ*/
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
    struct aw_pool              trp_pool;       /* USB����������ڴ��*/


    char ethif_name[32];                        /* ��̫���豸����*/
    uint32_t trans_start;                       /* ��¼��������ʱ��*/
    aw_bool_t is_removed;                       /* �豸�Ƿ��Ƴ�*/

    /* ���³�Աֻ����ʱ����*/
    ////////////////////////////////////////////////////////////////////////////////
    struct awbl_usbh_usbnet_device_stats stats; /* �豸״̬*/
    ///////////////////////////////////////////////////////////////////////////////
};

/* USB�����ȡ�˵㺯��*/
int awbl_usbnet_get_endpoints(struct awbl_usbh_usbnet *dev,
                              struct awbl_usbh_config *cfg,
                              struct awbl_usbh_interface *intf);
/* ��ȡ��̫����ַ*/
int awbl_usbnet_get_ethernet_addr(struct awbl_usbh_usbnet *dev, int iMACAddress);
/* ��ʼ��USB�����豸�ж�״̬�˵�*/
aw_err_t awbl_usbnet_init_status (struct awbl_usbh_usbnet *dev);
/* �Ƚ�������̫����ַ�Ƿ����*/
aw_bool_t awbl_usbnet_ether_addr_equal(const uint8_t *addr1, const uint8_t *addr2);
/* ���������豸�����г���*/
void awbl_usbnet_update_max_qlen(struct awbl_usbh_usbnet *dev);
/* ��ʼ�������̫����ַ*/
void awbl_usbnet_init_random_addr(uint8_t *addr);
/* ͨ��PID��VIDѰ�Ҷ�Ӧ��USB�����豸*/
struct awbl_usbh_usbnet *awbl_usbnet_dev_find(int pid, int vid);

aw_err_t awbl_usbnet_link_init (aw_netif_t *p_ethif);
aw_err_t  awbl_usbnet_up (struct aw_netif *p_ethif);
aw_err_t  awbl_usbnet_down (struct aw_netif *p_ethif);
aw_err_t awbl_usbnet_output (aw_netif_t *p_ethif, aw_net_buf_t *p_buf);
aw_err_t awbl_usbnet_ioctl (aw_netif_t *p_netif, int opt, int name, void *p_arg);
void awbl_usbnet_release (aw_netif_t *p_ethif);
aw_err_t awbl_usbnet_close(struct awbl_usbh_usbnet *dev);
#endif
