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
 * \file
 * \brief ARMv8-M 上下文
 *
 * \internal
 * \par modification history:
 * - 1.10 2019-08-28, A.J, armv8-m arch.
 * - 1.00 14-09-19  tee, first implementation
 * \endinternal
 */

#include "rtk.h"
#include "aw_io.h"
#include "aw_bitops.h"
#include "arch/arm/armv8-m/armv8m_core.h"
#include "armv8m_context.h"

#ifndef ROUND_DOWN
#define ROUND_DOWN(p, d)        (((int)(p)) & (~((d)-1)))
#endif
#ifndef ROUND_UP
#define ROUND_UP(x, align)      (((int) (x) + ((align) - 1)) & ~((align) - 1))
#endif

typedef struct {
    uint32_t        task_list_node_offset;
    uint32_t        task_sp_offset;
    uint32_t        context_use_flag;
    uint32_t        task_name_offset;
}rtk_armv8m_jlink_rtos_info_t;

aw_const rtk_armv8m_jlink_rtos_info_t   rtk_armv8m_jlink_rtos_info = {
        AW_OFFSET(struct rtk_task,task_list_node),
        AW_OFFSET(struct rtk_task,sp),
#if __RTK_ARMV8M_CONTEXT_USE_FLAGS
        1,
#else
        0,
#endif
        AW_OFFSET(struct rtk_task,name),
};

/**
 * initializes stack of thread
 */
unsigned char *rtk_arch_stack_init(
    void              *tentry,
    void              *parameter1,
    void              *parameter2,
    char              *stack_low,
    char              *stack_high,
    void              *texit,
    rtk_task_option_t  option)
{
    unsigned long              *stk;
#if __RTK_ARMV8M_CONTEXT_USE_FLAGS
    unsigned long               flag;
#endif

    stack_high -= sizeof(int);
    stk      = (unsigned long*)ROUND_DOWN(stack_high, 8);

    *(--stk)   = 0x01000000L;               /* PSR */
    *(--stk) = (unsigned long)tentry;       /* entry point, pc */
    *(--stk) = (unsigned long)texit;        /* lr */
    *(--stk) = 12;                          /* r12 */
    *(--stk) = 3;                           /* r3 */
    *(--stk) = 2;                           /* r2 */
    *(--stk) = (unsigned long)parameter2;   /* r1 */
    *(--stk) = (unsigned long)parameter1;   /* r0 : argument */

    *(--stk) = 11;                          /* r11 */
    *(--stk) = 10;                          /* r10 */
    *(--stk) = 9;                           /* r9 */
    *(--stk) = 8;                           /* r8 */

    *(--stk) = 7;                           /* r7 */
    *(--stk) = 6;                           /* r6 */
    *(--stk) = 5;                           /* r5 */
    *(--stk) = 4;                           /* r4 */


    /* 线程初始化不使能浮点运算器，使用特权模式 */
#if __RTK_ARMV8M_CONTEXT_USE_FLAGS
    flag = 0;
#endif
#if CONFIG_RTK_USERMODE
    if (option & ENUM_RTK_TASK_OPTION_UNPRIVILEGED) {

    }
    else {
        flag |= ARMV8M_CONTEXT_FLAGS_PRIVILEGED;
    }
#endif
#if __RTK_ARMV8M_CONTEXT_USE_FLAGS
    *(--stk) = flag;                           /* init with flag  = 0 */
#endif
    /* return task's current stack address */
    return (unsigned char *)stk;
}

static void __fpu_init(void)
{
    /* 如果编译参数中使能了FPU,则使能对CP10和CP11的访问， */
#if __RTK_ARMV8M_USE_FPU
    AW_REG_BITS_SET32(&ARMV8M_SCB->CPACR, 20, 4, 0xF);
    rtk_arm_barrier_dsb();

    /* 使能lazy context save */
    writel(0xC0000000, &ARMV8M_FPU->FPCCR);
    rtk_arm_barrier_dsb();
#else
    /* 否则就禁用对CP10和CP11的访问，以避免出现意料不到的问题 */
    AW_REG_BITS_SET32(&ARMV8M_SCB->CPACR, 20, 4, 0);
    rtk_arm_barrier_dsb();
#endif
}

void rtk_arch_init()
{
    int                 prio;
    int                 count;
    int                 i;
    uint32_t            prio_count;

    __fpu_init();

    /* 设置亚优先级为最低位0 */
    armv8m_nvic_set_prio_group(0);

    /* 检查优先级位数配置 */
    writeb(0xFF, &ARMV8M_NVIC->IPR[0]);
    prio = readb(&ARMV8M_NVIC->IPR[0]);
    count = 0;
    for (i=7; i>=0; i--) {
        if (prio & (1 << i)) {
            count ++;
        }
        else {
            break;
        }
    }

    if (count < CONFIG_RTK_ARMV8M_NVIC_PRIO_BITS) {
        while (1);
    }

    prio_count = armv8m_nvic_get_priority_count();
    /* PENDSV异常用于任务切换，将PENDSV异常的优先级设为最低 */
    prio = prio_count - 1;
    armv8m_nvic_set_priority(ARMv8m_PendSV_IRQn, prio);
    armv8m_nvic_set_priority(ARMv8m_DebugMonitor_IRQn, prio);
    /* SVC 异常通常用于实现系统调用，它不能被负责任务切换的pendsv打断 */
    /* 但可以被其它中断打断 */
    /* 所以其优先级和pendsv一样 */
    armv8m_nvic_set_priority(ARMv8m_SVCall_IRQn, prio);
    /* systick优先级要高于svc，但可低于其它中断 */
    prio -= 1;
    armv8m_nvic_set_priority(ARMv8m_SysTick_IRQn, prio);

    /* usage ,memmanage,bus这三种fault 优先级配置为0 */
    armv8m_nvic_set_priority(ARMv8m_MemManagement_IRQn, 0);
    armv8m_nvic_set_priority(ARMv8m_BusFault_IRQn, 0);
    armv8m_nvic_set_priority(ARMv8m_UsageFault_IRQn, 0);

    prio -= 1;
    /* 其它中断优先级配置为比systick大1 */
    count =( (ARMV8M_SCnSCB->ICTR & 0xF) + 1 ) * 32;
    for (i=0; i<count; i++) {
        armv8m_nvic_set_priority(i, prio);
        armv8m_nvic_irq_disable(i);
    }

    /* 使能usage ,memmanage,bus这三种fault */
    AW_REG_BIT_SET32(&ARMV8M_SCB->SHCSR, 18);
    AW_REG_BIT_SET32(&ARMV8M_SCB->SHCSR, 17);
    AW_REG_BIT_SET32(&ARMV8M_SCB->SHCSR, 16);

    /* 使能除零错误 */
    AW_REG_BIT_SET32(&ARMV8M_SCB->CCR, 4);
    rtk_arm_barrier_dsb();
}

#define CYCCNT_ADDR      ((volatile uint32_t *)0xE0001004)

unsigned long rtk_arch_read_cyccnt(void)
{
    return readl(CYCCNT_ADDR);
}

int rtk_cpu_is_int_context(void)
{
    register uint32_t   data;

    data = readl(&ARMV8M_SCB->ICSR);
    data &= 0x1FF;

    /* systick ，pendsv,svc异常也算作中断状态 */
    if (data >= 11) {
        return 1;
    }

    return 0;
}

int rtk_cpu_is_fault_context(void)
{
    register uint32_t   data;

    data = readl(&ARMV8M_SCB->ICSR);
    data &= 0x1FF;

    if (0 == data || data >= 11) {
        return 0;
    }

    return 1;
}
