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
 * \brief HTS221 (测温湿度模块) 头文件
 *
 * \internal
 * \par Modification History
 * - 1.00 18-10-12  wan, first implementation
 * \endinternal
 */

#ifndef __AWBL_HTS221_H
#define __AWBL_HTS221_H

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

#define AWBL_HTS221_NAME      "awbl_hts221"

/**
 * \brief HTS221 设备信息结构体
 */
typedef struct awbl_hts221_devinfo {
    /**
     * \brief 该传感器通道的起始id
     *
     * HTS221 共有1路湿度采集通道和1路温度采集通道,从start_id开始(包括start_id)
     * 连续两个ID分配给该模块的两个通道，其中 start_id 对应模块的湿度传感器通道，
     * start_id + 1 对应模块的温度传感器通道
     * eg：将start_id 配置为0, 则ID = 0,对应模块的通道1，ID = 1对应模块的通道2.
     */
    int     start_id;

    /**
     * \brief 数据准备就绪触发引脚
     *
     * 该引脚在AWorks接口的AW-Sensor板上已经接为 GPIO1_27,故直接传入该引脚即可
     */
    int     trigger_pin;

    /*
     * \brief I2C 7位 设备地址
     *
     * 该设备7位地址固定为 0x5f
     */
    uint8_t i2c_addr;

    /** \brief 平台初始化回调函数（回调注册触发引脚已被系统使用） */
    void (*pfunc_plfm_init)(void);

} awbl_hts221_devinfo_t;

/**
 * \brief HTS221 湿度和温度校准值保存
 */
typedef struct awbl_hts221_calibration_data {
    int16_t   x0;
    int8_t    y0;
    int16_t   x1;
    int8_t    y1;
} awbl_hts221_calibration_data_t;

/**
 * \brief HTS221 设备结构体
 */
typedef struct awbl_hts221_dev {

    /** \brief 继承自 AWBus 设备 */
    struct awbl_dev                      dev;

    /** \brief I2C 从机设备 */
    aw_i2c_device_t                      i2c_dev;

    /** \brief sensor 服务  */
    awbl_sensor_service_t                sensor_serv;

    /** \brief 分别保存湿度和温度的校准值 */
    awbl_hts221_calibration_data_t       cal_val[2];

    /** \brief 数据准备就绪触发回调函数 */
    aw_sensor_trigger_cb_t               pfn_trigger_cb[2];

    /** \brief 数据准备就绪触发回调函数参数 */
    void                                *p_arg[2];

    /** \brief 触发标志位 */
    uint8_t                              flags[2];

    /** \brief 中断延时处理任务 */
    struct aw_isr_defer_job              g_myjob;

    /** \brief 采样频率，每秒采样的次数 */
    aw_sensor_val_t                      sampling_rate;

    /** \brief 记录标志位 */
    uint8_t                              trigger;

    /** \brief 数据准备就绪的值保存 */
    aw_sensor_val_t                      current_data[2];

} awbl_hts221_dev_t;

/*
 * \brief 注册 HTS221 驱动
 */
void awbl_hts221_drv_register (void);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_HTS221_H */

/* end of file */
