/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief Nandflash chip attribute
 *
 * \internal
 * \par modification history:
 * - 1.00 20-04-14  vih, first implementation
 * \endinternal
 */

#ifndef __AWBL_NAND_CHIP_ATTR_H
#define __AWBL_NAND_CHIP_ATTR_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */


/**
 * \name nand flash chip attribute
 * @{
 */

/** \brief SAMSUNG K9F2G08时序 */
#define NANDFLASH_K9F2G08_TIMING \
    20, \
    5, \
    15, \
    5, \
    15, \
    5, \
    15, \
    5, \
    15, \
    20, \
    25, \
    30, \
    30, \
    100, \
    10, \
    15, \
    30

/** \brief SAMSUNG K9F2G08 ECC校验强度 */
#define NANDFLASH_K9F2G08_ECC_STRENGTH \
    4

/** \brief SAMSUNG K9F2G08芯片信息 */
#define NANDFLASH_K9F2G08 \
    2048, \
    64, \
    64, \
    2048, \
    0, \
    8, \
    128*1024,\
    256*1024*1024, \
    0xEC, \
    0xDA

/** \brief MXIC MX30LF1G08时序 */
#define NANDFLASH_MX30LF1G08_TIMING \
    20, \
    5, \
    15, \
    5, \
    15, \
    5, \
    5, \
    5, \
    15, \
    20, \
    25, \
    50, \
    30, \
    50, \
    15, \
    15, \
    30

/** \brief MXIC MX30LF1G08 ECC校验强度 */
#define NANDFLASH_MX30LF1G08_ECC_STRENGTH \
    4

/** \brief MXIC MX30LF1G08芯片信息 */
#define NANDFLASH_MX30LF1G08 \
    2048, \
    64, \
    64, \
    1024, \
    0, \
    8, \
    128*1024,\
    128*1024*1024, \
    0xC2, \
    0xF1

/** \brief MXIC MX30LF2G28时序 */
#define NANDFLASH_MX30LF2G28_TIMING \
    15, \
    5, \
    10, \
    5, \
    10, \
    5, \
    7, \
    5, \
    10, \
    16, \
    25, \
    50, \
    20, \
    100, \
    10, \
    10, \
    20

/** \brief MXIC MX30LF2G28 ECC校验强度 */
#define NANDFLASH_MX30LF2G28_ECC_STRENGTH \
    8

/** \brief MXIC MX30LF2G28芯片信息 */
#define NANDFLASH_MX30LF2G28\
    2048, \
    112, \
    64, \
    2048, \
    0, \
    8, \
    128*1024,\
    128*1024*2048, \
    0xC2, \
    0xDA

/** \brief SPANSION S34ML01G2时序 */
#define NANDFLASH_S34ML01G2_TIMING \
    20, \
    5, \
    10, \
    5, \
    10, \
    5, \
    10, \
    5, \
    12, \
    20, \
    25, \
    30, \
    25, \
    100, \
    10, \
    12, \
    25

/** \brief SPANSION S34ML01G2 ECC校验强度 */
#define NANDFLASH_S34ML01G2_ECC_STRENGTH \
    4

/** \brief SPANSION S34ML01G2芯片信息 */
#define NANDFLASH_S34ML01G2\
    2048, \
    64, \
    64, \
    1024, \
    0, \
    8, \
    128*1024, \
    128*1024*1024, \
    0x01, \
    0xF1

/** \brief SPANSION S34ML02G2时序 */
#define NANDFLASH_S34ML02G2_TIMING \
    20, \
    5, \
    10, \
    5, \
    10, \
    5, \
    10, \
    15, \
    12, \
    20, \
    25, \
    30, \
    25, \
    100, \
    10, \
    12, \
    25

/** \brief SPANSION S34ML02G2 ECC校验强度 */
#define NANDFLASH_S34ML02G2_ECC_STRENGTH \
    4

/** \brief SPANSION S34ML02G2芯片信息 */
#define NANDFLASH_S34ML02G2\
    2048, \
    128, \
    64, \
    2048, \
    0, \
    8, \
    128*1024, \
    128*1024*2048, \
    0x01, \
    0xDA

/**
 * @} nand flash chip attribute
 */

#endif /* __AWBL_NAND_CHIP_ATTR_H */

/* end of file */
