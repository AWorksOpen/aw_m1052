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
 * \brief TPS02R (测温模块) 头文件
 *
 * \internal
 * \par Modification History
 * - 1.00 18-05-11  vir, first implementation
 * \endinternal
 */

#ifndef __AWBL_TPS02R_H
#define __AWBL_TPS02R_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aworks.h"
#include "awbus_lite.h"
#include "aw_spinlock.h"
#include "awbl_i2cbus.h"
#include "aw_sensor.h"
#include "awbl_sensor.h"


#define AWBL_TPS02R_NAME      "awbl_tps02r"

/*
 * \name grp_tps02r_chan
 */
#define AW_TPS02R_CHAN_1      0    /**< \brief TPS02R 的通道1 */
#define AW_TPS02R_CHAN_2      1    /**< \brief TPS02R 的通道2 */

/**
 * \brief TPS02R 设备信息结构体
 */
typedef struct awbl_tps02r_devinfo {
    /**
     *  \brief 该传感器通道的起始id
     *
     *  TPS02R 共有2路温度采集通道，从start_id开始(包括start_id)连续两个ID分配给
     *  该模块的两个通道，其中 start_id 对应模块的通道1 ，  start_id + 1 对应
     *  模块的 通道 2
     *  eg： 将start_id 配置为  0 , 则 ID = 0 对应模块的通道1， ID = 1，对应模块
     *  的通道 2.
     */
    int     start_id;

    int     alert_pin;     /**< \brief 报警引脚 */

    /*
     * \brief I2C 7位 设备地址
     *
     *  A0引脚 接地(0x48)，接 VDD 或者悬空(0x49)
     */
    uint8_t i2c_addr;

}awbl_tps02r_devinfo_t;


/**
 * \brief TPS02R 设备结构体
 */
typedef struct awbl_tps02r_dev {
    /** \brief 继承自 AWBus 设备 */
    struct awbl_dev                      dev;

    /** \brief I2C 从机设备 */
    aw_i2c_device_t                      i2c_dev;

    /** \brief sensor 服务  */
    awbl_sensor_service_t                sensor_serv;

    /** \brief 门限触发回调 函数*/
    aw_sensor_trigger_cb_t               pfn_trigger_cb;

    /** \brief 门限触发回调函数参数 */
    void                                 *p_arg;

    /** \brief 下限温度 */
    aw_sensor_val_t                     temp_low[2];

    /** \brief 上限温度 */
    aw_sensor_val_t                     temp_high[2];

    /** \brief 触发报警次数 */
    uint8_t                             count[2];

    /** \brief 报警通道 */
    uint8_t                              alert_chan;

    /** \brief 采样频率，每秒采样的次数 */
    uint8_t                              sampling_rate;
    ;

}awbl_tps02r_dev_t;

/*
 * \brief 注册 TPS02R 驱动
 */
void awbl_tps02r_drv_register (void);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_TPS02R_H */

/* end of file */
