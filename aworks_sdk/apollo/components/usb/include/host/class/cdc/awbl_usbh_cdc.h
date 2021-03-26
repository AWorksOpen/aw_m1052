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

/* USB CDC�������*/
#define AW_USB_CDC_SUBCLASS_ACM            0x02
#define AW_USB_CDC_SUBCLASS_ETHERNET       0x06   /* ��̫��*/
#define AW_USB_CDC_SUBCLASS_WHCM           0x08
#define AW_USB_CDC_SUBCLASS_DMM            0x09
#define AW_USB_CDC_SUBCLASS_MDLM           0x0a
#define AW_USB_CDC_SUBCLASS_OBEX           0x0b
#define AW_USB_CDC_SUBCLASS_EEM            0x0c
#define AW_USB_CDC_SUBCLASS_NCM            0x0d
#define AW_USB_CDC_SUBCLASS_MBIM           0x0e

/* ��Э������*/
#define AW_USB_CDC_PROTO_NONE          0


/* �ض���������*/
#define AW_USB_CDC_HEADER_TYPE     0x00             /* ͷ������*/
#define AW_USB_CDC_CALL_MANAGEMENT_TYPE    0x01     /* call_mgmt_descriptor */
#define AW_USB_CDC_ACM_TYPE        0x02             /* ACM������*/
#define AW_USB_CDC_UNION_TYPE      0x06             /* ����������*/
#define AW_USB_CDC_COUNTRY_TYPE        0x07
#define AW_USB_CDC_NETWORK_TERMINAL_TYPE   0x0a     /* �����ж�������*/
#define AW_USB_CDC_ETHERNET_TYPE       0x0f         /* ��̫��������*/
#define AW_USB_CDC_WHCM_TYPE       0x11
#define AW_USB_CDC_MDLM_TYPE       0x12             /* �ƶ�ֱ��ģ�������� */
#define AW_USB_CDC_MDLM_DETAIL_TYPE    0x13         /* �ƶ�ֱ��ģ��ϸ�������� */
#define AW_USB_CDC_DMM_TYPE        0x14
#define AW_USB_CDC_OBEX_TYPE       0x15
#define AW_USB_CDC_NCM_TYPE        0x1a
#define AW_USB_CDC_MBIM_TYPE       0x1b
#define AW_USB_CDC_MBIM_EXTENDED_TYPE  0x1c

/* ��62; �ಥ�˲���λ*/
#define AW_USB_CDC_PACKET_TYPE_PROMISCUOUS     (1 << 0)
#define AW_USB_CDC_PACKET_TYPE_ALL_MULTICAST   (1 << 1) /* ���˲��� */
#define AW_USB_CDC_PACKET_TYPE_DIRECTED        (1 << 2)
#define AW_USB_CDC_PACKET_TYPE_BROADCAST       (1 << 3)
#define AW_USB_CDC_PACKET_TYPE_MULTICAST       (1 << 4) /* ���˲�*/

/* ��淶�������� (6.2)*/
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

/* "header ����������" CDC�淶5.2.3.1*/
struct usb_cdc_header_desc {
    uint8_t    bLength;             /* ��������С(�ֽ�)*/
    uint8_t    bDescriptorType;     /* ����������*/
    uint8_t    bDescriptorSubType;  /* ��������������*/

    uint16_t  bcdCDC;               /* ����ͨ���豸��USB�ඨ��淶������(�����Ʊ���ʮ����)*/
} __attribute__ ((packed));

/* "union ����������" CDC�淶5.2.3.8*/
struct usb_cdc_union_desc {
    uint8_t    bLength;              /* ��������С(�ֽ�)*/
    uint8_t    bDescriptorType;      /* ����������*/
    uint8_t    bDescriptorSubType;   /* ��������������*/
    /* �����нӿڵ������Ǵ�0��ʼ*/
    uint8_t    bMasterInterface0;    /* ͨ�Ż�������ӿڵĽӿںţ�ָ��Ϊ���ϵĿ��ƽӿ�*/
    uint8_t    bSlaveInterface0;     /* �����е�һ���ӽӿڵĽӿں�*/
    /* ���������������ӽӿ�*/
} __attribute__ ((packed));

