/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https:/*www.zlg.cn
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

#ifndef __ARMV7_CONTEXT_H
#define __ARMV7_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "rtk_config.h"

/* \brief 确定整体编译环境是否使用了浮点数 */
#if (  ((defined( __CC_ARM ) || defined(__ARMCOMPILER_VERSION)) && defined ( __TARGET_FPU_VFP )) \
    || (defined ( __ICCARM__ ) && defined ( __ARMVFP__ )) \
    || (defined ( __GNUC__ ) && defined ( __VFP_FP__ ) && !defined(__SOFTFP__)) )
#define __RTK_ARMV7_USE_FPU     1
#else
#define __RTK_ARMV7_USE_FPU     0
#endif

/* armv7 FPEXC标志的定义 */

#define ARMV7_FPEXC_FPU_ENABLE_FLAGS    (1 << 30);

/* armv7 CPSR标志的定义 */
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

/* NSACR标志位定义 */
#define NSACR_NS_SMP        (1<<18)
#define NSACR_NS_L2ERR      (1<<17)
#define NSACR_NSASEDIS      (1<<15)
#define NSACR_NSD32DIS      (1<<14)
#define NSACR_CP11          (1<<11)
#define NSACR_CP10          (1<<10)

/* CPACR标志位定义 */
#define CPACR_ASEDIS        (1<<31)
#define CPACR_D32DIS        (1<<30)
#define CPACR_CP11_FULL     (0x3<<22)
#define CPACR_CP10_FULL     (0x3<<20)


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __ARMV7_CONTEXT_H */

/* end of file */
