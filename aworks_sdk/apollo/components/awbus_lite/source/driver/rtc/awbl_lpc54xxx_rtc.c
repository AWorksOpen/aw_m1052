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
 * \brief lpc54xxx 系列 RTC 实时时钟驱动
 *
 * \internal
 * \par modification history:
 * - 1.00 14-08-20  tee, first implementation
 * \endinternal
 */
 
#include "apollo.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "awbl_rtc.h"
#include "driver/rtc/awbl_lpc54xxx_rtc.h"
#include <time.h>
#include <string.h>

#define __LPC54XXX_RTC_DEV_DECL(p_this, p_dev) \
    struct awbl_lpc54xxx_rtc_dev *p_this = \
        (struct awbl_lpc54xxx_rtc_dev *)(p_dev)

#define __LPC54XXX_RTC_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_lpc54xxx_rtc_devinfo *p_devinfo = \
        (struct awbl_lpc54xxx_rtc_devinfo *)AWBL_DEVINFO_GET(p_dev)

/* LPC54XXX rtc 信号量初始化 */
#define __LPC54XXX_RTC_DEV_LOCK_INIT(p_dev) \
    do { \
        AW_MUTEX_INIT(((struct awbl_lpc54xxx_rtc_dev *)(p_dev))->devlock, \
                         AW_SEM_Q_PRIORITY ); \
    } while(0)
/* 上锁 */
#define __LPC54XXX_RTC_DEV_LOCK(p_dev) \
    do { \
        AW_MUTEX_LOCK(((struct awbl_lpc54xxx_rtc_dev *)(p_dev))->devlock, \
                      AW_SEM_WAIT_FOREVER); \
    } while(0)
/* 去锁 */
#define __LPC54XXX_RTC_DEV_UNLOCK(p_dev) \
    do { \
        AW_MUTEX_UNLOCK(((struct awbl_lpc54xxx_rtc_dev *)(p_dev))->devlock); \
    } while(0)

/** \brief macro to read from lpc54xxx rtc 32-bit register */
#define __AWBL_RTC_REG_READ(reg, value)         value = readl(&(reg))

/** \brief macro to write to lpc54xxx  32-bit  register */
#define __AWBL_RTC_REG_WRITE(reg, value)        writel(value, &(reg))
    
/*******************************************************************************
  forward declarations
*******************************************************************************/
aw_local void __lpc54xxx_rtc_inst_init1(awbl_dev_t *p_dev);
aw_local void __lpc54xxx_rtc_inst_init2(awbl_dev_t *p_dev);
aw_local void __lpc54xxx_rtc_inst_connect(awbl_dev_t *p_dev);

aw_local aw_err_t __lpc54xxx_rtc_time_get (void *p_cookie, aw_tm_t *p_time);
aw_local aw_err_t __lpc54xxx_rtc_time_set (void *p_cookie, aw_tm_t *p_time);
aw_local aw_err_t __lpc54xxx_rtcserv_get (struct awbl_dev *p_dev, void *p_arg);

/*******************************************************************************
  locals
*******************************************************************************/

/* driver functions (must defined as aw_const) */

aw_local aw_const struct awbl_drvfuncs __g_lpc54xxx_rtc_drvfuncs = {
    __lpc54xxx_rtc_inst_init1,
    __lpc54xxx_rtc_inst_init2,
    __lpc54xxx_rtc_inst_connect
};

/* lpc54xxx rtc service functions (must defined as aw_const) */

aw_local aw_const struct awbl_rtc_servopts __g_lpc54xxx_rtc_servopts = {
    __lpc54xxx_rtc_time_get,                     /* time_get */
    __lpc54xxx_rtc_time_set,                     /* time_set */
    NULL
};

/* driver methods (must defined as aw_const) */
AWBL_METHOD_IMPORT(awbl_rtcserv_get);

aw_local aw_const struct awbl_dev_method __g_lpc54xxx_rtc_dev_methods[] = {
    AWBL_METHOD(awbl_rtcserv_get, __lpc54xxx_rtcserv_get),
    AWBL_METHOD_END
};

aw_local aw_const awbl_plb_drvinfo_t __g_lpc54xxx_rtc_drv_registration = {
    {
        AWBL_VER_1,                       /* awb_ver */
        AWBL_BUSID_PLB,                   /* bus_id */
        AWBL_LPC54XXX_RTC_NAME,           /* p_drvname */
        &__g_lpc54xxx_rtc_drvfuncs,       /* p_busfuncs */
        &__g_lpc54xxx_rtc_dev_methods[0], /* p_methods */
        NULL                              /* pfunc_drv_probe */
    }
};


/*******************************************************************************
  implementation
*******************************************************************************/

