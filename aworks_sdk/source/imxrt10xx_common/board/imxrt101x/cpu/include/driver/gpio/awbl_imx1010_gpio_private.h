/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief AWBus-lite iMX RT1010 GPIO 驱动上私有的一些功能
 * \internal
 * \par modification history:
 * - 1.00 17-10-10  mex, first implementation
 * \endinternal
 */

#ifndef __AWBL_IMX1010_GPIO_PRIVATE_H
#define __AWBL_IMX1010_GPIO_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aworks.h"
#include "awbl_gpio.h"
#include "awbl_intctlr.h"


typedef enum __tag_imx1010_pinmux_sel {
    PIN_MUX_ALT0 = 0x0,
    PIN_MUX_ALT1 = 0x1,
    PIN_MUX_ALT2 = 0x2,
    PIN_MUX_ALT3 = 0x3,
    PIN_MUX_ALT4 = 0x4,
    PIN_MUX_ALT5 = 0x5,
    PIN_MUX_ALT6 = 0x6,
    PIN_MUX_ALT7 = 0x7,
    PIN_MUX_SI_ON = 0x10,
    PIN_MUX_SET_ENABLE = 0x20,
}imx1010_pin_mux_sel_t;


typedef enum __tag_imx1010_pad_ctl_sel {
    SRE_0_Slow_Slew_Rate = 0x0 << 0,
    SRE_0_Fast_Slew_Rate = 0x1 << 0,

    DSE_0_output_driver_disabled = 0x0 << 3,
    DSE_1_R0 = 0x1 <<3,
    DSE_2_R0_2 = 0x2 << 3,
    DSE_3_R0_3 = 0x3 << 3,
    DSE_4_R0_4 = 0x4 << 3,
    DSE_5_R0_5 = 0x5 << 3,
    DSE_6_R0_6 = 0x6 << 3,
    DSE_7_R0_7 = 0x7 << 3,

    SPEED_0_low_50MHz = 0x0 << 6,
    SPEED_1_medium_100MHz = 0x1 << 6,
    SPEED_2_medium_100MHz = 0x2 << 6,
    SPEED_3_max_200MHz = 0x3 << 6,

    Open_Drain_Disabled  = 0x0 << 11,
    Open_Drain_Enabled = 0x1 << 11,

    PKE_0_Pull_Keeper_Disabled  = 0x0 << 12,
    PKE_1_Pull_Keeper_Enabled = 0x1 << 12,

    PUE_0_Keeper  = 0x0 << 13,
    PUE_1_Pull = 0x1 << 13,

    PUS_0_100K_Ohm_Pull_Down = 0x0 <<14,
    PUS_1_47K_Ohm_Pull_Up = 0x1 <<14,
    PUS_2_100K_Ohm_Pull_Up = 0x2 <<14,
    PUS_3_22K_Ohm_Pull_Up = 0x3 <<14,

    HYS_0_Hysteresis_Disabled  = 0x0 << 16,
    HYS_1_Hysteresis_Enabled = 0x1 << 16,

    PAD_CTL_SET_ENABLE = 0x1 << 17,
} imx1010_pad_ctl_sel_t;


/** \brief imx1010 GPIO引脚复用功能位段定义  */
#define IMX1010_PIN_MUX_BITS_START       6
#define IMX1010_PIN_MUX_BITS_LEN         6
#define IMX1010_PIN_MUX(x)               ( ((x) | PIN_MUX_SET_ENABLE) << IMX1010_PIN_MUX_BITS_START)
#define IMX1010_PIN_MUX_GET(data)        AW_BITS_GET((data), \
                                                 IMX1010_PIN_MUX_BITS_START, \
                                                 IMX1010_PIN_MUX_BITS_LEN - 1 )
#define IS_PIN_MUX_SET_ENABLE(flag)      AW_BIT_ISSET(flag, \
                                             IMX1010_PIN_MUX_BITS_START + IMX1010_PIN_MUX_BITS_LEN -1)

/** \brief imx1010 GPIO PAD位段定义  */
#define IMX1010_PAD_CTL_BITS_START       12
#define IMX1010_PAD_CTL_BITS_LEN         18
#define IMX1010_PAD_CTL(x)               (((x) | PAD_CTL_SET_ENABLE) << IMX1010_PAD_CTL_BITS_START)
#define IMX1010_PAD_CTL_GET(data)        AW_BITS_GET((data), \
                                                 IMX1010_PAD_CTL_BITS_START, \
                                                 IMX1010_PAD_CTL_BITS_LEN - 1 )
