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

/**
 * \file
 * \brief inum defines for i.MX1020 .
 *
 * \internal
 * \par modification history:
 * - 1.00 2018-09-21, mex, first implementation.
 * \endinternal
 */

#ifndef __IMX1020_INUM_H
#define __IMX1020_INUM_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_imx1020_inums IMX1020平台中断号定义
 * @{
 */

/** 
 * \name CPU中断号定义
 * @{ 
 */
#define IMX_EDMA_CHAN0_16       0   /**< \brief eDMA Channel 0 and 16 Transfer Complete */
#define IMX_EDMA_CHAN1_17       1   /**< \brief eDMA Channel 1 and 17 Transfer Complete */
#define IMX_EDMA_CHAN2_18       2   /**< \brief eDMA Channel 2 and 18 Transfer Complete */
#define IMX_EDMA_CHAN3_19       3   /**< \brief eDMA Channel 3 and 19 Transfer Complete */
#define IMX_EDMA_CHAN4_20       4   /**< \brief eDMA Channel 4 and 20 Transfer Complete */
#define IMX_EDMA_CHAN5_21       5   /**< \brief eDMA Channel 5 and 21 Transfer Complete */
#define IMX_EDMA_CHAN6_22       6   /**< \brief eDMA Channel 6 and 22 Transfer Complete */
#define IMX_EDMA_CHAN7_23       7   /**< \brief eDMA Channel 7 and 23 Transfer Complete */
#define IMX_EDMA_CHAN8_24       8   /**< \brief eDMA Channel 8 and 24 Transfer Complete */
#define IMX_EDMA_CHAN9_25       9   /**< \brief eDMA Channel 9 and 25 Transfer Complete */
#define IMX_EDMA_CHAN10_26      10  /**< \brief eDMA Channel 10 and 26 Transfer Complete */
#define IMX_EDMA_CHAN11_27      11  /**< \brief eDMA Channel 11 and 27 Transfer Complete */
#define IMX_EDMA_CHAN12_28      12  /**< \brief eDMA Channel 12 and 28 Transfer Complete */
#define IMX_EDMA_CHAN13_29      13  /**< \brief eDMA Channel 13 and 29 Transfer Complete */
#define IMX_EDMA_CHAN14_30      14  /**< \brief eDMA Channel 14 and 30 Transfer Complete */
#define IMX_EDMA_CHAN15_31      15  /**< \brief eDMA Channel 15 and 31 Transfer Complete */
#define IMX_EDMA_ERR            16  /**< \brief eDMA Error Interrupt, Channels 0-15 / 16-31 */
#define INUM_LPUART1            20  /**< \brief UART1 interrupt */
#define INUM_LPUART2            21  /**< \brief UART2 interrupt */
#define INUM_LPUART3            22  /**< \brief UART3 interrupt */
#define INUM_LPUART4            23  /**< \brief UART4 interrupt */
#define INUM_LPUART5            24  /**< \brief UART5 interrupt */
#define INUM_LPUART6            25  /**< \brief UART6 interrupt */
#define INUM_LPUART7            26  /**< \brief UART7 interrupt */
#define INUM_LPUART8            27  /**< \brief UART8 interrupt */
#define INUM_LPI2C1             28  /**< \brief I2C1 interrupt  */
#define INUM_LPI2C2             29  /**< \brief I2C2 interrupt  */
#define INUM_LPI2C3             30  /**< \brief I2C3 interrupt  */
#define INUM_LPI2C4             31  /**< \brief I2C4 interrupt  */
#define INUM_LPSPI1             32  /**< \brief LPSPI1 interrupt request line to the core */
#define INUM_LPSPI2             33  /**< \brief LPSPI2 interrupt request line to the core */
#define INUM_LPSPI3             34  /**< \brief LPSPI3 interrupt request line to the core */
#define INUM_LPSPI4             35  /**< \brief LPSPI4 interrupt request line to the core */
#define INUM_CAN1               36  /**< \brief CAN1 interrupt */
#define INUM_CAN2               37  /**< \brief CAN2 interrupt */
#define INUM_SAI1               56  /**< \brief SAI1 interrupt */
#define INUM_SAI2               57  /**< \brief SAI2 interrupt */
#define INUM_SAI3               59  /**< \brief SAI3 interrupt */
#define INUM_TEMPMON            63  /**< \brief TEMPMON interrupt */
#define INUM_ADC1               67  /**< \brief ADC1 interrupt */
#define INUM_ADC2               68  /**< \brief ADC2 interrupt */
#define INUM_GPIO1_0_15         80  /**< \brief Combined interrupt for GPIO1 signal 0 throughout 15 */
#define INUM_GPIO1_16_31        81  /**< \brief Combined interrupt for GPIO1 signal 16 throughout 31 */
#define INUM_GPIO2_0_15         82  /**< \brief Combined interrupt for GPIO2 signal 0 throughout 15 */
#define INUM_GPIO2_16_31        83  /**< \brief Combined interrupt for GPIO2 signal 16 throughout 31 */
#define INUM_GPIO3_0_15         84  /**< \brief Combined interrupt for GPIO3 signal 0 throughout 15 */
#define INUM_GPIO3_16_31        85  /**< \brief Combined interrupt for GPIO3 signal 16 throughout 31 */
#define INUM_GPIO5_0_15         88  /**< \brief Combined interrupt for GPIO5 signal 0 throughout 15 */
#define INUM_GPIO5_16_31        89  /**< \brief Combined interrupt for GPIO5 signal 16 throughout 31 */
#define INUM_FLEXIO1            90  /**< \brief FlexIO1 interrupt */
#define INUM_FLEXIO2            91  /**< \brief FlexIO2 interrupt */
#define INUM_GPT1               100 /**< \brief GPT1 all interrupts combined */
#define INUM_GPT2               101 /**< \brief GPT2 all interrupts combined */
#define INUM_FLEXSPI            108 /**< \brief Flexspi IRQ */
#define INUM_SEMC               109  /**< \brief SEMC interrupt */
#define INUM_USDHC1             110 /**< \brief USDHC1 interrupt */
#define INUM_USDHC2             111 /**< \brief USDHC2 interrupt */
#define INUM_USB_OTG1           113 /**< \brief USB_OTG1 interrupt */
#define INUM_ENET               114 /**< \brief ENET interrupt */
#define INUM_PIT                122 /**< \brief Combined interrupt for PIT */
#define INUM_ACMP1              123  /**< ACMP interrupt */
#define INUM_ACMP2              124  /**< ACMP interrupt */
#define INUM_ACMP3              125  /**< ACMP interrupt */
#define INUM_ACMP4              126  /**< ACMP interrupt */ 
#define INUM_ENC1               129  /**< \brief ENC1 interrupt */
#define INUM_ENC2               130  /**< \brief ENC2 interrupt */
#define INUM_QTIMER1            133  /**< QTimer1 interrupt */
#define INUM_QTIMER2            134  /**< QTimer2 interrupt */


/**@} CPU中断号定义*/

#ifdef __cplusplus
}
#endif


#endif /* __IMX1020_INUM_H */

/* end of file */

