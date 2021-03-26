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
 * \brief AWBus-lite iMX RT1050 GPIO 驱动上私有的一些功能
 * \internal
 * \par modification history:
 * - 1.00 17-10-10  mex, first implementation
 * \endinternal
 */

#ifndef __IMX1050_PIN_PROPERTY_H
#define __IMX1050_PIN_PROPERTY_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aworks.h"
#include "awbl_gpio.h"
#include "awbl_intctlr.h"


/**
 * \brief imx1050 pinmux select
 */
#define    PIN_MUX_ALT0                    0x0
#define    PIN_MUX_ALT1                    0x1
#define    PIN_MUX_ALT2                    0x2
#define    PIN_MUX_ALT3                    0x3
#define    PIN_MUX_ALT4                    0x4
#define    PIN_MUX_ALT5                    0x5
#define    PIN_MUX_ALT6                    0x6
#define    PIN_MUX_ALT7                    0x7
#define    PIN_MUX_SI_ON                   0x10
#define    PIN_MUX_SET_ENABLE              0x20

/**
 * \brief imx1050 pad ctl select
 */
#define    SRE_0_Slow_Slew_Rate             (0x0 << 0)
#define    SRE_0_Fast_Slew_Rate             (0x1 << 0)

#define    DSE_0_output_driver_disabled     (0x0 << 3)
#define    DSE_1_R0                         (0x1 <<3 )
#define    DSE_2_R0_2                       (0x2 << 3)
#define    DSE_3_R0_3                       (0x3 << 3)
#define    DSE_4_R0_4                       (0x4 << 3)
#define    DSE_5_R0_5                       (0x5 << 3)
#define    DSE_6_R0_6                       (0x6 << 3)
#define    DSE_7_R0_7                       (0x7 << 3)

#define    SPEED_0_low_50MHz                (0x0 << 6)
#define    SPEED_1_medium_100MHz            (0x1 << 6)
#define    SPEED_2_medium_100MHz            (0x2 << 6)
#define    SPEED_3_max_200MHz               (0x3 << 6)

#define    Open_Drain_Disabled              (0x0 << 11)
#define    Open_Drain_Enabled               (0x1 << 11)

#define    PKE_0_Pull_Keeper_Disabled       (0x0 << 12)
#define    PKE_1_Pull_Keeper_Enabled        (0x1 << 12)

#define    PUE_0_Keeper                     (0x0 << 13)
#define    PUE_1_Pull                       (0x1 << 13)

#define    PUS_0_100K_Ohm_Pull_Down         (0x0 << 14)

#define    PUS_1_47K_Ohm_Pull_Up            (0x1 << 14)
#define    PUS_2_100K_Ohm_Pull_Up           (0x2 << 14)
#define    PUS_3_22K_Ohm_Pull_Up            (0x3 << 14)

#define    HYS_0_Hysteresis_Disabled        (0x0 << 16)
#define    HYS_1_Hysteresis_Enabled         (0x1 << 16)

#define    PAD_CTL_SET_ENABLE               (0x1 << 17)


/** \brief imx1050 GPIO引脚复用功能位段定义  */
#define IMX1050_PIN_MUX_BITS_START       6
#define IMX1050_PIN_MUX_BITS_LEN         6
#define IMX1050_PIN_MUX(x)               ( ((x) | PIN_MUX_SET_ENABLE) << IMX1050_PIN_MUX_BITS_START)
#define IMX1050_PIN_MUX_GET(data)        AW_BITS_GET((data), \
                                                 IMX1050_PIN_MUX_BITS_START, \
                                                 IMX1050_PIN_MUX_BITS_LEN - 1 )
#define IS_PIN_MUX_SET_ENABLE(flag)      AW_BIT_ISSET(flag, \
                                             IMX1050_PIN_MUX_BITS_START + IMX1050_PIN_MUX_BITS_LEN -1)

/** \brief imx1050 GPIO PAD位段定义  */
#define IMX1050_PAD_CTL_BITS_START       12
#define IMX1050_PAD_CTL_BITS_LEN         18
#define IMX1050_PAD_CTL(x)               (((x) | PAD_CTL_SET_ENABLE) << IMX1050_PAD_CTL_BITS_START)
#define IMX1050_PAD_CTL_GET(data)        AW_BITS_GET((data), \
                                                 IMX1050_PAD_CTL_BITS_START, \
                                                 IMX1050_PAD_CTL_BITS_LEN - 1 )
#define IS_PAD_CTL_SET_ENABLE(flag)      AW_BIT_ISSET(flag, \
                                             IMX1050_PAD_CTL_BITS_START+IMX1050_PAD_CTL_BITS_LEN - 1)


/******************************************************************************/
/** \brief imx1050 GPIO 管脚功能编号 */

