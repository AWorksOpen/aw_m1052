/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file armv7m_context_gcc.S
 * \brief rtk中armv7m架构任务上下文相关函数的arm-none-eabi-gcc汇编实现 *
 *
 * \par 源代码
 * \internal
 * \par Modification history
 * - 1.00 2018-06-13 sni, first version.
 * \endinternal
 */

#include "../armv7m_context.h"
#include "armv7m_gcc_common.h"

    .set SCB_VTOR,              0xE000ED08          /* Vector Table Offset Register */
    .set NVIC_INT_CTRL,         0xE000ED04          /* interrupt control state register */
    .set NVIC_SYSPRI2,          0xE000ED20          /* system priority register (2) */
    .set NVIC_PENDSV_PRI,       0x00FF0000          /* PendSV priority value (lowest) */
    .set NVIC_PENDSVSET,        0x10000000          /* value to trigger PendSV exception */

    // 声明全局函数
    .global _rtk_sched_end_hook_exec
    .global _rtk_sched_begin_hook_exec

#if CONFIG_RTK_ARMV7M_CRITICAL_USE_BASEPRI

/*
 * uint32_t rtk_cpu_enter_critical();
 */
ARM_FUNC_BEGIN(rtk_cpu_enter_critical)
    mov     r1, #CONFIG_RTK_ARMV7M_CRITICAL_BASEPRI_VAL
    MRS     r0, BASEPRI
    msr     BASEPRI,r1
    BX      LR
ARM_FUNC_END()

/* void rtk_cpu_exit_critical(uint32_t old); */
ARM_FUNC_BEGIN(rtk_cpu_exit_critical)
    MSR     BASEPRI, r0
    BX      LR
ARM_FUNC_END()

/*
 * uint32_t rtk_cpu_intr_disable();
 */
ARM_FUNC_BEGIN(rtk_cpu_intr_disable)
    MRS     r0, PRIMASK
    CPSID   I
    BX      LR
ARM_FUNC_END()

/* void rtk_cpu_intr_enable(uint32_t old); */
ARM_FUNC_BEGIN(rtk_cpu_intr_enable)
    MSR     PRIMASK, r0
    BX      LR
ARM_FUNC_END()

#else

/*
 * uint32_t rtk_cpu_intr_disable();
 */
// uint32_t rtk_cpu_enter_critical();
ARM_FUNC_BEGIN(rtk_cpu_enter_critical)
    .global rtk_cpu_intr_disable
rtk_cpu_intr_disable:
    MRS     r0, PRIMASK
    CPSID   I
    BX      LR
ARM_FUNC_END()

/* void rtk_cpu_intr_enable(uint32_t old); */
// void rtk_cpu_exit_critical(uint32_t old);
ARM_FUNC_BEGIN(rtk_cpu_exit_critical)
    .global rtk_cpu_intr_enable
rtk_cpu_intr_enable:
    MSR     PRIMASK, r0
    BX      LR
ARM_FUNC_END()


#endif

    // void rtk_arch_context_switch(void);
    // void rtk_arch_context_switch_interrupt(void);
    .global rtk_arch_context_switch
    .type   rtk_arch_context_switch,%function
ARM_FUNC_BEGIN(rtk_arch_context_switch_interrupt)
rtk_arch_context_switch:
    LDR r0, =NVIC_INT_CTRL              /* trigger the PendSV exception (causes context switch) */
    ldr r1, =NVIC_PENDSVSET
    STR r1, [r0]
    dsb
    BX      LR
ARM_FUNC_END()

    .global _rtk_switch_to_tcb_sp_addr
// psr, pc, lr, r12, r3, r2, r1, r0 are pushed into [from] stack
ARM_FUNC_BEGIN(armv7m_exc_pendsv_Handler)
    /* disable interrupt to protect context switch */
    // 此时属于任务切换时间，可能造成中断延迟
    // r2 -> old_primask
    push        {lr}
#if CONFIG_RTK_SCHEDULE_HOOK
    bl          _rtk_sched_begin_hook_exec;
