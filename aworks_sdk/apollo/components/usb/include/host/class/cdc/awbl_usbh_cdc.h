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
#ifndef __AWBL_USBH_CDC_H
#define __AWBL_USBH_CDC_H

/* USB CDC子类代码*/
#define AW_USB_CDC_SUBCLASS_ACM            0x02
#define AW_USB_CDC_SUBCLASS_ETHERNET       0x06   /* 以太网*/
#define AW_USB_CDC_SUBCLASS_WHCM           0x08
#define AW_USB_CDC_SUBCLASS_DMM            0x09
#define AW_USB_CDC_SUBCLASS_MDLM           0x0a
#define AW_USB_CDC_SUBCLASS_OBEX           0x0b
#define AW_USB_CDC_SUBCLASS_EEM            0x0c
#define AW_USB_CDC_SUBCLASS_NCM            0x0d
#define AW_USB_CDC_SUBCLASS_MBIM           0x0e

/* 无协议类型*/
#define AW_USB_CDC_PROTO_NONE          0


/* 特定类描述符*/
#define AW_USB_CDC_HEADER_TYPE     0x00             /* 头描述符*/
#define AW_USB_CDC_CALL_MANAGEMENT_TYPE    0x01     /* call_mgmt_descriptor */
#define AW_USB_CDC_ACM_TYPE        0x02             /* ACM描述符*/
#define AW_USB_CDC_UNION_TYPE      0x06             /* 联合描述符*/
#define AW_USB_CDC_COUNTRY_TYPE        0x07
#define AW_USB_CDC_NETWORK_TERMINAL_TYPE   0x0a     /* 网络中端描述符*/
#define AW_USB_CDC_ETHERNET_TYPE       0x0f         /* 以太网描述符*/
#define AW_USB_CDC_WHCM_TYPE       0x11
#define AW_USB_CDC_MDLM_TYPE       0x12             /* 移动直行模型描述符 */
#define AW_USB_CDC_MDLM_DETAIL_TYPE    0x13         /* 移动直行模型细节描述符 */
#define AW_USB_CDC_DMM_TYPE        0x14
#define AW_USB_CDC_OBEX_TYPE       0x15
#define AW_USB_CDC_NCM_TYPE        0x1a
#define AW_USB_CDC_MBIM_TYPE       0x1b
#define AW_USB_CDC_MBIM_EXTENDED_TYPE  0x1c

/* 表62; 多播滤波器位*/
#define AW_USB_CDC_PACKET_TYPE_PROMISCUOUS     (1 << 0)
#define AW_USB_CDC_PACKET_TYPE_ALL_MULTICAST   (1 << 1) /* 无滤波器 */
#define AW_USB_CDC_PACKET_TYPE_DIRECTED        (1 << 2)
#define AW_USB_CDC_PACKET_TYPE_BROADCAST       (1 << 3)
#define AW_USB_CDC_PACKET_TYPE_MULTICAST       (1 << 4) /* 已滤波*/

/* 类规范控制请求 (6.2)*/
#define AW_USB_CDC_SEND_ENCAPSULATED_COMMAND       0x00
#define AW_USB_CDC_GET_ENCAPSULATED_RESPONSE       0x01
#define AW_USB_CDC_REQ_SET_LINE_CODING             0x20
#define AW_USB_CDC_REQ_GET_LINE_CODING             0x21
#define AW_USB_CDC_REQ_SET_CONTROL_LINE_STATE      0x22
#define AW_USB_CDC_REQ_SEND_BREAK                  0x23
#define AW_USB_CDC_SET_ETHERNET_MULTICAST_FILTERS  0x40
#define AW_USB_CDC_SET_ETHERNET_PM_PATTERN_FILTER  0x41
#define AW_USB_CDC_GET_ETHERNET_PM_PATTERN_FILTER  0x42
#define AW_USB_CDC_SET_ETHERNET_PACKET_FILTER      0x43
#define AW_USB_CDC_GET_ETHERNET_STATISTIC          0x44
#define AW_USB_CDC_GET_NTB_PARAMETERS              0x80
#define AW_USB_CDC_GET_NET_ADDRESS                 0x81
#define AW_USB_CDC_SET_NET_ADDRESS                 0x82
#define AW_USB_CDC_GET_NTB_FORMAT                  0x83
#define AW_USB_CDC_SET_NTB_FORMAT                  0x84
#define AW_USB_CDC_GET_NTB_INPUT_SIZE              0x85
#define AW_USB_CDC_SET_NTB_INPUT_SIZE              0x86
#define AW_USB_CDC_GET_MAX_DATAGRAM_SIZE           0x87
#define AW_USB_CDC_SET_MAX_DATAGRAM_SIZE           0x88
#define AW_USB_CDC_GET_CRC_MODE                    0x89
#define AW_USB_CDC_SET_CRC_MODE                    0x8a

