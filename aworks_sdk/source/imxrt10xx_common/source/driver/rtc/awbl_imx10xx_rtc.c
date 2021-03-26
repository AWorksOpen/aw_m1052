/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-present Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/
/**
 * \file
 * \brief imx10xx SNVS RTC驱动
 *
 * \internal
 * \par modification history:
 * - 1.00 17-08-30  mex, first implementation
 * \endinternal
 */

#include "apollo.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "awbl_rtc.h"
#include "time.h"
#include "driver/rtc/awbl_imx10xx_rtc.h"


#define   __I     volatile const   /**< \brief \brief Defines 'read only' permissions */
#define   __O     volatile         /**< \brief \brief Defines 'write only' permissions */
#define   __IO    volatile         /**< \brief \brief Defines 'read / write' permissions */

/**
 * \brief SNVS - Register Layout Typedef
 */
typedef struct {

  __IO uint32_t HPLR;                    /**< \brief SNVS_HP Lock Register, offset: 0x0 */
  __IO uint32_t HPCOMR;                  /**< \brief SNVS_HP Command Register, offset: 0x4 */
  __IO uint32_t HPCR;                    /**< \brief SNVS_HP Control Register, offset: 0x8 */
  __IO uint32_t HPSICR;                  /**< \brief SNVS_HP Security Interrupt Control Register, offset: 0xC */
  __IO uint32_t HPSVCR;                  /**< \brief SNVS_HP Security Violation Control Register, offset: 0x10 */
  __IO uint32_t HPSR;                    /**< \brief SNVS_HP Status Register, offset: 0x14 */
  __IO uint32_t HPSVSR;                  /**< \brief SNVS_HP Security Violation Status Register, offset: 0x18 */
  __IO uint32_t HPHACIVR;                /**< \brief SNVS_HP High Assurance Counter IV Register, offset: 0x1C */
  __I  uint32_t HPHACR;                  /**< \brief SNVS_HP High Assurance Counter Register, offset: 0x20 */
  __IO uint32_t HPRTCMR;                 /**< \brief SNVS_HP Real Time Counter MSB Register, offset: 0x24 */
  __IO uint32_t HPRTCLR;                 /**< \brief SNVS_HP Real Time Counter LSB Register, offset: 0x28 */
  __IO uint32_t HPTAMR;                  /**< \brief SNVS_HP Time Alarm MSB Register, offset: 0x2C */
  __IO uint32_t HPTALR;                  /**< \brief SNVS_HP Time Alarm LSB Register, offset: 0x30 */
  __IO uint32_t LPLR;                    /**< \brief SNVS_LP Lock Register, offset: 0x34 */
  __IO uint32_t LPCR;                    /**< \brief SNVS_LP Control Register, offset: 0x38 */
  __IO uint32_t LPMKCR;                  /**< \brief SNVS_LP Master Key Control Register, offset: 0x3C */
  __IO uint32_t LPSVCR;                  /**< \brief SNVS_LP Security Violation Control Register, offset: 0x40 */
       uint8_t RESERVED_0[4];
  __IO uint32_t LPTDCR;                  /**< \brief SNVS_LP Tamper Detectors Configuration Register, offset: 0x48 */
  __IO uint32_t LPSR;                    /**< \brief SNVS_LP Status Register, offset: 0x4C */
  __IO uint32_t LPSRTCMR;                /**< \brief SNVS_LP Secure Real Time Counter MSB Register, offset: 0x50 */
  __IO uint32_t LPSRTCLR;                /**< \brief SNVS_LP Secure Real Time Counter LSB Register, offset: 0x54 */
  __IO uint32_t LPTAR;                   /**< \brief SNVS_LP Time Alarm Register, offset: 0x58 */
  __I  uint32_t LPSMCMR;                 /**< \brief SNVS_LP Secure Monotonic Counter MSB Register, offset: 0x5C */
  __I  uint32_t LPSMCLR;                 /**< \brief SNVS_LP Secure Monotonic Counter LSB Register, offset: 0x60 */
  __IO uint32_t LPPGDR;                  /**< \brief SNVS_LP Power Glitch Detector Register, offset: 0x64 */
  __IO uint32_t LPGPR0_LEGACY_ALIAS;     /**< \brief SNVS_LP General Purpose Register 0 (legacy alias), offset: 0x68 */
  __IO uint32_t LPZMKR[8];               /**< \brief SNVS_LP Zeroizable Master Key Register, array offset: 0x6C, array step: 0x4 */
       uint8_t RESERVED_1[4];
  __IO uint32_t LPGPR_ALIAS[4];          /**< \brief SNVS_LP General Purpose Registers 0 .. 3, array offset: 0x90, array step: 0x4 */
       uint8_t RESERVED_2[96];
  __IO uint32_t LPGPR[4];                /**< \brief SNVS_LP General Purpose Registers 0 .. 3, array offset: 0x100, array step: 0x4 */
       uint8_t RESERVED_3[2792];
  __I  uint32_t HPVIDR1;                 /**< \brief SNVS_HP Version ID Register 1, offset: 0xBF8 */
  __I  uint32_t HPVIDR2;                 /**< \brief SNVS_HP Version ID Register 2, offset: 0xBFC */

} imx10xx_snvs_regs_t;


