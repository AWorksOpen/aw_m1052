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


#ifndef __AWBL_HWCONF_IMX1050_RTC_H
#define __AWBL_HWCONF_IMX1050_RTC_H

#include "driver/rtc/awbl_imx10xx_rtc.h"

#ifdef AW_DEV_IMX1050_RTC

/* RTC Platform Initialize */
aw_local void __imx1050_rtc_plfm_init (void)
{
    aw_clk_enable(IMX1050_CLK_CG_SNVS_HP);
    aw_clk_enable(IMX1050_CLK_CG_SNVS_LP);
}


/* The RTC device information */
aw_local aw_const struct awbl_imx10xx_rtc_devinfo  __g_imx1050_rtc_devinfo = {
    {
        RTC_ID1,               /**< \brief RTC ID */
    },
    IMX1050_SNVS_BASE,         /**< \brief RTC */
    __imx1050_rtc_plfm_init    /**< \brief RTC plfm deinit */
};

/* 设备实例内存静态分配 */
aw_local struct awbl_imx10xx_rtc_dev __g_imx1050_rtc_dev;

#define AWBL_HWCONF_IMX1050_RTC   \
{                                 \
    AWBL_IMX10XX_RTC_NAME,        \
    0,                            \
    AWBL_BUSID_PLB,               \
    0,                            \
    (struct awbl_dev *)&(__g_imx1050_rtc_dev), \
    &(__g_imx1050_rtc_devinfo)    \
},
#else   /* AW_DEV_IMX1050_RTC */

#define AWBL_HWCONF_IMX1050_RTC

#endif

#endif /* __AWBL_HWCONF_IMX1050_RTC_H */
