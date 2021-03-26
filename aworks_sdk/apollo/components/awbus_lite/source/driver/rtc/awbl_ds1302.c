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
 * \brief  ds1302 driver
 *
 * this driver is the implementation of RTC device for DS1302
 *
 * \internal
 * \par modification history:
 * - 1.00 18-03-07  vir, first implementation
 * \endinternal
 */

#include "apollo.h"

#include "awbus_lite.h"

#include "awbl_rtc.h"
#include "driver/rtc/awbl_ds1302.h"

#include "time.h"

/******************************************************************************/

#define __DS1302_REG_SECOND_WD               0X80
#define __DS1302_REG_SECOND_RD               0X81
#define __DS1302_REG_MIN_WD                  0X82
#define __DS1302_REG_MIN_RD                  0X83
#define __DS1302_REG_HR_WD                   0X84
#define __DS1302_REG_HR_RD                   0X85
#define __DS1302_REG_DATE_WD                 0X86
#define __DS1302_REG_DATE_RD                 0X87
#define __DS1302_REG_MONTH_WD                0X88
#define __DS1302_REG_MONTH_RD                0X89
#define __DS1302_REG_DAY_WD                  0X8A
#define __DS1302_REG_DAY_RD                  0X8B
#define __DS1302_REG_YEAR_WD                 0X8C
#define __DS1302_REG_YEAR_RD                 0X8D
#define __DS1302_REG_CONTROL_WD              0X8E
#define __DS1302_REG_CONTROL_RD              0X8F
#define __DS1302_REG_TRICKLE_WR              0X90
#define __DS1302_REG_TRICKLE_RD              0X91
#define __DS1302_REG_RAM_WD                  0XC0
#define __DS1302_REG_RAM_RD                  0XC1

#define __DS1302_REG_CLOCK_BURST_WD          0XBE
#define __DS1302_REG_CLOCK_BURST_RD          0XBF


#define __DS1302_YEAR_OFFS     100

/******************************************************************************/

/* declare ds1302 device instance */
#define __DS1302_DEV_DECL(p_this, p_dev) \
    struct awbl_ds1302_dev *p_this = \
        (struct awbl_ds1302_dev *)(p_dev)

/* declare ds1302 device infomation */
#define __DS1302_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_ds1302_devinfo *p_devinfo = \
        (struct awbl_ds1302_devinfo *)AWBL_DEVINFO_GET(p_dev)

#define __DS1302_DEV_LOCK_INIT(p_dev) \
    do { \
        AW_MUTEX_INIT(((struct awbl_ds1302_dev *)(p_dev))->devlock, \
                         AW_SEM_Q_PRIORITY ); \
    } while(0)

#define __DS1302_DEV_LOCK(p_dev) \
    do { \
        AW_MUTEX_LOCK(((struct awbl_ds1302_dev *)(p_dev))->devlock, \
                      AW_SEM_WAIT_FOREVER); \
    } while(0)

#define __DS1302_DEV_UNLOCK(p_dev) \
    do { \
        AW_MUTEX_UNLOCK(((struct awbl_ds1302_dev *)(p_dev))->devlock); \
    } while(0)

/*******************************************************************************
  forward declarations
*******************************************************************************/
aw_local void __ds1302_inst_init1(awbl_dev_t *p_dev);
aw_local void __ds1302_inst_init2(awbl_dev_t *p_dev);
aw_local void __ds1302_inst_connect(awbl_dev_t *p_dev);

aw_local aw_err_t __ds1302_time_get (void *p_cookie, aw_tm_t *p_time);
aw_local aw_err_t __ds1302_time_set (void *p_cookie, aw_tm_t *p_time);

aw_local aw_err_t __ds1302_rtcserv_get (struct awbl_dev *p_dev, void *p_arg);

/*******************************************************************************
  locals
*******************************************************************************/

/* driver functions (must defined as aw_const) */

aw_local aw_const struct awbl_drvfuncs __g_ds1302_drvfuncs = {
    __ds1302_inst_init1,
    __ds1302_inst_init2,
    __ds1302_inst_connect
};

/* ds1302 service functions (must defined as aw_const) */

aw_local aw_const struct awbl_rtc_servopts __g_ds1302_servopts = {
    __ds1302_time_get,                     /* time_get */
    __ds1302_time_set,                     /* time_set */
    NULL
};

/* driver methods (must defined as aw_const) */

AWBL_METHOD_IMPORT(awbl_rtcserv_get);

aw_local aw_const struct awbl_dev_method __g_ds1302_dev_methods[] = {
    AWBL_METHOD(awbl_rtcserv_get, __ds1302_rtcserv_get),
    AWBL_METHOD_END
};

/* driver registration (must defined as aw_const) */
aw_local aw_const awbl_spi_drvinfo_t __g_ds1302_drv_registration = {
    {
        AWBL_VER_1,                    /* awb_ver */
        AWBL_BUSID_SPI,                /* bus_id */
        AWBL_DS1302_NAME,              /* p_drvname */
        &__g_ds1302_drvfuncs,          /* p_busfuncs */
        &__g_ds1302_dev_methods[0],    /* p_methods */
        NULL                           /* pfunc_drv_probe */
    }
};

/*******************************************************************************
  implementation
*******************************************************************************/

/******************************************************************************/

aw_local aw_err_t __ds1302_spi_write(struct awbl_spi_device *p_dev,
                                     uint8_t                 addr,
                                     uint8_t                *p_buf,
                                     uint8_t                 nbytes)
{
    return awbl_spi_write_then_write(p_dev, &addr, 1,p_buf, nbytes);

}

