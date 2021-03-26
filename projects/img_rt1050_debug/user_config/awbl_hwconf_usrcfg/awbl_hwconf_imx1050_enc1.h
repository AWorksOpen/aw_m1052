/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

#ifndef __AWBL_HWCONF_IMX1050_ENC1_H
#define __AWBL_HWCONF_IMX1050_ENC1_H

#ifdef AW_DEV_IMX1050_ENC1

#include "driver/enc/awbl_imx10xx_enc.h"

/* Macros for entire XBARA_SELx register.  */
#define XBARA_SELx(base, output) \
    (*(volatile uint16_t *)((base) + ((output) / 2U) * 2U))

/* Set the XBARA_SELx_SELx field to a new value. */
#define XBARA_WR_SELx_SELx(base, input, output)                                   \
    (XBARA_SELx((base), (output)) =                                               \
         ((XBARA_SELx((base), (output)) & ~(0xFFU << (8UL * ((output) % 2UL)))) | \
          ((input) << (8UL * ((output) % 2UL)))))


#define ENC_PAD_CTRL    (PKE_1_Pull_Keeper_Enabled | \
                         SPEED_2_medium_100MHz     | \
                         DSE_6_R0_6 )

#define ENC_XBARA_SET_SIGNALS_CONNECT(base, input, output) \
    XBARA_WR_SELx_SELx(base, (((uint16_t)(input)) & 0xFFU), (((uint16_t)(output)) & 0xFFU))

/**< brief 获取IOMUXC_GPR(x)的值 */
#define READ_IOMUXC_GPR(x) \
    (*(volatile uint32_t *)(IMX1050_IOMUXC_GPR_BASE + sizeof(int) * x))

/**< brief 设置IOMUXC_GPR(x)的值 */
#define WRITE_IOMUXC_GPR(x, value) \
    ((*(volatile uint32_t *)(IMX1050_IOMUXC_GPR_BASE + sizeof(int) * x)) = value)

aw_local void __imx1050_enc1_plfm_init (void)
{

    aw_local aw_const int enc1_gpios[] = {
        GPIO1_14,
        GPIO1_15,
        GPIO1_11,
    };

    if (aw_gpio_pin_request("enc1_gpios",
                             enc1_gpios,
                             AW_NELEMENTS(enc1_gpios)) == AW_OK) {

        /* 引脚复用为 GPIO1_14_XBAR1_IN24功能*/
        aw_gpio_pin_cfg(GPIO1_14, GPIO1_14_XBAR1_IN24 | IMX1050_PAD_CTL(ENC_PAD_CTRL));

        /* 引脚复用为 GPIO1_11_XBAR1_IN25功能*/
        aw_gpio_pin_cfg(GPIO1_15, GPIO1_15_XBAR1_IN25 | IMX1050_PAD_CTL(ENC_PAD_CTRL));

        /* 引脚复用为 GPIO1_11_XBAR1_IN23功能*/
        aw_gpio_pin_cfg(GPIO1_11, GPIO1_11_XBAR1_IN23 | IMX1050_PAD_CTL(ENC_PAD_CTRL));
    }
    aw_clk_enable(IMX1050_CLK_CG_ENC1);
    aw_clk_enable(IMX1050_CLK_CG_XBAR1);

    /* A相输入 */
    ENC_XBARA_SET_SIGNALS_CONNECT(IMX1050_XBARA1_BASE, 0x118, 0x142);/*XBAR1_OUT66--ENC1_PHASEA_INPUT*/
    /* B相输入 */
    ENC_XBARA_SET_SIGNALS_CONNECT(IMX1050_XBARA1_BASE, 0x119, 0x143);/*XBAR1_OUT67--ENC1_PHASEB_INPUT*/
    /* Z相输入*/
    ENC_XBARA_SET_SIGNALS_CONNECT(IMX1050_XBARA1_BASE, 0x117, 0x144);/*XBAR1_OUT68--ENC1_INDEX*/
};

