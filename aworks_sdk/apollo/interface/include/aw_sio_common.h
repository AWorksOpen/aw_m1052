/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief 串行设备通用I/O控制命令
 *
 * \internal
 * \par modification history
 * - 1.00 12-10-26  orz, created
 * \endinternal
 */

#ifndef __AW_SIO_COMMON_H
#define __AW_SIO_COMMON_H

/**
 * \addtogroup grp_aw_if_serial
 * @{
 */

/**
 * \defgroup aw_sio_comm_ioctl 串行设备通用I/O控制命令
 * \copydoc aw_sio_common.h
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_ioctl_generic.h"

/**
 * \name serial device I/O controls base
 * @{
 */
#define _SIO(nr)        __AW_IO('s', nr)
#define _SIOR(nr, size) __AW_IOR('s', nr, size)
#define _SIOW(nr, size) __AW_IOW('s', nr, size)
/** @} */

/**
 * \name serial device most common ioctl commands
 * @{
 */
#define SIO_BAUD_SET            _SIOW(3, int)
#define SIO_BAUD_GET            _SIOR(4, int)

#define SIO_HW_OPTS_SET         _SIOW(5, int)
#define SIO_HW_OPTS_GET         _SIOR(6, int)

#define SIO_MODE_SET            _SIOW(7, int)
#define SIO_MODE_GET            _SIOR(8, int)
#define SIO_AVAIL_MODES_GET     _SIOR(9, int)
/** @} */

/**
 * \name ioctl commands used to communicate open/close between layers
 * @{
 */
#define SIO_OPEN                _SIOW(10, int)
#define SIO_HUP                 _SIOW(11, int)
/** @} */

/**
 * \name The ioctl commands for reading and setting the modem lines.
 * @{
 */
#define SIO_MSTAT_GET       _SIOR(12, int)  /* get modem status lines */
#define SIO_MCTRL_BITS_SET  _SIOW(13, int)  /* set selected modem lines */
#define SIO_MCTRL_BITS_CLR  _SIOW(14, int)  /* clear selected modem lines  */
#define SIO_MCTRL_ISIG_MASK _SIOW(15, int)  /* mask of lines that can be read */
#define SIO_MCTRL_OSIG_MASK _SIOW(16, int)  /* mask of lines that can be set */
/** @} */

/**
 * \name ioctl cmds for reading/setting keyboard mode
 * @{
 */
#define SIO_KYBD_MODE_SET   _SIOW(17, int)
#define SIO_KYBD_MODE_GET   _SIOR(18, int)
/** @} */

/**
 * \name ioctl cmds for reading/setting keyboard led state
 * @{
 */
#define SIO_KYBD_LED_SET    _SIOW(19, int)
#define SIO_KYBD_LED_GET    _SIOR(20, int)
/** @} */

/**
 * \name ioctl cmds for taking/giving local ISR spinlock.
 * The cmds are available only in SMP mode.
 * @{
 */
#define SIO_DEV_LOCK        _SIO(21)
#define SIO_DEV_UNLOCK      _SIO(22)
/** @} */

/**
 * \name options to SIO_MODE_SET
 * @{
 */
#define SIO_MODE_POLL   1
#define SIO_MODE_INT    2
/** @} */

/**
 * \name options to SIO_HW_OPTS_SET (ala POSIX), bitwise or'ed together
 * @{
 */
#define CLOCAL          0x1     /* ignore modem status lines */
#define CREAD           0x2     /* enable device reciever */

#define CSIZE           0xc     /* bits 3 and 4 encode the character size */
#define CS5             0x0     /* 5 bits */
#define CS6             0x4     /* 6 bits */
#define CS7             0x8     /* 7 bits */
#define CS8             0xc     /* 8 bits */

#define HUPCL           0x10    /* hang up on last close */
#define STOPB           0x20    /* send two stop bits (else one) */
#define PARENB          0x40    /* parity detection enabled (else disabled) */
#define PARODD          0x80    /* odd parity  (else even) */

/** 
 * \brief 为了兼容aw_serial_dcb_set 中可能设置1.5位的停止位,通过aw_serial_ioctrl
 *        获取option不能表征1.5位停止位。
 *        如果STOPONE5为真，则是1.5位停止位,否则就是1位或者2位, STOPONE5 
 *        优先判断
 */
#define STOPONE5        0x100
/** @} */

/**
 * \name Modem signals definitions
 * @{
 * The following six macros define the different modem signals. They
 * are used as arguments to the modem ioctls commands to specify
 * the signals to read(set) and also to parse the returned value. They
 * provide hardware independence, as modem signals bits vary from one
 * chip to another.
 */
#define SIO_MODEM_DTR   0x01    /* data terminal ready */
#define SIO_MODEM_RTS   0x02    /* request to send */
#define SIO_MODEM_CTS   0x04    /* clear to send */
#define SIO_MODEM_CD    0x08    /* carrier detect */
#define SIO_MODEM_RI    0x10    /* ring */
#define SIO_MODEM_DSR   0x20    /* data set ready */
/** @} */

/**
 * \name options to SIO_KYBD_MODE_SET
 * @{
 * These macros are used as arguments by the keyboard ioctl comands.
 * e.g. with SIO_KYBD_MODE_SET
 */

/* Requests the keyboard driver to return raw key values as read
 * by the keyboard controller */
#define SIO_KYBD_MODE_RAW     1

/* Requests the keyboard driver to return ASCII codes read from a
 * known table that maps raw key values to ASCII */
#define SIO_KYBD_MODE_ASCII   2

/* Requests the keyboard driver to return 16 bit UNICODE values for
 * multiple languages support */
#define SIO_KYBD_MODE_UNICODE 3
/** @} */

/**
 * \name options to SIO_KYBD_LED_SET
 * @{
 * These macros are used as arguments by the keyboard ioctl comands.
 * e.g. SIO_KYBD_LED_SET
 */
#define SIO_KYBD_LED_NUM      1 /* Sets the Num Lock LED on the keyboard */
#define SIO_KYBD_LED_CAP      2 /* Sets the Caps Lock LED on the keyboard */
#define SIO_KYBD_LED_SCR      4 /* Sets the Scroll Lock LED on the keyboard */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} aw_sio_comm_ioctl */

/** @} grp_aw_if_serial */

#endif /* __AW_SIO_COMMON_H */

/* end of file */
