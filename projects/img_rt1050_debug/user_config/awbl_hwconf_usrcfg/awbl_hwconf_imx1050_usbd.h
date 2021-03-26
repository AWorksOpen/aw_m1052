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

#ifndef __AWBL_HWCONF_IMX1050_USBD_H
#define __AWBL_HWCONF_IMX1050_USBD_H

#ifdef AW_DEV_IMX1050_USBD

#include "aw_gpio.h"
#include "aw_io.h"

#include "driver/usb/awbl_imx10xx_usbd.h"
#include "imx1050_reg_base.h"
#include "imx1050_pin.h"
#include "aw_gpio.h"
#include "imx1050_inum.h"

#define OTG_PAD_CTRL    (SRE_0_Fast_Slew_Rate | DSE_3_R0_3 |      \
                         SPEED_1_medium_100MHz|                   \
                         PKE_1_Pull_Keeper_Enabled | PUE_1_Pull | \
                         PUS_1_47K_Ohm_Pull_Up| HYS_1_Hysteresis_Enabled)

#define __REG32_READ(addr)           readl((volatile void __iomem *)addr)
#define __REG32_WRITE(addr, data)    writel(data, (volatile void __iomem *)(addr))
#define __REG32_SET(addr, data)      writel(data, (volatile void __iomem *)(addr + 4))
#define __REG32_CLR(addr, data)      writel(data, (volatile void __iomem *)(addr + 8))

#define __CCM_ANALOG_PLL_USB1           0x400D8010
#define __CCM_ANALOG_PLL_USB1_LOCK      (1 << 31)
#define __CCM_ANALOG_PLL_USB1_EN        (1 << 13)
#define __CCM_ANALOG_PLL_USB1_PWR       (1 << 12)
#define __CCM_ANALOG_PLL_USB1_EN_CLKS   (1 << 6)

#define __CCM_CCGR6                     0x400FC080
#define __PMU_REG_3P0                   0x400D8120

#define __PMU_REG_3P0_OUTPUT_TRG        (0x17 << 8)
#define __PMU_REG_3P0_ENABLE_LINREG_MASK (0x1U)


aw_local void __imx1050_usbd_pwr_ctrl (aw_bool_t on)
{
    if (on) {
        aw_gpio_set(GPIO1_17, 1);
    } else {
        aw_gpio_set(GPIO1_17, 0);
    }
}

void usbd_clk_init(void)
{
    uint32_t temp = 0;

    __REG32_WRITE(__CCM_ANALOG_PLL_USB1,
                  __CCM_ANALOG_PLL_USB1_EN |
                  __CCM_ANALOG_PLL_USB1_PWR |
                  __CCM_ANALOG_PLL_USB1_EN_CLKS);

    while (!(__REG32_READ(__CCM_ANALOG_PLL_USB1) &
            __CCM_ANALOG_PLL_USB1_LOCK));

    temp  = __REG32_READ(__CCM_CCGR6);
    temp |= 0x03;
    __REG32_WRITE(__CCM_CCGR6, temp);

    temp = (__REG32_READ(__PMU_REG_3P0) & (~0x1F00)) |
           (__PMU_REG_3P0_OUTPUT_TRG | __PMU_REG_3P0_ENABLE_LINREG_MASK);
    __REG32_WRITE(__PMU_REG_3P0, temp);

}


aw_local void __imx1050_usbd_plfm_init (void)
{
//    int      gpios[] = {GPIO1_17, GPIO1_1};
//    if (aw_gpio_pin_request("usbh1_gpios", gpios, AW_NELEMENTS(gpios)) == AW_OK) {
//
//        /* 配置电源控制引脚 */
//        aw_gpio_pin_cfg(GPIO1_17, AW_GPIO_OUTPUT);
//
//        /* OTG1 ID */
//        aw_gpio_pin_cfg(GPIO1_1, GPIO1_1_USB_OTG1_ID | IMX1050_PAD_CTL(OTG_PAD_CTRL));
//
//        /* 电源关 */
//        __imx1050_usbd_pwr_ctrl(0);
//    }

    /* 配置USB Device时钟 */
    usbd_clk_init();
};


/* 设备信息 */
aw_local aw_const struct awbl_imx10xx_usbd_info  __g_imx1050_usbd_info = {
    {
        "/dev/usbd",
        AWBL_USBD_CFG_SELF_POWERED
    },

    IMX1050_USB1_BASE_ADDR,        /**< 寄存器基地址 */
    IMX1050_USBPHY1_BASE_ADDR,
    INUM_USB_OTG1,
    5,                            /**< 处理usb中断业务的任务优先级 */
    32,
    __imx1050_usbd_plfm_init       /**< 平台相关初始化：初始化引脚 */

};

/* 设备实例内存静态分配 */
static struct awbl_imx10xx_usbd __g_imx1050_usbd_dev;

#define AWBL_HWCONF_IMX1050_USBD                   \
    {                                              \
        AWBL_IMX10XX_USBD_DRV_NAME,                \
        0,                                         \
        AWBL_BUSID_PLB,                            \
        0,                                         \
        (struct awbl_dev *)&__g_imx1050_usbd_dev,  \
        &(__g_imx1050_usbd_info)                   \
    },

#else
#define AWBL_HWCONF_IMX1050_USBD

#endif  /* AWBL_HWCONF_IMX1050_USBD */


#endif  /* __AWBL_HWCONF_IMX1050_USBD_H */

/* end of file */
