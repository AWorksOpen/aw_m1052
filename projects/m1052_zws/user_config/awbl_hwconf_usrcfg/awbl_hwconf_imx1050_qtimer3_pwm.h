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

/** \brief PWM禁能时GPIO的配置 */
#define __PWM_PIN_DFUNC_CFG   (IMX1050_PIN_MUX(PIN_MUX_ALT5) | AW_GPIO_OUTPUT)

aw_local aw_const awbl_imx10xx_qtimer_pwm_devcfg_t __g_imx1050_qtimer3_pwm_devcfg_list[] = {

    /* PWM ID , 通道号,  GPIO, PWM引脚功能, 默认引脚功能 */
    /* PWM8 和 PWM9中使用的 GPIO1_16 和 GPIO1_17引脚与
     * i2c1外设引脚冲突，如果使用PWM8或者PWM9请关闭I2C1外设宏
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


/* 平台相关初始化 */
aw_local void __imx1050_qtimer3_pwm_plfm_init (void)
{
    int pwm_gpios[AW_NELEMENTS(__g_imx1050_qtimer3_pwm_devcfg_list)];
    int i;

    /* 引脚申请 */
    for (i = 0; i < AW_NELEMENTS(__g_imx1050_qtimer3_pwm_devcfg_list); i++) {
        pwm_gpios[i] = __g_imx1050_qtimer3_pwm_devcfg_list[i].gpio;
    }

    aw_gpio_pin_request("qtimer3_pwm_gpios", pwm_gpios, AW_NELEMENTS(pwm_gpios));
    aw_clk_enable(IMX1050_CLK_CG_TIMER3);
}

aw_local aw_const awbl_imx10xx_qtimer_pwm_devinfo_t __g_imx1050_qtimer3_pwm_devinfo = {
    IMX1050_QTIMER3_BASE,                     /**< \brief PWM寄存器基地址 */
    INUM_QTIMER3,                             /**< \brief PWM中断号 */
    IMX1050_CLK_CG_TIMER3,                    /**< \brief PWM时钟ID */
    {
        PWM8,                                 /**< \brief PWM最小ID号 */
        PWM11                                 /**< \brief PWM最大ID号 */
    },
    &__g_imx1050_qtimer3_pwm_devcfg_list[0],  /**< \brief PWM配置信息 */

    /**< \brief 使用的PWM通道数 */
    AW_NELEMENTS(__g_imx1050_qtimer3_pwm_devcfg_list),
    __imx1050_qtimer3_pwm_plfm_init,          /**< \brief 平台初始化 */
};

/* 设备实例内存静态分配 */
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