#define SNVS_HPCOMR_NPSWA_EN_MASK   (0x1UL << 31)  /* Non-Privileged Software Access Enable */
#define SNVS_HPCOMR_SW_SV_MASK      (0x1UL << 8)
#define SNVS_HPCR_RTC_EN_MASK       (0x1UL)        /* HP Real Time Counter Enable */
#define SNVS_LPCR_SRTC_ENV_MASK     (0x1UL)
#define SNVS_HPCR_HP_TS_MASK        (0x1UL << 16)


#define __IMX10xx_RTC_DEV_DECL(p_this, p_dev) \
    struct awbl_imx10xx_rtc_dev *p_this = \
        (struct awbl_imx10xx_rtc_dev *)(p_dev)

#define __IMX10xx_RTC_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_imx10xx_rtc_devinfo *p_devinfo = \
        (struct awbl_imx10xx_rtc_devinfo *)AWBL_DEVINFO_GET(p_dev)

#define __IMX10xx_RTC_HW_DECL(p_hw_snvs, p_devinfo) \
        imx10xx_snvs_regs_t *p_hw_snvs =  \
        ((imx10xx_snvs_regs_t *)((p_devinfo)->regbase))


/* IMX10xx rtc 信号量初始化 */
#define __IMX10xx_RTC_DEV_LOCK_INIT(p_dev) \
    do { \
        AW_MUTEX_INIT(((struct awbl_imx10xx_rtc_dev *)(p_dev))->dev_lock, AW_SEM_Q_PRIORITY ); \
    } while(0)

/* 设备上锁 */
#define __IMX10xx_RTC_DEV_LOCK(p_dev) \
    do { \
        AW_MUTEX_LOCK(((struct awbl_imx10xx_rtc_dev *)(p_dev))->dev_lock, \
                      AW_SEM_WAIT_FOREVER); \
    } while(0)

/* 设备去锁 */
#define __IMX10xx_RTC_DEV_UNLOCK(p_dev) \
    do { \
        AW_MUTEX_UNLOCK(((struct awbl_imx10xx_rtc_dev *)(p_dev))->dev_lock); \
    } while(0)



/*******************************************************************************
  forward declarations
*******************************************************************************/
aw_local void __imx10xx_rtc_inst_init1(awbl_dev_t *p_dev);
aw_local void __imx10xx_rtc_inst_init2(awbl_dev_t *p_dev);
aw_local void __imx10xx_rtc_inst_connect(awbl_dev_t *p_dev);

aw_local aw_err_t __imx10xx_timespec_time_get (void *p_cookie, aw_timespec_t *p_tv);
aw_local aw_err_t __imx10xx_timespec_time_set (void *p_cookie, aw_timespec_t *p_tv);

aw_local aw_err_t __imx10xx_time_get (void *p_cookie, aw_tm_t *p_tm);
aw_local aw_err_t __imx10xx_time_set (void *p_cookie, aw_tm_t *p_tm);
aw_local aw_err_t __imx10xx_rtcserv_get (struct awbl_dev *p_dev, void *p_arg);

