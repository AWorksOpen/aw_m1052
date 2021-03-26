/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief MMC5883MA (三轴磁传感器) 头文件
 *
 * \internal
 * \par Modification History
 * - 1.00 18-11-30  ipk, first implementation
 * \endinternal
 */

#ifndef __AWBL_MMC5883MA_H
#define __AWBL_MMC5883MA_H

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
#include "aw_timer.h"

#define AWBL_MMC5883MA_NAME      "awbl_mmc5883ma"

/**
 * \brief MMC5883MA 设备信息结构体
 */
typedef struct awbl_mmc5883ma_devinfo {
    /**
     * \brief 该传感器通道的起始id
     *
     * MMC5883MA 共有4路采集通道,分别为X轴，Y轴，Z轴磁感应强度和一路温度数据，从
     * start_id开始(包括start_id)连续四个ID分配给该模块的四个通道，其中
     * start_id 对应模块X轴磁感应强度，start_id + 1 对应模块的Y轴磁感应强度
     * eg： 将start_id配置为0, 则ID = 0,对应模块的通道1，ID = 1对应模块的通道2.
     */
    int     start_id;

    /** \brief I2C从机地址,7位地址 */
    uint8_t i2c_addr;

    /** \brief 中断触发引脚 */
    int     tergger_pin;

    /** \brief 平台初始化 */
    void   (* pfn_plfm_init)(void);


} awbl_mmc5883ma_devinfo_t;

/**
 * \brief MMC5883MA 设备结构体
 */
typedef struct awbl_mmc5883ma_dev {

    /** \brief 继承自 AWBus 设备 */
    struct awbl_dev         dev;

    /** \brief I2C从机设备  */
    aw_i2c_device_t         i2c_dev;

    /** \brief sensor 服务  */
    awbl_sensor_service_t   sensor_serv;

    /** \brief 软件定时器，用于对温度通道定时采集 */
    aw_timer_t              timer;

    /** \brief 通道使能，对应位为1时表示该通道已使能 (4通道，0~3bit有效)*/
    uint8_t                 channel;

    /** \brief 通道触发使能 ，对应位为1时表示该通道已使能 (4通道，0~3bit有效)*/
    uint8_t                 ch_trigger;

    /** \brief 记录控制寄存器1的值 */
    uint8_t                 ctrl1_reg_buf;

    /** \brief 记录控制寄存器2的值 */
    uint8_t                 ctrl2_reg_buf;

    /** \brief 每秒采样次数（采样频率） */
    aw_sensor_val_t         sampling_rate[2];

    /** \brief X,Y,Z通道的上限阈值 */
    aw_sensor_val_t         threshold[3];

    /** \brief 中断延迟作业 */
    struct aw_isr_defer_job defer_job[2];

    /** \brief 通道触发回调函数 */
    aw_sensor_trigger_cb_t  pfn_trigger_cb[4];

    /** \brief 通道触发回调函数参数 */
    void                   *p_argc[4];

    /** \brief 通道触发标志 */
    uint32_t                flags[4];

} awbl_mmc5883ma_dev_t;


/*
 * \brief 注册 MMC5883MA 驱动
 */
void awbl_mmc5883ma_drv_register (void);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_MMC5883MA_H */

/* end of file */
