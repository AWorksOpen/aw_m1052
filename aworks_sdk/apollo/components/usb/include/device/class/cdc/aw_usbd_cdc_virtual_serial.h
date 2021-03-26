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


#ifndef __AWBL_USBD_CDC_VIRTUAL_SERIAL_H
#define __AWBL_USBD_CDC_VIRTUAL_SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_usb_os.h"
#include "device/aw_usbd.h"
#include "aw_list.h"
#include "aw_rngbuf.h"

#define AW_USBD_CDC_SERIAL_TASK_SIZE         1024     /* 数据接收任务堆栈 */
//#define AW_USBD_CDC_PASS_BACK                     /* 是否开启CDC回传--定义则开启 */

/** \brief RX TX buffer size */
#define AW_USBD_CDC_RX_RNGBUF_SIZE      (1024)
#define AW_USBD_CDC_TX_RNGBUF_SIZE      (1024)

/** \brief CDC数据接收/发送交换缓冲区字节大小 */
#define AW_USBD_CDC_RX_SWAP_SIZE        (1024)
#define AW_USBD_CDC_TX_SWAP_SIZE        (1024)

//#define AW_USBD_CDC_ANLYZER                       /* 是否开启数据统计分析--调试使用  */

//#define AW_USBD_MAX_CDC_DEV              5        /* 系统支持的最多的CDC串口个数 */


/*----------------------------------------------------------------------------
 *      Definitions  based on usbcdc11.pdf (www.usb.org)
 *---------------------------------------------------------------------------*/
/* Communication device class specification version 1.10 */
#define AW_CDC_V1_10                               0x0110

/* Communication interface class code */
/* (usbcdc11.pdf, 4.2, Table 15) */
#define AW_CDC_COMMUNICATION_INTERFACE_CLASS       0x02

/* Communication interface class subclass codes */
/* (usbcdc11.pdf, 4.3, Table 16) */
#define AW_CDC_DIRECT_LINE_CONTROL_MODEL           0x01
#define AW_CDC_ABSTRACT_CONTROL_MODEL              0x02
#define AW_CDC_TELEPHONE_CONTROL_MODEL             0x03
#define AW_CDC_MULTI_CHANNEL_CONTROL_MODEL         0x04
#define AW_CDC_CAPI_CONTROL_MODEL                  0x05
#define AW_CDC_ETHERNET_NETWORKING_CONTROL_MODEL   0x06
#define AW_CDC_ATM_NETWORKING_CONTROL_MODEL        0x07

/* Communication interface class control protocol codes */
/* (usbcdc11.pdf, 4.4, Table 17) */
#define AW_CDC_PROTOCOL_COMMON_AT_COMMANDS         0x01

/* Data interface class code */
/* (usbcdc11.pdf, 4.5, Table 18) */
#define AW_CDC_DATA_INTERFACE_CLASS                0x0A

/* Data interface class protocol codes */
/* (usbcdc11.pdf, 4.7, Table 19) */
#define AW_CDC_PROTOCOL_ISDN_BRI                   0x30
#define AW_CDC_PROTOCOL_HDLC                       0x31
#define AW_CDC_PROTOCOL_TRANSPARENT                0x32
#define AW_CDC_PROTOCOL_Q921_MANAGEMENT            0x50
#define AW_CDC_PROTOCOL_Q921_DATA_LINK             0x51
#define AW_CDC_PROTOCOL_Q921_MULTIPLEXOR           0x52
#define AW_CDC_PROTOCOL_V42                        0x90
#define AW_CDC_PROTOCOL_EURO_ISDN                  0x91
#define AW_CDC_PROTOCOL_V24_RATE_ADAPTATION        0x92
#define AW_CDC_PROTOCOL_CAPI                       0x93
#define AW_CDC_PROTOCOL_HOST_BASED_DRIVER          0xFD
#define AW_CDC_PROTOCOL_DESCRIBED_IN_PUFD          0xFE

/* Type values for bDescriptorType field of functional descriptors */
/* (usbcdc11.pdf, 5.2.3, Table 24) */
#define AW_CDC_CS_INTERFACE                        0x24
#define AW_CDC_CS_ENDPOINT                         0x25

/* Type values for bDescriptorSubtype field of functional descriptors */
/* (usbcdc11.pdf, 5.2.3, Table 25) */
#define AW_CDC_HEADER                              0x00
#define AW_CDC_CALL_MANAGEMENT                     0x01
#define AW_CDC_ABSTRACT_CONTROL_MANAGEMENT         0x02
#define AW_CDC_DIRECT_LINE_MANAGEMENT              0x03
#define AW_CDC_TELEPHONE_RINGER                    0x04
#define AW_CDC_REPORTING_CAPABILITIES              0x05
#define AW_CDC_UNION                               0x06
#define AW_CDC_COUNTRY_SELECTION                   0x07
#define AW_CDC_TELEPHONE_OPERATIONAL_MODES         0x08
#define AW_CDC_USB_TERMINAL                        0x09
#define AW_CDC_NETWORK_CHANNEL                     0x0A
#define AW_CDC_PROTOCOL_UNIT                       0x0B
#define AW_CDC_EXTENSION_UNIT                      0x0C
#define AW_CDC_MULTI_CHANNEL_MANAGEMENT            0x0D
#define AW_CDC_CAPI_CONTROL_MANAGEMENT             0x0E
#define AW_CDC_ETHERNET_NETWORKING                 0x0F
#define AW_CDC_ATM_NETWORKING                      0x10

