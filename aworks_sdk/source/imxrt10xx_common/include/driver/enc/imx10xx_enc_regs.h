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

#ifndef __IMX10xx_ENC_REGS_H
#define __IMX10xx_ENC_REGS_H

#include "stdint.h"

#define __IO  volatile
#define __O   volatile
#define __I   volatile const

typedef struct {

    __IO uint16_t CTRL;         /**< \brief Control Register, offset: 0x0 */
    __IO uint16_t FILT;         /**< \brief Input Filter Register, offset: 0x2 */
    __IO uint16_t WTR;          /**< \brief Watchdog Timeout Register, offset: 0x4 */
    __IO uint16_t POSD;         /**< \brief Position Difference Counter Register, offset: 0x6 */
    __I  uint16_t POSDH;        /**< \brief Position Difference Hold Register, offset: 0x8 */
    __IO uint16_t REV;          /**< \brief Revolution Counter Register, offset: 0xA */
    __I  uint16_t REVH;         /**< \brief Revolution Hold Register, offset: 0xC */
    __IO uint16_t UPOS;         /**< \brief Upper Position Counter Register, offset: 0xE */
    __IO uint16_t LPOS;         /**< \brief Lower Position Counter Register, offset: 0x10 */
    __I  uint16_t UPOSH;        /**< \brief Upper Position Hold Register, offset: 0x12 */
    __I  uint16_t LPOSH;        /**< \brief Lower Position Hold Register, offset: 0x14 */
    __IO uint16_t UINIT;        /**< \brief Upper Initialization Register, offset: 0x16 */
    __IO uint16_t LINIT;        /**< \brief Lower Initialization Register, offset: 0x18 */
    __I  uint16_t IMR;          /**< \brief Input Monitor Register, offset: 0x1A */
    __IO uint16_t TST;          /**< \brief Test Register, offset: 0x1C */
    __IO uint16_t CTRL2;        /**< \brief Control 2 Register, offset: 0x1E */
    __IO uint16_t UMOD;         /**< \brief Upper Modulus Register, offset: 0x20 */
    __IO uint16_t LMOD;         /**< \brief Lower Modulus Register, offset: 0x22 */
    __IO uint16_t UCOMP;        /**< \brief Upper Position Compare Register, offset: 0x24 */
    __IO uint16_t LCOMP;        /**< \brief Lower Position Compare Register, offset: 0x26 */

} imx10xx_enc_regs_t;



/*******************************************************************************
  Control Register
*******************************************************************************/


/* Compare Interrupt Enable */
#define __ENC_CTRL_CMPIE_SHIFT           0
#define __ENC_CTRL_CMPIE_MASK           (0x1 << __ENC_CTRL_CMPIE_SHIFT)

/* Compare Interrupt Request */
#define __ENC_CTRL_CMPIRQ_SHIFT           1
#define __ENC_CTRL_CMPIRQ_MASK           (0x1 << __ENC_CTRL_CMPIRQ_SHIFT)

/* Watchdog Enable */
#define __ENC_CTRL_WDE_SHIFT              2
#define __ENC_CTRL_WDE_MASK              (0x1 << __ENC_CTRL_WDE_SHIFT)

/* Watchdog Timeout Interrupt Enable */
#define __ENC_CTRL_DIE_SHIFT              3
#define __ENC_CTRL_DIE_MASK              (0x1 << __ENC_CTRL_DIE_SHIFT)

/* Watchdog Timeout Interrupt Request */
#define __ENC_CTRL_DIRQ_SHIFT             4
#define __ENC_CTRL_DIRQ_MASK             (0x1 << __ENC_CTRL_DIRQ_SHIFT)

/* Use Negative Edge of INDEX Pulse */
#define __ENC_CTRL_XNE_SHIFT              5
#define __ENC_CTRL_XNE_MASK              (0x1 << __ENC_CTRL_XNE_SHIFT)

/* INDEX Triggered Initialization of Position Counters UPOS and LPOS */
#define __ENC_CTRL_XIP_SHIFT              6
#define __ENC_CTRL_XIP_MASK              (0x1 << __ENC_CTRL_XIP_SHIFT)

/* INDEX Pulse Interrupt Enable */
#define __ENC_CTRL_XIE_SHIFT              7
#define __ENC_CTRL_XIE_MASK              (0x1 << __ENC_CTRL_XIE_SHIFT)

/* INDEX Pulse Interrupt Request */
#define __ENC_CTRL_XIRQ_SHIFT             8
#define __ENC_CTRL_XIRQ_MASK             (0x1 << __ENC_CTRL_XIRQ_SHIFT)

/* Enable Signal Phase Count Mode */
#define __ENC_CTRL_PH1_SHIFT              9
#define __ENC_CTRL_PH1_MASK              (0x1 << __ENC_CTRL_PH1_SHIFT)

/* Enable Signal Phase Count Mode */
#define __ENC_CTRL_REV_SHIFT              10
#define __ENC_CTRL_REV_MASK              (0x1 << __ENC_CTRL_REV_SHIFT)

