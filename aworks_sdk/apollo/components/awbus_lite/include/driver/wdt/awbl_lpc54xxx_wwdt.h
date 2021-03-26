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
 * \brief lpc54xxx 系列 WWDT 窗口看门狗 驱动
 *
 * 本驱动实现了 lpc54xxx 24-bit The watchdog timer 的看门狗功能，
 * 提供了启动和喂狗功能
 *
 * \par 1.驱动信息
 *
 *  - 驱动名:  "lpc54xxx_wwdt"
 *  - 总线类型: AWBL_BUSID_PLB
 *  - 设备信息: struct awbl_lpc54xxx_wwdt_param
 *
 * \par 2.兼容设备
 *
 *  - NXP LPC54XXX 系列MCU
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
 * - 1.00 14-08-20  tee, first implementation
 * \endinternal
 */

#ifndef __AWBL_LPC54XX_WWDT_H
#define __AWBL_LPC54XX_WWDT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "awbus_lite.h"
#include "awbl_wdt.h"
    
/**
 * \addtogroup grp_awbl_drv_lpc11xx_16_32timer
 * \copydetails awbl_lpc11xx_timer.h
 * @{
 */

#define LPC54XXX_WDT_NAME      "lpc54xxx_wwdt"

/** @} */


#define WWDT_MODE_WDEN      (1ul << 0) /**< \brief  enable wdt bit in mode reg */
#define WWDT_MODE_WDRESET   (1ul << 1) /**< \brief  reset wdt bit in mode reg */
#define WWDT_MODE_WDTOF     (1ul << 2) /**< \brief  watchdog time out flag    */
#define WWDT_MODE_WDINT     (1ul << 3) /**< \brief  Warning interrupt flag    */
#define WWDT_MODE_WDPROTECT (1ul << 4) /**< \brief  Watchdog update mode      */   
#define WWDT_MODE_WDLOCK    (1ul << 5) /**< \brief  Watchdog lock             */   
#define WWDT_MODE_BITMASK   ((uint32_t) 0x3F)  /** \brief Watchdog Mode Bitmask */

/******************************************************************************/

/** \brief lpc54xx watchdog timer registers */
struct lpc54xxx_wwdt_reg {
    uint32_t mode;     /**< \brief [00] Watchdog mode register */
    uint32_t tc;       /**< \brief [04] the Watchdog timer constant register */
    uint32_t feed;     /**< \brief [08] Watchdog feed sequence register. */
    
    /** \brief [0C] Watchdog timer value register.Read Only */
    uint32_t tv;       
    uint32_t rsv;      /**< \brief [10] reserved */
    
    /** \brief [14] Watchdog Warning Interrupt compare value */
    uint32_t warnint;  
    uint32_t window;   /**< \brief [18] Watchdog Window compare value */
};


/** \brief lpc54xx wwdt param */
struct awbl_lpc54xxx_wwdt_param {
         
    uint32_t    regbase;                /**< \brief 寄存器基地址           */
    uint32_t    wdt_time;               /**< \brief watchdog time (ms)     */
    uint32_t    wdt_clkin;              /**< \brief watchdog clk in freq   */
    void (*pfunc_plfm_init) (void);     /**< \brief platform initializing  */
};

/******************************************************************************/
/** \brief lpc54xx standard Timer type for AWBus */
struct awbl_lpc54xxx_wwdt_dev {
    
    /** \brief always go first */   
    awbl_dev_t                     dev;         /**< \brief AWBus device data */
    struct awbl_hwwdt_info         info;        /**< \brief hwwdt info    */
    struct lpc54xxx_wwdt_reg       *p_reg;      /**< \brief register base */
};
       
/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_LPC11XX_TIMER_H */

/* end of file */
