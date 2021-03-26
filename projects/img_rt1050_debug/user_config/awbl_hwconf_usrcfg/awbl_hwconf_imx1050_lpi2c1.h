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

/*******************************************************************************
  IMX1050 ����I2C  ������Ϣ
*******************************************************************************/

#ifndef AWBL_HWCONF_IMX1050_LPI2C1_H_
#define AWBL_HWCONF_IMX1050_LPI2C1_H_

#include "driver/lpi2c/awbl_imx10xx_lpi2c.h"
#include "imx1050_reg_base.h"

#ifdef AW_DEV_IMX1050_LPI2C1

#define AW_CFG_IMX1050_I2C_BUS_SPEED      200000


aw_local void __imx1050_lpi2c1_plat_init (void);

aw_local void __imx1050_lpi2c1_pinmux_init (void) {

    int lpi2c1_gpios[] = {GPIO1_17, GPIO1_16};

    if (aw_gpio_pin_request("lpi2c1_gpios",
                            lpi2c1_gpios,
                            AW_NELEMENTS(lpi2c1_gpios)) == AW_OK) {
        aw_gpio_pin_cfg(GPIO1_17, GPIO1_17_LPI2C1_SDA | AW_GPIO_OPEN_DRAIN);
        aw_gpio_pin_cfg(GPIO1_16, GPIO1_16_LPI2C1_SCL | AW_GPIO_OPEN_DRAIN);
    }
}

aw_local void __imx1050_lpi2c1_rst (void) {

    aw_gpio_pin_cfg(GPIO1_16, AW_GPIO_OUTPUT | AW_GPIO_OPEN_DRAIN);
    aw_gpio_pin_cfg(GPIO1_16, GPIO1_16_LPI2C1_SCL | AW_GPIO_OPEN_DRAIN);
}

/*
 * imx1050 LPI2C1 �豸��Ϣ
 * bus_idle_timeout size: 0us   ~ 60us
 * pin_low_timeout  size: 200us ~ 16ms
 */
aw_local aw_const struct awbl_imx10xx_lpi2c_devinfo __g_imx1050_lpi2c1_devinfo = {
    {
        IMX1050_LPI2C1_BUSID,         /* ����������Ӧ�����߱�� */
        AW_CFG_IMX1050_I2C_BUS_SPEED, /* �����������ٶ� */
        1000000,                      /* SDA & SCL�͵�ƽ��ʱ (ns) */
    },
    200,                              /* SDA�˲����  (ns) */
    200,                              /* SCL�˲����  (ns) */
    60000,                            /* ���߿��г�ʱ(ns) */

    IMX1050_LPI2C1_BASE,
    INUM_LPI2C1,
    IMX1050_CLK_CG_LPI2C1,
    __imx1050_lpi2c1_plat_init,
    __imx1050_lpi2c1_rst
};


aw_local void __imx1050_lpi2c1_plat_init (void) {
    aw_clk_enable(IMX1050_CLK_CG_LPI2C1);
    __imx1050_lpi2c1_pinmux_init();
}

/* imx1050�豸ʵ���ڴ澲̬���� */
aw_local struct awbl_imx10xx_lpi2c_dev __g_imx1050_lpi2c1_dev;

#define AWBL_HWCONF_IMX1050_LPI2C1  \
    {                               \
        AWBL_IMX10XX_LPI2C_NAME,    \
        0,                          \
        AWBL_BUSID_PLB,             \
        0,                          \
        (struct awbl_dev *)&__g_imx1050_lpi2c1_dev,  \
        &(__g_imx1050_lpi2c1_devinfo)   \
    },

#else
#define AWBL_HWCONF_IMX1050_LPI2C1
#endif

#endif /* AWBL_HWCONF_IMX1050_LPI2C1_H_ */
