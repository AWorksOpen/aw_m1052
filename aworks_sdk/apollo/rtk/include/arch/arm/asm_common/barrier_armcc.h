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
 * \file  barrier_gcc.h
 * \brief rtk关于barrier的在gcc中的定义和函数
 *
 * \internal
 * \par modification history:
 * - 1.00 18-11-19  sni, first implement
 * \endinternal
 */

#ifndef     __BARRIER_GCC_H__
#define     __BARRIER_GCC_H__

#if defined(__ARMCOMPILER_VERSION) && (__ARMCOMPILER_VERSION > 6000000)
#include <arm_compat.h>
#include <arm_acle.h>
#endif

#ifndef __iomem
#define __iomem
#endif

#ifndef __force
#define __force
#endif

/* compiler barrier */
#define rtk_barrier()           __schedule_barrier()

/* cpu spec barrier */
#define rtk_arm_barrier_dmb()   __schedule_barrier(); \
                                __dmb(0xF); \
                                __schedule_barrier();

#define rtk_arm_barrier_dsb()   __schedule_barrier(); \
                                __dsb(0xF); \
                                __schedule_barrier();

#define rtk_arm_barrier_isb()   __schedule_barrier(); \
                                __isb(0xF); \
                                __schedule_barrier();

#define rtk_rmb()                       rtk_arm_barrier_dmb()
#define rtk_wmb()                       rtk_arm_barrier_dmb()
#define rtk_mb()                        rtk_arm_barrier_dmb()

#define rtk_mmiowb()                    rtk_arm_barrier_dsb();

#ifdef      CONFIG_RTK_SMP_ENABLE

#define rtk_smp_rmb()                   rtk_arm_barrier_dmb()
#define rtk_smp_wmb()                   rtk_arm_barrier_dmb()
#define rtk_smp_mb()                    rtk_arm_barrier_dmb()

/* arm处理器不存在data dependency barrier*/
#define rtk_smp_read_barrier_depends()  rtk_barrier()

#else

#define rtk_smp_rmb()                   rtk_barrier()
#define rtk_smp_wmb()                   rtk_barrier()
#define rtk_smp_mb()                    rtk_barrier()
#define rtk_smp_read_barrier_depends()  rtk_barrier()

#endif

#endif
