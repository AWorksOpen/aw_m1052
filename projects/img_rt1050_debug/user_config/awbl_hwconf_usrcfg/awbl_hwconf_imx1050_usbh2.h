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

#ifndef __AWBL_HWCONF_IMX1050_USBH2_H
#define __AWBL_HWCONF_IMX1050_USBH2_H

#ifdef AW_DEV_IMX1050_USBH2

#include "aw_gpio.h"
#include "imx1050_pin.h"
#include "driver/usb/awbl_imx10xx_usbh.h"



#define __REG32_READ(addr)           readl((volatile void __iomem *)addr)
#define __REG32_WRITE(addr, data)    writel(data, (volatile void __iomem *)(addr))
#define __REG32_SET(addr, data)      writel(data, (volatile void __iomem *)(addr + 4))
#define __REG32_CLR(addr, data)      writel(data, (volatile void __iomem *)(addr + 8))


#define __CCM_ANALOG_PLL_USB2           0x400D8020
#define __CCM_ANALOG_PLL_USB2_LOCK      (1 << 31)
#define __CCM_ANALOG_PLL_USB2_EN        (1 << 13)
#define __CCM_ANALOG_PLL_USB2_PWR       (1 << 12)
#define __CCM_ANALOG_PLL_USB2_EN_CLKS   (1 << 6)

#define __CCM_CCGR6                     0x400FC080


//aw_local void __imx1050_usbh2_pwr_ctrl (aw_bool_t on)
//{
//    if (on) {
//        aw_gpio_set(GPIO1_15, 1);
//    } else {
//        aw_gpio_set(GPIO1_15, 0);
//    }
//}


aw_local void __imx1050_usbh2_plfm_init (void)
{
    uint32_t temp;
//    int      gpios[] = {GPIO1_15};
//
//    if (aw_gpio_pin_request("usbh2_gpios", gpios, AW_NELEMENTS(gpios)) == AW_OK) {
//        /* ???ù??????? */
//        aw_gpio_pin_cfg(GPIO1_15, AW_GPIO_OUTPUT);
//        aw_gpio_set(GPIO1_15, 0);
//    }

    __REG32_WRITE(__CCM_ANALOG_PLL_USB2,
                  __CCM_ANALOG_PLL_USB2_EN |
                  __CCM_ANALOG_PLL_USB2_PWR |
                  __CCM_ANALOG_PLL_USB2_EN_CLKS);

    while (!(__REG32_READ(__CCM_ANALOG_PLL_USB2) &
            __CCM_ANALOG_PLL_USB2_LOCK));

    temp  = __REG32_READ(__CCM_CCGR6);
    temp |= 0x03;
    __REG32_WRITE(__CCM_CCGR6, temp);
};

/* ?豸??Ϣ */
aw_local aw_const struct awbl_imx10xx_usbh_info  __g_imx1050_usbh2_info = {
        {IMX1050_USBH2_BUSID},
        1,
        IMX1050_USB2_BASE_ADDR,          /**< ?Ĵ???????ַ */
        IMX1050_USBPHY2_BASE_ADDR,
        INUM_USB_OTG2,                   /**< ?жϺ? */

        16,/*??????Ҫ?????????豸?????ò???????2??*/
        32,/*??????Ҫ?????????豸?????ò???????2??*/
        32,
        0,

        NULL,//__imx1050_usbh2_pwr_ctrl,
        __imx1050_usbh2_plfm_init        /**< ƽ̨???س?ʼ??????ʼ?????? */
};

/* ?豸ʵ???ڴ澲̬???? */
aw_local struct awbl_imx10xx_usbh __g_imx1050_usbh2_dev;

#define AWBL_HWCONF_IMX1050_USBH2                     \
    {                                                 \
        AWBL_IMX10XX_USBH_DRV_NAME,                   \
        0,                                            \
        AWBL_BUSID_PLB,                               \
        0,                                            \
        (struct awbl_dev *)&__g_imx1050_usbh2_dev,    \
        &(__g_imx1050_usbh2_info)                     \
    },

#else
#define AWBL_HWCONF_IMX1050_USBH2

#endif  /* AWBL_HWCONF_IMX1050_USBH2 */

#endif /* __AWBL_HWCONF_IMX1050_USBH2_H */

/* end of file */
