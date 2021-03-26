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

#ifndef __AWBL_HWCONF_IMX1050_LPSPI1_H
#define __AWBL_HWCONF_IMX1050_LPSPI1_H


#ifdef AW_DEV_IMX1050_LPSPI1

#include "aw_gpio.h"
#include "aw_clk.h"
#include "driver/lpspi/awbl_imx10xx_lpspi.h"


/* 平台相关初始化 */
aw_local void __imx1050_lpspi1_plfm_init (void)
{
    int lpspi1_gpios[] = {
        GPIO3_12,  //CLK
        GPIO3_14,  //MOSI
        GPIO3_15,  //MISO
    };

    if (aw_gpio_pin_request("lpspi1_gpios",
                             lpspi1_gpios,
                             AW_NELEMENTS(lpspi1_gpios)) == AW_OK) {

        /* 配置功能引脚 */
        aw_gpio_pin_cfg(GPIO3_14, GPIO3_14_LPSPI1_SDO);
        aw_gpio_pin_cfg(GPIO3_15, GPIO3_15_LPSPI1_SDI);
        aw_gpio_pin_cfg(GPIO3_12, GPIO3_12_LPSPI1_SCK);
    }
    aw_clk_enable(IMX1050_CLK_CG_LPSPI1);
}


/* 设备信息 */
aw_local aw_const struct awbl_imx10xx_lpspi_devinfo __g_imx1050_lpspi1_devinfo = {
    {
        IMX1050_LPSPI1_BUSID,       /**< \brief 总线编号 */
    },
    IMX1050_LPSPI1_BASE,            /**< \brief 寄存器基地址 */
    INUM_LPSPI1,                    /**< \brief 中断编号 */
    IMX1050_CLK_CG_LPSPI1,          /**< \brief 时钟号 */
    AW_FALSE,                       /**< \brief 硬件CS模式 */
    AW_FALSE,                       /**< \brief 高电平有效 */
    200,                            /**< \brief cs有效到第一个sck edge的delay cycle */
    100,                            /**< \brief 最后一个sck edge到cs无效的delay cycle */
    __imx1050_lpspi1_plfm_init      /**< \brief 平台相关初始化 */
};

/* 设备实例内存静态分配 */
aw_local struct awbl_imx10xx_lpspi_dev __g_imx1050_lpspi1_dev;

#define AWBL_HWCONF_IMX1050_LPSPI1                   \
    {                                                \
        AWBL_IMX10XX_LPSPI_NAME,                     \
        0,                                           \
        AWBL_BUSID_PLB,                              \
        0,                                           \
        (struct awbl_dev *)&__g_imx1050_lpspi1_dev,  \
        &__g_imx1050_lpspi1_devinfo                  \
    },

#else
#define AWBL_HWCONF_IMX1050_LPSPI1

#endif /* AW_DEV_IMX1050_LPSPI */

#endif /* __AWBL_HWCONF_IMX1050_LPSPI_H */

/* end of file */
