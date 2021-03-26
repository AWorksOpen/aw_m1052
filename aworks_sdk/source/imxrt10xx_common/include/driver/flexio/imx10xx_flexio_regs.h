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
 * \brief iMX RT10xx FlexIO Regs
 *
 * \internal
 * \par modification history:
 * - 1.00 17-11-01  pea, first implementation
 * \endinternal
 */

#ifndef __IMX10xx_FLEXIO_REGS_H
#define __IMX10xx_FLEXIO_REGS_H

#define __IO  volatile
#define __O   volatile
#define __I   volatile const

/**
 * \brief iMX RT10xx FlexIO ¼Ä´æÆ÷¿é¶¨Òå
 */
typedef struct {

    __I  uint32_t VERID;              /**< Version ID Register, offset: 0x0 */
    __I  uint32_t PARAM;              /**< Parameter Register, offset: 0x4 */
    __IO uint32_t CTRL;               /**< FlexIO Control Register, offset: 0x8 */
    __I  uint32_t PIN;                /**< Pin State Register, offset: 0xC */
    __IO uint32_t SHIFTSTAT;          /**< Shifter Status Register, offset: 0x10 */
    __IO uint32_t SHIFTERR;           /**< Shifter Error Register, offset: 0x14 */
    __IO uint32_t TIMSTAT;            /**< Timer Status Register, offset: 0x18 */
         uint8_t  RESERVED_0[4];
    __IO uint32_t SHIFTSIEN;          /**< Shifter Status Interrupt Enable, offset: 0x20 */
    __IO uint32_t SHIFTEIEN;          /**< Shifter Error Interrupt Enable, offset: 0x24 */
    __IO uint32_t TIMIEN;             /**< Timer Interrupt Enable Register, offset: 0x28 */
         uint8_t  RESERVED_1[4];
    __IO uint32_t SHIFTSDEN;          /**< Shifter Status DMA Enable, offset: 0x30 */
         uint8_t  RESERVED_2[12];
    __IO uint32_t SHIFTSTATE;         /**< Shifter State Register, offset: 0x40 */
         uint8_t  RESERVED_3[60];
    __IO uint32_t SHIFTCTL[4];        /**< Shifter Control N Register, array offset: 0x80, array step: 0x4 */
         uint8_t  RESERVED_4[112];
    __IO uint32_t SHIFTCFG[4];        /**< Shifter Configuration N Register, array offset: 0x100, array step: 0x4 */
         uint8_t  RESERVED_5[240];
    __IO uint32_t SHIFTBUF[4];        /**< Shifter Buffer N Register, array offset: 0x200, array step: 0x4 */
         uint8_t  RESERVED_6[112];
    __IO uint32_t SHIFTBUFBIS[4];     /**< Shifter Buffer N Bit Swapped Register, array offset: 0x280, array step: 0x4 */
         uint8_t  RESERVED_7[112];
    __IO uint32_t SHIFTBUFBYS[4];     /**< Shifter Buffer N Byte Swapped Register, array offset: 0x300, array step: 0x4 */
         uint8_t  RESERVED_8[112];
    __IO uint32_t SHIFTBUFBBS[4];     /**< Shifter Buffer N Bit Byte Swapped Register, array offset: 0x380, array step: 0x4 */
         uint8_t  RESERVED_9[112];
    __IO uint32_t TIMCTL[4];          /**< Timer Control N Register, array offset: 0x400, array step: 0x4 */
         uint8_t  RESERVED_10[112];
    __IO uint32_t TIMCFG[4];          /**< Timer Configuration N Register, array offset: 0x480, array step: 0x4 */
         uint8_t  RESERVED_11[112];
    __IO uint32_t TIMCMP[4];          /**< Timer Compare N Register, array offset: 0x500, array step: 0x4 */
         uint8_t  RESERVED_12[368];
    __IO uint32_t SHIFTBUFNBS[4];     /**< Shifter Buffer N Nibble Byte Swapped Register, array offset: 0x680, array step: 0x4 */
         uint8_t  RESERVED_13[112];
    __IO uint32_t SHIFTBUFHWS[4];     /**< Shifter Buffer N Half Word Swapped Register, array offset: 0x700, array step: 0x4 */
         uint8_t  RESERVED_14[112];
    __IO uint32_t SHIFTBUFNIS[4];     /**< Shifter Buffer N Nibble Swapped Register, array offset: 0x780, array step: 0x4 */

} imx10xx_flexio_regs_t;

#endif /* __IMX10xx_FLEXIO_REGS_H */

/* end of file */