//aw_local void __imx1050_enc1_plfm_init (void)
//{
//    uint32_t    reg_val = 0;
//    aw_local aw_const int enc1_gpios[] = {
//        GPIO3_13,           /**< brief QEPZ GPIO3_13_XBAR1_INOUT05*/
//        GPIO3_14,           /**< brief QEPB GPIO3_14_XBAR1_INOUT06*/
//        GPIO3_15,           /**< brief QEPA GPIO3_15_XBAR1_INOUT07*/
//    };
//
//    if (aw_gpio_pin_request("enc1_gpios",
//                             enc1_gpios,
//                             AW_NELEMENTS(enc1_gpios)) == AW_OK) {
//
//        /* 引脚复用为 GPIO3_13_XBAR1_INOUT05功能*/
//        aw_gpio_pin_cfg(GPIO3_13, GPIO3_13_XBAR1_INOUT05 | IMX1050_PAD_CTL(ENC_PAD_CTRL));
//        /* 引脚复用为 GPIO3_14_XBAR1_INOUT06功能*/
//        aw_gpio_pin_cfg(GPIO3_14, GPIO3_14_XBAR1_INOUT06 | IMX1050_PAD_CTL(ENC_PAD_CTRL));
//        /* 引脚复用为 GPIO3_15_XBAR1_INOUT07功能*/
//        aw_gpio_pin_cfg(GPIO3_15, GPIO3_15_XBAR1_INOUT07 | IMX1050_PAD_CTL(ENC_PAD_CTRL));
//    }
//    aw_clk_enable(IMX1050_CLK_CG_ENC1);
//    aw_clk_enable(IMX1050_CLK_CG_XBAR1);
//
//    /**<brief 设置XBAR1_INOUT05、XBAR1_INOUT06、XBAR1_INOUT07为输入模式*/
//    reg_val = READ_IOMUXC_GPR(6);
//    reg_val &= 0xFFF1FFFF;
//    WRITE_IOMUXC_GPR(6, reg_val);
//
//    /* A相输入 */
//    ENC_XBARA_SET_SIGNALS_CONNECT(IMX1050_XBARA1_BASE, 0x107, 0x142);/*XBAR1_OUT66--ENC1_PHASEA_INPUT*/
//    /* B相输入 */
//    ENC_XBARA_SET_SIGNALS_CONNECT(IMX1050_XBARA1_BASE, 0x106, 0x143);/*XBAR1_OUT67--ENC1_PHASEB_INPUT*/
//    /* Z相输入*/
//    ENC_XBARA_SET_SIGNALS_CONNECT(IMX1050_XBARA1_BASE, 0x105, 0x144);/*XBAR1_OUT68--ENC1_INDEX*/
//};


/* 设备信息 */
aw_local aw_const struct awbl_imx10xx_enc_devinfo __g_imx1050_enc1_devinfo = {
    IMX1050_ENC1_BASE,              /**< \brief 寄存器基地址*/
    INUM_ENC1,                      /**< \brief 中断号 */
    IMX1050_ENC1,                   /**< \brief 当前通道号 */
    enc_index_trigger_rising_edge,     /**< \brief index触发 */
    0,                              /**< \brief 滤波器采样计数 */
    10,                             /**< \brief 滤波器采样周期 */
    __imx1050_enc1_plfm_init        /**< \brief 平台初始化 */
};

/* 设备实例内存静态分配 */
aw_local struct awbl_imx10xx_enc_dev __g_imx1050_enc1_dev;

#define AWBL_HWCONF_IMX1050_ENC1                 \
{                                                \
    AWBL_IMX10XX_ENC_NAME,                       \
    0,                                           \
    AWBL_BUSID_PLB,                              \
    0,                                           \
    (struct awbl_dev *)&__g_imx1050_enc1_dev,    \
    &(__g_imx1050_enc1_devinfo)                  \
},

#else
#define AWBL_HWCONF_IMX1050_ENC1
#endif  /* AW_DEV_IMX1050_ENC1 */

#endif /* __AWBL_HWCONF_IMX1050_ENC1_H */

/* end of file */