#define IS_PAD_CTL_SET_ENABLE(flag)      AW_BIT_ISSET(flag, \
                                             IMX1010_PAD_CTL_BITS_START + IMX1010_PAD_CTL_BITS_LEN - 1)


/******************************************************************************/

/** \brief IMX1010 GPIO 管脚功能编号 */
#define GPIO1_0_FLEXSPI_B_DQS                IMX1010_PIN_MUX(PIN_MUX_ALT0 | PIN_MUX_SI_ON)
#define GPIO1_0_SAI3_MCLK                    IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO1_0_LPSPI2_PCS3                  IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO1_0_LPSPI1_PCS3                  IMX1010_PIN_MUX(PIN_MUX_ALT3)
#define GPIO1_0_PIT_TRIGGER00                IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO1_0_GPIOMUX_IO00                 IMX1010_PIN_MUX(PIN_MUX_ALT5)

#define GPIO1_1_SAI1_RX_BCLK                 IMX1010_PIN_MUX(PIN_MUX_ALT0 | PIN_MUX_SI_ON)
#define GPIO1_1_WDOG1_ANY                    IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO1_1_FLEXPWM1_PWM0_B              IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO1_1_LPI2C1_SDA                   IMX1010_PIN_MUX(PIN_MUX_ALT3 | PIN_MUX_SI_ON)
#define GPIO1_1_KPP_ROW03                    IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO1_1_GPIOMUX_IO01                 IMX1010_PIN_MUX(PIN_MUX_ALT5)

#define GPIO1_2_SAI1_RX_SYNC                 IMX1010_PIN_MUX(PIN_MUX_ALT0 | PIN_MUX_SI_ON)
#define GPIO1_2_WDOG2_B                      IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO1_2_FLEXPWM1_PWM0_A              IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO1_2_LPI2C1_SCL                   IMX1010_PIN_MUX(PIN_MUX_ALT3 | PIN_MUX_SI_ON)
#define GPIO1_2_KPP_COL03                    IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO1_2_GPIOMUX_IO02                 IMX1010_PIN_MUX(PIN_MUX_ALT5)

#define GPIO1_3_SAI1_RX_DATA00               IMX1010_PIN_MUX(PIN_MUX_ALT0 | PIN_MUX_SI_ON)
#define GPIO1_3_GPT1_COMPARE3                IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO1_3_FLEXPWM1_PWM1_B              IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO1_3_SPDIF_SR_CLK                 IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO1_3_GPIOMUX_IO03                 IMX1010_PIN_MUX(PIN_MUX_ALT5)

#define GPIO1_4_SAI1_TX_DATA00               IMX1010_PIN_MUX(PIN_MUX_ALT0 | PIN_MUX_SI_ON)
#define GPIO1_4_GPT1_CAPTURE2                IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO1_4_FLEXPWM1_PWM1_A              IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO1_4_SPDIF_IN                     IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO1_4_GPIOMUX_IO04                 IMX1010_PIN_MUX(PIN_MUX_ALT5)

#define GPIO1_5_SAI1_TX_DATA01               IMX1010_PIN_MUX(PIN_MUX_ALT0 | PIN_MUX_SI_ON)
#define GPIO1_5_GPT1_COMPARE2                IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO1_5_FLEXPWM1_PWM2_B              IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO1_5_LPUART4_RXD                  IMX1010_PIN_MUX(PIN_MUX_ALT3)
#define GPIO1_5_SPDIF_OUT                    IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO1_5_GPIOMUX_IO05                 IMX1010_PIN_MUX(PIN_MUX_ALT5)

#define GPIO1_6_SAI1_TX_BCLK                 IMX1010_PIN_MUX(PIN_MUX_ALT0 | PIN_MUX_SI_ON)
#define GPIO1_6_GPT1_CAPTURE1                IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO1_6_FLEXPWM1_PWM2_A              IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO1_6_LPUART4_TXD                  IMX1010_PIN_MUX(PIN_MUX_ALT3)
#define GPIO1_6_SPDIF_EXT_CLK                IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO1_6_GPIOMUX_IO06                 IMX1010_PIN_MUX(PIN_MUX_ALT5)

