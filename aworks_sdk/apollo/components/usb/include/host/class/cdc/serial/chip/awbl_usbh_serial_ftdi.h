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
#ifndef __AWBL_USBH_SERIAL_FTDI_H
#define __AWBL_USBH_SERIAL_FTDI_H
#include "aw_usb_os.h"
#include "aw_list.h"
#include "host/awbl_usbh.h"
#include "aw_serial.h"

#define INTERFACE_A     1
#define INTERFACE_B     2
#define INTERFACE_C     3
#define INTERFACE_D     4

/* FTDIоƬ����*/
enum ftdi_chip_type {
    SIO = 1,
    FT8U232AM = 2,
    FT232BM = 3,
    FT2232C = 4,
    FT232RL = 5,
    FT2232H = 6,
    FT4232H = 7,
    FT232H  = 8,
    FTX     = 9,
};

/* ������*/
enum ftdi_sio_baudrate {
    ftdi_sio_b300 =    0,
    ftdi_sio_b600 =    1,
    ftdi_sio_b1200 =   2,
    ftdi_sio_b2400 =   3,
    ftdi_sio_b4800 =   4,
    ftdi_sio_b9600 =   5,
    ftdi_sio_b19200 =  6,
    ftdi_sio_b38400 =  7,
    ftdi_sio_b57600 =  8,
    ftdi_sio_b115200 = 9
};

#define FTDI_SIO_RESET_SIO 0

#define FTDI_SIO_RESET                0 /* ��λ�˿�*/
#define FTDI_SIO_MODEM_CTRL           1 /* Set the modem control register */
#define FTDI_SIO_SET_FLOW_CTRL        2 /* ���������ƼĴ���*/
#define FTDI_SIO_SET_BAUDRATE_REQUEST 3 /* ���ò�����*/
#define FTDI_SIO_SET_DATA             4 /* ���ö˿���������*/


#define FTDI_SIO_RESET_REQUEST_TYPE          0x40  /* ��λUSB��������*/
#define FTDI_SIO_SET_DATA_REQUEST_TYPE       0x40  /* ��������USB��������*/
#define FTDI_SIO_SET_FLOW_CTRL_REQUEST_TYPE  0x40  /* ����������USB��������*/
#define FTDI_SIO_SET_BAUDRATE_REQUEST_TYPE   0x40  /* ���ò�����USB��������*/
#define FTDI_SIO_SET_MODEM_CTRL_REQUEST_TYPE 0x40  /* */

#define FTDI_NDI_HUC_PID            0xDA70  /* NDI Host USB Converter */
#define FTDI_NDI_SPECTRA_SCU_PID    0xDA71  /* NDI Spectra SCU */
#define FTDI_NDI_FUTURE_2_PID       0xDA72  /* NDI future device #2 */
#define FTDI_NDI_FUTURE_3_PID       0xDA73  /* NDI future device #3 */
#define FTDI_NDI_AURORA_SCU_PID     0xDA74  /* NDI Aurora SCU */

/* ֹͣλ����*/
#define FTDI_SIO_SET_DATA_STOP_BITS_1   (0x0 << 11)
#define FTDI_SIO_SET_DATA_STOP_BITS_15  (0x1 << 11)
#define FTDI_SIO_SET_DATA_STOP_BITS_2   (0x2 << 11)
/* У������*/
#define FTDI_SIO_SET_DATA_PARITY_NONE   (0x0 << 8)
#define FTDI_SIO_SET_DATA_PARITY_ODD    (0x1 << 8)
#define FTDI_SIO_SET_DATA_PARITY_EVEN   (0x2 << 8)
#define FTDI_SIO_SET_DATA_PARITY_MARK   (0x3 << 8)
#define FTDI_SIO_SET_DATA_PARITY_SPACE  (0x4 << 8)
/* ������*/
#define FTDI_SIO_RTS_CTS_HS (0x1 << 8)
#define FTDI_SIO_DTR_DSR_HS (0x2 << 8)
#define FTDI_SIO_XON_XOFF_HS (0x4 << 8)

#define FTDI_SIO_SET_DTR_MASK 0x1
#define FTDI_SIO_SET_DTR_HIGH (1 | (FTDI_SIO_SET_DTR_MASK  << 8))
#define FTDI_SIO_SET_DTR_LOW  (0 | (FTDI_SIO_SET_DTR_MASK  << 8))
#define FTDI_SIO_SET_RTS_MASK 0x2
#define FTDI_SIO_SET_RTS_HIGH (2 | (FTDI_SIO_SET_RTS_MASK << 8))
#define FTDI_SIO_SET_RTS_LOW  (0 | (FTDI_SIO_SET_RTS_MASK << 8))

/* FTDI���ڽṹ��*/
struct awbl_usbh_serial_ftdi{
    struct awbl_usbh_function *p_fun; /* ��Ӧ�Ĺ����豸�ṹ��*/
    enum ftdi_chip_type chip_type;    /* оƬ�ͺ�*/
    int baud_base;                    /* �����ʻ���*/
    uint16_t interface;               /* �ӿڱ��*/
};

aw_err_t awbl_usbh_serial_ftdi_init(struct awbl_usbh_serial *usb_serial);

#endif



