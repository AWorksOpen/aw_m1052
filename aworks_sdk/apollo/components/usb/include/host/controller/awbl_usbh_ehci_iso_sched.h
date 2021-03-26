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

#ifndef __AWBL_USBH_EHCI_ISO_SCHED_H
#define __AWBL_USBH_EHCI_ISO_SCHED_H
//#include "apollo.h"
//#include "host/controller/awbl_usbh_ehci.h"

struct awbl_usbh_ehci_qh;
struct awbl_usbh_ehci_itd;
struct awbl_usbh_ehci;

#define NO_FRAME    29999           /* 尚未分配帧*/

/* 带宽使用*/
//#define EHCI_BANDWIDTH_SIZE 64

#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))

/* UVC打印函数*/
#define __USB_ISO_TRACE(info) \
            do { \
                AW_INFOF(("USBH-ISO: ")); \
                AW_INFOF(info); \
            } while (0)

/* 通用带宽分配常量/支持*/
#define BitTime(bytecount) (7 * 8 * bytecount / 6)
#define NS_TO_US(ns)    AW_DIV_ROUND_UP(ns, 1000L)
#define USB2_HOST_DELAY 5
#define HS_NSECS_ISO(bytes) (((38 * 8 * 2083) \
    + (2083UL * (3 + BitTime(bytes))))/1000 \
    + USB2_HOST_DELAY)
#define HS_USECS_ISO(bytes) NS_TO_US(HS_NSECS_ISO(bytes))


/* 高速端点描述符中编码的高带宽乘法器*/
#define hb_mult(wMaxPacketSize) (1 + (((wMaxPacketSize) >> 11) & 0x03))
/* ...和包大小，用于任何类型的端点描述符*/
#define max_packet(wMaxPacketSize) ((wMaxPacketSize) & 0x07ff)

/* 为分割事务的高速设备和全/低速设备的周期传输调度和预算信息*/
struct ehci_per_sched {
    struct awbl_usbh_device    *udev;           /* 访问事务转换器*/
    struct awbl_usbh_endpoint  *ep;

    uint16_t                   phase;           /* 真实时期，帧部分*/
    uint8_t                    bw_phase;        /* 一样，为带宽保留*/
    uint8_t                    phase_uf;        /* uframe part of the phase */
    uint8_t                    usecs, c_usecs;  /* 高速总线上的时间*/
    uint8_t                    bw_uperiod;      /* 带宽预留的微帧周期*/
    uint16_t                   period;          /* 帧的真实周期*/
    uint8_t                    bw_period;       /* 带宽帧周期*/
};

/* 一个等时传输事务的描述符(如果是高速最多3KB)*/
struct ehci_iso_packet {
    /* These will be copied to iTD when scheduling */
    uint64_t         bufp;               /* itd->hw_bufp{,_hi}[pg] |= */
    uint32_t          transaction;       /* itd->hw_transaction[i] |= */
    uint8_t          cross;              /* 缓存跨页*/
    /* 用于全速输出分割*/
    uint32_t         buf1;
};

/* 来自等时USB请求块的数据包临时调度数据(两种速度)。每个数据包是设备的
 * 一个逻辑USB事务(不是分割事务)，从stream->next_uframe 开始*/
struct ehci_iso_sched {
    struct aw_list_head td_list;         /* 传输描述符链表*/
    unsigned        span;                /* 这次调度在帧周期中的帧/微帧跨度*/
    unsigned        first_packet;
    struct ehci_iso_packet  *packet;
};

///* EHCI等时流结构体*/
//struct ehci_iso_stream {
//    struct awbl_usbh_ehci_qh *hw;
//    struct aw_list_head td_list;    /* 入队列的等时传输描述符/分割事务等时传输描述符*/
//    struct aw_list_head free_list;  /* 空闲的等时传输描述符/分割事务等时传输描述符*/
//    struct ehci_per_sched   ps;     /* 调度信息*/
//    uint32_t next_uframe;
//
//    uint8_t bEndpointAddress;       /* 端点地址*/
//    uint8_t highspeed;              /* 高速传输*/
//    uint16_t uperiod;               /* 微帧周期*/
//    uint32_t bandwidth;             /* 带宽*/
//    uint16_t maxp;                  /* 最大包大小*/
//    uint32_t splits;                /* 分割事务标志*/
//    uint32_t refcount;
//    struct awbl_usbh_endpoint *ep;
//
//    /* 这是用于初始化等时传输描述符的 hw_bufp 字段*/
//    uint32_t buf0;
//    uint32_t buf1;
//    uint32_t buf2;
//};

/* EHCI等时流结构体*/
struct ehci_iso_stream {
    struct awbl_usbh_ehci_qh *hw;
    struct awbl_usbh_device  *udev; /* 访问事务转换器*/
    struct aw_list_head td_list;    /* 入队列的等时传输描述符/分割事务等时传输描述符*/
    struct aw_list_head free_list;  /* 空闲的等时传输描述符/分割事务等时传输描述符*/
    uint32_t next_uframe;

    uint8_t bEndpointAddress;       /* 端点地址*/
    uint8_t highspeed;              /* 高速传输*/
    uint16_t uperiod;               /* 微帧周期*/
    uint32_t bandwidth;             /* 带宽*/
    uint16_t maxp;                  /* 最大包大小*/
    uint32_t splits;                /* 分割事务标志*/
    uint32_t refcount;              /* 2.6*/
    struct awbl_usbh_endpoint *ep;  /* 2.6*/
    uint8_t usecs, c_usecs;         /* 2.6*/
    uint8_t interval;               /* 2.6*/
    uint16_t depth;                 /* 2.6*/
    struct ehci_per_sched   ps;     /* 调度信息 4.x.y*/

    /* 这是用于初始化等时传输描述符的 hw_bufp 字段*/
    uint32_t buf0;
    uint32_t buf1;
    uint32_t buf2;
};

aw_err_t iso_stream_release(struct awbl_usbh_ehci *p_ehci, struct ehci_iso_stream *stream);
void scan_periodic(struct awbl_usbh_ehci *p_ehci);
struct ehci_iso_stream *__iso_stream_get (struct awbl_usbh_ehci *p_ehci, struct awbl_usbh_endpoint *p_ep);
aw_err_t __iso_req(struct awbl_usbh_ehci *p_ehci, struct awbl_usbh_trp *p_trp, struct ehci_iso_stream  *stream);
aw_err_t __iso_sched_make(struct awbl_usbh_ehci *p_ehci, struct awbl_usbh_trp *p_trp, struct ehci_iso_stream  *stream);
#endif
