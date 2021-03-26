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
 * \brief AWBus-lite RTC PCF8563 驱动
 *
 * PCF8563 是低功耗的 CMOS 实时时钟／日历芯片，它提供一个可编程时钟输出，一个
 * 中断输出和掉电检测器，所有的地址和数据通过 I 2 C 总线接口串行传递。
 * 最大总线速度为400Kbits/s，每次读写数据后，内嵌的字地址寄存器会自动产生增量。
 *
 * 特性:
 * - 低工作电流：典型值为 0.25μA（VDD=3.0V，Tamb=25℃时）。
 * - 世纪标志
 * - 大工作电压范围：1.0～5.5
 * - 低休眠电流；典型值为 0.25μA(V DD =3.0V,T amb =25℃)
 * - 400KHz 的 I 2 C  总线接口（VDD=1.8～5.5V  时）。
 * - 可编程时钟输出频率为：32.768KHz，1024Hz，32Hz，1Hz。
 * - 报警和定时器。
 * - 掉电检测器。
 * - 内部集成的振荡器电容。
 * - 片内电源复位功能。
 * - I2C总线从地址：0x50
 *
 * \par 1.驱动信息
 *
 *  - 驱动名:   "pcf8563"
 *  - 总线类型: AWBL_BUSID_PLB
 *  - 设备信息: struct awbl_pcf8563_devinfo
 *
 * \par 2.兼容设备
 *
 *  - PCF8563 兼容设备
 *
 * \par 3.设备定义/配置
 *
 *  - \ref grp_awbl_pcf8563_hwconf
 *
 * \par 4.用户接口
 *
 *  - \ref grp_aw_serv_rtc
 *
 * \par 5.实现相关
 *
 *  - \ref grp_awbl_pcf8563_impl
 *
 * \internal
 * \par modification history:
 * - 1.00 12-09-05  zen, first implementation
 * \endinternal
 */

#ifndef __AWBL_PCF8563_H
#define __AWBL_PCF8563_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_awbl_drv_pcf8563
 * \copydetails awbl_pcf8563.h
 * @{
 */

/**
 * \defgroup  grp_awbl_pcf8563_impl 实现相关
 * \todo
 *
 */

/**
 * \defgroup  grp_awbl_pcf8563_hwconf 设备定义/配置
 * \todo
 */

/** @} grp_awbl_drv_pcf8563 */

#include "awbl_rtc.h"
#include "awbl_i2cbus.h"


/** \brief PCF8563 驱动名 */
#define AWBL_PCF8563_NAME   "pcf8563"

/**
 * \brief PCF8563 设备信息
 */
typedef struct awbl_pcf8563_devinfo {

    /** \brief RTC 服务配置信息 */
    struct awbl_rtc_servinfo  rtc_servinfo;

    /** \brief 设备从机地址 */
    uint8_t addr;

} awbl_pcf8563_devinfo_t;

/**
 * \brief PCF8563 设备实例
 */
typedef struct awbl_pcf8563_dev {

    /** \brief 继承自 AWBus I2C device 设备 */
    struct awbl_i2c_device super;

    /** \brief RTC 服务 */
    struct awbl_rtc_service rtc_serv;

    /** \brief 设备锁 */
    AW_MUTEX_DECL(devlock);
} awbl_pcf8563_dev_t;

/**
 * \brief PCF8563 driver register
 */
void awbl_pcf8563_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_PCF8563_H */

/* end of file */

