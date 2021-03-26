    .section  .text     /* section name */
    .align      2       /* aligned at 2^n */
    .syntax  unified



#define __FUNC_DECL(name)   .global name; .type name, %function; .func name
#define __FUNC_END          .endfunc


/*******************************************************************************
   global functions
*******************************************************************************/
    __FUNC_DECL(pm_cpu_idle)
pm_cpu_idle:
    dsb
    wfi
    bx      lr
    __FUNC_END

    .word 0
