/*
 * aw_atomic.c
 *
 *  Created on: 2019Äê10ÔÂ19ÈÕ
 *      Author: weike
 */

#include "armv7m_gcc_common.h"


ARM_FUNC_BEGIN(atomic_add)
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


ARM_FUNC_BEGIN(atomic_sub)
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


ARM_FUNC_BEGIN(atomic_cmpxchg)
//R0: old_val    R1: new_val  R2: &p_val->counter
    push    {r4}
    dmb
    dsb

cmpxchg_opt:
    ldrex   r3, [r2]
    cmp     r3, r0
    itt     ne
//    teq     r3, r0
    strexne r4, r1, [r2]
    cmpne   r4, #0
    bne     cmpxchg_opt

    mov     r0, r3
    dmb
    dsb
    pop     {r4}
    bx      lr
ARM_FUNC_END()


