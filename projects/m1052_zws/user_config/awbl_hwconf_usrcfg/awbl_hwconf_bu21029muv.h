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

#ifndef __AWBL_HWCONF_BU21029MUV_H
#define __AWBL_HWCONF_BU21029MUV_H

#ifdef AW_DEV_BU21029MUV

#include "aw_gpio.h"
#include "aw_ts.h"

#include "driver/input/touchscreen/awbl_bu21029muv.h"
#include "imx1050_pin.h"

/* configure infomation */
aw_local struct awbl_bu21029muv_cfg __g_bu21029cfg={
        /*.dev_addr = */ 0x40,
        /*.rst_pin  = */ -1,
        /*.int_pin  = */ GPIO1_11,
        {
         "bu21029muv",                 /**< \brief 触摸关联的显示屏名字       */
         0x00,                         /**< \brief 触摸关联的显示屏产商ID */
         0x00,                         /**< \brief 触摸关联的显示屏产品ID */
         5,                            /**< \brief 支持最多触摸点的个数.    */
         3,                            /**< \brief 采样个数, 最小值为1  */
         1,                            /**< \brief 滤波个数, 不能大于采样个数, 最小值为1 */
         10,                           /**< \brief 消抖时长      */
         10,                           /**< \brief 采样门限阀值,用于消抖,根据关联不同的显示屏可做适配调整. */
         5,                            /**< \brief 校准门限阀值,用于验证校准系数,根据关联不同的显示屏可做适配调整. */
         AW_TS_LIB_FIVE_POINT,         /**< \brief 五点校准算法   */
        },
};

/* 设备实例内存静态分配 */
aw_local struct awbl_bu21029muv __g_bu21029muv;

#define AWBL_HWCONF_BU21029MUV       \
    {                                \
        AWBL_TOUCHSCREEN_NAME,       \
        0,                           \
        AWBL_BUSID_I2C,              \
        IMX1050_LPI2C1_BUSID,        \
        &(__g_bu21029muv.super),     \
        &(__g_bu21029cfg)            \
    },
#else

#define AWBL_HWCONF_BU21029MUV

#endif /* AW_DEV_BU21029MUV */

#endif /* __AWBL_HWCONF_BU21029MUV_H */

