/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

#ifndef  __AWBL_HWCONF_IMX1050_WDT1_H
#define  __AWBL_HWCONF_IMX1050_WDT1_H

#ifdef  AW_DEV_IMX1050_WDT1

#include "driver/wdt/awbl_imx10xx_wdt.h"

aw_local void __imx1050_wdt1_plfm_init (void)
{
    /* enable clock */
    aw_clk_enable(IMX1050_CLK_CG_WDOG1);
};

/* watchdog feed device infomation */
aw_local aw_const struct awbl_imx10xx_wdt_devinfo __g_imx1050_wdt1_devinfo = {
    IMX1050_WDOG1_BASE,         /* reg base addr */
    1000,                       /* watchdog time (ms)*/
    __imx1050_wdt1_plfm_init
};

/* allocate wdt1 device instance RAM */
aw_local struct awbl_imx10xx_wdt_dev __g_imx1050_wdt1_dev;

#define  AWBL_HWCONF_IMX1050_WDT1    \
    {                                \
        AWBL_IMX10XX_WDT_NAME,       \
        1,                           \
        AWBL_BUSID_PLB,              \
        0,                           \
        &(__g_imx1050_wdt1_dev.dev), \
        &(__g_imx1050_wdt1_devinfo)  \
    },

#else
#define AWBL_HWCONF_IMX1050_WDT1

#endif  /* AW_DEV_IMX1050_WDT1 */

#endif  /* __AWBL_HWCONF_IMX1050_WDT1_H */

/* end of file */
