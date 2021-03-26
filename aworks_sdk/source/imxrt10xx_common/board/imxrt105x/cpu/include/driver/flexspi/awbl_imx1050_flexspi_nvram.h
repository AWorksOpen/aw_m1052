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
 * \brief Flexspi NOR Flash NVRAM 接口适配
 *
 * \internal
 * \par modification history:
 * - 1.00 18-01-31  mex, first implementation
 * \endinternal
 */

#ifndef __AWBL_IMX1050_FLEXSPI_NVRAM_H
#define __AWBL_IMX1050_FLEXSPI_NVRAM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "driver/flexspi/awbl_flexspi_nor_flash.h"
#include "awbl_nvram.h"
#include "awbus_lite.h"


struct awbl_flexspi_nor_flash_nvram_info {

    /** \brief 非易失性存储段配置列表 */
    const struct awbl_nvram_segment   *p_seglst_list;

    /** \brief 非易失性存储段配置列表条目数 */
    size_t                             seglst_count;

    /** \brief NVRAM接口写缓存 */
    uint8_t                           *p_blk_buf;
};



aw_err_t awbl_flexspi_nor_flash_nvram_get (struct awbl_dev *p_awdev,
                                           char            *p_name,
                                           int              unit,
                                           void            *p_buf,
                                           int              offset,
                                           int              len);



aw_err_t awbl_flexspi_nor_flash_nvram_set (struct awbl_dev *p_awdev,
                                           char            *p_name,
                                           int              unit,
                                           char            *p_buf,
                                           int              offset,
                                           int              len);


#ifdef __cplusplus
}
#endif


#endif /* __AWBL_IMX1050_FLEXSPI_NVRAM_H */


/* end of file */



