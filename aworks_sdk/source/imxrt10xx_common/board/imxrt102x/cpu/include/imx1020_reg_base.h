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
 * \brief IMX1020各模块寄存器基址
 *
 * 提供各模块的寄存器组的物理基地址
 *
 * \par 1.兼容设备
 *
 *  - IMX1020
 *
 * \internal
 * \par modification history
 * - 1.01 20-04-28  cml, add XTALOSC and PMU reg base
 * - 1.00 18-09-21  mex, first implementation
 * \endinternal
 */

#ifndef __IMX1020_REG_BASE_H
#define __IMX1020_REG_BASE_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus*/

/**
 * \addtogroup grp_imx1020_bsp IMX1020 支持
 * @{
 */

/**
 * \addtogroup grp_imx1020_bsp_modules_base 各模块基地址
 * @{
 */

/**
 * \name Memory space
 * @{
 */

#define IMX1020_CCM_BASE                0x400FC000UL    /**< \brief IMX1020 CCM base address */
#define IMX1020_CCM_ANALOG_BASE         0x400D8000UL    /**< \brief IMX1020 CCM analog base address */
#define IMX1020_XTALOSC24M_BASE         0x400D8000UL    /**< \brief XTALOSC24M base address */
#define IMX1020_PMU_BASE                0x400D8000UL    /**< \brief PMU base address */

