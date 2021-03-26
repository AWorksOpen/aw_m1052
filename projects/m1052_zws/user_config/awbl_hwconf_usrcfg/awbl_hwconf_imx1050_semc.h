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

#ifndef __AWBL_HWCONF_IMX1050_SEMC_H
#define __AWBL_HWCONF_IMX1050_SEMC_H

#ifdef AW_DEV_IMX1050_SEMC

#include "driver/busctlr/awbl_imx1050_semc.h"
#include "awbl_nand_bus.h"
#include "imx1050_pin.h"

#define   __IMX1050_SEMC_PAD_CTL   IMX1050_PAD_CTL(PUS_1_47K_Ohm_Pull_Up     | \
                                                   PKE_1_Pull_Keeper_Enabled | \
                                                   PUE_1_Pull                | \
                                                   Open_Drain_Disabled       | \
                                                   SPEED_3_max_200MHz        | \
                                                   DSE_1_R0                  | \
                                                   SRE_0_Fast_Slew_Rate)

#define   __IMX1050_SEMC_nWP       IMX1050_PAD_CTL(PUS_1_47K_Ohm_Pull_Up     | \
                                                   PKE_1_Pull_Keeper_Enabled | \
                                                   PUE_1_Pull                | \
                                                   Open_Drain_Disabled       | \
                                                   SPEED_2_medium_100MHz        | \
                                                   DSE_1_R0                  | \
                                                   SRE_0_Fast_Slew_Rate)

/** \brief NAND 引脚信息 */
aw_local aw_const int __semc_gpios[] = {
    GPIO3_26,
    GPIO3_27,
    GPIO5_1,
};

aw_local void __g_imx1050_semc_plat_init (void)
{

    if (aw_gpio_pin_request("semc_gpios",
                             __semc_gpios,
                             AW_NELEMENTS(__semc_gpios)) == AW_OK) {

        /* NAND nCE */
        aw_gpio_pin_cfg(GPIO3_27, GPIO3_27_SEMC_CSX00 | __IMX1050_SEMC_PAD_CTL);

        /* NAND_READY_B  */
        aw_gpio_pin_cfg(GPIO3_26, GPIO3_26_SEMC_RDY | __IMX1050_SEMC_PAD_CTL);

        /* NAND nWP*/
        aw_gpio_pin_cfg(GPIO5_1, GPIO5_1_GPIO |
                                 AW_GPIO_OUTPUT_INIT_HIGH|
                                 __IMX1050_SEMC_nWP);

        /* enable the clock */
        aw_clk_enable(IMX1050_CLK_CG_SEMC);
    }
}

aw_local aw_const struct awbl_imx1050_semc_devinfo __g_imx1050_semc_devinfo = {
    IMX1050_NAND0_BUSID,             /* NAND BUS ID号 */
    IMX1050_SEMC_BASE,               /* SEMC 基地址 */
    IMX1050_CLK_CG_SEMC,             /* IO时钟ID        */
    INUM_SEMC,                       /* 中断       */
    __g_imx1050_semc_plat_init
};

aw_local struct awbl_imx1050_semc __g_imx1050_semc_dev = {
        0
};

#define AWBL_HWCONF_IMX1050_SEMC                                 \
    {                                                           \
        AWBL_IMX1050_SEMC_NAME,                                  \
        0,                                                      \
        AWBL_BUSID_PLB,                                         \
        0,                                                      \
        &(__g_imx1050_semc_dev.nandbus.awbus.super),    \
        &(__g_imx1050_semc_devinfo)                              \
    },

#else

#define AWBL_HWCONF_IMX1050_SEMC

#endif  /* AW_DEV_IMX1050_SEMC */

#endif /* __AWBL_HWCONF_IMX1050_SEMC_H */

/* end of file */
