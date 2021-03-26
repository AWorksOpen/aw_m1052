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
 * \brief DC-Relay driver.
 *
 * 本驱动适用于任何可通过标准GPIO接口控制的直流继电器。
 *
 * \par 1.驱动信息
 *
 *  - 驱动名:   "dc-relay"
 *  - 总线类型: AWBL_BUSID_PLB
 *  - 设备信息: struct dc_relay_param
 *
 * \par 2.兼容设备
 *
 *  - 任何可以使用标准GPIO接口控制的直流继电器
 *
 * \par 3.设备定义/配置
 *
 *  - \ref grp_awbl_relay_dc_hwconf
 *
 * \par 4.用户接口
 *
 *  - \ref grp_aw_serv_relay
 *
 * \internal
 * \par modification history
 * - 1.00 14-07-30  hbt, first implementation.
 * \endinternal
 */

#ifndef __AWBL_RELAY_DC_H
#define __AWBL_RELAY_DC_H

#include "apollo.h"
#include "awbus_lite.h"
#include "aw_common.h"
#include "aw_types.h"
#include "awbl_relay.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_awbl_drv_relay_dc
 * \copydetails awbl_relay_dc.h
 * @{
 */

/**
 * \defgroup  grp_awbl_relay_dc_hwconf 设备定义/配置
 *
 * 直流继电器硬件设备的定义如下面的代码所示，用户在定义目标系统的硬件资源时，
 * 可以直接将这段代码嵌入到 awbus_lite_hwconf_usrcfg.c 中对应的位置，然后对设备
 * 信息进行必要的配置(可配置的项目用【x】标识)。
 *
 * \note 目前的系统中只有一个继电器，以后根据硬件升级可添加多个继电器

 * \include  hwconf_usrcfg_relay_dc.c
 * \note 以下配置项中带【用户配置】标识的，表示用户在开发应用时可以配置此参数。
 *
 * - 【1】 直流继电器设备信息 \n
 *
 *         (1) GPIO 列表 \n
 *
 *         (2) 继电器设备数目 \n
 *
 *         (3) 继电器起始基数 \n
 *
 *         (4) 继电器控制状态 \n
 *
 */

/** @} */

#define DC_RELAY_NAME  "dc-relay"

/** \brief dc-relay device platform data */
struct dc_relay_param {
    const uint16_t *relay_gpios;   /**< \brief GPIO number table */
    uint8_t         relay_num;     /**< \brief numbers of relays */
    uint8_t         base_num;      /**< \brief base number of relay */
    uint8_t         active_low;    /**< \brief driven polarity of the GPIO */
};

/** \brief dc-relay device */
struct dc_relay {
    struct  awbl_dev     dev;        /**< \brief the AWBus device */
    struct  dc_relay    *next;        /**< \brief the next relay device */
};

/** \brief get dc-relay parameter from device */
#define dev_get_dc_relay_par(p_dev)    ((void *)AWBL_DEVINFO_GET(p_dev))

/** \brief convert a awbl_dev pointer to dc_relay pointer */
#define dev_to_dc_relay(p_dev)    AW_CONTAINER_OF(p_dev, struct dc_relay, dev)

/** \brief convert a relay_dev pointer to dc_relay pointer */
#define relay_to_dc_relay(dev)   ((struct dc_relay *)(dev))

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_RELAY_DC_H */

/* end of file */
