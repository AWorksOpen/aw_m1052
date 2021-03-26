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
 * \brief AWBus-lite RTC PCF85263 驱动
 *        just for 1970~2070
 *
 * \internal
 * \par modification history:
 * - 1.00 18-08-13  mex, first implementation
 * \endinternal
 */

#include "driver/rtc/awbl_pcf85263.h"
#include "time.h"

/******************************************************************************/

/* 寄存器定义 */
#define __PCF85263_100TH_SEC         0x00
#define __PCF85263_REG_SEC           0x01
#define __PCF85263_REG_MIN           0x02
#define __PCF85263_REG_HOUR          0x03
#define __PCF85263_REG_DAY           0x04
#define __PCF85263_REG_WEEK          0x05
#define __PCF85263_REG_MONTH         0x06
#define __PCF85263_REG_YEAR          0x07

#define __PCF85263_REG_OSC           0x25
#define __PCF85263_REG_FUNC          0x28
#define __PCF85263_REG_STOP          0x2E
#define __PCF85263_REG_RESET         0x2F

/* 寄存器位定义 */
#define __PCF85263_REG_FUNC_100TH  (0x01 << 7) /**< \brief 100th seconds mode */
#define __PCF85263_REG_FUNC_RTCM   (0x01 << 4) /**< \brief RTC mode */
#define __PCF85263_CPR             (0xA4)      /**< \brief clear prescaler */

/******************************************************************************/

/** \brief declare pcf85263 device instance */
#define __PCF85263_DEV_DECL(p_this, p_dev) \
    struct awbl_pcf85263_dev *p_this = \
        (struct awbl_pcf85263_dev *)(p_dev)

/** \brief declare pcf85263 device infomation */
#define __PCF85263_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_pcf85263_devinfo *p_devinfo = \
        (struct awbl_pcf85263_devinfo *)AWBL_DEVINFO_GET(p_dev)

#define __PCF85263_DEV_LOCK_INIT(p_dev) \
    do { \
        AW_MUTEX_INIT(((struct awbl_pcf85263_dev *)(p_dev))->devlock, \
                         AW_SEM_Q_PRIORITY ); \
    } while(0)

#define __PCF85263_DEV_LOCK(p_dev) \
    do { \
        AW_MUTEX_LOCK(((struct awbl_pcf85263_dev *)(p_dev))->devlock, \
                      AW_SEM_WAIT_FOREVER); \
    } while(0)

#define __PCF85263_DEV_UNLOCK(p_dev) \
    do { \
        AW_MUTEX_UNLOCK(((struct awbl_pcf85263_dev *)(p_dev))->devlock); \
    } while(0)

/*******************************************************************************
  forward declarations
*******************************************************************************/
aw_local void __pcf85263_inst_init1(awbl_dev_t *p_dev);
aw_local void __pcf85263_inst_init2(awbl_dev_t *p_dev);
aw_local void __pcf85263_inst_connect(awbl_dev_t *p_dev);

aw_local aw_err_t __pcf85263_time_get (void *p_cookie, aw_tm_t *p_tm);
aw_local aw_err_t __pcf85263_time_set (void *p_cookie, aw_tm_t *p_tm);

aw_local aw_err_t __pcf85263_timespec_get (void *p_cookie, aw_timespec_t *p_tv);
aw_local aw_err_t __pcf85263_timespec_set (void *p_cookie, aw_timespec_t *p_tv);

aw_local aw_err_t __pcf85263_rtcserv_get (struct awbl_dev *p_dev, void *p_arg);


/*******************************************************************************
  locals
*******************************************************************************/

/* driver functions (must defined as aw_const) */

aw_local aw_const struct awbl_drvfuncs __g_pcf85263_drvfuncs = {
    __pcf85263_inst_init1,
    __pcf85263_inst_init2,
    __pcf85263_inst_connect
};

/* pcf85263 service functions (must defined as aw_const) */
aw_local aw_const struct awbl_rtc_servopts __g_pcf85263_servopts = {
    __pcf85263_time_get,      /* time_get */
    __pcf85263_time_set,      /* time_set */
    NULL,
    __pcf85263_timespec_get,  /* timespec_get */
    __pcf85263_timespec_set   /* timespec_set */
};


/* driver methods (must defined as aw_const) */

AWBL_METHOD_IMPORT(awbl_rtcserv_get);

aw_local aw_const struct awbl_dev_method __g_pcf85263_dev_methods[] = {
    AWBL_METHOD(awbl_rtcserv_get, __pcf85263_rtcserv_get),
    AWBL_METHOD_END
};

