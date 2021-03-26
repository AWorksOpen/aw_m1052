/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file armv7m_context_gcc.S
 * \brief rtk中armv7m架构原子操作相关函数的arm-none-eabi-gcc汇编实现 *
 *
 * \par 源代码
 * \internal
 * \par Modification history
 * - 1.00 2019-10-23 wk, first version.
 * \endinternal
 */

#include "../armv7_context.h"
#include "armv7_gcc_common.h"


ARM_FUNC_BEGIN(rtk_cpu_atomic_add)
//R0: add_val    R1: &p_val->counter
    dmb
    dsb
add_opt:
    ldrex   r2, [r1]
    add     r2, r2, r0
    strex   r3, r2, [r1]
    teq     r3, #0
    bne     add_opt

    mov     r0, r2
    dmb
    dsb
    bx      lr
ARM_FUNC_END()


ARM_FUNC_BEGIN(rtk_cpu_atomic_sub)
//R0: sub_val    R1: &p_val->counter
    dmb
    dsb
sub_opt:
    ldrex   r2, [r1]
    sub     r2, r2, r0
    strex   r3, r2, [r1]
    teq     r3, #0
    bne     sub_opt

    mov     r0, r2
    dmb
    dsb
    bx      lr
ARM_FUNC_END()


ARM_FUNC_BEGIN(rtk_cpu_atomic_cmpxchg)
//R0: old_val    R1: new_val  R2: &p_val->counter
    push    {r4}

    dmb
    dsb

cmpxchg_opt:
    ldrex   r3, [r2]
    teq     r3, r0
    strexne r4, r1, [r2]
    teqne   r4, #0
    bne     cmpxchg_opt

    mov     r0, r3

    dmb
    dsb
    pop     {r4}
    bx      lr
ARM_FUNC_END()


