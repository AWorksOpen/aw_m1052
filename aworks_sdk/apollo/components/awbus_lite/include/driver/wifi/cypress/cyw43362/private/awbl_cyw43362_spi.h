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
 * \brief AWBus cyw43362 spi driver private functions
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * #include "awbl_cyw43362_spi.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 17-07-11  phd, first implementation
 * \endinternal
 */

#ifndef __AWBL_CYW43362_SPI_PRIV_H
#define __AWBL_CYW43362_SPI_PRIV_H

#ifdef AW_DRV_AWBL_CYW43362_WIFI_SPI

aw_err_t awbl_cyw43362_spi_transfer (wwd_bus_transfer_direction_t  dir,
                                     uint8_t                      *buffer,
                                     uint16_t                      buffer_length);

#endif

#endif /* __AWBL_CYW43362_SPI_PRIV_H */

/* end of file */