/*******************************************************************************
  locals
*******************************************************************************/

/* driver functions (must defined as aw_const) */

aw_local aw_const struct awbl_drvfuncs __g_imx10xx_rtc_drvfuncs = {
    __imx10xx_rtc_inst_init1,
    __imx10xx_rtc_inst_init2,
    __imx10xx_rtc_inst_connect
};

/* IMX10xx rtc service functions (must defined as aw_const) */

aw_local aw_const struct awbl_rtc_servopts __g_imx10xx_rtc_servopts = {
    __imx10xx_time_get,
    __imx10xx_time_set,
    NULL,
    __imx10xx_timespec_time_get,
    __imx10xx_timespec_time_set
};

/* driver methods (must defined as aw_const) */
AWBL_METHOD_IMPORT(awbl_rtcserv_get);

aw_local aw_const struct awbl_dev_method __g_imx10xx_rtc_dev_methods[] = {
    AWBL_METHOD(awbl_rtcserv_get, __imx10xx_rtcserv_get),
    AWBL_METHOD_END
};

aw_local aw_const awbl_plb_drvinfo_t __g_imx10xx_rtc_drv_registration = {
    {
        AWBL_VER_1,                       /* awb_ver */
        AWBL_BUSID_PLB,                   /* bus_id */
        AWBL_IMX10XX_RTC_NAME,            /* p_drvname */
        &__g_imx10xx_rtc_drvfuncs,        /* p_busfuncs */
        &__g_imx10xx_rtc_dev_methods[0],  /* p_methods */
        NULL                              /* pfunc_drv_probe */
    }
};


/*******************************************************************************
  implementation
*******************************************************************************/

/* 启动SNVS HP RTC */
aw_static_inline
void __imx10xx_snvs_hp_rtc_start (imx10xx_snvs_regs_t *p_hw_snvs)
{
    /* HP Real Time Counter Enable */
    writel(SNVS_HPCR_RTC_EN_MASK, &p_hw_snvs->HPCR);

    while (!(readl(&p_hw_snvs->HPCR) & SNVS_HPCR_RTC_EN_MASK)) {
    }
}

/* 停止SNVS HP RTC */
aw_static_inline
void __imx10xx_snvs_hp_rtc_stop (imx10xx_snvs_regs_t *p_hw_snvs)
{
    uint32_t reg = 0;

    /* HP Real Time Counter Disable */
    reg = readl(&p_hw_snvs->HPCR);
    reg &= ~SNVS_HPCR_RTC_EN_MASK;
    writel(reg, &p_hw_snvs->HPCR);

    while (readl(&p_hw_snvs->HPCR) & SNVS_HPCR_RTC_EN_MASK) {
    }
}

/* 启动SNVS LP RTC */
aw_static_inline
void __imx10xx_snvs_lp_rtc_start (imx10xx_snvs_regs_t *p_hw_snvs)
{

    /* LP Real Time Counter Enable */
    writel(SNVS_LPCR_SRTC_ENV_MASK, &p_hw_snvs->LPCR);

    while (!(readl(&p_hw_snvs->LPCR) & SNVS_LPCR_SRTC_ENV_MASK)) {
    }
}

/* 停止SNVS LP RTC */
aw_static_inline
void __imx10xx_snvs_lp_rtc_stop (imx10xx_snvs_regs_t *p_hw_snvs)
{
    uint32_t reg = 0;

    /* LP Real Time Counter Disable */
    reg = readl(&p_hw_snvs->LPCR);
    reg &= ~SNVS_LPCR_SRTC_ENV_MASK;
    writel(reg, &p_hw_snvs->LPCR);

    while (readl(&p_hw_snvs->LPCR) & SNVS_LPCR_SRTC_ENV_MASK) {
    }
}