#define GPIO1_7_SAI1_TX_SYNC                 IMX1010_PIN_MUX(PIN_MUX_ALT0 | PIN_MUX_SI_ON)
#define GPIO1_7_GPT1_COMPARE1                IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO1_7_FLEXPWM1_PWM3_B              IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO1_7_LPUART3_RXD                  IMX1010_PIN_MUX(PIN_MUX_ALT3)
#define GPIO1_7_SPDIF_LOCK                   IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO1_7_GPIOMUX_IO07                 IMX1010_PIN_MUX(PIN_MUX_ALT5)
#define GPIO1_7_LPUART1_RTS_B                IMX1010_PIN_MUX(PIN_MUX_ALT6)

#define GPIO1_8_SAI1_MCLK                    IMX1010_PIN_MUX(PIN_MUX_ALT0 | PIN_MUX_SI_ON)
#define GPIO1_8_GPT1_CLK                     IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO1_8_FLEXPWM1_PWM3_A              IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO1_8_LPUART3_TXD                  IMX1010_PIN_MUX(PIN_MUX_ALT3)
#define GPIO1_8_FLEXIO1_IO00                 IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO1_8_GPIOMUX_IO08                 IMX1010_PIN_MUX(PIN_MUX_ALT5)
#define GPIO1_8_LPUART1_CTS_B                IMX1010_PIN_MUX(PIN_MUX_ALT6)

#define GPIO1_9_LPUART1_RXD                  IMX1010_PIN_MUX(PIN_MUX_ALT0)
#define GPIO1_9_WDOG1_B                      IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO1_9_FLEXSPI_A_SS1_B              IMX1010_PIN_MUX(PIN_MUX_ALT2 | PIN_MUX_SI_ON)
#define GPIO1_9_LPI2C2_SDA                   IMX1010_PIN_MUX(PIN_MUX_ALT3 | PIN_MUX_SI_ON)
#define GPIO1_9_FLEXIO1_IO01                 IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO1_9_GPIOMUX_IO09                 IMX1010_PIN_MUX(PIN_MUX_ALT5)
#define GPIO1_9_SPDIF_SR_CLK                 IMX1010_PIN_MUX(PIN_MUX_ALT6)

#define GPIO1_10_LPUART1_TXD                 IMX1010_PIN_MUX(PIN_MUX_ALT0)
#define GPIO1_10_LPI2C1_HREQ                 IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO1_10_EWM_OUT_B                   IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO1_10_LPI2C2_SCL                  IMX1010_PIN_MUX(PIN_MUX_ALT3 | PIN_MUX_SI_ON)
#define GPIO1_10_FLEXIO1_IO02                IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO1_10_GPIOMUX_IO10                IMX1010_PIN_MUX(PIN_MUX_ALT5)
#define GPIO1_10_SPDIF_IN                    IMX1010_PIN_MUX(PIN_MUX_ALT6)

#define GPIO1_11_LPUART3_RXD                 IMX1010_PIN_MUX(PIN_MUX_ALT0)
#define GPIO1_11_LPI2C1_SDA                  IMX1010_PIN_MUX(PIN_MUX_ALT1 | PIN_MUX_SI_ON)
#define GPIO1_11_KPP_ROW00                   IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO1_11_FLEXSPI_B_SS1_B             IMX1010_PIN_MUX(PIN_MUX_ALT3 | PIN_MUX_SI_ON)
#define GPIO1_11_FLEXIO1_IO03                IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO1_11_GPIOMUX_IO11                IMX1010_PIN_MUX(PIN_MUX_ALT5)
#define GPIO1_11_SPDIF_OUT                   IMX1010_PIN_MUX(PIN_MUX_ALT6)
#define GPIO1_11_ARM_CM7_TRACE03             IMX1010_PIN_MUX(PIN_MUX_ALT7)

#define GPIO1_12_LPUART3_TXD                 IMX1010_PIN_MUX(PIN_MUX_ALT0)
#define GPIO1_12_LPI2C1_SCL                  IMX1010_PIN_MUX(PIN_MUX_ALT1 | PIN_MUX_SI_ON)
#define GPIO1_12_KPP_COL00                   IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO1_12_USB_OTG1_OC                 IMX1010_PIN_MUX(PIN_MUX_ALT3)
#define GPIO1_12_FLEXIO1_IO04                IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO1_12_GPIOMUX_IO12                IMX1010_PIN_MUX(PIN_MUX_ALT5)
#define GPIO1_12_SPDIF_EXT_CLK               IMX1010_PIN_MUX(PIN_MUX_ALT6)
#define GPIO1_12_ARM_CM7_TRACE02             IMX1010_PIN_MUX(PIN_MUX_ALT7)

