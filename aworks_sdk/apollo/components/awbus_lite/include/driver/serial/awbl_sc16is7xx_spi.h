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
 * \brief PHILIPS SC16IS752/SC16IS762 SPI-bus interface driver support
 *
 * SC16IS7xx series are NS16550 compatible Dual UART with I2C-bus/SPI interface
 *
 * \internal
 * \par modification history:
 * - 1.00 12-11-28  zen, first implementation
 * \endinternal
 */

#include "aw_common.h"
#include "aw_spinlock.h"
#include "awbus_lite.h"
#include "awbl_access.h"

#include "driver/serial/awbl_ns16550.h"

#include "awbl_spibus.h"

/*******************************************************************************
    types
*******************************************************************************/

/** \brief SC16IS7xx SPI-bus channel */
struct awbl_sc16is7xx_spi_chan {
    struct awbl_spi_device   spidev;    /**< \brief spi device data */
    struct awbl_ns16550_chan nschan;    /**< \brief NS16550 channel data */
    struct awbl_reg_access   access;    /**< \brief the register access func */
    int                      chan_no;   /**< \brief SC16IS7xx interal channel */
};

/** \brief SC16IS7xx SPI-bus channel parameter */
struct awbl_sc16is7xx_spi_chan_param {

    /** \brief NS16550 parameter, always go first */
    struct awbl_ns16550_chan_param  nsparam;
     
    int             cs_pin;     /**< \brief SC16IS7xx chip select pin number */
    uint32_t        max_speed_hz;   /**< \brief SC16IS7xx chip max speed (Hz) */
    unsigned char   chan_no;    /**< \brief SC16IS7xx interal channel */
};

/* end of file */