#define IMX1020_LPUART1_BASE            0x40184000      /**< \brief IMX1020 lpuart1 base address */
#define IMX1020_LPUART2_BASE            0x40188000      /**< \brief IMX1020 lpuart2 base address */
#define IMX1020_LPUART3_BASE            0x4018C000      /**< \brief IMX1020 lpuart3 base address */
#define IMX1020_LPUART4_BASE            0x40190000      /**< \brief IMX1020 lpuart4 base address */
#define IMX1020_LPUART5_BASE            0x40194000      /**< \brief IMX1020 lpuart5 base address */
#define IMX1020_LPUART6_BASE            0x40198000      /**< \brief IMX1020 lpuart6 base address */
#define IMX1020_LPUART7_BASE            0x4019C000      /**< \brief IMX1020 lpuart7 base address */
#define IMX1020_LPUART8_BASE            0x401A0000      /**< \brief IMX1020 lpuart8 base address */
#define IMX1020_GPIO1_BASE              0x401B8000      /**< \brief IMX1020 GPIO1 base address */
#define IMX1020_GPIO2_BASE              0x401BC000      /**< \brief IMX1020 GPIO2 base address */
#define IMX1020_GPIO3_BASE              0x401C0000      /**< \brief IMX1020 GPIO3 base address */
#define IMX1020_GPIO5_BASE              0x400C0000      /**< \brief IMX1020 GPIO5 base address */
#define IMX1020_IOMUXC_SNVS_BASE        0x400A8000      /**< \brief IMX1020 IOMUXC SNVS base address */
#define IMX1020_IOMUXC_GPR_BASE         0x400AC000      /**< \brief IMX1020 IOMUXC GPR base address */
#define IMX1020_IOMUXC_BASE             0x401F8000      /**< \brief IMX1020 IOMUXC base address */
#define IMX1020_LPSPI1_BASE             0x40394000      /**< \brief IMX1020 lpspi1 base address */
#define IMX1020_LPSPI2_BASE             0x40398000      /**< \brief IMX1020 lpspi2 base address */
#define IMX1020_LPSPI3_BASE             0x4039C000      /**< \brief IMX1020 lpspi3 base address */
#define IMX1020_LPSPI4_BASE             0x403A0000      /**< \brief IMX1020 lpspi4 base address */
#define IMX1020_WDOG1_BASE              0x400B8000      /**< \brief IMX1020 wdog1 base address */
#define IMX1020_RTWDOG_BASE             0x400BC000      /**< \brief IMX1020 rtwdog base address */
#define IMX1020_GPT1_BASE               0x401EC000      /**< \brief IMX1020 GPT1 base address */
#define IMX1020_GPT2_BASE               0x401F0000      /**< \brief IMX1020 GPT2 base address */
#define IMX1020_PIT_BASE                0x40084000      /**< \brief IMX1020 PIT base address */
#define IMX1020_ADC1_BASE               0X400C4000      /**< \brief IMX1020 ADC1 base address */
#define IMX1020_ADC2_BASE               0X400C8000      /**< \brief IMX1020 ADC2 base address */
#define IMX1020_USDHC1_BASE             0x402C0000      /**< \brief IMX1020 USDHC1 base address */
#define IMX1020_USDHC2_BASE             0x402C4000      /**< \brief IMX1020 USDHC2 base address */
#define IMX1020_EDMA_BASE               0x400E8000      /**< \brief IMX1020 eDMA base address */
#define IMX1020_DMAMUX_BASE             0x400EC000      /**< \brief IMX1020 DMA MUX base address */
#define IMX1020_SAI1_BASE               0x40384000      /**< \brief IMX1020 SAI1 base address */
#define IMX1020_SAI3_BASE               0x4038C000      /**< \brief IMX1020 SAI3 base address */
#define IMX1020_LPI2C1_BASE             0x403F0000      /**< \brief IMX1020 LPI2C1 base address */
#define IMX1020_LPI2C2_BASE             0x403F4000      /**< \brief IMX1020 LPI2C2 base address */
#define IMX1020_LPI2C3_BASE             0x403F8000      /**< \brief IMX1020 LPI2C3 base address */
#define IMX1020_LPI2C4_BASE             0x403FC000      /**< \brief IMX1020 LPI2C4 base address */
#define IMX1020_OCOTP_BASE              0x401F4000      /**< \brief IMX1020 OCOTP base address */
#define IMX1020_CAN1_BASE               0x401D0000      /**< \brief IMX1020 USDHC2 base address */
#define IMX1020_CAN2_BASE               0X401D4000      /**< \brief IMX1020 USDHC2 base address */
#define IMX1020_FLEXSPI_BASE            0x402A8000      /**< \brief IMX1020 Flexspi base address */
#define IMX1020_FLEXPWM1_BASE           0x403DC000      /**< \brief IMX1020 eFlexPWM1 base address */
#define IMX1020_FLEXPWM2_BASE           0x403E0000      /**< \brief IMX1020 eFlexPWM2 base address */
#define IMX1020_XBARA1_BASE             0x403BC000      /**< \brief IMX1020 XBARA1 base address */
#define IMX1020_QTIMER1_BASE            0x401DC000      /**< \brief IMX1020 QTimer1 base address */
#define IMX1020_QTIMER2_BASE            0x401E0000      /**< \brief IMX1020 QTimer2 base address */
#define IMX1020_SEMC_BASE               0x402F0000      /**< \brief IMX1020 SEMC base address */
#define IMX1020_FLEXIO1_BASE            0x401AC000      /**< \brief IMX1020 FlexIO1 base address */
#define IMX1020_FLEXIO2_BASE            0x401B0000      /**< \brief IMX1020 FlexIO2 base address */
#define IMX1020_USB1_BASE_ADDR          0x402E0000      /**< \brief IMX1020 USB1 base address */
#define IMX1020_USBPHY1_BASE_ADDR       0x400D9000      /**< \brief IMX1020 USBPHY1 base address */
#define IMX1020_ENC1_BASE               0x403C8000      /**< \brief IMX1020 ENC1 base address */
#define IMX1020_ENC2_BASE               0x403CC000      /**< \brief IMX1020 ENC2 base address */
#define IMX1020_TEMPMON_BASE            0x400D8180      /**< \brief IMX1020 TEMPMON base address */
#define IMX1020_ACMP1_BASE              0x40094000      /**< \brief IMX1020 TEMPMON base address */
#define IMX1020_ACMP2_BASE              0x40094008      /**< \brief IMX1020 TEMPMON base address */
#define IMX1020_ACMP3_BASE              0x40094010      /**< \brief IMX1020 TEMPMON base address */
#define IMX1020_ACMP4_BASE              0x40094018      /**< \brief IMX1020 TEMPMON base address */
#define IMX1020_ENET_BASE               0x402D8000      /**< \brief IMX1020 ENET base address */
#define IMX1020_SNVS_BASE               0x400D4000      /**< \brief IMX1020 SNVS base address */

/** @} */


/** @} end of grp_imx1020_bsp_modules_base */
/** @} end of grp_imx1020_bsp */


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __IMX1020_REGBASE_H */

/* end of file */
