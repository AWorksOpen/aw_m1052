/**************************************************************************
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
 * \file riscv_context_gcc.S
 * \brief riscv context switch handle.
 *
 * \internal
 * \par History
 * \endinternal
 */

    .text
    .global __SAVE_CONTEXT
    .global __RESTORE_CONTEXT
    .global __switch_to_new_task
    .global global_interrupt_status
    .global rtk_cpu_enter_critical
    .global rtk_cpu_exit_critical
    .global rtk_arch_context_switch
    .global rtk_arch_start_first_thread
    .global rtk_cpu_is_int_context
    .global rtk_cpu_is_fault_context
    .global riscv_get_current_int_vector_addr
    .global Ecall_Handler

    .type rtk_cpu_enter_critical, %function
rtk_cpu_enter_critical:
    csrrci a0, mstatus, 8
    ret

    .type rtk_cpu_exit_critical, %function
rtk_cpu_exit_critical:
    csrw mstatus, a0
    ret

    .type riscv_get_int_vector_addr, %function
riscv_get_current_int_vector_addr:
    csrr a0, 0x305
    nop
    ret

    .type rtk_cpu_is_int_context, %function
rtk_cpu_is_int_context:
    lui a0, 0xE0041
    addi a0, a0, 40
    lw a0, 0(a0)
    snez a0, a0
    andi a0, a0, 255
    ret

    .type rtk_cpu_is_fault_context, %function
rtk_cpu_is_fault_context:
    addi sp, sp, -32
    sw a3, 28(sp)
    sw a4, 24(sp)
    sw a5, 20(sp)
    csrr a0, mcause
    mv a5, a0
    srli a5, a5, 31
    beqz a5, has_exception_occured
    li a0, 0
return_result:
    lw a3, 28(sp)
    lw a4, 24(sp)
    lw a5, 20(sp)
    addi sp, sp, 32
    ret
has_exception_occured:
    mv      a5, a0
    andi    a4, a5, 0x1F
    li      a0, 1
    li      a3, 2
    beq     a4, a3, return_result
    andi    a4, a5, 0x1F
    li      a3, 5
    beq     a4, a3, return_result
    andi    a4, a5, 0x1F
    li      a3, 7
    beq     a4, a3, return_result
    li      a0, 0
    j       return_result

    .type global_interrupt_status, %function
global_interrupt_status:
    csrr a0, 0x300
    andi a0, a0, 0x08
    ret

    .type rtk_arch_context_switch, %function
rtk_arch_context_switch:
    ecall
    ret

#define TASK_CONTEXT_SIZE 0x7c
    .type __SAVE_CONTEXT, %function
__SAVE_CONTEXT:
    addi sp, sp, -TASK_CONTEXT_SIZE
    sw x10, 0x54(sp)
    sw x11, 0x58(sp)
    sw x12, 0x5c(sp)
    sw x13, 0x60(sp)
    sw x14, 0x64(sp)
    sw x15, 0x68(sp)
    sw x16, 0x6c(sp)
    sw x17, 0x70(sp)
                     // ra is already saved
    csrr a0, 0x300
    sw a0, 0x00(sp)  // mstatus
    sw x8, 0x04(sp)  // s0
    sw x9, 0x08(sp)  // s1
    sw x18, 0x0c(sp)
    sw x19, 0x10(sp)
    sw x20, 0x14(sp)
    sw x21, 0x18(sp)
    sw x22, 0x1c(sp)
    sw x23, 0x20(sp)
    sw x24, 0x24(sp)
    sw x25, 0x28(sp)
    sw x26, 0x2c(sp)
    sw x27, 0x30(sp)
    sw x28, 0x34(sp)
    sw x29, 0x38(sp)
    sw x30, 0x3c(sp)
    sw x31, 0x40(sp)
    sw x4, 0x44(sp)  // tp
    sw x5, 0x48(sp)  // t0
    sw x6, 0x4c(sp)  // t1
    sw x7, 0x50(sp)  // t2
    jalr x0, x1
    
    .type __RESTORE_CONTEXT, %function
__RESTORE_CONTEXT:
    lw a0, 0x00(sp)  /* mstatus */
    csrrw x0, 0x300, a0
    lw x8, 0x04(sp)  /* x8-x9, x18-x31 */
    lw x9, 0x08(sp)
    lw x18, 0x0c(sp)
    lw x19, 0x10(sp)
    lw x20, 0x14(sp)
    lw x21, 0x18(sp)
    lw x22, 0x1c(sp)
    lw x23, 0x20(sp)
    lw x24, 0x24(sp)
    lw x25, 0x28(sp)
    lw x26, 0x2c(sp)
    lw x27, 0x30(sp)
    lw x28, 0x34(sp)
    lw x29, 0x38(sp)
    lw x30, 0x3c(sp)
    lw x31, 0x40(sp)
    lw a0, 0x78(sp) /* mepc */
    csrrw x0, 0x341, a0
    lw  x1, 0x74(sp)  // ra
    lw  x4, 0x44(sp)  // tp
    lw  x5, 0x48(sp)  // t0
    lw  x6, 0x4c(sp)  // t1
    lw  x7, 0x50(sp)  // t2
    lw x10, 0x54(sp)  // a0
    lw x11, 0x58(sp)  // a1
    lw x12, 0x5c(sp)  // a2
    lw x13, 0x60(sp)  // a3
    lw x14, 0x64(sp)  // a4
    lw x15, 0x68(sp)  // a5
    lw x16, 0x6c(sp)  // a6
    lw x17, 0x70(sp)  // a7
    addi sp, sp, TASK_CONTEXT_SIZE
    mret
    
    .type __switch_to_new_task, %function
__switch_to_new_task:
    lw a0, _rtk_switch_to_tcb_sp_addr
    mv s1, a0
    jal x1, _rtk_task_begin_exec
    lw sp, (s1)
    j __RESTORE_CONTEXT

    .type Ecall_handler, %function
Ecall_Handler:
    sw x1, -0x08(sp) /* first saved ra before change it! */
    jal x1, __SAVE_CONTEXT
    csrr a0, 0x341   /* mepc + 4 point to next instruction after exception instruction */
    addi a0, a0, 4
    sw a0, 0x78(sp)
    mv s1, sp
    la sp, __StackTop /* change sp to system stask */
    jal x1, __rtk_cur_task_stop_exec_ec
    sw  s1, (a0)     /* save task's current sp to sp in task struct */
    j __switch_to_new_task

    .type rtk_arch_start_first_thread, %function
rtk_arch_start_first_thread:
    jal x1, _rtk_highest_task_loop_get_ec  /* sp is the first element in task struct */
    la a1, _rtk_switch_to_tcb_sp_addr
    sw a0, (a1)
    j __switch_to_new_task


/* end of file */
