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
 * \brief IMX1010各模块寄存器基址
 *
 * 提供各模块的寄存器组的物理基地址
 *
 * \par 1.兼容设备
 *
 *  - IMX1010
 *
 * \internal
 * \par modification history
 * - 1.00 18-09-21  mex, first implementation
 * \endinternal
 */

#ifndef __IMX1010_REG_BASE_H
#define __IMX1010_REG_BASE_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * \addtogroup grp_imx1010_bsp IMX1010 支持
 * @{
 */

/**
 * \addtogroup grp_imx1010_bsp_modules_base 各模块基地址
 * @{
 */

/**
 * \name Memory space
 * @{
 */

#define IMX1010_CCM_BASE                0x400FC000UL    /**< \brief IMX1010 CCM base address */
#define IMX1010_CCM_ANALOG_BASE         0x400D8000UL    /**< \brief IMX1010 CCM analog base address */
#define IMX1010_XTALOSC24M_BASE         0x400D8000UL    /**< \brief XTALOSC24M base address */
#define IMX1010_PMU_BASE                0x400D8000UL    /**< \brief PMU base address */

#define IMX1010_LPUART1_BASE            0x40184000      /**< \brief IMX1010 lpuart1 base address */
#define IMX1010_LPUART2_BASE            0x40188000      /**< \brief IMX1010 lpuart2 base address */
#define IMX1010_LPUART3_BASE            0x4018C000      /**< \brief IMX1010 lpuart3 base address */
#define IMX1010_LPUART4_BASE            0x40190000      /**< \brief IMX1010 lpuart4 base address */
#define IMX1010_GPIO1_BASE              0x401B8000      /**< \brief IMX1010 GPIO1 base address */
#define IMX1010_GPIO2_BASE              0x42000000      /**< \brief IMX1010 GPIO2 base address */
#define IMX1010_GPIO5_BASE              0x400C0000      /**< \brief IMX1010 GPIO5 base address */
#define IMX1010_IOMUXC_SNVS_GPR_BASE    0x400A4000      /**< \brief IMX1010 IOMUXC SNVS GPR base address */
#define IMX1010_IOMUXC_SNVS_BASE        0x400A8000      /**< \brief IMX1010 IOMUXC SNVS base address */
#define IMX1010_IOMUXC_GPR_BASE         0x400AC000      /**< \brief IMX1010 IOMUXC GPR base address */
#define IMX1010_IOMUXC_BASE             0x401F8000      /**< \brief IMX1010 IOMUXC base address */
#define IMX1010_LPSPI1_BASE             0x40194000      /**< \brief IMX1010 lpspi1 base address */
#define IMX1010_LPSPI2_BASE             0x40198000      /**< \brief IMX1010 lpspi2 base address */
#define IMX1010_WDOG1_BASE              0x400B8000      /**< \brief IMX1010 wdog1 base address */
#define IMX1010_WDOG2_BASE              0x400D0000      /**< \brief IMX1010 wdog2 base address */
#define IMX1010_WDOG3_BASE              0x400BC000      /**< \brief IMX1010 wdog3 base address */
#define IMX1010_GPT1_BASE               0x401EC000      /**< \brief IMX1010 GPT1 base address */
#define IMX1010_GPT2_BASE               0x401F0000      /**< \brief IMX1010 GPT2 base address */
#define IMX1010_PIT_BASE                0x40084000      /**< \brief IMX1010 PIT base address */
#define IMX1010_ADC1_BASE               0X400C4000      /**< \brief IMX1010 ADC1 base address */
#define IMX1010_EDMA_BASE               0x400E8000      /**< \brief IMX1010 eDMA base address */
#define IMX1010_DMAMUX_BASE             0x400EC000      /**< \brief IMX1010 DMA MUX base address */
#define IMX1010_SAI1_BASE               0x401E0000      /**< \brief IMX1010 SAI1 base address */
#define IMX1010_SAI3_BASE               0x401E8000      /**< \brief IMX1010 SAI3 base address */
#define IMX1010_LPI2C1_BASE             0x401A4000      /**< \brief IMX1010 LPI2C1 base address */
#define IMX1010_LPI2C2_BASE             0x401A8000      /**< \brief IMX1010 LPI2C2 base address */
#define IMX1010_OCOTP_BASE              0x401F4000      /**< \brief IMX1010 OCOTP base address */
#define IMX1010_FLEXSPI_BASE            0x400A0000      /**< \brief IMX1010 Flexspi base address */
#define IMX1010_TEMPMON_BASE            0x400D8180      /**< \brief IMX1010 TEMPMON base address */
#define IMX1010_FLEXPWM1_BASE           0x401CC000      /**< \brief IMX1010 eFlexPWM1 base address */
#define IMX1010_XBARA1_BASE             0x40098000      /**< \brief IMX1010 XBARA1 base address */
#define IMX1010_FLEXIO_BASE             0x401AC000      /**< \brief IMX1010 FlexIO base address */
#define IMX1010_USB_BASE_ADDR           0x400E4000      /**< \brief IMX1010 USB base address */
#define IMX1010_USBPHY_BASE_ADDR        0x400D9000      /**< \brief IMX1010 USBPl301 base address */
#define IMX1010_SNVS_BASE               0x400D4000      /**< \brief IMX1010 SNVS base address */

/** @} */


/** @} end of grp_imx1010_bsp_modules_base */
/** @} end of grp_imx1010_bsp */


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __IMX1010_REGBASE_H */

/* end of file */
