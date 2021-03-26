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
 * \brief lpc11xx pcf8563 driver
 *
 * this driver is the implementation of RTC device for PCF8563
 *
 * \internal
 * \par modification history:
 * - 1.00 12-09-05  zen, first implementation
 * \endinternal
 */

#include "apollo.h"

#include "awbus_lite.h"
#include "awbl_i2cbus.h"

#include "awbl_rtc.h"
#include "driver/rtc/awbl_pcf8563.h"

#include "time.h"

/******************************************************************************/
#define __PCF8563_REG_CS1       0x00
#define __PCF8563_REG_CS2       0x01

#define __PCF8563_REG_SEC       0x02
#define __PCF8563_REG_MIN       0x03
#define __PCF8563_REG_HOUR      0x04
#define __PCF8563_REG_DAY       0x05
#define __PCF8563_REG_WEEK      0x06
#define __PCF8563_REG_MONTH     0x07
#define __PCF8563_REG_YEAR      0x08

#define __PCF8563_REG_MIN_AL    0x09
#define __PCF8563_REG_HOUR_AL   0x0A
#define __PCF8563_REG_DAY_AL    0x0B
#define __PCF8563_REG_WEEK_AL   0x0C

#define __PCF8563_REG_CLKOUT    0x0D
#define __PCF8563_REG_TC        0x0E
#define __PCF8563_REG_TV        0x0F

/******************************************************************************/

/* declare pcf8563 device instance */
#define __PCF8563_DEV_DECL(p_this, p_dev) \
    struct awbl_pcf8563_dev *p_this = \
        (struct awbl_pcf8563_dev *)(p_dev)

/* declare pcf8563 device infomation */
#define __PCF8563_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_pcf8563_devinfo *p_devinfo = \
        (struct awbl_pcf8563_devinfo *)AWBL_DEVINFO_GET(p_dev)

#define __PCF8563_DEV_LOCK_INIT(p_dev) \
    do { \
        AW_MUTEX_INIT(((struct awbl_pcf8563_dev *)(p_dev))->devlock, \
                         AW_SEM_Q_PRIORITY ); \
    } while(0)

#define __PCF8563_DEV_LOCK(p_dev) \
    do { \
        AW_MUTEX_LOCK(((struct awbl_pcf8563_dev *)(p_dev))->devlock, \
                      AW_SEM_WAIT_FOREVER); \
    } while(0)

#define __PCF8563_DEV_UNLOCK(p_dev) \
    do { \
        AW_MUTEX_UNLOCK(((struct awbl_pcf8563_dev *)(p_dev))->devlock); \
    } while(0)

/*******************************************************************************
  forward declarations
*******************************************************************************/
aw_local void __pcf8563_inst_init1(awbl_dev_t *p_dev);
aw_local void __pcf8563_inst_init2(awbl_dev_t *p_dev);
aw_local void __pcf8563_inst_connect(awbl_dev_t *p_dev);

aw_local aw_err_t __pcf8563_time_get (void *p_cookie, aw_tm_t *p_time);
aw_local aw_err_t __pcf8563_time_set (void *p_cookie, aw_tm_t *p_time);

aw_local aw_err_t __pcf8563_rtcserv_get (struct awbl_dev *p_dev, void *p_arg);

/*******************************************************************************
  locals
*******************************************************************************/

/* driver functions (must defined as aw_const) */

aw_local aw_const struct awbl_drvfuncs __g_pcf8563_drvfuncs = {
    __pcf8563_inst_init1,
    __pcf8563_inst_init2,
    __pcf8563_inst_connect
};

/* pcf8563 service functions (must defined as aw_const) */

aw_local aw_const struct awbl_rtc_servopts __g_pcf8563_servopts = {
    __pcf8563_time_get,                     /* time_get */
    __pcf8563_time_set,                     /* time_set */
    NULL
};

/* driver methods (must defined as aw_const) */

AWBL_METHOD_IMPORT(awbl_rtcserv_get);

aw_local aw_const struct awbl_dev_method __g_pcf8563_dev_methods[] = {
    AWBL_METHOD(awbl_rtcserv_get, __pcf8563_rtcserv_get),
    AWBL_METHOD_END
};