#define GPIO1_13_LPUART2_RXD                 IMX1010_PIN_MUX(PIN_MUX_ALT0)
#define GPIO1_13_LPSPI2_PCS2                 IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO1_13_KPP_ROW03                   IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO1_13_OTG1_ID                     IMX1010_PIN_MUX(PIN_MUX_ALT3)
#define GPIO1_13_FLEXIO1_IO05                IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO1_13_GPIOMUX_IO13                IMX1010_PIN_MUX(PIN_MUX_ALT5)
#define GPIO1_13_SPDIF_LOCK                  IMX1010_PIN_MUX(PIN_MUX_ALT6)
#define GPIO1_13_ARM_CM7_TRACE01             IMX1010_PIN_MUX(PIN_MUX_ALT7)

#define GPIO1_14_LPUART2_TXD                 IMX1010_PIN_MUX(PIN_MUX_ALT0)
#define GPIO1_14_LPSPI1_PCS2                 IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO1_14_KPP_COL03                   IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO1_14_USB_OTG1_PWR                IMX1010_PIN_MUX(PIN_MUX_ALT3)
#define GPIO1_14_FLEXIO1_IO20                IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO1_14_GPIOMUX_IO14                IMX1010_PIN_MUX(PIN_MUX_ALT5)
#define GPIO1_14_NMI_GLUE_NMI                IMX1010_PIN_MUX(PIN_MUX_ALT6)
#define GPIO1_14_ARM_CM7_TRACE00             IMX1010_PIN_MUX(PIN_MUX_ALT7)

#define GPIO1_15_LPUART4_RXD                 IMX1010_PIN_MUX(PIN_MUX_ALT0)
#define GPIO1_15_LPSPI2_PCS1                 IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO1_15_WDOG1_ANY                   IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO1_15_LPI2C2_SDA                  IMX1010_PIN_MUX(PIN_MUX_ALT3 | PIN_MUX_SI_ON)
#define GPIO1_15_MQS_LEFT                    IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO1_15_GPIOMUX_IO15                IMX1010_PIN_MUX(PIN_MUX_ALT5)
#define GPIO1_15_USB_OTG1_OC                 IMX1010_PIN_MUX(PIN_MUX_ALT6)
#define GPIO1_15_ARM_CM7_TRACE_SWO           IMX1010_PIN_MUX(PIN_MUX_ALT7)

#define GPIO1_16_LPUART4_TXD                 IMX1010_PIN_MUX(PIN_MUX_ALT0)
#define GPIO1_16_LPSPI1_PCS1                 IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO1_16_WDOG2_B                     IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO1_16_LPI2C2_SCL                  IMX1010_PIN_MUX(PIN_MUX_ALT3 | PIN_MUX_SI_ON)
#define GPIO1_16_MQS_RIGHT                   IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO1_16_GPIOMUX_IO16                IMX1010_PIN_MUX(PIN_MUX_ALT5)
#define GPIO1_16_ARM_CM7_TRACE_CLK           IMX1010_PIN_MUX(PIN_MUX_ALT7)

#define GPIO1_17_LPSPI1_SDI                  IMX1010_PIN_MUX(PIN_MUX_ALT0)
#define GPIO1_17_PIT_TRIGGER03               IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO1_17_FLEXPWM1_PWM2_B             IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO1_17_KPP_ROW02                   IMX1010_PIN_MUX(PIN_MUX_ALT3)
#define GPIO1_17_GPT2_CLK                    IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO1_17_GPIOMUX_IO17                IMX1010_PIN_MUX(PIN_MUX_ALT5)
#define GPIO1_17_SNVS_HP_VIO_5_B             IMX1010_PIN_MUX(PIN_MUX_ALT6)
#define GPIO1_17_JTAG_DE_B                   IMX1010_PIN_MUX(PIN_MUX_ALT7)

