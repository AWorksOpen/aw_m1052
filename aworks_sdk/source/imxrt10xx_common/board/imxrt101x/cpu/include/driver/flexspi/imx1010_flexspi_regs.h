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
 * \brief iMX RT1010 FlexSPI regs
 *
 * \internal
 * \par modification history:
 * - 1.01 19-5-24   ral, fix bug
 * - 1.00 17-10-27  mex, first implementation
 * \endinternal
 */

#ifndef __IMX1010_FLEXSPI_REGS_H
#define __IMX1010_FLEXSPI_REGS_H

#include "stdint.h"

#define __IO  volatile
#define __O   volatile
#define __I   volatile const

typedef struct {

  __IO uint32_t MCR0;                  /**< \brief Module Control Register 0, offset: 0x0 */
  __IO uint32_t MCR1;                  /**< \brief Module Control Register 1, offset: 0x4 */
  __IO uint32_t MCR2;                  /**< \brief Module Control Register 2, offset: 0x8 */
  __IO uint32_t AHBCR;                 /**< \brief AHB Bus Control Register, offset: 0xC */
  __IO uint32_t INTEN;                 /**< \brief Interrupt Enable Register, offset: 0x10 */
  __IO uint32_t INTR;                  /**< \brief Interrupt Register, offset: 0x14 */
  __IO uint32_t LUTKEY;                /**< \brief LUT Key Register, offset: 0x18 */
  __IO uint32_t LUTCR;                 /**< \brief LUT Control Register, offset: 0x1C */
  __IO uint32_t AHBRXBUFCR0[4];        /**< \brief AHB RX Buffer 0 Control Register 0..AHB RX Buffer 3 Control Register 0, array offset: 0x20, array step: 0x4 */
       uint8_t RESERVED_0[48];
  __IO uint32_t FLSHCR0[4];            /**< \brief Flash A1 Control Register 0..Flash B2 Control Register 0, array offset: 0x60, array step: 0x4 */
  __IO uint32_t FLSHCR1[4];            /**< \brief Flash A1 Control Register 1..Flash B2 Control Register 1, array offset: 0x70, array step: 0x4 */
  __IO uint32_t FLSHCR2[4];            /**< \brief Flash A1 Control Register 2..Flash B2 Control Register 2, array offset: 0x80, array step: 0x4 */
       uint8_t RESERVED_1[16];
  __IO uint32_t IPCR0;                 /**< \brief IP Control Register 0, offset: 0xA0 */
  __IO uint32_t IPCR1;                 /**< \brief IP Control Register 1, offset: 0xA4 */
       uint8_t RESERVED_2[8];
  __IO uint32_t IPCMD;                 /**< \brief IP Command Register, offset: 0xB0 */
  __IO uint32_t DLPR;                  /**< \brief Data Learn Pattern Register, offset: 0xB4 */
  __IO uint32_t IPRXFCR;               /**< \brief IP RX FIFO Control Register, offset: 0xB8 */
  __IO uint32_t IPTXFCR;               /**< \brief IP TX FIFO Control Register, offset: 0xBC */
  __IO uint32_t DLLCR[2];              /**< \brief DLL Control Register 0, array offset: 0xC0, array step: 0x4 */
       uint8_t RESERVED_3[24];
  __I  uint32_t STS0;                  /**< \brief Status Register 0, offset: 0xE0 */
  __I  uint32_t STS1;                  /**< \brief Status Register 1, offset: 0xE4 */
  __I  uint32_t STS2;                  /**< \brief Status Register 2, offset: 0xE8 */
  __I  uint32_t AHBSPNDSTS;            /**< \brief AHB Suspend Status Register, offset: 0xEC */
  __I  uint32_t IPRXFSTS;              /**< \brief IP RX FIFO Status Register, offset: 0xF0 */
  __I  uint32_t IPTXFSTS;              /**< \brief IP TX FIFO Status Register, offset: 0xF4 */
       uint8_t RESERVED_4[8];
  __I  uint32_t RFDR[32];              /**< \brief IP RX FIFO Data Register 0..IP RX FIFO Data Register 31, array offset: 0x100, array step: 0x4 */
  __O  uint32_t TFDR[32];              /**< \brief IP TX FIFO Data Register 0..IP TX FIFO Data Register 31, array offset: 0x180, array step: 0x4 */
  __IO uint32_t LUT[64];               /**< \brief LUT 0..LUT 63, array offset: 0x200, array step: 0x4 */

} imx1010_flexspi_regs_t;