/* "��̫�����繦��������" CDC�淶5.2.3.16*/
struct usb_cdc_ether_desc {
    uint8_t    bLength;               /* ��������С(�ֽ�)*/
    uint8_t    bDescriptorType;       /* ����������*/
    uint8_t    bDescriptorSubType;    /* ������������*/

    uint8_t    iMACAddress;           /* �ַ���������������������ַ�������������48λ��̫��MAC��ַ��
                                         MAC��ַ��Unicode��ʾ���£���һ��Unicode�ַ���ʾ�����ֽ���
                                                                                                                           ��MAC��ַ��һ���ֽڵĸ�λ���ֽڣ���һ���ַ���ʾ����������
                                                                                                                           λ���Դ����ơ�iMACAddress����Ϊ0��*/
    uint32_t  bmEthernetStatistics;   /* ָʾ�豸�ռ�����̫��ͳ�ƹ��ܡ������һ��λ����Ϊ0��������
                                                                                                                           �����������򽫱�����Ӧͳ�Ƶļ���(�������)��*/
    uint16_t  wMaxSegmentSize;        /* ��̫���豸֧�ֵ����δ�С��һ����1514�ֽڣ������Ա���չ*/
    uint16_t  wNumberMCFilters;       /* ���������������õĶಥ�˲�����������*/
    uint8_t    bNumberPowerFilters;   /* ���������ڵ����������ѵĵ����˲���������*/
} __attribute__ ((packed));

/* "������ƹ���������" CDC�淶5.2.3.3 */
struct usb_cdc_acm_descriptor {
    uint8_t    bLength;
    uint8_t    bDescriptorType;
    uint8_t    bDescriptorSubType;

    uint8_t    bmCapabilities;
} __attribute__ ((packed));

/* "�ƶ�ֱ��ģ�͹�����������" CDC WMC(Wireless Mobile Communication Device�����ƶ�ͨѶ�豸)�淶 6.7.2.3 */
struct usb_cdc_mdlm_desc {
    uint8_t    bLength;
    uint8_t    bDescriptorType;
    uint8_t    bDescriptorSubType;

    uint16_t  bcdVersion;
    uint8_t    bGUID[16];
} __attribute__ ((packed));

/* "�ƶ�ֱ��ģ��ϸ�ڹ�����������" CDC WMC(�����ƶ�ͨѶ�豸)�淶  6.7.2.4 */
struct usb_cdc_mdlm_detail_desc {
    uint8_t    bLength;
    uint8_t    bDescriptorType;
    uint8_t    bDescriptorSubType;

    /* ���͹����� mdlm_desc.bGUID */
    uint8_t    bGuidDescriptorType;
    uint8_t    bDetailData[0];
} __attribute__ ((packed));


/*
 * ͨ���жϴ��䷢�͵����ض�֪ͨ(6.3)
 *
 * CDC�淶��3.8.2�ڱ�11�г�����̫����֪ͨ
 * ��3.6.2.1�ڱ�5�涨��RNDIS���ܵ�ACM֪ͨ
 * RNDIS���������Լ���λ������֪ͨ
 */

#define AWBL_USB_CDC_NOTIFY_NETWORK_CONNECTION   0x00    /* ��������*/
#define AWBL_USB_CDC_NOTIFY_RESPONSE_AVAILABLE   0x01    /* �õ���Ӧ*/
#define AWBL_USB_CDC_NOTIFY_SERIAL_STATE         0x20    /* ����״̬*/
#define AWBL_USB_CDC_NOTIFY_SPEED_CHANGE         0x2a    /* �ٶȸñ�*/

struct usb_cdc_notification {
    uint8_t    bmRequestType;
    uint8_t    bNotificationType;
    uint16_t  wValue;
    uint16_t  wIndex;
    uint16_t  wLength;
} __attribute__ ((packed));

#endif
