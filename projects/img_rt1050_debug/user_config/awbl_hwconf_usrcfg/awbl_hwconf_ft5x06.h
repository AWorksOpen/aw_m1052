/*******************************************************************************
*                                 AnyWhere
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      anywhere.support@zlg.cn
*******************************************************************************/
/**
 * \file
 * \brief 触摸屏控制器驱动配置信息
 *
 * \internal
 * \par modification history:
 * - 1.01 16-08-18  anu, first implemetation
 * \endinternal
 */

#ifndef __AWBL_HWCONF_FT5X06_H
#define __AWBL_HWCONF_FT5X06_H

#ifdef AW_DEV_FT5X06

#include "aw_gpio.h"

#include "imx1050_pin.h"
#include "driver/input/touchscreen/awbl_ft5x06_ts.h"
#include "aw_ts.h"


/* configure infomation */
aw_local struct awbl_ft5x06_ts_devinfo __g_ft5x06_ts_devinfo={
        FT5x06_ADDRESS,
        -1,
        GPIO1_11,
        {
            "ft5x06",                    /**< \brief 触摸关联的显示屏名字       */
            0,                            /**< \brief 触摸关联的显示屏产商ID */
            0,                            /**< \brief 触摸关联的显示屏产品ID */
            5,                            /**< \brief 支持最多触摸点的个数.    */
            3,                            /**< \brief 采样个数, 最小值为1  */
            1,                            /**< \brief 滤波个数, 不能大于采样个数, 最小值为1 */
            30,                           /**< \brief 消抖时长      */
            40,                           /**< \brief 采样门限阀值,用于消抖,根据关联不同的显示屏可做适配调整. */
            5,                            /**< \brief 校准门限阀值,用于验证校准系数,根据关联不同的显示屏可做适配调整. */
            AW_TS_LIB_FIVE_POINT,         /**< \brief 五点校准算法   */
        },
};

/* 设备实例内存静态分配 */
aw_local struct awbl_ft5x06_ts_dev __g_ft5x06_ts;

#define AWBL_HWCONF_FT5X06              \
    {                                   \
        AWBL_FT5X06_TOUCHSCREEN_NAME,   \
        0,                              \
        AWBL_BUSID_I2C,                 \
        IMX1050_LPI2C1_BUSID,           \
        &(__g_ft5x06_ts.super),         \
        &(__g_ft5x06_ts_devinfo)        \
    },
#else

#define AWBL_HWCONF_FT5X06

#endif /* AW_DEV_FT5X06 */

#endif /* __AWBL_HWCONF_FT5X06_H */