/**
 * \brief 命令类型
 */
typedef enum __flexspi_command_type
{
    flexspi_read,       /** \brief 读操作 */
    flexspi_write,      /** \brief 写操作 */
} flexspi_command_type_t;


/**
 * \brief FLEXSPI AHB写等待间隔时间单元
 */
typedef enum __flexspi_ahb_write_wait_unit
{
    AhbWriteWaitUnit2AhbCycle = 0x0UL,      /**< \brief AWRWAIT unit is 2 ahb clock cycle. */
    AhbWriteWaitUnit8AhbCycle = 0x1UL,      /**< \brief AWRWAIT unit is 8 ahb clock cycle. */
    AhbWriteWaitUnit32AhbCycle = 0x2UL,     /**< \brief AWRWAIT unit is 32 ahb clock cycle. */
    AhbWriteWaitUnit128AhbCycle = 0x3UL,    /**< \brief AWRWAIT unit is 128 ahb clock cycle. */
    AhbWriteWaitUnit512AhbCycle = 0x4UL,    /**< \brief AWRWAIT unit is 512 ahb clock cycle. */
    AhbWriteWaitUnit2048AhbCycle = 0x5UL,   /**< \brief AWRWAIT unit is 2048 ahb clock cycle. */
    AhbWriteWaitUnit8192AhbCycle = 0x6UL,   /**< \brief AWRWAIT unit is 8192 ahb clock cycle. */
    AhbWriteWaitUnit32768AhbCycle = 0x7UL,  /**< \brief AWRWAIT unit is 32768 ahb clock cycle. */
} flexspi_ahb_write_wait_unit_t;


/*******************************************************************************
  Module Control Register 0
*******************************************************************************/

/* Software Reset bit */
#define __FLEXSPI_MCR0_SWRESET_SHIFT                0
#define __FLEXSPI_MCR0_SWRESET_MASK                (0x1 << __FLEXSPI_MCR0_SWRESET_SHIFT)

/* Module Disable bit */
#define __FLEXSPI_MCR0_MDIS_SHIFT                   1
#define __FLEXSPI_MCR0_MDIS_MASK                   (0x1 << __FLEXSPI_MCR0_MDIS_SHIFT)

/* Sample Clock source selection for Flash Reading */
#define __FLEXSPI_MCR0_RXCLKSRC_SHIFT               4
#define __FLEXSPI_MCR0_RXCLKSRC(x)                 (((x) & 0x03) << __FLEXSPI_MCR0_RXCLKSRC_SHIFT)

/* Enable AHB bus Read Access to IP RX FIFO */
#define __FLEXSPI_MCR0_ARDFEN_SHIFT                 6
#define __FLEXSPI_MCR0_ARDFEN(x)                   (((x) & 0x01) << __FLEXSPI_MCR0_ARDFEN_SHIFT)

/* Enable AHB bus Write Access to IP TX FIFO */
#define __FLEXSPI_MCR0_ATDFEN_SHIFT                 7
#define __FLEXSPI_MCR0_ATDFEN(x)                   (((x) & 0x01) << __FLEXSPI_MCR0_ATDFEN_SHIFT)

/* Half Speed Serial Flash access Enable */
#define __FLEXSPI_MCR0_HSEN_SHIFT                   11
#define __FLEXSPI_MCR0_HSEN(x)                     (((x) & 0x01) << __FLEXSPI_MCR0_HSEN_SHIFT)

/* Doze mode enable bit */
#define __FLEXSPI_MCR0_DOZEEN_SHIFT                 12
#define __FLEXSPI_MCR0_DOZEEN(x)                   (((x) & 0x01) << __FLEXSPI_MCR0_DOZEEN_SHIFT)

/* This bit is to support Flash Octal mode access by combining Port A and B Data pins */
#define __FLEXSPI_MCR0_COMBINATIONEN_SHIFT          13
#define __FLEXSPI_MCR0_COMBINATIONEN(x)            (((x) & 0x01) << __FLEXSPI_MCR0_COMBINATIONEN_SHIFT)

