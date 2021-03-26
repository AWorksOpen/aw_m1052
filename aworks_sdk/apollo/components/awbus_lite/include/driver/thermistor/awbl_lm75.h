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
 * \brief LM75温度传感器驱动
 *

 *
 * \par 1.驱动信息
 *
 *  - 驱动名:   "lm75_temperature"
 *  - 总线类型: AWBL_BUSID_I2C
 *  - 设备信息: struct awbl_lm75_par
 *
 * \par 2.兼容设备
 *
 * \par 3.设备定义/配置
 *
 *  - \ref grp_awbl_lm75_hwconf
 *
 * \par 4.用户接口
 *
 *  - \ref grp_aw_serv_lm75
 *
 * \internal
 * \par modification history
 * - 1.00 14-07-30  hbt, first implementation.
 * \endinternal
 */

#ifndef __AWBL_LM75_H
#define __AWBL_LM75_H

#include "apollo.h"
#include "awbus_lite.h"
#include "aw_common.h"
#include "aw_types.h"
#include "aw_i2c.h"

#include "awbl_temp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_awbl_drv_lm75
 * @{
 */

/**
 * \defgroup  grp_awbl_lm75_hwconf 设备定义/配置
 *
 *
 * \include  awbl_lm75.c
 *
 */

/** @} */


/** \brief lm75温度传感器设备信息 */
struct awbl_lm75_par {

    uint32_t   id;                   /**< \brief 设备id号 */

    uint32_t   slave_addr;           /**< \brief 从机设备地址 */
};

/** \brief lm75温度传感器设备实例 */
struct awbl_lm75_dev {

    /** \brief 继承自 AWBus 设备 */
    struct awbl_dev dev;

    /** \brief lm75从机设备结构体*/
    aw_i2c_device_t lm75;

    /** \brief  温度传感器服务 */
    struct awbl_temp_service  temp_serv;
};

#define LM75_TEMP_NAME    "lm75_temperature"

#define SERV_GET_LM75_DEV(p_serv)   \
        (AW_CONTAINER_OF(p_serv, struct awbl_lm75_dev, temp_serv))

#define __SERV_TO_DEV(p_serv)   \
        ((AW_CONTAINER_OF(p_serv, struct awbl_lm75_dev, temp_serv))->dev)

#define SERV_GET_LM75_PAR(p_serv) \
        ((struct awbl_lm75_par *)AWBL_DEVINFO_GET(&__SERV_TO_DEV(p_serv)))

#define DEV_TO_LM75_DEV(p_dev)  \
        (AW_CONTAINER_OF(p_dev, struct awbl_lm75_dev, dev))

#define LM75_DEV_GET_PAR(p_therm_dev)  \
        ((struct awbl_lm75_par *)((p_therm_dev)->dev.p_devhcf->p_devinfo))

#define DEV_GET_LM75_PAR(p_dev)   ((void *)AWBL_DEVINFO_GET(p_dev))

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_LM75_H */

/* end of file */
