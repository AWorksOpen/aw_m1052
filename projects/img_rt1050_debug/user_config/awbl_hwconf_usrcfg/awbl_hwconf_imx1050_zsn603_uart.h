/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

#ifndef __AWBL_HWCONF_ZSN603_UART_H__
#define __AWBL_HWCONF_ZSN603_UART_H__

#include "imx1050_reg_base.h"
#include "imx1050_pin.h"
#include "imx1050_inum.h"
#include "driver/gpio/awbl_imx1050_gpio.h"
#include "driver/rfid/awbl_zsn603_uart.h"
#include "driver/rfid/zsn603.h"
#include "aw_prj_params.h"
#include "aw_serial.h"

#ifdef    AW_DEV_UART_ZSN603

/* ZSN603 (UART mode) 设备信息 */
aw_local aw_const awbl_zsn603_uart_devinfo_t __g_zsn603_uart_devinfo = {
    0xb2,
    9600,
    IMX1050_LPUART3_COMID,
    GPIO1_8
};

/* ZSN603 (UART mode) 设备实例内存静态分配 */
aw_local awbl_zsn603_uart_dev_t __g_zsn603_uart_dev;

#define AWBL_HWCONF_ZSN603_UART           \
    {                                \
        AWBL_ZSN603_UART_NAME,            \
        0,                           \
        AWBL_BUSID_PLB,              \
        0,                           \
        &__g_zsn603_uart_dev.super,  \
        &__g_zsn603_uart_devinfo     \
    },
#else
#define AWBL_HWCONF_ZSN603_UART
#endif /* __AWBL_HWCONF_ZSN603_UART_H__ */

#endif /* __AWBL_HWCONF_ZSN603_UART_H__ */

/* end of file */
