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

#ifndef  __AWBL_HWCONF_IMX1050_RTWDT_H
#define  __AWBL_HWCONF_IMX1050_RTWDT_H

#ifdef  AW_DEV_IMX1050_RTWDT

#include "driver/rtwdt/awbl_imx1050_rtwdt.h"

aw_local void __imx1050_rtwdt_plfm_init (void)
{
    /* enable clock */
    aw_clk_enable(IMX1050_CLK_CG_WDOG3);
};

/* watchdog feed device infomation */
aw_local aw_const struct awbl_imx1050_rtwdt_devinfo __g_imx1050_rtwdt_devinfo = {
    IMX1050_RTWDOG_BASE,        /* reg base addr */
    1000,                       /* watchdog time (ms)*/
    __imx1050_rtwdt_plfm_init
};

/* allocate rtwdt device instance RAM */
aw_local struct awbl_imx1050_rtwdt_dev __g_imx1050_rtwdt_dev;

#define  AWBL_HWCONF_IMX1050_RTWDT    \
    {                                 \
        IMX1050_RTWDT_NAME,           \
        3,                            \
        AWBL_BUSID_PLB,               \
        0,                            \
        &(__g_imx1050_rtwdt_dev.dev), \
        &(__g_imx1050_rtwdt_devinfo)  \
    },

#else
#define AWBL_HWCONF_IMX1050_RTWDT

#endif  /* AW_DEV_IMX1050_RTWDT */

#endif  /* __AWBL_HWCONF_IMX1050_RTWDT_H */

/* end of file */
