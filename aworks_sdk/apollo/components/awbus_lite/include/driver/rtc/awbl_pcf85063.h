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
 * \brief AWBus-lite RTC PCF85063 驱动
 *
 * PCF85063 是低功耗的 CMOS 实时时钟／日历芯片，它提供一个可编程时钟输出，一个
 * 偏移寄存器允许微调时钟，所有的地址和数据通过 I2C 总线接口串行传递。最大总线
 * 速度为400Kbits/s，每次读写数据后，内嵌的字地址寄存器会自动产生增量。
 *
 * 注意:
 * - 时间设定范围为1970~2070，超出范围会返回错误。
 *
 * 特性:
 * - 基于32.768 kHz的石英晶振提供年、月、日、周、时、分、秒计时。
 * - 低工作电流：典型值为 0.25μA（VDD=3.0V，Tamb=25℃时）。
 * - 可编程时钟输出频率为：32.768KHz，16.384 kHz, 8.192 kHz, 4.096 kHz, 
 *                         2.048 kHz, 1.024 kHz, 1 Hz。
 * - 报警功能。
 * - 分钟以及半分钟中断。
 * - 片内上电复位功能。
 * - 高温处工作环境：-40℃ to +105℃
 * - 大工作电压范围：0.9～5.5V
 * - 400KHz 的 I2C 总线接口（VDD=1.8～5.5V 时）。
 * - 可选内部集成振荡器电容 7pF 或 12.5pF。
 * - 倒计数定时器。
 * - 晶振停止检测功能。
 * - 可编程的时间偏移修正。
 * - I2C总线从地址：0x50。
 *
 * \par 1.驱动信息
 *
 *  - 驱动名:   "pcf85063"
 *  - 总线类型: AWBL_BUSID_PLB
 *  - 设备信息: struct awbl_pcf85063_devinfo
 *
 * \par 2.兼容设备
 *
 *  - PCF85063 兼容设备
 *
 * \par 3.设备定义/配置
 *
 *  - \ref grp_awbl_pcf85063_hwconf
 *
 * \par 4.用户接口
 *
 *  - \ref grp_aw_serv_rtc
 *
 * \par 5.实现相关
 *
 *  - \ref grp_awbl_pcf85063_impl
 *
 * \internal
 * \par modification history:
 * - 1.00 16-04-26  sky, first implementation
 * \endinternal
 */

#ifndef __AWBL_PCF85063_H
#define __AWBL_PCF85063_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_awbl_drv_pcf85063
 * \copydetails awbl_pcf85063.h
 * @{
 */

/**
 * \defgroup  grp_awbl_pcf85063_impl 实现相关
 * \todo
 *
 */

/**
 * \defgroup  grp_awbl_pcf85063_hwconf 设备定义/配置
 * \todo
 */

/** @} grp_awbl_drv_pcf85063 */

#include "awbl_rtc.h"
#include "awbl_i2cbus.h"

/** \brief PCF85063 驱动名 */
#define AWBL_PCF85063_NAME   "pcf85063"

/**
 * \brief PCF85063 设备信息
 */
typedef struct awbl_pcf85063_devinfo {

    /** \brief RTC 服务配置信息 */
    struct awbl_rtc_servinfo  rtc_servinfo;

    /** \brief 设备从机地址 */
    uint8_t addr;
	
	/** \brief 时钟偏置电容选择：0——7pf; 1——12.5pf */
	uint8_t cap_set;

} awbl_pcf85063_devinfo_t;

/**
 * \brief PCF85063 设备实例
 */
typedef struct awbl_pcf85063_dev {

    /** \brief 继承自 AWBus I2C device 设备 */
    struct awbl_i2c_device super;

    /** \brief RTC 服务 */
    struct awbl_rtc_service rtc_serv;

    /** \brief 设备锁 */
    AW_MUTEX_DECL(devlock);
    
} awbl_pcf85063_dev_t;

/**
 * \brief PCF85063 driver register
 */
void awbl_pcf85063_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_PCF85063_H */

/* end of file */
