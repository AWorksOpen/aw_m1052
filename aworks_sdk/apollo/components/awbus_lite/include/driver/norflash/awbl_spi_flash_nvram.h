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
 * \brief SPI-Flash 驱动头文件
 *
 * \internal
 * \par modification history:
 * - 1.00 17-08-31  wengyedong, first implementation
 * \endinternal
 */

#ifndef __AWBL_SPI_FLASH_NVRAM_H
#define __AWBL_SPI_FLASH_NVRAM_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "driver/norflash/awbl_spi_flash.h"
#include "awbl_nvram.h"




struct awbl_spi_flash_nvram_info {
    const struct awbl_nvram_segment   *p_seglst;
    uint_t                             seglst_count;
    
    /** \brief buffer for NVRAM to write */
    uint8_t                           *p_blk_buf;     
};




aw_err_t awbl_spi_flash_nvram_get (struct awbl_dev *p_awdev,
                                   char            *p_name,
                                   int              unit,
                                   void            *p_buf,
                                   int              offset,
                                   int              len);

aw_err_t awbl_spi_flash_nvram_set (struct awbl_dev *p_awdev,
                                   char            *p_name,
                                   int              unit,
                                   char            *p_buf,
                                   int              offset,
                                   int              len);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_SPI_FLASH_NVRAM_H */

/* end of file */

