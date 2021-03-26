
                PRESERVE8
                THUMB

MPU_CTL_ADDR            EQU     0xe000ed94
__BIT_MPU_CTL_ENABLE    EQU     (1 << 0)    ;enable


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
                IMPORT  armv7m_dcache_disable
                IMPORT  armv7m_dcache_clean_all_no_context_save
                IMPORT  armv7m_dcache_invalidate_all_no_context_save

arch_mpu_disable    __FUNC_DECL
    push    {r0-r12,lr}

    bl      armv7m_dcache_disable

    ldr     r1, =MPU_CTL_ADDR;
    mov     r0, #0
    str     r0, [r1]
    dsb

    pop     {r0-r12,lr}
    bx      lr
    __FUNC_END

__armv7m_mpu_enable __FUNC_DECL
    push    {r0-r12,lr}

    bl      armv7m_dcache_clean_all_no_context_save
    bl      armv7m_dcache_invalidate_all_no_context_save

    ldr     r1, =MPU_CTL_ADDR;
    mov     r0, #1
    str     r0, [r1];
    dsb

    pop     {r0-r12, lr}
    bx      lr
    __FUNC_END


    END

