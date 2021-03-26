/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

#ifndef __AWBL_HWCONF_UBLOX_H
#define __AWBL_HWCONF_UBLOX_H

#ifdef AW_DEV_GNSS_UBLOX
#include "driver/gps/awbl_ublox.h"
#include "aw_serial.h"


/*******************************************************************************
    配置信息
*******************************************************************************/
aw_local char __g_msg_buffer[1024];

aw_local struct awbl_ublox_devinfo __g_gnss_ublox_devinfo = {
    {
        IMX1050_LPUART3_COMID,      /* 要使用的串口 */
        9600,                       /* 串口波特率 */
        100,                        /* 串口超时(ms) */

        __g_msg_buffer,
        sizeof(__g_msg_buffer),
    },
    {
        0                           /* GNSS 设备编号  */
    },
	GPIO3_0,                         /* 复位引脚 */
	GPIO3_1                          /* 使能引脚 */
};

aw_local struct awbl_ublox_dev __g_gnss_ublox_dev;

#define AWBL_HWCONF_UBLOX                                \
    {                                                     \
		AWBL_UBLOX_GNSS,                                 \
        0,                                                \
        AWBL_BUSID_PLB,                                   \
        0,                                                \
        (struct awbl_dev *)&__g_gnss_ublox_dev.super,    \
        &__g_gnss_ublox_devinfo                          \
    },

#else
#define AWBL_HWCONF_UBLOX

#endif    /* AWBL_HWCONF_UBLOX */

#endif  /* __AWBL_HWCONF_UBLOX */

/* end of file */