#endif

#if CONFIG_RTK_ARMV7M_CRITICAL_USE_BASEPRI
    // 直接禁用中断，
    // 此处不用保存原来的BAEPRI的值，因为此刻BAEPRI肯定是0
    mov     r1, #CONFIG_RTK_ARMV7M_CRITICAL_BASEPRI_VAL
    msr     BASEPRI,r1
#else
    // 直接禁用中断，
    // 此处不用保存原来的PRIMASK的值，因为此刻PRIMASk肯定是0
    // 禁用后变为1
    CPSID       I
#endif

    // r0 -> cur task'tcb sp addr
    bl          __rtk_cur_task_stop_exec_ec;

    // r3 -> highest tcb sp addr
    ldr         r3,= _rtk_switch_to_tcb_sp_addr;
    ldr         r3,[r3]

    // 如果当前任务和最高优先级任务相同，则不需要切换
    cmp         r0,r3
    IT          eq
    beq         pendsv_exit

    MRS         r1, psp                             /* get from thread stack pointer */
    STMFD       r1!, {r4 - r11}                     /* push r4 - r11 register */

    // push context flags
#if __RTK_ARMV7M_CONTEXT_USE_FLAGS
    MOV         r4, #0x00                           /* flag = 0 */
#if CONFIG_RTK_USERMODE
    mrs         r5,CONTROL
    TST         r5,0x01                             // check if thread is in privileged mode
    IT          EQ
    orreq       r4,#ARMV7M_CONTEXT_FLAGS_PRIVILEGED
#endif

#if __RTK_ARMV7M_USE_FPU
    ldr         lr,[sp]
    TST         lr, #0x10                               /* if(!EXC_RETURN[4]) */
    ITT         EQ
    orreq       r4, #ARMV7M_CONTEXT_FLAGS_FPU_ENABLE    /* flag |= ARMV7M_CONTEXT_FLAGS_FPU_ENABLE */
    VSTMDBEQ    r1!, {d8 - d15}                         /* push FPU register s16~s31 */
#endif
    STMFD       r1!, {r4}                               /* push flag */
#endif          // __RTK_ARMV7M_CONTEXT_USE_FLAGS

    STR         r1, [r0]                                /* update from thread stack pointer */

switch_to_thread:
    LDR         r1, [r3]                                /* load thread stack pointer */

    // pop context flags
#if __RTK_ARMV7M_CONTEXT_USE_FLAGS
    LDMFD       r1!, {r4}                               /* pop context flag */
#if __RTK_ARMV7M_USE_FPU
    ldr         lr,[sp]
    TST         r4, #ARMV7M_CONTEXT_FLAGS_FPU_ENABLE    /* if(flag_r3 & ARMV7M_CONTEXT_FLAGS_FPU_ENABLE) */
    ITTE        NE
    VLDMIANE    r1!, {d8 - d15}                         /* pop FPU register s16~s31 */
    BICNE       lr, lr, #0x10                           /* lr &= ~(1 << 4), set FPCA. */
    ORREQ       lr, lr, #0x10                           /* lr |=  (1 << 4), clean FPCA. */
    str         lr,[sp]
#endif
#if CONFIG_RTK_USERMODE
    // process privileged mode
    mrs     r5,CONTROL
    // 默认非特权
    orr     r5,#0x01
    // 判断新线程是否在特权模式下
    tst     r4,#ARMV7M_CONTEXT_FLAGS_PRIVILEGED
    it      ne
    bicne   r5,#0x01
    msr     CONTROL,r5
    isb
#endif
#endif

    LDMFD   r1!, {r4 - r11}                             /* pop r4 - r11 register */
    MSR     psp, r1                                     /* update stack pointer */

    mov     r0,r3;
    bl      _rtk_task_begin_exec;

pendsv_exit:
    /* restore interrupt */
#if CONFIG_RTK_ARMV7M_CRITICAL_USE_BASEPRI
    // BASEPRI设置为0，表示启用中断
    mov     r1, #0
    msr     BASEPRI,r1
