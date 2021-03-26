/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/


#ifndef __AWBL_HWCONF_IMX1050_QTIMER4_CAP_H
#define __AWBL_HWCONF_IMX1050_QTIMER4_CAP_H

#include "driver/cap/awbl_imx10xx_qtimer_cap.h"

#ifdef AW_DEV_IMX1050_QTIMER4_CAP


/* QTimer4 Capture 通道与引脚信息 */
aw_local aw_const awbl_imx10xx_qtimer_cap_devcfg_t __g_imx1050_qtimer4_cap_devcfg_list[] = {

//    /* CAP ID , 通道号,  GPIO, CAP引脚功能, 默认引脚功能 */
//    {
//        QTIMER4_CAP0,
//        kQTMR_CAP_Channel_0,
//        GPIO2_9,
//        GPIO2_9_QTIMER4_TIMER0,
//    },
//    {
//        QTIMER4_CAP1,
//        kQTMR_CAP_Channel_1,
//        GPIO2_10,
//        GPIO2_10_QTIMER4_TIMER1,
//    },
    {
        QTIMER4_CAP2,
        kQTMR_CAP_Channel_2,
        GPIO2_11,
        GPIO2_11_QTIMER4_TIMER2,
    },
    {
        QTIMER4_CAP3,
        kQTMR_CAP_Channel_3,
        GPIO2_27,
        GPIO2_27_QTIMER4_TIMER3,
    },
};

/* QTimer4 Capture 平台初始化 */
aw_local void __imx1050_qtimer4_cap_plfm_init (void)
{
    int cap_gpios[AW_NELEMENTS(__g_imx1050_qtimer4_cap_devcfg_list)];
    int i;

    for (i = 0; i < AW_NELEMENTS(__g_imx1050_qtimer4_cap_devcfg_list); i++) {
        cap_gpios[i] = __g_imx1050_qtimer4_cap_devcfg_list[i].gpio;
    }

    /* 引脚申请与配置 */
    if (aw_gpio_pin_request("qtimer4_pwm_gpios",
                             cap_gpios,
                             AW_NELEMENTS(cap_gpios)) == AW_OK) {

        for (i = 0; i < AW_NELEMENTS(__g_imx1050_qtimer4_cap_devcfg_list); i++) {

            aw_gpio_pin_cfg(cap_gpios[i],
                            __g_imx1050_qtimer4_cap_devcfg_list[i].func);
        }
    }
    aw_clk_enable(IMX1050_CLK_CG_TIMER4);
}

/* QTimer4 Capture 设备信息 */
aw_local awbl_imx10xx_qtimer_cap_devinfo_t __g_imx1050_qtimer4_cap_devinfo = {
    IMX1050_QTIMER4_BASE,                    /**< \brief PWM寄存器基地址 */
    IMX1050_CLK_CG_TIMER4,                     /**< \brief PWM时钟ID */
    INUM_QTIMER4,                            /**< \brief 中断编号 */

    /* 注意: 1.当需要捕获的信号频率比较高时，则需要将预分频值设置小一些，如捕获10MHz的PWM时，
     * 预分频值可以设置为kQTMR_ClockDivide_1;
     *       2.当需要捕获的信号频率较低时，需要将预分频值设置大一些, 如捕获1KHz的PWM时，
     * 预分频值可以设置为kQTMR_ClockDivide_32。
     */
    kQTMR_ClockDivide_32,                       /**< \brief 时钟预分频 */
    &__g_imx1050_qtimer4_cap_devcfg_list[0], /**< \brief PWM配置信息 */
    AW_NELEMENTS(__g_imx1050_qtimer4_cap_devcfg_list), /**< \brief 使用的CAP通道数 */
    {
        QTIMER4_CAP0,                        /**< \brief 最小ID编号 */
        QTIMER4_CAP3,                        /**< \brief 最大ID编号 */
    },
    __imx1050_qtimer4_cap_plfm_init,         /**< \brief 平台初始化 */
};


/* QTimer4 Capture 设备实例内存静态分配 */
aw_local struct awbl_imx10xx_qtimer_cap_dev __g_imx1050_qtimer4_cap_dev;

#define AWBL_HWCONF_IMX1050_QTIMER4_CAP  \
{                                        \
    AWBL_IMX10XX_QTIMER_CAP_NAME,        \
    0,                                   \
    AWBL_BUSID_PLB,                      \
    0,                                   \
    (struct awbl_dev *)&(__g_imx1050_qtimer4_cap_dev), \
    &(__g_imx1050_qtimer4_cap_devinfo)           \
},


#else   /* AWBL_HWCONF_IMX1050_QTIMER4_CAP */

#define AWBL_HWCONF_IMX1050_QTIMER4_CAP

#endif

#endif /* __AWBL_HWCONF_IMX1050_QTIMER4_CAP_H */
