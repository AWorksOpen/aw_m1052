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

#ifndef __AWBL_HWCONF_IMX1050_LPSPI3_H
#define __AWBL_HWCONF_IMX1050_LPSPI3_H


#ifdef AW_DEV_IMX1050_LPSPI3

#include "aw_gpio.h"
#include "aw_clk.h"
#include "driver/lpspi/awbl_imx10xx_lpspi.h"


/* 平台相关初始化 */
aw_local void __imx1050_lpspi3_plfm_init (void)
{
    int lpspi3_gpios[] = {GPIO1_31,  //SCK
                          GPIO1_30,  //MOSI
                          GPIO1_29,  //MISO
                          GPIO1_28,  //使用软件cs时这里不需要这个引脚
                          };

    if (aw_gpio_pin_request("lpspi3_gpios",
                             lpspi3_gpios,
                             AW_NELEMENTS(lpspi3_gpios)) == AW_OK) {

        /* 配置功能引脚 */
        aw_gpio_pin_cfg(GPIO1_31, GPIO1_31_LPSPI3_SCK);
        aw_gpio_pin_cfg(GPIO1_30, GPIO1_30_LPSPI3_SDO);
        aw_gpio_pin_cfg(GPIO1_29, GPIO1_29_LPSPI3_SDI);
        aw_gpio_pin_cfg(GPIO1_28, GPIO1_28_LPSPI3_PCS0);    //使用软件cs时这里不需要这个引脚
    }

    aw_clk_enable(IMX1050_CLK_CG_LPSPI3);
}


/* 设备信息 */
aw_local aw_const struct awbl_imx10xx_lpspi_devinfo __g_imx1050_lpspi3_devinfo = {
    {
        IMX1050_LPSPI3_BUSID,       /**< \brief 总线编号 */
    },
    IMX1050_LPSPI3_BASE,            /**< \brief 寄存器基地址 */
    INUM_LPSPI3,                    /**< \brief 中断编号 */
    IMX1050_CLK_CG_LPSPI3,          /**< \brief 时钟号 */
    AW_TRUE,                        /**< \brief 硬件CS模式 */
    AW_FALSE,                       /**< \brief 高电平有效 */
    200,                            /**< \brief cs有效到第一个sck edge的delay cycle */
    100,                            /**< \brief 最后一个sck edge到cs无效的delay cycle */
    __imx1050_lpspi3_plfm_init      /**< \brief 平台相关初始化 */
};

/* 设备实例内存静态分配 */
aw_local struct awbl_imx10xx_lpspi_dev __g_imx1050_lpspi3_dev;

#define AWBL_HWCONF_IMX1050_LPSPI3                   \
    {                                                \
        AWBL_IMX10XX_LPSPI_NAME,                     \
        0,                                           \
        AWBL_BUSID_PLB,                              \
        0,                                           \
        (struct awbl_dev *)&__g_imx1050_lpspi3_dev,  \
        &__g_imx1050_lpspi3_devinfo                  \
    },

#else
#define AWBL_HWCONF_IMX1050_LPSPI3

#endif /* AW_DEV_IMX1050_LPSPI3 */

#endif /* __AWBL_HWCONF_IMX1050_LPSPI3_H */

/* end of file */
