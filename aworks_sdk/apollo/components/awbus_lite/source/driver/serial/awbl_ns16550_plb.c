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
 * \brief PLB NS16550 device support
 *
 * \internal
 * \par modification history:
 * - 1.00 12-11-07  orz, first implementation
 * \endinternal
 */

#include "aw_common.h"
#include "aw_spinlock.h"
#include <awbus_lite.h>

#include "driver/serial/awbl_ns16550.h"
#include "driver/serial/awbl_ns16550_plb.h"
#include "awbl_sio.h"

/**
 * \brief METHOD: instance to serail channel
 *
 * This routine returns the p_chan structure pointer from device instance.
 */
aw_local void __ns16550_plb_dev2chan (awbl_dev_t *p_dev, void *p_arg)
{
    *(struct awbl_ns16550_chan **)p_arg =
        &((struct awbl_ns16550_plb_chan *)p_dev)->nschan;
}

/* methods for PLB NS16550 driver */
aw_local aw_const struct awbl_dev_method __g_ns16550_plb_awbl_methods[] = {
    AWBL_METHOD(aw_sio_chan_get,        ns16550_sio_chan_get),
    AWBL_METHOD(aw_sio_chan_connect,    ns16550_sio_chan_connect),
    AWBL_METHOD(aw_sio_dev2chan,        __ns16550_plb_dev2chan),
    AWBL_METHOD_END
};

/**
 * supported PLB devices:
 * Many soldered-on ns16550 compatible devices
 *
 * driver registration (must defined as aw_const)
 */
aw_local aw_const struct awbl_drvinfo __g_ns16550_plb_drv_registration = {
    AWBL_VER_1,                     /* awb_ver */
    AWBL_BUSID_PLB,                 /* bus_id */
    "ns16550",                      /* p_drvname */
    &g_ns16550_awbl_drvfuncs,       /* p_busfuncs */
    __g_ns16550_plb_awbl_methods,   /* p_methods */
    ns16550_probe                   /* pfunc_drv_probe */
};

/**
 * \brief register ns16550 driver
 *
 * This routine registers the ns16550 driver and device recognition
 * data with the AWBus subsystem.
 *
 * NOTE: This routine is called early during system initialization, and
 * *MUST NOT* make calls to OS facilities such as memory allocation
 * and I/O.
 */
void awbl_ns16550_plb_drv_register (void)
{
    awbl_drv_register(&__g_ns16550_plb_drv_registration);
}

/* end of file */
