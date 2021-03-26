/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https:/*www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief MMU function for ARMv4 arch.
 *
 * \internal
 * \par History
 * - 1.00 14-09-23  orz, first implementation.
 * \endinternal
 */

#include "arch/arm/armv4/armv4_mmu.h"

/******************************************************************************/
static unsigned int *__gp_page_table; /* level1 page table */

/******************************************************************************/
static void __mmu_mtt_set (uint32_t vaddr_start,
                           uint32_t vaddr_end,
                           uint32_t paddr_start,
                           uint32_t attr)
{
    uint32_t *ptt     = (uint32_t *)__gp_page_table + (vaddr_start >> 20);
    int       i, nsec = (vaddr_end >> 20) - (vaddr_start >> 20);

    for (i = 0; i <= nsec; i++) {
        *ptt = attr | (((paddr_start >> 20) + i) << 20);
        ptt++;
    }
}

/******************************************************************************/
void mmu_dcache_clean (uint32_t buffer, uint32_t size)
{
    unsigned int ptr;

    ptr = buffer & ~(CACHE_LINE_SIZE - 1); /*todo: assert the buffer*/

    while (ptr < buffer + size) {
        arch_mmu_dcache_clean_mva(ptr);
        ptr += CACHE_LINE_SIZE;
    }
}

void mmu_dcache_invalidate (uint32_t buffer, uint32_t size)
{
    unsigned int ptr;

    ptr = buffer & ~(CACHE_LINE_SIZE - 1); /*todo: assert the buffer*/

    while (ptr < buffer + size) {
        arch_mmu_dcache_invalidate_mva(ptr);
        ptr += CACHE_LINE_SIZE;
    }
}

/******************************************************************************/
void aw_hw_mmu_init (const struct mem_desc *p_mdesc,
                     uint32_t               size,
                     void                  *p_page_table)
{
    __gp_page_table = p_page_table;

    /* disable I/D cache */
    arch_mmu_dcache_disable();
    arch_mmu_icache_disable();
    arch_mmu_disable();
    arch_mmu_tlb_invalidate();

    /* set page table */
    for (; size > 0; size--, p_mdesc++) {
        __mmu_mtt_set(p_mdesc->vaddr_start,
                      p_mdesc->vaddr_end,
                      p_mdesc->paddr_start,
                      p_mdesc->attr);
    }

    /* set MMU table address */
    arch_mmu_tt_base_set((uint32_t)p_page_table);

    /* enables MMU and I/D cache */
    arch_mmu_enable();
    arch_mmu_icache_enable();
    arch_mmu_dcache_enable();

    arch_mmu_icache_invalidate();
    arch_mmu_dcache_invalidate();
}

/* end of file */
