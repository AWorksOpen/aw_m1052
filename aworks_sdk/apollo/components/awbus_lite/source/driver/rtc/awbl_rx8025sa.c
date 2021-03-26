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
 * \brief lpc11xx rx8025sa driver
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
#include "driver/rtc/awbl_rx8025sa.h"

#include "time.h"

/******************************************************************************/

/**
 * \note hight 4bit: reg addr , low 4bit: standard mode
 */
#define __RX8025SA_REG_SEC            0x00
#define __RX8025SA_REG_MIN            0x10
#define __RX8025SA_REG_HOUR           0x20
#define __RX8025SA_REG_WEEK           0x30
#define __RX8025SA_REG_DAY            0x40
#define __RX8025SA_REG_MONTH          0x50
#define __RX8025SA_REG_YEAR           0x60

#define __RX8025SA_REG_DIGTAL_OFFSET  0x70

#define __RX8025SA_REG_MIN_AL         0x80
#define __RX8025SA_REG_HOUR_AL        0x90
#define __RX8025SA_REG_WEEK_DAY_AL    0xA0

#define __RX8025SA_REG_MIN_AL_D       0xB0
#define __RX8025SA_REG_HOUR_AL_D      0xC0

#define __RX8025SA_REG_RESERVED       0xD0
#define __RX8025SA_REG_CTRL1          0xE0
#define __RX8025SA_REG_CTRL2          0xF0

#define __RX8025SA_24H_FORMAT         0

/******************************************************************************/

/* declare rx8025sa device instance */
#define __RX8025SA_DEV_DECL(p_this, p_dev) \
    struct awbl_rx8025sa_dev *p_this = \
        (struct awbl_rx8025sa_dev *)(p_dev)

/* declare rx8025sa device infomation */
#define __RX8025SA_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_rx8025sa_devinfo *p_devinfo = \
        (struct awbl_rx8025sa_devinfo *)AWBL_DEVINFO_GET(p_dev)

#define __RX8025SA_DEV_LOCK_INIT(p_dev) \
    do { \
        AW_MUTEX_INIT(((struct awbl_rx8025sa_dev *)(p_dev))->devlock, \
                         AW_SEM_Q_PRIORITY ); \
    } while(0)

#define __RX8025SA_DEV_LOCK(p_dev) \
    do { \
        AW_MUTEX_LOCK(((struct awbl_rx8025sa_dev *)(p_dev))->devlock, \
                      AW_SEM_WAIT_FOREVER); \
    } while(0)

#define __RX8025SA_DEV_UNLOCK(p_dev) \
    do { \
        AW_MUTEX_UNLOCK(((struct awbl_rx8025sa_dev *)(p_dev))->devlock); \
    } while(0)

/*******************************************************************************
  forward declarations
*******************************************************************************/
aw_local void __rx8025sa_inst_init1(awbl_dev_t *p_dev);
aw_local void __rx8025sa_inst_init2(awbl_dev_t *p_dev);
aw_local void __rx8025sa_inst_connect(awbl_dev_t *p_dev);

aw_local aw_err_t __rx8025sa_time_get (void *p_cookie, aw_tm_t *p_time);
aw_local aw_err_t __rx8025sa_time_set (void *p_cookie, aw_tm_t *p_time);

aw_local aw_err_t __rx8025sa_rtcserv_get (struct awbl_dev *p_dev, void *p_arg);

/*******************************************************************************
  locals
*******************************************************************************/

/* driver functions (must defined as aw_const) */

aw_local aw_const struct awbl_drvfuncs __g_rx8025sa_drvfuncs = {
    __rx8025sa_inst_init1,
    __rx8025sa_inst_init2,
    __rx8025sa_inst_connect
};

/* rx8025sa service functions (must defined as aw_const) */

aw_local aw_const struct awbl_rtc_servopts __g_rx8025sa_servopts = {
    __rx8025sa_time_get,                     /* time_get */
    __rx8025sa_time_set,                     /* time_set */
    NULL
};

/* driver methods (must defined as aw_const) */

AWBL_METHOD_IMPORT(awbl_rtcserv_get);

aw_local aw_const struct awbl_dev_method __g_rx8025sa_dev_methods[] = {
    AWBL_METHOD(awbl_rtcserv_get, __rx8025sa_rtcserv_get),
    AWBL_METHOD_END
};

/* driver registration (must defined as aw_const) */
aw_local aw_const awbl_i2c_drvinfo_t __g_rx8025sa_drv_registration = {
    {
        AWBL_VER_1,                      /* awb_ver */
        AWBL_BUSID_I2C,                  /* bus_id */
        AWBL_RX8025SA_NAME,              /* p_drvname */
        &__g_rx8025sa_drvfuncs,          /* p_busfuncs */
        &__g_rx8025sa_dev_methods[0],    /* p_methods */
        NULL                             /* pfunc_drv_probe */
    }
};

/*******************************************************************************
  implementation
*******************************************************************************/

