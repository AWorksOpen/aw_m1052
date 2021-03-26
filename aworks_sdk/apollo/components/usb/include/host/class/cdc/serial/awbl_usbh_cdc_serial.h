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

/* USB转串口打印函数*/
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

/* USB转串口芯片类型*/
enum usb_serial_chip_type{
    UNIVERSAL = 0,         /* 通用的*/
    FTDI      = 1,         /* FTDI系列*/
    CP210x    = 2,
    CH34x     = 3,
    PL2303    = 4,
    OTHER     = 5,         /* 4G通讯端口*/
};

#define SERIAL_READ_SYNC_MODE   0
#define SERIAL_READ_ASYNC_MODE  1

/* USB串口配置结构描述*/
struct aw_usb_serial_dcb {
    uint32_t                baud_rate;           /* 波特率*/
    uint8_t                 read_mode;           /* 读数据模式*/

    uint32_t                byte_size:4;         /* 指定当前端口使用的数据位(范围5~8)*/
    uint32_t                f_parity :1;         /* 是否允许奇偶校验,为0不允许校验，为1则校验方式看parity的值*/
    uint32_t                parity   :1;         /* 指定端口数据传输的校验方法*/
    uint32_t                stop_bits:2;         /* 指定端口当前使用的停止位数*/
    uint32_t                      dtr:1;         /* 数据终端准备*/
    uint32_t                      rts:1;         /* 请求发送*/
    uint32_t                  crtscts:1;         /* 流控制*/
    uint32_t                    ixoff:1;
    uint32_t                f_dummy:24;
};

enum{
    USBH_SERIAL_RX_ASYNC_SUBMIT,
    USBH_SERIAL_RX_ASYNC_CANCEL,
    USBH_SERIAL_RX_ASYNC_DONE
};


/* USB串口信号量结构体*/
struct awbl_usbh_serial_semb{
    struct aw_list_head node;    /* 节点*/
    aw_usb_sem_handle_t semb;    /* 信号量*/
    int pid;                     /* 设备pid*/
    int vid;                     /* 设备vid*/
    void *priv;                  /* 私有数据域*/
};

/* USB转串口管道结构体*/
struct awbl_usbh_serial_pipe{
    struct awbl_usbh_endpoint *p_pipe;
    void                      *p_buf;
    AW_MUTEX_DECL(             lock);        /* USB管道互斥锁*/
    uint32_t                   size;         /* 一次传输的数据大小*/
    uint32_t                   timeout;      /* 发送/接收超时时间*/
};

/* USB转串口结构体*/
struct awbl_usbh_serial {
    char                             name[32];     /* USB串口设备名字*/
    struct awbl_usbh_function       *p_fun;        /* 相应的功能设备结构体*/
    enum usb_serial_chip_type        chip_type;    /* 具体芯片类型*/
    uint8_t                          nports;       /* 端口数量*/
    void                            *priv;         /* 私有数据(放相关芯片类型的结构体)*/
    struct awbl_usbh_serial_pipe     p_in;         /* IN管道*/
    struct awbl_usbh_serial_pipe     p_out;        /* OUT管道*/
    struct awbl_usbh_trp            *rx_asyc_trp;  /* 异步接收请求包*/
    uint8_t                          rx_asyc_sta;  /* 异步接收状态*/
    struct aw_list_head              node;         /* 当前USB串口节点*/
    struct aw_refcnt                 ref;          /* USB串口引用计数*/
    struct aw_usb_serial_dcb         cur_dcb;      /* 当前的数据控制块*/
    aw_bool_t                        removed;      /* 移除状态*/
    aw_bool_t                        is_init;      /* 是否已经初始化完成*/
    aw_usb_task_handle_t             task;         /* 异步接收任务*/
    aw_usb_sem_handle_t              sem;          /* 异步接收信号量*/
    AW_MUTEX_DECL(                   lock);        /* USB串口互斥锁*/

    /* 同步读回调函数*/
    int (*read_sync) (struct awbl_usbh_serial *usb_serial, void *p_buf, int len);
    /* 同步写回调函数*/
    int (*write_sync) (struct awbl_usbh_serial *usb_serial, void *p_buf, int len);
    /* 异步写回调函数*/
    aw_err_t (*write_async) (struct awbl_usbh_serial *usb_serial, void *p_buf, int len);
    /* 设置串口回调函数*/
    aw_err_t (*dcb_set) (struct awbl_usbh_serial *usb_serial, struct aw_usb_serial_dcb *p_dcb);
    /* 读回调函数*/
    aw_err_t (*intr) (struct awbl_usbh_serial *usb_serial, int len);
    /* 移除函数*/
    aw_err_t (*remove) (void *arg);
};

/* USB串口管道初始化*/
aw_err_t __usb_serial_pipe_init (struct awbl_usbh_serial *usb_serial,
                               struct awbl_usbh_function *p_fun);
/* 打开USB转串口*/
struct awbl_usbh_serial *awbl_usbh_serial_open(int pid, int vid, int intfnum);

/* 等待打开对应的USB串口
 * pid    ：USB转串设备的厂品ID
 * vid    ：USB转串设备的供应商ID
 * intfnum：指定特定的接口(不需要则设置为-1)
 * timeout：等待超时时间
 * 返回 ：成功返回找到的USB串口结构体*/
struct awbl_usbh_serial *awbl_usbh_serial_wait_open(int pid, int vid, int intfnum, int timeout);

/* 关闭USB转串口*/
void awbl_usbh_serial_close (void *usb_serial);
/* 获取USB串口数据控制块*/
aw_err_t aw_usb_serial_dcb_get (void *serial, struct aw_usb_serial_dcb *p_dcb);
/* 设置USB串口数据控制块*/
aw_err_t aw_usb_serial_dcb_set (void *serial, struct aw_usb_serial_dcb *p_dcb);
#if 0
/* 获取USB串口事件超时参数*/
aw_err_t aw_usb_serial_timeout_get (void *serial, uint8_t pipe_type, uint32_t *timeout);
/* 设置USB串口事件超时参数*/
aw_err_t aw_usb_serial_timeout_set (void *serial, uint8_t pipe_type, uint32_t *timeout);
#endif
/* 获取USB串口事件超时参数*/
aw_err_t aw_usb_serial_timeout_get (void *serial, uint32_t *timeout);
/* 设置USB串口事件超时参数*/
aw_err_t aw_usb_serial_timeout_set (void *serial, uint32_t *timeout);
/* 设置单次传输大小*/
aw_err_t awbl_usbh_serial_set_tranfersize(struct awbl_usbh_serial *usb_serial,
                                                uint8_t  pipe_type,
                                                uint32_t tranfer_size);
/* 获取单次传输大小*/
aw_err_t awbl_usbh_serial_get_tranfersize(struct awbl_usbh_serial *usb_serial,
                                                uint8_t  pipe_type,
                                                uint32_t *tranfer_size);
/* USB串口同步发送函数*/
int aw_usb_serial_write_sync (void *serial, void *p_buffer, size_t nbytes);
/* USB串口异步发送函数*/
aw_err_t aw_usb_serial_write_async (void *serial, void *p_buffer, size_t nbytes);
/* USB串口同步读函数*/
int aw_usb_serial_read_sync (void *serial, void *p_buffer, size_t nbytes);
/* USB串口异步读函数*/
int aw_usb_serial_read_async (void *serial, void *p_buffer, size_t nbytes);
/* 其他设备初始化*/
aw_err_t awbl_usbh_serial_other_init(struct awbl_usbh_serial *usb_serial);
#endif
