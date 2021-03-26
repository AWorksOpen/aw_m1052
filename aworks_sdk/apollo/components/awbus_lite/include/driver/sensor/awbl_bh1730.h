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
 * \brief BH1730 (光照强度及近距离传感器) 头文件
 *
 * \internal
 * \par Modification History
 * - 1.00 18-10-18  vir, first implementation
 * \endinternal
 */

#ifndef __AWBL_BH1730_H
#define __AWBL_BH1730_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aworks.h"
#include "awbus_lite.h"
#include "awbl_i2cbus.h"
#include "awbl_sensor.h"


#define AWBL_BH1730_NAME      "awbl_bh1730"

/*
 * \name bh1730 gain
 */
#define BH1730_GAIN_1X        0X00    /**< \brief 100k lx */
#define BH1730_GAIN_2X        0X01    /**< \brief 50k lx */
#define BH1730_GAIN_64X       0X02    /**< \brief 1.5k lx */
#define BH1730_GAIN_128X      0X03    /**< \brief 780 lx */
/**
 * \brief BH1730 设备信息结构体
 */
typedef struct awbl_bh1730_devinfo {
    /**
     *  \brief 该传感器光照度采集的通道启示id
     *
     *  BH1730 共有2路光照度采集通道，从start_id开始(包括start_id)，
     *  其中 start_id 对应模块的通道1 ，start_id + 1 对应模块的通道2.
     *  eg： 将start_id 配置为  0 , 则 ID = 0 对应模块的通道1， ID = 1，
     *  对应模块的通道 2.
     */
    int     start_id;

    /** \brief 报警引脚 */
    int     alert_pin;     

    /*
     * \brief I2C 7位 设备地址，固定为0x23(占高7位)
     */
    uint8_t i2c_addr;

    /** \brief 增益，影响光照度的计算值 */
    uint8_t                         gain;
} awbl_bh1730_devinfo_t;


/**
 * \brief TPS02R 设备结构体
 */
typedef struct awbl_bh1730_dev {
    /** \brief 继承自 AWBus 设备 */
    struct awbl_i2c_device          dev;

    /** \brief sensor 服务  */
    awbl_sensor_service_t           sensor_serv;

    struct aw_isr_defer_job         defer_job;

    /** \brief 门限触发回调函数*/
    aw_sensor_trigger_cb_t          pfn_trigger_cb;

    /** \brief 门限触发回调函数参数 */
    void                           *p_arg;

    /** \brief 门限触发下限值 */
    aw_sensor_val_t                 thres_low;

    /** \brief 门限触发上限值 */
    aw_sensor_val_t                 thres_up;

    /** \brief 测量时间，影响光照度的计算值 */
    int32_t                         itime_ms;

    /** \brief 当前模式  */
    uint8_t                         mode;
} awbl_bh1730_dev_t;

/*
 * \brief 注册 BH1730 驱动
 */
void awbl_bh1730_drv_register (void);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_BH1730_H */

/* end of file */


