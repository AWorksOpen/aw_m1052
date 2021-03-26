/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief iMX RT1064 时间戳
 *
 * 本驱动使用systick实现时间戳
 *
 * \internal
 * \par modification history
 * - 1.00 20-04-04  wk, first implementation
 * \endinternal
 */


#ifndef __AWBL_IMX1010_TIMESTAMP_H_
#define __AWBL_IMX1010_TIMESTAMP_H_

#define AWBL_IMX1010_TIMESTAMP_NAME     "awbl_imx1010_timestamp"

#include "aw_timer.h"
#include "aw_sem.h"

#define MAX_TIMESTAMP_FREQUENCY		1000000

/** \brief 设备配置信息 */
typedef struct awbl_imx1010_timestamp_devinfo {
    uint32_t        update_period;      /**< \brief 更新计数的周期时间（ms）. */
    uint32_t        freq;               /**< \brief 自定义的时钟频率. */

    /** \brief 指向平台初始化函数的指针 */
    void             (*pfn_plfm_init) (void);
} awbl_imx1010_timestamp_devinfo_t;

/** \brief 设备实例 */
typedef struct awbl_imx1010_timestamp_dev {
    awbl_dev_t      dev;                /**< \brief AWBus 设备实例结构体 */

    aw_timer_t      p_timer;            /**< \brief 软件定时器. */

} awbl_imx1010_timestamp_dev_t;

/**
 * \brief 将 IMX10xx PIT 驱动注册到 AWBus 子系统中
 *
 * \note 本函数应在 aw_prj_early_init() 中初始化，本函数中禁止调用操作系统相关函数
 */
void awbl_imx1010_timestamp_drv_register (void);


#endif /* __AWBL_IMX1010_TIMESTAMP_H_ */