/******************************************************************************/
aw_local aw_err_t __lpc54xxx_rtc_time_get (void *p_cookie, aw_tm_t *p_tm)
{

    aw_time_t  TimeValue   = 0;
    
    __LPC54XXX_RTC_DEV_DECL(p_this, p_cookie);
    
    if (p_tm == NULL) {
        return  -EFAULT;
    }

    /* 得到的是日历时间 */
    __AWBL_RTC_REG_READ(p_this->reg->count,TimeValue);
    
 
    /* 将日历时间（秒数）转换为细分时间 */
    if (aw_time_to_tm(&TimeValue, p_tm) != AW_OK) {
        return  -EINVAL;
    }

    return AW_OK;
   
}

/******************************************************************************/
aw_local aw_err_t __lpc54xxx_rtc_time_set (void *p_cookie, aw_tm_t *p_tm)
{
    aw_time_t  TimeValue = 0;
    uint32_t value;
    
    __LPC54XXX_RTC_DEV_DECL(p_this, p_cookie);
    
    if (p_tm == NULL) {
        return  -EFAULT;
    }
    
    /* 将细分时间转换为日历时间（秒数） */
    if (aw_tm_to_time(p_tm, &TimeValue) != AW_OK) {
        return  -EFAULT;
    }

    __LPC54XXX_RTC_DEV_LOCK(p_cookie);

    __AWBL_RTC_REG_READ(p_this->reg->ctrl,value);
    value &= (~RTC_CTRL_RTC_EN);                     /* disable the RTC  */
    __AWBL_RTC_REG_WRITE(p_this->reg->ctrl,value);   
    
    __AWBL_RTC_REG_WRITE(p_this->reg->count,TimeValue);
    
    value |= (RTC_CTRL_RTC_EN);                      /* enable the RTC  */
    __AWBL_RTC_REG_WRITE(p_this->reg->ctrl,value);       
    
    __LPC54XXX_RTC_DEV_UNLOCK(p_cookie);

    return  AW_OK;
}

/**
 * \brief first level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __lpc54xxx_rtc_inst_init1(awbl_dev_t *p_dev)
{
    __LPC54XXX_RTC_DEVINFO_DECL(p_devinfo, p_dev);
    __LPC54XXX_RTC_DEV_DECL(p_this,p_dev);
    
    uint32_t value;
    uint32_t i;
    
    if (p_devinfo->pfunc_plfm_init != NULL) {
        p_devinfo->pfunc_plfm_init();
    }
    
    p_this->reg = (struct awbl_lpc54xxx_rtc_reg *)(p_devinfo->Base_Addr);
    
    __AWBL_RTC_REG_READ(p_this->reg->ctrl,value);
    value &= (~RTC_CTRL_SWRESET);                   /* cancel the reset */
    value &= (~RTC_CTRL_RTC_EN);                    /* disable the RTC*/     
    __AWBL_RTC_REG_WRITE(p_this->reg->ctrl,value);
    
    for (i = 0; i < 10000; i++){
        __AWBL_RTC_REG_READ(p_this->reg->ctrl,value);
        if ((value & RTC_CTRL_OFD) == 0) {          /* RTC oscillator is run */
            break;
        }
         
    }
    
    if (i == 10000) {                          /* RTC oscillator detect fail */
        return;
    }
    
    __AWBL_RTC_REG_WRITE(p_this->reg->count,0);    /* clear the counter */
    
    value |= RTC_CTRL_RTC_EN;
    
    __AWBL_RTC_REG_WRITE(p_this->reg->ctrl,value); /* enable the RTC */
    
}

/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __lpc54xxx_rtc_inst_init2(awbl_dev_t *p_dev)
{
    __LPC54XXX_RTC_DEV_LOCK_INIT(p_dev);
    return;
}

/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __lpc54xxx_rtc_inst_connect(awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief method "__lpc54xxx_rtcserv_get" handler
 */
aw_local aw_err_t __lpc54xxx_rtcserv_get (struct awbl_dev *p_dev, void *p_arg)
{
    __LPC54XXX_RTC_DEV_DECL(p_this, p_dev);
    __LPC54XXX_RTC_DEVINFO_DECL(p_devinfo, p_dev);

    struct awbl_rtc_service *p_serv = NULL;

    /* 各个设备的rtc服务函数 */
    p_serv = &p_this->rtc_serv;

    /* 对服务函数进行初始化 */
    p_serv->p_next      = NULL;
    p_serv->p_servinfo  = &p_devinfo->rtc_servinfo;
    p_serv->p_servopts  = &__g_lpc54xxx_rtc_servopts;
    p_serv->p_cookie    = (void *)p_dev;

    *(struct awbl_rtc_service **)p_arg = p_serv;

    return AW_OK;
}

/******************************************************************************/
void awbl_lpc54xxx_rtc_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_lpc54xxx_rtc_drv_registration);
}

/* end of file */
