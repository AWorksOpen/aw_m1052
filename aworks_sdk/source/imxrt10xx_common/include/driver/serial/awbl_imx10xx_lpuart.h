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
 * \brief IMX10XX LPUART
 *
 * \internal
 * \par modification history:
 * - 1.00 17-08-25  smc, first implementation
 * \endinternal
 */

#ifndef __AWBL_IMX10XX_LPUART_H__
#define __AWBL_IMX10XX_LPUART_H__

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_spinlock.h"
#include "aw_serial.h"
#include "awbl_sio.h"
#include "driver/serial/awbl_serial_private.h"

#define AWBL_IMX10XX_LPUART_DRV_NAME       "imx10xx_lpuart"

/*******************************************************************************
     types
*******************************************************************************/

/** \brief imx10xx lpuart initialization structure
 *         for plb based AWBus devices
 * */
typedef struct awbl_imx10xx_lpuart_dev_info {
    int             inum;                   /**< \brief Actual architectural vector */
    uint32_t        regbase;                /**< \brief Virt address of register set */
    aw_clk_id_t     async_parent_clk_id;    /**< \brief clk id */
    char           *recv_buf;               /**< \brief receive buffer base */
    uint32_t        recv_size;              /**< \brief receive buffer size */
    uint32_t        init_baudrate;          /**< \brief baud init */
    /** \brief BSP function to pre initialize the device */
    void            (*pfunc_plfm_init)(void);
    void            (*pfunc_rs485_dir)(uint8_t dir);
}awbl_imx10xx_lpuart_dev_info_t;

/** \brief imx10xx uart dev. one dev may contain multi chan.  */
typedef struct awbl_imx10xx_uart_dev {
    /** \brief awbl device data */
    struct awbl_dev             dev;
    /** \brief aw serial data */
    struct awbl_serial          serial;
    AW_SEMB_DECL(w_semb);
    AW_SEMB_DECL(w_485semb);
}awbl_imx10xx_uart_dev_t;

void awbl_imx10xx_lpuart_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX10XX_LPUART_H__ */

/* end of file */


