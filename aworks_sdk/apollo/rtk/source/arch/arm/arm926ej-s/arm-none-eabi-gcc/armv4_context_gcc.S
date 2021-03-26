#include "../armv4_context.h"
#include "armv4_gcc_common.h"

// static void switch_to_new_task(void);
ARM_LOCAL_FUNC_BEGIN(switch_to_new_task)
    ldr     r6,  =_rtk_switch_to_tcb_sp_addr
    ldr     r6,  [r6]
    mov     r0,r6

    bl      _rtk_task_begin_exec;
    ldr     lr,  [r6]                   /* get new task's stack pointer     */


    // CPSR
    ldmfd   lr!, {r4}       @ pop new task spsr
    msr     spsr_cxsf, r4

    // r0-r12,lr
    ldmia   lr,{r0-r12,lr}^
    nop
    add     lr,#56

    //pc
    push    {r0}
    ldmia   lr!,{r0}

    // sp
    push    {lr}
    ldmia   sp,{sp}^
    nop
    add     sp,#4

    // return
    mov     lr,r0
    pop     {r0}
    movs    pc,lr
ARM_FUNC_END()

ARM_LOCAL_FUNC_BEGIN(context_switch_common)
    push            {r0}

    // 首先，我们先保存任务的上下文，
    // sp_usr ->r0
    sub         sp,#4
    stm         sp,{sp}^
    nop
    pop         {r0}

    // 保存任务的PC
    stmfd   r0!,{lr}
    mov     lr,r0
    // r0-r12,lr
    pop     {r0}
    stmfd   lr,{r0-r12,lr}^
    nop
    sub     lr,#56
    // 保存任务的CPSR
    mrs     r0,spsr
    stmfd   lr!,{r0}

    mov     r4,lr
    bl      __rtk_cur_task_stop_exec_ec
    str     r4,[r0]

    // 此刻中断应该已经关闭
    // 前一个任务的上下文已经正确保存，是时候直接切换到新任务了

    // 获得要切换的新任务
    // r0 -> highest tcb sp addr
    b       switch_to_new_task
ARM_FUNC_END()

/****************************************************************************/
ARM_FUNC_BEGIN(irq_handle)
    // 根据APCS规范，我们需要保存r0-r3,r12和lr
    // 剩下的由编译器负责保存

    stmfd   sp!, {r0-r3,r12,lr}

    ldr     r0,_irq_entry
    blx     r0

    /* if rt_thread_switch_interrupt_flag set, jump to _interrupt_thread_switch and don't return */
    ldr     r0, =rt_thread_switch_interrupt_flag
    ldr     r1, [r0]
    cmp     r1, #1
    beq     _interrupt_thread_switch

    ldmfd   sp!, {r0-r3,r12,lr}
    subs    pc, lr, #4
_irq_entry:
    .word irq_entry

_interrupt_thread_switch:
    mov     r1,  #0                 /* clear rt_thread_switch_interrupt_flag*/
    str     r1,  [r0]

    ldmfd   sp!, {r0-r3,r12,lr}     /* reload saved registers           */
    sub     lr,#4
    b       context_switch_common
ARM_FUNC_END()

/****************************************************************************/
ARM_FUNC_BEGIN(swi_handler)
    // 此刻中断肯定打开，先关闭中断
    push            {r0}
    mrs             r0,cpsr
    orr             r0,#CPSR_FLAG_IRQ_MASK
    msr             cpsr,r0
    pop             {r0}

    b               context_switch_common
ARM_FUNC_END()

/*
 * uint32_t rtk_cpu_enter_critical();
 */
ARM_FUNC_BEGIN(rtk_cpu_enter_critical)
    mrs         r0, cpsr
    orr         r1,r0,#CPSR_FLAG_IRQ_MASK
    msr         cpsr,r1
    bx          lr
ARM_FUNC_END()

/*
 * void rtk_cpu_exit_critical(uint32_t old_int_flags);
 */
ARM_FUNC_BEGIN(rtk_cpu_exit_critical)
    msr         cpsr_c, r0
    bx          lr
ARM_FUNC_END()

ARM_FUNC_BEGIN(rtk_cpu_is_int_context)
    mov         r0,#0
    mrs         r1, cpsr
    and         r1,r1,#CPSR_MODE_MASK
    cmp         r1,#CPSR_MODE_IRQ
    moveq       r0,#1
    bx          lr
ARM_FUNC_END()

ARM_FUNC_BEGIN(rtk_cpu_is_fault_context)
    mov         r0,#0
    mrs         r1, cpsr
    and         r1,r1,#CPSR_MODE_MASK
    cmp         r1,#CPSR_MODE_UNDEF
    moveq       r0,#1
    cmp         r1,#CPSR_MODE_ABORT
    moveq       r0,#1
    bx          lr
ARM_FUNC_END()

ARM_FUNC_BEGIN(rtk_arch_context_switch)
    // 首先禁用中断，阻止任务调度等
    svc         #0
    bx          lr
ARM_FUNC_END()

/*
 * void rtk_arch_start_first_thread(void);
 */
    .global     _rtk_switch_to_tcb_sp_addr
ARM_FUNC_BEGIN(rtk_arch_start_first_thread)
    mrs         r0, cpsr
    orr         r1,r0,#CPSR_FLAG_IRQ_MASK
    msr         cpsr,r1
    bl          _rtk_highest_task_loop_get_ec
    ldr         r1,=_rtk_switch_to_tcb_sp_addr;
    str         r0,[r1]
    b           switch_to_new_task
ARM_FUNC_END()

/*
 * void arch_context_switch_interrupt(rt_uint32 from, rt_uint32 to);
 */
    .global rt_thread_switch_interrupt_flag
    .global rt_interrupt_from_thread
    .global rt_interrupt_to_thread
ARM_FUNC_BEGIN(rtk_arch_context_switch_interrupt)
    ldr     r0, =rt_thread_switch_interrupt_flag
    mov     r1,#1
    str     r1,[r0]
    bx      lr
ARM_FUNC_END()


