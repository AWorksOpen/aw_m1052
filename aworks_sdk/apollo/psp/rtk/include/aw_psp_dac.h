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
 * \brief DAC平台相关标准定义头文件
 *
 * \internal
 * \par modification history:
 * - 1.00 15-04-02  tee, first implementation
 * \endinternal
 */

#ifndef __AW_PSP_DAC_H
#define __AW_PSP_DAC_H

#ifdef __cplusplus
extern "C" {
#endif


#if defined(AW_IMG_PRJ_BUILD) || defined(AW_BSP_LIB_BUILD)
#include "aw_bsp_dac.h"
#endif

typedef uint32_t aw_psp_dac_channel_t;
typedef uint32_t aw_psp_dac_val_t;

#ifdef __cplusplus
}
#endif    /* __cplusplus     */

#endif    /* __AW_PSP_DAC_H */

/* end of file */
