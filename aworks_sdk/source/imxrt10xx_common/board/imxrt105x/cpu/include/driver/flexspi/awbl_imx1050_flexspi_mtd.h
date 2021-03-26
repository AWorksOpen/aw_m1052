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
**** ***************************************************************************/

/**
 * \file
 * \brief Flexspi NOR Flash MTD 接口适配
 *
 * \internal
 * \par modification history:
 * - 1.00 18-01-31  mex, first implementation
 * \endinternal
 */


#ifndef __AWBL_IMX1050_FLEXSPI_MTD_H
#define __AWBL_IMX1050_FLEXSPI_MTD_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "mtd/aw_mtd.h"
#include "driver/flexspi/awbl_imx1050_flexspi.h"
#include "driver/flexspi/awbl_flexspi_nor_flash.h"


struct awbl_flexspi_flash_mtd_info {
    const char   *name;           /**< \brief MTD设备名称   */
    uint32_t      start_blk;      /**< \brief MTD设备起始块 */
    uint32_t      nblks;          /**< \brief MTD设备块个数 */
};


struct awbl_flexspi_flash_mtd {
    struct aw_mtd_info                        mtd;
    struct awbl_imx1050_flexspi_dev           *p_flash;
    const struct awbl_flexspi_flash_mtd_info  *p_info;

    off_t     erase_align_mask;       /** \brief mask erase align on block boundary */
    int       addr_offset;            /** \brief start address in the full chip */
};


aw_err_t awbl_flexspi_flash_mtd_init (struct awbl_imx1050_flexspi_dev *p_flash,
                                      void                            *p_obj,
                                      const void                      *p_info);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX1050_FLEXSPI_MTD_H */

/* end of file */
