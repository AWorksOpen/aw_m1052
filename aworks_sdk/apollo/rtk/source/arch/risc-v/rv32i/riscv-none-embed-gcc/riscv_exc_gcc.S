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
 * \file riscv_exc_gcc.S
 * \brief riscv exception handler.
 *
 * \internal
 * \par History
 * \endinternal
 */

/****************************************************************************/
    .global __g_illegal_instruction_handler
    .global __g_lsu_err_handler

#define EXCEPTION_STACK_SIZE 0x5c

    .type store_regs, %function
store_regs:
    sw  x3, 0x00(x2)  // gp
    sw  x4, 0x04(x2)  // tp
    sw  x5, 0x08(x2)  // t0
    sw  x6, 0x0c(x2)  // t1
    sw  x7, 0x10(x2)  // t2
                      // a0 is already saved
    sw x11, 0x18(x2)  // a1
    sw x12, 0x1c(x2)  // a2
    sw x13, 0x20(x2)  // a3
    sw x14, 0x24(x2)  // a4
    sw x15, 0x28(x2)  // a5
    sw x16, 0x2c(x2)  // a6
    sw x17, 0x30(x2)  // a7
    csrr a0, 0x341
    addi a0, a0, -0x8 // attention this!!!
    sw a0, 0x4c(x2)   // mepc
    csrr a1, 0x300
    li a2, 0xFFFF1800 // disable interrupt
    and a1, a1, a2
    sw a1, 0x50(x2)  // mstatus
                     // ra is already saved
                     // sp is already saved
    jalr x0, x1

    .type restore_regs, %function
restore_regs:
    lw a1, 0x50(x2)  // mstatus
    csrrw x0, 0x300, a1
    lw a0, 0x4c(x2)  // mepc
    csrrw x0, 0x341, a0
    lw  x3, 0x00(x2)  // gp
    lw  x4, 0x04(x2)  // tp
    lw  x5, 0x08(x2)  // t0
    lw  x6, 0x0c(x2)  // t1
    lw  x7, 0x10(x2)  // t2
    lw x10, 0x14(x2)  // a0
    lw x11, 0x18(x2)  // a1
    lw x12, 0x1c(x2)  // a2
    lw x13, 0x20(x2)  // a3
    lw x14, 0x24(x2)  // a4
    lw x15, 0x28(x2)  // a5
    lw x16, 0x2c(x2)  // a6
    lw x17, 0x30(x2)  // a7

    lw  x1, 0x54(x2)
    addi x2, x2, EXCEPTION_STACK_SIZE
    lw x2, -0x04(x2)
    mret

    .type __g_lsu_err_handler, %function
__g_lsu_err_handler:
    addi x2, x2, -0x4
    sw a0, 0x00(x2)
    mv a0, x2
    la x2, __StackTop
    addi x2, x2, -EXCEPTION_STACK_SIZE
    addi a0, a0, 0x4
    sw a0, 0x58(x2)
    lw a0, -0x04(a0)
    sw a0, 0x14(x2)
    sw x1, 0x54(x2)
    jal x1, store_regs
    csrr a0, 0x341
    mv a0, x2
    jal x1, lsu_error
    jal x1, restore_regs

    .type __g_illegal_instruction_handler, %function
__g_illegal_instruction_handler:
    addi x2, x2, -0x4
    sw a0, 0x00(x2)
    mv a0, x2
    la x2, __StackTop
    addi x2, x2, -EXCEPTION_STACK_SIZE
    addi a0, a0, 0x4
    sw a0, 0x58(x2)
    lw a0, -0x04(a0)
    sw a0, 0x14(x2)
    sw x1, 0x54(x2)
    jal x1, store_regs
    csrr a0, 0x341
    mv a0, x2
    jal x1, illegal_instruction
    j .

/* end of file */
