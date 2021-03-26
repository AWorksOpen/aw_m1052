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
 * - 1.00 12-08-23  zen, first implementation
 * \endinternal 
 */

#ifndef __AWBL_LPC11XX_GPIO_USRCFG_H
#define __AWBL_LPC11XX_GPIO_USRCFG_H

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus	*/

/**
 * \addtogroup grp_lpc11xx_gpio_usrcfg
 * @{
 */
 
/** 
 * \name LPC11XX GPIO 模块配置
 * @{
 */

/**
 * \brief 配置触发器信息条数
 *
 * 由于LPC11XX资源有限，为了避免不必要的内存消耗，用户可以将此参数设置为实际需要
 * 连接触发器的引脚的个数。
 * 例如，只有3个引脚会被连接触发器，则可以将此参数 #AWBL_LPC11XX_GPIO_TRIGINFO_COUNT
 * 设置为3。
 *
 * \attention LCP11XX 有42个GPIO引脚，因此，此参数最大有效值为42
 */
#define AWBL_LPC11XX_GPIO_TRIGINFO_COUNT     3

/** @} */
/** @} grp_lpc11xx_gpio_usrcfg */

#ifdef __cplusplus
}
#endif	/* __cplusplus 	*/

#endif	/* __AWBL_LPC11XX_GPIO_USRCFG_H */

/* end of file */

