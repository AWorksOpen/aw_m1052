/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2015 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief IMX1050 SEMC head file
 *
 * define IMX1050 SEMC data structure and functions
 *
 * \internal
 * \par modification history:
 * - 1.00 17-12-18  smc, first implementation
 * \endinternal
 */
#ifndef __AWBL_IMX1050_SEMC_NAND_H
#define __AWBL_IMX1050_SEMC_NAND_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

#include "aworks.h"
#include "aw_sem.h"
#include "awbl_nand_bus.h"
#include "driver/nand/awbl_imx1050_nand.h"
//#include "driver/dma/awbl_imx1050_dma.h"
#include "aw_cpu_clk.h"

/** \brief The Driver name define */
#define AWBL_IMX1050_SEMC_NAME   "awbl_imx1050_semc"

/** \brief CE ID define, The SEMC supports up to four NAND chip selects */
#define AWBL_IMX1050_SEMC_CE0        0
#define AWBL_IMX1050_SEMC_CE1        1
#define AWBL_IMX1050_SEMC_CE2        2
#define AWBL_IMX1050_SEMC_CE3        3

/** \brief IMX1050 SEMC device info */
struct awbl_imx1050_semc_devinfo {

    /** \brief  The nand controller information */
//    struct awbl_nandflash_controller_devinfo controller_info;

    uint8_t      nandbus_index;

    /** \brief The semc regbase */
    uint32_t     semc_regbase;

    /** \brief get the semc io_clock frequency */
    aw_clk_id_t  clk_io;

    int                inum;           /**< \brief ÖÐ¶ÏºÅ */

    /** \brief The platform */
    void (*pfn_plfm_init)(void);
};

/**
 * struct bch_geometry - BCH geometry description.
 * @gf_len:                   The length of Galois Field. (e.g., 13 or 14)
 * @ecc_strength:             A number that describes the strength of the ECC
 *                            algorithm.
 * @page_total_size:          The size, in bytes, of a physical page, including
 *                            both data and OOB.
 * @metadata_size:            The size, in bytes, of the metadata.
 * @ecc_chunk_size:           The size, in bytes, of a single ECC chunk. Note
 *                            the first chunk in the page includes both data and
 *                            metadata, so it's a bit larger than this value.
 * @ecc_chunk_count:          The number of ECC chunks in the page,
 * @payload_size:             The size, in bytes, of the payload buffer.
 * @auxiliary_size:           The size, in bytes, of the auxiliary buffer.
 * @auxiliary_status_offset:  The offset into the auxiliary buffer at which
 *                            the ECC status appears.
 * @block_mark_byte_offset:   The byte offset in the ECC-based page view at
 *                            which the underlying physical block mark appears.
 * @block_mark_bit_offset:    The bit offset into the ECC-based page view at
 *                            which the underlying physical block mark appears.
 */
struct bch_geometry {
    unsigned int  gf_len;
    unsigned int  ecc_strength;
    unsigned int  page_total_size;
    unsigned int  metadata_size;
    unsigned int  ecc_chunk_size;
    unsigned int  ecc_chunk_count;
    unsigned int  payload_size;
    unsigned int  auxiliary_size;
    unsigned int  auxiliary_status_offset;
    unsigned int  block_mark_byte_offset;
    unsigned int  block_mark_bit_offset;
    int           oob_free_len;
    unsigned int  bch_max_ecc_strength;
};

struct __imx1050_semc_regs;

/** \brief IMX1050 SEMC device */
struct awbl_imx1050_semc {
    struct awbl_nandbus                nandbus;
    awbl_nand_info_t                  *p_nand_info;

    struct __imx1050_semc_regs        *p_semc_regs;

};

void awbl_imx1050_semc_drv_register (void);

#ifdef __cplusplus
}
#endif    /* __cplusplus */

#endif /* __AWBL_IMX1050_SEMC_NAND_H */

/* end of file */


