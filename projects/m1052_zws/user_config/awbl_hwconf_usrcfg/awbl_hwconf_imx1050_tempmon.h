/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

#ifndef __AWBL_HWCONF_IMX1050_TEMPMON_H
#define __AWBL_HWCONF_IMX1050_TEMPMON_H

#ifdef AW_DEV_IMX1050_TEMPMON

#include "driver/tempmon/awbl_imx10xx_tempmon.h"

aw_local void __imx1050_tempmon_plfm_init (void)
{
    /* enable clock */
    aw_clk_enable(IMX1050_CLK_CG_OCOTP);
    aw_clk_enable(IMX1050_CLK_PLL_USB1);

};

/* 设备信息 */
aw_local aw_const struct awbl_imx10xx_tempmon_devinfo __g_imx1050_tempmon_devinfo = {
    IMX1050_TEMPMON_BASE,           /* base addr */
    INUM_TEMPMON,                   /* inum */
    0,                              /* low limit : 0 ℃ */
    70000,                          /* high limit: 70 ℃  */
    100000,                         /* reset limit: 100 ℃ */
    5,                              /* task prio */
    __imx1050_tempmon_plfm_init
};

/* 设备实例内存静态分配 */
aw_local struct awbl_imx10xx_tempmon_dev __g_imx1050_tempmon_dev;

#define AWBL_HWCONF_IMX1050_TEMPMON                  \
    {                                                \
        AWBL_IMX10XX_TEMPMON_NAME,                   \
        0,                                           \
        AWBL_BUSID_PLB,                              \
        0,                                           \
        (struct awbl_dev *)&__g_imx1050_tempmon_dev, \
        &(__g_imx1050_tempmon_devinfo)               \
},

#else
#define AWBL_HWCONF_IMX1050_TEMPMON

#endif  /* AW_DEV_IMX1050_TEMPMON */

#endif /* __AWBL_HWCONF_IMX1050_TEMPMON_H */

/* end of file */