/* "header 功能描述符" CDC规范5.2.3.1*/
struct usb_cdc_header_desc {
    uint8_t    bLength;             /* 描述符大小(字节)*/
    uint8_t    bDescriptorType;     /* 描述符类型*/
    uint8_t    bDescriptorSubType;  /* 描述符子类类型*/

    uint16_t  bcdCDC;               /* 用于通信设备的USB类定义规范发布号(二进制编码十进制)*/
} __attribute__ ((packed));

/* "union 功能描述符" CDC规范5.2.3.8*/
struct usb_cdc_union_desc {
    uint8_t    bLength;              /* 描述符大小(字节)*/
    uint8_t    bDescriptorType;      /* 描述符类型*/
    uint8_t    bDescriptorSubType;   /* 描述符子类类型*/
    /* 配置中接口的索引是从0开始*/
    uint8_t    bMasterInterface0;    /* 通信或数据类接口的接口号，指定为联合的控制接口*/
    uint8_t    bSlaveInterface0;     /* 联合中第一个子接口的接口号*/
    /* 这里可以添加其他子接口*/
} __attribute__ ((packed));

/* "以太网网络功能描述符" CDC规范5.2.3.16*/
struct usb_cdc_ether_desc {
    uint8_t    bLength;               /* 描述符大小(字节)*/
    uint8_t    bDescriptorType;       /* 描述符类型*/
    uint8_t    bDescriptorSubType;    /* 描述符子类型*/

    uint8_t    iMACAddress;           /* 字符串描述符的索引。这个字符串描述符包含48位以太网MAC地址。
                                         MAC地址的Unicode表示如下：第一个Unicode字符表示网络字节序
                                                                                                                           的MAC地址第一个字节的高位半字节，下一个字符表示接下来的四
                                                                                                                           位，以此类推。iMACAddress不能为0。*/
    uint32_t  bmEthernetStatistics;   /* 指示设备收集的以太网统计功能。如果有一个位设置为0，则主机
                                                                                                                           网络驱动程序将保留响应统计的计数(如果可以)。*/
    uint16_t  wMaxSegmentSize;        /* 以太网设备支持的最大段大小，一般是1514字节，但可以被扩展*/
    uint16_t  wNumberMCFilters;       /* 包含主机可以配置的多播滤波器的数量。*/
    uint8_t    bNumberPowerFilters;   /* 包含可用于导致主机唤醒的典型滤波器的数量*/
} __attribute__ ((packed));

/* "抽象控制管理描述符" CDC规范5.2.3.3 */
struct usb_cdc_acm_descriptor {
    uint8_t    bLength;
    uint8_t    bDescriptorType;
    uint8_t    bDescriptorSubType;

    uint8_t    bmCapabilities;
} __attribute__ ((packed));

/* "移动直行模型功能性描述符" CDC WMC(Wireless Mobile Communication Device无线移动通讯设备)规范 6.7.2.3 */
struct usb_cdc_mdlm_desc {
    uint8_t    bLength;
    uint8_t    bDescriptorType;
    uint8_t    bDescriptorSubType;

    uint16_t  bcdVersion;
    uint8_t    bGUID[16];
} __attribute__ ((packed));

/* "移动直行模型细节功能性描述符" CDC WMC(无线移动通讯设备)规范  6.7.2.4 */
struct usb_cdc_mdlm_detail_desc {
    uint8_t    bLength;
    uint8_t    bDescriptorType;
    uint8_t    bDescriptorSubType;

    /* 类型关联于 mdlm_desc.bGUID */
    uint8_t    bGuidDescriptorType;
    uint8_t    bDetailData[0];
} __attribute__ ((packed));


/*
 * 通过中断传输发送的类特定通知(6.3)
 *
 * CDC规范的3.8.2节表11列出了以太网的通知
 * 第3.6.2.1节表5规定了RNDIS接受的ACM通知
 * RNDIS还定义了自己的位不兼容通知
 */

#define AWBL_USB_CDC_NOTIFY_NETWORK_CONNECTION   0x00    /* 网络连接*/
#define AWBL_USB_CDC_NOTIFY_RESPONSE_AVAILABLE   0x01    /* 得到响应*/
#define AWBL_USB_CDC_NOTIFY_SERIAL_STATE         0x20    /* 串行状态*/
#define AWBL_USB_CDC_NOTIFY_SPEED_CHANGE         0x2a    /* 速度该表*/

struct usb_cdc_notification {
    uint8_t    bmRequestType;
    uint8_t    bNotificationType;
    uint16_t  wValue;
    uint16_t  wIndex;
    uint16_t  wLength;
} __attribute__ ((packed));

#endif
