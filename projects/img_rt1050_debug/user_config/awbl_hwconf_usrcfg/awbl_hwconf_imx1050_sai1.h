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
#ifndef  __AWBL_HWCONF_IMX1050_SAI1_H
#define  __AWBL_HWCONF_IMX1050_SAI1_H
#ifdef AW_DEV_IMX1050_SAI1
/*******************************************************************************
 SAI1控制器配置信息
*******************************************************************************/
#include "aworks.h"
#include "aw_spinlock.h"
#include "awbus_lite.h"
#include "driver/sai/awbl_imx1050_sai.h"
#include "driver/edma/aw_imx10xx_edma.h"
#include "aw_gpio.h"
#define     EDMA_EVENTS_SAI1_TX         IMX_DMATYPE_SAI1_TX
#define     EDMA_EVENTS_SAI1_RX         IMX_DMATYPE_SAI1_RX
aw_local void __imx1050_sai1_plat_init (void)
{
    /* 引脚初始化  */
    int sai1_gpios[] = {
            GPIO1_25,   /* MCLK */
            GPIO1_28,   /* RX_DATA00 */
            GPIO1_29,   /* TX_DATA00 */
            GPIO1_30,   /* TX_BCLK */
            GPIO1_31    /* TX_SYNC */
    };
    if (aw_gpio_pin_request("sai1_gpios",
                            sai1_gpios,
                            AW_NELEMENTS(sai1_gpios)) == AW_OK) {
        /* 配置功能引脚 */
        aw_gpio_pin_cfg(GPIO1_25, GPIO1_25_SAI1_MCLK |
                                  IMX1050_PAD_CTL(SPEED_2_medium_100MHz |
                                                  DSE_6_R0_6 |
                                                  PKE_1_Pull_Keeper_Enabled));
        aw_gpio_pin_cfg(GPIO1_28, GPIO1_28_SAI1_RX_DATA00 |
                                  IMX1050_PAD_CTL(SPEED_2_medium_100MHz |
                                                  DSE_6_R0_6 |
                                                  PKE_1_Pull_Keeper_Enabled));
        aw_gpio_pin_cfg(GPIO1_29, GPIO1_29_SAI1_TX_DATA00 |
                                  IMX1050_PAD_CTL(SPEED_2_medium_100MHz |
                                                  DSE_6_R0_6 |
                                                  PKE_1_Pull_Keeper_Enabled));
        aw_gpio_pin_cfg(GPIO1_30, GPIO1_30_SAI1_TX_BCLK |
                                  IMX1050_PAD_CTL(SPEED_2_medium_100MHz |
                                                  DSE_6_R0_6 |
                                                  PKE_1_Pull_Keeper_Enabled));
        aw_gpio_pin_cfg(GPIO1_31, GPIO1_31_SAI1_TX_SYNC |
                                  IMX1050_PAD_CTL(SPEED_2_medium_100MHz |
                                                  DSE_6_R0_6 |
                                                  PKE_1_Pull_Keeper_Enabled));
        AW_REG_WRITE32(0x400AC004, AW_REG_READ32(0x400AC004) | (1 << 19));
    }
    aw_clk_rate_set(IMX1050_CLK_PLL_AUDIO, 44100*32*2*8*8);
    aw_clk_rate_get(IMX1050_CLK_PLL_AUDIO);
    aw_clk_parent_set(IMX1050_CLK_SAI1_CLK_SEL ,IMX1050_CLK_PLL_AUDIO);
    /* 时钟初始化  */
    aw_clk_enable(IMX1050_CLK_CG_SAI1);
    aw_clk_enable(IMX1050_CLK_SAI1_CLK_ROOT);
}
/* 设备信息 */
aw_local aw_const struct awbl_imx1050_sai_devinfo __g_imx1050_sai1_devinfo = {
    IMX1050_SAI1_BASE,                  /* 寄存器基址 */
    INUM_SAI1,                          /* 中断号          */
    IMX1050_CLK_SAI1_CLK_ROOT,          /* 时钟id    */
    AW_BL_IMX1050_SAI_MCLK_SEL_1,       /* 使用MQS时，该值必须为 1 */
    {"imxrt1050_sai0_playback", "imxrt1050_sai0_capture"},
    {"imxrt1050_dai_sai0"},
    __imx1050_sai1_plat_init,           /* 平台相关初始化 */
    EDMA_EVENTS_SAI1_TX,
    EDMA_EVENTS_SAI1_RX,
};
/* 设备实例内存静态分配 */
aw_local struct awbl_imx1050_sai_dev __g_imx1050_sai1_dev;
#define AWBL_HWCONF_IMX1050_SAI1                    \
    {                                               \
        AWBL_IMX1050_SAI_NAME,                       \
        0,                                          \
        AWBL_BUSID_PLB,                             \
        0,                                          \
        (struct awbl_dev *)&__g_imx1050_sai1_dev,    \
        &__g_imx1050_sai1_devinfo                    \
    },
#else
#define AWBL_HWCONF_IMX1050_SAI1
#endif /* AW_DEV_IMX1050_SAI1 */
#endif /* __AWBL_HWCONF_IMX1050_SAI1_H */
