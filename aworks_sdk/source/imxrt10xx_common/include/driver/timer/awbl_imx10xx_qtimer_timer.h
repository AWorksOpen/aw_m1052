/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/
/**
 * \file
 * \brief iMX RT1050 QTimer定时器驱动
 *
 * \internal
 * \par modification history:
 * - 1.00 19-06-28  ral, first implemetation
 * \endinternal
 */
#ifndef __AWBL_IMX10XX_QTIMER_TIMER_H
#define __AWBL_IMX10XX_QTIMER_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "apollo.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "aw_clk.h"
#include "awbl_timer.h"

/* 驱动名 */
#define AWBL_IMX10XX_QTIMER_TIMER_NAME   "imx10xx_qtmr"

/**
 * \brief QTimer Timer 通道选择列表
 */
typedef enum qtimer_timer_channel_sel {
    kQTMR_Timer_Channel_0 = 0,    /**< \brief QTimer Timer Channel 0 */
    kQTMR_Timer_Channel_1,        /**< \brief QTimer Timer Channel 1 */
    kQTMR_Timer_Channel_2,        /**< \brief QTimer Timer Channel 2 */
    kQTMR_Timer_Channel_3,        /**< \brief QTimer Timer Channel 3 */
} qtimer_timer_channel_sel_t;

/**
 * \brief 时钟预分频选择列表
 */
typedef enum qtimer_timer_primary_count_source {
    QTimer_ClockDivide_1   = 1,             /**< \brief IP bus clock divide by 1   prescaler */
    QTimer_ClockDivide_2   = 2,             /**< \brief IP bus clock divide by 2   prescaler */
    QTimer_ClockDivide_4   = 4,             /**< \brief IP bus clock divide by 4   prescaler */
    QTimer_ClockDivide_8   = 8,             /**< \brief IP bus clock divide by 8   prescaler */
    QTimer_ClockDivide_16  = 16,            /**< \brief IP bus clock divide by 16  prescaler */
    QTimer_ClockDivide_32  = 32,            /**< \brief IP bus clock divide by 32  prescaler */
    QTimer_ClockDivide_64  = 64,            /**< \brief IP bus clock divide by 64  prescaler */
    QTimer_ClockDivide_128 = 128            /**< \brief IP bus clock divide by 128 prescaler */
} qtimer_timer_primary_count_source_t;

enum qtimer_timer_count_mode {
    QTimer_Count_Mode_Off = AW_FALSE,   /**< \brief QTimer count mode off */
    QTimer_Count_Mode_On  = AW_TRUE,    /**< \brief QTimer count mode on */
};

/**
 * \brief iMX RT10xx QTimer Timer 设备信息
 */
typedef struct awbl_imx10xx_qtimer_timer_devinfo {
    uint32_t                                alloc_byname;       /**< \brief 带 1 的位表示该定时器可以通过名称进行分配 */
    uint32_t                                inum;               /**< \brief 中断向量号 */
    uint32_t                                regbase;            /**< \brief QTimer寄存器基地址 */
    aw_clk_id_t                             clk_id;             /**< \brief QTimer时钟ID */
    qtimer_timer_primary_count_source_t     prescaler_div;      /**< \brief 时钟预分频 */
    qtimer_timer_channel_sel_t              channel;            /**< \brief 通道号 */
    void (*pfn_plfm_init) (void);                               /**< \breif platform initializing */
    aw_bool_t                               count_mode;         /**< \brief 输入计数模式开关 */
} awbl_imx10xx_qtimer_timer_devinfo_t;

/**
 * \brief iMX RT10xx QTimer Timer device
 */
typedef struct awbl_imx10xx_qtimer_timer_dev {

    struct awbl_dev         dev;                    /**< \brief 继承自 AWBus 设备 */
    struct awbl_timer       tmr;                    /**< \brief 标准定时器结构 */

    uint32_t                count;                  /**< \brief 当前最大计数值 */
    uint32_t                rollover_period;        /**< \brief 翻转周期 */

    struct awbl_timer_param param;                  /**< \brief 标准定时器参数 */

    void                  (*pfn_isr)(void *p_arg);  /**< \brief 定时器中断回调函数 */
    void                   *p_arg;                  /**< \brief 定时器中断回调函数参数 */
    volatile int            enabled;                /**< \brief 定时器是否使能 */

} awbl_imx10xx_qtimer_timer_dev_t;

/**
 * \brief iMX RT10xx QTimer Timer driver register
 */
void awbl_imx10xx_qtimer_timer_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX10XX_QTIMER_TIMER_H */
