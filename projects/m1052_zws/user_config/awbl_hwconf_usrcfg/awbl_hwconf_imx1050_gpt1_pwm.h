/*******************************************************************************
*                                 AnyWhere
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      anywhere.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief AWBus-lite iMX1050 GPT1 PWM �����ļ�
 *
 * \internal
 * \par modification history
 * - 1.00 2020-09-11  licl, first implementation
 * \endinternal
 */

#ifndef __AWBL_HWCONF_IMX1050_GPT1_PWM_H
#define __AWBL_HWCONF_IMX1050_GPT1_PWM_H

#include "driver/pwm/awbl_imx10xx_gpt_pwm.h"

#ifdef AW_DEV_IMX1050_GPT1_PWM

/** \brief PWM����ʱGPIO������ */
#define __PWM_PIN_DFUNC_CFG   (IMX1050_PIN_MUX(PIN_MUX_ALT5) | AW_GPIO_OUTPUT)

aw_local aw_const awbl_imx10xx_gpt_pwm_devcfg_t __g_imx1050_gpt1_pwm_devcfg_list[] = {

    /* PWM ID , ͨ����,  GPIO, PWM���Ź���, Ĭ�����Ź��� */
    {
        PWM14,
        GPT_Channel_0,
        GPIO3_21,
        GPIO3_21_GPT1_COMPARE1,
        __PWM_PIN_DFUNC_CFG
    }
    /* ��gptӲ���������ƣ�ͨ��0ֻ�����ռ�ձ�50%��PWM��������ͨ�����ʺ��������PWM�� */
};


/* ƽ̨��س�ʼ�� */
aw_local void __imx1050_gpt1_pwm_plfm_init (void)
{
    int pwm_gpios[AW_NELEMENTS(__g_imx1050_gpt1_pwm_devcfg_list)];
    int i;

    /* �������� */
    for (i = 0; i < AW_NELEMENTS(__g_imx1050_gpt1_pwm_devcfg_list); i++) {
        pwm_gpios[i] = __g_imx1050_gpt1_pwm_devcfg_list[i].gpio;
    }
    aw_gpio_pin_request("GPT1_pwm_gpios", pwm_gpios, AW_NELEMENTS(pwm_gpios));
    for (i = 0; i < AW_NELEMENTS(__g_imx1050_gpt1_pwm_devcfg_list); i++) {
        aw_gpio_pin_cfg(pwm_gpios[i], __g_imx1050_gpt1_pwm_devcfg_list[i].func);
    }

    aw_clk_enable(IMX1050_CLK_CG_GPT_SERIAL);
}

aw_local aw_const awbl_imx10xx_gpt_pwm_devinfo_t __g_imx1050_gpt1_pwm_devinfo = {
    IMX1050_GPT1_BASE,                      /**< \brief PWM�Ĵ�������ַ */
    INUM_GPT1,                              /**< \brief PWM�жϺ� */
    IMX1050_CLK_CG_GPT_SERIAL,              /**< \brief PWMʱ��ID */
    {
        PWM8,                               /**< \brief PWM��СID�� */
        PWM8                                /**< \brief PWM���ID�� */
    },
    &__g_imx1050_gpt1_pwm_devcfg_list[0],   /**< \brief PWM������Ϣ */

    /**< \brief ʹ�õ�PWMͨ���� */
    AW_NELEMENTS(__g_imx1050_gpt1_pwm_devcfg_list),
    __imx1050_gpt1_pwm_plfm_init,           /**< \brief ƽ̨��ʼ�� */
};

/* �豸ʵ���ڴ澲̬���� */
aw_local awbl_imx10xx_gpt_pwm_dev_t __g_imx1050_gpt1_pwm;

#define AWBL_HWCONF_IMX1050_GPT1_PWM        \
    {                                       \
        AWBL_IMX10XX_GPT_PWM_NAME,          \
        0,                                  \
        AWBL_BUSID_PLB,                     \
        0,                                  \
        &(__g_imx1050_gpt1_pwm.super),      \
        &(__g_imx1050_gpt1_pwm_devinfo)     \
    },

#else

#define AWBL_HWCONF_IMX1050_GPT1_PWM

#endif

#endif /* __AWBL_HWCONF_IMX1050_GPT1_PWM_H */
