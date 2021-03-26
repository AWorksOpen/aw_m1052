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
 * \brief NTC 热敏电阻驱动
 *
 * 本驱动适用于电阻分压式采样电路的NTC热敏电阻驱动。
 *
 * \par 1.驱动信息
 *
 *  - 驱动名:   "ntc-nameless"
 *  - 总线类型: AWBL_BUSID_PLB
 *  - 设备信息: struct awbl_ntc_therm_par
 *
 * \par 2.兼容设备
 *
 *  - 任何使用分压电路进行电压采样的NTC热敏电阻
 *
 * \par 3.设备定义/配置
 *
 *  - \ref grp_awbl_thermistor_ntc_hwconf
 *
 * \par 4.用户接口
 *
 *  - \ref grp_aw_serv_ntc_thermistor
 *
 * \internal
 * \par modification history
 * - 1.00 14-07-30  hbt, first implementation.
 * \endinternal
 */

#ifndef __AWBL_THERM_NTC_H
#define __AWBL_THERM_NTC_H

#include "apollo.h"
#include "awbus_lite.h"
#include "aw_common.h"
#include "aw_types.h"

#include "awbl_temp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_awbl_drv_ntc_thermistor
 * @{
 */

/**
 * \defgroup  grp_awbl_ntc_thermistor_hwconf 设备定义/配置
 *
 * 通用热敏电阻设备的定义如下面的代码所示，用户在定义目标系统的硬件资源时，
 * 可以直接将这段代码嵌入到 awbus_lite_hwconf_usrcfg.c 中对应的位置，然后对设备
 * 信息进行必要的配置(可配置的项目用【x】标识)。
 *
 * \note 有多少个热敏电阻设备，就将下面的代码嵌入多少次，注意根据不同的热敏电阻
 *       的型号及连接方式修改必要的信息。
 *
 * \include  awbl_thermistor_ntc.c
 * \note 以下配置项中带【用户配置】标识的，表示用户在开发应用时可以配置此参数。
 *
 * - 【1】 
 *
 * - 【2】 热敏电阻设备信息：(1) 温度传感器ID号，使驱动与服务一一对应 \n
 *
 *                           (2) 对应热敏电阻的R-T数据表 \n
 *
 *                           (3) R-T数据表成员数 \n
 * 
 *                           (5) ADC 通道号 \n
 
 *                           (6) 采样电路连接方式 \n
 *
 *                           (7) 采样电路最高电压 \n
 *
 *                           (8) 接最高电压电阻阻值 \n
 *
 *                           (9) 接地电阻阻值 \n
 *
 *                           (10) 与平台相关初始化 \n
 */

/** @} */

/** \brief 连接方式选择 */
#define CONNECT_UP          0
#define CONNECT_DOWN        1
#define CONNECT_PARA_SER    2

/** \brief 温度与电阻值匹配 */
struct awbl_temp_ohm_match {
    int32_t   temp_c;     /**< \brief 温度值，单位 ℃ */
    int32_t   ohm;        /**< \brief 电阻值，单位 ohm */
};

/** \brief NCT 热敏电阻设备信息 */
struct awbl_ntc_therm_par {
    uint32_t                             id;     /**< \brief 设备id号 */
    aw_const struct awbl_temp_ohm_match *p_tempohm_lst;  /**< \brief 数据表 */
    size_t                               lst_count; /**< \brief 数据表中数据数目 */
    
    uint8_t   channel;               /**< \brief ADC 通道 */
    
    uint8_t   connect;               /**< \brief 连接方式 */
    int16_t   full_mv;               /**< \brief 满电压 */
    int32_t   pullup_ohm;            /**< \brief 串上拉电阻值 */
    int32_t   pulldown_ohm;          /**< \brief 串下拉电阻值 */

    void (*pfunc_plfm_init) (void);  /**< \brief 平台相关初始化 */
};

/** \brief NTC 热敏电阻设备实例 */
struct awbl_ntc_therm_dev {

    /** \brief 继承自 AWBus 设备 */
    struct awbl_dev dev;

    /** \brief  温度传感器服务 */
    struct awbl_temp_service  temp_serv;
};

#define NTC_THERMISTOR_NAME    "ntc-nameless"

#define SERV_GET_THERM_DEV(p_serv)   \
        (AW_CONTAINER_OF(p_serv, struct awbl_ntc_therm_dev, temp_serv))

#define __SERV_GET_DEV(p_serv)   \
        ((AW_CONTAINER_OF(p_serv, struct awbl_ntc_therm_dev, temp_serv))->dev)

#define SERV_GET_NTC_THERM_PAR(p_serv) \
        ((struct awbl_ntc_therm_par *)AWBL_DEVINFO_GET(&__SERV_GET_DEV(p_serv)))

#define DEV_TO_THERM_DEV(p_dev)  \
        (AW_CONTAINER_OF(p_dev, struct awbl_ntc_therm_dev, dev))

#define THERM_DEV_GET_PAR(p_therm_dev)  \
        ((struct awbl_ntc_therm_par *)((p_therm_dev)->dev.p_devhcf->p_devinfo))

#define DEV_GET_NTC_THERM_PAR(p_dev)   ((void *)AWBL_DEVINFO_GET(p_dev))

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_THERM_NTC_H */

/* end of file */