/* This bit is used to force SCK output free-running */
#define __FLEXSPI_MCR0_SCKFREERUNEN_SHIFT           14
#define __FLEXSPI_MCR0_SCKFREERUNEN(x)             (((x) & 0x01) << __FLEXSPI_MCR0_SCKFREERUNEN_SHIFT)

/* Time out wait cycle for IP command grant */
#define __FLEXSPI_MCR0_IPGRANTWAIT_SHIFT            16
#define __FLEXSPI_MCR0_IPGRANTWAIT(x)              (((x) & 0xFF) << __FLEXSPI_MCR0_IPGRANTWAIT_SHIFT)

/* Timeout wait cycle for AHB command grant */
#define __FLEXSPI_MCR0_AHBGRANTWAIT_SHIFT           24
#define __FLEXSPI_MCR0_AHBGRANTWAIT(x)             (((x) & 0xFF) << __FLEXSPI_MCR0_AHBGRANTWAIT_SHIFT)


/*******************************************************************************
  Module Control Register 1
*******************************************************************************/

/* AHB Read/Write access to Serial Flash Memory space timeout set */
#define __FLEXSPI_MCR1_AHBBUSWAIT_SHIFT             0
#define __FLEXSPI_MCR1_AHBBUSWAIT(x)               (((x) & 0xFFFF) << __FLEXSPI_MCR1_AHBBUSWAIT_SHIFT)

/* Command Sequence Execution timeout set */
#define __FLEXSPI_MCR1_SEQWAIT_SHIFT                16
#define __FLEXSPI_MCR1_SEQWAIT(x)                  (((x) & 0xFFFF) << __FLEXSPI_MCR1_SEQWAIT_SHIFT)

/*******************************************************************************
  Module Control Register 2
*******************************************************************************/


/* This bit determines whether AHB RX Buffer and AHB TX Buffer will be cleaned automaticaly when
 *  FlexSPI returns STOP mode ACK
 */
#define __FLEXSPI_MCR2_CLRAHBBUFOPT_SHIFT           11
#define __FLEXSPI_MCR2_CLRAHBBUFOPT(x)             (((x) & 0x1) << __FLEXSPI_MCR2_CLRAHBBUFOPT_SHIFT)

/* The sampling clock phase selection will be reset to phase 0 when this bit is written with 0x1 */
#define __FLEXSPI_MCR2_CLRLEARNPHASE_SHIFT          14
#define __FLEXSPI_MCR2_CLRLEARNPHASE(x)            (((x) & 0x1) << __FLEXSPI_MCR2_CLRLEARNPHASE_SHIFT)

/* All external devices are same devices (both in types and size) for A1/A2/B1/B2 */
#define __FLEXSPI_MCR2_SAMEDEVICEEN_SHIFT           15
#define __FLEXSPI_MCR2_SAMEDEVICEEN(x)             (((x) & 0x1) << __FLEXSPI_MCR2_SAMEDEVICEEN_SHIFT)

/* SCKB pad can be used as SCKA differential clock output (inverted clock to SCKA) */
#define __FLEXSPI_MCR2_SCKBDIFFOPT_SHIFT            19
#define __FLEXSPI_MCR2_SCKBDIFFOPT(x)              (((x) & 0x1) << __FLEXSPI_MCR2_SCKBDIFFOPT_SHIFT)

/* Wait cycle (in AHB clock cycle) for idle state before suspended command sequence resumed */
#define __FLEXSPI_MCR2_RESUMEWAIT_SHIFT             24
#define __FLEXSPI_MCR2_RESUMEWAIT(x)               (((x) & 0xff) << __FLEXSPI_MCR2_RESUMEWAIT_SHIFT)

#define __SCKB_USED_AS_B_SCK                        0
#define __SCKB_USED_AS_A_SCK                        1


/*******************************************************************************
  AHB Bus Control Register
*******************************************************************************/

/* Flash will be accessed in Individual mode */
#define __FLASH_INDIVIDUAL_MODE                     0

/* Flash will be accessed in Parallel mode */
#define __FLASH_PARALLEL_MODE                       1