aw_local aw_err_t  __ds1302_spi_read(struct awbl_spi_device *p_dev,
                                     uint8_t                 addr,
                                     uint8_t                *p_buf,
                                     uint8_t                 nbytes)
{
    return awbl_spi_write_then_read(p_dev, &addr,1, p_buf, nbytes);
}


aw_local aw_err_t __ds1302_time_get (void *p_cookie, aw_tm_t *p_tm)
{
    uint8_t buf[8] = {0};
    int     ret    = AW_OK;
    uint8_t temp   = 0;
    __DS1302_DEV_DECL(p_this, p_cookie);
    __DS1302_DEVINFO_DECL(p_devinfo, p_cookie);

    struct awbl_spi_device *p_drv = &(p_this->super);

    if ( __ds1302_spi_read( &(p_this->super), \
                __DS1302_REG_CLOCK_BURST_RD, buf, 8) != AW_OK) {
        ret = AW_EIO;
    }

    p_tm->tm_sec  = AW_BCD_TO_HEX(buf[0] & ~0X80);
    p_tm->tm_min  = AW_BCD_TO_HEX(buf[1] & ~0X80);
    p_tm->tm_hour = AW_BCD_TO_HEX(buf[2] & ~0XC0);
    p_tm->tm_mday = AW_BCD_TO_HEX(buf[3] & ~0XC0);
    p_tm->tm_mon  = AW_BCD_TO_HEX(buf[4] & 0X1F) - 1;
    p_tm->tm_wday = AW_BCD_TO_HEX(buf[5] & 0X07) - 1;
    p_tm->tm_year = AW_BCD_TO_HEX(buf[6]) + __DS1302_YEAR_OFFS;

    return ret;
}

/******************************************************************************/
aw_local aw_err_t __ds1302_time_set (void *p_cookie, aw_tm_t *p_tm)
{
    uint8_t buf[8] = {0};
    uint8_t data   = 0;
    int     ret    = AW_OK;

    __DS1302_DEV_DECL(p_this, p_cookie);
    __DS1302_DEVINFO_DECL(p_devinfo, p_cookie);

    struct awbl_spi_device *p_drv = &(p_this->super);

    if (__ds1302_spi_write( &(p_this->super), \
                      __DS1302_REG_CONTROL_WD, &data, 1) != AW_OK ) {
        return -AW_EIO;
    }

    buf[0] = AW_HEX_TO_BCD(p_tm->tm_sec);
    buf[1] = AW_HEX_TO_BCD(p_tm->tm_min);
    buf[2] = AW_HEX_TO_BCD(p_tm->tm_hour);
    buf[3] = AW_HEX_TO_BCD(p_tm->tm_mday);
    buf[4] = AW_HEX_TO_BCD(p_tm->tm_mon + 1);
    buf[5] = AW_HEX_TO_BCD(p_tm->tm_wday + 1);
    buf[6] = AW_HEX_TO_BCD((p_tm->tm_year - __DS1302_YEAR_OFFS) % 100);
    buf[7] = 0;

    if ( __ds1302_spi_write( &(p_this->super), \
                   __DS1302_REG_CLOCK_BURST_WD, buf, 8)) {
        ret = -AW_EIO;
    }

    data = 0X80;
    __ds1302_spi_write( &(p_this->super), __DS1302_REG_CONTROL_WD, &data, 1);

    return ret;
}

/**
 * \brief first level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __ds1302_inst_init1(awbl_dev_t *p_dev)
{
}

/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __ds1302_inst_init2(awbl_dev_t *p_dev)
{
    __DS1302_DEV_DECL(p_this, p_dev);
    __DS1302_DEVINFO_DECL(p_devinfo, p_dev);

    /* SPI device */
    aw_spi_mkdev(&(p_this->super.spi_dev),
                 p_this->super.super.p_devhcf->bus_index,
                 8,
                 AW_SPI_MODE_0 | AW_SPI_CS_HIGH | AW_SPI_3WIRE |AW_SPI_LSB_FIRST,
                 p_devinfo->spi_speed,
                 p_devinfo->spi_cs_pin,
                 NULL);

    aw_spi_setup(&(p_this->super.spi_dev));

    __DS1302_DEV_LOCK_INIT(p_dev);

    return;
}

/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __ds1302_inst_connect(awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief method "awbl_ds1302serv_get" handler
 */
aw_local aw_err_t __ds1302_rtcserv_get (struct awbl_dev *p_dev, void *p_arg)
{
    uint8_t data = 0;
    __DS1302_DEV_DECL(p_this, p_dev);
    __DS1302_DEVINFO_DECL(p_devinfo, p_dev);

    struct awbl_rtc_service *p_serv = NULL;


    data = 0;
    __ds1302_spi_write( &(p_this->super), __DS1302_REG_CONTROL_WD, &data, 1);
    __ds1302_spi_write( &(p_this->super), __DS1302_REG_SECOND_WD, &data, 1);
    __ds1302_spi_write( &(p_this->super), __DS1302_REG_HR_WD, &data, 1);

    data = 0X80;
    __ds1302_spi_write( &(p_this->super), __DS1302_REG_CONTROL_WD, &data, 1);

    /* get intcltr service instance */
    p_serv = &p_this->rtc_serv;

    /* initialize the ds1302 service instance */
    p_serv->p_next      = NULL;
    p_serv->p_servinfo  = &p_devinfo->rtc_servinfo;
    p_serv->p_servopts  = &__g_ds1302_servopts;
    p_serv->p_cookie    = (void *)p_dev;

    /* send back the service instance */
    *(struct awbl_rtc_service **)p_arg = p_serv;

    return AW_OK;
}

/******************************************************************************/
void awbl_ds1302_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_ds1302_drv_registration);
}

/* end of file */