/* driver registration (must defined as aw_const) */
aw_local aw_const awbl_i2c_drvinfo_t __g_pcf85263_drv_registration = {
    {
        AWBL_VER_1,                     /* awb_ver */
        AWBL_BUSID_I2C,                 /* bus_id */
        AWBL_PCF85263_NAME,             /* p_drvname */
        &__g_pcf85263_drvfuncs,         /* p_busfuncs */
        &__g_pcf85263_dev_methods[0],   /* p_methods */
        NULL                            /* pfunc_drv_probe */
    }
};

/*******************************************************************************
  implementation
*******************************************************************************/


/* 获取时间 */
aw_local aw_err_t __time_get (struct awbl_pcf85263_dev *p_dev,
                              aw_tm_t                  *p_tm,
                              uint16_t                 *p_msec)
{
    __PCF85263_DEVINFO_DECL(p_devinfo, p_dev);

    aw_err_t   ret = AW_OK;
    uint8_t data[7] = {0};

    if (NULL == p_tm || p_msec == NULL) {
        return -AW_EINVAL;
    }

    __PCF85263_DEV_LOCK(p_dev);

    if (AW_OK != awbl_i2c_read((struct awbl_i2c_device *)p_dev,
                               AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                               p_devinfo->addr,
                               __PCF85263_100TH_SEC,
                               data,
                               8)) {
        ret =  -AW_EIO;
        goto clean_up;
    }

    *p_msec = AW_BCD_TO_HEX(data[0]) * 10;

    p_tm->tm_sec  = AW_BCD_TO_HEX(data[1] & ~0x80);
    p_tm->tm_min  = AW_BCD_TO_HEX(data[2] & ~0x80);
    p_tm->tm_hour = AW_BCD_TO_HEX(data[3] & ~0xC0);
    p_tm->tm_mday = AW_BCD_TO_HEX(data[4] & ~0xC0);
    p_tm->tm_wday = AW_BCD_TO_HEX(data[5] & ~0xF8);
    p_tm->tm_mon  = AW_BCD_TO_HEX(data[6] & ~0xE0) - 1;
    p_tm->tm_year = AW_BCD_TO_HEX(data[7]);

    /* time revise */
    if (p_tm->tm_year < 70) {
        p_tm->tm_year += 100;
    }

clean_up:
    __PCF85263_DEV_UNLOCK(p_dev);

    return ret;
}


/* 设置时间 */
aw_local aw_err_t __time_set (struct awbl_pcf85263_dev *p_dev,
                              aw_tm_t                  *p_tm,
                              uint16_t                  msec)
{

    __PCF85263_DEVINFO_DECL(p_devinfo, p_dev);

    aw_err_t ret = AW_OK;
    uint8_t data[7];

    /* time overflow(valid 1970~2070) */
    if ((p_tm->tm_year < 70) || (p_tm->tm_year > 170) || (msec > 999)) {
        return -AW_ETIME;
    }


    __PCF85263_DEV_LOCK(p_dev);

    /* 先停止时钟  */
    data[0] = 0x1;
    if (awbl_i2c_write((struct awbl_i2c_device *)p_dev,
                         AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                         p_devinfo->addr,
                         __PCF85263_REG_STOP,
                         data,
                         1) != AW_OK) {
        ret =  -AW_EIO;
        goto clean_up;
     }

    /* clear prescaler */
    data[0] = __PCF85263_CPR;
    if (awbl_i2c_write((struct awbl_i2c_device *)p_dev,
                        AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                        p_devinfo->addr,
                        __PCF85263_REG_RESET,
                        data,
                        1) != AW_OK) {
        ret =  -AW_EIO;
        goto clean_up;
    }

    /* 设置时间 */

    data[0] = AW_HEX_TO_BCD(msec / 10);
    data[1] = AW_HEX_TO_BCD(p_tm->tm_sec);
    data[2] = AW_HEX_TO_BCD(p_tm->tm_min);
    data[3] = AW_HEX_TO_BCD(p_tm->tm_hour);
    data[4] = AW_HEX_TO_BCD(p_tm->tm_mday);
    data[5] = AW_HEX_TO_BCD(p_tm->tm_wday);
    data[6] = AW_HEX_TO_BCD(p_tm->tm_mon + 1);
    data[7] = AW_HEX_TO_BCD(p_tm->tm_year % 100);
    if (awbl_i2c_write((struct awbl_i2c_device *)p_dev,
                       AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                       p_devinfo->addr,
                       __PCF85263_100TH_SEC,
                       data,
                       8) != AW_OK) {
        ret =  -AW_EIO;
        goto clean_up;
    }

    /* 启动 */
    data[0] = 0x00;
    if (awbl_i2c_write((struct awbl_i2c_device *)p_dev,
                         AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                         p_devinfo->addr,
                         __PCF85263_REG_STOP,
                         data,
                         1) != AW_OK) {
        ret =  -AW_EIO;
        goto clean_up;
    }

clean_up:
    __PCF85263_DEV_UNLOCK(p_dev);

    return ret;
}