/* Software-Triggered Initialization of Position Counters UPOS and LPOS */
#define __ENC_CTRL_SWIP_SHIFT             11
#define __ENC_CTRL_SWIP_MASK             (0x1 << __ENC_CTRL_SWIP_SHIFT)

/* Use Negative Edge of HOME Input */
#define __ENC_CTRL_HNE_SHIFT              12
#define __ENC_CTRL_HNE_MASK              (0x1 << __ENC_CTRL_HNE_SHIFT)

/* Enable HOME to Initialize Position Counters UPOS and LPOS */
#define __ENC_CTRL_HIP_SHIFT              13
#define __ENC_CTRL_HIP_MASK              (0x1 << __ENC_CTRL_HIP_SHIFT)

/* HOME Interrupt Enable */
#define __ENC_CTRL_HIE_SHIFT              14
#define __ENC_CTRL_HIE_MASK              (0x1 << __ENC_CTRL_HIE_SHIFT)

/* HOME Interrupt Enable */
#define __ENC_CTRL_HIRQ_SHIFT             15
#define __ENC_CTRL_HIRQ_MASK             (0x1 << __ENC_CTRL_HIRQ_SHIFT)


/*******************************************************************************
  Control 2 Register
*******************************************************************************/

/* Update Hold Registers */
#define __ENC_CTRL2_UPDHLD_SHIFT             0
#define __ENC_CTRL2_UPDHLD_MASK             (0x1 << __ENC_CTRL2_UPDHLD_SHIFT)

/* Update Position Registers */
#define __ENC_CTRL2_UPDPOS_SHIFT             1
#define __ENC_CTRL2_UPDPOS_MASK             (0x1 << __ENC_CTRL2_UPDPOS_SHIFT)

/* Enable Modulo Counting */
#define __ENC_CTRL2_MOD_SHIFT                2
#define __ENC_CTRL2_MOD_MASK                (0x1 << __ENC_CTRL2_MOD_SHIFT)

/* Count Direction Flag */
#define __ENC_CTRL2_DIR_SHIFT                3
#define __ENC_CTRL2_DIR_MASK                (0x1 << __ENC_CTRL2_DIR_SHIFT)

/* Roll-under Interrupt Enable */
#define __ENC_CTRL2_RUIE_SHIFT               4
#define __ENC_CTRL2_RUIE_MASK               (0x1 << __ENC_CTRL2_RUIE_SHIFT)

/* Roll-under Interrupt Request */
#define __ENC_CTRL2_RUIRQ_SHIFT              5
#define __ENC_CTRL2_RUIRQ_MASK              (0x1 << __ENC_CTRL2_RUIRQ_SHIFT)

/* Roll-over Interrupt Enable */
#define __ENC_CTRL2_ROIE_SHIFT               6
#define __ENC_CTRL2_ROIE_MASK               (0x1 << __ENC_CTRL2_ROIE_SHIFT)

/* Roll-over Interrupt Request */
#define __ENC_CTRL2_ROIRQ_SHIFT              7
#define __ENC_CTRL2_ROIRQ_MASK              (0x1 << __ENC_CTRL2_ROIRQ_SHIFT)

/* Revolution Counter Modulus Enable */
#define __ENC_CTRL2_REVMOD_SHIFT             8
#define __ENC_CTRL2_REVMOD_MASK             (0x1 << __ENC_CTRL2_REVMOD_SHIFT)

/* Output Control */
#define __ENC_CTRL2_OUTCTL_SHIFT             9
#define __ENC_CTRL2_OUTCTL_MASK             (0x1 << __ENC_CTRL2_OUTCTL_SHIFT)


/*******************************************************************************
  Input Filter Register
*******************************************************************************/

/* input Filter Sample Count */
#define __ENC_FILT_CNT_SHIFT             8
#define __ENC_FILT_CNT_MASK             (0x7 << __ENC_FILT_CNT_SHIFT)
#define __ENC_FILT_CNT(x)               (((x) & 0x7) << __ENC_FILT_CNT_SHIFT)

/* Input Filter Sample Period */
#define __ENC_FILT_PER_SHIFT             0
#define __ENC_FILT_PER_MASK             (0xFF << __ENC_FILT_PER_SHIFT)
#define __ENC_FILT_PER(x)               (((x) & 0xFF) << __ENC_FILT_PER_SHIFT)

/*******************************************************************************
  Upper Initialization Register
*******************************************************************************/

#define __ENC_UINIT_INIT(x)             ((x) & 0xFFFF)

/*******************************************************************************
  Lower Initialization Register
*******************************************************************************/

#define __ENC_LINIT_INIT(x)             ((x) & 0xFFFF)

/*******************************************************************************
  Upper Position Compare Register
*******************************************************************************/

#define __ENC_UCOMP_COMP(x)             ((x) & 0xFFFF)

/*******************************************************************************
  Lower Position Compare Register
*******************************************************************************/

#define __ENC_LCOMP_COMP(x)             ((x) & 0xFFFF)

#endif /* __IMX10xx_ENC_REGS_H */

/* end of file */
