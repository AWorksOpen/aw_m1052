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
 * \brief 三轴陀螺仪传感器 BMG160 头文件
 *
 * \internal
 * \par modification history
 * - 1.00 18-12-11  ipk, first implementation.
 * \endinternal
 */
 
#ifndef __AW_SENSOR_BMG160_H
#define __AW_SENSOR_BMG160_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_i2c.h"
#include "awbl_sensor.h"
#include "aw_isr_defer.h"
    

#define AWBL_BMG160_NAME      "awbl_bmg160"

/**
 * \brief 传感器 BMG160 设备信息结构体
 */
typedef struct awbl_bmg160_devinfo {
    /**
     *  \brief 该传感器光照度采集的通道启示id
     *
     *  BH1730 共有3路角速度采集通道，从start_id开始(包括start_id)，
     *  其中 start_id 对应模块的通道1.
     *  eg： 将start_id 配置为  0 , 则 ID = 0 对应模块的通道1.
     *
     *  每个通道读取到的数据单位是: °/S
     */
    int     start_id;

    /**
     * \brief 数据准备就绪触发引脚 INT1
     */
    int     trigger_pin;

    /*
     * \brief I2C 7位 设备地址
     *
     * 该设备7位地址固定为 0x68
     */
    uint8_t i2c_addr;

} awbl_bmg160_devinfo_t;

/****************************** 校准值待处理************************************/

/**
 * \breif 传感器 BMG160 设备结构体定义
 */
typedef struct awbl_bmg160_dev {

    /** \brief 继承自 AWBus 设备 */
    struct awbl_dev                bmg160_dev;

    /** \brief I2C从机设备  */
    aw_i2c_device_t                i2c_dev;

    /** \brief sensor 服务  */
    awbl_sensor_service_t          sensor_serv;

    void                          *p_arg[3];     /**< \brief 触发回调函数参数 */
    uint8_t                        flags[3];     /**< \brief 触发标志位       */
    struct aw_isr_defer_job        g_myjob;      /**< \brief 中断延时处理任务 */
    aw_sensor_val_t                range;        /**< \brief 量程         */
    uint8_t                        trigger;      /**< \brief 记录标志位       */
    aw_sensor_val_t                data[3];      /**< \brief 内部数据缓存     */
    /** \brief 数据准备就绪触发回调函数 */
    aw_sensor_trigger_cb_t         pfn_trigger_cb[3];

} awbl_bmg160_dev_t;

/*
 * \brief 注册 BMG160 驱动
 */
void awbl_bmg160_drv_register (void);


#ifdef __cplusplus
}
#endif

#endif /* __AW_SENSOR_BMG160_H */

/* end of file */