/* Parallel mode enabled for AHB triggered Command (both read and write) */
#define __FLEXSPI_AHBCR_APAREN_SHIFT                0
#define __FLEXSPI_AHBCR_APAREN(x)                  (((x) & 0x1) << __FLEXSPI_AHBCR_APAREN_SHIFT)

/* Enable AHB bus cachable read access support */
#define __FLEXSPI_AHBCR_CACHABLEEN_SHIFT            3
#define __FLEXSPI_AHBCR_CACHABLEEN(x)              (((x) & 0x1) << __FLEXSPI_AHBCR_CACHABLEEN_SHIFT)

/* Enable AHB bus bufferable write access support */
#define __FLEXSPI_AHBCR_BUFFERABLEEN_SHIFT          4
#define __FLEXSPI_AHBCR_BUFFERABLEEN(x)            (((x) & 0x1) << __FLEXSPI_AHBCR_BUFFERABLEEN_SHIFT)

/* AHB Read Prefetch Enable */
#define __FLEXSPI_AHBCR_PREFETCHEN_SHIFT            5
#define __FLEXSPI_AHBCR_PREFETCHEN(x)              (((x) & 0x1) << __FLEXSPI_AHBCR_PREFETCHEN_SHIFT)

/*
 * AHB Read Address option bit. This option bit is intend to remove AHB burst
 * start address alignment limitation
 */
#define __FLEXSPI_AHBCR_READADDROPT_SHIFT           6
#define __FLEXSPI_AHBCR_READADDROPT(x)             (((x) & 0x1) << __FLEXSPI_AHBCR_READADDROPT_SHIFT)


/*******************************************************************************
  AHB RX Buffer 0 Control Register 0
*******************************************************************************/

/* AHB RX Buffer Size in 64 bits.Refer AHB RX Buffer Management for more details */
#define __FLEXSPI_AHBRXBUFCR0_BUFSZ_SHIFT           0
#define __FLEXSPI_AHBRXBUFCR0_BUFSZ(x)             (((x) & 0xFF) << __FLEXSPI_AHBRXBUFCR0_BUFSZ_SHIFT)

/* This AHB RX Buffer is assigned according to AHB Master with ID (MSTR_ID) */
#define __FLEXSPI_AHBRXBUFCR0_MSTRID_SHIFT          16
#define __FLEXSPI_AHBRXBUFCR0_MSTRID(x)            (((x) & 0xF) << __FLEXSPI_AHBRXBUFCR0_MSTRID_SHIFT)

/* This priority for AHB Master Read which this AHB RX Buffer is assigned */
#define __FLEXSPI_AHBRXBUFCR0_PRIORITY_SHIFT        24
#define __FLEXSPI_AHBRXBUFCR0_PRIORITY(x)          (((x) & 0x3) << __FLEXSPI_AHBRXBUFCR0_PRIORITY_SHIFT)

/*******************************************************************************
  Flash a Control Register 1
*******************************************************************************/

/* Serial Flash CS setup time */
#define __FLEXSPI_FLSHCR1_TCSS_SHIFT                0
#define __FLEXSPI_FLSHCR1_TCSS(x)                  (((x) & 0x1F) << __FLEXSPI_FLSHCR1_TCSS_SHIFT)

/* Serial Flash CS Hold time */
#define __FLEXSPI_FLSHCR1_TCSH_SHIFT                5
#define __FLEXSPI_FLSHCR1_TCSH(x)                  (((x) & 0x1F) << __FLEXSPI_FLSHCR1_TCSH_SHIFT)

/* Word Addressable */
#define __FLEXSPI_FLSHCR1_WA_SHIFT                  10
#define __FLEXSPI_FLSHCR1_WA(x)                    (((x) & 0x1) << __FLEXSPI_FLSHCR1_WA_SHIFT)

/* Column Address Size */
#define __FLEXSPI_FLSHCR1_CAS_SHIFT                 11
#define __FLEXSPI_FLSHCR1_CAS(x)                   (((x) & 0xF) << __FLEXSPI_FLSHCR1_CAS_SHIFT)

