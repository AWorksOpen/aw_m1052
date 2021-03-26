/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief PM100功率计前面板资源定义
 *
 * \internal
 * \par modification history:
 * - 1.00 14-12-05  ops, first implementation.
 * \endinternal
 */
#ifndef AWBL_PANEL_PM1000_DEF_H_
#define AWBL_PANEL_PM1000_DEF_H_

#include "aw_input_code.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief PM1000 LED Resource
 */
#define LED_RES_ID           (20)

#define LED1_m               (LED_RES_ID + 0 )
#define LED2_k               (LED_RES_ID + 1 )
#define LED3_M               (LED_RES_ID + 2 )
#define LED4_V               (LED_RES_ID + 3 )
#define LED5_A               (LED_RES_ID + 4 )
#define LED6_W               (LED_RES_ID + 5 )
#define LED7_VA              (LED_RES_ID + 6 )
#define LED8_var             (LED_RES_ID + 7 )
#define LED9_TIME            (LED_RES_ID + 8 )
#define LED10_m              (LED_RES_ID + 9 )
#define LED11_k              (LED_RES_ID + 10)
#define LED12_M              (LED_RES_ID + 11)
#define LED13_V              (LED_RES_ID + 12)
#define LED14_A              (LED_RES_ID + 13)
#define LED15_W              (LED_RES_ID + 14)
#define LED16_PF             (LED_RES_ID + 15)
#define LED17_O              (LED_RES_ID + 16)
#define LED18_pk             (LED_RES_ID + 17)
#define LED19_m              (LED_RES_ID + 18)
#define LED20_k              (LED_RES_ID + 19)
#define LED21_M              (LED_RES_ID + 20)
#define LED22_V              (LED_RES_ID + 21)
#define LED23_A              (LED_RES_ID + 22)
#define LED24_W              (LED_RES_ID + 23)
#define LED25_Hz             (LED_RES_ID + 24)
#define LED26_h              (LED_RES_ID + 25)
#define LED27_h_p_m          (LED_RES_ID + 26)   /* h+/h- */
#define LED28_m              (LED_RES_ID + 27)
#define LED29_k              (LED_RES_ID + 28)
#define LED30_M              (LED_RES_ID + 29)
#define LED31_V              (LED_RES_ID + 30)
#define LED32_A              (LED_RES_ID + 31)
#define LED33_W              (LED_RES_ID + 32)
#define LED34_PF             (LED_RES_ID + 33)
#define LED35_Hz             (LED_RES_ID + 34)
#define LED36_percent        (LED_RES_ID + 35)
#define LED37_A_RANGE        (LED_RES_ID + 36)
#define LED38_B_RANGE        (LED_RES_ID + 37)
#define LED39_C_MATH         (LED_RES_ID + 38)
#define LED40_D_THD          (LED_RES_ID + 39)
#define LED41_UPDATE         (LED_RES_ID + 40)
#define LED42_MAX_HOLD       (LED_RES_ID + 41)
#define LED43_RMS            (LED_RES_ID + 42)
#define LED44_V_MEAN         (LED_RES_ID + 43)
#define LED45_DC             (LED_RES_ID + 44)
#define LED46_VOLTAGE        (LED_RES_ID + 45)
#define LED47_CURRENT        (LED_RES_ID + 46)
#define LED48_SCALING        (LED_RES_ID + 47)
#define LED49_AVG            (LED_RES_ID + 48)
#define LED50_HARMONICS      (LED_RES_ID + 49)
#define LED51_LINE           (LED_RES_ID + 50)
#define LED52_FREQ           (LED_RES_ID + 51)
#define LED53_STORE          (LED_RES_ID + 52)
#define LED54_REMOTE         (LED_RES_ID + 53)
#define LED55_AUTO           (LED_RES_ID + 54)
#define LED56_AUTO           (LED_RES_ID + 55)
#define LED57_KEY_PROTECT    (LED_RES_ID + 56)
#define LED58_VOLTAGE        (LED_RES_ID + 57)
#define LED59_CURRENT        (LED_RES_ID + 58)
#define LED60_HOLD           (LED_RES_ID + 59)
#define LED61_START          (LED_RES_ID + 60)
#define LED62_STOP           (LED_RES_ID + 61)
#define LED63_SHIFT          (LED_RES_ID + 62)


/**
 * \brief PM1000 KEY Resource
 */
#define KEY_RES_ID           (KEY_CNT)

#define KEY_S1_Fa            (KEY_RES_ID + 0x01)
#define KEY_S2_VOLTAGE       (KEY_RES_ID + 0x02)
#define KEY_S3_CURRENT       (KEY_RES_ID + 0x03)
#define KEY_S4_HOLD          (KEY_RES_ID + 0x04)
#define KEY_S5_Fb            (KEY_RES_ID + 0x05)
#define KEY_S6_DOWN          (KEY_RES_ID + 0x06)
#define KEY_S7_UP            (KEY_RES_ID + 0x07)
#define KEY_S8_SET           (KEY_RES_ID + 0x08)
#define KEY_S9_Fc            (KEY_RES_ID + 0x09)
#define KEY_S10_START        (KEY_RES_ID + 0x0A)
#define KEY_S11_STOP         (KEY_RES_ID + 0x0B)
#define KEY_S12_INTERFACE    (KEY_RES_ID + 0x0C)
#define KEY_S13_Fd           (KEY_RES_ID + 0x0D)
#define KEY_S14_SETUP        (KEY_RES_ID + 0x0E)
#define KEY_S15_SAVE_LOAD    (KEY_RES_ID + 0x0F)
#define KEY_S16_SHIFT        (KEY_RES_ID + 0x10)


#ifdef __cplusplus
}
#endif


#endif /* AWBL_PANEL_PM1000_DEF_H_ */
