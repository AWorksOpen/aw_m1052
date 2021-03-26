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


#ifndef __AWBL_HWCONF_IMX1050_EFLEX_PWM1_H
#define __AWBL_HWCONF_IMX1050_EFLEX_PWM1_H

#include "driver/pwm/awbl_imx10xx_eflex_pwm.h"

#ifdef AW_DEV_IMX1050_EFLEX_PWM1


/** \brief PWM����ʱGPIO������ */
#define __PWM_PIN_DFUNC_CFG   (IMX1050_PIN_MUX(PIN_MUX_ALT5) | AW_GPIO_OUTPUT)

/* Macros for entire XBARA_SELx register.  */
#define XBARA_SELx(base, output) \
    (*(volatile uint16_t *)((base) + ((output) / 2U) * 2U))

/* Set the XBARA_SELx_SELx field to a new value. */
#define XBARA_WR_SELx_SELx(base, input, output)                                   \
    (XBARA_SELx((base), (output)) =                                               \
         ((XBARA_SELx((base), (output)) & ~(0xFFU << (8UL * ((output) % 2UL)))) | \
          ((input) << (8UL * ((output) % 2UL)))))


aw_local void xbara_set_signals_connection_pwm1 (uint32_t   base,
                                                 uint16_t   input,
                                                 uint16_t   output)
{
    XBARA_WR_SELx_SELx(base, (((uint16_t)input) & 0xFFU), (((uint16_t)output) & 0xFFU));
}

aw_local aw_const awbl_imx10xx_eflex_pwm_devcfg_t __g_imx1050_eflex_pwm1_devcfg_list[] = {

    /* PWM ID ,��ģ���, ͨ���ţ�PWMģʽ, GPIO, PWM���Ź���, Ĭ�����Ź��� */
    {
        PWM12,
        kPWM_Module_3,
        kPWM_PwmA,
        kPWM_SignedCenterAligned,
        GPIO1_10,
        GPIO1_10_FLEXPWM1_PWMA03,
        __PWM_PIN_DFUNC_CFG,
        105
    },
//    {
//        PWM13,
//        kPWM_Module_3,
//        kPWM_PwmB,
//        kPWM_SignedCenterAligned,
//        GPIO3_1,
//        GPIO3_1_FLEXPWM1_PWMB03,
//        __PWM_PIN_DFUNC_CFG
//    }
};


/* ƽ̨��س�ʼ�� */
aw_local void __imx1050_eflex_pwm1_plfm_init (void)
{

    int pwm_gpios[AW_NELEMENTS(__g_imx1050_eflex_pwm1_devcfg_list)];
    int i;

    /* �������� */
    for (i = 0; i < AW_NELEMENTS(__g_imx1050_eflex_pwm1_devcfg_list); i++) {
        pwm_gpios[i] = __g_imx1050_eflex_pwm1_devcfg_list[i].gpio;
    }

    aw_gpio_pin_request("eflex_pwm1_gpios", pwm_gpios, AW_NELEMENTS(pwm_gpios));

    aw_clk_enable(IMX1050_CLK_CG_XBAR1);
    aw_clk_enable(IMX1050_CLK_CG_PWM1);

    xbara_set_signals_connection_pwm1(IMX1050_XBARA1_BASE, 0x101, 0x123);
    xbara_set_signals_connection_pwm1(IMX1050_XBARA1_BASE, 0x101, 0x124);
    xbara_set_signals_connection_pwm1(IMX1050_XBARA1_BASE, 0x101, 0x125);
    xbara_set_signals_connection_pwm1(IMX1050_XBARA1_BASE, 0x101, 0x126);
}

aw_local awbl_imx10xx_eflex_pwm_devinfo_t __g_imx1050_eflex_pwm1_devinfo = {

    IMX1050_FLEXPWM1_BASE,                    /**< \brief PWM�Ĵ�������ַ */
    IMX1050_CLK_CG_PWM1,                      /**< \brief PWMʱ��ID */
    {
        PWM12,                               /**< \brief PWM��СID�� */
        PWM13                                /**< \brief PWM���ID�� */
    },
    &__g_imx1050_eflex_pwm1_devcfg_list[0],  /**< \brief PWM������Ϣ */

    /** \brief ʹ�õ�PWMͨ���� */
    AW_NELEMENTS(__g_imx1050_eflex_pwm1_devcfg_list),
    __imx1050_eflex_pwm1_plfm_init,          /**< \brief ƽ̨��ʼ�� */
};

/* �豸ʵ���ڴ澲̬���� */
aw_local awbl_imx10xx_eflex_pwm_dev_t __g_imx1050_eflex_pwm1;

#define AWBL_HWCONF_IMX1050_EFLEX_PWM1      \
    {                                       \
        AWBL_IMX10XX_EFLEX_PWM_NAME,        \
        0,                                  \
        AWBL_BUSID_PLB,                     \
        0,                                  \
        &(__g_imx1050_eflex_pwm1.super),    \
        &(__g_imx1050_eflex_pwm1_devinfo)   \
    },

#else

#define AWBL_HWCONF_IMX1050_EFLEX_PWM1

#endif

#endif /* __AWBL_HWCONF_IMX1050_EFLEX_PWM1_H */

