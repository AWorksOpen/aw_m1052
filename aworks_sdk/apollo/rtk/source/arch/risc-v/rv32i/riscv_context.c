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
 * \file riscv_context.c
 * \brief context switch c relative functions.
 *
 * \internal
 * \par History
 * \endinternal
 */

#include "rtk.h"
#include "aw_io.h"
#include <string.h>

extern uint32_t __global_pointer;
uint32_t __need_context_switch_in_interrupt;

#define MSTATUS_MPIE_MASK (0x01 << 7)
#define MSTATUS_MPP_MASK (0x3 << 11)
#define int_from_pointer(pointer) ((char*)pointer - (char*)0)

#ifndef ROUND_DOWN
#define ROUND_DOWN(p, d)        ((p) & (~((d)-1)))
#endif

unsigned char *rtk_arch_stack_init(
        void *tentry,
        void *a,
        void *b,
        char *stack_low,
        char *stack_high,
        void *texit,
        rtk_task_option_t option)
{
    unsigned long *top_of_stack;

    stack_high = (char*)stack_high - sizeof(int);
    top_of_stack = (uint32_t *)ROUND_DOWN(int_from_pointer(stack_high), 8);

    top_of_stack--;
    *top_of_stack = int_from_pointer(tentry);  /* mepc */
    top_of_stack--;
    *top_of_stack = int_from_pointer(texit);   /* ra */
    top_of_stack -= 6;                         /* reg x17 -> x12 */

    top_of_stack--;
    *top_of_stack = int_from_pointer(b);       /* reg a1 arg1 of function task_entry_exit */

    top_of_stack--;
    *top_of_stack = int_from_pointer(a);       /* reg a0 arg0 of function task_entry_exit */

    top_of_stack -= 4;                         /* reg x4 - x7 */

    top_of_stack -= 16;                                   /* reg x8 - x9, x18 - x31 */
    top_of_stack--;
    *top_of_stack = MSTATUS_MPIE_MASK | MSTATUS_MPP_MASK; /* mstatus */

    (void)option;

    return (unsigned char*)top_of_stack;
}


void rtk_arch_init()
{

}

int rtk_cpu_ffs(unsigned long x)
{
    register int r =  0;

    if (!(x & 0xffff)) {
        x >>= 16;
        r += 16;
    }
    if (!(x & 0xff) ) {
        x >>= 8;
        r += 8;
    }
    if (!(x & 0xf) ) {
        x >>= 4;
        r += 4;
    }

    if (!(x & 0x3) ) {
        x >>= 2;
        r += 2;
    }

    if (!(x & 0x1) ) {
        x >>= 1;
        r += 1;
    }
    r += 1;
    return r;
}

void rtk_arch_context_switch_interrupt(void)
{
    __raw_writel(1, &__need_context_switch_in_interrupt);
}

/* end of file */