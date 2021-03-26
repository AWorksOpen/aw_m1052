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
 * \brief LPC11XX和LPC11Cxx系列定时器驱动
 *
 * 本驱动只实现了 LPC11XX 16-bit/32-bit 定时器的定时器功能，不提供计数、匹配、
 * 捕获和PWM功能。
 *
 * \par 1.驱动信息
 *
 *  - 驱动名:  "lpc11xx_timer32"(32-bit timer) ; "lpc11xx_timer16"(16-bit timer)
 *  - 总线类型: AWBL_BUSID_PLB
 *  - 设备信息: struct awbl_lpc11xx_timer_param
 *
 * \par 2.兼容设备
 *
 *  - NXP LPC11XX 系列MCU
 *  - NXP LPC13XX 系列MCU
 *  - 其它与LPC11XX TIMER 兼容的设备
 *
 * \par 3.设备定义/配置
 *
 *  - \ref grp_awbl_drv_lpc11xx_16_32timer_hwconf
 *
 * \par 4.用户接口
 *
 *  - \ref grp_aw_serv_hwtimer
 *
 * \internal
 * \par modification history
 * - 1.00 12-11-15  orz, first implementation
 * \endinternal
 */

#ifndef __AWBL_LPC11XX_TIMER_H
#define __AWBL_LPC11XX_TIMER_H