/* GPIO1 */
#define  GPIO1_0_FLEXPWM2_PWMA03             IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_0_XBAR1_INOUT14               IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_0_USB_OTG2_ID                 IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_0_LPI2C1_SCLS                 IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_0_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_0_USDHC1_RESET_B              IMX1050_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_0_LPSPI3_SCK                  IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO1_1_FLEXPWM2_PWMB03             IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_1_XBAR1_INOUT15               IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_1_USB_OTG1_ID                 IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_1_LPI2C1_SDAS                 IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_1_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_1_EWM_OUT_B                   IMX1050_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_1_LPSPI3_SDO                  IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO1_2_FLEXCAN2_TX                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_2_XBAR1_INOUT16               IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_2_LPUART6_TX                  IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_2_USB_OTG1_PWR                IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_2_FLEXPWM1_PWMX00             IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_2_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_2_LPI2C1_HREQ                 IMX1050_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_2_LPSPI3_SDI                  IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO1_3_FLEXCAN2_RX                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_3_XBAR1_INOUT17               IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_3_LPUART6_RX                  IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_3_USB_OTG1_OC                 IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_3_FLEXPWM1_PWMX01             IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_3_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_3_LPSPI3_PCS0                 IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO1_4_SRC_BOOT_MODE00             IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_4_MQS_RIGHT                   IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_4_ENET_TX_DATA03              IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_4_SAI2_TX_SYNC                IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_4_CSI_DATA09                  IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_4_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_4_PIT_TRIGGER00               IMX1050_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_4_LPSPI3_PCS1                 IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO1_5_SRC_BOOT_MODE01             IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_5_MQS_LEFT                    IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_5_ENET_TX_DATA02              IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_5_SAI2_TX_BCLK                IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_5_CSI_DATA08                  IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_5_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_5_XBAR1_INOUT17               IMX1050_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_5_LPSPI3_PCS2                 IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO1_6_JTAG_TMS                    IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_6_GPT2_COMPARE1               IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_6_ENET_RX_CLK                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_6_SAI2_RX_BCLK                IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_6_CSI_DATA07                  IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_6_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_6_XBAR1_INOUT18               IMX1050_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_6_LPSPI3_PCS3                 IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO1_7_JTAG_TCK                    IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_7_GPT2_COMPARE2               IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_7_ENET_TX_ER                  IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_7_SAI2_RX_SYNC                IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_7_CSI_DATA06                  IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_7_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_7_XBAR1_INOUT19               IMX1050_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_7_ENET_1588_EVENT3_OUT        IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO1_8_JTAG_MOD                    IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_8_GPT2_COMPARE3               IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_8_ENET_RX_DATA03              IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_8_SAI2_RX_DATA                IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_8_CSI_DATA05                  IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_8_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_8_XBAR1_IN20                  IMX1050_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_8_ENET_1588_EVENT3_IN         IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO1_9_JTAG_TDI                    IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_9_FLEXPWM2_PWMA03             IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_9_ENET_RX_DATA02              IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_9_SAI2_TX_DATA                IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_9_CSI_DATA04                  IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_9_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_9_XBAR1_IN21                  IMX1050_PIN_MUX(PIN_MUX_ALT6 | PIN_MUX_SI_ON)
#define  GPIO1_9_GPT2_CLK                    IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO1_10_JTAG_TDO                   IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_10_FLEXPWM1_PWMA03            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_10_ENET_CRS                   IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_10_SAI2_MCLK                  IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_10_CSI_DATA03                 IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_10_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_10_XBAR1_IN22                 IMX1050_PIN_MUX(PIN_MUX_ALT6 | PIN_MUX_SI_ON)
#define  GPIO1_10_ENET_1588_EVENT0_OUT       IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO1_11_JTAG_TRSTB                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_11_FLEXPWM1_PWMB03            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_11_ENET_COL                   IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_11_WDOG1_WDOG_B               IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_11_CSI_DATA02                 IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_11_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_11_XBAR1_IN23                 IMX1050_PIN_MUX(PIN_MUX_ALT6 | PIN_MUX_SI_ON)
#define  GPIO1_11_ENET_1588_EVENT0_IN        IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO1_12_LPI2C4_SCL                 IMX1050_PIN_MUX(PIN_MUX_ALT0 | PIN_MUX_SI_ON)
#define  GPIO1_12_CCM_PMIC_READY             IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_12_LPUART1_TX                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_12_WDOG2_WDOG_B               IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_12_FLEXPWM1_PWMX02            IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_12_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_12_ENET_1588_EVENT1_OUT       IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO1_13_LPI2C4_SDA                 IMX1050_PIN_MUX(PIN_MUX_ALT0 | PIN_MUX_SI_ON)
#define  GPIO1_13_GPT1_CLK                   IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_13_LPUART1_RX                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_13_EWM_OUT_B                  IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_13_FLEXPWM1_PWMX03            IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_13_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_13_ENET_1588_EVENT1_IN        IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO1_14_USB_OTG2_OC                IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_14_XBAR1_IN24                 IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_14_LPUART1_CTS_B              IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_14_ENET_1588_EVENT0_OUT       IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_14_CSI_VSYNC                  IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_14_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_14_FLEXCAN2_TX                IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO1_15_USB_OTG2_PWR               IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_15_XBAR1_IN25                 IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_15_LPUART1_RTS_B              IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_15_ENET_1588_EVENT0_IN        IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_15_CSI_HSYNC                  IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_15_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_15_FLEXCAN2_RX                IMX1050_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_15_WDOG1_WDOG_RST_B_DEB       IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO1_16_USB_OTG2_ID                IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_16_QTIMER3_TIMER0             IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_16_LPUART2_CTS_B              IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_16_LPI2C1_SCL                 IMX1050_PIN_MUX(PIN_MUX_ALT3 | PIN_MUX_SI_ON)
#define  GPIO1_16_WDOG1_B                    IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_16_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_16_USDHC1_WP                  IMX1050_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_16_KPP_ROW07                  IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO1_17_USB_OTG1_PWR               IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_17_QTIMER3_TIMER1             IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_17_LPUART2_RTS_B              IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_17_LPI2C1_SDA                 IMX1050_PIN_MUX(PIN_MUX_ALT3 | PIN_MUX_SI_ON)
#define  GPIO1_17_CCM_PMIC_READY             IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_17_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_17_USDHC1_VSELECT             IMX1050_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_17_KPP_COL07                  IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO1_18_USB_OTG1_ID                IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_18_QTIMER3_TIMER2             IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_18_LPUART2_TX                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_18_SPDIF_OUT                  IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_18_ENET_1588_EVENT2_OUT       IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_18_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_18_USDHC1_CD_B                IMX1050_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_18_KPP_ROW06                  IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO1_19_USB_OTG1_OC                IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_19_QTIMER3_TIMER3             IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_19_LPUART2_RX                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_19_SPDIF_IN                   IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_19_ENET_1588_EVENT2_IN        IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_19_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_19_USDHC2_CD_B                IMX1050_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_19_KPP_COL06                  IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO1_20_FLEXSPIB_DATA03            IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_20_ENET_MDC                   IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_20_LPUART3_CTS_B              IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_20_SPDIF_SR_CLK               IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_20_CSI_PIXCLK                 IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_20_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_20_USDHC2_DATA0               IMX1050_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_20_KPP_ROW05                  IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO1_21_FLEXSPIB_DATA02            IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_21_ENET_MDIO                  IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_21_LPUART3_RTS_B              IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_21_SPDIF_OUT                  IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_21_CSI_MCLK                   IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_21_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_21_USDHC2_DATA1               IMX1050_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_21_KPP_COL05                  IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO1_22_FLEXSPIB_DATA01            IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_22_LPI2C3_SDA                 IMX1050_PIN_MUX(PIN_MUX_ALT1 | PIN_MUX_SI_ON)
#define  GPIO1_22_LPUART3_TX                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_22_SPDIF_LOCK                 IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_22_CSI_VSYNC                  IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_22_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_22_USDHC2_DATA2               IMX1050_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_22_KPP_ROW04                  IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO1_23_FLEXSPIB_DATA00            IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_23_LPI2C3_SCL                 IMX1050_PIN_MUX(PIN_MUX_ALT1 | PIN_MUX_SI_ON)
#define  GPIO1_23_LPUART3_RX                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_23_SPDIF_EXT_CLK              IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_23_CSI_HSYNC                  IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_23_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_23_USDHC2_DATA3               IMX1050_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_23_KPP_COL04                  IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO1_24_FLEXSPIA_SS1_B             IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_24_FLEXPWM4_PWMA00            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_24_FLEXCAN1_TX                IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_24_CCM_PMIC_READY             IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_24_CSI_DATA09                 IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_24_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_24_USDHC2_CMD                 IMX1050_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_24_KPP_ROW03                  IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO1_25_FLEXSPIA_DQS               IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_25_FLEXPWM4_PWMA01            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_25_FLEXCAN1_RX                IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_25_SAI1_MCLK                  IMX1050_PIN_MUX(PIN_MUX_ALT3 | PIN_MUX_SI_ON)
#define  GPIO1_25_CSI_DATA08                 IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_25_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_25_USDHC2_CLK                 IMX1050_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_25_KPP_COL03                  IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO1_26_FLEXSPIA_DATA03            IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_26_WDOG1_B                    IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_26_LPUART8_TX                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_26_SAI1_RX_SYNC               IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_26_CSI_DATA07                 IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_26_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_26_USDHC2_WP                  IMX1050_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_26_KPP_ROW02                  IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO1_27_FLEXSPIA_DATA02            IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_27_EWM_OUT_B                  IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_27_LPUART8_RX                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_27_SAI1_RX_BCLK               IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_27_CSI_DATA06                 IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_27_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_27_USDHC2_RESET_B             IMX1050_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_27_KPP_COL02                  IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO1_28_FLEXSPIA_DATA01            IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_28_ACMP_OUT00                 IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_28_LPSPI3_PCS0                IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_28_SAI1_RX_DATA00             IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO1_28_CSI_DATA05                 IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_28_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_28_USDHC2_DATA4               IMX1050_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_28_KPP_ROW01                  IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO1_29_FLEXSPIA_DATA00            IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_29_ACMP_OUT01                 IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_29_LPSPI3_SDI                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_29_SAI1_TX_DATA00             IMX1050_PIN_MUX(PIN_MUX_ALT3 | PIN_MUX_SI_ON)
#define  GPIO1_29_CSI_DATA04                 IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_29_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_29_USDHC2_DATA5               IMX1050_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_29_KPP_COL01                  IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO1_30_FLEXSPIA_SCLK              IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_30_ACMP_OUT02                 IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_30_LPSPI3_SDO                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_30_SAI1_TX_BCLK               IMX1050_PIN_MUX(PIN_MUX_ALT3 | PIN_MUX_SI_ON)
#define  GPIO1_30_CSI_DATA03                 IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_30_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_30_USDHC2_DATA6               IMX1050_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_30_KPP_ROW00                  IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO1_31_FLEXSPIA_SS0_B             IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO1_31_ACMP_OUT03                 IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO1_31_LPSPI3_SCK                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO1_31_SAI1_TX_SYNC               IMX1050_PIN_MUX(PIN_MUX_ALT3 | PIN_MUX_SI_ON)
#define  GPIO1_31_CSI_DATA02                 IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO1_31_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO1_31_USDHC2_DATA7               IMX1050_PIN_MUX(PIN_MUX_ALT6)
#define  GPIO1_31_KPP_COL00                  IMX1050_PIN_MUX(PIN_MUX_ALT7)

