/*
 * awbl_hwconf_csi.h
 *
 *  Created on: 2018年5月15日
 *      Author: yanghuatao
 */

#ifndef __AWBL_HWCONF_IMX1050_CSI_H
#define __AWBL_HWCONF_IMX1050_CSI_H

#ifdef AW_DEV_IMX1050_CSI

#include "aw_gpio.h"
#include "aw_clk.h"
#include "imx1050_inum.h"
#include "driver/camera/awbl_imx1050_csi.h"

#define CAMERA_DATA_BUS_ID_0      0

void __imx1050_csi_plfm_init(void)
{
    int lpcsi_gpios[] = {
            GPIO1_31,
            GPIO1_30,
            GPIO1_29,
            GPIO1_28,
            GPIO1_27,
            GPIO1_26,
            GPIO1_25,
            GPIO1_24,
            GPIO1_22,
            GPIO1_23,
            GPIO1_20,
            GPIO1_21,
    };


    aw_clk_parent_set(IMX1050_CLK_CSI_CLK_SEL,IMX1050_CLK_OSC_24M);
    aw_clk_rate_set(IMX1050_CLK_CSI_PODF, 24000000);
    aw_clk_enable(IMX1050_CLK_CG_CSI);

    if (aw_gpio_pin_request("csi_gpios",
            lpcsi_gpios,
                             AW_NELEMENTS(lpcsi_gpios)) == AW_OK) {

        /* 配置功能引脚 */
        aw_gpio_pin_cfg(GPIO1_31, GPIO1_31_CSI_DATA02);
        aw_gpio_pin_cfg(GPIO1_30, GPIO1_30_CSI_DATA03);
        aw_gpio_pin_cfg(GPIO1_29, GPIO1_29_CSI_DATA04);
        aw_gpio_pin_cfg(GPIO1_28, GPIO1_28_CSI_DATA05);

        aw_gpio_pin_cfg(GPIO1_27, GPIO1_27_CSI_DATA06);
        aw_gpio_pin_cfg(GPIO1_26, GPIO1_26_CSI_DATA07);
        aw_gpio_pin_cfg(GPIO1_25, GPIO1_25_CSI_DATA08);
        aw_gpio_pin_cfg(GPIO1_24, GPIO1_24_CSI_DATA09);
        aw_gpio_pin_cfg(GPIO1_22, GPIO1_22_CSI_VSYNC);
        aw_gpio_pin_cfg(GPIO1_23, GPIO1_23_CSI_HSYNC);
        aw_gpio_pin_cfg(GPIO1_20, GPIO1_20_CSI_PIXCLK);
        aw_gpio_pin_cfg(GPIO1_21, GPIO1_21_CSI_MCLK);
    }
}
/* 设备信息 */
aw_local aw_const struct awbl_imx1050_csi_devinfo __g_imx1050_csi_devinfo = {
    CAMERA_DATA_BUS_ID_0,         /* 控制器ID */
    IMX1050_CSI_BASE,             /* ADC1寄存器基址 */
    INUM_CSI,                     /* 中断号 */
    GATED_CLOCK,
    USE_EXT_VSYNC,
    PROGRESSIVE_MODE,
    6,                            /* CSI任务优先级  */
    __imx1050_csi_plfm_init
};
/* 设备实例内存静态分配 */
aw_local struct awbl_imx1050_csi_dev __g_imx1050_csi_dev;

#define AWBL_HWCONF_IMX1050_CSI                  \
{                                                \
    AWBL_IMX1050_CSI_NAME,                       \
    1,                                           \
    AWBL_BUSID_PLB,                              \
    0,                                           \
    (struct awbl_dev *)&__g_imx1050_csi_dev,     \
    &(__g_imx1050_csi_devinfo)                   \
},

#else
#define AWBL_HWCONF_IMX1050_CSI
#endif  /* AW_DEV_IMX1050_CSI */

#endif /* __AWBL_HWCONF_IMX1050_CSI_H */
