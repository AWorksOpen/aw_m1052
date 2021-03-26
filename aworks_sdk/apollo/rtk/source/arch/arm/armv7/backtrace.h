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
 * \brief 
 *
 * \internal
 * \par History
 * - 1.00 14-09-23  orz, first implementation.
 * \endinternal
 */
#ifndef __ARCH_ARMBACKTRACE_H
#define __ARCH_ARMBACKTRACE_H
#include "rtk.h"

/*
 * Do not compiling with -fbounded-pointers!
 */

int task_backtrace(
        struct rtk_task *task,
        void           **array,
        int              max,
        void           * stack_frame_base);

int task_backtrace_cur_task(
        struct rtk_task *task,
        void           **array,
        int              max,
        void           * stack_frame_base,
        void           * lr);


/**
 * arm-none-eabi-gcc eg.
 *
 * void func (void)
 * {
 *     call_func();
 * }
 *
 *00000000 <func>:
 * 000:   e92d4800    push    {fp, lr}
 * 004:   e28db004    add fp, sp, #4
 * 008:   e3a00001    mov r0, #1
 * 00c:   ebffffbb    bl  0 <call_func>
 * 010:   e24bd004    sub sp, fp, #4
 * 014:   e8bd4800    pop {fp, lr}
 * 018:   e12fff1e    bx  lr
 *
 * fp is r11 register.
 * __builtin_frame_address(0) will return r11 register.
 * "add fp, sp, #4" fp point to return address so, we will sub 4 to detemine frame address.
 *
 * void func (void)
 * {
 *     *((int *)0x12345678) = 1;
 * }
 *
 * 00000100 <func>:
 *  100:   e52db004    push    {fp}
 *  104:   e28db000    add fp, sp, #0
 *  108:   e59f3010    ldr r3, [pc, #16]
 *  10c:   e3a02001    mov r2, #1
 *  110:   e5832000    str r2, [r3]
 *  114:   e28bd000    add sp, fp, #0
 *  118:   e49db004    pop {fp}
 *  11c:   e12fff1e    bx  lr
 *
 * lr do not push to stack, so this func can not backtrace. (in signal context)
 */

#endif