/* GPIO 2 */

#define  GPIO2_0_LCD_CLK                     IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_0_QTIMER1_TIMER0              IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_0_MQS_RIGHT                   IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_0_LPSPI4_PCS0                 IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_0_FLEXIO2_FLEXIO00            IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_0_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_0_SEMC_CSX01                  IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO2_1_LCD_ENABLE                  IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_1_QTIMER1_TIMER1              IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_1_MQS_LEFT                    IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_1_LPSPI4_SDI                  IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_1_FLEXIO2_FLEXIO01            IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_1_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_1_SEMC_CSX02                  IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO2_2_LCD_HSYNC                   IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_2_QTIMER1_TIMER2              IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_2_FLEXCAN1_TX                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_2_LPSPI4_SDO                  IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_2_FLEXIO2_FLEXIO02            IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_2_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_2_SEMC_CSX03                  IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO2_3_LCD_VSYNC                   IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_3_QTIMER2_TIMER0              IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_3_FLEXCAN1_RX                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_3_LPSPI4_SCK                  IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_3_FLEXIO2_FLEXIO03            IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_3_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_3_WDOG2_RESET_B_DEB           IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO2_4_LCD_DATA00                  IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_4_QTIMER2_TIMER1              IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_4_LPI2C2_SCL                  IMX1050_PIN_MUX(PIN_MUX_ALT2 | PIN_MUX_SI_ON)
#define  GPIO2_4_FLEXIO2_FLEXIO04            IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_4_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_4_SRC_BOOT_CFG00              IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO2_5_LCD_DATA01                  IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_5_QTIMER2_TIMER2              IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_5_LPI2C2_SDA                  IMX1050_PIN_MUX(PIN_MUX_ALT2 | PIN_MUX_SI_ON)
#define  GPIO2_5_FLEXIO2_FLEXIO05            IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_5_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_5_SRC_BOOT_CFG01              IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO2_6_LCD_DATA02                  IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_6_QTIMER3_TIMER0              IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_6_FLEXPWM2_PWMA00             IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_6_FLEXIO2_FLEXIO06            IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_6_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_6_SRC_BOOT_CFG02              IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO2_7_LCD_DATA03                  IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_7_QTIMER3_TIMER1              IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_7_FLEXPWM2_PWMB00             IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_7_FLEXIO2_FLEXIO07            IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_7_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_7_SRC_BOOT_CFG03              IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO2_8_LCD_DATA04                  IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_8_QTIMER3_TIMER2              IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_8_FLEXPWM2_PWMA01             IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_8_LPUART3_TX                  IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_8_FLEXIO2_FLEXIO08            IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_8_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_8_SRC_BOOT_CFG04              IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO2_9_LCD_DATA05                  IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_9_QTIMER4_TIMER0              IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_9_FLEXPWM2_PWMB01             IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_9_LPUART3_RX                  IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_9_FLEXIO2_FLEXIO09            IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_9_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_9_SRC_BOOT_CFG05              IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO2_10_LCD_DATA06                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_10_QTIMER4_TIMER1             IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_10_FLEXPWM2_PWMA02            IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_10_SAI1_TX_DATA03             IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_10_FLEXIO2_FLEXIO10           IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_10_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_10_SRC_BOOT_CFG06             IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO2_11_LCD_DATA07                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_11_QTIMER4_TIMER2             IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_11_FLEXPWM2_PWMB02            IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_11_SAI1_TX_DATA02             IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_11_FLEXIO2_FLEXIO11           IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_11_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_11_SRC_BOOT_CFG07             IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO2_12_LCD_DATA08                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_12_XBAR1_INOUT10              IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_12_SAI1_TX_DATA01             IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_12_FLEXIO2_FLEXIO12           IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_12_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_12_SRC_BOOT_CFG08             IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO2_13_LCD_DATA09                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_13_XBAR1_INOUT11              IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_13_SAI1_MCLK                  IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_13_FLEXIO2_FLEXIO13           IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_13_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_13_SRC_BOOT_CFG09             IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO2_14_LCD_DATA10                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_14_XBAR1_INOUT12              IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_14_ARM_CM7_TXEV               IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_14_SAI1_RX_SYNC               IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_14_FLEXIO2_FLEXIO14           IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_14_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_14_SRC_BOOT_CFG10             IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO2_15_LCD_DATA11                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_15_XBAR1_INOUT13              IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_15_ARM_CM7_RXEV               IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_15_SAI1_RX_BCLK               IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_15_FLEXIO2_FLEXIO15           IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_15_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_15_SRC_BOOT_CFG11             IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO2_16_LCD_DATA12                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_16_XBAR1_INOUT14              IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_16_LPUART4_TX                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_16_SAI1_RX_DATA00             IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_16_FLEXIO2_FLEXIO16           IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_16_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_16_FLEXPWM1_PWMA03            IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO2_17_LCD_DATA13                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_17_XBAR1_INOUT15              IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_17_LPUART4_RX                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_17_SAI1_TX_DATA00             IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_17_FLEXIO2_FLEXIO17           IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_17_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_17_FLEXPWM1_PWMB03            IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO2_18_LCD_DATA14                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_18_XBAR1_INOUT16              IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_18_LPSPI4_PCS2                IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_18_SAI1_TX_BCLK               IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_18_FLEXIO2_FLEXIO18           IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_18_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_18_FLEXPWM2_PWMA03            IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO2_19_LCD_DATA15                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_19_XBAR1_INOUT17              IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_19_LPSPI4_PCS1                IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_19_SAI1_TX_SYNC               IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_19_FLEXIO2_FLEXIO19           IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_19_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_19_FLEXPWM2_PWMB03            IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO2_20_LCD_DATA16                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_20_LPSPI4_PCS0                IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_20_CSI_DATA15                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_20_ENET_RX_DATA00             IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_20_FLEXIO2_FLEXIO20           IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_20_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_20_CSU_CSU_ALARM_AUT02        IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO2_21_LCD_DATA17                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_21_LPSPI4_SDI                 IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_21_CSI_DATA14                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_21_ENET_RX_DATA01             IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_21_FLEXIO2_FLEXIO21           IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_21_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_21_CSU_CSU_ALARM_AUT01        IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO2_22_LCD_DATA18                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_22_LPSPI4_SDO                 IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_22_CSI_DATA13                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_22_ENET_RX_EN                 IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_22_FLEXIO2_FLEXIO22           IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_22_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_22_CSU_CSU_ALARM_AUT00        IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO2_23_LCD_DATA19                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_23_LPSPI4_SCK                 IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_23_CSI_DATA12                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_23_ENET_TX_DATA00             IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_23_FLEXIO2_FLEXIO23           IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_23_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_23_CSU_CSU_INT_DEB            IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO2_24_LCD_DATA20                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_24_QTIMER1_TIMER3             IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_24_CSI_DATA11                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_24_ENET_TX_DATA01             IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_24_FLEXIO2_FLEXIO24           IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_24_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_24_FLEXCAN2_TX                IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO2_25_LCD_DATA21                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_25_QTIMER2_TIMER3             IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_25_CSI_DATA10                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_25_ENET_TX_EN                 IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_25_FLEXIO2_FLEXIO25           IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_25_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_25_FLEXCAN2_RX                IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO2_26_LCD_DATA22                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_26_QTIMER3_TIMER3             IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_26_CSI_DATA00                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_26_ENET_TX_CLK                IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_26_FLEXIO2_FLEXIO26           IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_26_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_26_ENET_REF_CLK               IMX1050_PIN_MUX(PIN_MUX_ALT6 | PIN_MUX_SI_ON)

