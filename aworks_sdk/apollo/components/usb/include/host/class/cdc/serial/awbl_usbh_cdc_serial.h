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
#ifndef __AWBL_USBH_CDC_SERIAL_H
#define __AWBL_USBH_CDC_SERIAL_H
#include "aw_usb_os.h"
#include "aw_list.h"
#include "host/awbl_usbh.h"

/* USBת���ڴ�ӡ����*/
#define __USB_SERIAL_TRACE(info, ...) \
            do { \
                AW_INFOF(("USBH-SERIAL:"info, ##__VA_ARGS__)); \
            } while (0)


#ifdef USB_SERIAL_DEBUG
    #define __USB_SERIAL_TRACE_DEBUG  __USB_SERIAL_TRACE
    #define usb_serial_printf  aw_kprintf
#else
    #define __USB_SERIAL_TRACE_DEBUG(info) \
            do { \
            } while (0)
#endif

#define default_vid 0xFFFF
#define default_pid 0xFFFF

#define USBH_SERIAL_PIPE_IN  0
#define USBH_SERIAL_PIPE_OUT 1

/* USBת����оƬ����*/
enum usb_serial_chip_type{
    UNIVERSAL = 0,         /* ͨ�õ�*/
    FTDI      = 1,         /* FTDIϵ��*/
    CP210x    = 2,
    CH34x     = 3,
    PL2303    = 4,
    OTHER     = 5,         /* 4GͨѶ�˿�*/
};

#define SERIAL_READ_SYNC_MODE   0
#define SERIAL_READ_ASYNC_MODE  1

/* USB�������ýṹ����*/
struct aw_usb_serial_dcb {
    uint32_t                baud_rate;           /* ������*/
    uint8_t                 read_mode;           /* ������ģʽ*/

    uint32_t                byte_size:4;         /* ָ����ǰ�˿�ʹ�õ�����λ(��Χ5~8)*/
    uint32_t                f_parity :1;         /* �Ƿ�������żУ��,Ϊ0������У�飬Ϊ1��У�鷽ʽ��parity��ֵ*/
    uint32_t                parity   :1;         /* ָ���˿����ݴ����У�鷽��*/
    uint32_t                stop_bits:2;         /* ָ���˿ڵ�ǰʹ�õ�ֹͣλ��*/
    uint32_t                      dtr:1;         /* �����ն�׼��*/
    uint32_t                      rts:1;         /* ������*/
    uint32_t                  crtscts:1;         /* ������*/
    uint32_t                    ixoff:1;
    uint32_t                f_dummy:24;
};

enum{
    USBH_SERIAL_RX_ASYNC_SUBMIT,
    USBH_SERIAL_RX_ASYNC_CANCEL,
    USBH_SERIAL_RX_ASYNC_DONE
};


/* USB�����ź����ṹ��*/
struct awbl_usbh_serial_semb{
    struct aw_list_head node;    /* �ڵ�*/
    aw_usb_sem_handle_t semb;    /* �ź���*/
    int pid;                     /* �豸pid*/
    int vid;                     /* �豸vid*/
    void *priv;                  /* ˽��������*/
};

/* USBת���ڹܵ��ṹ��*/
struct awbl_usbh_serial_pipe{
    struct awbl_usbh_endpoint *p_pipe;
    void                      *p_buf;
    AW_MUTEX_DECL(             lock);        /* USB�ܵ�������*/
    uint32_t                   size;         /* һ�δ�������ݴ�С*/
    uint32_t                   timeout;      /* ����/���ճ�ʱʱ��*/
};

/* USBת���ڽṹ��*/
struct awbl_usbh_serial {
    char                             name[32];     /* USB�����豸����*/
    struct awbl_usbh_function       *p_fun;        /* ��Ӧ�Ĺ����豸�ṹ��*/
    enum usb_serial_chip_type        chip_type;    /* ����оƬ����*/
    uint8_t                          nports;       /* �˿�����*/
    void                            *priv;         /* ˽������(�����оƬ���͵Ľṹ��)*/
    struct awbl_usbh_serial_pipe     p_in;         /* IN�ܵ�*/
    struct awbl_usbh_serial_pipe     p_out;        /* OUT�ܵ�*/
    struct awbl_usbh_trp            *rx_asyc_trp;  /* �첽���������*/
    uint8_t                          rx_asyc_sta;  /* �첽����״̬*/
    struct aw_list_head              node;         /* ��ǰUSB���ڽڵ�*/
    struct aw_refcnt                 ref;          /* USB�������ü���*/
    struct aw_usb_serial_dcb         cur_dcb;      /* ��ǰ�����ݿ��ƿ�*/
    aw_bool_t                        removed;      /* �Ƴ�״̬*/
    aw_bool_t                        is_init;      /* �Ƿ��Ѿ���ʼ�����*/
    aw_usb_task_handle_t             task;         /* �첽��������*/
    aw_usb_sem_handle_t              sem;          /* �첽�����ź���*/
    AW_MUTEX_DECL(                   lock);        /* USB���ڻ�����*/

    /* ͬ�����ص�����*/
    int (*read_sync) (struct awbl_usbh_serial *usb_serial, void *p_buf, int len);
    /* ͬ��д�ص�����*/
    int (*write_sync) (struct awbl_usbh_serial *usb_serial, void *p_buf, int len);
    /* �첽д�ص�����*/
    aw_err_t (*write_async) (struct awbl_usbh_serial *usb_serial, void *p_buf, int len);
    /* ���ô��ڻص�����*/
    aw_err_t (*dcb_set) (struct awbl_usbh_serial *usb_serial, struct aw_usb_serial_dcb *p_dcb);
    /* ���ص�����*/
    aw_err_t (*intr) (struct awbl_usbh_serial *usb_serial, int len);
    /* �Ƴ�����*/
    aw_err_t (*remove) (void *arg);
};

/* USB���ڹܵ���ʼ��*/
aw_err_t __usb_serial_pipe_init (struct awbl_usbh_serial *usb_serial,
                               struct awbl_usbh_function *p_fun);
/* ��USBת����*/
struct awbl_usbh_serial *awbl_usbh_serial_open(int pid, int vid, int intfnum);

/* �ȴ��򿪶�Ӧ��USB����
 * pid    ��USBת���豸�ĳ�ƷID
 * vid    ��USBת���豸�Ĺ�Ӧ��ID
 * intfnum��ָ���ض��Ľӿ�(����Ҫ������Ϊ-1)
 * timeout���ȴ���ʱʱ��
 * ���� ���ɹ������ҵ���USB���ڽṹ��*/
struct awbl_usbh_serial *awbl_usbh_serial_wait_open(int pid, int vid, int intfnum, int timeout);

/* �ر�USBת����*/
void awbl_usbh_serial_close (void *usb_serial);
/* ��ȡUSB�������ݿ��ƿ�*/
aw_err_t aw_usb_serial_dcb_get (void *serial, struct aw_usb_serial_dcb *p_dcb);
/* ����USB�������ݿ��ƿ�*/
aw_err_t aw_usb_serial_dcb_set (void *serial, struct aw_usb_serial_dcb *p_dcb);
#if 0
/* ��ȡUSB�����¼���ʱ����*/
aw_err_t aw_usb_serial_timeout_get (void *serial, uint8_t pipe_type, uint32_t *timeout);
/* ����USB�����¼���ʱ����*/
aw_err_t aw_usb_serial_timeout_set (void *serial, uint8_t pipe_type, uint32_t *timeout);
#endif
/* ��ȡUSB�����¼���ʱ����*/
aw_err_t aw_usb_serial_timeout_get (void *serial, uint32_t *timeout);
/* ����USB�����¼���ʱ����*/
aw_err_t aw_usb_serial_timeout_set (void *serial, uint32_t *timeout);
/* ���õ��δ����С*/
aw_err_t awbl_usbh_serial_set_tranfersize(struct awbl_usbh_serial *usb_serial,
                                                uint8_t  pipe_type,
                                                uint32_t tranfer_size);
/* ��ȡ���δ����С*/
aw_err_t awbl_usbh_serial_get_tranfersize(struct awbl_usbh_serial *usb_serial,
                                                uint8_t  pipe_type,
                                                uint32_t *tranfer_size);
/* USB����ͬ�����ͺ���*/
int aw_usb_serial_write_sync (void *serial, void *p_buffer, size_t nbytes);
/* USB�����첽���ͺ���*/
aw_err_t aw_usb_serial_write_async (void *serial, void *p_buffer, size_t nbytes);
/* USB����ͬ��������*/
int aw_usb_serial_read_sync (void *serial, void *p_buffer, size_t nbytes);
/* USB�����첽������*/
int aw_usb_serial_read_async (void *serial, void *p_buffer, size_t nbytes);
/* �����豸��ʼ��*/
aw_err_t awbl_usbh_serial_other_init(struct awbl_usbh_serial *usb_serial);
#endif
