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
 * \brief lpc54xxx 片上RTC 实时时钟的驱动 
  *
 * 本驱动实现了 lpc54xxx 片上RTC 实时时钟的驱动    
 *
 * \par 1.驱动信息
 *
 *  - 驱动名:  "lpc54xxx_rtc"
 *  - 总线类型: AWBL_BUSID_PLB
 *  - 设备信息: struct awbl_lpc54xxx_rtc_param
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
 
#ifndef AWBL_LPC54XXX_RTC_H_
#define AWBL_LPC54XXX_RTC_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "awbl_rtc.h"
#include "aw_sem.h"

/** \brief lpc54xxx rtc 驱动名 */
#define AWBL_LPC54XXX_RTC_NAME   "lpc54xxx-RTC"


/** \brief 控制寄存器位功能定义 */
#define   RTC_CTRL_SWRESET  	 (1u << 0)
#define   RTC_CTRL_OFD           (1u << 1)
#define   RTC_CTRL_ALARM1HZ      (1u << 2)
#define   RTC_CTRL_WAKE1KHZ      (1u << 3)
#define   RTC_CTRL_ALARMDPD_EN   (1u << 4)
#define   RTC_CTRL_WAKEDPD_EN    (1u << 5)
#define   RTC_CTRL_RTC1KHZ_EN    (1u << 6)
#define   RTC_CTRL_RTC_EN        (1u << 7)
 
/** lpc54xxx 寄存器列表 */
struct awbl_lpc54xxx_rtc_reg {
    
    uint32_t ctrl;                 /** \brief RTC control register */
    uint32_t match;                /** \brief RTC match register   */
    uint32_t count;                /** \brief RTC counter register */
    uint32_t wake;                 /** \brief RTC control register */
};

/**
 * \brief lpc54xxxRTC 设备信息
 * 进行上层设别号与底层硬件的匹配，这样上层只需要传入设备号就可以操作相应的设备了
 */
 
typedef struct awbl_lpc54xxx_rtc_devinfo {

    /** \brief RTC 服务配置信息 */
    struct awbl_rtc_servinfo  rtc_servinfo;

    /** \brief RTC 寄存器的基地址 */
    uint32_t  Base_Addr;
    
    /** \brief 平台初始化函数 */
    void (*pfunc_plfm_init)(void);   

} awbl_lpc54xxx_rtc_devinfo_t;

/**
 * \brief lpc54xxxRTC 设备实例
 */
typedef struct awbl_lpc54xxx_rtc_dev {

    /** \brief 驱动设备 */
    struct awbl_dev super;   /**< \brief 必须要的成员 */

    /** \brief 提供 RTC 服务 */
    struct awbl_rtc_service rtc_serv;
    
    struct awbl_lpc54xxx_rtc_reg *reg;

    /** \brief 设备锁 */
    AW_MUTEX_DECL(devlock);
} awbl_lpc54xxx_rtc_dev_t;




/**
 * \brief lpc54xxx RTC driver register
 */
void awbl_lpc54xxx_rtc_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* AWBL_lpc54xxx_RTC_H_ */

/* end of file */

