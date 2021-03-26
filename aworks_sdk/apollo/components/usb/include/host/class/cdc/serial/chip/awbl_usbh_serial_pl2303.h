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
#ifndef __AWBL_USBH_SERIAL_PL2303_H
#define __AWBL_USBH_SERIAL_PL2303_H
#include "aw_usb_os.h"
#include "aw_list.h"
#include "host/awbl_usbh.h"
#include "aw_serial.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define VENDOR_WRITE_REQUEST_TYPE   0x40
#define VENDOR_WRITE_REQUEST        0x01
#define VENDOR_READ_REQUEST_TYPE    0xc0
#define VENDOR_READ_REQUEST         0x01

#define SET_CONTROL_REQUEST_TYPE    0x21
#define SET_CONTROL_REQUEST         0x22
#define SET_LINE_REQUEST_TYPE       0x21
#define SET_LINE_REQUEST            0x20
#define GET_LINE_REQUEST_TYPE       0xa1
#define GET_LINE_REQUEST            0x21

#define CONTROL_DTR         0x01
#define CONTROL_RTS         0x02

/* pl2303芯片类型*/
enum pl2303_type {
    TYPE_01,    /* 类型 0 和 1 (区别未知) */
    TYPE_HX,    /* pl2303芯片的 HX 版本*/
    TYPE_COUNT
};

/* pl2303类型数据*/
struct pl2303_type_data {
    uint32_t max_baud_rate;
    unsigned long quirks;
};

/* pl2303串口私有数据*/
struct pl2303_serial_private {
    const struct pl2303_type_data *type;
    unsigned long quirks;
};

/* PL2303 串口结构体*/
struct awbl_usbh_serial_pl2303{
    struct awbl_usbh_function *p_fun;    /* 相应的功能设备结构体*/
    aw_spinlock_isr_t  lock;             /* 自旋锁*/
    struct pl2303_serial_private *spriv; /* pl2303串口私有数据*/
    uint8_t line_settings[7];
    uint8_t line_control;
};



aw_err_t awbl_usbh_serial_pl2303_init(struct awbl_usbh_serial *usb_serial);

#endif
