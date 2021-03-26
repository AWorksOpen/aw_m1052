/*******************************************************************************
 *                                 AWorks
 *                       ----------------------------
 *                       innovating embedded platform
 *
 * Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
 * All rights reserved.
 *
 * Contact information:
 * web site:    http://www.zlg.cn/
 *******************************************************************************/

/**
 * \file  barrier_gcc.h
 * \brief rtk 关于 barrier 的在 gcc 中的定义和函数
 *
 * \internal
 * \par modification history:
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

// compiler barrier
#define rtk_barrier()            asm volatile("" : : : "memory")

#define rtk_riscv_barrier_dmb()   rtk_barrier()
#define rtk_riscv_barrier_dsb()   rtk_barrier()
#define rtk_riscv_barrier_isb()   rtk_barrier()

#define rtk_rmb()               rtk_barrier()
#define rtk_wmb()               rtk_barrier()
#define rtk_mb()                rtk_barrier()
#define rtk_mmiowb()            rtk_barrier()


#ifdef      CONFIG_RTK_SMP_ENABLE

#define rtk_smp_rmb()           rtk_barrier()
#define rtk_smp_wmb()           rtk_barrier()
#define rtk_smp_mb()            rtk_barrier()

#define rtk_smp_read_barrier_depends()  rtk_barrier()

#else

#define rtk_smp_rmb()           rtk_barrier()
#define rtk_smp_wmb()           rtk_barrier()
#define rtk_smp_mb()            rtk_barrier()

#define rtk_smp_read_barrier_depends()  rtk_barrier()

#endif

#endif

/* end of file */