#include "awbl_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_awbl_drv_lpc11xx_16_32timer
 * \copydetails awbl_lpc11xx_timer.h
 * @{
 */

/**
 * \defgroup  grp_awbl_drv_lpc11xx_16_32timer_hwconf 设备定义/配置
 *
 * LPC11XX TIMER 硬件设备的定义如下面的代码所示，用户在定义目标系统的硬件资源时,
 * 可以直接将这段代码嵌入到 awbus_lite_hwconf_usrcfg.c 中对应的位置，然后对设备
 * 信息进行必要的配置(可配置的项目用【x】标识)。
 *
 * \note 有多少个TIMER，就将下面的代码嵌入多少次，注意将变量修改为不同的名字。
 *
 * \include  hwconf_usrcfg_hwtimer_lpc11xx.c
 * \note 以下配置项中带【用户配置】标识的，表示用户在开发应用时可以配置此参数。
 *
 * - 【1】平台相关初始化函数 \n
 * 在平台初始化函数中完成使能时钟等与平台相关的操作。
 *
 * - 【2】定时器可达到的最小频率 \n
 * 请查阅手册中定时器的可用分频设置来确定该参数
 *
 * - 【3】定时器可达到的最大频率 \n
 * 请查阅手册中定时器的可用分频设置来确定该参数
 *
 * - 【4】定时器的输入时钟频率 \n
 * 请查阅手册及系统时钟设置来确定该参数
 *
 * - 【5】定时器的溢出翻转周期 \n
 *
 * - 【6】定时器寄存器起始地址 \n
 * 请查阅手册确定该参数
 *
 * - 【8】设备名称 \n
 * 16位定时器请填“lpc11xx_timer16”，32位定时器请填写“lpc11xx_timer32”
 *
 * - 【9】定时器设备的单元号 \n
 * 用于区分同类设备，如16位定时器0和16位定时器1
 */

/** @} */

/**
 * \name register defines for lpc11xx and lpc11cxx
 * @{
 */
/* IR, n = [0, 3] */
#define IR_MR(n)   (1u << (n))  /**< \brief interrupt flag for match chan n */
#define IR_CR0      0x10u       /**< \brief interrupt flag for capture chan 0 */

/* TCR */
#define TCR_CEN     0x01u       /**< \brief Count Enable */
#define TCR_CRST    0x02u       /**< \brief Counter Reset */

/* MCR, n = [0, 3] */
#define MCR_MRI(n)  (1u << ((n) * 3u))  /**< \brief Interrupt on MRn */
#define MCR_MRR(n)  (2u << ((n) * 3u))  /**< \brief Reset on MRn */
#define MCR_MRS(n)  (4u << ((n) * 3u))  /**< \brief Stop on MRn */

/* CCR, n = [0] */
#define CCR_CAPRE(n) (1u << ((n) * 3u)) /**< \brief Capture on rising edge */
#define CCR_CAPFE(n) (2u << ((n) * 3u)) /**< \brief Capture on falling edge */
#define CCR_CAPI(n)  (4u << ((n) * 3u)) /**< \brief Interrupt on CAP event */

/* EMR, n = [0, 4] */
#define EMR_EM(n)       (1u << (n))     /**< \brief External Match n */
#define EMR_EMC(n, v)   ((v) << ( 4u + ((n) << 1u)))
                                        /**< \brief External Match Control n */
#define EMR_EMC_NONE    0x0u    /**< \brief Do Nothing */
#define EMR_EMC_CLR     0x1u    /**< \brief Clear External Match bit/output */
#define EMR_EMC_SET     0x2u    /**< \brief Set External Match bit/output */
#define EMR_EMC_TOGGLE  0x3u    /**< \brief Toggle External Match bit/output */

/* CTCR */
/** \brief Timer Mode: every rising PCLK edge*/
#define CTCR_CTM_TIMER      0x0u
/** \brief Counter Mode: TC incremented on rising edges on the CAP input */
#define CTCR_CTM_COUNTER_RE 0x1u
/** \brief Counter Mode: TC incremented on falling edges on the CAP input */
#define CTCR_CTM_COUNTER_FE 0x2u
/** \brief Counter Mode: TC incremented on both edges on the CAP input */
#define CTCR_CTM_COUNTER_BE 0x3u
/** \brief Count Input Select. CAP0 pin is sampled for clocking */
#define CTCR_CIS_CAP0       0x0u

/** \brief PWMC, n = [0, 3] */
#define PWMC_PWMEN(n)       (1u << (n)) /**< \brief PWM is enabled for MATn */
/** @} */

/******************************************************************************/

/** \brief lpc11xx counter/timer registers */
struct lpc11xx_timer_reg {
    uint32_t ir;        /**< \brief [00] Interrupt Register */
    uint32_t tcr;       /**< \brief [04] Timer Control Register */
    uint32_t tc;        /**< \brief [08] Timer Counter Register */
    uint32_t pr;        /**< \brief [0C] Prescale Register */
    uint32_t pc;        /**< \brief [10] Prescale Counter Register */
    uint32_t mcr;       /**< \brief [14] Match Control Register */
    uint32_t mr[4];     /**< \brief [18] Match Register n, n = [0, 3] */
    uint32_t ccr;       /**< \brief [28] Capture Control Register */
    uint32_t cr0;       /**< \brief [2C] Capture Register 0 */
    uint32_t emr;       /**< \brief [30] External Match Register */
    uint32_t rsv[12];   /**< \brief [40 - 6C] reserved */
    uint32_t ctcr;      /**< \brief [70] Count Control Register */
    uint32_t pwmc;      /**< \brief [74] PWM Control Register */
};

/******************************************************************************/

/** \brief timer fixed parameter */
struct awbl_lpc11xx_timer_param {
    uint32_t    clk_frequency;          /**< \brief clock frequency         */
    
    /** 
     * \brief the bit with 1 means the chan the timer can any be alloc by name 
     */
    uint32_t     alloc_byname;          
    uint32_t         reg_addr;          /**< \brief register base address   */
    int                  inum;          /**< \brief interrupt number        */
    void (*pfunc_plfm_init) (void);     /**< \brief platform initializing   */
};

/** \brief convert AWBus timer device info to lpc11xx timer parameter */
#define awbldev_get_lpctimer_param(p_dev) \
    ((struct awbl_lpc11xx_timer_param *)AWBL_DEVINFO_GET(p_dev))

/******************************************************************************/

/** \brief lpc11xx Timer type for AWBus */
struct awbl_lpc11xx_timer {
    
    /** \brief always go first */   
    awbl_dev_t  dev;                    /**< \brief AWBus device data */
    
    struct awbl_timer           timer;  /**< \brief awbl timer  data */
    
    struct lpc11xx_timer_reg   *reg;    /**< \brief register base    */
    
    uint32_t                    count;  /**< \brief current max counts */
    uint32_t          rollover_period;  /**< \brief rollover_period */

    struct awbl_timer_param  param;     /**< \brief the timer's param */
    
    void (*pfunc_isr) (void *p_arg);    /**< \brief ISR */
    void  *p_arg;                       /**< \brief ISR argument */
};

 
/** \brief convert a awbl_dev pointer to awbl_lpc11xx_timer pointer */
#define awbldev_to_lpc_timer(p_dev) \
        ((struct awbl_lpc11xx_timer *)(p_dev))

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_LPC11XX_TIMER_H */

/* end of file */
