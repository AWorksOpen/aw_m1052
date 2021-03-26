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
 * \brief iMX RT1050 FlexSPI driver
 *
 * \internal
 * \par modification history:
 * - 1.00 17-11-1  mex, first implementation
 * \endinternal
 */

#ifndef __AWBL_IMX1050_FLEXSPI_OP_H
#define __AWBL_IMX1050_FLEXSPI_OP_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \brief Note on opcode nomenclature: some opcodes have a format like
 *        FLEXSPI_NOR_OP_FUNCTION{4,}_x_y_z{_D}. The numbers x, y, and z stand for the number
 *        of I/O lines used for the opcode, address, and data (respectively). The
 *        FUNCTION has an optional suffix of '4', to represent an opcode which
 *        requires a 4-byte (32-bit) address. The suffix of 'D' stands for the
 *        DDR mode.
 */

/* Flash opcodes. */
#define AWBL_FLEXSPI_NOR_OP_WREN          0x06    /* Write enable */
#define AWBL_FLEXSPI_NOR_OP_RDSR          0x05    /* Read status register */
#define AWBL_FLEXSPI_NOR_OP_WRSR          0x01    /* Write status register 1 byte */
#define AWBL_FLEXSPI_NOR_OP_READ          0x03    /* Read data bytes (low frequency) */
#define AWBL_FLEXSPI_NOR_OP_READ_FAST     0x0b    /* Read data bytes (high frequency) */
#define AWBL_FLEXSPI_NOR_OP_READ_1_1_2    0x3b    /* Read data bytes (Dual SPI) */
#define AWBL_FLEXSPI_NOR_OP_READ_1_1_4    0x6b    /* Read data bytes (Quad SPI) */
#define AWBL_FLEXSPI_NOR_OP_READ_1_1_4_4  0x6c    /* Read data bytes (Quad SPI 4 byte mode) */
#define AWBL_FLEXSPI_NOR_OP_READ_1_1_4_D  0x6d    /* Read data bytes (DDR Quad SPI) */
#define AWBL_FLEXSPI_NOR_OP_READ_1_4_4_D  0xed    /* Read data bytes (DDR Quad SPI) */
#define AWBL_FLEXSPI_NOR_OP_PP_1          0x02    /* Page program single mode(up to 256 bytes) */
#define AWBL_FLEXSPI_NOR_OP_PP_4          0x38    /* Page program quad mode(up to 256 bytes) */
#define AWBL_FLEXSPI_NOR_OP_PP_4_4        0x3E    /* Page program quad mode(up to 256 bytes , 4byte mode) */
#define AWBL_FLEXSPI_NOR_OP_BE_4K         0x20    /* Erase 4KB block */
#define AWBL_FLEXSPI_NOR_OP_BE_4K_4       0x20    /* Erase 4KB block (4byte mode) */
#define AWBL_FLEXSPI_NOR_OP_BE_4K_PMC     0xd7    /* Erase 4KB block on PMC chips */
#define AWBL_FLEXSPI_NOR_OP_BE_32K        0x52    /* Erase 32KB block */
#define AWBL_FLEXSPI_NOR_OP_CHIP_ERASE    0xc7    /* Erase whole flash chip */
#define AWBL_FLEXSPI_NOR_OP_SE            0xd8    /* Sector erase (usually 64KB) */
#define AWBL_FLEXSPI_NOR_OP_RDID          0x9f    /* Read JEDEC ID */
#define AWBL_FLEXSPI_NOR_OP_RDCR          0x35    /* Read configuration register */
#define AWBL_FLEXSPI_NOR_OP_RDFR          0x48    /* Read function register */
#define AWBL_FLEXSPI_NOR_OP_WRFR          0x42    /* write function register */
#define AWBL_FLEXSPI_NOR_OP_IRE           0x64    /* Information Row Erase Operation */
#define AWBL_FLEXSPI_NOR_OP_IRP           0x62    /* Information Row Program Operation */
#define AWBL_FLEXSPI_NOR_OP_IRRD          0x68    /* Information Row Read Operation */

/* 4-byte address opcodes - used on Spansion and some Macronix flashes. */
#define AWBL_FLEXSPI_NOR_OP_READ4         0x13    /* Read data bytes (low frequency) */
#define AWBL_FLEXSPI_NOR_OP_READ4_FAST    0x0c    /* Read data bytes (high frequency) */
#define AWBL_FLEXSPI_NOR_OP_READ4_1_1_2   0x3c    /* Read data bytes (Dual SPI) */
#define AWBL_FLEXSPI_NOR_OP_READ4_1_1_4   0x6c    /* Read data bytes (Quad SPI) */
#define AWBL_FLEXSPI_NOR_OP_READ4_1_4_4_D 0xee    /* Read data bytes (DDR Quad SPI) */
#define AWBL_FLEXSPI_NOR_OP_PP_4B         0x12    /* Page program (up to 256 bytes) */
#define AWBL_FLEXSPI_NOR_OP_SE_4B         0xdc    /* Sector erase (usually 64KiB) */

/* Used for SST flashes only. */
#define AWBL_FLEXSPI_NOR_OP_BP            0x02    /* Byte program */
#define AWBL_FLEXSPI_NOR_OP_WRDI          0x04    /* Write disable */
#define AWBL_FLEXSPI_NOR_OP_AAI_WP        0xad    /* Auto address increment word program */

/* Used for Macronix and Winbond flashes. */
#define AWBL_FLEXSPI_NOR_OP_EN4B          0xb7    /* Enter 4-byte mode */
#define AWBL_FLEXSPI_NOR_OP_EX4B          0xe9    /* Exit 4-byte mode */

/* Used for Spansion flashes only. */
#define AWBL_FLEXSPI_NOR_OP_BRWR          0x17    /* Bank register write */

/* Status Register bits. */
#define AWBL_FLEXSPI_NOR_SR_WIP           1       /* Write in progress */
#define AWBL_FLEXSPI_NOR_SR_WEL           2       /* Write enable latch */

/* meaning of other SR_* bits may differ between vendors */
#define AWBL_FLEXSPI_NOR_SR_BP0           4       /* Block protect 0 */
#define AWBL_FLEXSPI_NOR_SR_BP1           8       /* Block protect 1 */
#define AWBL_FLEXSPI_NOR_SR_BP2           0x10    /* Block protect 2 */
#define AWBL_FLEXSPI_NOR_SR_SRWD          0x80    /* SR write protect */

#define AWBL_FLEXSPI_NOR_SR_QUAD_EN_MX    0x40    /* Macronix Quad I/O */

/* Configuration Register bits. */
#define AWBL_FLEXSPI_NOR_CR_QUAD_EN_SPAN  0x2     /* Spansion Quad I/O */

typedef enum awbl_flexspi_read_mode {
    AWBL_FLEXSPI_NOR_NORMAL = 0,
    AWBL_FLEXSPI_NOR_FAST,
    AWBL_FLEXSPI_NOR_DUAL,
    AWBL_FLEXSPI_NOR_SDR_QUAD,
    AWBL_FLEXSPI_NOR_DDR_QUAD,
} awbl_flexspi_read_mode_t;

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX1050_FLEXSPI_OP_H */

/* end of file */
