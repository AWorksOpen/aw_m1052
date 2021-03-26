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
 * \file  aw_psp_barrier.h
 * \brief AWorks中关于Barrier操作的适配
 *
 * \internal
 * \par modification history:
 * - 2018-11-19  sni, first implement
 * \endinternal
 */

#ifndef         __AW_PSP_BARRIER_H__
#define         __AW_PSP_BARRIER_H__

#include "rtk_barrier.h"

#define aw_barrier()            rtk_barrier()

#define aw_rmb()                            rtk_rmb()
#define aw_wmb()                            rtk_wmb()
#define aw_mb()                             rtk_mb()

#define aw_smp_rmb()                        rtk_smp_rmb()
#define aw_smp_wmb()                        rtk_smp_wmb()
#define aw_smp_mb()                         rtk_smp_mb()
#define aw_smp_read_barrier_depends()       rtk_smp_read_barrier_depends()

#define aw_mmiowb()                         rtk_mmiowb()

#ifdef rtk_arm_barrier_dsb

#define aw_arm_barrier_dsb()    rtk_arm_barrier_dsb();
#define aw_arm_barrier_isb()    rtk_arm_barrier_isb();
#endif

#endif
