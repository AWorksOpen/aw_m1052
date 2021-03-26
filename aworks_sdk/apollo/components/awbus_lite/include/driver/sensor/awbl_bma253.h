/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/
/**
 * \file
 * \brief bma253三轴加速度传感器设备
 *
 * \internal
 * \par modification history:
 * - 2018-10-24 cat, first implementation.
 * \endinternals
 */

#ifndef __AWBL_BMA253_H
#define __AWBL_BMA253_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_sensor.h"
#include "awbl_sensor.h"
#include "aw_i2c.h"
#include "awbl_i2cbus.h"
#include "aw_task.h"
#include "aw_sem.h"

#define AWBL_BMA253_NAME        "awbl_bma253"  /**< \brief 设备名 */

/**
 * \brief 数据输出频率（通道采样频率）
 */
#define AW_BMA253_15_63_HZ        (0)
#define AW_BMA253_31_25_HZ        (1)
#define AW_BMA253_62_5_HZ         (2)
#define AW_BMA253_125_HZ          (3)
#define AW_BMA253_250_HZ          (4)
#define AW_BMA253_500_HZ          (5)
#define AW_BMA253_1000_HZ         (6)
#define AW_BMA253_2000_HZ         (7)

/**
 * \brief 通道量程值
 */
#define AW_BMA253_2G              (0)
#define AW_BMA253_4G              (1)
#define AW_BMA253_8G              (2)
#define AW_BMA253_16G             (3)

/**
 * \brief bma253 数据结构体
 */
struct awbl_bma253_data {

    int16_t  accel_x;                     /**< \brief 加速度计X轴 */
    int16_t  accel_y;                     /**< \brief 加速度计Y轴 */
    int16_t  accel_z;                     /**< \brief 加速度计Z轴 */
    int16_t  temp;                        /**< \brief 温度 */
};

/**
 * \brief sensor-bma253 设备信息
 */
typedef struct awbl_sensor_bma253_devinfo {

    uint8_t start_id;                  /**< \brief 传感器通道起始ID */

    uint8_t addr;                      /**< \brief 设备从机地址 */

    uint8_t int1_port;                 /**< \brief 中断1引脚号 */
    uint8_t int2_port;                 /**< \brief 中断2引脚号 */

    void (*pfunc_plfm_init) (void);    /**< \brief 平台初始化回调函数 */

}awbl_sensor_bma253_devinfo_t;

/**
 * \brief sensor-bma253 设备实例
 */
typedef struct awbl_sensor_bma253_dev {

    /**< \brief 继承自 AWBus I2C 从机实例 */
    struct awbl_i2c_device    super;

    /**< \brief sensor 设备服务  */
    struct awbl_sensor_service  sensor_serv;

    /**< \brief 通道数据 */
    struct awbl_bma253_data    bma253_val;

    /**< \brief bma253从设备结构 */
    aw_i2c_device_t    i2c_dev;

    /**< \brief bma253的数据输出速度（采样频率） */
    uint8_t   sample_rate;  /* 默认值 ：2000Hz */

    /**< \brief bma253的检测范围 */
    uint8_t   sample_range;  /* 默认值 ：±2g */

    /**< \brief bma253通道上门限 */
    uint8_t    high_th;

    /**< \brief bma253通道下门限 */
    uint8_t    low_th;

    /**< \brief bma253 z通道中断开启状态 */
    uint8_t    accel_z_int[2];

    /**< \brief bma253通道门限触发设置 */
    uint8_t    high_trigger_set[3];

    /**< \brief bma253下门限触发使能 */
    uint8_t    low_trigger_set;

    /**< \brief bma253的通道补偿值 */
    uint32_t   offset[3];

    /**< \brief bma253通道触发回调函数 */
    aw_sensor_trigger_cb_t pfn_accel_trigger_cb[3];

    /**< \brief bma253下门限触发回调函数 */
    aw_sensor_trigger_cb_t pfn_low_trigger_cb;

    /**< \brief 门限触发和新数据就绪触发回调函数参数 */
    void      *p_arg[4];

} awbl_sensor_bma253_dev_t;

/**
 * \brief sensor-bma253 driver register
 */
void awbl_bma253_drv_register (void);

/**
 * \brief 配置bma253下门限触发
 *
 * \param[in] dev_id   设备单元号
 * \param[in] p_low_th bma253的下门限值
 * \param[in] pfn_cb   下门限触发的回调函数
 * \param[in] p_arg    回调函数参数
 *
 * \retval AW_OK 配置成功
 * \retval  <0   配置失败
 */
aw_err_t awbl_bma253_low_trigger_cfg (int                      dev_id,
                                      const aw_sensor_val_t   *p_val,
                                      aw_sensor_trigger_cb_t   pfn_cb,
                                      void                    *p_arg);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_BMA253_H */


