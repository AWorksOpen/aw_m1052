/**
 * \file
 * \brief MMU function for ARMv4 arch.
 *
 * \internal
 * \par History
 * - 1.00 14-09-23  liuweiyun, first implementation.
 * \endinternal
 */

    .section  __cache_asm   /* section name */
    .text                   /* code section */
    .code       32          /* ARM mode */
    .align      2           /* aligned at 2^n */

/*******************************************************************************
    c1 bits mask of p15
*******************************************************************************/

    .set  __P15_R1_I,   (1 << 12) /* I Cache */
    .set  __P15_R1_BR,  (1 << 11) /* branch predictors */
    .set  __P15_R1_C,   (1 <<  2) /* D Cache */
    .set  __P15_R1_A,   (1 <<  1) /* align */
    .set  __P15_R1_M,   (1)       /* MMU */

    .set  __P15_R1_ACTLR_SMP,   (1 << 6)       /* SMP */

    .set  __P15_R1_W,   (1 <<  3) /* write buffer */
    .set  __P15_R1_RR,  (1 << 14) /* Cache */
    .set  __P15_R1_END, (1 <<  7) /* endian */

/******************************************************************************/
#define __FUNC_DECL(name)   .global name; .type name, %function; .func name
#define __LOCAL_FUNC_DECL(name)   .type name, %function; .func name
#define __FUNC_END          .endfunc

/*******************************************************************************
   global functions
*******************************************************************************/
    __FUNC_DECL(arch_mmu_enable)
arch_mmu_enable:
    mrc     p15, 0, r0, c1, c0, 1
    orr     r0, r0, #__P15_R1_ACTLR_SMP
    mcr     p15, 0, r0, c1, c0, 1
    mrc     p15, 0, r0, c1, c0, 0
    orr     r0, r0, #__P15_R1_M
    mcr     p15, 0, r0, c1, c0, 0
    mcr     p15, 0, r0, c7, c10, 4      //DSB
    mcr     p15, 0, r0, c7, c5, 4       //ISB
    bx      lr
    __FUNC_END

    __FUNC_DECL(arch_mmu_disable)
arch_mmu_disable:
    mrc     p15, 0, r0, c1, c0, 0
    bic     r0, r0, #__P15_R1_M
    mcr     p15, 0, r0, c1, c0, 0
    mcr     p15, 0, r0, c7, c10, 4      //DSB
    mcr     p15, 0, r0, c7, c5, 4       //ISB
    bx      lr
    __FUNC_END

    __FUNC_DECL(arch_get_debug_state)
arch_get_debug_state:
    mrc     p14, 0, r0, c0, c1, 0
    and     r0,r0,#3
    bx      lr
    __FUNC_END

    __FUNC_DECL(arch_mmu_tlb_invalidate)
arch_mmu_tlb_invalidate:
    mov     r0, #1
    mcr     p15, 0, r0, c8, c7, 0                 @ TLBIALL - Invalidate entire unified TLB
    dsb
    bx      lr
    __FUNC_END

    __FUNC_DECL(arch_mmu_tt_base_set)
arch_mmu_tt_base_set:
    /* Invalidates all TLBs.Domain access is selected as
     * client by configuring domain access register,
     * in that case access controlled by permission value
     * set by page table entry */
    mov     r1, #0
    mcr     p15, 0, r1, c8, c7, 0
    ldr     r1, =0x55555555
    mcr     p15, 0, r1, c3, c0, 0
    mcr     p15, 0, r0, c2, c0, 0
    bx      lr
    __FUNC_END

    .end

/* end of file */