/******************************************************************************/
aw_local aw_err_t __rx8025sa_time_get (void *p_cookie, aw_tm_t *p_tm)
{
    uint8_t data[7];
    uint8_t temp;
    __RX8025SA_DEV_DECL(p_this, p_cookie);
    __RX8025SA_DEVINFO_DECL(p_devinfo, p_cookie);

    if (awbl_i2c_read((struct awbl_i2c_device *)p_this,
                      AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                      p_devinfo->addr,
                      __RX8025SA_REG_SEC,
                      data,
                      7) != AW_OK) {
        return -AW_EIO;
    }

    p_tm->tm_sec  = AW_BCD_TO_HEX(data[0] & ~0x80);
    p_tm->tm_min  = AW_BCD_TO_HEX(data[1] & ~0x80);
#if __RX8025SA_24H_FORMAT
    p_tm->tm_hour = AW_BCD_TO_HEX(data[2] & ~0xC0);
#else
    temp = AW_BCD_TO_HEX(data[2] & ~0xE0);
    if (temp == 12) {
        temp = 0;
    }
    p_tm->tm_hour = temp + ((data[2] >> 5) & 0x01) * 12;
#endif
    p_tm->tm_wday = AW_BCD_TO_HEX(data[3] & 0x7F);
    p_tm->tm_mday = AW_BCD_TO_HEX(data[4] & ~0xC0);
    p_tm->tm_mon  = AW_BCD_TO_HEX(data[5] & ~0xE0) - 1;
    p_tm->tm_year = AW_BCD_TO_HEX(data[6]) + AWBL_RX8025SA_YEAR_OFFS;

    return AW_OK;
}

/******************************************************************************/
aw_local aw_err_t __rx8025sa_time_set (void *p_cookie, aw_tm_t *p_tm)
{
    uint8_t data[7];
    uint8_t temp;
    __RX8025SA_DEV_DECL(p_this, p_cookie);
    __RX8025SA_DEVINFO_DECL(p_devinfo, p_cookie);

    data[0] = AW_HEX_TO_BCD(p_tm->tm_sec);
    data[1] = AW_HEX_TO_BCD(p_tm->tm_min);
#if __RX8025SA_24H_FORMAT
    data[2] = AW_HEX_TO_BCD(p_tm->tm_hour);
#else
    temp = AW_HEX_TO_BCD((p_tm->tm_hour % 12));
    if (temp == 0) {
        temp = 0X12;
    }
    data[2] = temp | ((p_tm->tm_hour / 12) << 5);
#endif
    data[3] = AW_HEX_TO_BCD(p_tm->tm_wday);
    data[4] = AW_HEX_TO_BCD(p_tm->tm_mday);
    data[5] = AW_HEX_TO_BCD(p_tm->tm_mon + 1);
    data[6] = AW_HEX_TO_BCD((p_tm->tm_year - AWBL_RX8025SA_YEAR_OFFS) % 100);

    if (awbl_i2c_write((struct awbl_i2c_device *)p_this,
                       AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                       p_devinfo->addr,
                       __RX8025SA_REG_SEC,
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
aw_local void __rx8025sa_inst_init1(awbl_dev_t *p_dev)
{
}

/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __rx8025sa_inst_init2(awbl_dev_t *p_dev)
{
    __RX8025SA_DEV_LOCK_INIT(p_dev);
    return;
}

/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __rx8025sa_inst_connect(awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief method "awbl_rx8025saserv_get" handler
 */
aw_local aw_err_t __rx8025sa_rtcserv_get (struct awbl_dev *p_dev, void *p_arg)
{
    __RX8025SA_DEV_DECL(p_this, p_dev);
    __RX8025SA_DEVINFO_DECL(p_devinfo, p_dev);

    struct awbl_rtc_service *p_serv = NULL;

#if __RX8025SA_24H_FORMAT
    /* set time format to 24hour */
    {
        uint8_t reg = 0;

        if (awbl_i2c_read((struct awbl_i2c_device *)p_this,
                           AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                           p_devinfo->addr,
                           __RX8025SA_REG_CTRL1,
                           &reg,
                           1) != AW_OK) {
            return -AW_EIO;
        }

        if (!(reg & 0x20)) {
            reg |= 0x20;
            if (awbl_i2c_write((struct awbl_i2c_device *)p_this,
                               AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                               p_devinfo->addr,
                               __RX8025SA_REG_CTRL1,
                               &reg,
                               1) != AW_OK) {
                return -AW_EIO;
            }
        }
    }
#endif

    /* get intcltr service instance */

    p_serv = &p_this->rtc_serv;

    /* initialize the rx8025sa service instance */

    p_serv->p_next      = NULL;
    p_serv->p_servinfo  = &p_devinfo->rtc_servinfo;
    p_serv->p_servopts  = &__g_rx8025sa_servopts;
    p_serv->p_cookie    = (void *)p_dev;

    /* send back the service instance */

    *(struct awbl_rtc_service **)p_arg = p_serv;

    return AW_OK;
}

/******************************************************************************/
void awbl_rx8025sa_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_rx8025sa_drv_registration);
}

/* end of file */

