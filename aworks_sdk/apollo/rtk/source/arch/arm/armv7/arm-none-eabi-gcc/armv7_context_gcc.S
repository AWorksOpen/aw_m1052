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
#if _ARMV7_NO_VIRTUALIZATION
    push    {r0}
    ldm     sp,{lr}^
#else
    msr     lr_usr,r0
#endif

    //pc
    ldmia   lr!,{r0}

    // sp
#if _ARMV7_NO_VIRTUALIZATION
    push    {lr}
    ldm     sp,{sp}^
    add     sp,#8
#else
    msr     sp_usr,lr
#endif
    // return
    mov     lr,r0
    pop     {r0}
    movs    pc,lr
ARM_FUNC_END()

ARM_LOCAL_FUNC_BEGIN(context_switch_common)
    push            {r0}

    // 首先，我们先保存任务的上下文，
    // sp_usr ->r0
#if _ARMV7_NO_VIRTUALIZATION
    sub         sp,#4
    stm         sp,{sp}^
    ldr         r0,[sp]

#else
    mrs         r0,sp_usr
#endif

    // 保存任务的PC
    stmfd   r0!,{lr}
    mov     lr,r0
    // 保存任务的lr
#if _ARMV7_NO_VIRTUALIZATION
    stm     sp,{lr}^
    pop     {r0}
#else
    mrs     r0,lr_usr
#endif
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

    clrex
	
    mov     r4,lr
    bl      __rtk_cur_task_stop_exec_ec
    str     r4,[r0]

    // 此刻中断应该已经关闭
    // 前一个任务的上下文已经正确保存，是时候直接切换到新任务了

    // 获得要切换的新任务
    // r0 -> highest tcb sp addr
    b       switch_to_new_task
ARM_FUNC_END()

#if __RTK_ARMV7_USE_FPU

    .data
    .align      4
_use_vfp_before_irq:
    .word   0
_vfp_in_irq_sp:
    .word   0
VFP_FUNC_STACK_START:
    .rept   84
    .word   0
    .endr
VFP_FUNC_STACK_END:
    .word   0

ARM_FUNC_BEGIN(_armv7_undef_vfp_intr_process)
    ldr     r1,=VFP_FUNC_STACK_END;

    // FPSCR
    vmrs    r0,FPSCR
    stmdb   r1!,{r0}

    // D0-D7
    vstmdb  r1!,{d0-d7}
#if _ARMV7_VFP_D32
    vstmdb  r1!,{d16-d31}
#endif
    ldr     r2,=_vfp_in_irq_sp
    str     r1,[r2]
    bx      lr
ARM_FUNC_END()

#endif

/****************************************************************************/
ARM_FUNC_BEGIN(irq_handle)
    // 根据APCS规范，我们需要保存r0-r3,r12和lr
    // 剩下的由编译器负责保存
    stmfd   sp!, {r0-r3,r12,lr}

#if __RTK_ARMV7_USE_FPU
    // 首先保存进入中断处理函数前VFP是否使能
    vmrs    r0,FPEXC
    ldr     r1,=_use_vfp_before_irq
    str     r0,[r1]

    // 然后我们禁用FPU
    mov     r0,#0
    vmsr    FPEXC,r0
#endif

    ldr     r0,_irq_entry
    blx     r0

#if __RTK_ARMV7_USE_FPU
    // 查看irq函数是否使用了VFP
    vmrs    r0,FPEXC
    tst     r0,#ARMV7_FPEXC_FPU_ENABLE_FLAGS
    beq     1f
    // irq函数使用了VFP
    // 我们必须要恢复上下文
    ldr     r1,=_vfp_in_irq_sp;
    ldr     r1,[r1]
#if _ARMV7_VFP_D32
    VLDMIA  r1!,{d16-d31}
#endif
    VLDMIA  r1!,{d0-d7}
    // 弹出FPSCR
    ldmia   r1!,{r0}
    vmsr    FPSCR,r0
1:
    // 恢复FPEXC
    ldr     r1,=_use_vfp_before_irq
    ldr     r0,[r1]
    vmsr    FPEXC,r0

#endif

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
    cpsid       i;
#if CONFIG_RTK_GDB_JLINK_RTOS_PLUGIN_EN
    /* 此段代码的主要作用是确保rtk_armv7m_jlink_rtos_info在最终的固件中 */
    /* 以便jlink RTOS plugin可以通过这个结构找到追踪的关键信息 */
    ldr     r0,=rtk_armv7m_jlink_rtos_info
#endif
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
