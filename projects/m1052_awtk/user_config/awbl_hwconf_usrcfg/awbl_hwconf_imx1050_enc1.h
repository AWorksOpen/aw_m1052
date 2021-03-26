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

/**< brief ��ȡIOMUXC_GPR(x)��ֵ */
#define READ_IOMUXC_GPR(x) \
    (*(volatile uint32_t *)(IMX1050_IOMUXC_GPR_BASE + sizeof(int) * x))

/**< brief ����IOMUXC_GPR(x)��ֵ */
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

        /* ���Ÿ���Ϊ GPIO1_14_XBAR1_IN24����*/
        aw_gpio_pin_cfg(GPIO1_14, GPIO1_14_XBAR1_IN24 | IMX1050_PAD_CTL(ENC_PAD_CTRL));

        /* ���Ÿ���Ϊ GPIO1_11_XBAR1_IN25����*/
        aw_gpio_pin_cfg(GPIO1_15, GPIO1_15_XBAR1_IN25 | IMX1050_PAD_CTL(ENC_PAD_CTRL));

        /* ���Ÿ���Ϊ GPIO1_11_XBAR1_IN23����*/
        aw_gpio_pin_cfg(GPIO1_11, GPIO1_11_XBAR1_IN23 | IMX1050_PAD_CTL(ENC_PAD_CTRL));
    }
    aw_clk_enable(IMX1050_CLK_CG_ENC1);
    aw_clk_enable(IMX1050_CLK_CG_XBAR1);

    /* A������ */
    ENC_XBARA_SET_SIGNALS_CONNECT(IMX1050_XBARA1_BASE, 0x118, 0x142);/*XBAR1_OUT66--ENC1_PHASEA_INPUT*/
    /* B������ */
    ENC_XBARA_SET_SIGNALS_CONNECT(IMX1050_XBARA1_BASE, 0x119, 0x143);/*XBAR1_OUT67--ENC1_PHASEB_INPUT*/
    /* Z������*/
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
//        /* ���Ÿ���Ϊ GPIO3_13_XBAR1_INOUT05����*/
//        aw_gpio_pin_cfg(GPIO3_13, GPIO3_13_XBAR1_INOUT05 | IMX1050_PAD_CTL(ENC_PAD_CTRL));
//        /* ���Ÿ���Ϊ GPIO3_14_XBAR1_INOUT06����*/
//        aw_gpio_pin_cfg(GPIO3_14, GPIO3_14_XBAR1_INOUT06 | IMX1050_PAD_CTL(ENC_PAD_CTRL));
//        /* ���Ÿ���Ϊ GPIO3_15_XBAR1_INOUT07����*/
//        aw_gpio_pin_cfg(GPIO3_15, GPIO3_15_XBAR1_INOUT07 | IMX1050_PAD_CTL(ENC_PAD_CTRL));
//    }
//    aw_clk_enable(IMX1050_CLK_CG_ENC1);
//    aw_clk_enable(IMX1050_CLK_CG_XBAR1);
//
//    /**<brief ����XBAR1_INOUT05��XBAR1_INOUT06��XBAR1_INOUT07Ϊ����ģʽ*/
//    reg_val = READ_IOMUXC_GPR(6);
//    reg_val &= 0xFFF1FFFF;
//    WRITE_IOMUXC_GPR(6, reg_val);
//
//    /* A������ */
//    ENC_XBARA_SET_SIGNALS_CONNECT(IMX1050_XBARA1_BASE, 0x107, 0x142);/*XBAR1_OUT66--ENC1_PHASEA_INPUT*/
//    /* B������ */
//    ENC_XBARA_SET_SIGNALS_CONNECT(IMX1050_XBARA1_BASE, 0x106, 0x143);/*XBAR1_OUT67--ENC1_PHASEB_INPUT*/
//    /* Z������*/
//    ENC_XBARA_SET_SIGNALS_CONNECT(IMX1050_XBARA1_BASE, 0x105, 0x144);/*XBAR1_OUT68--ENC1_INDEX*/
//};


/* �豸��Ϣ */
aw_local aw_const struct awbl_imx10xx_enc_devinfo __g_imx1050_enc1_devinfo = {
    IMX1050_ENC1_BASE,              /**< \brief �Ĵ�������ַ*/
    INUM_ENC1,                      /**< \brief �жϺ� */
    IMX1050_ENC1,                   /**< \brief ��ǰͨ���� */
    enc_index_trigger_rising_edge,     /**< \brief index���� */
    0,                              /**< \brief �˲����������� */
    10,                             /**< \brief �˲����������� */
    __imx1050_enc1_plfm_init        /**< \brief ƽ̨��ʼ�� */
};

/* �豸ʵ���ڴ澲̬���� */
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
