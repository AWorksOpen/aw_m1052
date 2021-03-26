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
 * \brief imx10xx lpi2c regs
 *
 * \internal
 * \par modification history:
 * - 1.00 17-10-18  like, first implementation
 * \endinternal
 */


#ifndef IMX10XX_LPI2C_REG_H
#define IMX10XX_LPI2C_REG_H

#include "stdint.h"

#define     __IO    volatile
#define     __I     volatile const
#define     __O     volatile             /*!< Defines 'write only' permissions */


typedef struct __imx10xx_lpi2c_reg {
    __I  uint32_t verid;                             /**< Version ID Register, offset: 0x0 */
    __I  uint32_t param;                             /**< Parameter Register, offset: 0x4 */
         uint8_t reserved_0[8];
    __IO uint32_t mcr;                               /**< Master Control Register, offset: 0x10 */
    __IO uint32_t msr;                               /**< Master Status Register, offset: 0x14 */
    __IO uint32_t mier;                              /**< Master Interrupt Enable Register, offset: 0x18 */
    __IO uint32_t mder;                              /**< Master DMA Enable Register, offset: 0x1C */
    __IO uint32_t mcfgr0;                            /**< Master Configuration Register 0, offset: 0x20 */
    __IO uint32_t mcfgr1;                            /**< Master Configuration Register 1, offset: 0x24 */
    __IO uint32_t mcfgr2;                            /**< Master Configuration Register 2, offset: 0x28 */
    __IO uint32_t mcfgr3;                            /**< Master Configuration Register 3, offset: 0x2C */
         uint8_t reserved_1[16];
    __IO uint32_t mdmr;                              /**< Master Data Match Register, offset: 0x40 */
         uint8_t reserved_2[4];
    __IO uint32_t mccr0;                             /**< Master Clock Configuration Register 0, offset: 0x48 */
         uint8_t reserved_3[4];
    __IO uint32_t mccr1;                             /**< Master Clock Configuration Register 1, offset: 0x50 */
         uint8_t reserved_4[4];
    __IO uint32_t mfcr;                              /**< Master FIFO Control Register, offset: 0x58 */
    __I  uint32_t mfsr;                              /**< Master FIFO Status Register, offset: 0x5C */
    __O  uint32_t mtdr;                              /**< Master Transmit Data Register, offset: 0x60 */
         uint8_t reserved_5[12];
    __I  uint32_t mrdr;                              /**< Master Receive Data Register, offset: 0x70 */
         uint8_t reserved_6[156];
    __IO uint32_t scr;                               /**< Slave Control Register, offset: 0x110 */
    __IO uint32_t ssr;                               /**< Slave Status Register, offset: 0x114 */
    __IO uint32_t sier;                              /**< Slave Interrupt Enable Register, offset: 0x118 */
    __IO uint32_t sder;                              /**< Slave DMA Enable Register, offset: 0x11C */
         uint8_t reserved_7[4];
    __IO uint32_t scfgr1;                            /**< Slave Configuration Register 1, offset: 0x124 */
    __IO uint32_t scfgr2;                            /**< Slave Configuration Register 2, offset: 0x128 */
         uint8_t reserved_8[20];
    __IO uint32_t samr;                              /**< Slave Address Match Register, offset: 0x140 */
         uint8_t reserved_9[12];
    __I  uint32_t sasr;                              /**< Slave Address Status Register, offset: 0x150 */
    __IO uint32_t star;                              /**< Slave Transmit ACK Register, offset: 0x154 */
         uint8_t reserved_10[8];
    __O  uint32_t stdr;                              /**< Slave Transmit Data Register, offset: 0x160 */
         uint8_t reserved_11[12];
    __I  uint32_t srdr;                              /**< Slave Receive Data Register, offset: 0x170 */
} __imx10xx_lpi2c_reg_t;



#endif /* IMX10XX_LPI2C_REG_H */