#define  GPIO2_27_LCD_DATA23                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_27_QTIMER4_TIMER3             IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_27_CSI_DATA01                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_27_ENET_RX_ER                 IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_27_FLEXIO2_FLEXIO27           IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_27_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_27_LPSPI4_PCS3                IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO2_28_LPUART5_TX                 IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_28_CSI_PIXCLK                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_28_ENET_1588_EVENT0_IN        IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_28_FLEXIO2_FLEXIO28           IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_28_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_28_USDHC1_CD_B                IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO2_29_WDOG1_B                    IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_29_LPUART5_RX                 IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_29_CSI_VSYNC                  IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_29_ENET_1588_EVENT0_OUT       IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_29_FLEXIO2_FLEXIO29           IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_29_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_29_USDHC1_WP                  IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO2_30_ENET_MDC                   IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_30_FLEXPWM4_PWMA02            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_30_CSI_HSYNC                  IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_30_XBAR1_IN02                 IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_30_FLEXIO2_FLEXIO30           IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_30_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_30_USDHC1_VSELECT             IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO2_31_ENET_MDIO                  IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO2_31_FLEXPWM4_PWMA03            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO2_31_CSI_MCLK                   IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO2_31_XBAR1_IN03                 IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO2_31_FLEXIO2_FLEXIO31           IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO2_31_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO2_31_USDHC1_RESET_B             IMX1050_PIN_MUX(PIN_MUX_ALT6)

