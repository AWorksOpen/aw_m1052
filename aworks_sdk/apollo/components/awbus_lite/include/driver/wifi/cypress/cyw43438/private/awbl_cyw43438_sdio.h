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
 * \brief AWBus cyw43438 sdio driver private functions
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * #include "awbl_cyw43438_sdio_priv.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 20-04-13  hsg, first implementation
 * \endinternal
 */

#ifndef __AWBL_CYW43438_SDIO_PRIV_H
#define __AWBL_CYW43438_SDIO_PRIV_H

#include "wwd_constants.h"
#include "aw_netif.h"

wwd_result_t awbl_43438_sdio_enable_hsp (void);
wwd_result_t awbl_43438_sdio_enable_oob_int (void);
uint8_t awbl_43438_sdio_get_int_pin (void);
wwd_result_t awbl_43438_sdio_transfer (wwd_bus_transfer_direction_t  direction,
                                       int                           function,
                                       uint32_t                      address,
                                       uint16_t                      data_size,
                                       uint8_t                      *data);
wwd_result_t awbl_43438_sdio_unmask_sdio_int (void);
void awbl_cyw43438_sdio_drv_register (void);
void awbl_cyw43438_sdio_release (aw_netif_t *p_ethif);


#endif /* __AWBL_CYW43438_SDIO_PRIV_H */

/* end of file */