#define GPIO1_18_LPSPI1_SDO                  IMX1010_PIN_MUX(PIN_MUX_ALT0)
#define GPIO1_18_PIT_TRIGGER02               IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO1_18_FLEXPWM1_PWM2_A             IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO1_18_KPP_COL02                   IMX1010_PIN_MUX(PIN_MUX_ALT3)
#define GPIO1_18_GPT2_COMPARE1               IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO1_18_GPIOMUX_IO18                IMX1010_PIN_MUX(PIN_MUX_ALT5)
#define GPIO1_18_SNVS_HP_VIO_5_CTL           IMX1010_PIN_MUX(PIN_MUX_ALT6)

#define GPIO1_19_LPSPI1_PCS0                 IMX1010_PIN_MUX(PIN_MUX_ALT0)
#define GPIO1_19_PIT_TRIGGER01               IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO1_19_FLEXPWM1_PWM3_B             IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO1_19_KPP_ROW01                   IMX1010_PIN_MUX(PIN_MUX_ALT3)
#define GPIO1_19_GPT2_CAPTURE1               IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO1_19_GPIOMUX_IO19                IMX1010_PIN_MUX(PIN_MUX_ALT5)

#define GPIO1_20_LPSPI1_SCK                  IMX1010_PIN_MUX(PIN_MUX_ALT0)
#define GPIO1_20_PIT_TRIGGER00               IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO1_20_FLEXPWM1_PWM3_A             IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO1_20_KPP_COL01                   IMX1010_PIN_MUX(PIN_MUX_ALT3)
#define GPIO1_20_GPT2_COMPARE2               IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO1_20_GPIOMUX_IO20                IMX1010_PIN_MUX(PIN_MUX_ALT5)
#define GPIO1_20_LPI2C1_HREQ                 IMX1010_PIN_MUX(PIN_MUX_ALT6)

#define GPIO1_21_LPI2C2_SDA                  IMX1010_PIN_MUX(PIN_MUX_ALT0 | PIN_MUX_SI_ON)
#define GPIO1_21_LPUART3_RXD                 IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO1_21_ARM_CM7_RXEV                IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO1_21_LPUART2_RTS_B               IMX1010_PIN_MUX(PIN_MUX_ALT3)
#define GPIO1_21_GPT2_CAPTURE2               IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO1_21_GPIOMUX_IO21                IMX1010_PIN_MUX(PIN_MUX_ALT5)
#define GPIO1_21_OCOTP_FUSE_LATCHED          IMX1010_PIN_MUX(PIN_MUX_ALT6)
#define GPIO1_21_XBAR1_INOUT03               IMX1010_PIN_MUX(PIN_MUX_ALT7)

#define GPIO1_22_LPI2C2_SCL                  IMX1010_PIN_MUX(PIN_MUX_ALT0 | PIN_MUX_SI_ON)
#define GPIO1_22_LPUART3_TXD                 IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO1_22_ARM_CM7_TXEV                IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO1_22_LPUART2_CTS_B               IMX1010_PIN_MUX(PIN_MUX_ALT3)
#define GPIO1_22_GPT2_COMPARE3               IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO1_22_GPIOMUX_IO22                IMX1010_PIN_MUX(PIN_MUX_ALT5)
#define GPIO1_22_EWM_OUT_B                   IMX1010_PIN_MUX(PIN_MUX_ALT6)
#define GPIO1_22_JTAG_TRSTB                  IMX1010_PIN_MUX(PIN_MUX_ALT7)

#define GPIO1_23_LPSPI2_SDI                  IMX1010_PIN_MUX(PIN_MUX_ALT0)
#define GPIO1_23_FLEXPWM1_PWM3_X             IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO1_23_KPP_ROW02                   IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO1_23_ARM_TRACE_SWO               IMX1010_PIN_MUX(PIN_MUX_ALT3)
#define GPIO1_23_FLEXIO1_IO21                IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO1_23_GPIOMUX_IO23                IMX1010_PIN_MUX(PIN_MUX_ALT5)
#define GPIO1_23_REF_32K_OUT                 IMX1010_PIN_MUX(PIN_MUX_ALT6)
#define GPIO1_23_JTAG_TDO                    IMX1010_PIN_MUX(PIN_MUX_ALT7)

