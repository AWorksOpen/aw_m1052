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
 * \brief  rx8025t driver
 *
 * this driver is the implementation of RTC device for JYRTC-1
 *
 * \internal
 * \par modification history:
 * - 1.00 18-03-14  vir, first implementation
 * \endinternal
 */

#include "apollo.h"

#include "awbus_lite.h"
#include "awbl_i2cbus.h"

#include "awbl_rtc.h"
#include "driver/rtc/awbl_rx8025t.h"

#include "time.h"

/******************************************************************************/


#define __RX8025T_REG_SECONDS          0x00 /**< \brief Ãë¼Ä´æÆ÷µØÖ·   */
#define __RX8025T_REG_MINUTES          0x01 /**< \brief ·Ö¼Ä´æÆ÷µØÖ·   */
#define __RX8025T_REG_HOURS            0x02 /**< \brief Ê±¼Ä´æÆ÷µØÖ·   */
#define __RX8025T_REG_WEEKDAYS         0x03 /**< \brief ÐÇÆÚ¼Ä´æÆ÷µØÖ· */
#define __RX8025T_REG_DAYS             0x04 /**< \brief ÈÕ¼Ä´æÆ÷µØÖ·   */
#define __RX8025T_REG_MONTHS           0x05 /**< \brief ÔÂ¼Ä´æÆ÷µØÖ·   */
#define __RX8025T_REG_YEAERS           0x06 /**< \brief Äê¼Ä´æÆ÷µØÖ·   */

#define __RX8025T_REG_RAM              0x07 /**< \brief RAM¼Ä´æÆ÷µØÖ·   */

#define __RX8025T_REG_MINUTES_ALARM    0x08 /**< \brief ÄÖÖÓ·Ö¼Ä´æÆ÷µØÖ· */
#define __RX8025T_REG_HOURS_ALARM      0x09 /**< \brief ÄÖÖÓÊ±¼Ä´æÆ÷µØÖ· */

/** \brief ÄÖÖÓÐÇÆÚ»òÌì¼Ä´æÆ÷µØÖ· */
#define __RX8025T_REG_WEEK_DAY_ALARM   0x0a 

#define __RX8025T_REG_TIMER_COUNTER0   0x0b /**< \brief ¶¨Ê±¿ØÖÆ¼Ä´æÆ÷1µØÖ· */
#define __RX8025T_REG_TIMER_COUNTER1   0x0c /**< \brief ¶¨Ê±¿ØÖÆ¼Ä´æÆ÷2µØÖ· */

#define __RX8025T_REG_EXTENSION        0X0d /**< \brief À©Õ¹¼Ä´æÆ÷µØÖ· */
#define __RX8025T_REG_FLAG             0x0e /**< \brief ±êÖ¾¼Ä´æÆ÷µØÖ· */
#define __RX8025T_REG_CONTROL          0x0f /**< \brief ¿ØÖÆ¼Ä´æÆ÷µØÖ· */

/******************************************************************************/

/** \brief declare rx8025t device instance */
#define __RX8025T_DEV_DECL(p_this, p_dev) \
    struct awbl_rx8025t_dev *p_this = \
        (struct awbl_rx8025t_dev *)(p_dev)

/** \brief declare rx8025t device infomation */
#define __RX8025T_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_rx8025t_devinfo *p_devinfo = \
        (struct awbl_rx8025t_devinfo *)AWBL_DEVINFO_GET(p_dev)

#define __RX8025T_DEV_LOCK_INIT(p_dev) \
    do { \
        AW_MUTEX_INIT(((struct awbl_rx8025t_dev *)(p_dev))->devlock, \
                         AW_SEM_Q_PRIORITY ); \
    } while(0)

#define __RX8025T_DEV_LOCK(p_dev) \
    do { \
        AW_MUTEX_LOCK(((struct awbl_rx8025t_dev *)(p_dev))->devlock, \
                      AW_SEM_WAIT_FOREVER); \
    } while(0)

#define __RX8025T_DEV_UNLOCK(p_dev) \
    do { \
        AW_MUTEX_UNLOCK(((struct awbl_rx8025t_dev *)(p_dev))->devlock); \
    } while(0)

/*******************************************************************************
  forward declarations
*******************************************************************************/
aw_local void __rx8025t_inst_init1(awbl_dev_t *p_dev);
aw_local void __rx8025t_inst_init2(awbl_dev_t *p_dev);
aw_local void __rx8025t_inst_connect(awbl_dev_t *p_dev);

aw_local aw_err_t __rx8025t_time_get (void *p_cookie, aw_tm_t *p_time);
aw_local aw_err_t __rx8025t_time_set (void *p_cookie, aw_tm_t *p_time);

aw_local aw_err_t __rx8025t_rtcserv_get (struct awbl_dev *p_dev, void *p_arg);

/*******************************************************************************
  locals
*******************************************************************************/

/* driver functions (must defined as aw_const) */

aw_local aw_const struct awbl_drvfuncs __g_rx8025t_drvfuncs = {
    __rx8025t_inst_init1,
    __rx8025t_inst_init2,
    __rx8025t_inst_connect
};