/* RTC与SRTC同步 */
void __imx10xx_snvs_rtc_sync(imx10xx_snvs_regs_t *p_hw_snvs)
{
    uint32_t tmp = readl(&p_hw_snvs->HPCR);

    /* disable HP RTC */
    __imx10xx_snvs_hp_rtc_stop(p_hw_snvs);

    p_hw_snvs->HPCR |= SNVS_HPCR_HP_TS_MASK;

    /* reenable RTC in case that it was enabled before */
    if (tmp & SNVS_HPCR_RTC_EN_MASK) {
        __imx10xx_snvs_hp_rtc_start(p_hw_snvs);
    }
}


/* 初始化SNVS HP RTC */
aw_local void __imx10xx_snvs_rtc_hard_init(struct awbl_imx10xx_rtc_dev *p_this)
{
    __IMX10xx_RTC_DEVINFO_DECL(p_devinfo, p_this);
    __IMX10xx_RTC_HW_DECL(p_hw_snvs, p_devinfo);

    uint32_t reg = 0;

    reg = readl(&p_hw_snvs->HPCOMR);
    reg |= SNVS_HPCOMR_NPSWA_EN_MASK | SNVS_HPCOMR_SW_SV_MASK;
    writel(reg, &p_hw_snvs->HPCOMR);

    /* 先启动LP RTC */
    __imx10xx_snvs_lp_rtc_start(p_hw_snvs);

    /* 同步HP RTC与 LP RTC */
    __imx10xx_snvs_rtc_sync(p_hw_snvs);

    /* 启动HP RTC */
    __imx10xx_snvs_hp_rtc_start(p_hw_snvs);
}

/******************************************************************************/
aw_local aw_err_t __imx10xx_timespec_time_get (void *p_cookie, aw_timespec_t *p_tv)
{
    uint64_t ns_tmp = 0;

    __IMX10xx_RTC_DEV_DECL(p_this, p_cookie);
    __IMX10xx_RTC_DEVINFO_DECL(p_devinfo, p_this);
    __IMX10xx_RTC_HW_DECL(p_hw_snvs, p_devinfo);

    if (p_tv == NULL) {
        return  -AW_EINVAL;
    }

    uint64_t hprtcmr = readl(&p_hw_snvs->HPRTCMR) ;
    uint32_t hprtclr = readl(&p_hw_snvs->HPRTCLR) ;

    /* 计算秒数 */
    p_tv->tv_sec = (hprtcmr << 17UL) | (hprtclr >> 15UL);

    /* 计算ms数 */
    ns_tmp = hprtclr & 0x7FFFUL;
    p_tv->tv_nsec = (ns_tmp * 1000000000) >> 15;

    return AW_OK;
}



aw_local aw_err_t __imx10xx_timespec_time_set (void *p_cookie, aw_timespec_t *p_tv)
{
    __IMX10xx_RTC_DEV_DECL(p_this, p_cookie);
    __IMX10xx_RTC_DEVINFO_DECL(p_devinfo, p_this);
    __IMX10xx_RTC_HW_DECL(p_hw_snvs, p_devinfo);

    uint32_t lpsrtclr = 0;

    if (p_tv == NULL) {
        return  -AW_EINVAL;
    }

    __IMX10xx_RTC_DEV_LOCK(p_cookie);

    lpsrtclr = ((uint64_t)p_tv->tv_nsec << 15U) / 1000000000;
    lpsrtclr |= (p_tv->tv_sec << 15U);

    __imx10xx_snvs_hp_rtc_stop(p_hw_snvs);
    __imx10xx_snvs_lp_rtc_stop(p_hw_snvs);

    /* 写入时间 */
    writel((uint32_t)(p_tv->tv_sec >> 17U), &p_hw_snvs->LPSRTCMR);
    writel(lpsrtclr, &p_hw_snvs->LPSRTCLR);

    __imx10xx_snvs_lp_rtc_start(p_hw_snvs);
    __imx10xx_snvs_rtc_sync(p_hw_snvs);
    __imx10xx_snvs_hp_rtc_start(p_hw_snvs);

    __IMX10xx_RTC_DEV_UNLOCK(p_cookie);

    return  AW_OK;
}

