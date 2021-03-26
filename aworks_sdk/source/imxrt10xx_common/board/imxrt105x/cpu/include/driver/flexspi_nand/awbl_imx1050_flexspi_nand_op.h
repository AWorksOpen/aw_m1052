/*
 * awbl_imx1050_flexspi_nand_op.h
 *
 *  Created on: 2019Äê11ÔÂ18ÈÕ
 *      Author: zengqingyuhang
 */

#ifndef __AWBL_IMX1050_FLEXSPI_NAND_OP_H
#define __AWBL_IMX1050_FLEXSPI_NAND_OP_H

/**
 * Read Operation And Serial Output
 */

#define AWBL_FLEXSPI_NAND_CMD_BLOCK_ERASE                    0xD8
#define AWBL_FLEXSPI_NAND_CMD_GET_FEATURE                    0x0F
#define AWBL_FLEXSPI_NAND_CMD_SET_FEATURE                    0x1F
#define AWBL_FLEXSPI_NAND_CMD_WRITE_DISENABLE                0x04
#define AWBL_FLEXSPI_NAND_CMD_WRITE_ENABLE                   0x06
#define AWBL_FLEXSPI_NAND_CMD_PROGRAM_LOAD_X1                0x02
#define AWBL_FLEXSPI_NAND_CMD_PROGRAM_LOAD_X4                0x32
#define AWBL_FLEXSPI_NAND_CMD_PROGRAM_LOAD_RANDOM_DATA_X1    0x84
#define AWBL_FLEXSPI_NAND_CMD_PROGRAM_LOAD_RANDOM_DATA_X4    0x34
#define AWBL_FLEXSPI_NAND_CMD_PROGRAM_EXECUTE                0x10
#define AWBL_FLEXSPI_NAND_CMD_PAGE_READ_TO_CACHE             0x13
#define AWBL_FLEXSPI_NAND_CMD_READ_FROM_CACHE_X1             0x0B
#define AWBL_FLEXSPI_NAND_CMD_READ_FROM_CACHE_X2             0x3B
#define AWBL_FLEXSPI_NAND_CMD_READ_FROM_CACHE_X4             0x6B
#define AWBL_FLEXSPI_NAND_CMD_READ_ID                        0x9F
#define AWBL_FLEXSPI_NAND_CMD_RESET                          0xFF

/**
 * Get Status Register
 * Software can read status register during the NAND device operation by
 * issuing GET FEATURE (0Fh) command, followed by the feature address C0h.
 */
#endif  /* __AWBL_IMX1050_FLEXSPI_NAND_OP_H */