#define GPIO1_24_LPSPI2_SDO                  IMX1010_PIN_MUX(PIN_MUX_ALT0)
#define GPIO1_24_FLEXPWM1_PWM2_X             IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO1_24_KPP_COL02                   IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO1_24_PIT_TRIGGER03               IMX1010_PIN_MUX(PIN_MUX_ALT3)
#define GPIO1_24_FLEXIO1_IO22                IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO1_24_GPIOMUX_IO24                IMX1010_PIN_MUX(PIN_MUX_ALT5)
#define GPIO1_24_OTG1_ID                     IMX1010_PIN_MUX(PIN_MUX_ALT6)
#define GPIO1_24_JTAG_TDI                    IMX1010_PIN_MUX(PIN_MUX_ALT7)

#define GPIO1_25_LPSPI2_PCS0                 IMX1010_PIN_MUX(PIN_MUX_ALT0)
#define GPIO1_25_FLEXPWM1_PWM1_X             IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO1_25_KPP_ROW01                   IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO1_25_PIT_TRIGGER02               IMX1010_PIN_MUX(PIN_MUX_ALT3)
#define GPIO1_25_FLEXIO1_IO23                IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO1_25_GPIOMUX_IO25                IMX1010_PIN_MUX(PIN_MUX_ALT5)
#define GPIO1_25_WDOG1_B                     IMX1010_PIN_MUX(PIN_MUX_ALT6)
#define GPIO1_25_JTAG_MOD                    IMX1010_PIN_MUX(PIN_MUX_ALT7)

#define GPIO1_26_LPSPI2_SCK                  IMX1010_PIN_MUX(PIN_MUX_ALT0)
#define GPIO1_26_FLEXPWM1_PWM0_X             IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO1_26_KPP_COL01                   IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO1_26_PIT_TRIGGER01               IMX1010_PIN_MUX(PIN_MUX_ALT3)
#define GPIO1_26_FLEXIO1_IO24                IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO1_26_GPIOMUX_IO26                IMX1010_PIN_MUX(PIN_MUX_ALT5)
#define GPIO1_26_USB_OTG1_PWR                IMX1010_PIN_MUX(PIN_MUX_ALT6)
#define GPIO1_26_JTAG_TCK                    IMX1010_PIN_MUX(PIN_MUX_ALT7)

#define GPIO1_27_LPI2C1_SDA                  IMX1010_PIN_MUX(PIN_MUX_ALT0 | PIN_MUX_SI_ON)
#define GPIO1_27_LPUART3_RTS_B               IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO1_27_KPP_ROW00                   IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO1_27_LPUART4_RTS_B               IMX1010_PIN_MUX(PIN_MUX_ALT3)
#define GPIO1_27_FLEXIO1_IO25                IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO1_27_GPIOMUX_IO27                IMX1010_PIN_MUX(PIN_MUX_ALT5)
#define GPIO1_27_NMI_GLUE_NMI                IMX1010_PIN_MUX(PIN_MUX_ALT6)
#define GPIO1_27_JTAG_TMS                    IMX1010_PIN_MUX(PIN_MUX_ALT7)

#define GPIO1_28_LPI2C1_SCL                  IMX1010_PIN_MUX(PIN_MUX_ALT0 | PIN_MUX_SI_ON)
#define GPIO1_28_LPUART3_CTS_B               IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO1_28_KPP_COL00                   IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO1_28_LPUART4_CTS_B               IMX1010_PIN_MUX(PIN_MUX_ALT3)
#define GPIO1_28_FLEXIO1_IO26                IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO1_28_GPIOMUX_IO28                IMX1010_PIN_MUX(PIN_MUX_ALT5)
#define GPIO1_28_REF_CLK_24M                 IMX1010_PIN_MUX(PIN_MUX_ALT6)
#define GPIO1_28_XBAR1_INOUT02               IMX1010_PIN_MUX(PIN_MUX_ALT7)




#define GPIO2_0_FLEXSPI_B_SS0_B              IMX1010_PIN_MUX(PIN_MUX_ALT0 | PIN_MUX_SI_ON)
#define GPIO2_0_SAI3_TX_SYNC                 IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO2_0_ARM_CM7_RXEV                 IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO2_0_CCM_STOP                     IMX1010_PIN_MUX(PIN_MUX_ALT3)
#define GPIO2_0_FLEXIO1_IO06                 IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO2_0_GPIO                         IMX1010_PIN_MUX(PIN_MUX_ALT5)
#define GPIO2_0_SRC_BT_CFG02                 IMX1010_PIN_MUX(PIN_MUX_ALT6)