/* CDC class-specific request codes */
/* (usbcdc11.pdf, 6.2, Table 46) */
/* see Table 45 for info about the specific requests. */
#define AW_CDC_SEND_ENCAPSULATED_COMMAND           0x00
#define AW_CDC_GET_ENCAPSULATED_RESPONSE           0x01
#define AW_CDC_SET_COMM_FEATURE                    0x02
#define AW_CDC_GET_COMM_FEATURE                    0x03
#define AW_CDC_CLEAR_COMM_FEATURE                  0x04
#define AW_CDC_SET_AUX_LINE_STATE                  0x10
#define AW_CDC_SET_HOOK_STATE                      0x11
#define AW_CDC_PULSE_SETUP                         0x12
#define AW_CDC_SEND_PULSE                          0x13
#define AW_CDC_SET_PULSE_TIME                      0x14
#define AW_CDC_RING_AUX_JACK                       0x15
#define AW_CDC_SET_LINE_CODING                     0x20
#define AW_CDC_GET_LINE_CODING                     0x21
#define AW_CDC_SET_CONTROL_LINE_STATE              0x22
#define AW_CDC_SEND_BREAK                          0x23
#define AW_CDC_SET_RINGER_PARMS                    0x30
#define AW_CDC_GET_RINGER_PARMS                    0x31
#define AW_CDC_SET_OPERATION_PARMS                 0x32
#define AW_CDC_GET_OPERATION_PARMS                 0x33
#define AW_CDC_SET_LINE_PARMS                      0x34
#define AW_CDC_GET_LINE_PARMS                      0x35
#define AW_CDC_DIAL_DIGITS                         0x36
#define AW_CDC_SET_UNIT_PARAMETER                  0x37
#define AW_CDC_GET_UNIT_PARAMETER                  0x38
#define AW_CDC_CLEAR_UNIT_PARAMETER                0x39
#define AW_CDC_GET_PROFILE                         0x3A
#define AW_CDC_SET_ETHERNET_MULTICAST_FILTERS      0x40
#define AW_CDC_SET_ETHERNET_PMP_FILTER             0x41
#define AW_CDC_GET_ETHERNET_PMP_FILTER             0x42
#define AW_CDC_SET_ETHERNET_PACKET_FILTER          0x43
#define AW_CDC_GET_ETHERNET_STATISTIC              0x44
#define AW_CDC_SET_ATM_DATA_FORMAT                 0x50
#define AW_CDC_GET_ATM_DEVICE_STATISTICS           0x51
#define AW_CDC_SET_ATM_DEFAULT_VC                  0x52
#define AW_CDC_GET_ATM_VC_STATISTICS               0x53

/* Communication feature selector codes */
/* (usbcdc11.pdf, 6.2.2..6.2.4, Table 47) */
#define AW_CDC_ABSTRACT_STATE                      0x01
#define AW_CDC_COUNTRY_SETTING                     0x02

/* Feature Status returned for ABSTRACT_STATE Selector */
/* (usbcdc11.pdf, 6.2.3, Table 48) */
#define AW_CDC_IDLE_SETTING                        (1 << 0)
#define AW_CDC_DATA_MULTPLEXED_STATE               (1 << 1)


/* Control signal bitmap values for the SetControlLineState request */
/* (usbcdc11.pdf, 6.2.14, Table 51) */
#define AW_CDC_DTE_PRESENT                         (1 << 0)
#define AW_CDC_ACTIVATE_CARRIER                    (1 << 1)

/* CDC class-specific notification codes */
/* (usbcdc11.pdf, 6.3, Table 68) */
/* see Table 67 for Info about class-specific notifications */
#define AW_CDC_NOTIFICATION_NETWORK_CONNECTION     0x00
#define AW_CDC_RESPONSE_AVAILABLE                  0x01
#define AW_CDC_AUX_JACK_HOOK_STATE                 0x08
#define AW_CDC_RING_DETECT                         0x09
#define AW_CDC_NOTIFICATION_SERIAL_STATE           0x20
#define AW_CDC_CALL_STATE_CHANGE                   0x28
#define AW_CDC_LINE_STATE_CHANGE                   0x29
#define AW_CDC_CONNECTION_SPEED_CHANGE             0x2A

