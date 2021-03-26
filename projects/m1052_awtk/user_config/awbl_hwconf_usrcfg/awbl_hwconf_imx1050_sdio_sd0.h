/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

#ifndef AWBL_HWCONF_IMX1050_SDIO_SSP0_H_
#define AWBL_HWCONF_IMX1050_SDIO_SSP0_H_

#include "aw_gpio.h"
#include "aw_clk.h"
#include "awbl_sdiobus.h"
#include "aw_delay.h"
#include "imx1050_pin.h"
#include "driver/sdio/awbl_imx10xx_sdio.h"
#include "driver/sdiocard/awbl_sdcard.h"

#ifdef AW_DEV_IMX1050_USDHC1

#define USDHC1_PAD_CTRL    (SRE_0_Fast_Slew_Rate | \
                            DSE_6_R0_6 |           \
                            SPEED_3_max_200MHz|    \
                            PKE_1_Pull_Keeper_Enabled |\
                            PUE_1_Pull |           \
                            PUS_1_47K_Ohm_Pull_Up| \
                            HYS_1_Hysteresis_Enabled)

aw_local void __imx1050_sdio_ssp0_plfm_init (void);

/* 检测卡插拔状态 */
aw_local aw_bool_t __imx1050_sdio_ssp0_detect (awbl_imx10xx_sdio_dev_t *p_dev)
{
    return (aw_gpio_get(GPIO2_28) ? AW_FALSE : AW_TRUE);
}

/* 设备信息 */
aw_local aw_const awbl_imx10xx_sdio_dev_info_t __g_imx1050_sdio_ssp0_devinfo = {
    {
        0,
        AWBL_SDIO_SD_4B_M,
        AWBL_SDIO_CON_DEV_DYNAMIC,
        AW_TRUE,
        AW_FALSE,
        (AWBL_SD_OCR_VDD_27_28 | AWBL_SD_OCR_VDD_28_29 | AWBL_SD_OCR_VDD_29_30 |
         AWBL_SD_OCR_VDD_30_31 | AWBL_SD_OCR_VDD_31_32 | AWBL_SD_OCR_VDD_32_33 |
         AWBL_SD_OCR_VDD_33_34 | AWBL_SD_OCR_VDD_34_35 | AWBL_SD_OCR_VDD_35_36),
    },

    IMX1050_USDHC1_BASE,                /* 寄存器基地址 */
    IMX1050_CLK_CG_USDHC1,                /* 输入时钟ID  */
    10,                                  /* 传输任务优先级 */
    __imx1050_sdio_ssp0_plfm_init,      /* 平台相关初始化 */
    INUM_USDHC1,                        /* 中断号 */
    __imx1050_sdio_ssp0_detect
};


/* 平台相关初始化 */
aw_local void __imx1050_sdio_ssp0_plfm_init (void)
{
    /* 配置功能引脚 */

    int sdio_gpios[] = {
        GPIO2_28,    /* 检测插拔 */
        GPIO3_12,    /* GPIO_SD_B0_00(USDHC1_CMD) */
        GPIO3_13,    /* GPIO_SD_B0_01(USDHC1_CLK) */
        GPIO3_14,    /* GPIO_SD_B0_02(USDHC1_DATA0) */
        GPIO3_15,    /* GPIO_SD_B0_03(USDHC1_DATA1) */
        GPIO3_16,    /* GPIO_SD_B0_04(USDHC1_DATA2) */
        GPIO3_17,    /* GPIO_SD_B0_05(USDHC1_DATA3) */
    };

    if (aw_gpio_pin_request("sdio_gpios",
                             sdio_gpios,
                             AW_NELEMENTS(sdio_gpios)) == AW_OK) {

        /* 配置功能引脚 */
        aw_gpio_pin_cfg(GPIO2_28, AW_GPIO_INPUT | AW_GPIO_PULL_UP);
        aw_gpio_pin_cfg(GPIO3_12, GPIO3_12_USDHC1_CMD  | IMX1050_PAD_CTL(USDHC1_PAD_CTRL) );
        aw_gpio_pin_cfg(GPIO3_13, GPIO3_13_USDHC1_CLK  | IMX1050_PAD_CTL(USDHC1_PAD_CTRL) );
        aw_gpio_pin_cfg(GPIO3_14, GPIO3_14_USDHC1_DATA0| IMX1050_PAD_CTL(USDHC1_PAD_CTRL) );
        aw_gpio_pin_cfg(GPIO3_15, GPIO3_15_USDHC1_DATA1| IMX1050_PAD_CTL(USDHC1_PAD_CTRL) );
        aw_gpio_pin_cfg(GPIO3_16, GPIO3_16_USDHC1_DATA2| IMX1050_PAD_CTL(USDHC1_PAD_CTRL) );
        aw_gpio_pin_cfg(GPIO3_17, GPIO3_17_USDHC1_DATA3| IMX1050_PAD_CTL(USDHC1_PAD_CTRL) );
    }

    aw_clk_parent_set(IMX1050_CLK_USDHC1_CLK_SEL, IMX1050_CLK_PLL2_PFD2);
    aw_clk_enable(IMX1050_CLK_CG_USDHC1);
};

/* 设备实例内存静态分配 */
awbl_imx10xx_sdio_dev_t __g_imx1050_sdio_ssp0_dev;

#define AWBL_HWCONF_IMX1050_SDIO_SSP0                    \
    {                                                    \
        AWBL_IMX10XX_SDIO_NAME,                          \
        0,                                               \
        AWBL_BUSID_PLB,                                  \
        0,                                               \
        &(__g_imx1050_sdio_ssp0_dev.host.super.super),   \
        &__g_imx1050_sdio_ssp0_devinfo                   \
    },

#else

#define AWBL_HWCONF_IMX1050_SDIO_SSP0

#endif /* AWBL_HWCONF_IMX1050_SDIO_SSP0 */

#endif /* AWBL_HWCONF_IMX1050_SDIO_SSP0_H_ */

 /* end of file */
