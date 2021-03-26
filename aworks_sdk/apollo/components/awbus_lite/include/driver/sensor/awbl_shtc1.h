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
 * \brief SHTC1 (测温湿度模块) 头文件
 *
 * \internal
 * \par Modification History
 * - 1.00 18-10-23  wan, first implementation
 * \endinternal
 */

#ifndef __AWBL_SHTC1_H
#define __AWBL_SHTC1_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aworks.h"
#include "awbus_lite.h"
#include "awbl_i2cbus.h"
#include "aw_sensor.h"
#include "awbl_sensor.h"

#define AWBL_SHTC1_NAME      "awbl_shtc1"
    
/**
 * \brief SHTC1 设备信息结构体
 */
typedef struct awbl_shtc1_devinfo {

    /**
     * \brief 该传感器通道的起始id
     *
     * SHTC1 共有1路湿度采集通道和1路温度采集通道,从start_id开始(包括start_id)
     * 连续两个ID分配给该模块的两个通道，其中 start_id 对应模块的湿度传感器通道，
     * start_id + 1 对应模块的温度传感器通道
     * eg：将start_id 配置为0, 则 ID = 0,对应模块的通道1，ID = 1对应模块的通道2.
     */
    int     start_id;

    /*
     * \brief I2C 7位 设备地址
     *
     * 该设备7位地址固定为 0x70
     */
    uint8_t i2c_addr;

} awbl_shtc1_devinfo_t;

/**
 * \brief SHTC1 设备结构体
 */
typedef struct awbl_shtc1_dev {

    /** \brief 继承自 AWBus 设备 */
    struct awbl_dev                      dev;

    /** \brief I2C 从机设备 */
    aw_i2c_device_t                      i2c_dev;

    /** \brief sensor 服务  */
    awbl_sensor_service_t                sensor_serv;

} awbl_shtc1_dev_t;

/*
 * \brief 注册 SHTC1 驱动
 */
void awbl_shtc1_drv_register (void);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_SHTC1_H */

/* end of file */
