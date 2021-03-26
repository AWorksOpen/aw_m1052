/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

#ifndef __AWBL_HWCONF_IMX1050_USBH1_H
#define __AWBL_HWCONF_IMX1050_USBH1_H

#ifdef AW_DEV_IMX1050_USBH1

#include "aw_gpio.h"
#include "imx1050_pin.h"
#include "driver/usb/awbl_imx10xx_usbh.h"



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


//aw_local void __imx1050_usbh1_pwr_ctrl (aw_bool_t on)
//{
//    if (on) {
//        aw_gpio_set(GPIO1_17, 1);
//    } else {
//        aw_gpio_set(GPIO1_17, 0);
//    }
//}


aw_local void __imx1050_usbh1_plfm_init (void)
{
    uint32_t temp;
//    int      gpios[] = {GPIO1_17};

//    if (aw_gpio_pin_request("usbh1_gpios", gpios, AW_NELEMENTS(gpios)) == AW_OK) {
//        /* 配置功能引脚 */
//        aw_gpio_pin_cfg(GPIO1_17, AW_GPIO_OUTPUT);
//        aw_gpio_set(GPIO1_17, 0);
//    }

    __REG32_WRITE(__CCM_ANALOG_PLL_USB1,
                  __CCM_ANALOG_PLL_USB1_EN |
                  __CCM_ANALOG_PLL_USB1_PWR |
                  __CCM_ANALOG_PLL_USB1_EN_CLKS);

    while (!(__REG32_READ(__CCM_ANALOG_PLL_USB1) &
            __CCM_ANALOG_PLL_USB1_LOCK));

    temp  = __REG32_READ(__CCM_CCGR6);
    temp |= 0x03;
    __REG32_WRITE(__CCM_CCGR6, temp);

};

/* 设备信息 */
aw_local aw_const struct awbl_imx10xx_usbh_info  __g_imx1050_usbh1_info = {
        {IMX1050_USBH1_BUSID},
        1,
        IMX1050_USB1_BASE_ADDR,         /**< 寄存器基地址 */
        IMX1050_USBPHY1_BASE_ADDR,
        INUM_USB_OTG1,                  /**< 中断号 */

        16, /*如果需要接入多个设备，将该参数增加2倍*/
        32, /*如果需要接入多个设备，将该参数增加2倍*/
        32,
        0,

        NULL,
        __imx1050_usbh1_plfm_init       /**< 平台相关初始化：初始化引脚 */
};

/* 设备实例内存静态分配 */
struct awbl_imx10xx_usbh __g_imx1050_usbh1_dev;

#define AWBL_HWCONF_IMX1050_USBH1                     \
    {                                                 \
        AWBL_IMX10XX_USBH_DRV_NAME,                   \
        0,                                            \
        AWBL_BUSID_PLB,                               \
        0,                                            \
        (struct awbl_dev *)&__g_imx1050_usbh1_dev,    \
        &(__g_imx1050_usbh1_info)                     \
    },

#else
#define AWBL_HWCONF_IMX1050_USBH1

#endif  /* AWBL_HWCONF_IMX1050_USBH1 */

#endif /* __AWBL_HWCONF_IMX1050_USBH1_H */

/* end of file */
