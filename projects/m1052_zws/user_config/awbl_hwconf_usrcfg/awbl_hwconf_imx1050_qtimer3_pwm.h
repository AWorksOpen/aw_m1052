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


#ifndef __AWBL_HWCONF_IMX1050_QTIMER3_PWM_H
#define __AWBL_HWCONF_IMX1050_QTIMER3_PWM_H

#include "driver/pwm/awbl_imx10xx_qtimer_pwm.h"

#ifdef AW_DEV_IMX1050_QTIMER3_PWM

/** \brief PWM����ʱGPIO������ */
#define __PWM_PIN_DFUNC_CFG   (IMX1050_PIN_MUX(PIN_MUX_ALT5) | AW_GPIO_OUTPUT)

aw_local aw_const awbl_imx10xx_qtimer_pwm_devcfg_t __g_imx1050_qtimer3_pwm_devcfg_list[] = {

    /* PWM ID , ͨ����,  GPIO, PWM���Ź���, Ĭ�����Ź��� */
    /* PWM8 �� PWM9��ʹ�õ� GPIO1_16 �� GPIO1_17������
     * i2c1�������ų�ͻ�����ʹ��PWM8����PWM9��ر�I2C1�����
     */
//    {
//        PWM8,
//        kQTMR_Channel_0,
//        GPIO1_16,
//        GPIO1_16_QTIMER3_TIMER0,
//        __PWM_PIN_DFUNC_CFG
//    },
//    {
//        PWM9,
//        kQTMR_Channel_1,
//        GPIO1_17,
//        GPIO1_17_QTIMER3_TIMER1,
//        __PWM_PIN_DFUNC_CFG
//    },
    {
        PWM10,
        kQTMR_Channel_2,
        GPIO1_18,
        GPIO1_18_QTIMER3_TIMER2,
        __PWM_PIN_DFUNC_CFG
    },
//    {
//        PWM11,
//        kQTMR_Channel_3,
//        GPIO1_19,
//        GPIO1_19_QTIMER3_TIMER3,
//        __PWM_PIN_DFUNC_CFG
//    },
};


/* ƽ̨��س�ʼ�� */
aw_local void __imx1050_qtimer3_pwm_plfm_init (void)
{
    int pwm_gpios[AW_NELEMENTS(__g_imx1050_qtimer3_pwm_devcfg_list)];
    int i;

    /* �������� */
    for (i = 0; i < AW_NELEMENTS(__g_imx1050_qtimer3_pwm_devcfg_list); i++) {
        pwm_gpios[i] = __g_imx1050_qtimer3_pwm_devcfg_list[i].gpio;
    }

    aw_gpio_pin_request("qtimer3_pwm_gpios", pwm_gpios, AW_NELEMENTS(pwm_gpios));
    aw_clk_enable(IMX1050_CLK_CG_TIMER3);
}

aw_local aw_const awbl_imx10xx_qtimer_pwm_devinfo_t __g_imx1050_qtimer3_pwm_devinfo = {
    IMX1050_QTIMER3_BASE,                     /**< \brief PWM�Ĵ�������ַ */
    INUM_QTIMER3,                             /**< \brief PWM�жϺ� */
    IMX1050_CLK_CG_TIMER3,                    /**< \brief PWMʱ��ID */
    {
        PWM8,                                 /**< \brief PWM��СID�� */
        PWM11                                 /**< \brief PWM���ID�� */
    },
    &__g_imx1050_qtimer3_pwm_devcfg_list[0],  /**< \brief PWM������Ϣ */

    /**< \brief ʹ�õ�PWMͨ���� */
    AW_NELEMENTS(__g_imx1050_qtimer3_pwm_devcfg_list),
    __imx1050_qtimer3_pwm_plfm_init,          /**< \brief ƽ̨��ʼ�� */
};

/* �豸ʵ���ڴ澲̬���� */
aw_local awbl_imx10xx_qtimer_pwm_dev_t __g_imx1050_qtimer3_pwm;

#define AWBL_HWCONF_IMX1050_QTIMER3_PWM      \
    {                                        \
        AWBL_IMX10XX_QTIMER_PWM_NAME,        \
        0,                                   \
        AWBL_BUSID_PLB,                      \
        0,                                   \
        &(__g_imx1050_qtimer3_pwm.super),    \
        &(__g_imx1050_qtimer3_pwm_devinfo)   \
    },

#else

#define AWBL_HWCONF_IMX1050_QTIMER3_PWM

#endif

#endif /* __AWBL_HWCONF_IMX1050_QTIMER3_PWM_H */
