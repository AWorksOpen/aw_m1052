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
 * \brief imx1050 sai regs
 *
 * \internal
 * \par modification history:
 * - 1.00 2017-12-1  hsg, first implementation
 * \endinternal
 */

#ifndef __IMX1050_SAI_REGS_H__
#define __IMX1050_SAI_REGS_H__


typedef struct imx1050_sai_regs {
    volatile uint32_t verid;            /**< Version ID Register, offset: 0x0 */
    volatile uint32_t param;            /**< Parameter Register, offset: 0x4 */
    volatile uint32_t tcsr;             /**< SAI Transmit Control Register, offset: 0x8 */
    volatile uint32_t tcr1;             /**< SAI Transmit Configuration 1 Register, offset: 0xC */
    volatile uint32_t tcr2;             /**< SAI Transmit Configuration 2 Register, offset: 0x10 */
    volatile uint32_t tcr3;             /**< SAI Transmit Configuration 3 Register, offset: 0x14 */
    volatile uint32_t tcr4;             /**< SAI Transmit Configuration 4 Register, offset: 0x18 */
    volatile uint32_t tcr5;             /**< SAI Transmit Configuration 5 Register, offset: 0x1C */
    volatile uint32_t tdr[4];           /**< SAI Transmit Data Register, array offset: 0x20, array step: 0x4 */
             uint8_t  reserved_0[16];
    volatile uint32_t tfr[4];           /**< SAI Transmit FIFO Register, array offset: 0x40, array step: 0x4 */
             uint8_t  reserved_1[16];
    volatile uint32_t tmr;              /**< SAI Transmit Mask Register, offset: 0x60 */
             uint8_t  reserved_2[36];
    volatile uint32_t rcsr;             /**< SAI Receive Control Register, offset: 0x88 */
    volatile uint32_t rcr1;             /**< SAI Receive Configuration 1 Register, offset: 0x8C */
    volatile uint32_t rcr2;             /**< SAI Receive Configuration 2 Register, offset: 0x90 */
    volatile uint32_t rcr3;             /**< SAI Receive Configuration 3 Register, offset: 0x94 */
    volatile uint32_t rcr4;             /**< SAI Receive Configuration 4 Register, offset: 0x98 */
    volatile uint32_t rcr5;             /**< SAI Receive Configuration 5 Register, offset: 0x9C */
    volatile uint32_t rdr[4];           /**< SAI Receive Data Register, array offset: 0xA0, array step: 0x4 */
             uint8_t  reserved_3[16];
    volatile uint32_t rfr[4];           /**< SAI Receive FIFO Register, array offset: 0xC0, array step: 0x4 */
             uint8_t  reserved_4[16];
    volatile uint32_t rmr;              /**< SAI Receive Mask Register, offset: 0xE0 */
} imx1050_sai_regs_t;

#define __SAI_TCSR_TE                      (1u << 31)
#define __SAI_TCSR_STOPE                   (1u << 30)
#define __SAI_TCSR_DBGE                    (1u << 29)
#define __SAI_TCSR_BCE                     (1u << 28)
#define __SAI_TCSR_FR                      (1u << 25)
#define __SAI_TCSR_SR                      (1u << 24)
#define __SAI_TCSR_WSF                     (1u << 20)
#define __SAI_TCSR_SEF                     (1u << 19)
#define __SAI_TCSR_FEF                     (1u << 18)
#define __SAI_TCSR_FWF                     (1u << 17)
#define __SAI_TCSR_FRF                     (1u << 16)
#define __SAI_TCSR_WSIE                    (1u << 12)
#define __SAI_TCSR_SEIE                    (1u << 11)
#define __SAI_TCSR_FEIE                    (1u << 10)
#define __SAI_TCSR_FWIE                    (1u << 9)
#define __SAI_TCSR_FRIE                    (1u << 8)
#define __SAI_TCSR_FWDE                    (1u << 1)
#define __SAI_TCSR_FRDE                    (1u << 0)

#define __SAI_RCSR_TE                      (1u << 31)
#define __SAI_RCSR_STOPE                   (1u << 30)
#define __SAI_RCSR_DBGE                    (1u << 29)
#define __SAI_RCSR_BCE                     (1u << 28)
#define __SAI_RCSR_FR                      (1u << 25)
#define __SAI_RCSR_SR                      (1u << 24)
#define __SAI_RCSR_WSF                     (1u << 20)
#define __SAI_RCSR_SEF                     (1u << 19)
#define __SAI_RCSR_FEF                     (1u << 18)
#define __SAI_RCSR_FWF                     (1u << 17)
#define __SAI_RCSR_FRF                     (1u << 16)
#define __SAI_RCSR_WSIE                    (1u << 12)
#define __SAI_RCSR_SEIE                    (1u << 11)
#define __SAI_RCSR_FEIE                    (1u << 10)
#define __SAI_RCSR_FWIE                    (1u << 9)
#define __SAI_RCSR_FRIE                    (1u << 8)
#define __SAI_RCSR_FWDE                    (1u << 1)
#define __SAI_RCSR_FRDE                    (1u << 0)


#endif   /* __IMX1050_SAI_REGS_H__ */

/* enf of file */
