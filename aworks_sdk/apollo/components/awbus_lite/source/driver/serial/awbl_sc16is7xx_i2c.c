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
 * \brief PHILIPS SC16IS752/SC16IS762 I2C-bus interface driver support
 *
 * SC16IS7xx series are NS16550 compatible Dual UART with I2C-bus/SPI interface
 *
 * \internal
 * \par modification history:
 * - 1.00 12-11-07  orz, first implementation
 * \endinternal
 */

#include "aw_common.h"
#include "aw_spinlock.h"
#include "awbus_lite.h"
#include "awbl_access.h"

#include "driver/serial/awbl_sc16is7xx_i2c.h"
#include "awbl_sio.h"
#include "aw_assert.h"

/** \brief convert a handle to sc16is7xx channel */
#define __HANDLE_TO_SC_CHAN(p_hdl)  ((struct awbl_sc16is7xx_i2c_chan *)p_hdl)

#define __SC_GET_I2C_ADDR(p_chan) \
    ((struct awbl_sc16is7xx_i2c_chan_param *)AWBL_DEVINFO_GET(p_chan))->i2c_addr

/** \brief set register address of sc16is7xx channel */
#define __SC_REG_ADDR(chan, reg)    (((reg) << 3u) | ((chan) << 1u))

/*******************************************************************************
    implementation
*******************************************************************************/

/** \brief SC16IS7xx I2C mode register read */
aw_local uint8_t __sc16is7xx_i2c_reg_read8 (void *p_hdl, void *p_reg)
{
    struct awbl_sc16is7xx_i2c_chan *p_chan;
    uint32_t                        reg;
    uint8_t                         val;

    p_chan = __HANDLE_TO_SC_CHAN(p_hdl);
    reg    = __SC_REG_ADDR(p_chan->chan_no, (unsigned int)p_reg);

#if 0
        aw_i2c_read(&p_chan->i2cdev, reg, &val, 1);
#else
        awbl_i2c_read((struct awbl_i2c_device *)p_chan,
                      AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                      __SC_GET_I2C_ADDR(p_chan),
                      reg,
                      &val,
                      1);
#endif

    return val;
}

/** \brief SC16IS7xx I2C mode register write */
aw_local void __sc16is7xx_i2c_reg_write8 (void *p_hdl, void *p_reg, uint8_t val)
{
    struct awbl_sc16is7xx_i2c_chan *p_chan;
    uint32_t                        reg;
    uint8_t                         val8;

    p_chan = __HANDLE_TO_SC_CHAN(p_hdl);
    reg    = __SC_REG_ADDR(p_chan->chan_no, (unsigned int)p_reg);
    val8   = (uint8_t)val;

#if 0
    aw_i2c_write(&p_chan->i2cdev, reg, &val8, 1);
#else
    awbl_i2c_write((struct awbl_i2c_device *)p_chan,
                   AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                   __SC_GET_I2C_ADDR(p_chan),
                   reg,
                   &val8,
                   1);
#endif
}

/** \brief access operate */
aw_local aw_const struct awbl_access_opt __g_sc16is7xx_i2c_access_opt = {
    __sc16is7xx_i2c_reg_read8,  /* pfunc_read8       */
    __sc16is7xx_i2c_reg_write8, /* pfunc_write8      */
                                /* pfunc_read8_rep   */
                                /* pfunc_write8_rep  */
                                /* pfunc_read16      */
                                /* pfunc_write16     */
                                /* pfunc_read16_rep  */
                                /* pfunc_write16_rep */
                                /* pfunc_read32      */
                                /* pfunc_write32     */
                                /* pfunc_read32_rep  */
                                /* pfunc_write32_rep */
};

/** \brief PLB总线设备寄存器映射 */
aw_local struct awbl_reg_access *__sc16is7xx_reg_map (struct awbl_dev *p_dev,
                                                      void            *p_reg)
{
    struct awbl_sc16is7xx_i2c_chan       *p_chan;
    struct awbl_sc16is7xx_i2c_chan_param *p_param;

    aw_assert(NULL != p_dev);
    aw_assert(0 == (unsigned)p_reg);

    p_chan  = (struct awbl_sc16is7xx_i2c_chan *)p_dev;
    p_param = (struct awbl_sc16is7xx_i2c_chan_param *)AWBL_DEVINFO_GET(p_dev);

    /* initialize the internal channel */
    p_chan->chan_no = p_param->chan_no;
#if 0
    aw_i2c_mkdev(&p_chan->i2cdev,
                 p_param->i2c_busid,
                 p_param->i2c_addr,
                 AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE);
#endif

    /* fill the access structure */
    p_chan->access.p_hdl = (void *)p_chan;
    p_chan->access.p_opt = &__g_sc16is7xx_i2c_access_opt;

    return (&(p_chan->access));
}

/**
 * \brief METHOD: instance to serail channel
 *
 * This routine returns the p_chan structure pointer from device instance.
 */
aw_local void __sc16is7xx_dev2chan (awbl_dev_t *p_dev, void *p_arg)
{
    *(struct awbl_ns16550_chan **)p_arg =
        &((struct awbl_sc16is7xx_i2c_chan *)p_dev)->nschan;
}

/* methods for this driver */
aw_local aw_const struct awbl_dev_method __g_sc16is7xx_i2c_awbl_methods[] = {
    AWBL_METHOD(aw_sio_chan_get,        ns16550_sio_chan_get),
    AWBL_METHOD(aw_sio_chan_connect,    ns16550_sio_chan_connect),
    AWBL_METHOD(awbl_dev_reg_map,       __sc16is7xx_reg_map),
    AWBL_METHOD(aw_sio_dev2chan,        __sc16is7xx_dev2chan),
    AWBL_METHOD_END
};

/**
 * supported devices:
 * SC16IS752/SC16IS762 compatible devices
 *
 * driver registration (must defined as aw_const)
 */
aw_local aw_const struct awbl_drvinfo __g_sc16is7xx_i2c_drv_registration = {
    AWBL_VER_1,                     /* awb_ver */
    AWBL_BUSID_I2C,                 /* bus_id */
    "sc16is7xx_i2c",                /* p_drvname */
    &g_ns16550_awbl_drvfuncs,       /* p_busfuncs */
    __g_sc16is7xx_i2c_awbl_methods, /* p_methods */
#if 1
    NULL                            /* pfunc_drv_probe */
#else
    ns16550_probe                   /* pfunc_drv_probe */
#endif
};

/**
 * \brief register sc16is7xx driver
 *
 * This routine registers the sc16is7xx driver and device recognition
 * data with the AWBus subsystem.
 *
 * \note This routine must be called after I2C driver initialization
 */
void awbl_sc16is7xx_i2c_drv_register (void)
{
    awbl_drv_register(&__g_sc16is7xx_i2c_drv_registration);
}

/* end of file */
