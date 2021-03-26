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

/* 包含特有的定义*/
#if defined(AW_CORTEX_M7) || defined(AW_CORTEX_M3) || defined(AW_CORTEX_M4)

#include "arch/arm/armv7-m/asm/barrier.h"

#elif defined(AW_ARMV7)

#include "arch/arm/armv7/asm/barrier.h"

#elif defined(AW_ARM926EJ_S)

#include "arch/arm/armv4/asm/barrier.h"

#elif defined(AW_RISC_V)

#include "arch/risc-v/asm/barrier.h"

#elif defined(AW_CORTEX_M33)

#include "arch/arm/armv8-m/asm/barrier.h"

#endif
