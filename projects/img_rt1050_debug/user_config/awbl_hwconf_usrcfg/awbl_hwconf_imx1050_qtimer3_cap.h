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


#ifndef __AWBL_HWCONF_IMX1050_QTIMER3_CAP_H
#define __AWBL_HWCONF_IMX1050_QTIMER3_CAP_H

#include "driver/cap/awbl_imx10xx_qtimer_cap.h"

#ifdef AW_DEV_IMX1050_QTIMER3_CAP


/* QTimer3 Capture ͨ����������Ϣ */
aw_local aw_const awbl_imx10xx_qtimer_cap_devcfg_t __g_imx1050_qtimer3_cap_devcfg_list[] = {

//    /* CAP ID , ͨ����,  GPIO, CAP���Ź���, Ĭ�����Ź��� */
//    {
//        QTIMER3_CAP0,
//        kQTMR_CAP_Channel_0,
//        GPIO1_16,
//        GPIO1_16_QTIMER3_TIMER0,
//    },
//    {
//        QTIMER3_CAP1,
//        kQTMR_CAP_Channel_1,
//        GPIO1_17,
//        GPIO1_17_QTIMER3_TIMER1,
//    },
    {
        QTIMER3_CAP2,
        kQTMR_CAP_Channel_2,
        GPIO1_18,
        GPIO1_18_QTIMER3_TIMER2,
    },
    {
        QTIMER3_CAP3,
        kQTMR_CAP_Channel_3,
        GPIO1_19,
        GPIO1_19_QTIMER3_TIMER3,
    },
};

/* QTimer3 Capture ƽ̨��ʼ�� */
aw_local void __imx1050_qtimer3_cap_plfm_init (void)
{
    int cap_gpios[AW_NELEMENTS(__g_imx1050_qtimer3_cap_devcfg_list)];
    int i;

    for (i = 0; i < AW_NELEMENTS(__g_imx1050_qtimer3_cap_devcfg_list); i++) {
        cap_gpios[i] = __g_imx1050_qtimer3_cap_devcfg_list[i].gpio;
    }

    /* �������������� */
    if (aw_gpio_pin_request("qtimer3_pwm_gpios",
                             cap_gpios,
                             AW_NELEMENTS(cap_gpios)) == AW_OK) {

        for (i = 0; i < AW_NELEMENTS(__g_imx1050_qtimer3_cap_devcfg_list); i++) {

            aw_gpio_pin_cfg(cap_gpios[i],
                            __g_imx1050_qtimer3_cap_devcfg_list[i].func);
        }
    }
    aw_clk_enable(IMX1050_CLK_CG_TIMER3);
}

/* QTimer3 Capture �豸��Ϣ */
aw_local awbl_imx10xx_qtimer_cap_devinfo_t __g_imx1050_qtimer3_cap_devinfo = {
    IMX1050_QTIMER3_BASE,                    /**< \brief PWM�Ĵ�������ַ */
    IMX1050_CLK_CG_TIMER3,                     /**< \brief PWMʱ��ID */
    INUM_QTIMER3,                            /**< \brief �жϱ�� */

    /* ע��: 1.����Ҫ������ź�Ƶ�ʱȽϸ�ʱ������Ҫ��Ԥ��Ƶֵ����СһЩ���粶��10MHz��PWMʱ��
     * Ԥ��Ƶֵ��������ΪkQTMR_ClockDivide_1;
     *       2.����Ҫ������ź�Ƶ�ʽϵ�ʱ����Ҫ��Ԥ��Ƶֵ���ô�һЩ, �粶��1KHz��PWMʱ��
     * Ԥ��Ƶֵ��������ΪkQTMR_ClockDivide_32��
     */
    kQTMR_ClockDivide_32,                       /**< \brief ʱ��Ԥ��Ƶ */
    &__g_imx1050_qtimer3_cap_devcfg_list[0], /**< \brief PWM������Ϣ */
    AW_NELEMENTS(__g_imx1050_qtimer3_cap_devcfg_list), /**< \brief ʹ�õ�CAPͨ���� */
    {
        QTIMER3_CAP0,                        /**< \brief ��СID��� */
        QTIMER3_CAP3,                        /**< \brief ���ID��� */
    },
    __imx1050_qtimer3_cap_plfm_init,         /**< \brief ƽ̨��ʼ�� */
};


/* QTimer3 Capture �豸ʵ���ڴ澲̬���� */
aw_local struct awbl_imx10xx_qtimer_cap_dev __g_imx1050_qtimer3_cap_dev;

#define AWBL_HWCONF_IMX1050_QTIMER3_CAP  \
{                                        \
    AWBL_IMX10XX_QTIMER_CAP_NAME,        \
    0,                                   \
    AWBL_BUSID_PLB,                      \
    0,                                   \
    (struct awbl_dev *)&(__g_imx1050_qtimer3_cap_dev), \
    &(__g_imx1050_qtimer3_cap_devinfo)           \
},


#else   /* AWBL_HWCONF_IMX1050_QTIMER3_CAP */

#define AWBL_HWCONF_IMX1050_QTIMER3_CAP

#endif

#endif /* __AWBL_HWCONF_IMX1050_QTIMER3_CAP_H */
