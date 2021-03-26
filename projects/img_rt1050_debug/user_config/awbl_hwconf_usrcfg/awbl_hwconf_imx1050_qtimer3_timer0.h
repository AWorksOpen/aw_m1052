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


#ifndef __AWBL_HWCONF_IMX1050_QTIMER3_TIMER0_H
#define __AWBL_HWCONF_IMX1050_QTIMER3_TIMER0_H

#include "driver/timer/awbl_imx10xx_qtimer_timer.h"

#ifdef AW_DEV_IMX1050_QTIMER3_TIMER0

/* QTimer3 Timer ƽ̨��ʼ�� */
aw_local void __imx1050_qtimer3_timer_plfm_init (void)
{
    int input_gpios[] = {GPIO1_16};

    /* �������� */
    if (aw_gpio_pin_request("input_gpios",
                            input_gpios,
                            AW_NELEMENTS(input_gpios)) == AW_OK) {
        aw_gpio_pin_cfg(GPIO1_16, GPIO1_16_QTIMER3_TIMER0);
    }
    aw_clk_enable(IMX1050_CLK_CG_TIMER3);
}

/* QTimer3 Timer0 �豸��Ϣ */
aw_local awbl_imx10xx_qtimer_timer_devinfo_t __g_imx1050_qtimer3_timer0_devinfo =
{
    1,                                      /**< \brief ͨ�����Ʒ��䶨ʱ�� */
    INUM_QTIMER3,                           /**< \brief �жϱ�� */
    IMX1050_QTIMER3_BASE,                   /**< \brief �Ĵ�������ַ */
    IMX1050_CLK_CG_TIMER3,                  /**< \brief ʱ��ID */
    QTimer_ClockDivide_128,                 /**< \brief ʱ��Ԥ��Ƶϵ�� */
    kQTMR_Timer_Channel_0,                  /**< \brief ͨ���� */
    __imx1050_qtimer3_timer_plfm_init,      /**< \brief ƽ̨��ʼ�� */
    QTimer_Count_Mode_Off                   /**< \brief �������ģʽ���� */
};

/* QTimer3 Timer0 �豸ʵ���ڴ澲̬���� */
aw_local struct awbl_imx10xx_qtimer_timer_dev __g_imx1050_qtimer3_timer0_dev;

#define AWBL_HWCONF_IMX1050_QTIMER3_TIMER0                \
{                                                         \
    AWBL_IMX10XX_QTIMER_TIMER_NAME,                       \
    IMX1050_QTIMER3_BUSID,                                \
    AWBL_BUSID_PLB,                                       \
    0,                                                    \
    (struct awbl_dev *)&(__g_imx1050_qtimer3_timer0_dev), \
    &(__g_imx1050_qtimer3_timer0_devinfo)                 \
},

#else   /* AW_DEV_IMX1050_QTIMER3_TIMER0 */

#define AWBL_HWCONF_IMX1050_QTIMER3_TIMER0

#endif

#endif /* __AWBL_HWCONF_IMX1050_QTIMER3_TIMER0_H */
