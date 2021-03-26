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
 * \brief rtk����barrier����gcc�еĶ���ͺ���
 *
 * \internal
 * \par modification history:
 * - 2018-11-19  sni, first implement
 * \endinternal
 */

#ifndef     __BARRIER_GCC_H__
#define     __BARRIER_GCC_H__

#if defined(__ARMCOMPILER_VERSION) && (__ARMCOMPILER_VERSION > 6000000)
#include <arm_compat.h>
#endif

#ifndef __iomem
#define __iomem
#endif

#ifndef __force
#define __force
#endif

// compiler barrier
#define rtk_barrier()           __schedule_barrier()



#define rtk_rmb()               rtk_barrier()
#define rtk_wmb()               rtk_barrier()
#define rtk_mb()                rtk_barrier()

#define rtk_mmiowb()            rtk_barrier()

#ifdef      CONFIG_RTK_SMP_ENABLE

#define rtk_smp_rmb()           rtk_barrier()
#define rtk_smp_wmb()           rtk_barrier()
#define rtk_smp_mb()            rtk_barrier()

/** \brief arm������������data dependency barrier*/
#define rtk_smp_read_barrier_depends()  rtk_barrier()

#else

#define rtk_smp_rmb()           rtk_barrier()
#define rtk_smp_wmb()           rtk_barrier()
#define rtk_smp_mb()            rtk_barrier()
#define rtk_smp_read_barrier_depends()  rtk_barrier()
#endif

#endif
