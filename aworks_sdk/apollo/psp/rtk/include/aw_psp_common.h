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
 * \brief RTK 平台相关标准定义头文件
 *
 * \par 说明
 *  在所有依赖aworks1.0的模块(*.<b>c</b>文件)中，该文件必须被直接或间接地包含。
 *
 * \par 该头文件需要引入的标准定义有：\n
 *
 * \internal
 * \par modification history:
 * - 1.00 12-09-05  orz, move some defines to aw_common.h
 * - 1.00 12-08-27  zen, first implementation
 * \endinternal
 */

#ifndef __AW_PSP_COMMON_H
#define __AW_PSP_COMMON_H

#include "rtk.h"
#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

#define AW_PLATFORM        "aworks1.0"

/** CPU默认使用使用小端，可在bsp中覆盖这里的配置 */
#ifndef AW_CPU_ENDIAN
#define AW_CPU_ENDIAN   AW_LITTLE_ENDIAN
#endif

/******************************************************************************/
#if defined(AW_IMG_PRJ_BUILD) || defined(AW_BSP_LIB_BUILD)
#include "aw_bsp_common.h"
#endif
    
#ifdef __cplusplus
}
#endif    /* __cplusplus     */

#endif    /* __AW_PSP_COMMON_H    */

/* end of file */
