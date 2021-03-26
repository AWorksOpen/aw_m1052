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
 * \brief DC-Buzzer driver.
 *
 * 本驱动适用于任何可通过标准GPIO接口控制的直流蜂鸣器。
 *
 * \par 1.驱动信息
 *
 *  - 驱动名:   "dc-buzzer"
 *  - 总线类型: AWBL_BUSID_PLB
 *  - 设备信息: struct dc_buzzer_param
 *
 * \par 2.兼容设备
 *
 *  - 任何可以使用标准GPIO接口控制的直流蜂鸣器
 *
 * \par 3.设备定义/配置
 *
 *  - \ref grp_awbl_buzzer_dc_hwconf
 *
 * \par 4.用户接口
 *
 *  - \ref grp_aw_serv_buzzer
 *
 * \internal
 * \par modification history
 * - 1.00 13-03-08  orz, first implementation.
 * \endinternal
 */

#ifndef __AWBL_BUZZER_DC_H
#define __AWBL_BUZZER_DC_H

#include "apollo.h"
#include "awbus_lite.h"
#include "awbl_buzzer.h"
#include "aw_common.h"
#include "aw_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_awbl_drv_buzzer_dc
 * \copydetails awbl_buzzer_dc.h
 * @{
 */

/**
 * \defgroup  grp_awbl_buzzer_dc_hwconf 设备定义/配置
 *
 * 直流蜂鸣器硬件设备的定义如下面的代码所示，用户在定义目标系统的硬件资源时，
 * 可以直接将这段代码嵌入到 awbus_lite_hwconf_usrcfg.c 中对应的位置，然后对设备
 * 信息进行必要的配置(可配置的项目用【x】标识)。
 *
 * \note 一个系统中只能有一个蜂鸣器

 * \include  hwconf_usrcfg_buzzer_dc.c
 * \note 以下配置项中带【用户配置】标识的，表示用户在开发应用时可以配置此参数。
 *
 * - 【1】 控制直流蜂鸣器的GPIO \n
 * 在这里填写控制直流蜂鸣器的GPIO编号以及使蜂鸣器鸣叫的电平。
 */

/** @} */

#define DC_BUZZER_NAME  "dc-buzzer"

/** \brief dc-buzzer device platform data */
struct dc_buzzer_param {
    uint16_t    gpio;              /**< \brief GPIO number the buzzer is driven */
    uint16_t    active_low;        /**< \brief driven polarity of the GPIO */
    void (*pfn_plfm_init) (void);  /**< \brief platform initializing */
};

/** \brief get dc-buzzer parameter from device */
#define dev_get_dc_buzzer_par(p_dev)    ((void *)AWBL_DEVINFO_GET(p_dev))

/** \brief dc-buzzer device */
struct dc_buzzer {
    struct awbl_buzzer_dev  bdev;       /**< \brief buzzer device, should go first */
    struct awbl_dev         dev;        /**< \brief the AWBus device */
    uint16_t                gpio;       /**< \brief GPIO number the buzzer is driven */
    uint16_t                active_low; /**< \brief driven polarity of the GPIO */
};

/** \brief convert a awbl_dev pointer to dc_buzzer pointer */
#define dev_to_dc_buzzer(p_dev)    AW_CONTAINER_OF(p_dev, struct dc_buzzer, dev)

/** \brief convert a buzzer_dev pointer to dc_buzzer pointer */
#define buzzer_to_dc_buzzer(dev)   ((struct dc_buzzer *)(dev))

/******************************************************************************/
void awbl_dc_buzzer_drv_register (void);


#ifdef __cplusplus
}
#endif

#endif /* __AWBL_BUZZER_DC_H */

/* end of file */
