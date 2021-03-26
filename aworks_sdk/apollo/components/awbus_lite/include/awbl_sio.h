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
 * \brief Header file for serial driver interface
 *
 * \internal
 * \par modification history
 * - 1.00 12-10-26  orz, created
 * \endinternal
 */

#ifndef __AW_SIO_H
#define __AW_SIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_common.h"
#include "aw_sio_common.h"
#include "aw_ioctl.h"
#include "awbus_lite.h"
#include "aw_serial.h"

/**
 * \name SIO回调类型
 * @{
 */
#define AW_SIO_CALLBACK_GET_TX_CHAR     0
#define AW_SIO_CALLBACK_PUT_RCV_CHAR    1
#define AW_SIO_CALLBACK_ERROR           3
/** @} */

/**
 * \name SIO错误回调返回值
 * @{
 */
#define AW_SIO_ERROR_NONE               (-1)
#define AW_SIO_ERROR_FRAMING            0
#define AW_SIO_ERROR_PARITY             1
#define AW_SIO_ERROR_OFLOW              2
#define AW_SIO_ERROR_UFLOW              3
#define AW_SIO_ERROR_CONNECT            4
#define AW_SIO_ERROR_DISCONNECT         5
#define AW_SIO_ERROR_NO_CLK             6
#define AW_SIO_ERROR_UNKNWN             7
/** @} */

/**
 * \name SIO回调类型
 * @{
 */

/** \brief Get Tx Char callback */
typedef int  (*pfunc_sio_txchar_get_t)(void *p_arg, char *p_char);

/** \brief Put Rx Char callback */
typedef int  (*pfunc_sio_rxchar_put_t)(void *p_arg, char  ch);

/** \brief Error callback codes */
typedef int  (*pfunc_sio_err_t)(void *p_arg, int code, void *p_data, int size);

/** @} */

/**
 * \name serial device data structures
 * @{
 */

struct aw_sio_drv_funcs;

/** \brief a serial channel */
struct aw_sio_chan {
    const struct aw_sio_drv_funcs *p_drv_funcs;
};

/** \brief serial io channel information */
struct aw_sio_chan_info {
    int   chan_no;  /**< \brief channel number desired */
    void *p_chan;   /**< \brief p_chan for specified channel */
};

/** \brief serial io driver functions */
struct aw_sio_drv_funcs {
    int (*pfunc_tx) (struct aw_sio_chan *p_siochan,
                     const uint8_t      *p_buffer,
                     size_t              nbytes);

    /** \brief sio pool output */
    int (*pfunc_poll_output) (struct aw_sio_chan *p_siochan,
                              uint8_t             outchar);

    /** \brief sio dcb config */
    int (*pfunc_dcb_set) (struct aw_sio_chan         *p_siochan,
                          const struct aw_serial_dcb *p_dcb);

    /** \brief sio dcb get */
    int (*pfunc_dcb_get) (struct aw_sio_chan   *p_siochan,
                          struct aw_serial_dcb *p_dcb);

    /** \brief receive buffer decrease  */
    int (*pfunc_recv_decrease) (struct aw_sio_chan *p_siochan,
                                const int           freebytes);
};

/** @} */

/**
 * \name sio macros
 * @{
 */


#define aw_sio_tx(siochan, buf, nbytes) \
    ((siochan).p_drv_funcs->pfunc_tx(&siochan, buf, nbytes))

#define aw_sio_poll_output(siochan, thischar) \
    ((siochan).p_drv_funcs->pfunc_poll_output(&siochan, thischar))

#define aw_sio_dcb_set(siochan, dcb) \
    ((siochan).p_drv_funcs->pfunc_dcb_set(&siochan, dcb))

#define aw_sio_dcb_get(siochan, dcb) \
    ((siochan).p_drv_funcs->pfunc_dcb_get(&siochan, dcb))

#define aw_sio_recv_decrease(siochan, freebytes) \
    ((siochan).p_drv_funcs->pfunc_recv_decrease(&siochan, freebytes))


/** @} */

/** \brief sio methods */
AWBL_METHOD_IMPORT(aw_sio_chan_get);
AWBL_METHOD_IMPORT(aw_sio_chan_connect);

#ifdef __cplusplus
}
#endif

#endif  /* __AW_SIO_H */

/* end of file */
