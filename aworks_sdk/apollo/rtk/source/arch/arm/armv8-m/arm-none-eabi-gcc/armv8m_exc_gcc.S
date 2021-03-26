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
 * \file armv8m_exc_gcc.S
 * \brief rtk中armv8m架构执行相关函数的arm-none-eabi-gcc汇编实现，主要涉及到fault处理
 *
 * \par 源代码
 * \internal
 * \par Modification history
 * - 1.10 2019-09-02, A.J, armv8-m arch.
 * - 1.00 2018-06-13 sni, first version.
 * \endinternal
 */

#include "rtk_config.h"
#include "armv8m_gcc_common.h"

    .global __g_fatal_err_context
    .global armv8m_show_fault_error


/*
 * void armv8m_process_fault_common();
 */
ARM_FUNC_BEGIN(armv8m_process_fault_common)
    //获取保存
    ldr     r0,=__g_fault_err_context

    // R4-R11
    str R4,[r0,#16]
    str R5,[r0,#20]
    str R6,[r0,#24]
    str R7,[r0,#28]
    str R8,[r0,#32]
    str R9,[r0,#36]
    str R10,[r0,#40]
    str R11,[r0,#44]

    // LR_FATAL
    str lr,[r0,#76]
    // MSP
    mrs r1,msp
    str r1,[r0,#68]
    // PSP
    mrs r1,psp
    str r1,[r0,#72]

    // 找到异常栈帧保存的位置
    mrs     r1,msp
    tst     lr,0x4
    it      ne
    mrsne   r1,psp

    // r0~r3
    ldr r2,[r1,#0]
    str r2,[r0,#0]
    ldr r2,[r1,#4]
    str r2,[r0,#4]
    ldr r2,[r1,#8]
    str r2,[r0,#8]
    ldr r2,[r1,#12]
    str r2,[r0,#12]
    // r12
    ldr r2,[r1,#16]
    str r2,[r0,#48]

    // lr
    ldr r2,[r1,#20]
    str r2,[r0,#56]

    // pc
    ldr r2,[r1,#24]
    str r2,[r0,#60]

    // xPSR
    ldr r2,[r1,#28]
    str r2,[r0,#64]
    // sp
    add r2,r1,#32
    str r2,[r0,#52]

    // 调用错误打印函数
    push {LR}
    bl armv8m_show_fault_error
    pop {lr}
    BX      LR
ARM_FUNC_END()

    /*
    * void armv8m_exc_memmanage_handler();
    */
    .global armv8m_exc_memmanage_handler
    .type armv8m_exc_memmanage_handler, %function
    .global armv8m_exc_busfault_handler
    .type armv8m_exc_busfault_handler, %function
    .global armv8m_exc_usagefault_handler
    .type armv8m_exc_usagefault_handler,%function
ARM_FUNC_BEGIN(armv8m_exc_hardfault_handler)
armv8m_exc_memmanage_handler:
armv8m_exc_busfault_handler:
armv8m_exc_usagefault_handler:
    ldr             r0, = armv8m_process_fault_common
    bx              r0
    b               .
    bx              lr
ARM_FUNC_END()

    .set BIT_CONTROL_PRIV_EN,           0x01

    .global armv8m_exec_syscalls
    // void armv8m_exc_svcall_handler(void)
ARM_FUNC_BEGIN(armv8m_exc_svcall_handler)
#if CONFIG_RTK_USERMODE
    mrs  r1, psp

    ldr  r0, =armv8m_exec_syscalls
    str  r0, [r1, #24]

    //设置为特权模式
    mrs r0,  CONTROL
    bic r0,  BIT_CONTROL_PRIV_EN
    msr CONTROL, r0

    bx   lr
#else
    b   .
#endif

ARM_FUNC_END()

// int rtk_arch_is_privileged_mode(void)
ARM_FUNC_BEGIN(rtk_arch_is_privileged_mode)
    //检查当前是否为特权模式
    mov     r0,#0
    mrs     r1,  CONTROL
    tst     r1, 0x01
    it      eq
    moveq   r0,#1
    bx      lr

ARM_FUNC_END()

// void arch_switch_to_unprivileged(void)
ARM_FUNC_BEGIN(arch_switch_to_unprivileged)
    mrs     r0,CONTROL
    orr     r0,#0x01
    msr     CONTROL,r0;
ARM_FUNC_END()

#define SYSCALL_DEF(name,syscall_number) ARM_FUNC_BEGIN(name); \
            push    {r0-r3};\
            ldr     r0,= ##syscall_number; \
            svc     0; \
            bx      lr; \
            ARM_FUNC_END()
SYSCALL_DEF(rtk_arch_exec_task_exit_syscall,0);
