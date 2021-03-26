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
 * - 1.00 12-08-23  zen, first implemetation
 * \endinternal 
 */

#ifndef __AWBL_ARMCM_NVIC_USRCFG_H
#define __AWBL_ARMCM_NVIC_USRCFG_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

/**
 * \addtogroup grp_armcm_nvic_usrcfg
 * @{
 */
 
/** 
 * \name ARM Cortex-M NVIC 模块配置
 * @{
 */

/**
 * \brief 配置引脚中断信息条数
 *
 * 由于CPU资源可能有限，为了避免不必要的RAM消耗，用户可以将此参数设置为实际
 * 需要的中断数目。
 */
#define AW_CFG_ARMCM_NVIC_ISRINFO_COUNT     10

/** @} */
/** @} grp_armcm_nvic_usrcfg */

#ifdef __cplusplus
}
#endif    /* __cplusplus     */

#endif    /* __AWBL_ARMCM_NVIC_USRCFG_H */

/* end of file */