#define GPIO2_1_FLEXSPI_B_DATA01             IMX1010_PIN_MUX(PIN_MUX_ALT0 | PIN_MUX_SI_ON)
#define GPIO2_1_SAI3_TX_BCLK                 IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO2_1_FLEXPWM1_PWM0_B              IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO2_1_CCM_CLKO2                    IMX1010_PIN_MUX(PIN_MUX_ALT3)
#define GPIO2_1_FLEXIO1_IO07                 IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO2_1_GPIO                         IMX1010_PIN_MUX(PIN_MUX_ALT5)
#define GPIO2_1_SRC_BT_CFG01                 IMX1010_PIN_MUX(PIN_MUX_ALT6)

#define GPIO2_2_FLEXSPI_B_DATA02             IMX1010_PIN_MUX(PIN_MUX_ALT0 | PIN_MUX_SI_ON)
#define GPIO2_2_SAI3_TX_DATA                 IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO2_2_FLEXPWM1_PWM0_A              IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO2_2_CCM_CLKO1                    IMX1010_PIN_MUX(PIN_MUX_ALT3)
#define GPIO2_2_FLEXIO1_IO08                 IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO2_2_GPIO                         IMX1010_PIN_MUX(PIN_MUX_ALT5)
#define GPIO2_2_SRC_BT_CFG00                 IMX1010_PIN_MUX(PIN_MUX_ALT6)

#define GPIO2_3_FLEXSPI_B_DATA00             IMX1010_PIN_MUX(PIN_MUX_ALT0 | PIN_MUX_SI_ON)
#define GPIO2_3_SAI3_RX_DATA                 IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO2_3_FLEXPWM1_PWM1_B              IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO2_3_CCM_REF_EN_B                 IMX1010_PIN_MUX(PIN_MUX_ALT3)
#define GPIO2_3_FLEXIO1_IO09                 IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO2_3_GPIO                         IMX1010_PIN_MUX(PIN_MUX_ALT5)
#define GPIO2_3_SRC_BOOT_MODE01              IMX1010_PIN_MUX(PIN_MUX_ALT6)

#define GPIO2_4_FLEXSPI_B_DATA03             IMX1010_PIN_MUX(PIN_MUX_ALT0 | PIN_MUX_SI_ON)
#define GPIO2_4_SAI3_RX_SYNC                 IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO2_4_FLEXPWM1_PWM1_A              IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO2_4_CCM_WAIT                     IMX1010_PIN_MUX(PIN_MUX_ALT3)
#define GPIO2_4_FLEXIO1_IO10                 IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO2_4_GPIO                         IMX1010_PIN_MUX(PIN_MUX_ALT5)
#define GPIO2_4_SRC_BOOT_MODE00              IMX1010_PIN_MUX(PIN_MUX_ALT6)

#define GPIO2_5_FLEXSPI_A_SS1_B              IMX1010_PIN_MUX(PIN_MUX_ALT0 | PIN_MUX_SI_ON)
#define GPIO2_5_LPI2C1_SDA                   IMX1010_PIN_MUX(PIN_MUX_ALT1 | PIN_MUX_SI_ON)
#define GPIO2_5_LPSPI1_SDI                   IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO2_5_FLEXIO1_IO11                 IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO2_5_GPIO                         IMX1010_PIN_MUX(PIN_MUX_ALT5)

#define GPIO2_6_FLEXSPI_A_SS0_B              IMX1010_PIN_MUX(PIN_MUX_ALT0 | PIN_MUX_SI_ON)
#define GPIO2_6_LPI2C1_SCL                   IMX1010_PIN_MUX(PIN_MUX_ALT1 | PIN_MUX_SI_ON)
#define GPIO2_6_LPSPI1_SDO                   IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO2_6_FLEXIO1_IO12                 IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO2_6_GPIO                         IMX1010_PIN_MUX(PIN_MUX_ALT5)

#define GPIO2_7_FLEXSPI_A_DATA1              IMX1010_PIN_MUX(PIN_MUX_ALT0 | PIN_MUX_SI_ON)
#define GPIO2_7_LPI2C2_SDA                   IMX1010_PIN_MUX(PIN_MUX_ALT1 | PIN_MUX_SI_ON)
#define GPIO2_7_LPSPI1_PCS0                  IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO2_7_FLEXIO1_IO13                 IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO2_7_GPIO                         IMX1010_PIN_MUX(PIN_MUX_ALT5)