#else
    CPSIE       I
#endif

#if CONFIG_RTK_SCHEDULE_HOOK
    bl      _rtk_sched_end_hook_exec;
#endif
    pop     {pc}

ARM_FUNC_END()

    // void rtk_arch_start_first_thread(void);
    .global __rtk_highest_tcb_sp_addr_ec
    .global rtk_armv7m_jlink_rtos_info
ARM_FUNC_BEGIN(rtk_arch_start_first_thread)
#if CONFIG_RTK_GDB_JLINK_RTOS_PLUGIN_EN
    /* 此段代码的主要作用是确保rtk_armv7m_jlink_rtos_info在最终的固件中 */
    /* 以便jlink RTOS plugin可以通过这个结构找到追踪的关键信息 */
    ldr     r0,=rtk_armv7m_jlink_rtos_info
#endif
    // 禁用全局中断
    cpsid       I
    // BASEPRI允许所有中断
    mov     r1,#0
    msr     BASEPRI,r1

    /* restore MSP */
    LDR     r1, =SCB_VTOR
    LDR     r1, [r1]
    LDR     r1, [r1]
    NOP
    MSR     msp, r1

    // set initial psp
    bl      __rtk_highest_tcb_sp_addr_ec;
    mov     r2,r0
    ldr     r1,[r0]
    msr     psp,r1

    // now use psp
    mrs     r0,CONTROL
    orr     r0,#2
    msr     CONTROL,r0
    isb

    //强制浮点上下文 not active
    mrs     r0,CONTROL
    bic     r0,#4
    msr     CONTROL,r0
    isb

    // 修改了CONTROL寄存器后，必须使用ISB

    // 强制浮点上下文 not active
    mrs     r0,CONTROL
    bic     r0,#4
    msr     CONTROL,r0
    isb

    mov     r0,r2
    bl      _rtk_task_begin_exec;

    // 现在SP中已经存放了最高优先级的任务上下文
    // 开始弹出最高优先级的任务上下文

    // pop context flags
#if __RTK_ARMV7M_CONTEXT_USE_FLAGS
    pop         {r4}
#if __RTK_ARMV7M_USE_FPU
    // 此刻所有任务都是刚建立好，还没有使用过FPU
#endif
#if CONFIG_RTK_USERMODE
    // process privileged mode
    mrs     r5,CONTROL
    // 默认非特权
    orr     r5,#0x01
    // 判断新线程是否在特权模式下
    tst     r4,#ARMV7M_CONTEXT_FLAGS_PRIVILEGED
    it      ne
    bicne   r5,#0x01
    msr     CONTROL,r5
    isb
#endif
#endif

    // 弹出R4到R11
    pop     {r4 - r11};

    // 弹出r0-r3,r12,lr
    pop     {r0-r3,r12,lr}

    // 临时保存r0,以便后续使用
    push    {r0}
    // 弹出xPSR,因为是第一个任务所以肯定没有使用DSP指令
    ldr     r0,[sp,8]
    msr     xpsr_nzcvq,r0

    // 弹出pc,并将其保存到[sp,8]
    ldr     r0,[sp,4];
    str     r0,[sp,8];

    // 弹出保存的r0
    pop     {r0}

    add     sp,#4
    // 现在SP中就剩下一个PC，任务就开始执行了

    // 打开中断，任务时钟是在中断开启的情况才会切换
    cpsie   i

    // 此时也许会发生中断，但是任务的入口PC被sp保护
    // 所以中断发生后，依然会回到这里

    // 恢复任务的SP，并跳转到入口开始执行
    pop     {pc}
ARM_FUNC_END()

    // uint32 rtk_cpu_ffs(uint32_t data)
ARM_FUNC_BEGIN(rtk_cpu_ffs)
    rbit r0,r0
    clz  r0,r0
    add  r0, #1
    BX      lr
ARM_FUNC_END()

    // void arch_vfp_task_exit(void)
ARM_FUNC_BEGIN(arch_vfp_task_exit)
    BX      lr
ARM_FUNC_END()

