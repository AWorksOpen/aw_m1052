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
 * \brief IMX1050各模块寄存器基址
 *
 * 提供各模块的寄存器组的物理基地址
 *
 * \par 1.兼容设备
 *
 *  - IMX1050
 *
 * \internal
 * \par modification history
 * - 1.00 17-09-01  sni, first implementation
 * \endinternal
 */

#ifndef __IMX1050_REG_BASE_H
#define __IMX1050_REG_BASE_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus*/

/**
 * \addtogroup grp_imx1050_bsp IMX1050 支持
 * @{
 */

/**
 * \addtogroup grp_imx1050_bsp_modules_base 各模块基地址
 * @{
 */

/**
 * \name Memory space
 * @{
 */

#define IMX1050_CCM_BASE                0x400FC000UL    /**< \brief IMX1050 CCM base address */
#define IMX1050_CCM_ANALOG_BASE         0x400D8000UL    /**< \brief IMX1050 CCM analog base address */
#define IMX1050_XTALOSC24M_BASE         0x400D8000UL    /**< \brief XTALOSC24M base address */
#define IMX1050_DCDC_BASE               0x40080000UL    /**< \brief DCDC base address */
#define IMX1050_PMU_BASE                0x400D8000UL    /**< \brief PMU base address */

#define IMX1050_LPUART1_BASE            0x40184000      /**< \brief IMX1050 lpuart1 base address */
#define IMX1050_LPUART2_BASE            0x40188000      /**< \brief IMX1050 lpuart2 base address */
#define IMX1050_LPUART3_BASE            0x4018C000      /**< \brief IMX1050 lpuart3 base address */
#define IMX1050_LPUART4_BASE            0x40190000      /**< \brief IMX1050 lpuart4 base address */
#define IMX1050_LPUART5_BASE            0x40194000      /**< \brief IMX1050 lpuart5 base address */
#define IMX1050_LPUART6_BASE            0x40198000      /**< \brief IMX1050 lpuart6 base address */
#define IMX1050_LPUART7_BASE            0x4019C000      /**< \brief IMX1050 lpuart7 base address */
#define IMX1050_LPUART8_BASE            0x401A0000      /**< \brief IMX1050 lpuart8 base address */
#define IMX1050_GPIO1_BASE              0x401B8000      /**< \brief IMX1050 GPIO1 base address */
#define IMX1050_GPIO2_BASE              0x401BC000      /**< \brief IMX1050 GPIO2 base address */
#define IMX1050_GPIO3_BASE              0x401C0000      /**< \brief IMX1050 GPIO3 base address */
#define IMX1050_GPIO4_BASE              0x401C4000      /**< \brief IMX1050 GPIO4 base address */
#define IMX1050_GPIO5_BASE              0x400C0000      /**< \brief IMX1050 GPIO5 base address */
#define IMX1050_IOMUXC_SNVS_BASE        0x400A8000      /**< \brief IMX1050 IOMUXC SNVS base address */
#define IMX1050_IOMUXC_GPR_BASE         0x400AC000      /**< \brief IMX1050 IOMUXC GPR base address */
#define IMX1050_IOMUXC_BASE             0x401F8000      /**< \brief IMX1050 IOMUXC base address */
#define IMX1050_LPSPI1_BASE             0x40394000      /**< \brief IMX1050 lpspi1 base address */
#define IMX1050_LPSPI2_BASE             0x40398000      /**< \brief IMX1050 lpspi2 base address */
#define IMX1050_LPSPI3_BASE             0x4039C000      /**< \brief IMX1050 lpspi3 base address */
#define IMX1050_LPSPI4_BASE             0x403A0000      /**< \brief IMX1050 lpspi4 base address */
#define IMX1050_WDOG1_BASE              0x400B8000      /**< \brief IMX1050 wdog1 base address */
#define IMX1050_WDOG2_BASE              0x400D0000      /**< \brief IMX1050 wdog2 base address */
#define IMX1050_RTWDOG_BASE             0x400BC000      /**< \brief IMX1050 rtwdog base address */
#define IMX1050_GPT1_BASE               0x401EC000      /**< \brief IMX1050 GPT1 base address */
#define IMX1050_GPT2_BASE               0x401F0000      /**< \brief IMX1050 GPT2 base address */
#define IMX1050_PIT_BASE                0x40084000      /**< \brief IMX1050 PIT base address */
#define IMX1050_ADC1_BASE               0X400C4000      /**< \brief IMX1050 ADC1 base address */
#define IMX1050_ADC2_BASE               0X400C8000      /**< \brief IMX1050 ADC2 base address */
#define IMX1050_USDHC1_BASE             0x402C0000      /**< \brief IMX1050 USDHC1 base address */
#define IMX1050_USDHC2_BASE             0x402C4000      /**< \brief IMX1050 USDHC2 base address */
#define IMX1050_EDMA_BASE               0x400E8000      /**< \brief IMX1050 eDMA base address */
#define IMX1050_DMAMUX_BASE             0x400EC000      /**< \brief IMX1050 DMA MUX base address */
#define IMX1050_SAI1_BASE               0x40384000      /**< \brief IMX1050 SAI1 base address */
#define IMX1050_SAI3_BASE               0x4038C000      /**< \brief IMX1050 SAI3 base address */
#define IMX1050_LPI2C1_BASE             0x403F0000      /**< \brief IMX1050 LPI2C1 base address */
#define IMX1050_LPI2C2_BASE             0x403F4000      /**< \brief IMX1050 LPI2C2 base address */
#define IMX1050_LPI2C3_BASE             0x403F8000      /**< \brief IMX1050 LPI2C3 base address */
#define IMX1050_LPI2C4_BASE             0x403FC000      /**< \brief IMX1050 LPI2C4 base address */
#define IMX1050_OCOTP_BASE              0x401F4000      /**< \brief IMX1050 OCOTP base address */
#define IMX1050_CAN1_BASE               0x401D0000      /**< \brief IMX1050 USDHC2 base address */
#define IMX1050_CAN2_BASE               0X401D4000      /**< \brief IMX1050 USDHC2 base address */
#define IMX1050_LCDIF_BASE              0x402B8000      /**< \brief IMX1050 LCDIF base address */
#define IMX1050_TS_CTRL_BASE            0x400E0000      /**< \brief IMX1050 LCDIF base address */
#define IMX1050_FLEXSPI_BASE            0x402A8000      /**< \brief IMX1050 Flexspi base address */
#define IMX1050_FLEXPWM1_BASE           0x403DC000      /**< \brief IMX1050 eFlexPWM1 base address */
#define IMX1050_FLEXPWM2_BASE           0x403E0000      /**< \brief IMX1050 eFlexPWM2 base address */
#define IMX1050_FLEXPWM3_BASE           0x403E4000      /**< \brief IMX1050 eFlexPWM3 base address */
#define IMX1050_FLEXPWM4_BASE           0x403E8000      /**< \brief IMX1050 eFlexPWM4 base address */
#define IMX1050_XBARA1_BASE             0x403BC000      /**< \brief IMX1050 XBARA1 base address */
#define IMX1050_QTIMER1_BASE            0x401DC000      /**< \brief IMX1050 QTimer1 base address */
#define IMX1050_QTIMER2_BASE            0x401E0000      /**< \brief IMX1050 QTimer2 base address */
#define IMX1050_QTIMER3_BASE            0x401E4000      /**< \brief IMX1050 QTimer3 base address */
#define IMX1050_QTIMER4_BASE            0x401E8000      /**< \brief IMX1050 QTimer4 base address */
#define IMX1050_SEMC_BASE               0x402F0000      /**< \brief IMX1050 SEMC base address */
#define IMX1050_FLEXIO1_BASE            0x401AC000      /**< \brief IMX1050 FlexIO1 base address */
#define IMX1050_FLEXIO2_BASE            0x401B0000      /**< \brief IMX1050 FlexIO2 base address */
#define IMX1050_USB1_BASE_ADDR          0x402E0000      /**< \brief IMX1050 USB1 base address */
#define IMX1050_USB2_BASE_ADDR          0x402E0200      /**< \brief IMX1050 USB2 base address */
#define IMX1050_USBPHY1_BASE_ADDR       0x400D9000      /**< \brief IMX1050 USBPHY1 base address */
#define IMX1050_USBPHY2_BASE_ADDR       0x400DA000      /**< \brief IMX1050 USBPHY2 base address */
#define IMX1050_ENC1_BASE               0x403C8000      /**< \brief IMX1050 ENC1 base address */
#define IMX1050_ENC2_BASE               0x403CC000      /**< \brief IMX1050 ENC2 base address */
#define IMX1050_ENC3_BASE               0x403D0000      /**< \brief IMX1050 ENC3 base address */
#define IMX1050_ENC4_BASE               0x403D4000      /**< \brief IMX1050 ENC4 base address */
#define IMX1050_TEMPMON_BASE            0x400D8180      /**< \brief IMX1050 TEMPMON base address */
#define IMX1050_ACMP1_BASE              0x40094000      /**< \brief IMX1050 TEMPMON base address */
#define IMX1050_ACMP2_BASE              0x40094008      /**< \brief IMX1050 TEMPMON base address */
#define IMX1050_ACMP3_BASE              0x40094010      /**< \brief IMX1050 TEMPMON base address */
#define IMX1050_ACMP4_BASE              0x40094018      /**< \brief IMX1050 TEMPMON base address */
#define IMX1050_PXP_BASE                0x402B4000      /**< \brief IMX1050 PXP base address */
#define IMX1050_CSI_BASE                0x402BC000      /**< \brief IMX1050 CSI base address */
#define IMX1050_ENET_BASE               0x402D8000      /**< \brief IMX1050 ENET base address */
#define IMX1050_SNVS_BASE               0x400D4000      /**< \brief IMX1050 SNVS base address */

/** @} */


/** @} end of grp_imx1050_bsp_modules_base */
/** @} end of grp_imx1050_bsp */


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __IMX1050_REGBASE_H */

/* end of file */
