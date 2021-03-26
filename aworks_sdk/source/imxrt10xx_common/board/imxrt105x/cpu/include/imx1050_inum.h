/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief inum defines for i.MX1050 .
 *
 * \internal
 * \par modification history:
 * - 1.00 2017-10-09, sni, first implementation.
 * \endinternal
 */

#ifndef __IMX1050_INUM_H
#define __IMX1050_INUM_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_imx1050_inums IMX1050平台中断号定义
 * @{
 */

/** 
 * \name CPU中断号定义
 * @{ 
 */

#define IMX_EDMA_CHAN0_16       0    /**< \brief eDMA Channel 0 and 16 Transfer Complete */
#define IMX_EDMA_CHAN1_17       1    /**< \brief eDMA Channel 1 and 17 Transfer Complete */
#define IMX_EDMA_CHAN2_18       2    /**< \brief eDMA Channel 2 and 18 Transfer Complete */
#define IMX_EDMA_CHAN3_19       3    /**< \brief eDMA Channel 3 and 19 Transfer Complete */
#define IMX_EDMA_CHAN4_20       4    /**< \brief eDMA Channel 4 and 20 Transfer Complete */
#define IMX_EDMA_CHAN5_21       5    /**< \brief eDMA Channel 5 and 21 Transfer Complete */
#define IMX_EDMA_CHAN6_22       6    /**< \brief eDMA Channel 6 and 22 Transfer Complete */
#define IMX_EDMA_CHAN7_23       7    /**< \brief eDMA Channel 7 and 23 Transfer Complete */
#define IMX_EDMA_CHAN8_24       8    /**< \brief eDMA Channel 8 and 24 Transfer Complete */
#define IMX_EDMA_CHAN9_25       9    /**< \brief eDMA Channel 9 and 25 Transfer Complete */
#define IMX_EDMA_CHAN10_26      10   /**< \brief eDMA Channel 10 and 26 Transfer Complete */
#define IMX_EDMA_CHAN11_27      11   /**< \brief eDMA Channel 11 and 27 Transfer Complete */
#define IMX_EDMA_CHAN12_28      12   /**< \brief eDMA Channel 12 and 28 Transfer Complete */
#define IMX_EDMA_CHAN13_29      13   /**< \brief eDMA Channel 13 and 29 Transfer Complete */
#define IMX_EDMA_CHAN14_30      14   /**< \brief eDMA Channel 14 and 30 Transfer Complete */
#define IMX_EDMA_CHAN15_31      15   /**< \brief eDMA Channel 15 and 31 Transfer Complete */
#define IMX_EDMA_ERR            16   /**< \brief eDMA Error Interrupt, Channels 0-15 / 16-31 */
#define INUM_LPUART1            20   /**< \brief UART1 interrupt */
#define INUM_LPUART2            21   /**< \brief UART2 interrupt */
#define INUM_LPUART3            22   /**< \brief UART3 interrupt */
#define INUM_LPUART4            23   /**< \brief UART4 interrupt */
#define INUM_LPUART5            24   /**< \brief UART5 interrupt */
#define INUM_LPUART6            25   /**< \brief UART6 interrupt */
#define INUM_LPUART7            26   /**< \brief UART7 interrupt */
#define INUM_LPUART8            27   /**< \brief UART8 interrupt */
#define INUM_LPI2C1             28   /**< \brief I2C1 interrupt  */
#define INUM_LPI2C2             29   /**< \brief I2C2 interrupt  */
#define INUM_LPI2C3             30   /**< \brief I2C3 interrupt  */
#define INUM_LPI2C4             31   /**< \brief I2C4 interrupt  */
#define INUM_LPSPI1             32   /**< \brief LPSPI1 interrupt request line to the core */
#define INUM_LPSPI2             33   /**< \brief LPSPI2 interrupt request line to the core */
#define INUM_LPSPI3             34   /**< \brief LPSPI3 interrupt request line to the core */
#define INUM_LPSPI4             35   /**< \brief LPSPI4 interrupt request line to the core */
#define INUM_CAN1               36   /**< \brief CAN1 interrupt */
#define INUM_CAN2               37   /**< \brief CAN2 interrupt */
#define INUM_LCDIF              42   /**< \brief LCDIF interrupt request line to the core */
#define INUM_CSI                43   /**< \brief CSI interrupt */
#define INUM_PXP                44   /**< \brief PXP interrupt */
#define INUM_SAI1               56   /**< \brief SAI1 interrupt */
#define INUM_SAI3               59   /**< \brief SAI3 interrupt */
#define INUM_TEMPMON            63   /**< \brief TEMPMON interrupt */
#define INUM_ADC1               67   /**< \brief ADC1 interrupt */
#define INUM_ADC2               68   /**< \brief ADC2 interrupt */
#define INUM_GPIO1_0_15         80   /**< \brief Combined interrupt for GPIO1 signal 0 throughout 15 */
#define INUM_GPIO1_16_31        81   /**< \brief Combined interrupt for GPIO1 signal 16 throughout 31 */
#define INUM_GPIO2_0_15         82   /**< \brief Combined interrupt for GPIO2 signal 0 throughout 15 */
#define INUM_GPIO2_16_31        83   /**< \brief Combined interrupt for GPIO2 signal 16 throughout 31 */
#define INUM_GPIO3_0_15         84   /**< \brief Combined interrupt for GPIO3 signal 0 throughout 15 */
#define INUM_GPIO3_16_31        85   /**< \brief Combined interrupt for GPIO3 signal 16 throughout 31 */
#define INUM_GPIO4_0_15         86   /**< \brief Combined interrupt for GPIO4 signal 0 throughout 15 */
#define INUM_GPIO4_16_31        87   /**< \brief Combined interrupt for GPIO4 signal 16 throughout 31 */
#define INUM_GPIO5_0_15         88   /**< \brief Combined interrupt for GPIO5 signal 0 throughout 15 */
#define INUM_GPIO5_16_31        89   /**< \brief Combined interrupt for GPIO5 signal 16 throughout 31 */
#define INUM_FLEXIO1            90   /**< \brief FlexIO1 interrupt */
#define INUM_FLEXIO2            91   /**< \brief FlexIO2 interrupt */
#define INUM_RTWDOG             93   /**< \brief RTWDOG interrupt */
#define INUM_GPT1               100  /**< \brief GPT1 all interrupts combined */
#define INUM_GPT2               101  /**< \brief GPT2 all interrupts combined */
#define INUM_FLEXSPI            108  /**< \brief Flexspi IRQ */
#define INUM_SEMC               109  /**< \brief SEMC interrupt */
#define INUM_USDHC1             110  /**< \brief USDHC1 interrupt */
#define INUM_USDHC2             111  /**< \brief USDHC2 interrupt */
#define INUM_USB_OTG2           112  /**< \brief USB_OTG2 interrupt */
#define INUM_USB_OTG1           113  /**< \brief USB_OTG1 interrupt */
#define INUM_ENET               114  /**< \brief ENET   interrupt */
#define INUM_PIT                122  /**< \brief Combined interrupt for PIT */
#define INUM_ACMP1              123  /**< \brief ACMP1 interrupt */
#define INUM_ACMP2              124  /**< \brief ACMP2 interrupt */
#define INUM_ACMP3              125  /**< \brief ACMP3 interrupt */
#define INUM_ACMP4              126  /**< \brief ACMP4 interrupt */
#define INUM_ENC1               129  /**< \brief ENC1 interrupt */
#define INUM_ENC2               130  /**< \brief ENC2 interrupt */
#define INUM_ENC3               131  /**< \brief ENC3 interrupt */
#define INUM_ENC4               132  /**< \brief ENC4 interrupt */
#define INUM_QTIMER1            133  /**< \brief QTimer1 interrupt */
#define INUM_QTIMER2            134  /**< \brief QTimer2 interrupt */
#define INUM_QTIMER3            135  /**< \brief QTimer3 interrupt */
#define INUM_QTIMER4            136  /**< \brief QTimer4 interrupt */

#define INUM_GPIO1_0            137  /**< \brief GPIO1_0  interrupt */
#define INUM_GPIO1_1            138  /**< \brief GPIO1_1  interrupt */
#define INUM_GPIO1_2            139  /**< \brief GPIO1_2  interrupt */
#define INUM_GPIO1_3            140  /**< \brief GPIO1_3  interrupt */
#define INUM_GPIO1_4            141  /**< \brief GPIO1_4  interrupt */
#define INUM_GPIO1_5            142  /**< \brief GPIO1_5  interrupt */
#define INUM_GPIO1_6            143  /**< \brief GPIO1_6  interrupt */
#define INUM_GPIO1_7            144  /**< \brief GPIO1_7  interrupt */
#define INUM_GPIO1_8            145  /**< \brief  GPIO1_8  interrupt */
#define INUM_GPIO1_9            146  /**< \brief GPIO1_9  interrupt */
#define INUM_GPIO1_10           147  /**< \brief GPIO1_10 interrupt */
#define INUM_GPIO1_11           148  /**< \brief GPIO1_11 interrupt */
#define INUM_GPIO1_12           149  /**< \brief GPIO1_12 interrupt */
#define INUM_GPIO1_13           150  /**< \brief GPIO1_13 interrupt */
#define INUM_GPIO1_14           151  /**< \brief GPIO1_14 interrupt */
#define INUM_GPIO1_15           152  /**< \brief GPIO1_15 interrupt */
#define INUM_GPIO1_16           153  /**< \brief GPIO1_16 interrupt */
#define INUM_GPIO1_17           154  /**< \brief GPIO1_17 interrupt */
#define INUM_GPIO1_18           155  /**< \brief GPIO1_18 interrupt */
#define INUM_GPIO1_19           156  /**< \brief GPIO1_19 interrupt */
#define INUM_GPIO1_20           157  /**< \brief GPIO1_20 interrupt */
#define INUM_GPIO1_21           158  /**< \brief GPIO1_21 interrupt */
#define INUM_GPIO1_22           159  /**< \brief GPIO1_22 interrupt */
#define INUM_GPIO1_23           160  /**< \brief GPIO1_23 interrupt */
#define INUM_GPIO1_24           161  /**< \brief GPIO1_24 interrupt */
#define INUM_GPIO1_25           162  /**< \brief GPIO1_25 interrupt */
#define INUM_GPIO1_26           163  /**< \brief GPIO1_26 interrupt */
#define INUM_GPIO1_27           164  /**< \brief GPIO1_27 interrupt */
#define INUM_GPIO1_28           165  /**< \brief GPIO1_28 interrupt */
#define INUM_GPIO1_29           166  /**< \brief GPIO1_29 interrupt */
#define INUM_GPIO1_30           167  /**< \brief GPIO1_30 interrupt */
#define INUM_GPIO1_31           168  /**< \brief GPIO1_31 interrupt */

#define INUM_GPIO2_0            169  /**< \brief GPIO2_0  interrupt */
#define INUM_GPIO2_1            170  /**< \brief GPIO2_1  interrupt */
#define INUM_GPIO2_2            171  /**< \brief GPIO2_2  interrupt */
#define INUM_GPIO2_3            172  /**< \brief GPIO2_3  interrupt */
#define INUM_GPIO2_4            173  /**< \brief GPIO2_4  interrupt */
#define INUM_GPIO2_5            174  /**< \brief GPIO2_5  interrupt */
#define INUM_GPIO2_6            175  /**< \brief GPIO2_6  interrupt */
#define INUM_GPIO2_7            176  /**< \brief GPIO2_7  interrupt */
#define INUM_GPIO2_8            177  /**< \brief GPIO2_8  interrupt */
#define INUM_GPIO2_9            178  /**< \brief GPIO2_9  interrupt */
#define INUM_GPIO2_10           179  /**< \brief GPIO2_10 interrupt */
#define INUM_GPIO2_11           180  /**< \brief GPIO2_11 interrupt */
#define INUM_GPIO2_12           181  /**< \brief GPIO2_12 interrupt */
#define INUM_GPIO2_13           182  /**< \brief GPIO2_13 interrupt */
#define INUM_GPIO2_14           183  /**< \brief GPIO2_14 interrupt */
#define INUM_GPIO2_15           184  /**< \brief GPIO2_15 interrupt */
#define INUM_GPIO2_16           185  /**< \brief GPIO2_16 interrupt */
#define INUM_GPIO2_17           186  /**< \brief GPIO2_17 interrupt */
#define INUM_GPIO2_18           187  /**< \brief GPIO2_18 interrupt */
#define INUM_GPIO2_19           188  /**< \brief GPIO2_19 interrupt */
#define INUM_GPIO2_20           189  /**< \brief GPIO2_20 interrupt */
#define INUM_GPIO2_21           190  /**< \brief GPIO2_21 interrupt */
#define INUM_GPIO2_22           191  /**< \brief GPIO2_22 interrupt */
#define INUM_GPIO2_23           192  /**< \brief GPIO2_23 interrupt */
#define INUM_GPIO2_24           193  /**< \brief GPIO2_24 interrupt */
#define INUM_GPIO2_25           194  /**< \brief GPIO2_25 interrupt */
#define INUM_GPIO2_26           195  /**< \brief GPIO2_26 interrupt */
#define INUM_GPIO2_27           196  /**< \brief GPIO2_27 interrupt */
#define INUM_GPIO2_28           197  /**< \brief GPIO2_28 interrupt */
#define INUM_GPIO2_29           198  /**< \brief GPIO2_29 interrupt */
#define INUM_GPIO2_30           199  /**< \brief GPIO2_30 interrupt */
#define INUM_GPIO2_31           200  /**< \brief GPIO2_31 interrupt */

#define INUM_GPIO3_0            201  /**< \brief GPIO3_0  interrupt */
#define INUM_GPIO3_1            202  /**< \brief GPIO3_1  interrupt */
#define INUM_GPIO3_2            203  /**< \brief GPIO3_2  interrupt */
#define INUM_GPIO3_3            204  /**< \brief GPIO3_3  interrupt */
#define INUM_GPIO3_4            205  /**< \brief GPIO3_4  interrupt */
#define INUM_GPIO3_5            206  /**< \brief GPIO3_5  interrupt */
#define INUM_GPIO3_6            207  /**< \brief GPIO3_6  interrupt */
#define INUM_GPIO3_7            208  /**< \brief GPIO3_7  interrupt */
#define INUM_GPIO3_8            209  /**< \brief GPIO3_8  interrupt */
#define INUM_GPIO3_9            210  /**< \brief GPIO3_9  interrupt */
#define INUM_GPIO3_10           211  /**< \brief GPIO3_10 interrupt */
#define INUM_GPIO3_11           212  /**< \brief GPIO3_11 interrupt */
#define INUM_GPIO3_12           213  /**< \brief GPIO3_12 interrupt */
#define INUM_GPIO3_13           214  /**< \brief GPIO3_13 interrupt */
#define INUM_GPIO3_14           215  /**< \brief GPIO3_14 interrupt */
#define INUM_GPIO3_15           216  /**< \brief GPIO3_15 interrupt */
#define INUM_GPIO3_16           217  /**< \brief GPIO3_16 interrupt */
#define INUM_GPIO3_17           218  /**< \brief GPIO3_17 interrupt */
#define INUM_GPIO3_18           219  /**< \brief GPIO3_18 interrupt */
#define INUM_GPIO3_19           220  /**< \brief GPIO3_19 interrupt */
#define INUM_GPIO3_20           221  /**< \brief GPIO3_20 interrupt */
#define INUM_GPIO3_21           222  /**< \brief GPIO3_21 interrupt */
#define INUM_GPIO3_22           223  /**< \brief GPIO3_22 interrupt */
#define INUM_GPIO3_23           224  /**< \brief GPIO3_23 interrupt */
#define INUM_GPIO3_24           225  /**< \brief GPIO3_24 interrupt */
#define INUM_GPIO3_25           226  /**< \brief GPIO3_25 interrupt */
#define INUM_GPIO3_26           227  /**< \brief GPIO3_26 interrupt */
#define INUM_GPIO3_27           228  /**< \brief GPIO3_27 interrupt */
#define INUM_GPIO3_28           229  /**< \brief GPIO3_28 interrupt */
#define INUM_GPIO3_29           230  /**< \brief GPIO3_29 interrupt */
#define INUM_GPIO3_30           231  /**< \brief GPIO3_30 interrupt */
#define INUM_GPIO3_31           232  /**< \brief GPIO3_31 interrupt */

#define INUM_GPIO4_0            233  /**< \brief GPIO4_0  interrupt */
#define INUM_GPIO4_1            234  /**< \brief GPIO4_1  interrupt */
#define INUM_GPIO4_2            235  /**< \brief GPIO4_2  interrupt */
#define INUM_GPIO4_3            236  /**< \brief GPIO4_3  interrupt */
#define INUM_GPIO4_4            237  /**< \brief GPIO4_4  interrupt */
#define INUM_GPIO4_5            238  /**< \brief GPIO4_5  interrupt */
#define INUM_GPIO4_6            239  /**< \brief GPIO4_6  interrupt */
#define INUM_GPIO4_7            240  /**< \brief GPIO4_7  interrupt */
#define INUM_GPIO4_8            241  /**< \brief GPIO4_8  interrupt */
#define INUM_GPIO4_9            242  /**< \brief GPIO4_9  interrupt */
#define INUM_GPIO4_10           243  /**< \brief GPIO4_10 interrupt */
#define INUM_GPIO4_11           244  /**< \brief GPIO4_11 interrupt */
#define INUM_GPIO4_12           245  /**< \brief GPIO4_12 interrupt */
#define INUM_GPIO4_13           246  /**< \brief GPIO4_13 interrupt */
#define INUM_GPIO4_14           247  /**< \brief GPIO4_14 interrupt */
#define INUM_GPIO4_15           248  /**< \brief GPIO4_15 interrupt */
#define INUM_GPIO4_16           249  /**< \brief GPIO4_16 interrupt */
#define INUM_GPIO4_17           250  /**< \brief GPIO4_17 interrupt */
#define INUM_GPIO4_18           251  /**< \brief GPIO4_18 interrupt */
#define INUM_GPIO4_19           252  /**< \brief GPIO4_19 interrupt */
#define INUM_GPIO4_20           253  /**< \brief GPIO4_20 interrupt */
#define INUM_GPIO4_21           254  /**< \brief GPIO4_21 interrupt */
#define INUM_GPIO4_22           255  /**< \brief GPIO4_22 interrupt */
#define INUM_GPIO4_23           256  /**< \brief GPIO4_23 interrupt */
#define INUM_GPIO4_24           257  /**< \brief GPIO4_24 interrupt */
#define INUM_GPIO4_25           258  /**< \brief GPIO4_25 interrupt */
#define INUM_GPIO4_26           259  /**< \brief GPIO4_26 interrupt */
#define INUM_GPIO4_27           260  /**< \brief GPIO4_27 interrupt */
#define INUM_GPIO4_28           261  /**< \brief GPIO4_28 interrupt */
#define INUM_GPIO4_29           262  /**< \brief GPIO4_29 interrupt */
#define INUM_GPIO4_30           263  /**< \brief GPIO4_30 interrupt */
#define INUM_GPIO4_31           264  /**< \brief GPIO4_31 interrupt */

#define INUM_GPIO5_0            265  /**< \brief GPIO5_0 interrupt */
#define INUM_GPIO5_1            266  /**< \brief GPIO5_1 interrupt */
#define INUM_GPIO5_2            267  /**< \brief GPIO5_2 interrupt */

/**@} CPU中断号定义*/

#ifdef __cplusplus
}
#endif


#endif /* } __IMX1050_INUM_H */

/* end of file */

