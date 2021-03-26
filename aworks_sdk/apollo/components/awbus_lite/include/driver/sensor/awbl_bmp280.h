/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief BMP280 气压传感器驱动
 *

 *
 * \par 1.驱动信息
 *
 *  - 驱动名:   "bmp280_pressure"
 *  - 总线类型:   AWBL_BUSID_I2C
 *  - 设备信息:   struct awbl_bmp280_par
 *
 * \par 2.兼容设备
 *
 * \par 3.设备定义/配置
 *
 *  - \ref grp_awbl_bmp280_hwconf
 *
 * \par 4.用户接口
 *
 *  - \ref grp_aw_serv_bmp280
 *
 * \internal
 * \par modification history
 * - 1.00 18-10-16  dsg, first implementation.
 * \endinternal
 */

#ifndef __AWBL_BMP280_H
#define __AWBL_BMP280_H

/**
 * \addtogroup grp_awbl_if_bmp280
 * \copydoc awbl_bmp280.h
 * @{
 */

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

#include "apollo.h"
#include "awbus_lite.h"
#include "awbl_sensor.h"
#include "aw_sensor.h"
#include "aw_i2c.h"
#include "aw_common.h"

#define AWBL_BMP280_NAME "sensor-bmp280"

/**
 * \brief BMP280 传感器设备结构体
 */
typedef struct awbl_bmp280_param {
    uint8_t  start_id; /**< \brief 传感器起始ID (通道) */
    uint16_t addr;     /**< \brief 传感器从机地址 */
} awbl_bmp280_param_t;

/**
 * \brief bmp280 设备校准系数结构体
 */
typedef struct awbl_bmp280_cali {
    uint16_t   t1;   /**< \brief 校准系数T1 */
    int16_t    t2;   /**< \brief 校准系数T2 */
    int16_t    t3;   /**< \brief 校准系数T3 */

    uint16_t   p1;   /**< \brief 校准系数P1 */
    int16_t    p2;   /**< \brief 校准系数P2 */
    int16_t    p3;   /**< \brief 校准系数P3 */
    int16_t    p4;   /**< \brief 校准系数P4 */
    int16_t    p5;   /**< \brief 校准系数P5 */
    int16_t    p6;   /**< \brief 校准系数P6 */
    int16_t    p7;   /**< \brief 校准系数P7 */
    int16_t    p8;   /**< \brief 校准系数P8 */
    int16_t    p9;   /**< \brief 校准系数P9 */

    int32_t    up;   /**< \brief 未校准气压值 UP */
    int32_t    ut;   /**< \brief 未校准温度值 UT */
} awbl_bmp280_cali_t;

/**
 * \brief BMP280 传感器设备信息结构体
 */
typedef struct awbl_bmp280_dev {
    /** \brief 继承自 AWBus 设备 */
    awbl_dev_t              dev;

    /** \brief sensor 服务  */
    awbl_sensor_service_t   sensor_serv;

    /** \brief bmp280从设备结构 */
    aw_i2c_device_t         i2c_bmp280;

    /** \brief bmp280 设备校准数据结构体 */
    awbl_bmp280_cali_t      bmp280_cali;

    /** \brief 数据缓存区*/
    int32_t                 data_cache[2];
} awbl_bmp280_dev_t;

/**
 * \brief 注册 sensor-bmp280 驱动
 */
void awbl_bmp280_drv_register (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AWBL_BMP280_H */
