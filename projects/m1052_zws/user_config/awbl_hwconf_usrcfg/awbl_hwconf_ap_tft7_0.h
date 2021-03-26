/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

#ifndef __AWBL_HWCONF_AP_TFT7_0_H
#define __AWBL_HWCONF_AP_TFT7_0_H

#ifdef AW_DEV_AP_TFT7_0
/*******************************************************************************
  HW800480 配置信息
*******************************************************************************/
#include "driver/display/awbl_imx1050_tft_panel.h"


#define __AP_TFT7_0_V1_00  0
#define __AP_TFT7_0_V1_01  1

#define __TFT_VERSION   __AP_TFT7_0_V1_01

#define __HW800480_X_RES        800
#define __HW800480_Y_RES        480
#define __HW800480_BUS_BPP      16
#define __HW800480_WORD_LBPP    16

#if __TFT_VERSION == __AP_TFT7_0_V1_00
#define __HW800480_DCLK_F       48000000//32000000
#endif

#if __TFT_VERSION == __AP_TFT7_0_V1_01
#define __HW800480_DCLK_F       48000000
#endif

#define DOTCLK_H_ACTIVE         __HW800480_X_RES
#if __TFT_VERSION == __AP_TFT7_0_V1_00
#define DOTCLK_H_PULSE_WIDTH    48//48
#define DOTCLK_HF_PORCH         1000//300
#define DOTCLK_HB_PORCH         40//40
#endif
#if __TFT_VERSION == __AP_TFT7_0_V1_01
#define DOTCLK_H_PULSE_WIDTH    1
#define DOTCLK_HF_PORCH         1000
#define DOTCLK_HB_PORCH         46
#endif

#define DOTCLK_V_ACTIVE         __HW800480_Y_RES
#if __TFT_VERSION == __AP_TFT7_0_V1_00
#define DOTCLK_V_PULSE_WIDTH    3
#define DOTCLK_VF_PORCH         13
#define DOTCLK_VB_PORCH         29
#endif
#if __TFT_VERSION == __AP_TFT7_0_V1_01
#define DOTCLK_V_PULSE_WIDTH    1
#define DOTCLK_VF_PORCH         22
#define DOTCLK_VB_PORCH         23
#endif

typedef struct __hw800480f_gpio_cfg {
    int         gpio;
    uint32_t    func;

} __hw800480f_gpio_cfg_t;


aw_local aw_const  __hw800480f_gpio_cfg_t __g_hw800480f_gpios_list[] = {

    {GPIO1_10, AW_GPIO_OUTPUT_INIT_HIGH_VAL},  /* 背光控制,当使用PWM时将这句注释掉 */
    {GPIO2_0,  GPIO2_0_LCD_CLK},         /* LCD clk */
    {GPIO2_1,  GPIO2_1_LCD_ENABLE},      /* LCD enable */
    {GPIO2_2,  GPIO2_2_LCD_HSYNC},       /* LCD hsync */
    {GPIO2_3,  GPIO2_3_LCD_VSYNC},       /* LCD vsync */

#if (__HW800480_BUS_BPP == 16)
    //B
    {GPIO2_4,  GPIO2_4_LCD_DATA00},      /* LCD data0 */
    {GPIO2_5,  GPIO2_5_LCD_DATA01},      /* LCD data1 */
    {GPIO2_6,  GPIO2_6_LCD_DATA02},      /* LCD data2 */
    {GPIO2_7,  GPIO2_7_LCD_DATA03},      /* LCD data3 */
    {GPIO2_8,  GPIO2_8_LCD_DATA04},      /* LCD data4 */
    //G
    {GPIO2_9,  GPIO2_9_LCD_DATA05},      /* LCD data5 */
    {GPIO2_10, GPIO2_10_LCD_DATA06},     /* LCD data6 */
    {GPIO2_11, GPIO2_11_LCD_DATA07},     /* LCD data7 */
    {GPIO2_12, GPIO2_12_LCD_DATA08},     /* LCD data8 */
    {GPIO2_13, GPIO2_13_LCD_DATA09},     /* LCD data9 */
    {GPIO2_14, GPIO2_14_LCD_DATA10},     /* LCD data10 */
    //R
    {GPIO2_15, GPIO2_15_LCD_DATA11},     /* LCD data11 */
    {GPIO2_16, GPIO2_16_LCD_DATA12},     /* LCD data12 */
    {GPIO2_17, GPIO2_17_LCD_DATA13},     /* LCD data13 */
    {GPIO2_18, GPIO2_18_LCD_DATA14},     /* LCD data14 */
    {GPIO2_19, GPIO2_19_LCD_DATA15},     /* LCD data15 */
#endif

#if (__HW800480_BUS_BPP == 24)
    //B
    {GPIO2_4,  GPIO2_4_LCD_DATA00},/* LCD data0 */
    {GPIO2_5,  GPIO2_5_LCD_DATA01},/* LCD data1 */
    {GPIO2_6,  GPIO2_6_LCD_DATA02},/* LCD data2 */
    {GPIO2_7,  GPIO2_7_LCD_DATA03},/* LCD data3 */
    {GPIO2_8,  GPIO2_8_LCD_DATA04},/* LCD data4 */
    {GPIO2_9,  GPIO2_9_LCD_DATA05},/* LCD data5 */
    {GPIO2_10, GPIO2_10_LCD_DATA06},/* LCD data6 */
    {GPIO2_11, GPIO2_11_LCD_DATA07},/* LCD data7 */
    //G
    {GPIO2_12, GPIO2_12_LCD_DATA08},/* LCD data8 */
    {GPIO2_13, GPIO2_13_LCD_DATA09},/* LCD data9 */
    {GPIO2_14, GPIO2_14_LCD_DATA10},/* LCD data10 */
    {GPIO2_15, GPIO2_15_LCD_DATA11},/* LCD data11 */
    {GPIO2_16, GPIO2_16_LCD_DATA12},/* LCD data12 */
    {GPIO2_17, GPIO2_17_LCD_DATA13},/* LCD data13 */
    {GPIO2_18, GPIO2_18_LCD_DATA14},/* LCD data14 */
    {GPIO2_19, GPIO2_19_LCD_DATA15},/* LCD data15 */
    //R
    {GPIO2_20, GPIO2_20_LCD_DATA16},/* LCD data16 */
    {GPIO2_21, GPIO2_21_LCD_DATA17},/* LCD data17 */
    {GPIO2_22, GPIO2_22_LCD_DATA18},/* LCD data18 */
    {GPIO2_23, GPIO2_23_LCD_DATA19},/* LCD data19 */
    {GPIO2_24, GPIO2_24_LCD_DATA20},/* LCD data20 */
    {GPIO2_25, GPIO2_25_LCD_DATA21},/* LCD data21 */
    {GPIO2_26, GPIO2_26_LCD_DATA22},/* LCD data22 */
    {GPIO2_27, GPIO2_27_LCD_DATA23},/* LCD data23 */
#endif
};