/* GPIO3 */

#define  GPIO3_0_USDHC2_DATA3                IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_0_FLEXSPIB_DATA03             IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_0_FLEXPWM1_PWMA03             IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_0_SAI1_TX_DATA03              IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_0_LPUART4_TX                  IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_0_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO3_1_USDHC2_DATA2                IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_1_FLEXSPIB_DATA02             IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_1_FLEXPWM1_PWMB03             IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_1_SAI1_TX_DATA02              IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_1_LPUART4_RX                  IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_1_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_1_CCM_DI0_EXT_CLK             IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO3_2_USDHC2_DATA1                IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_2_FLEXSPIB_DATA01             IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_2_FLEXPWM2_PWMA03             IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_2_SAI1_TX_DATA01              IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_2_FLEXCAN1_TX                 IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_2_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_2_CCM_WAIT                    IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO3_3_USDHC2_DATA0                IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_3_FLEXSPIB_DATA00             IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_3_FLEXPWM2_PWMB03             IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_3_SAI1_MCLK                   IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_3_FLEXCAN1_RX                 IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_3_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_3_CCM_PMIC_READY              IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO3_4_USDHC2_CLK                  IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_4_FLEXSPIB_SCLK               IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_4_LPI2C1_SCL                  IMX1050_PIN_MUX(PIN_MUX_ALT2 | PIN_MUX_SI_ON)
#define  GPIO3_4_SAI1_RX_SYNC                IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_4_FLEXSPIA_SS1_B              IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_4_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_4_CCM_STOP                    IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO3_5_USDHC2_CMD                  IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_5_FLEXSPIA_DQS                IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_5_LPI2C1_SDA                  IMX1050_PIN_MUX(PIN_MUX_ALT2 | PIN_MUX_SI_ON)
#define  GPIO3_5_SAI1_RX_BCLK                IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_5_FLEXSPIB_SS0_B              IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_5_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO3_6_USDHC2_RESET_B              IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_6_FLEXSPIA_SS0_B              IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_6_LPUART7_CTS_B               IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_6_SAI1_RX_DATA00              IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_6_LPSPI2_PCS0                 IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_6_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO3_7_SEMC_CSX01                  IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_7_FLEXSPIA_SCLK               IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_7_LPUART7_RTS_B               IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_7_SAI1_TX_DATA00              IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_7_LPSPI2_SCK                  IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_7_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_7_CCM_REF_EN_B                IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO3_8_USDHC2_DATA4                IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_8_FLEXSPIA_DATA00             IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_8_LPUART7_TX                  IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_8_SAI1_TX_BCLK                IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_8_LPSPI2_SD0                  IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_8_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_8_SEMC_CSX02                  IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO3_9_USDHC2_DATA5                IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_9_FLEXSPIA_DATA01             IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_9_LPUART7_RX                  IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_9_SAI1_TX_SYNC                IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_9_LPSPI2_SDI                  IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_9_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO3_10_USDHC2_DATA6               IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_10_FLEXSPIA_DATA02            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_10_LPUART2_RX                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_10_LPI2C2_SDA                 IMX1050_PIN_MUX(PIN_MUX_ALT3 | PIN_MUX_SI_ON)
#define  GPIO3_10_LPSPI2_PCS2                IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_10_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_10_SRC_SYSTEM_RESET           IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO3_11_USDHC2_DATA7               IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_11_FLEXSPIA_DATA03            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_11_LPUART2_TX                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_11_LPI2C2_SCL                 IMX1050_PIN_MUX(PIN_MUX_ALT3 | PIN_MUX_SI_ON)
#define  GPIO3_11_LPSPI2_PCS3                IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_11_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_11_SRC_EARLY_RESET            IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO3_12_USDHC1_CMD                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_12_FLEXPWM1_PWMA00            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_12_LPI2C3_SCL                 IMX1050_PIN_MUX(PIN_MUX_ALT2 | PIN_MUX_SI_ON)
#define  GPIO3_12_XBAR1_INOUT04              IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_12_LPSPI1_SCK                 IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_12_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_12_FLEXSPIA_BUS2BIT_A_SS1_B   IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO3_13_USDHC1_CLK                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_13_FLEXPWM1_PWMB00            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_13_LPI2C3_SDA                 IMX1050_PIN_MUX(PIN_MUX_ALT2 | PIN_MUX_SI_ON)
#define  GPIO3_13_XBAR1_INOUT05              IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_13_LPSPI1_PCS0                IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_13_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_13_FLEXSPIB_BUS2BIT_B_SS1_B   IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO3_14_USDHC1_DATA0               IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_14_FLEXPWM1_PWMA01            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_14_LPUART8_CTS_B              IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_14_XBAR1_INOUT06              IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_14_LPSPI1_SDO                 IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_14_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO3_15_USDHC1_DATA1               IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_15_FLEXPWM1_PWMB01            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_15_LPUART8_RTS_B              IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_15_XBAR1_INOUT07              IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_15_LPSPI1_SDI                 IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_15_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO3_16_USDHC1_DATA2               IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_16_FLEXPWM1_PWMA02            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_16_LPUART8_TX                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_16_XBAR1_INOUT08              IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_16_FLEXSPIB_SS0_B             IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_16_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_16_CCM_CLKO1                  IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO3_17_USDHC1_DATA3               IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_17_FLEXPWM1_PWMB02            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_17_LPUART8_RX                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_17_XBAR1_INOUT09              IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_17_FLEXSPIB_DQS               IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_17_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_17_CCM_CLKO2                  IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO3_18_SEMC_DATA10                IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_18_FLEXPWM3_PWMB01            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_18_LPUART7_RX                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_18_CCM_PMIC_RDY               IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_18_CSI_DATA21                 IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_18_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO3_19_SEMC_DATA11                IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_19_FLEXPWM3_PWMA02            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_19_USDHC1_RESET_B             IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_19_SAI3_RX_DATA               IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_19_CSI_DATA20                 IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_19_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO3_20_SEMC_DATA12                IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_20_FLEXPWM3_PWMB02            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_20_USDHC1_VSELECT             IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_20_SAI3_RX_SYNC               IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_20_CSI_DATA19                 IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_20_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO3_21_SEMC_DATA13                IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_21_XBAR1_INOUT18              IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_21_GPT1_COMPARE1              IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_21_SAI3_RX_BCLK               IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_21_CSI_DATA18                 IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_21_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_21_USDHC1_CD_B                IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO3_22_SEMC_DATA14                IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_22_XBAR1_IN22                 IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_22_GPT1_COMPARE2              IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_22_SAI3_TX_DATA               IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_22_CSI_DATA17                 IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_22_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_22_USDHC1_WP                  IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO3_23_SEMC_DATA15                IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_23_XBAR1_IN23                 IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_23_GPT1_COMPARE3              IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_23_SAI3_MCLK                  IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_23_CSI_DATA16                 IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_23_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_23_USDHC2_WP                  IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO3_24_SEMC_DM01                  IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_24_FLEXPWM1_PWMA03            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_24_LPUART8_TX                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_24_SAI3_TX_BCLK               IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_24_CSI_FIELD                  IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_24_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_24_USDHC2_VSELECT             IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO3_25_SEMC_DQS                   IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_25_FLEXPWM1_PWMB03            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_25_LPUART8_RX                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_25_SAI3_TX_SYNC               IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_25_WDOG1_WDOG_B               IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_25_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_25_USDHC2_CD_B                IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO3_26_SEMC_RDY                   IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_26_GPT2_CAPTURE2              IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_26_LPSPI1_PCS2                IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_26_USB_OTG2_OC                IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_26_ENET_MDC                   IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_26_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_26_USDHC2_RESET_B             IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO3_27_SEMC_CSX00                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO3_27_GPT2_CAPTURE1              IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO3_27_LPSPI1_PCS3                IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO3_27_USB_OTG2_PWR               IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO3_27_ENET_MDIO                  IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO3_27_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO3_27_USDHC1_VSELECT             IMX1050_PIN_MUX(PIN_MUX_ALT6)


