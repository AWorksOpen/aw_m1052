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
    ������Ϣ
*******************************************************************************/
aw_local char __g_msg_buffer[1024];

aw_local struct awbl_ublox_devinfo __g_gnss_ublox_devinfo = {
    {
        IMX1050_LPUART3_COMID,      /* Ҫʹ�õĴ��� */
        9600,                       /* ���ڲ����� */
        100,                        /* ���ڳ�ʱ(ms) */

        __g_msg_buffer,
        sizeof(__g_msg_buffer),
    },
    {
        0                           /* GNSS �豸���  */
    },
	GPIO3_0,                         /* ��λ���� */
	GPIO3_1                          /* ʹ������ */
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