#define GPIO2_8_FLEXSPI_A_DATA2              IMX1010_PIN_MUX(PIN_MUX_ALT0)
#define GPIO2_8_LPI2C2_SCL                   IMX1010_PIN_MUX(PIN_MUX_ALT1 | PIN_MUX_SI_ON)
#define GPIO2_8_LPSPI1_SCK                   IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO2_8_FLEXIO1_IO14                 IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO2_8_GPIO                         IMX1010_PIN_MUX(PIN_MUX_ALT5)

#define GPIO2_9_FLEXSPI_A_DATA0              IMX1010_PIN_MUX(PIN_MUX_ALT0 | PIN_MUX_SI_ON)
#define GPIO2_9_LPSPI2_SDI                   IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO2_9_LPUART2_RXD                  IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO2_9_FLEXIO1_IO15                 IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO2_9_GPIO                         IMX1010_PIN_MUX(PIN_MUX_ALT5)

#define GPIO2_10_FLEXSPI_A_SCLK              IMX1010_PIN_MUX(PIN_MUX_ALT0 | PIN_MUX_SI_ON)
#define GPIO2_10_LPSPI2_SDO                  IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO2_10_LPUART2_TXD                 IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO2_10_FLEXIO1_IO16                IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO2_10_GPIO                        IMX1010_PIN_MUX(PIN_MUX_ALT5)

#define GPIO2_11_FLEXSPI_A_DATA3             IMX1010_PIN_MUX(PIN_MUX_ALT0 | PIN_MUX_SI_ON)
#define GPIO2_11_LPSPI2_SCK                  IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO2_11_LPUART1_RXD                 IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO2_11_FLEXIO1_IO17                IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO2_11_GPIO                        IMX1010_PIN_MUX(PIN_MUX_ALT5)
#define GPIO2_11_WDOG1_RST_B_DEB             IMX1010_PIN_MUX(PIN_MUX_ALT6)

#define GPIO2_12_FLEXSPI_A_DQS               IMX1010_PIN_MUX(PIN_MUX_ALT0 | PIN_MUX_SI_ON)
#define GPIO2_12_LPSPI2_PCS0                 IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO2_12_LPUART1_TXD                 IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO2_12_FLEXIO1_IO18                IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO2_12_GPIO                        IMX1010_PIN_MUX(PIN_MUX_ALT5)
#define GPIO2_12_WDOG2_RST_B_DEB             IMX1010_PIN_MUX(PIN_MUX_ALT6)

#define GPIO2_13_FLEXSPI_B_SCLK              IMX1010_PIN_MUX(PIN_MUX_ALT0 | PIN_MUX_SI_ON)
#define GPIO2_13_SAI3_RX_BCLK                IMX1010_PIN_MUX(PIN_MUX_ALT1)
#define GPIO2_13_ARM_CM7_TXEV                IMX1010_PIN_MUX(PIN_MUX_ALT2)
#define GPIO2_13_CCM_PMIC_RDY                IMX1010_PIN_MUX(PIN_MUX_ALT3)
#define GPIO2_13_FLEXIO1_IO19                IMX1010_PIN_MUX(PIN_MUX_ALT4)
#define GPIO2_13_GPIO                        IMX1010_PIN_MUX(PIN_MUX_ALT5)
#define GPIO2_13_SRC_BT_CFG03                IMX1010_PIN_MUX(PIN_MUX_ALT6)

#define GPIO2_14_FLEXSPI_A_DQS               IMX1010_PIN_MUX(PIN_MUX_ALT0 | PIN_MUX_SI_ON)
#define GPIO2_14_FLEXSPI_B_DQS               IMX1010_PIN_MUX(PIN_MUX_ALT1 | PIN_MUX_SI_ON)



#define GPIO5_0_SNVS_LP_PMIC_ON_REQ          IMX1010_PIN_MUX(PIN_MUX_ALT0)
#define GPIO5_0_GPIO                         IMX1010_PIN_MUX(PIN_MUX_ALT5)





#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX1010_GPIO_PRIVATE_H */

/* end of file */

