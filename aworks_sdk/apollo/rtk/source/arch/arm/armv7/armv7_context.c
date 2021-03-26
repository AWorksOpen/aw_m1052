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
 * \brief ARMv7-M 线程上下文
 *
 * 本模块为(ARMv7-M架构)的异常模块实现。
 *
 * \par 特性
 *
 * - armv7-m 线程上下文相关的定义
 *
 * \internal
 * \par modification history:
 * - 1.00 17-04-08  zen, first implementation
 * \endinternal
 */
 
#include "rtk.h"
#include "aw_io.h"
#include "aw_bitops.h"
#include "armv7_context.h"

#ifndef ROUND_DOWN
#define ROUND_DOWN(p, d)        (((int)(p)) & (~((d)-1)))
#endif
#ifndef ROUND_UP
#define ROUND_UP(x, align)      (((int) (x) + ((align) - 1)) & ~((align) - 1))
#endif

#define CPSR_REG_INIT_VALUE     (CPSR_FLAG_FIQ_MASK|CPSR_MODE_SYSTEM)

unsigned int rt_thread_switch_interrupt_flag;

typedef struct {
    uint32_t        task_list_node_offset;
    uint32_t        task_sp_offset;
    uint32_t        context_use_flag;
    uint32_t        task_name_offset;
    uint32_t        prio_offset;
    uint32_t        vfp_d_count;

}rtk_armv7_jlink_rtos_info_t;

aw_const rtk_armv7_jlink_rtos_info_t   rtk_armv7m_jlink_rtos_info = {
        AW_OFFSET(struct rtk_task,task_list_node),
        AW_OFFSET(struct rtk_task,sp),
#if __RTK_ARMV7_USE_FPU
        1,
#else
        0,
#endif
        AW_OFFSET(struct rtk_task,name),
        AW_OFFSET(struct rtk_task,waitq_prio),
#ifdef _ARMV7_VFP_D32
        32,
#else
        16,
#endif
};


unsigned char *rtk_arch_stack_init(
        void              *tentry,
        void              *a,
        void              *b,
        char              *stack_low,
        char              *stack_high,
        void              *texit,
        rtk_task_option_t  option)
{
    uint32_t                    *p;

#if __RTK_ARMV7_USE_FPU
    unsigned long               flag;
#endif

    stack_high -= sizeof(int);
    p = (uint32_t *)ROUND_DOWN(stack_high, 8);
    p --;

    /*
     *  r0-r12, lr, pc
     */
    *p-- = (int)tentry; /* pc */
    *p-- = (int)texit;  /* lr */
    *p-- = 0xcccccccc;  /* r12*/
    *p-- = 0xbbbbbbbb;  /* r11 */
    *p-- = 0xaaaaaaaa;  /* r10 */
    *p-- = 0x99999999;  /* r9 */
    *p-- = 0x88888888;  /* r8 */
    *p-- = 0x77777777;  /* r7 */
    *p-- = 0x66666666;  /* r6 */
    *p-- = 0x55555555;  /* r5 */
    *p-- = 0x44444444;  /* r4*/
    *p-- = 0x33333333;  /* r3*/
    *p-- = 0x22222222;  /* r2*/
    *p-- = (int)b;      /* r1: arg1 of function task_entry_exit */
    *p-- = (int)a;      /* r0: arg0 of function task_entry_exit */

     if ( (int)tentry & 0x00000001 ) {
        *p = CPSR_REG_INIT_VALUE|CPSR_FLAG_THUMB;   /* cpsr */
     } else {
        *p = CPSR_REG_INIT_VALUE;                   /* cpsr */
     }

      /* 线程初始化不使能浮点运算器，使用特权模式 */
#if __RTK_ARMV7_USE_FPU
    flag = 0;
    p--;
   *p = flag;
#endif

    /*
     *  full stack pointer
     */
    return (unsigned char *)p;
}


void armv7_vfp_init(void);

void rtk_arch_init()
{
    armv7_vfp_init();
}

int armv7_is_irq_mode(uint32_t cpsr)
{
    if (CPSR_MODE_IRQ == (cpsr &CPSR_MODE_MASK)) {
        return 1;
    }
    return 0;
}
/* end of file*/