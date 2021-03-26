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
 * \brief AWBus-lite RTC RX8025SA 驱动
 *
 * RX8025SA 是内置 32.768kHZ 数字温度补偿晶体振荡器（DTCXO），以 I2C 总线接口为
 * 通信方式的高精度实时时钟芯片。
 * 它有时钟和自动闰年校正日历功能，还有报警、固定周期定时中断、时间更
 * 新中断和使能 OE 的 37.768kHz 频率输出等多种功能。
 *
 * 特性:
 * - 内置高稳定度的 32.768kHz 的 DTCXO（数字温度补偿晶体振荡器）
 * - 支持 I2C总线高速模式（400K）。
 * - 定时报警功能（可设定：天，日期，小时，分钟）
 * - 固定周期定时中断功能。
 * - 时间更新中断功能
 * - 32.768kHz 频率输出（具有使能 OE 功能）
 * - 闰年自动调整功能。（2000 到 2099）
 * - 宽范围接口电压：2.2V 到 5.5V
 * - 宽范围时间保持电压：1.8V 到 5.5V
 * - 低电流功耗：<1.8uA/3V（Type）
 *
 * \internal
 * \par modification history:
 * - 1.00 14-12-22  zen, first implementation
 * \endinternal
 */

#ifndef __AWBL_RX8025SA_H
#define __AWBL_RX8025SA_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


#include "awbl_rtc.h"
#include "awbl_i2cbus.h"

/** \brief RX8025SA 驱动名 */
#define AWBL_RX8025SA_NAME   "rx8025sa"

#define AWBL_RX8025SA_YEAR_OFFS 100

/**
 * \brief RX8025SA 设备信息
 */
typedef struct awbl_rx8025sa_devinfo {

    /** \brief RTC 服务配置信息 */
    struct awbl_rtc_servinfo  rtc_servinfo;

    /** \brief 设备从机地址 */
    uint8_t addr;

} awbl_rx8025sa_devinfo_t;

/**
 * \brief RX8025SA 设备实例
 */
typedef struct awbl_rx8025sa_dev {

    /** \brief 继承自 AWBus I2C device 设备 */
    struct awbl_i2c_device super;

    /** \brief RTC 服务 */
    struct awbl_rtc_service rtc_serv;

    /** \brief 设备锁 */
    AW_MUTEX_DECL(devlock);
} awbl_rx8025sa_dev_t;

/**
 * \brief RX8025SA driver register
 */
void awbl_rx8025sa_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_RX8025SA_H */

/* end of file */

