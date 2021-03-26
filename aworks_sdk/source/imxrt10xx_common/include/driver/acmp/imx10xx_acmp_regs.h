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
 * \brief iMX RT10xx Quadrature Decoder regs
 *
 * \internal
 * \par modification history:
 * - 1.00 18-03-21  mex, first implementation
 * \endinternal
 */

#ifndef __IMX10xx_ACMP_REGS_H
#define __IMX10xx_ACMP_REGS_H

#include "stdint.h"

#define __IO  volatile
#define __O   volatile
#define __I   volatile const


typedef struct {
    __IO uint8_t CR0;          /**< \brief CMP Control Register 0, offset: 0x0 */
    __IO uint8_t CR1;          /**< \brief CMP Control Register 1, offset: 0x1 */
    __IO uint8_t FPR;          /**< \brief CMP Filter Period Register, offset: 0x2 */
    __IO uint8_t SCR;          /**< \brief CMP Status and Control Register, offset: 0x3 */
    __IO uint8_t DACCR;        /**< \brief DAC Control Register, offset: 0x4 */
    __IO uint8_t MUXCR;        /**< \brief MUX Control Register, offset: 0x5 */
} imx10xx_acmp_regs_t;


/*******************************************************************************
  CMP Control Register 0
*******************************************************************************/

/* Comparator Module Enable */
#define __CMP_CR0_HYSTCTR_SHIFT        0
#define __CMP_CR0_HYSTCTR(x)          (((x) & 0x3) << __CMP_CR0_HYSTCTR_SHIFT)

/* Filter Sample Count */
#define __CMP_CR0_FILTER_CNT_SHIFT     4
#define __CMP_CR0_FILTER_CNT(x)       (((x) & 0x7) << __CMP_CR0_FILTER_CNT_SHIFT)

/*******************************************************************************
  CMP Control Register 1
*******************************************************************************/

/* Comparator Module Enable */
#define __CMP_CR1_EN_SHIFT             0
#define __CMP_CR1_EN_MASK             (0x1 << __CMP_CR1_EN_SHIFT)

/* Comparator Output Pin Enable */
#define __CMP_CR1_OPE_SHIFT            1
#define __CMP_CR1_OPE_MASK            (0x1 << __CMP_CR1_OPE_SHIFT)

/* Comparator Output Select */
#define __CMP_CR1_COS_SHIFT            2
#define __CMP_CR1_COS_MASK            (0x1 << __CMP_CR1_COS_SHIFT)

/* Comparator INVERT */
#define __CMP_CR1_INV_SHIFT            3
#define __CMP_CR1_INV_MASK            (0x1 << __CMP_CR1_INV_SHIFT)

/* Power Mode Select */
#define __CMP_CR1_PMODE_SHIFT          4
#define __CMP_CR1_PMODE_MASK          (0x1 << __CMP_CR1_PMODE_SHIFT)

/* Windowing Enable */
#define __CMP_CR1_WE_SHIFT             6
#define __CMP_CR1_WE_MASK             (0x1 << __CMP_CR1_WE_SHIFT)

/* Sample Enable */
#define __CMP_CR1_SE_SHIFT             7
#define __CMP_CR1_SE_MASK             (0x1 << __CMP_CR1_SE_SHIFT)

/*******************************************************************************
  DAC Control Register
*******************************************************************************/

/* DAC Output Voltage Select */
#define __CMP_DACCR_VOSEL_SHIFT        0
#define __CMP_DACCR_VOSEL(x)          (((x) & 0x3f) << __CMP_DACCR_VOSEL_SHIFT)

/* Supply Voltage Reference Source Select */
#define __CMP_DACCR_VRSEL_SHIFT        6
#define __CMP_DACCR_VRSEL_MASK        (0x1 << __CMP_DACCR_VRSEL_SHIFT)

/* DAC Enable */
#define __CMP_DACCR_DACEN_SHIFT        7
#define __CMP_DACCR_DACEN_MASK        (0x1 << __CMP_DACCR_DACEN_SHIFT)

/*******************************************************************************
  MUX Control Register
*******************************************************************************/

/* Minus Input Mux Control */
#define __CMP_MUXCR_MSEL_SHIFT         0
#define __CMP_MUXCR_MSEL(x)           (((x) & 0xf) << __CMP_MUXCR_MSEL_SHIFT)

/* Plus Input Mux Control */
#define __CMP_MUXCR_PSEL_SHIFT         3
#define __CMP_MUXCR_PSEL(x)           (((x) & 0xf) << __CMP_MUXCR_PSEL_SHIFT)


/*******************************************************************************
  CMP Status and Control Register
*******************************************************************************/

/* Analog Comparator Output */
#define __CMP_SCR_COUT_SHIFT           0
#define __CMP_SCR_COUT_MASK           (0x1 << __CMP_SCR_COUT_SHIFT)

/* Analog Comparator Flag Falling */
#define __CMP_SCR_CFF_SHIFT            1
#define __CMP_SCR_CFF_MASK            (0x1 << __CMP_SCR_CFF_SHIFT)

/* Analog Comparator Flag Rising */
#define __CMP_SCR_CFR_SHIFT            2
#define __CMP_SCR_CFR_MASK            (0x1 << __CMP_SCR_CFR_SHIFT)

/* Comparator Interrupt Enable Falling */
#define __CMP_SCR_IEF_SHIFT            3
#define __CMP_SCR_IEF_MASK            (0x1 << __CMP_SCR_IEF_SHIFT)

/* Comparator Interrupt Enable Rising */
#define __CMP_SCR_IER_SHIFT            4
#define __CMP_SCR_IER_MASK            (0x1 << __CMP_SCR_IER_SHIFT)

/* DMA Enable Control */
#define __CMP_SCR_DMAEN_SHIFT          6
#define __CMP_SCR_DMAEN_MASK          (0x1 << __CMP_SCR_DMAEN_SHIFT)


#endif /* __IMX10xx_ACMP_REGS_H */

/* end of file */
