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
 * \brief aworks1.0 异常标准接口头文件
 *
 * \internal
 * \par modification history:
 * - 1.00 12-09-04  zen, first implementation
 * \endinternal
 */

#ifndef __AW_PSP_EXC_H
#define __AW_PSP_EXC_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

#ifdef AW_CORTEX_M0
#include "arch/arm/armv6m/armv6m_exc.h"
#endif

#if defined AW_CORTEX_M4 || defined AW_CORTEX_M3 || defined AW_CORTEX_M7
#include "arch/arm/armv7-m/armv7m_exc.h"
#endif

#if defined(AW_CORTEX_M33)
#include "arch/arm/armv8-m/armv8m_exc.h"
#endif

#ifdef __cplusplus
}
#endif    /* __cplusplus     */

#endif    /* __AW_PSP_EXC_H */

/* end of file */
