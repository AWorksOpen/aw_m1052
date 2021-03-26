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
 * \brief lpc11xx jyrtc1 driver
 *
 * this driver is the implementation of RTC device for JYRTC-1
 *
 * \internal
 * \par modification history:
 * - 1.00 12-12-22  zen, first implementation
 * \endinternal
 */

#include "apollo.h"

#include "awbus_lite.h"
#include "awbl_i2cbus.h"

#include "awbl_rtc.h"
#include "driver/rtc/awbl_jyrtc1.h"

#include "time.h"

/******************************************************************************/
#define __JYRTC1_REG_SEC            0x00
#define __JYRTC1_REG_MIN            0x01
#define __JYRTC1_REG_HOUR           0x02
#define __JYRTC1_REG_WEEK           0x03
#define __JYRTC1_REG_DAY            0x04
#define __JYRTC1_REG_MONTH          0x05
#define __JYRTC1_REG_YEAR           0x06

#define __JYRTC1_REG_RAM            0x07

#define __JYRTC1_REG_MIN_AL         0x08
#define __JYRTC1_REG_HOUR_AL        0x09
#define __JYRTC1_REG_WEEK_DAY_AL    0x0A

#define __JYRTC1_REG_TIMCNT0        0x0B
#define __JYRTC1_REG_TIMCNT1        0x0C

#define __JYRTC1_REG_EXT            0x0D
#define __JYRTC1_REG_FLAG           0x0E
#define __JYRTC1_REG_CTRL           0x0F

/******************************************************************************/

/* declare jyrtc1 device instance */
#define __JYRTC1_DEV_DECL(p_this, p_dev) \
    struct awbl_jyrtc1_dev *p_this = \
        (struct awbl_jyrtc1_dev *)(p_dev)

/* declare jyrtc1 device infomation */
#define __JYRTC1_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_jyrtc1_devinfo *p_devinfo = \
        (struct awbl_jyrtc1_devinfo *)AWBL_DEVINFO_GET(p_dev)

#define __JYRTC1_DEV_LOCK_INIT(p_dev) \
    do { \
        AW_MUTEX_INIT(((struct awbl_jyrtc1_dev *)(p_dev))->devlock, \
                         AW_SEM_Q_PRIORITY ); \
    } while(0)

#define __JYRTC1_DEV_LOCK(p_dev) \
    do { \
        AW_MUTEX_LOCK(((struct awbl_jyrtc1_dev *)(p_dev))->devlock, \
                      AW_SEM_WAIT_FOREVER); \
    } while(0)

#define __JYRTC1_DEV_UNLOCK(p_dev) \
    do { \
        AW_MUTEX_UNLOCK(((struct awbl_jyrtc1_dev *)(p_dev))->devlock); \
    } while(0)

/*******************************************************************************
  forward declarations
*******************************************************************************/
aw_local void __jyrtc1_inst_init1(awbl_dev_t *p_dev);
aw_local void __jyrtc1_inst_init2(awbl_dev_t *p_dev);
aw_local void __jyrtc1_inst_connect(awbl_dev_t *p_dev);

aw_local aw_err_t __jyrtc1_time_get (void *p_cookie, aw_tm_t *p_time);
aw_local aw_err_t __jyrtc1_time_set (void *p_cookie, aw_tm_t *p_time);

aw_local aw_err_t __jyrtc1_rtcserv_get (struct awbl_dev *p_dev, void *p_arg);

/*******************************************************************************
  locals
*******************************************************************************/

/* driver functions (must defined as aw_const) */

aw_local aw_const struct awbl_drvfuncs __g_jyrtc1_drvfuncs = {
    __jyrtc1_inst_init1,
    __jyrtc1_inst_init2,
    __jyrtc1_inst_connect
};

/* jyrtc1 service functions (must defined as aw_const) */

aw_local aw_const struct awbl_rtc_servopts __g_jyrtc1_servopts = {
    __jyrtc1_time_get,                     /* time_get */
    __jyrtc1_time_set,                     /* time_set */
    NULL
};

/* driver methods (must defined as aw_const) */

AWBL_METHOD_IMPORT(awbl_rtcserv_get);

aw_local aw_const struct awbl_dev_method __g_jyrtc1_dev_methods[] = {
    AWBL_METHOD(awbl_rtcserv_get, __jyrtc1_rtcserv_get),
    AWBL_METHOD_END
};