/*GPIO 4 */
#define  GPIO4_0_SEMC_DATA00                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_0_FLEXPWM4_PWMA00             IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_0_LPSPI2_SCK                  IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_0_XBAR1_XBAR_IN02             IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_0_FLEXIO1_FLEXIO00            IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_0_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_0_JTAG_DONE                   IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO4_1_SEMC_DATA01                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_1_FLEXPWM4_PWMB0              IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_1_LPSPI2_PCS0                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_1_XBAR1_IN03                  IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_1_FLEXIO1_FLEXIO01            IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_1_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_1_JTAG_DE_B                   IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO4_2_SEMC_DATA02                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_2_FLEXPWM4_PWMA01             IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_2_LPSPI2_SDO                  IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_2_XBAR1_INOUT04               IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_2_FLEXIO1_FLEXIO02            IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_2_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_2_JTAG_FAIL                   IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO4_3_SEMC_DATA03                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_3_FLEXPWM4_PWMB01             IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_3_LPSPI2_SDI                  IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_3_XBAR1_INOUT05               IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_3_FLEXIO1_FLEXIO03            IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_3_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_3_JTAG_ACTIVE                 IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO4_4_SEMC_DATA04                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_4_FLEXPWM4_PWMA02             IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_4_SAI2_TX_DATA                IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_4_XBAR1_INOUT06               IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_4_FLEXIO1_FLEXIO04            IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_4_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO4_5_SEMC_DATA05                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_5_FLEXPWM4_PWMB02             IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_5_SAI2_TX_SYNC                IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_5_XBAR1_INOUT07               IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_5_FLEXIO1_FLEXIO05            IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_5_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO4_6_SEMC_DATA06                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_6_FLEXPWM2_PWMA00             IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_6_SAI2_TX_BCLK                IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_6_XBAR1_INOUT08               IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_6_FLEXIO1_FLEXIO06            IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_6_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO4_7_SEMC_DATA07                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_7_FLEXPWM2_PWMB00             IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_7_SAI2_MCLK                   IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_7_XBAR1_INOUT09               IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_7_FLEXIO1_FLEXIO07            IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_7_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO4_8_SEMC_DM00                   IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_8_FLEXPWM2_PWMA01             IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_8_SAI2_RX_DATA                IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_8_XBAR1_INOUT17               IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_8_FLEXIO1_FLEXIO08            IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_8_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO4_9_SEMC_ADDR00                 IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_9_FLEXPWM2_PWMB01             IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_9_SAI2_RX_SYNC                IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_9_FLEXCAN2_TX                 IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_9_FLEXIO1_FLEXIO09            IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_9_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO4_10_SEMC_ADDR01                IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_10_FLEXPWM2_PWMA02            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_10_SAI2_RX_BCLK               IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_10_FLEXCAN2_RX                IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_10_FLEXIO1_FLEXIO10           IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_10_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO4_11_SEMC_ADDR02                IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_11_FLEXPWM2_PWMB02            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_11_LPI2C4_SDA                 IMX1050_PIN_MUX(PIN_MUX_ALT2 | PIN_MUX_SI_ON)
#define  GPIO4_11_USDHC2_RESET_B             IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_11_FLEXIO1_FLEXIO11           IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_11_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO4_12_SEMC_ADDR03                IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_12_XBAR1_IN24                 IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_12_LPI2C4_SCL                 IMX1050_PIN_MUX(PIN_MUX_ALT2 | PIN_MUX_SI_ON)
#define  GPIO4_12_USDHC1_WP                  IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_12_FLEXPWM1_PWMA03            IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_12_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO4_13_SEMC_ADDR04                IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_13_XBAR1_IN25                 IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_13_LPUART3_TX                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_13_MQS_RIGHT                  IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_13_FLEXPWM1_PWMB03            IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_13_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO4_14_SEMC_ADDR05                IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_14_XBAR1_INOUT19              IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_14_LPUART3_RX                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_14_MQS_LEFT                   IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_14_LPSPI2_PCS1                IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_14_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO4_15_SEMC_ADDR06                IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_15_XBAR1_IN20                 IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_15_LPUART3_CTS_B              IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_15_SPDIF_OUT                  IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_15_QTIMER3_TIMER0             IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_15_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO4_16_SEMC_ADDR07                IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_16_XBAR1_IN21                 IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_16_LPUART3_RTS_B              IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_16_SPDIF_IN                   IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_16_QTIMER3_TIMER1             IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_16_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO4_17_SEMC_ADDR08                IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_17_FLEXPWM4_PWMA03            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_17_LPUART4_CTS_B              IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_17_FLEXCAN1_TX                IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_17_QTIMER3_TIMER2             IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_17_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO4_18_SEMC_ADDR09                IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_18_FLEXPWM4_PWMB03            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_18_LPUART4_RTS_B              IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_18_FLEXCAN1_RX                IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_18_QTIMER3_TIMER3             IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_18_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_18_SNVS_VIO_5_CTL             IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO4_19_SEMC_ADDR11                IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_19_FLEXPWM2_PWMA03            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_19_LPUART4_TX                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_19_ENET_RDATA01               IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_19_QTIMER2_TIMER0             IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_19_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO4_19_SNVS_VIO_5                 IMX1050_PIN_MUX(PIN_MUX_ALT6)