aw_local void __hw800480_plfm_init (void)
{
    int i = 0;
    int lcd_gpios[AW_NELEMENTS(__g_hw800480f_gpios_list)];

    for (i = 0; i < AW_NELEMENTS(__g_hw800480f_gpios_list); i++) {
        lcd_gpios[i] = __g_hw800480f_gpios_list[i].gpio;
    }

    if (aw_gpio_pin_request("lcd_gpios",
                             lcd_gpios,
                             AW_NELEMENTS(lcd_gpios)) == AW_OK) {

        for (i = 0; i < AW_NELEMENTS(__g_hw800480f_gpios_list); i++) {
            /* 配置功能引脚 */
            aw_gpio_pin_cfg(__g_hw800480f_gpios_list[i].gpio,
                            __g_hw800480f_gpios_list[i].func);
        }
    }
}

/* 设备信息 */
aw_local aw_const awbl_imx1050_tft_panel_devinfo_t  __g_hw800480f_param = {
    /* panel_info */
    {
        __HW800480_X_RES,               /* 屏幕宽度 */
        __HW800480_Y_RES,               /* 屏幕高度 */
        __HW800480_BUS_BPP,             /* rgb硬件总线数 */
        __HW800480_WORD_LBPP,           /* 一个像素的长度，位为单位,如rgb565数据，该值为16 */
        __HW800480_DCLK_F,              /* 时钟频率 */
        AWBL_MXS_LCDIF_PANEL_DOTCLK,    /* 时钟源类型 */
        99,                             /* 默认的背光亮度 */
    },
    /* timing */
    {
        DOTCLK_H_PULSE_WIDTH,           /* h_pulse_width */
        DOTCLK_H_ACTIVE,                /* h_active */
        DOTCLK_HB_PORCH,                /* h_hb_porch */
        DOTCLK_HF_PORCH,                /* h_hf_porch */
        DOTCLK_V_PULSE_WIDTH,           /* v_pulse_width */
        DOTCLK_V_ACTIVE,                /* v_active */
        DOTCLK_VB_PORCH,                /* h_vb_porch */
        DOTCLK_VF_PORCH,                /* h_vf_porch */
        0,                              /* enable_present */
    },
    /* plfm_init */
    __hw800480_plfm_init
};

/* 设备实例内存静态分配 */
aw_local awbl_imx1050_tft_panel_t __g_hw800480f;

#define AWBL_HWCONF_AP_TFT7_0       \
    {                               \
        AWBL_IMX1050_TFT_PANEL_NAME,\
        0,                          \
        AWBL_BUSID_PLB,             \
        0,                          \
        &(__g_hw800480f.dev),       \
        &(__g_hw800480f_param)      \
    },

#else
#define AWBL_HWCONF_AP_TFT7_0

#endif  /* AWBL_HWCONF_AP_TFT7_0 */

#endif  /* __AWBL_HWCONF_AP_TFT7_0_H */

/* end of file */
