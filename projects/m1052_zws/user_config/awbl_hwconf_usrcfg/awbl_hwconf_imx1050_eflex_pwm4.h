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


#ifndef __AWBL_HWCONF_IMX1050_EFLEX_PWM4_H
#define __AWBL_HWCONF_IMX1050_EFLEX_PWM4_H

#include "driver/pwm/awbl_imx10xx_eflex_pwm.h"

#ifdef AW_DEV_IMX1050_EFLEX_PWM4


/** \brief PWM禁能时GPIO的配置 */
#define __PWM_PIN_DFUNC_CFG   (IMX1050_PIN_MUX(PIN_MUX_ALT5) | AW_GPIO_OUTPUT)

/* Macros for entire XBARA_SELx register.  */
#define XBARA_SELx(base, output) \
    (*(volatile uint16_t *)((base) + ((output) / 2U) * 2U))

/* Set the XBARA_SELx_SELx field to a new value. */
#define XBARA_WR_SELx_SELx(base, input, output)                                   \
    (XBARA_SELx((base), (output)) =                                               \
         ((XBARA_SELx((base), (output)) & ~(0xFFU << (8UL * ((output) % 2UL)))) | \
          ((input) << (8UL * ((output) % 2UL)))))


aw_local void xbara_set_signals_connection(uint32_t   base,
                                           uint16_t   input,
                                           uint16_t   output)
{
    XBARA_WR_SELx_SELx(base, (((uint16_t)input) & 0xFFU), (((uint16_t)output) & 0xFFU));
}

aw_local aw_const awbl_imx10xx_eflex_pwm_devcfg_t __g_imx1050_eflex_pwm4_devcfg_list[] = {

    /* PWM ID ,子模块号, 通道号，PWM模式, GPIO, PWM引脚功能, 默认引脚功能 */
    {
        PWM0,
        kPWM_Module_0,
        kPWM_PwmA,
        kPWM_EdgeAligned,
        GPIO1_24,
        GPIO1_24_FLEXPWM4_PWMA00,
        __PWM_PIN_DFUNC_CFG,
        147
    },

//    {
//        PWM1,
//        kPWM_Module_0,
//        kPWM_PwmB,
//        kPWM_EdgeAligned,
//        GPIO4_1,
//        GPIO4_1_FLEXPWM4_PWMB0,
//        __PWM_PIN_DFUNC_CFG
//    },
    {
        PWM2,
        kPWM_Module_1,
        kPWM_PwmA,
        kPWM_EdgeAligned,
        GPIO1_25,
        GPIO1_25_FLEXPWM4_PWMA01,
        __PWM_PIN_DFUNC_CFG,
        148
    },
//    {
//        PWM3,
//        kPWM_Module_1,
//        kPWM_PwmB,
//        kPWM_SignedCenterAligned,
//        GPIO4_3,
//        GPIO4_3_FLEXPWM4_PWMB01,
//        __PWM_PIN_DFUNC_CFG
//    },
    {
        PWM4,
        kPWM_Module_2,
        kPWM_PwmA,
        kPWM_EdgeAligned,
        GPIO2_30,
        GPIO2_30_FLEXPWM4_PWMA02,
        __PWM_PIN_DFUNC_CFG,
        149
    },
//    {
//        PWM5,
//        kPWM_Module_2,
//        kPWM_PwmB,
//        kPWM_SignedCenterAligned,
//        GPIO4_5,
//        GPIO4_5_FLEXPWM4_PWMB02,
//        __PWM_PIN_DFUNC_CFG
//    },
    {
        PWM6,
        kPWM_Module_3,
        kPWM_PwmA,
        kPWM_EdgeAligned,
        GPIO2_31,
        GPIO2_31_FLEXPWM4_PWMA03,
        __PWM_PIN_DFUNC_CFG,
        150
    },
//    {
//        PWM7,
//        kPWM_Module_3,
//        kPWM_PwmB,
//        kPWM_SignedCenterAligned,
//        GPIO4_18,
//        GPIO4_18_FLEXPWM4_PWMB03,
//        __PWM_PIN_DFUNC_CFG
//    },
};


/* 平台相关初始化 */
aw_local void __imx1050_eflex_pwm4_plfm_init (void)
{

    int pwm_gpios[AW_NELEMENTS(__g_imx1050_eflex_pwm4_devcfg_list)];
    int i;

    /* 引脚申请 */
    for (i = 0; i < AW_NELEMENTS(__g_imx1050_eflex_pwm4_devcfg_list); i++) {
        pwm_gpios[i] = __g_imx1050_eflex_pwm4_devcfg_list[i].gpio;
    }

    aw_gpio_pin_request("eflex_pwm4_gpios", pwm_gpios, AW_NELEMENTS(pwm_gpios));

    aw_clk_enable(IMX1050_CLK_CG_XBAR1);
    aw_clk_enable(IMX1050_CLK_CG_PWM4);

    xbara_set_signals_connection(IMX1050_XBARA1_BASE, 0x101, 0x13F);
    xbara_set_signals_connection(IMX1050_XBARA1_BASE, 0x101, 0x140);
    xbara_set_signals_connection(IMX1050_XBARA1_BASE, 0x101, 0x125);
    xbara_set_signals_connection(IMX1050_XBARA1_BASE, 0x101, 0x126);
}

aw_local awbl_imx10xx_eflex_pwm_devinfo_t __g_imx1050_eflex_pwm4_devinfo = {

    IMX1050_FLEXPWM4_BASE,                  /**< \brief PWM寄存器基地址 */
    IMX1050_CLK_CG_PWM4,                      /**< \brief PWM时钟ID */
    {
        PWM0,                               /**< \brief PWM最小ID号 */
        PWM7                                /**< \brief PWM最大ID号 */
    },
    &__g_imx1050_eflex_pwm4_devcfg_list[0],  /**< \brief PWM配置信息 */

    /** \brief 使用的PWM通道数 */
    AW_NELEMENTS(__g_imx1050_eflex_pwm4_devcfg_list),
    __imx1050_eflex_pwm4_plfm_init,          /**< \brief 平台初始化 */
};

/* 设备实例内存静态分配 */
aw_local awbl_imx10xx_eflex_pwm_dev_t __g_imx1050_eflex_pwm4;

#define AWBL_HWCONF_IMX1050_EFLEX_PWM4      \
    {                                       \
        AWBL_IMX10XX_EFLEX_PWM_NAME,         \
        0,                                  \
        AWBL_BUSID_PLB,                     \
        0,                                  \
        &(__g_imx1050_eflex_pwm4.super),    \
        &(__g_imx1050_eflex_pwm4_devinfo)   \
    },

#else

#define AWBL_HWCONF_IMX1050_EFLEX_PWM4

#endif

#endif /* __AWBL_HWCONF_IMX1050_EFLEX_PWM4_H */