/* CS interval unit */
#define __FLEXSPI_FLSHCR1_CSINTERVALUNIT_SHIFT      15
#define __FLEXSPI_FLSHCR1_CSINTERVALUNIT(x)        (((x) & 0x1) << __FLEXSPI_FLSHCR1_CSINTERVALUNIT_SHIFT)

/*
 * set the minimum interval between flash device Chip selection deassertion and
 * flash device Chip selection assertion
 */
#define __FLEXSPI_FLSHCR1_CSINTERVAL_SHIFT          16
#define __FLEXSPI_FLSHCR1_CSINTERVAL(x)            (((x) & 0xFFFF) << __FLEXSPI_FLSHCR1_CSINTERVAL_SHIFT)


/*******************************************************************************
  Flash a Control Register 2
*******************************************************************************/

/* Sequence Index for AHB Read triggered Command in LUT */
#define __FLEXSPI_FLSHCR2_ARDSEQID_SHIFT            0
#define __FLEXSPI_FLSHCR2_ARDSEQID(x)              (((x) & 0xF) << __FLEXSPI_FLSHCR2_ARDSEQID_SHIFT)

/* Sequence Number for AHB Read triggered Command in LUT */
#define __FLEXSPI_FLSHCR2_ARDSEQNUM_SHIFT           5
#define __FLEXSPI_FLSHCR2_ARDSEQNUM(x)             (((x) & 0x7) << __FLEXSPI_FLSHCR2_ARDSEQNUM_SHIFT)

/* Sequence Index for AHB Write triggered Command */
#define __FLEXSPI_FLSHCR2_AWRSEQID_SHIFT            8
#define __FLEXSPI_FLSHCR2_AWRSEQID(x)              (((x) & 0xF) << __FLEXSPI_FLSHCR2_AWRSEQID_SHIFT)

/* Sequence Number for AHB Write triggered Command */
#define __FLEXSPI_FLSHCR2_AWRSEQNUM_SHIFT           13
#define __FLEXSPI_FLSHCR2_AWRSEQNUM(x)             (((x) & 0x7) << __FLEXSPI_FLSHCR2_AWRSEQNUM_SHIFT)

/* AHB write wait interval */
#define __FLEXSPI_FLSHCR2_AWRWAIT_SHIFT             16
#define __FLEXSPI_FLSHCR2_AWRWAIT(x)               (((x) & 0xFFF) << __FLEXSPI_FLSHCR2_AWRWAIT_SHIFT)

/* AHB write wait unit */
#define __FLEXSPI_FLSHCR2_AWRWAITUNIT_SHIFT         28
#define __FLEXSPI_FLSHCR2_AWRWAITUNIT(x)           (((x) & 0x7) << __FLEXSPI_FLSHCR2_AWRWAITUNIT_SHIFT)

/* Clear the instruction pointer which is internally saved pointer by JMP_ON_CS */
#define __FLEXSPI_FLSHCR2_CLRINSTRPTR_SHIFT         31
#define __FLEXSPI_FLSHCR2_CLRINSTRPTR(x)           (((x) & 0x1) << __FLEXSPI_FLSHCR2_CLRINSTRPTR_SHIFT)

/*******************************************************************************
  LUT Register
*******************************************************************************/

/* OPERAND0 */
#define __FLEXSPI_LUT_OPERAND0_SHIFT                0
#define __FLEXSPI_LUT_OPERAND0(x)                  (((x) & 0xFF) << __FLEXSPI_LUT_OPERAND0_SHIFT)

/* NUM_PADS0 */
#define __FLEXSPI_LUT_NUM_PADS0_SHIFT               8
#define __FLEXSPI_LUT_NUM_PADS0(x)                 (((x) & 0x3) << __FLEXSPI_LUT_NUM_PADS0_SHIFT)

/* OPCODE0 */
#define __FLEXSPI_LUT_OPCODE0_SHIFT                 10
#define __FLEXSPI_LUT_OPCODE0(x)                   (((x) & 0x3F) << __FLEXSPI_LUT_OPCODE0_SHIFT)

/* OPERAND1 */
#define __FLEXSPI_LUT_OPERAND1_SHIFT                16
#define __FLEXSPI_LUT_OPERAND1(x)                  (((x) & 0xFF) << __FLEXSPI_LUT_OPERAND1_SHIFT)

