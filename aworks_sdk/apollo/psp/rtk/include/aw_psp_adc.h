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
 * \brief ADC平台相关标准定义头文件
 *
 * \internal
 * \par modification history:
 * - 1.00 12-11-06  zyr, first implementation
 * \endinternal
 */

#ifndef __AW_PSP_ADC_H
#define __AW_PSP_ADC_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

/******************************************************************************/
#if defined(AW_IMG_PRJ_BUILD) || defined(AW_BSP_LIB_BUILD)
#include "aw_bsp_adc.h"
#endif

typedef uint32_t aw_psp_adc_channel_t;
typedef uint32_t aw_psp_adc_val_t;

#ifdef __cplusplus
}
#endif    /* __cplusplus     */

#endif    /* __AW_PSP_ADC_H */

/* end of file */
