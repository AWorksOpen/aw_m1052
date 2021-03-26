
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
#ifndef __ARMV8M_NO_CACHE
                IMPORT  armv8m_dcache_disable
                IMPORT  armv8m_dcache_clean_all_no_context_save
                IMPORT  armv8m_dcache_invalidate_all_no_context_save
#endif

__armv8m_mpu_disable __FUNC_DECL
    push    {r0-r12,lr}

#ifndef __ARMV8M_NO_CACHE
    bl      armv8m_dcache_disable
#endif

    ldr     r1, =MPU_CTL_ADDR;
    mov     r0, #0
    str     r0, [r1]
    dsb

    pop     {r0-r12,lr}
    bx      lr
    __FUNC_END

__armv8m_mpu_enable __FUNC_DECL
    push    {r0-r12,lr}

#ifndef __ARMV8M_NO_CACHE
    bl      armv8m_dcache_clean_all_no_context_save
    bl      armv8m_dcache_invalidate_all_no_context_save
#endif

    ldr     r1, =MPU_CTL_ADDR;
    mov     r0, #1
    str     r0, [r1];
    dsb

    pop     {r0-r12, lr}
    bx      lr
    __FUNC_END


    END
