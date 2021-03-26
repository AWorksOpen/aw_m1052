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


#ifndef __AWBL_HWCONF_IMX1050_QTIMER4_TIMER0_H
#define __AWBL_HWCONF_IMX1050_QTIMER4_TIMER0_H

#include "driver/timer/awbl_imx10xx_qtimer_timer.h"

#ifdef AW_DEV_IMX1050_QTIMER4_TIMER0

/* QTimer4 Timer 平台初始化 */
aw_local void __imx1050_qtimer4_timer_plfm_init (void)
{
    int input_gpios[] = {GPIO2_9};

    /* 引脚申请 */
    if (aw_gpio_pin_request("input_gpios",
                            input_gpios,
                            AW_NELEMENTS(input_gpios)) == AW_OK) {
        aw_gpio_pin_cfg(GPIO2_9, GPIO2_9_QTIMER4_TIMER0);
    }
    aw_clk_enable(IMX1050_CLK_CG_TIMER4);
}

/* QTimer4 Timer0 设备信息 */
aw_local awbl_imx10xx_qtimer_timer_devinfo_t __g_imx1050_qtimer4_timer0_devinfo =
{
    1,                                      /**< \brief 通过名称分配定时器 */
    INUM_QTIMER4,                           /**< \brief 中断编号 */
    IMX1050_QTIMER4_BASE,                   /**< \brief 寄存器基地址 */
    IMX1050_CLK_CG_TIMER4,                  /**< \brief 时钟ID */
    QTimer_ClockDivide_128,                 /**< \brief 时钟预分频系数 */
    kQTMR_Timer_Channel_0,                  /**< \brief 通道号 */
    __imx1050_qtimer4_timer_plfm_init,      /**< \brief 平台初始化 */
    QTimer_Count_Mode_Off                   /**< \brief 输入计数模式开关 */
};

/* QTimer4 Timer0 设备实例内存静态分配 */
aw_local struct awbl_imx10xx_qtimer_timer_dev __g_imx1050_qtimer4_timer0_dev;

#define AWBL_HWCONF_IMX1050_QTIMER4_TIMER0                \
{                                                         \
    AWBL_IMX10XX_QTIMER_TIMER_NAME,                       \
    IMX1050_QTIMER4_BUSID,                                \
    AWBL_BUSID_PLB,                                       \
    0,                                                    \
    (struct awbl_dev *)&(__g_imx1050_qtimer4_timer0_dev), \
    &(__g_imx1050_qtimer4_timer0_devinfo)                 \
},

#else   /* AW_DEV_IMX1050_QTIMER4_TIMER0 */

#define AWBL_HWCONF_IMX1050_QTIMER4_TIMER0

#endif

#endif /* __AWBL_HWCONF_IMX1050_QTIMER4_TIMER0_H */
