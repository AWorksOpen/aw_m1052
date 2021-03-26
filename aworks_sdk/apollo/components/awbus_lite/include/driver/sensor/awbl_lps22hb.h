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
 * \brief LPS22HB 气压传感器驱动
 *

 *
 * \par 1.驱动信息
 *
 *  - 驱动名:   "lps22hb_pressure"
 *  - 总线类型:   AWBL_BUSID_I2C
 *  - 设备信息:   struct awbl_lps22hb_par
 *
 * \par 2.兼容设备
 *
 * \par 3.设备定义/配置
 *
 *  - \ref grp_awbl_lps22hb_hwconf
 *
 * \par 4.用户接口
 *
 *  - \ref grp_aw_serv_lps22hb
 *
 * \internal
 * \par modification history
 * - 1.00 18-10-21  dsg, first implementation.
 * \endinternal
 */

#ifndef __AWBL_LPS22HB_H
#define __AWBL_LPS22HB_H

/**
 * \addtogroup grp_awbl_if_lps22hb
 * \copydoc awbl_lps22hb.h
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
#include "aw_sys_defer.h"
#include "aw_sem.h"
#include "aw_int.h"
#include "aw_isr_defer.h"

#define AWBL_LPS22HB_NAME "sensor-lps22hb"

#define AW_LPS22HB_TRIGGER_FIFO  255

/**
 * \brief 传感器采样速率
 */
enum sampling_rate {
    AW_LPS22HB_OUTPUT_DATA_1Hz  = 1,
    AW_LPS22HB_OUTPUT_DATA_10Hz = 2,
    AW_LPS22HB_OUTPUT_DATA_25Hz = 3,
    AW_LPS22HB_OUTPUT_DATA_50Hz = 4,
    AW_LPS22HB_OUTPUT_DATA_75Hz = 5,
};

/* LPS22HB 传感器设备结构体 */
typedef struct awbl_lps22hb_param {
    uint8_t   start_id;             /**< \brief 传感器起始ID (通道) */
    uint16_t  alert_pin;            /**< \brief 报警引脚 */
    uint16_t  addr;                 /**< \brief 传感器从机地址 */

    void    (*pfn_plfm_init)(void); /**< \brief 平台初始化函数 */
} awbl_lps22hb_param_t;

/* LPS22HB 传感器设备信息结构体 */
typedef struct awbl_lps22hb_dev {
    /** \brief 继承自 AWBus 设备 */
    awbl_dev_t              dev;

    /** \brief sensor 服务  */
    awbl_sensor_service_t   sensor_serv;

    /** \brief lps22hb从设备结构 */
    aw_i2c_device_t         i2c_lps22hb;

    /** \brief 回调 函数*/
    aw_sensor_trigger_cb_t  pfn_trigger_cb;

    /** \brief 回调函数参数 */
    void                   *p_arg;

    /** \brief 门限值 */
    aw_sensor_val_t         threshold;

    /** \brief 门限模式 */
    uint8_t                 threshold_mod;

    /** \brief 报警通道 */
    uint8_t                 alert_chan;

    /** \brief 采样频率，每秒采样的次数 */
    uint8_t                 sampling_rate;

    /** \brief FIFO 模式 */
    uint8_t                 fifo_mode;

    /** \brief FIFO 水印标志 */
    uint8_t                 watermark;

    /** \brief 数据准备就绪标志 */
    uint8_t                 data_ready;

    /** \brief 数据缓存区*/
    int32_t                 data_cache[2];

    /** \brief 延迟对象节点 */
    struct aw_isr_defer_job g_djob;

} awbl_lps22hb_dev_t;

/**
 * \brief 注册 sensor-lps22hb 驱动
 */
void awbl_lps22hb_drv_register (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AWBL_LPS22HB_H */
