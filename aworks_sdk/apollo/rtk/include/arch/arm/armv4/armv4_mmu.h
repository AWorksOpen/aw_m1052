/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
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

#ifndef __ARMV4_MMU_H /* { */
#define __ARMV4_MMU_H

#include <aw_types.h>

/******************************************************************************/
#define CACHE_LINE_SIZE    32

/******************************************************************************/
#define DESC_SEC        (0x2 | (1 << 4))
#define CB              (3 << 2)  /**< \brief cache_on, write_back */
#define CNB             (2 << 2)  /**< \brief cache_on, write_through */
#define NCB             (1 << 2)  /**< \brief cache_off,WR_BUF on */
#define NCNB            (0 << 2)  /**< \brief cache_off,WR_BUF off */
#define AP_RW           (3 << 10) /**< \brief supervisor=RW, user=RW */
#define AP_RO           (2 << 10) /**< \brief supervisor=RW, user=RO */
#define DOMAIN_FAULT    (0x0)
#define DOMAIN_CHK      (0x1)
#define DOMAIN_NOTCHK   (0x3)
#define DOMAIN0         (0x0 << 5)
#define DOMAIN1         (0x1 << 5)

#define DOMAIN0_ATTR    (DOMAIN_CHK << 0)
#define DOMAIN1_ATTR    (DOMAIN_FAULT << 2)

/* Read/Write, cache, write back */
#define RW_CB            (AP_RW | DOMAIN0 | CB | DESC_SEC)
/* Read/Write, cache, write through */
#define RW_CNB            (AP_RW | DOMAIN0 | CNB | DESC_SEC)
/* Read/Write without cache and write buffer */
#define RW_NCNB            (AP_RW | DOMAIN0 | NCNB | DESC_SEC)
/* Read/Write without cache and write buffer */
#define RW_FAULT        (AP_RW | DOMAIN1 | NCNB | DESC_SEC)

/******************************************************************************/
struct mem_desc {
    uint32_t vaddr_start;
    uint32_t vaddr_end;
    uint32_t paddr_start;
    uint32_t attr;
};

/******************************************************************************/
extern void arch_mmu_enable (void);
extern void arch_mmu_disable (void);

extern void arch_mmu_icache_enable (void);
extern void arch_mmu_icache_disable (void);

extern void arch_mmu_dcache_enable (void);
extern void arch_mmu_dcache_disable (void);

extern void arch_mmu_icache_invalidate (void);
extern void arch_mmu_dcache_invalidate (void);

extern void arch_mmu_dcache_invalidate_mva           (uint32_t mva);
extern void arch_mmu_dcache_clean_mva                (uint32_t mva);
extern void arch_mmu_dcache_clean_and_invalidate_mva (uint32_t mva);

extern void arch_mmu_tlb_invalidate (void);

extern void arch_mmu_tt_base_set (uint32_t base);

/******************************************************************************/
void mmu_dcache_clean      (uint32_t buffer, uint32_t size);
void mmu_dcache_invalidate (uint32_t buffer, uint32_t size);

/******************************************************************************/
void aw_hw_mmu_init (const struct mem_desc *p_mdesc,
                     uint32_t               size,
                     void                  *p_page_table);

#endif /* } __ARMV4_MMU_H */

/* end of file */
