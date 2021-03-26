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

#ifndef __AWBL_HWCONF_PCF85063_H
#define __AWBL_HWCONF_PCF8563_H


#ifdef AW_DEV_EXTEND_PCF85063

#include "driver/rtc/awbl_pcf85063.h"

/**
 * \addtogroup grp_aw_plfm_ext_pcf85063_usrcfg
 * @{
 */

/* PCF8563 �豸��Ϣ */
aw_local aw_const struct awbl_pcf85063_devinfo __g_pcf85063_devinfo = {
    {
        RTC_ID0     /* RTC �豸��� */
    },
    0x51      /* I2C �ӻ���ַ */
};

/* PCF8563 �豸ʵ���ڴ澲̬���� */
aw_local struct awbl_pcf85063_dev __g_pcf85063_dev;

#define AWBL_HWCONF_PCF85063                      \
    {                                             \
        AWBL_PCF85063_NAME,                       \
        0,                                        \
        AWBL_BUSID_I2C,                           \
        IMX1050_LPI2C1_BUSID,                     \
        (struct awbl_dev *)&__g_pcf85063_dev,     \
        &__g_pcf85063_devinfo                     \
    },

#else
#define AWBL_HWCONF_PCF85063

#endif  /* AW_DEV_EXTEND_PCF85063 */

#endif  /* __AWBL_HWCONF_PCF85063_H */

/* end of file */