/* NUM_PADS1 */
#define __FLEXSPI_LUT_NUM_PADS1_SHIFT               24
#define __FLEXSPI_LUT_NUM_PADS1(x)                 (((x) & 0x3) << __FLEXSPI_LUT_NUM_PADS1_SHIFT)

/* OPCODE1 */
#define __FLEXSPI_LUT_OPCODE1_SHIFT                 26
#define __FLEXSPI_LUT_OPCODE1(x)                   (((x) & 0x3F) << __FLEXSPI_LUT_OPCODE1_SHIFT)


/*******************************************************************************
  Interrupt Enable Register
*******************************************************************************/

/* IP triggered Command Sequences Execution finished interrupt enable. */
#define __FLEXSPI_INTEN_IPCMDDONEEN_SHIFT           0
#define __FLEXSPI_INTEN_IPCMDDONEEN_MASK           (0x1 << __FLEXSPI_INTEN_IPCMDDONEEN_SHIFT)

/* IP triggered Command Sequences Grant Timeout interrupt enable. */
#define __FLEXSPI_INTEN_IPCMDGEEN_SHIFT             1
#define __FLEXSPI_INTEN_IPCMDGEEN_MASK             (0x1 << __FLEXSPI_INTEN_IPCMDGEEN_SHIFT)

/* AHB triggered Command Sequences Grant Timeout interrupt enable. */
#define __FLEXSPI_INTEN_AHBCMDGEEN_SHIFT            2
#define __FLEXSPI_INTEN_AHBCMDGEEN_MASK            (0x1 << __FLEXSPI_INTEN_AHBCMDGEEN_SHIFT)

/* IP triggered Command Sequences Error Detected interrupt enable. */
#define __FLEXSPI_INTEN_IPCMDERREN_SHIFT            3
#define __FLEXSPI_INTEN_IPCMDERREN_MASK            (0x1 << __FLEXSPI_INTEN_IPCMDERREN_SHIFT)

/* AHB triggered Command Sequences Error Detected interrupt enable. */
#define __FLEXSPI_INTEN_AHBCMDERREN_SHIFT           4
#define __FLEXSPI_INTEN_AHBCMDERREN_MASK           (0x1 << __FLEXSPI_INTEN_AHBCMDERREN_SHIFT)

/* IP RX FIFO WaterMark available interrupt enable. */
#define __FLEXSPI_INTEN_IPRXWAEN_SHIFT              5
#define __FLEXSPI_INTEN_IPRXWAEN_MASK              (0x1 << __FLEXSPI_INTEN_IPRXWAEN_SHIFT)

/* IP TX FIFO WaterMark empty interrupt enable. */
#define __FLEXSPI_INTEN_IPTXWEEN_SHIFT              6
#define __FLEXSPI_INTEN_IPTXWEEN_MASK              (0x1 << __FLEXSPI_INTEN_IPTXWEEN_SHIFT)

/* AHB Bus timeout interrupt.Refer Interrupts chapter for more details. */
#define __FLEXSPI_INTEN_AHBBUSTIMEOUTEN_SHIFT       10
#define __FLEXSPI_INTEN_AHBBUSTIMEOUTEN_MASK       (0x1 << __FLEXSPI_INTEN_AHBBUSTIMEOUTEN_SHIFT)

/* Sequence execution timeout interrupt enable.Refer Interrupts chapter for more details. */
#define __FLEXSPI_INTEN_SEQTIMEOUTEN_SHIFT          11
#define __FLEXSPI_INTEN_SEQTIMEOUTEN_MASK          (0x1 << __FLEXSPI_INTEN_SEQTIMEOUTEN_SHIFT)


/*******************************************************************************
  Interrupt Register
*******************************************************************************/

/* IP triggered Command Sequences Execution finished interrupt. */
#define __FLEXSPI_INTR_IPCMDDONE_MASK               0x1u
#define __FLEXSPI_INTR_IPCMDDONE_SHIFT              0

/* IP triggered Command Sequences Grant Timeout interrupt */
#define __FLEXSPI_INTR_IPCMDGE_MASK                 0x2u
#define __FLEXSPI_INTR_IPCMDGE_SHIFT                1

