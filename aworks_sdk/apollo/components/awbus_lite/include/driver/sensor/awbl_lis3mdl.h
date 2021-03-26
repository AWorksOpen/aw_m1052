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
 * \brief LIS3MDL (三轴磁传感器) 头文件
 *
 * \internal
 * \par Modification History
 * - 1.00 18-10-25  wan, first implementation
 * \endinternal
 */

#ifndef __AWBL_LIS3MDL_H
#define __AWBL_LIS3MDL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aworks.h"
#include "awbus_lite.h"
#include "aw_spinlock.h"
#include "awbl_i2cbus.h"
#include "aw_sensor.h"
#include "awbl_sensor.h"
#include "aw_isr_defer.h"

#define AWBL_LIS3MDL_NAME      "awbl_lis3mdl"

/**
 * \brief LIS3MDL 设备信息结构体
 */
typedef struct awbl_lis3mdl_devinfo {

    /**
     * \brief 该传感器通道的起始id
     *
     * LIS3MDL 共有4路采集通道,分别为X轴，Y轴，Z轴磁感应强度和一路温度数据，从
     * start_id开始(包括start_id)连续四个ID分配给该模块的四个通道，其中
     * start_id 对应模块X轴磁感应强度，start_id + 1 对应模块的Y轴磁感应强度
     * eg： 将start_id配置为0, 则ID = 0,对应模块的通道1，ID = 1对应模块的通道2.
     */
    int     start_id;

    int     drdy_pin;  /*< \brief 数据准备就绪触发引脚 */
    int     int_pin;   /*< \brief x,y,z轴数据阈值或数据溢出中断引脚 */
    uint8_t i2c_addr;  /*< \brief I2C 7位 设备地址(该设备7位地址固定为 0x1C) */

    /** \brief 平台初始化回调函数（回调注册触发引脚已被系统使用） */
    void    (*pfunc_plfm_init)(void);

} awbl_lis3mdl_devinfo_t;

/**
 * \brief LIS3MDL 设备结构体
 */
typedef struct awbl_lis3mdl_dev {

    /** \brief 继承自 AWBus 设备 */
    struct awbl_dev                      dev;

    /** \brief I2C 从机设备 */
    aw_i2c_device_t                      i2c_dev;

    /** \brief sensor 服务  */
    awbl_sensor_service_t                sensor_serv;

    /** \brief 通道触发回调函数 */
    aw_sensor_trigger_cb_t               pfn_trigger_fnc[3];

    /** \brief 通道触发回调函数参数 */
    void                                *p_argc[3];

    /** \brief 通道触发标志 */
    uint32_t                             flags[3];

    /** \brief 中断延时处理任务 */
    struct aw_isr_defer_job              g_myjob[2];

    /** \brief 采样频率，每秒采样的次数 */
    aw_sensor_val_t                      sampling_rate;

    /** \brief 上下阈值的绝对值（上下阈值互为相反数） */
    aw_sensor_val_t                      threshold_data;

    /** \brief 当前的量程 */
    uint8_t                              full_scale;

    /** \brief 数据准备就绪的值保存 */
    aw_sensor_val_t                      current_data[3];

    /** \brief 触发使能位，每位对应一个通道 */
    uint8_t                              trigger;

} awbl_lis3mdl_dev_t;

/*
 * \brief 注册 LIS3MDL 驱动
 */
void awbl_lis3mdl_drv_register (void);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_LIS3MDL_H */

/* end of file */
