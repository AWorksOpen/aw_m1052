/*******************************************************************************
*                                 AnyWhere
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      anywhere.support@zlg.cn
*******************************************************************************/

#include "aworks.h"
#include "awbl_gpio.h"
#include "imx1050_reg_base.h"
#include "driver/gpio/awbl_imx1050_gpio.h"


/* ��������״̬�� */
aw_local uint8_t __g_gpio_cfg_state[(IMX1050_GPIO_PINS + 7) / 8];

/**
 * \brief imx1050 ÿ��GPIO�˿ڵļĴ�������ַ
 */
aw_const aw_local uint32_t imx1050_gpio_regs_base[IMX1050_GPIO_PORTS] = {

    IMX1050_GPIO1_BASE,      /**< \brief GPIO1�Ĵ�������ַ */
    IMX1050_GPIO2_BASE,      /**< \brief GPIO2�Ĵ�������ַ */
    IMX1050_GPIO3_BASE,      /**< \brief GPIO3�Ĵ�������ַ */
    IMX1050_GPIO4_BASE,      /**< \brief GPIO4�Ĵ�������ַ */
    IMX1050_GPIO5_BASE       /**< \brief GPIO5�Ĵ�������ַ */
};


/**
 * \brief imx1050 �豸��Ϣ
 */
aw_local aw_const struct awbl_imx1050_gpio_devinfo __g_imx1050_gpio_devinfo = {
    NULL,
    imx1050_gpio_regs_base,
    IMX1050_IOMUXC_BASE,
    IMX1050_IOMUXC_SNVS_BASE,
    __g_gpio_cfg_state,
    {
            GPIO1_0,
            GPIO5_2
    },
    {
            INUM_GPIO1_0_15,
            INUM_GPIO5_16_31,
    },
};


struct awbl_imx1050_gpio_dev __g_imx1050_gpio_dev;

#define AWBL_HWCONF_IMX1050_GPIO        \
    {                                   \
        AWBL_IMX1050_GPIO_NAME,         \
        0,                              \
        AWBL_BUSID_PLB,                 \
        0,                              \
        &(__g_imx1050_gpio_dev.awdev),  \
        &(__g_imx1050_gpio_devinfo)     \
    },


/* end of file */

