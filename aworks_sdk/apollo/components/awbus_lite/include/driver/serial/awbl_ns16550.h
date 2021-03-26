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
*******************************************************************************/

/**
 * \file
 * \brief National Semiconductor 16550 compatible UART header file
 *
 * \internal
 * \par modification history
 *
 * - 2.00 20-02-12  deo, refactoring (to support new serial)
 * - 1.00 12-10-25  orz, first implementation
 * \endinternal
 */


#ifndef __AWBL_NS16550_H
#define __AWBL_NS16550_H

#ifdef __cplusplus
extern "C" {
#endif

#include "awbus_lite.h"
#include "driver/serial/awbl_serial_private.h"
#include "aw_sem.h"

typedef struct awbl_ns16550_info {
    int         inum;      /**< \brief interrupt number */
    uint32_t    regbase;   /**< \brief register start address */
    uint16_t    regsize;   /**< \brief register size (2^regsize) */
    uint16_t    fifosize;  /**< \brief FIFO depth */
    uint8_t    *p_rxbuf;   /**< \brief receive buffer */
    uint32_t    bufsize;   /**< \brief receive buffer size */

    const struct aw_serial_dcb *init_cfg;

    /* baud rate divisor register calculate, return divisor value */
    uint32_t  (*pfn_divisor) (uint32_t baudrate);

    void      (*pfn_plfm_init) (void);

    void      (*pfn_rs485_dir) (uint8_t dir);
#define NS16550_RS485_TX_MODE           0x01
#define NS16550_RS485_RX_MODE           0x00
} awbl_ns16550_info_t;

typedef struct awbl_ns16550_dev {
    /** \brief AWBus device */
    struct awbl_dev             awdev;
    /** \brief serial device */
    struct awbl_serial          serial;

    uint8_t                     ier;
    uint8_t                     lcr;
    uint8_t                     mcr;
    uint8_t                     bmask;
    
    AW_SEMB_DECL(               semb);
//    AW_SEMB_DECL(w_485semb);
} awbl_ns16550_dev_t;

void awbl_ns16550_drv_register (void);


#ifdef __cplusplus
}
#endif

#endif /* __AWBL_NS16550_H */

/* end of file */