/* driver registration (must defined as aw_const) */
aw_local aw_const awbl_i2c_drvinfo_t __g_jyrtc1_drv_registration = {
    {
        AWBL_VER_1,                    /* awb_ver */
        AWBL_BUSID_I2C,                /* bus_id */
        AWBL_JYRTC1_NAME,              /* p_drvname */
        &__g_jyrtc1_drvfuncs,          /* p_busfuncs */
        &__g_jyrtc1_dev_methods[0],    /* p_methods */
        NULL                           /* pfunc_drv_probe */
    }
};

/*******************************************************************************
  implementation
*******************************************************************************/

/******************************************************************************/
aw_local aw_err_t __jyrtc1_time_get (void *p_cookie, aw_tm_t *p_tm)
{
    uint8_t data[7];
    __JYRTC1_DEV_DECL(p_this, p_cookie);
    __JYRTC1_DEVINFO_DECL(p_devinfo, p_cookie);

    if (awbl_i2c_read((struct awbl_i2c_device *)p_this,
                      AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                      p_devinfo->addr,
                      __JYRTC1_REG_SEC,
                      data,
                      7) != AW_OK) {
        return -AW_EIO;
    }

    p_tm->tm_sec  = AW_BCD_TO_HEX(data[0] & ~0x80);
    p_tm->tm_min  = AW_BCD_TO_HEX(data[1] & ~0x80);
    p_tm->tm_hour = AW_BCD_TO_HEX(data[2] & ~0xC0);
    p_tm->tm_wday = (data[3] & 0x7F) - 1;
    p_tm->tm_mday = AW_BCD_TO_HEX(data[4] & ~0xC0);
    p_tm->tm_mon  = AW_BCD_TO_HEX(data[5] & ~0xE0) - 1;
    p_tm->tm_year = 100 + AW_BCD_TO_HEX(data[6]);

    return AW_OK;
}

/******************************************************************************/
aw_local aw_err_t __jyrtc1_time_set (void *p_cookie, aw_tm_t *p_tm)
{
    uint8_t data[7];
    __JYRTC1_DEV_DECL(p_this, p_cookie);
    __JYRTC1_DEVINFO_DECL(p_devinfo, p_cookie);

    data[0] = AW_HEX_TO_BCD(p_tm->tm_sec);
    data[1] = AW_HEX_TO_BCD(p_tm->tm_min);
    data[2] = AW_HEX_TO_BCD(p_tm->tm_hour);
    data[3] = p_tm->tm_wday + 1;
    data[4] = AW_HEX_TO_BCD(p_tm->tm_mday);
    data[5] = AW_HEX_TO_BCD(p_tm->tm_mon + 1);
    data[6] = AW_HEX_TO_BCD(p_tm->tm_year % 100);

    if (awbl_i2c_write((struct awbl_i2c_device *)p_this,
                       AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                       p_devinfo->addr,
                       __JYRTC1_REG_SEC,
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
aw_local void __jyrtc1_inst_init1(awbl_dev_t *p_dev)
{
}

/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __jyrtc1_inst_init2(awbl_dev_t *p_dev)
{
    __JYRTC1_DEV_LOCK_INIT(p_dev);
    return;
}

/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __jyrtc1_inst_connect(awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief method "awbl_jyrtc1serv_get" handler
 */
aw_local aw_err_t __jyrtc1_rtcserv_get (struct awbl_dev *p_dev, void *p_arg)
{
    __JYRTC1_DEV_DECL(p_this, p_dev);
    __JYRTC1_DEVINFO_DECL(p_devinfo, p_dev);

    struct awbl_rtc_service *p_serv = NULL;


    /* get intcltr service instance */

    p_serv = &p_this->rtc_serv;

    /* initialize the jyrtc1 service instance */

    p_serv->p_next      = NULL;
    p_serv->p_servinfo  = &p_devinfo->rtc_servinfo;
    p_serv->p_servopts  = &__g_jyrtc1_servopts;
    p_serv->p_cookie    = (void *)p_dev;

    /* send back the service instance */

    *(struct awbl_rtc_service **)p_arg = p_serv;

    return AW_OK;
}

/******************************************************************************/
void awbl_jyrtc1_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_jyrtc1_drv_registration);
}

/* end of file */

