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
 * \brief 外部使用的配置文件
 *
 * 用户在此文件中对模块的相关参数进行配置
 *
 * \internal
 * \par modification history:
 * - 1.00 12-09-24  zen, first implementation
 * \endinternal
 */

#ifndef __AWBUS_LITE_USRCFG_H
#define __AWBUS_LITE_USRCFG_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

/**
 * \addtogroup grp_awbl_usrcfg
 * @{
 */

/**
 * \name AWBus 模块配置 (lite)
 * @{
 */

/**
 * \brief  AWBus lite 支持的最大驱动数
 *
 * 为了避免不必要的内存消耗，用户可以将此参数设置为系统中实际会使用的驱动个数。
 * 例如，如果只用到了中断控制器、GPIO两个驱动，则可以将此参数设置为2。
 *
 * \attention 有效值范围为 1 ~ 128
 */
#define AWBUS_LITE_DRIVER_MAX   15

/**
 * \brief AWBus lite 支持的最大总线类型数
 *
 * 为了避免不必要的内存消耗，用户可以将此参数设置为系统中实际会使用的总线类型数。
 * 例如，如果只用到了CPU本地总线 (必须使用)、I2C两种总线类型，则可以将此参数设置为2。
 *
 * \attention 有效值范围为 1 ~ 128
 */
#define AWBUS_LITE_BUSTYPE_MAX  3

/** @} */

/** @} grp_lpc11xx_gpio_usrcfg */

#ifdef __cplusplus
}
#endif    /* __cplusplus     */

#endif    /* __AWBUS_LITE_USRCFG_H */

/* end of file */

