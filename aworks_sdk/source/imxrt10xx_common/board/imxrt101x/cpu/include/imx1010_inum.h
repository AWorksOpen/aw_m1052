/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-present Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief inum defines for i.MX1010 .
 *
 * \internal
 * \par modification history:
 * - 1.00 2018-09-21, mex, first implementation.
 * \endinternal
 */

#ifndef __IMX1010_INUM_H
#define __IMX1010_INUM_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_imx1010_inums IMX1010平台中断号定义
 * @{
 */

/** 
 * \name CPU中断号定义
 * @{ 
 */
#define INUM_EDMA_CHAN0         0   /**< \brief eDMA Channel 0 Transfer Complete */
#define INUM_EDMA_CHAN1         1   /**< \brief eDMA Channel 1 Transfer Complete */
#define INUM_EDMA_CHAN2         2   /**< \brief eDMA Channel 2 Transfer Complete */
#define INUM_EDMA_CHAN3         3   /**< \brief eDMA Channel 3 Transfer Complete */
#define INUM_EDMA_CHAN4         4   /**< \brief eDMA Channel 4 Transfer Complete */
#define INUM_EDMA_CHAN5         5   /**< \brief eDMA Channel 5 Transfer Complete */
#define INUM_EDMA_CHAN6         6   /**< \brief eDMA Channel 6 Transfer Complete */
#define INUM_EDMA_CHAN7         7   /**< \brief eDMA Channel 7 Transfer Complete */
#define INUM_EDMA_CHAN8         8   /**< \brief eDMA Channel 8 Transfer Complete */
#define INUM_EDMA_CHAN9         9   /**< \brief eDMA Channel 9 Transfer Complete */
#define INUM_EDMA_CHAN10        10  /**< \brief eDMA Channel 10 Transfer Complete */
#define INUM_EDMA_CHAN11        11  /**< \brief eDMA Channel 11 Transfer Complete */
#define INUM_EDMA_CHAN12        12  /**< \brief eDMA Channel 12 Transfer Complete */
#define INUM_EDMA_CHAN13        13  /**< \brief eDMA Channel 13 Transfer Complete */
#define INUM_EDMA_CHAN14        14  /**< \brief eDMA Channel 14 Transfer Complete */
#define INUM_EDMA_CHAN15        15  /**< \brief eDMA Channel 15 Transfer Complete */
#define INUM_EDMA_ERR           16  /**< \brief eDMA Error Interrupt, Channels 0-15 / 16-31 */
#define INUM_CTI0_ERR           17  /**< \brief CTI trigger outputs */
#define INUM_CTI1_ERR           18  /**< \brief CTI trigger outputs */
#define INUM_CORE               19  /**< \brief CorePlatform exception IRQ */
#define INUM_LPUART1            20  /**< \brief UART1 interrupt */
#define INUM_LPUART2            21  /**< \brief UART2 interrupt */
#define INUM_LPUART3            22  /**< \brief UART3 interrupt */
#define INUM_LPUART4            23  /**< \brief UART4 interrupt */
#define INUM_PIT                24  /**< \brief Combined interrupt for PIT */
#define INUM_USB_OTG1           25  /**< \brief USB_OTG1 interrupt */
#define INUM_FLEXSPI            26  /**< \brief Flexspi IRQ */
#define INUM_FLEXRAM            27  /**< \brief FlexRAM address out of range Or access hit IRQ */
#define INUM_LPI2C1             28  /**< \brief I2C1 interrupt  */
#define INUM_LPI2C2             29  /**< \brief I2C2 interrupt  */
#define INUM_GPT1               30  /**< \brief GPT1 all interrupts combined */
#define INUM_GPT2               31  /**< \brief GPT2 all interrupts combined */
#define INUM_LPSPI1             32  /**< \brief LPSPI1 interrupt request line to the core */
#define INUM_LPSPI2             33  /**< \brief LPSPI2 interrupt request line to the core */
#define INUM_FLEXPWM1_0         34  /**< \brief FlexPWM1 PWM0 interrupt */
#define INUM_FLEXPWM1_1         35  /**< \brief FlexPWM1 PWM1 interrupt */
#define INUM_FLEXPWM1_2         36  /**< \brief FlexPWM1 PWM2 interrupt */
#define INUM_FLEXPWM1_3         37  /**< \brief FlexPWM1 PWM3 interrupt */
#define INUM_FLEXPWM1_ERR       38  /**< \brief FlexPWM1 fault interrupt */
#define INUM_KPP                39  /**< \brief Keypad interrupt */
#define INUM_SRC                40  /**< \brief SRC interrupt */
#define INUM_GPR                41  /**< \brief Used to notify cores on exception condition while boot */
#define INUM_CCM_1              42  /**< \brief CCM interrupt 1 */
#define INUM_CCM_2              43  /**< \brief CCM interrupt 2 */
#define INUM_EWM                44  /**< \brief EWM IRQ */
#define INUM_WDOG2              45  /**< \brief Watchdog Timer reset */
#define INUM_SNVS_HP_WRAPPER    46  /**< \brief SNVS Functional Interrupt */
#define INUM_SNVS_HP_WRAPPER_TZ 47  /**< \brief SNVS Security Interrupt */
#define INUM_SNVS_LP_WRAPPER    48  /**< \brief ON-OFF button press shorter than 5 secs (pulse event) */
#define INUM_CSU                49  /**< \brief CSU interrupt */
#define INUM_DCP                50  /**< \brief Combined DCP channel interrupts(except channel 0) and CRC interrupt */
#define INUM_DCP_VMI            51  /**< \brief IRQ of DCP channel 0 */
#define INUM_TRNG               53  /**< \brief TRNG Interrupt */
#define INUM_SAI1               56  /**< \brief SAI1 interrupt */
#define INUM_RTWDOG             57  /**< \brief Watchdog Timer reset */
#define INUM_SAI3_RX            58  /**< \brief SAI3 RX interrupt */
#define INUM_SAI3_TX            59  /**< \brief SAI3 TX interrupt */
#define INUM_SPDIF              60  /**< \brief SPDIF interrupt */
#define INUM_PMU                61  /**< \brief Brown-out event on either the 1.1, 2.5 or 3.0 regulators */
#define INUM_XBAR1              62  /**< \brief XBAR IRQ */
#define INUM_TEMPMON            63  /**< \brief TEMPMON interrupt */
#define INUM_USB_PHY            65  /**< \brief USBPHY interrupt */
#define INUM_GPC                66  /**< \brief GPC interrupt */
#define INUM_ADC1               67  /**< \brief ADC1 interrupt */
#define INUM_FLEXIO1            68  /**< \brief FlexIO interrupt */
#define INUM_DCDC               69  /**< \brief DCDC interrupt */
#define INUM_GPIO1_0_15         70  /**< \brief Combined interrupt for GPIO1 signal 0 throughout 15 */
#define INUM_GPIO1_16_31        71  /**< \brief Combined interrupt for GPIO1 signal 16 throughout 31 */
#define INUM_GPIO2_0_15         72  /**< \brief Combined interrupt for GPIO2 signal 0 throughout 15 */
#define INUM_GPIO5_0_15         73  /**< \brief Combined interrupt for GPIO5 signal 0 throughout 15 */
#define INUM_WDOG1              74  /**< \brief Watchdog Timer reset */
#define INUM_ADC_ETC_0          75  /**< \brief adc_etc IRQ0 */
#define INUM_ADC_ETC_1          76  /**< \brief adc_etc IRQ1 */
#define INUM_ADC_ETC_2          77  /**< \brief adc_etc IRQ2 */
#define INUM_ADC_ETC_3          78  /**< \brief adc_etc IRQ3 */
#define INUM_ADC_ETC_ERR        79  /**< \brief adc_etc Error IRQ */

/**@} CPU中断号定义*/

#ifdef __cplusplus
}
#endif

#endif /* __IMX1010_INUM_H */

/* end of file */
