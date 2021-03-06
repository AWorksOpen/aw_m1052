/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief iMX RT10xx PIT 驱动
 *
 * 本驱动实现了定时器的周期中断功能。
 *
 * \internal
 * \par modification history
 * - 1.00 17-11-13  pea, first implementation
 * \endinternal
 */

#ifndef __AWBL_IMX10xx_PIT_H
#define __AWBL_IMX10xx_PIT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "awbus_lite.h"
#include "awbl_timer.h"
#include "aw_clk.h"
#include "driver/timer/imx10xx_pit_regs.h"

#define AWBL_IMX10XX_PIT_DRV_NAME   "imx10xx_pit"

/** \brief 设备配置信息 */
typedef struct awbl_imx10xx_pit_devinfo {
    uint32_t           alloc_byname;   /**< \brief 带 1 的位表示该定时器可以通过名称进行分配 */
    uint32_t           regbase;        /**< \brief 寄存器基地址 */
    int                inum;           /**< \brief 中断号 */
    const aw_clk_id_t  clk_id;         /**< \brief 指向时钟 ID 数组的指针 */

    /** \brief 指向平台初始化函数的指针 */
    void             (*pfn_plfm_init) (void);
} awbl_imx10xx_pit_devinfo_t;

/** \brief 设备实例 */
typedef struct awbl_imx10xx_pit_dev {
    awbl_dev_t              dev;                   /**< \brief AWBus 设备实例结构体 */
    struct awbl_timer       tmr;                   /**< \brief 标准定时器结构 */

    uint32_t                count;                 /**< \brief 当前最大计数值 */
    uint32_t                rollover_period;       /**< \brief 翻转周期 */

    struct awbl_timer_param param;                 /**< \brief 标准定时器参数 */

    void                  (*pfn_isr)(void *p_arg); /**< \brief 定时器中断回调函数 */
    void                   *p_arg;                 /**< \brief 定时器中断回调函数参数 */
    volatile int            enabled;               /**< \brief 定时器是否使能 */
} awbl_imx10xx_pit_dev_t;

/**
 * \brief 将 IMX10xx PIT 驱动注册到 AWBus 子系统中
 *
 * \note 本函数应在 aw_prj_early_init() 中初始化，本函数中禁止调用操作系统相关函数
 */
void awbl_imx10xx_pit_drv_register (void);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_IMX10xx_PIT_H */

/* end of file */
