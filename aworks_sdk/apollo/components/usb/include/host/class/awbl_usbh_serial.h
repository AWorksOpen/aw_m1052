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

#ifndef __AWBL_USBH_SERIAL_H
#define __AWBL_USBH_SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_usb_os.h"
#include "aw_list.h"
#include "host/awbl_usbh.h"
#include "awbl_sio.h"


/** \brief USB serial  */
struct awbl_usbh_serial {
    /** \brief point to USB interface . */
    struct awbl_usbh_function       *p_fun;

    aw_usb_mutex_handle_t            lock;
    struct aw_list_head              node;

    /** \brief ports of this serial. */
    struct awbl_usbh_serial_port    *ports;
    /** \brief count of ports. */
    uint8_t                          nports;
    size_t                           buf_size;

    /** \brief private pointer for sub device. */
    void                            *priv;


    /** \brief start up this serial, call by connect. */
    aw_err_t (*pfn_startup) (struct awbl_usbh_serial_port *p_port);
    /** \brief set serial termios. */
    aw_err_t (*pfn_set_termios) (struct awbl_usbh_serial_port *p_port);
    /** \brief callback function of EPs. */
    aw_err_t (*pfn_in_cb) (struct awbl_usbh_serial_port *p_port);
    aw_err_t (*pfn_out_cb) (struct awbl_usbh_serial_port *p_port);
    aw_err_t (*pfn_intr_cb) (struct awbl_usbh_serial_port *p_port);

};



struct awbl_usbh_serial_buf {
    void      *p_buf;
    int        len;
};



/** \brief USB serial port pipe */
struct awbl_usbh_serial_pipe {
    struct awbl_usbh_endpoint   *p_ep;
    struct awbl_usbh_trp         trp;        /**< \brief USB TRP for this EP. */
    aw_bool_t                    done;
    struct awbl_usbh_serial_buf  buf[2];
};




/** \brief USB serial channel */
struct awbl_usbh_serial_chan {
    struct aw_sio_chan       sio;           /**< \brief USB EndPoint address */
    int                    (*pfn_txchar_get) (void *, char *);
    int                    (*pfn_rxchar_put) (void *, char);
    void                    *p_txget_arg;
    void                    *p_rxput_arg;
    int                      channel_no;     /**< \brief channel number */
    uint8_t                  ctrl_line;
    uint8_t                  channel_mode;
    uint16_t                 options;        /**< \brief channel options */
    uint32_t                 baud_rate;
};






/** \brief USB serial port */
struct awbl_usbh_serial_port {

    /** \brief point to father USB serial. */
    struct awbl_usbh_serial        *p_ser;

    /** \brief all EPs for this port */
    struct awbl_usbh_serial_pipe    in;
    struct awbl_usbh_serial_pipe    out;
    struct awbl_usbh_serial_pipe    intr;

    /** \brief SIO chan */
    struct awbl_usbh_serial_chan    chan;

    uint8_t                         line_sta;
    uint8_t                         event;
    /** \brief private pointer for sub driver. */
    void                           *priv;

};

aw_err_t awbl_usbh_serial_init (void);


aw_err_t awbl_usbh_serial_create (struct awbl_usbh_serial   *p_ser,
                                  struct awbl_usbh_function *p_fun,
                                  size_t                     buf_size);

void awbl_usbh_serial_destroy (struct awbl_usbh_serial   *p_ser);

void awbl_usbh_serial_sio_chan_get (struct awbl_usbh_serial *p_ser,
                                    struct aw_sio_chan_info *p_info);

void awbl_usbh_serial_sio_chan_connect (struct awbl_usbh_serial *p_ser,
                                        struct aw_sio_chan_info *p_info);


/** \brief get serial COM number */
int awbl_usbh_serial_com_get (int pid, int vid, int inum, int port);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_USBH_SERIAL_H */
