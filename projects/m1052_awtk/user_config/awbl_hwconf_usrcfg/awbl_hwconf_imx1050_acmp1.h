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

#ifndef __AWBL_HWCONF_IMX1050_ACMP1_H
#define __AWBL_HWCONF_IMX1050_ACMP1_H

#ifdef AW_DEV_IMX1050_ACMP1

#include "driver/acmp/awbl_imx10xx_acmp.h"

#define __ACMP_PAD_CTRL    (DSE_6_R0_6                | \
                            SPEED_2_medium_100MHz     | \
                            PKE_1_Pull_Keeper_Enabled)

aw_local void __imx1050_acmp1_plfm_init (void)
{
    int  i = 0;
    aw_local aw_const int acmp1_gpios[] = {
//            GPIO1_17,    /* ACMP1_IN0 */
              GPIO1_22,    /* ACMP1_IN1 */
//            GPIO1_13,    /* ACMP1_IN2 */
//            GPIO1_18,    /* ACMP1_IN3 */
//            GPIO1_0,     /* ACMP1_IN4 */
//            GPIO1_23,    /* ACMP1_IN5 */
//            GPIO1_27,    /* ACMP1_IN6 */
    };

    if (aw_gpio_pin_request("acmp1_gpios",
                             acmp1_gpios,
                             AW_NELEMENTS(acmp1_gpios)) == AW_OK) {

        for (i = 0; i < AW_NELEMENTS(acmp1_gpios); i++) {
              aw_gpio_pin_cfg(acmp1_gpios[i], IMX1050_PAD_CTL(__ACMP_PAD_CTRL));
        }
    }

    aw_clk_enable(IMX1050_CLK_CG_ACMP1);
};


/* 设备信息 */
aw_local aw_const struct awbl_imx10xx_acmp_devinfo __g_imx1050_acmp1_devinfo = {
    IMX1050_ACMP1_BASE,              /**< \brief 寄存器基地址 */
    INUM_ACMP1,                      /**< \brief 中断号 */
    IMX1050_ACMP1,                   /**< \brief ACMP的ID号 */
    __imx1050_acmp1_plfm_init        /**< \brief 平台初始化 */
};

/* 设备实例内存静态分配 */
aw_local struct awbl_imx10xx_acmp_dev __g_imx1050_acmp1_dev;

#define AWBL_HWCONF_IMX1050_ACMP1                 \
{                                                 \
    AWBL_IMX10XX_ACMP_NAME,                       \
    0,                                            \
    AWBL_BUSID_PLB,                               \
    0,                                            \
    (struct awbl_dev *)&__g_imx1050_acmp1_dev,    \
    &(__g_imx1050_acmp1_devinfo)                  \
},

#else
#define AWBL_HWCONF_IMX1050_ACMP1
#endif  /* AW_DEV_IMX1050_ACMP1 */

#endif /* __AWBL_HWCONF_IMX1050_ACMP1_H */

/* end of file */