#define  GPIO4_20_SEMC_ADDR12                IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_20_FLEXPWM2_PWMB03            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_20_LPUART4_RX                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_20_ENET_RDATA00               IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_20_QTIMER2_TIMER1             IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_20_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO4_21_SEMC_BA0                   IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_21_FLEXPWM3_PWMA03            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_21_LPI2C3_SDA                 IMX1050_PIN_MUX(PIN_MUX_ALT2 | PIN_MUX_SI_ON)
#define  GPIO4_21_ENET_TDATA01               IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_21_QTIMER2_TIMER2             IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_21_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO4_22_SEMC_BA1                   IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_22_FLEXPWM3_PWMB03            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_22_LPI2C3_SCL                 IMX1050_PIN_MUX(PIN_MUX_ALT2 | PIN_MUX_SI_ON)
#define  GPIO4_22_ENET_TDATA00               IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_22_QTIMER2_TIMER3             IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_22_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO4_23_SEMC_ADDR10                IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_23_FLEXPWM1_PWMA00            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_23_LPUART5_TX                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_23_ENET_RX_EN                 IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_23_GPT1_CAPTURE2              IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_23_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO4_24_SEMC_CAS                   IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_24_FLEXPWM1_PWMB00            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_24_LPUART5_RX                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_24_ENET_TX_EN                 IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_24_GPT1_CAPTURE1              IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_24_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO4_25_SEMC_RAS                   IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_25_FLEXPWM1_PWMA01            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_25_LPUART6_TX                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_25_ENET_TX_CLK                IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_25_ENET_REF_CLK               IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_25_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO4_26_SEMC_CLK                   IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_26_FLEXPWM1_PWMB01            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_26_LPUART6_RX                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_26_ENET_RX_ER                 IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_26_FLEXIO1_FLEXIO12           IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_26_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO4_27_SEMC_CKE                   IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_27_FLEXPWM1_PWMA02            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_27_LPUART5_RTS_B              IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_27_LPSPI1_SDO                 IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_27_FLEXIO1_FLEXIO1            IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_27_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO4_28_SEMC_WE                    IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_28_FLEXPWM1_PWMB02            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_28_LPUART5_CTS_B              IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_28_LPSPI1_SDO                 IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_28_FLEXIO1_FLEXIO14           IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_28_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO4_29_SEMC_CS0                   IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_29_FLEXPWM3_PWMA00            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_29_LPUART6_RTS_B              IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_29_LPSPI1_SDI                 IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_29_FLEXIO1_FLEXIO15           IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_29_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO4_30_SEMC_DATA08                IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_30_FLEXPWM3_PWMB00            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_30_LPUART6_CTS_B              IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_30_LPSPI1_PCS0                IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_30_CSI_DATA23                 IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_30_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO4_31_SEMC_DATA09                IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO4_31_FLEXPWM3_PWMA01            IMX1050_PIN_MUX(PIN_MUX_ALT1)
#define  GPIO4_31_LPUART7_TX                 IMX1050_PIN_MUX(PIN_MUX_ALT2)
#define  GPIO4_31_LPSPI1_PCS1                IMX1050_PIN_MUX(PIN_MUX_ALT3)
#define  GPIO4_31_CSI_DATA22                 IMX1050_PIN_MUX(PIN_MUX_ALT4)
#define  GPIO4_31_GPIO                       IMX1050_PIN_MUX(PIN_MUX_ALT5)

/* GPIO 5 */

#define  GPIO5_0_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)
#define  GPIO5_0_NMI_GLUE_NMI                IMX1050_PIN_MUX(PIN_MUX_ALT7)

#define  GPIO5_1_SNVS_LP_PMIC_ON_REQ         IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO5_1_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)

#define  GPIO5_2_CCM_PMIC_VSTBY_REQ          IMX1050_PIN_MUX(PIN_MUX_ALT0)
#define  GPIO5_2_GPIO                        IMX1050_PIN_MUX(PIN_MUX_ALT5)


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __IMX1050_PIN_PROPERTY_H */

/* end of file */

