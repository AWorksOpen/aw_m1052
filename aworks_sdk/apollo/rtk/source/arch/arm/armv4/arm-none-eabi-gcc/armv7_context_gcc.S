#include "../armv7_context.h"
#include "armv7_gcc_common.h"

// static void switch_to_new_task(void);
ARM_LOCAL_FUNC_BEGIN(switch_to_new_task)
    ldr     r6,  =_rtk_switch_to_tcb_sp_addr
    ldr     r6,  [r6]
    mov     r0,r6

    bl      _rtk_task_begin_exec;
    ldr     lr,  [r6]                   /* get new task's stack pointer     */
#if __RTK_ARMV7_USE_FPU
    // 弹出FPEXC
    ldmia   lr!,{r0}
    vmsr    FPEXC,r0
    tst     r0,#ARMV7_FPEXC_FPU_ENABLE_FLAGS

    //如果本任务VFP被禁用，则无需恢复，直接退出
    beq     1f
#if _ARMV7_VFP_D32
    VLDMIA  lr!,{d16-d31}
#endif
    // 执行实际的还原操作
    VLDMIA  lr!,{d0-d15}
    ldmia   lr!,{r0}
    VMSR    FPSCR,r0
1:
#endif

    // CPSR
    ldmfd   lr!, {r4}       @ pop new task spsr
    msr     spsr_cxsf, r4

    // r0-r12
    ldmia   lr!,{r0-r12}
    // lr
    push    {r0}
    ldmia   lr!,{r0}
    msr     lr_usr,r0

    //pc
    ldmia   lr!,{r0}

    // sp
    msr     sp_usr,lr
    // return
    mov     lr,r0
    pop     {r0}
    movs    pc,lr
    movs    pc,lr
ARM_FUNC_END()

ARM_LOCAL_FUNC_BEGIN(context_switch_common)
    push            {r0}

    // 首先，我们先保存任务的上下文，
    // sp_usr ->r0
    mrs         r0,sp_usr

    // 保存任务的PC
    stmfd   r0!,{lr}
    mov     lr,r0
    // 保存任务的lr
    mrs     r0,lr_usr
    stmfd   lr!,{r0}
    // r0-r12
    pop     {r0}
    stmfd   lr!,{r0-r12}
    // 保存任务的CPSR
    mrs     r0,spsr
    stmfd   lr!,{r0}

#if __RTK_ARMV7_USE_FPU
    // 保存FPEXC
    vmrs    r0,FPEXC

    tst     r0,#ARMV7_FPEXC_FPU_ENABLE_FLAGS

    //如果本任务VFP被禁用，则无需保存VFP剩余的内容，退出
    beq     1f
    // 执行实际的保存操作
    vmrs    r1,FPSCR
    stmdb   lr!,{r1}
    VSTMDB  lr!,{d0-d15}
#if _ARMV7_VFP_D32
    VSTMDB  lr!,{d16-d31}
#endif
1:
    stmdb   lr!,{r0}
#endif

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
    mov     lr, pc
    bx      r0

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
    cpsid           i;

    b               context_switch_common
ARM_FUNC_END()

/*
 * uint32_t rtk_cpu_enter_critical();
 */
ARM_FUNC_BEGIN(rtk_cpu_enter_critical)
    mrs         r0, cpsr
    cpsid       i
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
    cpsid       i;
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

ARM_FUNC_BEGIN(rtk_cpu_ffs)
    rbit r0,r0
    clz  r0,r0
    add  r0, #1
    BX      lr
ARM_FUNC_END()
