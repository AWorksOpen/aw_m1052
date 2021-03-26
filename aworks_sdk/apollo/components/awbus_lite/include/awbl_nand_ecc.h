/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief nand controler software ecc driver head file
 *
 * \internal
 * \par modification history:
 * - 1.00 17-3-22  vih, first implementation
 * - 2.00 02-04-14 vih, refactoring
 * \endinternal
 */

#ifndef __AWBL_NAND_ECC_H
#define __AWBL_NAND_ECC_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "awbl_nand_pf.h"
#include "mtd/aw_mtd.h"

typedef struct awbl_nand_ecc_init awbl_nand_ecc_init_t;
typedef struct awbl_nand_ecc awbl_nand_ecc_t;


typedef struct awbl_nand_ecc_init {
    awbl_nand_info_t          *p_info;
    awbl_nand_platform_serv_t *p_pf_serv;
} awbl_nand_ecc_init_t;

typedef struct {

    /** \brief 存放计算得到的ECC的缓冲区,大小为oob_layout中ecc的长度 */
    uint8_t *calc_ecc;

    /** \brief oob的缓冲区，大小为oobsize */
    uint8_t *oob_buf;

} awbl_nand_ecc_buffers_t;

typedef struct awbl_nand_ecc_itf {
    void (*hwctl)         (awbl_nand_ecc_t           *p_this, int mode);

    int (*read_page_raw)  (awbl_nand_ecc_t           *p_this,
                           uint8_t                   *data_buf,
                           uint8_t                   *oob_free,
                           uint16_t                   oob_free_size,
                           aw_bool_t                  oob_required,
                           uint32_t                   page);

    int (*write_page_raw) (awbl_nand_ecc_t           *p_this,
                           uint8_t                   *data_buf,
                           uint8_t                   *oob_free,
                           uint16_t                   oob_free_size,
                           aw_bool_t                  oob_required,
                           uint32_t                   page);

    int (*read_page)      (awbl_nand_ecc_t           *p_this,
                           uint8_t                   *buf,
                           uint8_t                   *oob_free,
                           uint16_t                   oob_free_size,
                           aw_bool_t                  oob_required,
                           uint32_t                   page);

    int (*write_page)     (awbl_nand_ecc_t           *p_this,
                           uint8_t                   *buf,
                           uint8_t                   *oob_free,
                           uint16_t                   oob_free_size,
                           aw_bool_t                  oob_required,
                           uint32_t                   page);

    int (*read_oob_free)       (awbl_nand_ecc_t         *p_this,
                                uint32_t                 page,
                                uint32_t                 free_offs,
                                uint8_t                 *oob_free,
                                uint16_t                 free_buf_len);

    int (*write_oob_free)      (awbl_nand_ecc_t         *p_this,
                                uint32_t                 page,
                                uint32_t                 free_offs,
                                uint8_t                 *oob_free,
                                uint16_t                 free_buf_len);

} awbl_nand_ecc_itf_t;


typedef struct awbl_nand_ecc {
    awbl_nand_ecc_itf_t  itf;
    int                  size;          /**< \brief 每个 ecc 步骤计算的数据量 */
    int                  steps;         /**< \brief 产生 ecc 步骤  */
    int                  bytes;         /**< \brief 每步产生的  ecc 字节 */
    int                  total_bytes;   /**< \brief bytes * steps */
    int                  strength;
    uint8_t             *ecc_buf;       /**< \brief 缓存 ecc 数据  */
    uint16_t             ecc_buf_size;  /**< \brief 缓存 ecc 大小 */
    void                *priv;

    awbl_nand_info_t          *p_info;
    awbl_nand_platform_serv_t *p_pf_serv;

    awbl_nand_ecc_buffers_t   *buffers;
} awbl_nand_ecc_t;

#ifdef __cplusplus
}
#endif /* __cplusplus   */

awbl_nand_ecc_t *awbl_nand_ecc_create (awbl_nand_ecc_init_t *p_init);
aw_err_t awbl_nand_ecc_register (enum awbl_nand_ecc_mode ecc_mode,
                                 awbl_nand_ecc_t *(*pfn_custom_create) (
                                         awbl_nand_ecc_init_t *p));

void awbl_nand_ecc_itf_def_set (awbl_nand_ecc_t *p_ecc);


#endif  /* __AWBL_NAND_ECC_H */

/* end of file */