/* rx8025t service functions (must defined as aw_const) */

aw_local aw_const struct awbl_rtc_servopts __g_rx8025t_servopts = {
    __rx8025t_time_get,                     /* time_get */
    __rx8025t_time_set,                     /* time_set */
    NULL
};

/* driver methods (must defined as aw_const) */

AWBL_METHOD_IMPORT(awbl_rtcserv_get);

aw_local aw_const struct awbl_dev_method __g_rx8025t_dev_methods[] = {
    AWBL_METHOD(awbl_rtcserv_get, __rx8025t_rtcserv_get),
    AWBL_METHOD_END
};

/* driver registration (must defined as aw_const) */
aw_local aw_const awbl_i2c_drvinfo_t __g_rx8025t_drv_registration = {
    {
        AWBL_VER_1,                     /* awb_ver */
        AWBL_BUSID_I2C,                 /* bus_id */
        AWBL_RX8025T_NAME,              /* p_drvname */
        &__g_rx8025t_drvfuncs,          /* p_busfuncs */
        &__g_rx8025t_dev_methods[0],    /* p_methods */
        NULL                            /* pfunc_drv_probe */
    }
};

/*******************************************************************************
  implementation
*******************************************************************************/

/******************************************************************************/
aw_local aw_err_t __rx8025t_time_get (void *p_cookie, aw_tm_t *p_tm)
{
    uint8_t data[7] = {0};
    uint8_t temp = 0;
    __RX8025T_DEV_DECL(p_this, p_cookie);
    __RX8025T_DEVINFO_DECL(p_devinfo, p_cookie);

    if (awbl_i2c_read((struct awbl_i2c_device *)p_this,
                      AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                      p_devinfo->addr,
                      __RX8025T_REG_SECONDS,
                      data,
                      7) != AW_OK) {
        return -AW_EIO;
    }

    p_tm->tm_sec  = AW_BCD_TO_HEX(data[0] & ~0x80);
    p_tm->tm_min  = AW_BCD_TO_HEX(data[1] & ~0x80);
    p_tm->tm_hour = AW_BCD_TO_HEX(data[2] & ~0xC0);

    while((data[3] >>= 1)) {
        temp++;
    }
    p_tm->tm_wday =temp ;

    p_tm->tm_mday = AW_BCD_TO_HEX(data[4] & ~0xC0);
    p_tm->tm_mon  = AW_BCD_TO_HEX(data[5] & ~0xE0) - 1;
    p_tm->tm_year = AW_BCD_TO_HEX(data[6]) + AWBL_RX8025T_YEAR_OFFS;

    return AW_OK;
}

/******************************************************************************/
aw_local aw_err_t __rx8025t_time_set (void *p_cookie, aw_tm_t *p_tm)
{
    uint8_t data[7] = {0};
    __RX8025T_DEV_DECL(p_this, p_cookie);
    __RX8025T_DEVINFO_DECL(p_devinfo, p_cookie);

    data[0] = AW_HEX_TO_BCD(p_tm->tm_sec);
    data[1] = AW_HEX_TO_BCD(p_tm->tm_min);
    data[2] = AW_HEX_TO_BCD(p_tm->tm_hour);
    data[3] = 1 << p_tm->tm_wday;
    data[4] = AW_HEX_TO_BCD(p_tm->tm_mday);
    data[5] = AW_HEX_TO_BCD(p_tm->tm_mon + 1);
    data[6] = AW_HEX_TO_BCD((p_tm->tm_year - AWBL_RX8025T_YEAR_OFFS) % 100);

    if (awbl_i2c_write((struct awbl_i2c_device *)p_this,
                       AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                       p_devinfo->addr,
                       __RX8025T_REG_SECONDS,
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
aw_local void __rx8025t_inst_init1(awbl_dev_t *p_dev)
{
}

/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __rx8025t_inst_init2(awbl_dev_t *p_dev)
{
    __RX8025T_DEV_LOCK_INIT(p_dev);
    return;
}

/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __rx8025t_inst_connect(awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief method "awbl_rx8025tserv_get" handler
 */
aw_local aw_err_t __rx8025t_rtcserv_get (struct awbl_dev *p_dev, void *p_arg)
{
    __RX8025T_DEV_DECL(p_this, p_dev);
    __RX8025T_DEVINFO_DECL(p_devinfo, p_dev);

    struct awbl_rtc_service *p_serv = NULL;

    /* get intcltr service instance */

    p_serv = &p_this->rtc_serv;

    /* initialize the rx8025t service instance */

    p_serv->p_next      = NULL;
    p_serv->p_servinfo  = &p_devinfo->rtc_servinfo;
    p_serv->p_servopts  = &__g_rx8025t_servopts;
    p_serv->p_cookie    = (void *)p_dev;

    /* send back the service instance */

    *(struct awbl_rtc_service **)p_arg = p_serv;

    return AW_OK;
}

/******************************************************************************/
void awbl_rx8025t_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_rx8025t_drv_registration);
}

/* end of file */