/* AHB triggered Command Sequences Grant Timeout interrupt */
#define __FLEXSPI_INTR_AHBCMDGE_MASK                0x4u
#define __FLEXSPI_INTR_AHBCMDGE_SHIFT               2

/* IP triggered Command Sequences Error Detected interrupt */
#define __FLEXSPI_INTR_IPCMDERR_MASK                0x8u
#define __FLEXSPI_INTR_IPCMDERR_SHIFT               3

/* AHB triggered Command Sequences Error Detected interrupt */
#define __FLEXSPI_INTR_AHBCMDERR_MASK               0x10u
#define __FLEXSPI_INTR_AHBCMDERR_SHIFT              4

/* IP RX FIFO watermark available interrupt */
#define __FLEXSPI_INTR_IPRXWA_MASK                  0x20u
#define __FLEXSPI_INTR_IPRXWA_SHIFT                 5

/* IP TX FIFO watermark empty interrupt */
#define __FLEXSPI_INTR_IPTXWE_MASK                  0x40u
#define __FLEXSPI_INTR_IPTXWE_SHIFT                 6

/* AHB Bus timeout interrupt.Refer Interrupts chapter for more details */
#define __FLEXSPI_INTR_AHBBUSTIMEOUT_MASK           0x400u
#define __FLEXSPI_INTR_AHBBUSTIMEOUT_SHIFT          10

/* Sequence execution timeout interrupt */
#define __FLEXSPI_INTR_SEQTIMEOUT_MASK              0x800u
#define __FLEXSPI_INTR_SEQTIMEOUT_SHIFT             11


/*******************************************************************************
  IP RX FIFO Control Register
*******************************************************************************/

/* Clear all valid data entries in IP TX FIFO */
#define __FLEXSPI_IPTXFCR_CLRIPTXF_SHIFT            0


/*******************************************************************************
  IP TX FIFO Control Register
*******************************************************************************/

/* Clear all valid data entries in IP RX FIFO */
#define __FLEXSPI_IPRXFCR_CLRIPRXF_SHIFT            0

/*******************************************************************************
  IP Control Register 1
*******************************************************************************/

/* Flash Read/Program Data Size (in Bytes) for IP command */
#define __FLEXSPI_IPCR1_IDATSZ_SHIFT                0
#define __FLEXSPI_IPCR1_IDATSZ(x)                  (((x) & 0xFFFF) << __FLEXSPI_IPCR1_IDATSZ_SHIFT)

/* Sequence Index in LUT for IP command */
#define __FLEXSPI_IPCR1_ISEQID_SHIFT                16
#define __FLEXSPI_IPCR1_ISEQID(x)                  (((x) & 0xF) << __FLEXSPI_IPCR1_ISEQID_SHIFT)

/* Sequence Number for IP command: ISEQNUM+1 */
#define __FLEXSPI_IPCR1_ISEQNUM_SHIFT               24
#define __FLEXSPI_IPCR1_ISEQNUM(x)                 (((x) & 0x7) << __FLEXSPI_IPCR1_ISEQNUM_SHIFT)

/* Parallel mode Enabled for IP command */
#define __FLEXSPI_IPCR1_IPAREN_SHIFT                31
#define __FLEXSPI_IPCR1_IPAREN(x)                  (((x) & 0x1) << __FLEXSPI_IPCR1_IPAREN_SHIFT)

/*******************************************************************************
  IP TX FIFO Control Register
*******************************************************************************/

/* Clear all valid data entries in IP TX FIFO */
#define __FLEXSPI_IPTXFCR_CLRIPTXF_SHIFT            0
#define __FLEXSPI_IPTXFCR_CLRIPTXF_MASK            (0x1 << __FLEXSPI_IPTXFCR_CLRIPTXF_SHIFT)

/* IP TX FIFO filling by DMA enabled  */
#define __FLEXSPI_IPTXFCR_TXDMAEN_SHIFT             1
#define __FLEXSPI_IPTXFCR_TXDMAEN_MASK             (0x1 << __FLEXSPI_IPTXFCR_TXDMAEN_SHIFT)

/* Watermark level is (TXWMRK+1)*64 Bits */
#define __FLEXSPI_IPTXFCR_TXWMRK_SHIFT              2
#define __FLEXSPI_IPTXFCR_TXWMRK(x)                (((x) & 0xF) << __FLEXSPI_IPTXFCR_TXWMRK_SHIFT)

