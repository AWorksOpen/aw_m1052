/**
 * \file
 * \brief ARMV4 exception handler.
 *
 * \internal
 * \par History
 * - 140504 liuweiyun, first implementation.
 * \endinternal
 */

/****************************************************************************/
#include "../armv4_context.h"
#include "armv4_gcc_common.h"

#define     ENABLE_VFP_IN_INTR

    .data
    .align      4
EN_VFP_IN_INTR:
    .word   0

/****************************************************************************/
    .global __g_fatal_err_context
ARM_FUNC_BEGIN(data_abort_handler)
    //这里的一个nop指令，作用比较奇特，当你正在调试其中运行时，突然发生了数据终止
    //首先假设你并未在data_abort_handler设置断点，那么数据终止处理代码会很快输出错误
    //信息，并停留在死循环,这时点击eclipse 上的suspend按钮，当前执行位置就会停止死循环处
    //这时可以查看调用栈找到引发错误的代码，没有这个nop指令则不可以，这可能是
    //arm-none-eabi-gdb分析问题的特性
    nop
    //保存上下文
    stmfd   sp!, {r0-r12,lr}

    //获取保存
    push {r1}
    push {r0}
    LDR r0,=__g_fatal_err_context
    pop {r1}
    str R1,[R0,#0]
    pop {r1}
    str R1,[R0,#4]
    str R2,[R0,#8]
    str R3,[R0,#12]
    str R4,[R0,#16]
    str R5,[R0,#20]
    str R6,[R0,#24]
    str R7,[R0,#28]
    str R8,[R0,#32]
    str R9,[R0,#36]
    str R10,[R0,#40]
    str R11,[R0,#44]
    str R12,[R0,#48]
    //保存异常返回地址
    str LR,[R0,#60]

    //保存异常状态下的CPSR
    mrs r1,cpsr
    str r1,[R0,#64]
     //保存异常状态下的SPSR
    mrs r1,spsr
    str r1,[R0,#68]
    //保存fault状态和地址
    mrc p15,0,r1,c5,c0,0
    mrc p15,0,r2,c6,c0,0
    str r1,[R0,#72]
    str r2,[R0,#76]

    //获取产生异常的LR和SP,直接切换回原模式获取
    mrs r1,spsr
    mrs r2,cpsr

    orr r1,r1,#(CPSR_FLAG_IRQ_MASK|CPSR_FLAG_FIQ_MASK)
    bic r1,#CPSR_FLAG_THUMB
    msr cpsr_c,r1
    mov r3,sp
    mov r4,lr
    msr cpsr,r2
    //保存获取到的sp和lr
    str r3,[R0,#52]
    str r4,[R0,#56]

    push {lr}
    bl      data_abort
    pop {lr}
1:
    b 1b
    //恢复上下文，并返回
    ldmfd   sp!, {r0-r12,lr}
    subs    pc, lr, #8
_data_abort:
    .word data_abort
ARM_FUNC_END()

/****************************************************************************/
ARM_FUNC_BEGIN(prefetch_abort_handler)
    stmfd   sp!, {r0-r12,lr}

    //获取保存错误上下文的地址并保存主要寄存器
    push {r1}
    push {r0}
    LDR r0,=__g_fatal_err_context
    pop {r1}
    str R1,[R0,#0]
    pop {r1}
    str R1,[R0,#4]
    str R2,[R0,#8]
    str R3,[R0,#12]
    str R4,[R0,#16]
    str R5,[R0,#20]
    str R6,[R0,#24]
    str R7,[R0,#28]
    str R8,[R0,#32]
    str R9,[R0,#36]
    str R10,[R0,#40]
    str R11,[R0,#44]
    str R12,[R0,#48]
    //保存异常返回地址
    str LR,[R0,#60]

    //保存异常状态下的CPSR
    mrs r1,cpsr
    str r1,[R0,#64]
     //保存异常状态下的SPSR
    mrs r1,spsr
    str r1,[R0,#68]
    //保存fault状态和地址
    mrc p15,0,r1,c5,c0,1
    mrc p15,0,r2,c6,c0,0
    str r1,[R0,#72]
    str r2,[R0,#76]
    //获取产生异常的LR和SP
    mrs r1,spsr
    mrs r2,cpsr
    orr r1,r1,#(CPSR_FLAG_IRQ_MASK|CPSR_FLAG_FIQ_MASK)
    bic r1,#CPSR_FLAG_THUMB
    msr cpsr_c,r1
    mov r3,sp
    mov r4,lr
    msr cpsr,r2
    //保存获取到的sp和lr
    str r3,[R0,#52]
    str r4,[R0,#56]

    ldr r1,_prefetch_abort
    mov lr, pc
    bx      r1

    ldmfd   sp!, {r0-r12,lr}
    subs    pc, lr, #4
_prefetch_abort:
    .word prefetch_abort
ARM_FUNC_END()

    .global arch_vfp_task_startup
    .global task_self
    .global rtk_is_int_context
    .global undefined_instruction_intr_vfp
    .global arch_vfp_intr_save_context
/****************************************************************************/
ARM_FUNC_BEGIN(undef_instruction_handler)
    stmfd   sp!, {r0-r12,lr}

    //获取保存错误上下文的地址并保存主要寄存器
    push {r1}
    push {r0}
    LDR r0,=__g_fatal_err_context
    pop {r1}
    str R1,[R0,#0]
    pop {r1}
    str R1,[R0,#4]
    str R2,[R0,#8]
    str R3,[R0,#12]
    str R4,[R0,#16]
    str R5,[R0,#20]
    str R6,[R0,#24]
    str R7,[R0,#28]
    str R8,[R0,#32]
    str R9,[R0,#36]
    str R10,[R0,#40]
    str R11,[R0,#44]
    str R12,[R0,#48]
    //保存异常返回地址
    str LR,[R0,#60]

    //保存异常状态下的CPSR
    mrs r1,cpsr
    str r1,[R0,#64]
     //保存异常状态下的SPSR
    mrs r1,spsr
    str r1,[R0,#68]
    //保存fault状态和地址
    mrc p15,0,r1,c5,c0,1
    mrc p15,0,r2,c6,c0,0
    str r1,[R0,#72]
    str r2,[R0,#76]
    //获取产生异常的LR和SP
    mrs r1,spsr
    mrs r2,cpsr
    orr r1,r1,#(CPSR_FLAG_IRQ_MASK|CPSR_FLAG_FIQ_MASK)
    bic r1,#CPSR_FLAG_THUMB
    msr cpsr_c,r1
    mov r3,sp
    mov r4,lr
    msr cpsr,r2
    //保存获取到的sp和lr
    str r3,[R0,#52]
    str r4,[R0,#56]

    ldr r1,_undefined_instruction
    mov lr, pc
    bx      r1
    ldmfd   sp!, {r0-r12,lr}
    subs    pc, lr, #4
_undefined_instruction:
    .word undefined_instruction
ARM_FUNC_END()

/****************************************************************************/
ARM_FUNC_BEGIN(fiq_handler)
    stmfd   sp!, {r0-r12,lr}

    //我们并不使用swi，执行到这里意味着程序可能除了问题
    //获取保存错误上下文的地址并保存主要寄存器
    push {r1}
    push {r0}
    LDR r0,=__g_fatal_err_context
    pop {r1}
    str R1,[R0,#0]
    pop {r1}
    str R1,[R0,#4]
    str R2,[R0,#8]
    str R3,[R0,#12]
    str R4,[R0,#16]
    str R5,[R0,#20]
    str R6,[R0,#24]
    str R7,[R0,#28]
    str R8,[R0,#32]
    str R9,[R0,#36]
    str R10,[R0,#40]
    str R11,[R0,#44]
    str R12,[R0,#48]
    //保存异常返回地址
    str LR,[R0,#60]

    //保存异常状态下的CPSR
    mrs r1,cpsr
    str r1,[R0,#64]
     //保存异常状态下的SPSR
    mrs r1,spsr
    str r1,[R0,#68]
    //保存fault状态和地址
    mrc p15,0,r1,c5,c0,1
    mrc p15,0,r2,c6,c0,2
    str r1,[R0,#72]
    str r2,[R0,#76]
    //获取产生异常的LR和SP
    mrs r1,spsr
    mrs r2,cpsr
    orr r1,r1,#(CPSR_FLAG_IRQ_MASK|CPSR_FLAG_FIQ_MASK)
    bic r1,#CPSR_FLAG_THUMB
    msr cpsr_c,r1
    mov r3,sp
    mov r4,lr
    msr cpsr,r2
    //保存获取到的sp和lr
    str r3,[R0,#52]
    str r4,[R0,#56]

    ldr r1,__fiq_handle
    mov lr, pc
    bx      r1
    ldmfd   sp!, {r0-r12,lr}
    subs    pc, lr, #4
__fiq_handle:
    .word fiq_handle
ARM_FUNC_END()


/****************************************************************************/
ARM_FUNC_BEGIN(arch_enable_vfp_in_intr)
#ifdef __RTK_ARMV7_USE_FPU
    push    {r0-r1}
    mov     r1,#1
    ldr     r0,=EN_VFP_IN_INTR;
    str     r1,[r0]
    pop     {r0-r1}
#endif
    bx      lr;
ARM_FUNC_END()

/* end of file */
