
                PRESERVE8
                THUMB

BIT_CONTROL_PRIV_EN     EQU     0x01

; macros
    MACRO
$label  __FUNC_DECL
$label  PROC
    EXPORT  $label
    MEND

    MACRO
$label  __LOCAL_FUNC_DECL
$label  PROC
    MEND

    MACRO
$label  __FUNC_END
    ENDP
    MEND

                AREA    |.text|, CODE, READONLY
                IMPORT  armv8m_show_fault_error
                IMPORT  __g_fault_err_context
                IMPORT  armv8m_exec_syscalls

; void armv8m_process_fault_common();
armv8m_process_fault_common __FUNC_DECL

; 获取保存
    ldr     r0,=__g_fault_err_context

; R4-R11
    str R4,[r0,#16]
    str R5,[r0,#20]
    str R6,[r0,#24]
    str R7,[r0,#28]
    str R8,[r0,#32]
    str R9,[r0,#36]
    str R10,[r0,#40]
    str R11,[r0,#44]

; LR_FATAL
    str lr,[r0,#76]
; MSP
    mrs r1,msp
    str r1,[r0,#68]
; PSP
    mrs r1,psp
    str r1,[r0,#72]

; 找到异常栈帧保存的位置
    mrs     r1,msp
    tst     lr,#0x4
    it      ne
    mrsne   r1,psp

; r0~r3
    ldr r2,[r1,#0]
    str r2,[r0,#0]
    ldr r2,[r1,#4]
    str r2,[r0,#4]
    ldr r2,[r1,#8]
    str r2,[r0,#8]
    ldr r2,[r1,#12]
    str r2,[r0,#12]
; r12
    ldr r2,[r1,#16]
    str r2,[r0,#48]

; lr
    ldr r2,[r1,#20]
    str r2,[r0,#56]

; pc
    ldr r2,[r1,#24]
    str r2,[r0,#60]

; xPSR
    ldr r2,[r1,#28]
    str r2,[r0,#64]
; sp
    add r2,r1,#32
    str r2,[r0,#52]

; 调用错误打印函数
    push {LR}
    bl armv8m_show_fault_error
    pop {lr}
    BX      LR
    __FUNC_END

armv8m_exc_hardfault_handler
    EXPORT  armv8m_exc_hardfault_handler
armv8m_exc_memmanage_handler
    EXPORT  armv8m_exc_memmanage_handler
armv8m_exc_busfault_handler
    EXPORT  armv8m_exc_busfault_handler
armv8m_exc_usagefault_handler   PROC
    EXPORT  armv8m_exc_usagefault_handler

    ldr             r0, = armv8m_process_fault_common
    bx              r0
    b               .

    __FUNC_END

;函数实现: void armv8m_exc_svcall_handler(void)
armv8m_exc_svcall_handler __FUNC_DECL
#if CONFIG_RTK_USERMODE
    mrs  r1, psp

    ldr  r0, =armv8m_exec_syscalls
    str  r0, [r1, #24]

    ; 设置为特权模式
    mrs r0,  CONTROL
    bic r0,  BIT_CONTROL_PRIV_EN
    msr CONTROL, r0

    bx   lr
#else
    b   .
#endif
    __FUNC_END


;函数实现: int rtk_arch_is_privileged_mode(void)
rtk_arch_is_privileged_mode __FUNC_DECL
    ; 检查当前是否为特权模式
    mov     r0, #0
    mrs     r1, CONTROL
    tst     r1, #0x01
    it      eq
    moveq   r0, #1
    bx      lr
    __FUNC_END

;函数实现: void arch_switch_to_unprivileged(void)
arch_switch_to_unprivileged __FUNC_DECL
    mrs     r0,CONTROL
    orr     r0,#0x01
    msr     CONTROL,r0;
    __FUNC_END

;ARMCC编译不过,暂时不实现
#if 0

#define SYSCALL_DEF(name,syscall_number) __FUNC_DECL(name); \
            push    {r0-r3};\
            ldr     r0,= ##syscall_number; \
            svc     0; \
            bx      lr; \
            __FUNC_END
SYSCALL_DEF(rtk_arch_exec_task_exit_syscall,0);

#endif

    END