aw_local aw_err_t __pcf85263_timespec_get (void *p_cookie, aw_timespec_t *p_tv)
{
    __PCF85263_DEV_DECL(p_this, p_cookie);

    aw_tm_t  tm   = {0};
    uint16_t msec = 0;

    if (__time_get(p_this, &tm, &msec) != AW_OK) {
        return AW_ERROR;
    }

    /* 将细分时间转换为日历时间 */
    if (aw_tm_to_time(&tm, &p_tv->tv_sec) != AW_OK) {
        return AW_ERROR;
    }

    p_tv->tv_nsec = msec * 1000000;

    return AW_OK;
}

/******************************************************************************/
aw_local aw_err_t __pcf85263_timespec_set (void *p_cookie, aw_timespec_t *p_tv)
{
    __PCF85263_DEV_DECL(p_this, p_cookie);

    aw_tm_t  tm   = {0};
    uint16_t msec = 0;

    msec = p_tv->tv_nsec / 1000000;

    /* 将日历时间转换为细分时间 */
    if (aw_time_to_tm(&p_tv->tv_sec, &tm) != AW_OK) {
        return AW_ERROR;
    }

    if (__time_set(p_this, &tm, msec) != AW_OK) {
        return AW_ERROR;
    }

    return AW_OK;
}


/******************************************************************************/
aw_local aw_err_t __pcf85263_time_get (void *p_cookie, aw_tm_t *p_tm)
{
    __PCF85263_DEV_DECL(p_this, p_cookie);
    uint16_t msec = 0;

    if (__time_get(p_this, p_tm, &msec) != AW_OK) {
        return AW_ERROR;
    }

    return AW_OK;
}

/******************************************************************************/
aw_local aw_err_t __pcf85263_time_set (void *p_cookie, aw_tm_t *p_tm)
{
    __PCF85263_DEV_DECL(p_this, p_cookie);
    uint16_t msec = 0;

    if (__time_set(p_this, p_tm, msec) != AW_OK) {
        return AW_ERROR;
    }

    return AW_OK;
}


/******************************************************************************/

/**
 * \brief first level initialization routine
 *
 * \param[in] p_dev : device instance
 */
aw_local void __pcf85263_inst_init1(awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev : device instance
 */
aw_local void __pcf85263_inst_init2(awbl_dev_t *p_dev)
{ 
    __PCF85263_DEV_LOCK_INIT(p_dev);
    return;
}

/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev : device instance
 */
aw_local void __pcf85263_inst_connect(awbl_dev_t *p_dev)
{
    __PCF85263_DEV_DECL(p_this, p_dev);
    __PCF85263_DEVINFO_DECL(p_devinfo, p_this);
    
    uint8_t reg_val = 0;
    if (AW_OK != awbl_i2c_read((struct awbl_i2c_device *)p_this,
                               AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                               p_devinfo->addr,
                               __PCF85263_REG_FUNC,
                               &reg_val,
                               1)) {
        return ;
    }
    
    /* 使能100th second mode */
    reg_val |= __PCF85263_REG_FUNC_100TH;

    /* 并使用实时时钟模式 */
    reg_val &=  ~__PCF85263_REG_FUNC_RTCM;

    awbl_i2c_write((struct awbl_i2c_device *)p_this,
                   AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                   p_devinfo->addr,
                   __PCF85263_REG_FUNC,
                   &reg_val,
                   1);

    return;
}

/**
 * \brief method "awbl_pcf85263serv_get" handler
 */
aw_local aw_err_t __pcf85263_rtcserv_get (struct awbl_dev *p_dev, void *p_arg)
{
    __PCF85263_DEV_DECL(p_this, p_dev);
    __PCF85263_DEVINFO_DECL(p_devinfo, p_dev);

    struct awbl_rtc_service *p_serv = NULL;


    /* get intcltr service instance */

    p_serv = &p_this->rtc_serv;

    /* initialize the pcf85263 service instance */

    p_serv->p_next      = NULL;
    p_serv->p_servinfo  = &p_devinfo->rtc_servinfo;
    p_serv->p_servopts  = &__g_pcf85263_servopts;
    p_serv->p_cookie    = (void *)p_dev;

    /* send back the service instance */

    *(struct awbl_rtc_service **)p_arg = p_serv;

    return AW_OK;
}

/******************************************************************************/
void awbl_pcf85263_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_pcf85263_drv_registration);
}

/* end of file */
