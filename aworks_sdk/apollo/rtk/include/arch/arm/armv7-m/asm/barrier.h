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
 * \file  rtk_barrier.h
 * \brief rtk关于barrier的定义和函数
 *
 * \internal
 * \par modification history:
 * - 1.00 18-11-19  sni, first implement
 * \endinternal
 */

#ifndef         __BARRIER_H__
#define         __BARRIER_H__

#if defined (__CC_ARM) || defined(__ARMCOMPILER_VERSION)

#include "barrier_armcc.h"

#elif defined ( __GNUC__ )

#include "barrier_gcc.h"

#endif

#endif
