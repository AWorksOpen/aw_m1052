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

#ifndef __ARMV7_MMU_H /* { */
#define __ARMV7_MMU_H

#include <aw_types.h>

/******************************************************************************/
uint32_t armv7_get_cache_line_size(void);
/******************************************************************************/
#define CACHE_LINE_SIZE         ( armv7_get_cache_line_size() )

/** \brief Memory region attributes.*/
typedef enum _mmu_memory_type
{
    kStronglyOrdered,
    kDevice,
    kOuterInner_WB_NOWA,
    kOuterInner_WT,
    kNoncacheable,
} mmu_memory_type_t;

/** \brief Memory region shareability options.*/
typedef enum _mmu_shareability
{
    kShareable = 1,
    kNonshareable = 0
} mmu_shareability_t;

/** \brief Access permissions for a memory region.*/
typedef enum _mmu_access
{
    kNoAccess,
    kROAccess,
    kRWAccess
} mmu_access_t;

/** \brief  Memory region shareability options.*/
typedef enum _mmu_executable
{
    kExecutable = 0,
    kNonExecutable = 1,
} mmu_executable_t;

/******************************************************************************/
struct sub_mem_desc {
    uint32_t            va;
    uint32_t            va_end;
    uint32_t            pa;
    mmu_memory_type_t   memoryType;
    mmu_shareability_t  isShareable;
    mmu_access_t        access;
    mmu_executable_t    executable;
};

struct mem_desc {
    uint32_t            va;
    uint32_t            va_end;
    uint32_t            pa;

    mmu_memory_type_t   memoryType;
    mmu_shareability_t  isShareable;
    mmu_access_t        access;
    mmu_executable_t    executable;

    const struct sub_mem_desc  *p_sub_mem_desc;
    uint32_t                    sub_desc_count;
};



/******************************************************************************/
extern void arch_mmu_enable (void);
extern void arch_mmu_disable (void);

extern void arch_mmu_icache_enable (void);
extern void arch_mmu_icache_disable (void);

extern void arch_mmu_dcache_enable (void);
extern void arch_mmu_dcache_disable (void);

extern void arch_mmu_icache_invalidate (void);

extern void armv7_mmu_dcache_invalidate_mva(uint32_t mva);
extern void arch_mmu_dcache_clean_and_invalidate_mva (uint32_t mva);
extern void armv7_mmu_dcache_clean_mva(uint32_t mva);

extern void armv7_dsb_cp15(void);
extern void armv7_dmb_cp15(void);
extern void armv7_isb_cp15(void);

extern void arch_mmu_tlb_invalidate (void);
extern void arch_mmu_tt_base_set (uint32_t base);

/******************************************************************************/
void mmu_dcache_clean      (uint32_t buffer, uint32_t size);
void mmu_dcache_invalidate (uint32_t buffer, uint32_t size);

/******************************************************************************/
void aw_hw_mmu_init (const struct mem_desc *p_mdesc,
                     uint32_t               size,
                     void                  *p_page_table,
                     uint32_t               page_table_size);

#endif /* } __ARMV4_MMU_H */

/* end of file */