/*******************************************************************************
  IP RX FIFO Control Register
*******************************************************************************/

/* Clear all valid data entries in IP RX FIFO */
#define __FLEXSPI_IPRXFCR_CLRIPRXF_SHIFT            0
#define __FLEXSPI_IPRXFCR_CLRIPRXF_MASK            (0x1 << __FLEXSPI_IPTXFCR_CLRIPTXF_SHIFT)

/* IP RX FIFO reading by DMA enabled. */
#define __FLEXSPI_IPRXFCR_RXDMAEN_SHIFT             1
#define __FLEXSPI_IPRXFCR_RXDMAEN_MASK             (0x1 << __FLEXSPI_IPTXFCR_TXDMAEN_SHIFT)

/* Watermark level is (RXWMRK+1)*64 Bits */
#define __FLEXSPI_IPRXFCR_RXWMRK_SHIFT              2
#define __FLEXSPI_IPRXFCR_RXWMRK(x)                (((x) & 0xF) << __FLEXSPI_IPTXFCR_TXWMRK_SHIFT)


/*******************************************************************************
  IP RX FIFO Status Register
*******************************************************************************/

/* Fill level of IP RX FIFO */
#define __FLEXSPI_IPRXFSTS_FILL_SHIFT            0
#define __FLEXSPI_IPRXFSTS_FILL_MASK            (0xFF << __FLEXSPI_IPRXFSTS_FILL_SHIFT)

/* Total Read Data Counter: RDCNTR * 64 Bits */
#define __FLEXSPI_IPRXFSTS_RDCNTR_SHIFT          16
#define __FLEXSPI_IPRXFSTS_RDCNTR_MASK          (0xFFFF << __FLEXSPI_IPRXFSTS_RDCNTR_SHIFT)

/*******************************************************************************
  DLL Control Register
*******************************************************************************/

/* DLL calibration enable */
#define __FLEXSPI_DLLCR_DLLEN_SHIFT               0
#define __FLEXSPI_DLLCR_DLLEN_MASK               (0x1 << __FLEXSPI_DLLCR_DLLEN_SHIFT)

/* Software could force a reset on DLL by setting this field to 0x1 */
#define __FLEXSPI_DLLCR_DLLRESET_SHIFT            1
#define __FLEXSPI_DLLCR_DLLRESET_MASK            (0x1 << __FLEXSPI_DLLCR_DLLRESET_SHIFT)

/*
 * The delay target for slave delay line is: ((SLVDLYTARGET+1) * 1/32 * clock cycle
 * of reference clock
 */
#define __FLEXSPI_DLLCR_SLVDLYTARGET_SHIFT        3
#define __FLEXSPI_DLLCR_SLVDLYTARGET(x)          (((x) & 0xF) << __FLEXSPI_DLLCR_SLVDLYTARGET_SHIFT)

/* Slave clock delay line delay cell number selection override enable */
#define __FLEXSPI_DLLCR_OVRDEN_SHIFT              8
#define __FLEXSPI_DLLCR_OVRDEN_MASK              (0x1 << __FLEXSPI_DLLCR_OVRDEN_SHIFT)

/* Slave clock delay line delay cell number selection override value */
#define __FLEXSPI_DLLCR_OVRDVAL_SHIFT             9
#define __FLEXSPI_DLLCR_OVRDVAL(x)               (((x) & 0x3F) << __FLEXSPI_DLLCR_OVRDVAL_SHIFT)

/*******************************************************************************
  Flash Control Register 4
*******************************************************************************/

#define FLEXSPI_FLSHCR4_WMENA_SHIFT               2
#define FLEXSPI_FLSHCR4_WMENA_MASK               (0x1 << FLEXSPI_FLSHCR4_WMENA_SHIFT)

/*******************************************************************************
  Flash Status Register 0
*******************************************************************************/
#define FLEXSPI_STATUS0_ARBIDLE_MASK              0x2
#define FLEXSPI_STATUS0_SEQIDLE_MASK              0x1

#endif /* __IMX1010_LPSPI_REGS_H */

/* end of file */