/* driver registration (must defined as aw_const) */
aw_local aw_const awbl_i2c_drvinfo_t __g_pcf8563_drv_registration = {
    {
        AWBL_VER_1,                     /* awb_ver */
        AWBL_BUSID_I2C,                 /* bus_id */
        AWBL_PCF8563_NAME,              /* p_drvname */
        &__g_pcf8563_drvfuncs,          /* p_busfuncs */
        &__g_pcf8563_dev_methods[0],    /* p_methods */
        NULL                            /* pfunc_drv_probe */
    }
};

/*******************************************************************************
  implementation
*******************************************************************************/

/******************************************************************************/
aw_local aw_err_t __pcf8563_time_get (void *p_cookie, aw_tm_t *p_tm)
{
    uint8_t data[7];
    __PCF8563_DEV_DECL(p_this, p_cookie);
    __PCF8563_DEVINFO_DECL(p_devinfo, p_cookie);

    if (awbl_i2c_read((struct awbl_i2c_device *)p_this,
                      AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                      p_devinfo->addr,
                      __PCF8563_REG_SEC,
                      data,
                      7) != AW_OK) {
        return -AW_EIO;
    }

    p_tm->tm_sec  = AW_BCD_TO_HEX(data[0] & ~0x80);
    p_tm->tm_min  = AW_BCD_TO_HEX(data[1] & ~0x80);
    p_tm->tm_hour = AW_BCD_TO_HEX(data[2] & ~0xC0);
    p_tm->tm_mday = AW_BCD_TO_HEX(data[3] & ~0xC0);
    p_tm->tm_mon  = AW_BCD_TO_HEX(data[5] & ~0xE0) - 1;
    p_tm->tm_year = ((data[5] & 0x80) ? 0 : 100)
                    + AW_BCD_TO_HEX(data[6]);

    return AW_OK;
}

/******************************************************************************/
aw_local aw_err_t __pcf8563_time_set (void *p_cookie, aw_tm_t *p_tm)
{
    uint8_t data[7];
    __PCF8563_DEV_DECL(p_this, p_cookie);
    __PCF8563_DEVINFO_DECL(p_devinfo, p_cookie);

    data[0] = AW_HEX_TO_BCD(p_tm->tm_sec);
    data[1] = AW_HEX_TO_BCD(p_tm->tm_min);
    data[2] = AW_HEX_TO_BCD(p_tm->tm_hour);
    data[3] = AW_HEX_TO_BCD(p_tm->tm_mday);
    data[4] = AW_HEX_TO_BCD(p_tm->tm_wday);
    data[5] = AW_HEX_TO_BCD(p_tm->tm_mon + 1) |
              (p_tm->tm_year < 100 ? 0x80 : 0x00);

    data[6] = AW_HEX_TO_BCD(p_tm->tm_year % 100);

    if (awbl_i2c_write((struct awbl_i2c_device *)p_this,
                       AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                       p_devinfo->addr,
                       __PCF8563_REG_SEC,
                       data,
                       7) != AW_OK) {
        return -AW_EIO;
    }

    return AW_OK;
}

/**
 * \brief first level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __pcf8563_inst_init1(awbl_dev_t *p_dev)
{
}

/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __pcf8563_inst_init2(awbl_dev_t *p_dev)
{
    __PCF8563_DEV_LOCK_INIT(p_dev);
    return;
}

/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __pcf8563_inst_connect(awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief method "awbl_pcf8563serv_get" handler
 */
aw_local aw_err_t __pcf8563_rtcserv_get (struct awbl_dev *p_dev, void *p_arg)
{
    __PCF8563_DEV_DECL(p_this, p_dev);
    __PCF8563_DEVINFO_DECL(p_devinfo, p_dev);

    struct awbl_rtc_service *p_serv = NULL;


    /* get intcltr service instance */

    p_serv = &p_this->rtc_serv;

    /* initialize the pcf8563 service instance */

    p_serv->p_next      = NULL;
    p_serv->p_servinfo  = &p_devinfo->rtc_servinfo;
    p_serv->p_servopts  = &__g_pcf8563_servopts;
    p_serv->p_cookie    = (void *)p_dev;

    /* send back the service instance */

    *(struct awbl_rtc_service **)p_arg = p_serv;

    return AW_OK;
}

/******************************************************************************/
void awbl_pcf8563_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_pcf8563_drv_registration);
}

/* end of file */

