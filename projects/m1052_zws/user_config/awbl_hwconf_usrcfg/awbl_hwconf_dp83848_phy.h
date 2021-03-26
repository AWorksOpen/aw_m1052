/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      aworks.support@zlg.cn
*******************************************************************************/

#ifndef __AWBL_HWCONF_DP83848_PHY_H
#define __AWBL_HWCONF_DP83848_PHY_H

#ifdef AW_DEV_DP83848_PHY

#include "aw_gpio.h"
#include "aw_delay.h"

#include "driver/phy/awbl_dp83848_phy.h"

/* Ӳ�� PHY ��ַ */
#define  DP83848_PHY_ADDR       0x01

/* PHY��λ���� */
#define  DP83848_PHY_RESET_PIN  GPIO1_8

/* ƽ̨��س�ʼ�� */
aw_local void __dp83848_phy_plfm_init (void)
{
    aw_gpio_pin_cfg(DP83848_PHY_RESET_PIN, AW_GPIO_OUTPUT | AW_GPIO_PULL_UP);

    /**
     * Hard Reset PHY
     */
    aw_gpio_set(DP83848_PHY_RESET_PIN, 1);
    aw_udelay(1000);
    aw_gpio_set(DP83848_PHY_RESET_PIN, 0);
    aw_udelay(1000);          /* ����1us�ĵ͵�ƽ���ḴλPHYоƬ��DP83848��*/
    aw_gpio_set(DP83848_PHY_RESET_PIN, 1);
}

/* �豸��Ϣ */
aw_local aw_const struct awbl_dp83848_phy_devinfo __g_dp83848_phy_devinfo = {
    "eth0",
    DP83848_PHY_ADDR,          /* phy addr */
    AWBL_PHY_LINK_AUTONEG,      /* PHY mode */
    __dp83848_phy_plfm_init,   /* ƽ̨��س�ʼ�� */
};

/* �豸ʵ���ڴ澲̬���� */
aw_local struct awbl_dp83848_phy_dev __g_dp83848_phy_dev;

#define AWBL_HWCONF_DP83848_PHY                        \
    {                                                   \
        AWBL_DP83848_PHY_NAME,                          \
        0,                                              \
        AWBL_BUSID_MII,                                 \
        IMX1050_MII_BUSID,                              \
        (struct awbl_dev *)&__g_dp83848_phy_dev,       \
        &__g_dp83848_phy_devinfo                       \
    },

#else
#define AWBL_HWCONF_DP83848_PHY

#endif  /* AW_DEV_DP83848_PHY */

#endif  /* __AWBL_HWCONF_DP83848_PHY_H */

/* end of file */
