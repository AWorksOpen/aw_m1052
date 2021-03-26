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
 * \brief ARMv7-M 线程上下文
 *
 * 本模块为(ARMv7-M架构)的异常模块实现。
 *
 * \par 特性
 *
 * - armv7-m 线程上下文相关的定义
 *
 * \internal
 * \par modification history:
 * - 1.00 17-04-08  zen, first implementation
 * \endinternal
 */

#ifndef __ARMV4_CONTEXT_H
#define __ARMV4_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "rtk_config.h"

/* \brief 确定整体编译环境是否使用了浮点数 */
#if (  ((defined( __CC_ARM ) || defined(__ARMCOMPILER_VERSION)) && defined ( __TARGET_FPU_VFP )) \
    || (defined ( __ICCARM__ ) && defined ( __ARMVFP__ )) \
    || (defined ( __GNUC__ ) && defined ( __VFP_FP__ ) && !defined(__SOFTFP__)) )
#define __RTK_ARMV4_USE_FPU     1
#else
#define __RTK_ARMV4_USE_FPU     0
#endif

/** \brief armv4 FPEXC标志的定义*/

#define ARMV4_FPEXC_FPU_ENABLE_FLAGS    (1 << 30);

/** \brief armv7 CPSR标志的定义 */
#define CPSR_MODE_MASK      0x1F
#define CPSR_MODE_USR       0x10
#define CPSR_MODE_FIQ       0x11
#define CPSR_MODE_IRQ       0x12
#define CPSR_MODE_SVC       0x13
#define CPSR_MODE_MONITOR   0x16
#define CPSR_MODE_ABORT     0x17
#define CPSR_MODE_HYP       0x1A
#define CPSR_MODE_UNDEF     0x1B
#define CPSR_MODE_SYSTEM    0x1F

#define CPSR_FLAG_IRQ_MASK  (1 << 7)
#define CPSR_FLAG_FIQ_MASK  (1 << 6)
#define CPSR_FLAG_THUMB     (1 << 5)




#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __ARMV4_CONTEXT_H */

/* end of file */
