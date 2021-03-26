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
 * \brief LTR-553ALS-01 (光照强度及近距离传感器) 头文件
 *
 * \internal
 * \par Modification History
 * - 1.00 18-10-18  vir, first implementation
 * \endinternal
 */

#ifndef __AWBL_LTR553_H
#define __AWBL_LTR553_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aworks.h"
#include "awbus_lite.h"
#include "awbl_i2cbus.h"
#include "awbl_sensor.h"


#define AWBL_LTR553_NAME      "awbl_ltr553"

/*
 * \name ltr553_integration_time
 */
#define LTR553_INTEGRATION_TIME_100MS   0X00    /*< \brief 100ms */
#define LTR553_INTEGRATION_TIME_50MS    0X01    /*< \brief 50ms */
#define LTR553_INTEGRATION_TIME_200MS   0X02    /*< \brief 200ms */
#define LTR553_INTEGRATION_TIME_400MS   0X03    /*< \brief 400ms */
#define LTR553_INTEGRATION_TIME_150MS   0X04    /*< \brief 150ms */
#define LTR553_INTEGRATION_TIME_250MS   0X05    /*< \brief 250ms */
#define LTR553_INTEGRATION_TIME_300MS   0X06    /*< \brief 300ms */
#define LTR553_INTEGRATION_TIME_350MS   0X07    /*< \brief 350ms */

/*
 * \name ltr553_gain
 */
#define LTR553_GAIN_1X                  0X00    /*< \brief 1     ~ 64k lx*/
#define LTR553_GAIN_2X                  0X01    /*< \brief 0.5   ~ 32k lx*/
#define LTR553_GAIN_4X                  0X02    /*< \brief 0.25  ~ 16k lx*/
#define LTR553_GAIN_8X                  0X03    /*< \brief 0.125 ~ 8k  lx*/
#define LTR553_GAIN_48X                 0X06    /*< \brief 0.02  ~ 1.3klx*/
#define LTR553_GAIN_96X                 0X07    /*< \brief 0.01  ~ 600 lx*/

/**
 * \brief LTR553 设备信息结构体
 */
typedef struct awbl_ltr553_devinfo {
    /**
     *  \brief 该传感器光照度采集的通道启示id
     *
     *  LTR553 共有2路光照度采集通道，从start_id开始(包括start_id)，
     *  其中 start_id 对应模块的通道1 ，  start_id + 1 对应模块的 通道2.
     *  eg： 将start_id 配置为  0 , 则 ID = 0 对应模块的通道1， ID = 1，
     *  对应模块的通道 2.
     */
    int32_t     start_id;

    /** \brief 报警引脚 */
    int32_t     alert_pin;

    /** \brief 取样窗口时间  */
    int32_t     als_integration_time;

    /** \brief I2C 7位 设备地址，固定为0x23(占高7位) */
    uint8_t     i2c_addr;
    
    /** \brief ALS增益 */
    uint8_t     als_gain;               
} awbl_ltr553_devinfo_t;


/**
 * \brief TPS02R 设备结构体
 */
typedef struct awbl_ltr553_dev {
    /** \brief 继承自 AWBus 设备 */
    struct awbl_i2c_device          dev;

    /** \brief sensor 服务  */
    awbl_sensor_service_t           sensor_serv;

    /** \brief ALS门限触发回调函数*/
    aw_sensor_trigger_cb_t          pfn_als_trigger_cb;

    /** \brief ALS门限触发回调函数参数 */
    void                           *p_als_arg;

    /** \brief ALS增益 */
    int32_t                         als_meas_time;

    /** \brief ALS低门限报警值 */
    int32_t                         als_thres_low;

    /** \brief ALS高门限报警值 */
    int32_t                         als_thres_up;

    /** \brief PS低门限报警值 */
    int32_t                         ps_thres_low;

    /** \brief ALS高门限报警值 */
    int32_t                         ps_thres_up;

    /** \brief PS门限触发回调函数*/
    aw_sensor_trigger_cb_t          pfn_ps_trigger_cb;

    /** \brief PS门限触发回调函数参数 */
    void                           *p_ps_arg;

    /** \brief ALS PS模式 */
    uint8_t                         als_ps_mode;

    /** \brief 报警通道 */
    uint8_t                         alert_chan;
} awbl_ltr553_dev_t;

/*
 * \brief 注册 LTR553 驱动
 */
void awbl_ltr553_drv_register (void);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_LTR553_H */

/* end of file */