/* UART state bitmap values (Serial state notification). */
/* (usbcdc11.pdf, 6.3.5, Table 69) */
#define AW_CDC_SERIAL_STATE_OVERRUN                (1 << 6)  /* receive data overrun error has occurred */
#define AW_CDC_SERIAL_STATE_PARITY                 (1 << 5)  /* parity error has occurred */
#define AW_CDC_SERIAL_STATE_FRAMING                (1 << 4)  /* framing error has occurred */
#define AW_CDC_SERIAL_STATE_RING                   (1 << 3)  /* state of ring signal detection */
#define AW_CDC_SERIAL_STATE_BREAK                  (1 << 2)  /* state of break detection */
#define AW_CDC_SERIAL_STATE_TX_CARRIER             (1 << 1)  /* state of transmission carrier */
#define AW_CDC_SERIAL_STATE_RX_CARRIER             (1 << 0)  /* state of receiver carrier */


#ifdef AW_USBD_CDC_ANLYZER
struct cdc_alyser {
    uint32_t max_buf;
    uint32_t total_len;
};
#endif


enum{
    AW_CDC_VS_TRANFER_ERROR = 0,
    AW_CDC_VS_TRANFER_SYNC,
    AW_CDC_VS_TRANFER_ASYNC
};


/** \brief cdc串口参数结构体 line_coding */
typedef struct aw_usbd_cdc_line_coding {
  uint32_t  baud_rate;
  uint8_t   stop_bits;
  uint8_t   parity;
  uint8_t   word_length;
} __attribute__ ((packed)) aw_usbd_cdc_line_coding_t;


/* USB设备CDC 虚拟串口结构体 */
typedef struct aw_usbd_cdc_vs {
    struct aw_usbd_fun          cfun;          /* 命令接口*/
    struct aw_usbd_pipe         intr;          /* 中断管道*/
    struct aw_usbd_fun          dfun;          /* 数据接口*/
    struct aw_usbd_pipe         in;            /* 输入管道*/
    struct aw_usbd_pipe         out;           /* 输出管道*/
    uint8_t                     id;            /* 设备id编号 */
    char                        name[32];      /* 虚拟串口名字*/
    aw_usbd_cdc_line_coding_t   line_control;  /* 串口配置*/
    aw_usb_mutex_handle_t       mutex;         /* 设备互斥锁*/
    struct aw_list_head         node;          /* 设备节点 */
    char                       *p_rbuf;        /* 接收缓存*/
    aw_usb_sem_handle_t         sync_semb;     /* 信号量*/
    aw_task_id_t                rx_task;       /* 接收任务*/
    struct aw_rngbuf            r_rngbuf;      /* 环形缓冲区*/
    uint8_t                     tx_mode;       /* 发送模式*/
    uint8_t                     rx_mode;       /* 接收模式*/

    int                       (*pfn_rx_callback)(void *p_buf, int len, void *p_arg);
    void                       *p_rxcb_arg;
}aw_usbd_cdc_vs_t;

/**
 * \brief 创建虚拟串口设备
 *
 */
aw_err_t aw_usbd_cdc_vs_create (struct aw_usbd        *p_obj,
                                struct aw_usbd_cdc_vs *p_vs,
                                uint8_t                id,
                                const char            *p_name,
                                int                    buf_size);
/**
 * \brief 销毁虚拟串口设备
 *
 */
aw_err_t aw_usbd_cdc_vs_destroy (struct aw_usbd_cdc_vs *p_vs);


/**
 * \brief 虚拟串口设备异步写
 *
 */
aw_err_t aw_usbd_cdc_vs_async_write (struct aw_usbd_cdc_vs       *p_vs,
                                     struct aw_usbd_trans        *p_trans);
/**
 * \brief 虚拟串口设备异步读
 *
 */
aw_err_t aw_usbd_cdc_vs_async_read (struct aw_usbd_cdc_vs  *p_vs,
                                    void                   *p_buf,
                                    size_t                  nbytes,
                                    size_t                 *p_act_len);
/**
 * \brief 虚拟串口设备同步写
 *
 */
aw_err_t aw_usbd_cdc_vs_poll_write (struct aw_usbd_cdc_vs *p_vs,
                                    void                  *p_buffer,
                                    size_t                 nbytes,
                                    int                    timeout);
/**
 * \brief 虚拟串口设备同步读
 *
 */
aw_err_t aw_usbd_cdc_vs_poll_read (struct aw_usbd_cdc_vs  *p_vs,
                                  void                   *p_buf,
                                  size_t                  nbytes,
                                  int                    timeout);


aw_err_t aw_usbd_cdc_rx_callback_install(int id, int (*pfn_callback)(void*, int, void*), void *p_arg);
aw_err_t aw_usbd_cdc_tx_char_put(int id, char *p_char);
aw_err_t aw_usbd_cdc_vs_set_rx_mode(struct aw_usbd_cdc_vs *p_vs, uint8_t mode);
aw_err_t aw_usbd_cdc_vs_set_tx_mode(struct aw_usbd_cdc_vs *p_vs, uint8_t mode);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_USBD_CDC_H */

/* end of file */
