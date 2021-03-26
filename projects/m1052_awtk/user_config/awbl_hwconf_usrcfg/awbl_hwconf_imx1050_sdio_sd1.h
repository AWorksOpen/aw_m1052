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

#ifndef AWBL_HWCONF_IMX1050_SDIO_SSP1_H_
#define AWBL_HWCONF_IMX1050_SDIO_SSP1_H_

#include "aw_gpio.h"
#include "aw_clk.h"
#include "awbl_sdiobus.h"
#include "aw_delay.h"
#include "imx1050_pin.h"
#include "driver/sdio/awbl_imx10xx_sdio.h"
#include "driver/sdiocard/awbl_sdcard.h"

#ifdef AW_DEV_IMX1050_USDHC2

#define USDHC2_PAD_CTRL    (SRE_0_Fast_Slew_Rate | \
                            DSE_6_R0_6 |           \
                            SPEED_3_max_200MHz|    \
                            PKE_1_Pull_Keeper_Enabled |\
                            PUE_1_Pull |           \
                            PUS_1_47K_Ohm_Pull_Up| \
                            HYS_1_Hysteresis_Enabled)

aw_local void __imx1050_sdio_ssp1_plfm_init (void);

/* 检测卡插拔状态 */
aw_local aw_bool_t __imx1050_sdio_ssp1_detect (awbl_imx10xx_sdio_dev_t *p_dev)
{
    return AW_TRUE;
}

/* 设备信息 */
aw_local aw_const awbl_imx10xx_sdio_dev_info_t __g_imx1050_sdio_ssp1_devinfo = {
    {
        IMX1050_USBH2_BUSID,
        AWBL_SDIO_SD_4B_M,
        AWBL_SDIO_CON_DEV_STATIC,
        AW_TRUE,
        AW_FALSE,
        (AWBL_SD_OCR_VDD_27_28 | AWBL_SD_OCR_VDD_28_29 | AWBL_SD_OCR_VDD_29_30 |
         AWBL_SD_OCR_VDD_30_31 | AWBL_SD_OCR_VDD_31_32 | AWBL_SD_OCR_VDD_32_33 |
         AWBL_SD_OCR_VDD_33_34 | AWBL_SD_OCR_VDD_34_35 | AWBL_SD_OCR_VDD_35_36),
    },

    IMX1050_USDHC2_BASE,                /* 寄存器基地址 */
    IMX1050_CLK_CG_USDHC2,                /* 输入时钟ID  */
    1,                                  /* 传输任务优先级 */
    __imx1050_sdio_ssp1_plfm_init,      /* 平台相关初始化 */
    INUM_USDHC2,                        /* 中断号 */
    __imx1050_sdio_ssp1_detect
};


/* 平台相关初始化 */
aw_local void __imx1050_sdio_ssp1_plfm_init (void)
{
    /* 配置功能引脚 */

    int sdio_gpios[] = {
        GPIO3_5,    /* GPIO_SD_B0_00(USDHC1_CMD) */
        GPIO3_4,    /* GPIO_SD_B0_01(USDHC1_CLK) */
        GPIO3_3,    /* GPIO_SD_B0_02(USDHC1_DATA0) */
        GPIO3_2,    /* GPIO_SD_B0_03(USDHC1_DATA1) */
        GPIO3_1,    /* GPIO_SD_B0_04(USDHC1_DATA2) */
        GPIO3_0,    /* GPIO_SD_B0_05(USDHC1_DATA3) */
    };

    if (aw_gpio_pin_request("sdio_gpios",
                             sdio_gpios,
                             AW_NELEMENTS(sdio_gpios)) == AW_OK) {

        /* 配置功能引脚 */
        aw_gpio_pin_cfg(GPIO3_5, GPIO3_5_USDHC2_CMD  | IMX1050_PAD_CTL(USDHC2_PAD_CTRL) );
        aw_gpio_pin_cfg(GPIO3_4, GPIO3_4_USDHC2_CLK  | IMX1050_PAD_CTL(USDHC2_PAD_CTRL) );
        aw_gpio_pin_cfg(GPIO3_0, GPIO3_0_USDHC2_DATA3| IMX1050_PAD_CTL(USDHC2_PAD_CTRL) );
        aw_gpio_pin_cfg(GPIO3_1, GPIO3_1_USDHC2_DATA2| IMX1050_PAD_CTL(USDHC2_PAD_CTRL) );
        aw_gpio_pin_cfg(GPIO3_2, GPIO3_2_USDHC2_DATA1| IMX1050_PAD_CTL(USDHC2_PAD_CTRL) );
        aw_gpio_pin_cfg(GPIO3_3, GPIO3_3_USDHC2_DATA0| IMX1050_PAD_CTL(USDHC2_PAD_CTRL) );
    }

    aw_clk_parent_set(IMX1050_CLK_USDHC2_CLK_SEL, IMX1050_CLK_PLL2_PFD2);
    aw_clk_enable(IMX1050_CLK_CG_USDHC2);
};

/* 设备实例内存静态分配 */
awbl_imx10xx_sdio_dev_t __g_imx1050_sdio_ssp1_dev;

#define AWBL_HWCONF_IMX1050_SDIO_SSP1                    \
    {                                                    \
        AWBL_IMX10XX_SDIO_NAME,                          \
        IMX1050_USBH2_BUSID,                             \
        AWBL_BUSID_PLB,                                  \
        0,                                               \
        &(__g_imx1050_sdio_ssp1_dev.host.super.super),   \
        &__g_imx1050_sdio_ssp1_devinfo                   \
    },

#else

#define AWBL_HWCONF_IMX1050_SDIO_SSP1

#endif /* AWBL_HWCONF_IMX1050_SDIO_SSP1 */

#endif /* AWBL_HWCONF_IMX1050_SDIO_SSP1_H_ */

 /* end of file */