/******************************************************************************/
aw_local aw_err_t __imx10xx_time_get (void *p_cookie, aw_tm_t *p_tm)
{
    aw_time_t time_val = 0;

    __IMX10xx_RTC_DEV_DECL(p_this, p_cookie);
    __IMX10xx_RTC_DEVINFO_DECL(p_devinfo, p_this);
    __IMX10xx_RTC_HW_DECL(p_hw_snvs, p_devinfo);

    if (p_tm == NULL) {
        return  -AW_EINVAL;
    }

    time_val = (readl(&p_hw_snvs->HPRTCMR) << 17UL) |
               (readl(&p_hw_snvs->HPRTCLR) >> 15UL);

    /* 将日历时间（秒数）转换为细分时间 */
    if (aw_time_to_tm(&time_val, p_tm) != AW_OK) {
        return  -AW_EINVAL;
    }

    return AW_OK;

}

aw_local aw_err_t __imx10xx_time_set (void *p_cookie, aw_tm_t *p_tm)
{
    aw_time_t time_val = 0;
    __IMX10xx_RTC_DEV_DECL(p_this, p_cookie);
    __IMX10xx_RTC_DEVINFO_DECL(p_devinfo, p_this);
    __IMX10xx_RTC_HW_DECL(p_hw_snvs, p_devinfo);

    if (p_tm == NULL) {
        return  -AW_EINVAL;
    }

    /** 将细分时间转换为日历时间（秒数） */
    if (aw_tm_to_time(p_tm, &time_val) != AW_OK) {
        return  -AW_EINVAL;
    }

    __IMX10xx_RTC_DEV_LOCK(p_cookie);

    __imx10xx_snvs_hp_rtc_stop(p_hw_snvs);
    __imx10xx_snvs_lp_rtc_stop(p_hw_snvs);

    /* 写入时间 */
    writel((uint32_t)(time_val >> 17U), &p_hw_snvs->LPSRTCMR);
    writel((uint32_t)(time_val << 15U), &p_hw_snvs->LPSRTCLR);

    __imx10xx_snvs_lp_rtc_start(p_hw_snvs);
    __imx10xx_snvs_rtc_sync(p_hw_snvs);
    __imx10xx_snvs_hp_rtc_start(p_hw_snvs);

    __IMX10xx_RTC_DEV_UNLOCK(p_cookie);

    return  AW_OK;
}


/******************************************************************************/


/**
 * \brief first level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __imx10xx_rtc_inst_init1(awbl_dev_t *p_dev)
{
    __IMX10xx_RTC_DEV_DECL(p_this,p_dev);
    __IMX10xx_RTC_DEVINFO_DECL(p_devinfo, p_this);

    if (p_devinfo->pfunc_plfm_init != NULL) {
        p_devinfo->pfunc_plfm_init();
    }

    /* 初始化SNVS RTC */
    __imx10xx_snvs_rtc_hard_init(p_this);
}

/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __imx10xx_rtc_inst_init2(awbl_dev_t *p_dev)
{
    __IMX10xx_RTC_DEV_LOCK_INIT(p_dev);
    return;
}

/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __imx10xx_rtc_inst_connect(awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief method "__imx10xx_rtcserv_get" handler
 */
aw_local aw_err_t __imx10xx_rtcserv_get (struct awbl_dev *p_dev, void *p_arg)
{
    __IMX10xx_RTC_DEV_DECL(p_this, p_dev);
    __IMX10xx_RTC_DEVINFO_DECL(p_devinfo, p_dev);

    struct awbl_rtc_service *p_serv = NULL;

    /* 各个设备的rtc服务函数 */
    p_serv = &p_this->rtc_serv;

    /* 对服务函数进行初始化 */
    p_serv->p_next      = NULL;
    p_serv->p_servinfo  = &p_devinfo->rtc_servinfo;
    p_serv->p_servopts  = &__g_imx10xx_rtc_servopts;
    p_serv->p_cookie    = (void *)p_dev;

    *(struct awbl_rtc_service **)p_arg = p_serv;

    return AW_OK;
}

/******************************************************************************/
void awbl_imx10xx_rtc_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_imx10xx_rtc_drv_registration);
}


/* end of file */
