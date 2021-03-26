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


#ifndef __iomem
#define __iomem
#endif

#ifndef __force
#define __force
#endif

/* compiler barrier*/
#define rtk_barrier()           asm volatile("" : : : "memory")

#define rtk_arm_barrier_dmb()   asm volatile("" : : : "memory")
#define rtk_arm_barrier_dsb()   asm volatile("" : : : "memory")
#define rtk_arm_barrier_isb()   asm volatile("" : : : "memory")

#define rtk_rmb()               rtk_barrier()
#define rtk_wmb()               rtk_barrier()
#define rtk_mb()                rtk_barrier()
#define rtk_mmiowb()            rtk_barrier();


#ifdef      CONFIG_RTK_SMP_ENABLE

#define rtk_smp_rmb()           rtk_barrier()
#define rtk_smp_wmb()           rtk_barrier()
#define rtk_smp_mb()            rtk_barrier()

/** \brief arm处理器不存在data dependency barrier*/
#define rtk_smp_read_barrier_depends()  rtk_barrier()           

#else

#define rtk_smp_rmb()           rtk_barrier()
#define rtk_smp_wmb()           rtk_barrier()
#define rtk_smp_mb()            rtk_barrier()

#define rtk_smp_read_barrier_depends()  rtk_barrier()

#endif

#endif
