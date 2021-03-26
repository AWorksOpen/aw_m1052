/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

#ifndef __AWBL_HWCONF_IMX1050_ADC1_H
#define __AWBL_HWCONF_IMX1050_ADC1_H

#ifdef AW_DEV_IMX1050_ADC1

#include "driver/adc/awbl_imx10xx_adc.h"

aw_local void __imx1050_adc1_plfm_init (void)
{
    int i = 0;

    aw_local aw_const int adc1_gpios[] = {

        GPIO1_27,      /* channel 0 */
       //GPIO1_12,     /* channel 1 */
       //GPIO1_13,     /* channel 2 */
       GPIO1_14,       /* channel 3 */
       GPIO1_15,       /* channel 4 */
       //GPIO1_16,     /* channel 5 */
       //GPIO1_17,     /* channel 6 */
       //GPIO1_18,     /* channel 7 */
       //GPIO1_19,     /* channel 8 */
       //GPIO1_20,     /* channel 9 */
       //GPIO1_21,     /* channel 10 */
       //GPIO1_22,     /* channel 11 */
       //GPIO1_23,     /* channel 12 */
       //GPIO1_24,     /* channel 13 */
       //GPIO1_25,     /* channel 14 */
       //GPIO1_26,     /* channel 15 */
    };


    if (aw_gpio_pin_request("adc1_gpios",
                             adc1_gpios,
                             AW_NELEMENTS(adc1_gpios)) == AW_OK) {

        for (i = 0; i <  AW_NELEMENTS(adc1_gpios); i++) {

            /* 配置AD引脚 */
            aw_gpio_pin_cfg(adc1_gpios[i], IMX1050_PAD_CTL(0));
        }
    }

    aw_clk_enable(IMX1050_CLK_CG_ADC1);
};


/* 设备信息 */
aw_local aw_const struct awbl_imx10xx_adc_devinfo __g_imx1050_adc1_devinfo = {
    {
        IMX10xx_ADC_MIN_CH,        /* 最小通道号 */
        IMX10xx_ADC_MAX_CH         /* 最大通道号 */
    },
    IMX1050_ADC1_BASE,             /* ADC1寄存器基址 */
    INUM_ADC1,                     /* 中断号 */
    12,                            /* 采样位数 */
    3300,                          /* 参考电压 */
    IMX1050_CLK_CG_ADC1,           /* 时钟ID */
    __imx1050_adc1_plfm_init
};

/* 设备实例内存静态分配 */
aw_local struct awbl_imx10xx_adc_dev __g_imx1050_adc1_dev;

#define AWBL_HWCONF_IMX1050_ADC1                 \
{                                                \
    AWBL_IMX10XX_ADC_NAME,                       \
    1,                                           \
    AWBL_BUSID_PLB,                              \
    0,                                           \
    (struct awbl_dev *)&__g_imx1050_adc1_dev,    \
    &(__g_imx1050_adc1_devinfo)                  \
},

#else
#define AWBL_HWCONF_IMX1050_ADC1
#endif  /* AW_DEV_IMX1050_ADC1 */

#endif /* __AWBL_HWCONF_IMX1050_ADC1_H */

/* end of file */
