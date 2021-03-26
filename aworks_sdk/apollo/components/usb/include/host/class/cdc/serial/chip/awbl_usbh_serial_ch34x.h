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
/* �����ʼ�������*/
/*******************************/
#define CH34x_BAUDBASE_FACTOR 1532620800
#define CH34x_BAUDBASE_DIVMAX 3

#define CH34x_BIT_RTS (1 << 6)
#define CH34x_BIT_DTR (1 << 5)

#define CH34x_BITS_MODEM_STAT 0x0f /* ����λ*/

/* CH34x���ڽṹ��*/
struct awbl_usbh_serial_ch34x{
    struct awbl_usbh_function *p_fun; /* ��Ӧ�Ĺ����豸�ṹ��*/
    aw_spinlock_isr_t  lock;          /* ������*/
    uint8_t line_control;             /* ���� line control value RTS/DTR */
    uint8_t line_status;              /* modem ��������ļ����λ*/
};





aw_err_t awbl_usbh_serial_ch34x_init(struct awbl_usbh_serial *usb_serial);

#endif
