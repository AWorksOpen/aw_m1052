
                PRESERVE8
                THUMB

#include "rtk_config.h"

; 可被外部引用的函数申明
    MACRO
$label  __FUNC_DECL
$label  PROC
    EXPORT  $label
    MEND

; 本地使用的函数申明
    MACRO
$label  __LOCAL_FUNC_DECL
$label  PROC
    MEND

; 函数结束
    MACRO
        __FUNC_END
    ENDP
    MEND

                AREA    |.text|, CODE, READONLY

;函数实现 void armv7m_exec_syscalls();
armv7m_exec_syscalls __FUNC_DECL

#if CONFIG_RTK_USERMODE  
    IMPORT  rtk_get_syscalls_function_from_number

    push    {lr}

    ; 此时r0为系统调用编号,由系统调用编号获取真正的内核负责函数
    bl      rtk_get_syscalls_function_from_number
    pop     {lr}
    ; 如果rtk_get_syscalls_function_from_number返回NULL，则返回-AW_ENOSYS
    cmp     r0, #0
    it      ne
    bne     syscall_number_correct
    ;  处理错误
    pop     {r0-r3}
    ldr     r0, =-71
    bx      lr
syscall_number_correct
    ;  将实际内核函数地址压栈
    push    {r0};

    ;  此刻sp开始的堆栈值应该是pc,r0,r1,r2,r3
    ;  弹出这些寄存器之后,sp刚好对应了应用调用syscall时的参数堆栈
    ;  此时内核sys函数和用户syscall函数的参数就完全一致了
    ;  但是如何弹出这些寄存器却非常尴尬，
    ;  因为堆栈顺序必须是r0,r1,r2,r3,pc，才能用pop {r0-r3,pc},一条指令弹出

    ;  如果不能一次弹出，如果先弹出pc，则r0-r3的值还未弹出，
    ;  pc就已经跳转到sys_这样的c函数中了，此时sys_系列c函数看到的参数和用户传入的参数不一致
    ;  如果用ldr先取出r0-r3，这时如果先弹出pc，则sp的值为改变就跳转到c函数，参数不一致
    ;       如果先修改sp的值，再使用ldr来获取pc，这样子如果修改sp的值之后，突然来了中断，
    ;           则因为pc的保存位置已经在sp栈顶之外了，中断的自动压栈会使得pc保存位置的值被修改
    ;           使用ldr获取的pc就不正确
    ;               当然，如果在修改sp之前先禁用中断，则在获取pc的值之前不能启用中断，否则
    ;                   问题依旧
    ;               但是获取pc的值之后，就已经跳转到c函数中了，这时就必须在c函数中启用中断
    ;                   操作更麻烦了

    ;  综上所述，一条指令同时弹出pc和修改sp的值至正确的位置是必须的，
    ;  此时，我们通过修正栈中的寄存器顺序，使得可以使用pop {r0-r3,pc}，完成所有操作

    ;  搬移r0-r3
    ldr         r1, [sp, #4]
    str         r1, [sp, #0]

    ldr         r1, [sp, #8]
    str         r1, [sp, #4]

    ldr         r1, [sp, #12]
    str         r1, [sp, #8]

    ldr         r1, [sp, #16]
    str         r1, [sp, #12]
    ;  重压入pc
    str         r0, [sp, #16]

    ;  恢复并跳转到sys系列c函数
    pop {r0-r3, pc}

#else 
    b .
#endif
    __FUNC_END

    END
