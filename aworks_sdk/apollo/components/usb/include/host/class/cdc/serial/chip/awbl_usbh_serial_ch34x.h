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
#ifndef __AWBL_USBH_SERIAL_CH34x_H
#define __AWBL_USBH_SERIAL_CH34x_H
#include "aw_usb_os.h"
#include "aw_list.h"
#include "host/awbl_usbh.h"
#include "aw_serial.h"


/*******************************/
/* 波特率计算因子*/
/*******************************/
#define CH34x_BAUDBASE_FACTOR 1532620800
#define CH34x_BAUDBASE_DIVMAX 3

#define CH34x_BIT_RTS (1 << 6)
#define CH34x_BIT_DTR (1 << 5)

#define CH34x_BITS_MODEM_STAT 0x0f /* 所有位*/

/* CH34x串口结构体*/
struct awbl_usbh_serial_ch34x{
    struct awbl_usbh_function *p_fun; /* 相应的功能设备结构体*/
    aw_spinlock_isr_t  lock;          /* 自旋锁*/
    uint8_t line_control;             /* 设置 line control value RTS/DTR */
    uint8_t line_status;              /* modem 控制输入的激活的位*/
};





aw_err_t awbl_usbh_serial_ch34x_init(struct awbl_usbh_serial *usb_serial);

#endif
