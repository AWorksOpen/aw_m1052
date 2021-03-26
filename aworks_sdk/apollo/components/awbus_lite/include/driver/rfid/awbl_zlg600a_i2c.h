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
 * \brief AWBus ZLG600A UART接口头文件
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * #include "awbl_zlg600a.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 16-05-03  win, first implementation
 * \endinternal
 */

#ifndef __AWBL_ZLG600A_I2C_H
#define __AWBL_ZLG600A_I2C_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_awbl_drv_zlg600a_i2c
 * \copydetails awbl_zlg600a_i2c.h
 * @{
 */

/**
 * \defgroup  grp_awbl_zlg600a_i2c_impl 实现相关
 * \todo
 *
 */

/**
 * \defgroup  grp_awbl_zlg600a_i2c_hwconf 设备定义/配置
 * \todo
 */

/** @} grp_awbl_drv_zlg600a_i2c */

#include "awbl_zlg600a.h"
#include "awbl_i2cbus.h"

/** \brief PCF8563 驱动名 */
#define AWBL_ZLG600A_I2C_NAME   "zlg600a_i2c"

/**
 * \brief ZLG600A I2C设备信息
 */
typedef struct awbl_zlg600a_i2c_devinfo {

    /** \brief ZLG600A 服务配置信息  */
    struct awbl_zlg600a_servinfo zlg600a_servinfo;

    int pin;

    uint8_t   frame_fmt;    /**< \brief 默认使用的帧格式 */
    uint8_t   addr;         /**< \brief 默认传输的地址 */
    uint8_t   mode;         /**< \brief 通信模式 */

    /** \brief 超时（单位ms） */
    int       timeout;

    /** 平台相关初始化：开启时钟、初始化引脚等操作 */
    void (*pfunc_plfm_init)(void);

} awbl_zlg600a_i2c_devinfo_t;

/**
 * \brief ZLG600A I2C设备结构体定义
 */
typedef struct awbl_zlg600a_i2c_dev {

    /** \brief 继承自 AWBus I2C device 设备 */
    struct awbl_i2c_device super;

    /** \brief 卡片服务结构体 */
    awbl_zlg600a_service_t serv;

    uint8_t  addr;         /**< \brief 默认传输的地址      */

    AW_MUTEX_DECL(devlock);  /**< \brief 设备锁 */

    /** \brief 信号量 */
    AW_SEMB_DECL(semb);

} awbl_zlg600a_i2c_dev_t;

/**
 * \brief ZLG600A I2C driver register
 */
void awbl_zlg600a_i2c_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_ZLG600A_I2C_H */

/* end of file */
