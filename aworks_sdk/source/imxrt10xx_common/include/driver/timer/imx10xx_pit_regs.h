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
 * \brief iMX RT10xx PIT Regs
 *
 * \internal
 * \par modification history:
 * - 1.00 17-11-13  pea, first implementation
 * \endinternal
 */

#ifndef __IMX10xx_PIT_REGS_H
#define __IMX10xx_PIT_REGS_H

#define __IO  volatile
#define __O   volatile
#define __I   volatile const

/**
 * \brief iMX RT10xx PIT ¼Ä´æÆ÷¿é¶¨Òå
 */
typedef struct {
  __IO uint32_t mcr;            /**< \brief PIT Module Control Register, offset: 0x0 */
       uint8_t  reserved_0[220];
  __I  uint32_t ltmr64h;        /**< \brief PIT Upper Lifetime Timer Register, offset: 0xE0 */
  __I  uint32_t ltmr64l;        /**< \brief PIT Lower Lifetime Timer Register, offset: 0xE4 */
       uint8_t  reserved_1[24];
  struct {                      /**< \brief offset: 0x100, array step: 0x10 */
    __IO uint32_t ldval;        /**< \brief Timer Load Value Register, array offset: 0x100, array step: 0x10 */
    __I  uint32_t cval;         /**< \brief Current Timer Value Register, array offset: 0x104, array step: 0x10 */
    __IO uint32_t tctrl;        /**< \brief Timer Control Register, array offset: 0x108, array step: 0x10 */
    __IO uint32_t tflg;         /**< \brief Timer Flag Register, array offset: 0x10C, array step: 0x10 */
  } channel[4];
} imx10xx_pit_regs_t;

/** \brief PIT Module Control Register */
#define __PIT_MCR_FRZ_MASK             AW_BIT(0)
#define __PIT_MCR_MDIS_MASK            AW_BIT(1)

/** \brief The count of PIT Channel */
#define __PIT_CHAN_COUNT               (0x4u)

/** \brief Timer Control Register */
#define __PIT_TCTRL_TEN_MASK           AW_BIT(0)
#define __PIT_TCTRL_TIE_MASK           AW_BIT(1)
#define __PIT_TCTRL_CHN_MASK           AW_BIT(2)

/** \brief Timer Flag Register */
#define __PIT_TFLG_TIF_MASK            AW_BIT(0)

#endif /* __IMX10xx_PIT_REGS_H */

/* end of file */
