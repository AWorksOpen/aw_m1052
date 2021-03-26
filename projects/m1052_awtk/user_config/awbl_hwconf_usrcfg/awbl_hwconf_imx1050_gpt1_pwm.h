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
 * \brief AWBus-lite iMX1050 GPT1 PWM 配置文件
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

/** \brief PWM禁能时GPIO的配置 */
#define __PWM_PIN_DFUNC_CFG   (IMX1050_PIN_MUX(PIN_MUX_ALT5) | AW_GPIO_OUTPUT)

aw_local aw_const awbl_imx10xx_gpt_pwm_devcfg_t __g_imx1050_gpt1_pwm_devcfg_list[] = {

    /* PWM ID , 通道号,  GPIO, PWM引脚功能, 默认引脚功能 */
    {
        PWM14,
        GPT_Channel_0,
        GPIO3_21,
        GPIO3_21_GPT1_COMPARE1,
        __PWM_PIN_DFUNC_CFG
    }
    /* 受gpt硬件特性限制，通道0只能输出占空比50%的PWM波，其他通道不适合用来输出PWM波 */
};


/* 平台相关初始化 */
aw_local void __imx1050_gpt1_pwm_plfm_init (void)
{
    int pwm_gpios[AW_NELEMENTS(__g_imx1050_gpt1_pwm_devcfg_list)];
    int i;

    /* 引脚申请 */
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
    IMX1050_GPT1_BASE,                      /**< \brief PWM寄存器基地址 */
    INUM_GPT1,                              /**< \brief PWM中断号 */
    IMX1050_CLK_CG_GPT_SERIAL,              /**< \brief PWM时钟ID */
    {
        PWM8,                               /**< \brief PWM最小ID号 */
        PWM8                                /**< \brief PWM最大ID号 */
    },
    &__g_imx1050_gpt1_pwm_devcfg_list[0],   /**< \brief PWM配置信息 */

    /**< \brief 使用的PWM通道数 */
    AW_NELEMENTS(__g_imx1050_gpt1_pwm_devcfg_list),
    __imx1050_gpt1_pwm_plfm_init,           /**< \brief 平台初始化 */
};

/* 设备实例内存静态分配 */
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
