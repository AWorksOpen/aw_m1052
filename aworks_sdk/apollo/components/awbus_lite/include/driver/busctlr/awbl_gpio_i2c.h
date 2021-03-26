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
 * \brief AWBus GPIO I2C 驱动头文件
 *
 * 本驱动使用 Apollo 标准GPIO接口来模拟I2C主机功能
 *
 * \par 1.驱动信息
 *
 *  - 驱动名:   "gpio_i2c"
 *  - 总线类型: AWBL_BUSID_PLB
 *  - 设备信息: struct awbl_gpio_i2c_devinfo
 *
 * \par 2.兼容设备
 *
 *  - 任何实现Apollo标准GPIO接口的设备
 *
 * \par 3.设备定义/配置
 *
 *  - \ref grp_awbl_gpio_i2c_hwconf
 *
 * \par 4.用户接口
 *
 *  - \ref grp_aw_serv_i2c
 *
 * \internal
 * \par modification history:
 * - 1.00 13-08-26  zen, first implementation
 * \endinternal
 */

#ifndef __AWBL_GPIO_I2C_H
#define __AWBL_GPIO_I2C_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_awbl_drv_gpio_i2c
 * \copydetails awbl_gpio_i2c.h
 * @{
 */

/**
 * \defgroup  grp_awbl_gpio_i2c_hwconf 设备定义/配置
 *  //TODO
 */

/** @} grp_awbl_drv_gpio_i2c */

#include "aw_task.h"
#include "aw_sem.h"
#include "aw_spinlock.h"
#include "awbl_i2cbus.h"

#define AWBL_GPIO_I2C_NAME   "gpio_i2c"

/**
 * \brief GPIO I2C 设备信息
 */
struct awbl_gpio_i2c_devinfo {

    /** \brief 继承自 AWBus I2C 控制器设备信息 */
    struct awbl_i2c_master_devinfo i2c_master_devinfo;

    int      pin_scl;               /**< \brief SCL 引脚编号 */
    int      pin_sda;               /**< \brief SDA 引脚编号 */

    /**  
     * \brief 总线速度,通常配合逻辑分析仪来确定实际的总线速度 
     */
    unsigned speed_exp;             

    void (*pfunc_sda_set_input)  (struct awbl_gpio_i2c_devinfo *);
    void (*pfunc_sda_set_output) (struct awbl_gpio_i2c_devinfo *);

    /**
     * \brief 平台相关初始化：开启时钟、初始化引脚
     */
    void (*pfunc_plfm_init)(void); 
};

/**
 * \brief GPIO I2C 设备实例
 */
struct awbl_gpio_i2c_dev {

    /** \brief 继承自 AWBus I2C 主机控制器 */
    struct awbl_i2c_master  super;

    /** \brief 码间超时时间 */
    uint32_t    timeout;

    /** \brief 总线空闲时间 us */
    uint32_t    bus_idle_time;

    /** \brief 使用aw_udelay进行的延时 */
    uint32_t    speed;
};

/**
 * \brief GPIO I2C master driver register
 */
void awbl_gpio_i2c_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_GPIO_I2C_H */

/* end of file */

