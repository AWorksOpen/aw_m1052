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

#define NO_FRAME    29999           /* ��δ����֡*/

/* ����ʹ��*/
//#define EHCI_BANDWIDTH_SIZE 64

#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))

/* UVC��ӡ����*/
#define __USB_ISO_TRACE(info) \
            do { \
                AW_INFOF(("USBH-ISO: ")); \
                AW_INFOF(info); \
            } while (0)

/* ͨ�ô�����䳣��/֧��*/
#define BitTime(bytecount) (7 * 8 * bytecount / 6)
#define NS_TO_US(ns)    AW_DIV_ROUND_UP(ns, 1000L)
#define USB2_HOST_DELAY 5
#define HS_NSECS_ISO(bytes) (((38 * 8 * 2083) \
    + (2083UL * (3 + BitTime(bytes))))/1000 \
    + USB2_HOST_DELAY)
#define HS_USECS_ISO(bytes) NS_TO_US(HS_NSECS_ISO(bytes))


/* ���ٶ˵��������б���ĸߴ���˷���*/
#define hb_mult(wMaxPacketSize) (1 + (((wMaxPacketSize) >> 11) & 0x03))
/* ...�Ͱ���С�������κ����͵Ķ˵�������*/
#define max_packet(wMaxPacketSize) ((wMaxPacketSize) & 0x07ff)

/* Ϊ�ָ�����ĸ����豸��ȫ/�����豸�����ڴ�����Ⱥ�Ԥ����Ϣ*/
struct ehci_per_sched {
    struct awbl_usbh_device    *udev;           /* ��������ת����*/
    struct awbl_usbh_endpoint  *ep;

    uint16_t                   phase;           /* ��ʵʱ�ڣ�֡����*/
    uint8_t                    bw_phase;        /* һ����Ϊ������*/
    uint8_t                    phase_uf;        /* uframe part of the phase */
    uint8_t                    usecs, c_usecs;  /* ���������ϵ�ʱ��*/
    uint8_t                    bw_uperiod;      /* ����Ԥ����΢֡����*/
    uint16_t                   period;          /* ֡����ʵ����*/
    uint8_t                    bw_period;       /* ����֡����*/
};

/* һ����ʱ���������������(����Ǹ������3KB)*/
struct ehci_iso_packet {
    /* These will be copied to iTD when scheduling */
    uint64_t         bufp;               /* itd->hw_bufp{,_hi}[pg] |= */
    uint32_t          transaction;       /* itd->hw_transaction[i] |= */
    uint8_t          cross;              /* �����ҳ*/
    /* ����ȫ������ָ�*/
    uint32_t         buf1;
};

/* ���Ե�ʱUSB���������ݰ���ʱ��������(�����ٶ�)��ÿ�����ݰ����豸��
 * һ���߼�USB����(���Ƿָ�����)����stream->next_uframe ��ʼ*/
struct ehci_iso_sched {
    struct aw_list_head td_list;         /* ��������������*/
    unsigned        span;                /* ��ε�����֡�����е�֡/΢֡���*/
    unsigned        first_packet;
    struct ehci_iso_packet  *packet;
};

///* EHCI��ʱ���ṹ��*/
//struct ehci_iso_stream {
//    struct awbl_usbh_ehci_qh *hw;
//    struct aw_list_head td_list;    /* ����еĵ�ʱ����������/�ָ������ʱ����������*/
//    struct aw_list_head free_list;  /* ���еĵ�ʱ����������/�ָ������ʱ����������*/
//    struct ehci_per_sched   ps;     /* ������Ϣ*/
//    uint32_t next_uframe;
//
//    uint8_t bEndpointAddress;       /* �˵��ַ*/
//    uint8_t highspeed;              /* ���ٴ���*/
//    uint16_t uperiod;               /* ΢֡����*/
//    uint32_t bandwidth;             /* ����*/
//    uint16_t maxp;                  /* ������С*/
//    uint32_t splits;                /* �ָ������־*/
//    uint32_t refcount;
//    struct awbl_usbh_endpoint *ep;
//
//    /* �������ڳ�ʼ����ʱ������������ hw_bufp �ֶ�*/
//    uint32_t buf0;
//    uint32_t buf1;
//    uint32_t buf2;
//};

/* EHCI��ʱ���ṹ��*/
struct ehci_iso_stream {
    struct awbl_usbh_ehci_qh *hw;
    struct awbl_usbh_device  *udev; /* ��������ת����*/
    struct aw_list_head td_list;    /* ����еĵ�ʱ����������/�ָ������ʱ����������*/
    struct aw_list_head free_list;  /* ���еĵ�ʱ����������/�ָ������ʱ����������*/
    uint32_t next_uframe;

    uint8_t bEndpointAddress;       /* �˵��ַ*/
    uint8_t highspeed;              /* ���ٴ���*/
    uint16_t uperiod;               /* ΢֡����*/
    uint32_t bandwidth;             /* ����*/
    uint16_t maxp;                  /* ������С*/
    uint32_t splits;                /* �ָ������־*/
    uint32_t refcount;              /* 2.6*/
    struct awbl_usbh_endpoint *ep;  /* 2.6*/
    uint8_t usecs, c_usecs;         /* 2.6*/
    uint8_t interval;               /* 2.6*/
    uint16_t depth;                 /* 2.6*/
    struct ehci_per_sched   ps;     /* ������Ϣ 4.x.y*/

    /* �������ڳ�ʼ����ʱ������������ hw_bufp �ֶ�*/
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
