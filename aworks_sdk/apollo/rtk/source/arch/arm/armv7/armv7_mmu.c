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
 * \brief MMU function for ARMv7 arch.
 *
 * \internal
 * \par History
 * - 1.00 16-03-01  jac, first implementation.
 * \endinternal
 */

#include "arch/arm/armv7/armv7_mmu.h"
#include <string.h>


/******************************************************************************/
static uint32_t     *__gp_page_table;           /* 页表基地址 */
static uint32_t     __g_page_table_size;        /* 页表总共包含的字节数 */
static uint32_t     __g_used_page_table_size;   /* 页表已经使用的字节数 */

/*******************************************************************************
 Definitions
*******************************************************************************/

/** \brief Size in bytes of the first-level page table. */
#define MMU_L1_PAGE_TABLE_SIZE  (16 * 1024)
#define PAGE_SIZE_4K            (4*1024)

/** \brief First-level 1MB section descriptor entry. */
typedef union mmu_l1_section {
    uint32_t u;
    struct {
        uint32_t id:2;              /**< \brief ID */
        uint32_t b:1;               /**< \brief Bufferable */
        uint32_t c:1;               /**< \brief Cacheable */
        uint32_t xn:1;              /**< \brief Execute-not */
        uint32_t domain:4;          /**< \brief Domain */
        uint32_t _impl_defined:1;   /**< \brief Implementation defined, should be zero. */
        uint32_t ap1_0:2;      /**< \brief Access permissions AP[1:0] */
        uint32_t tex:3;        /**< \brief TEX remap */
        uint32_t ap2:1;        /**< \brief Access permissions AP[2] */
        uint32_t s:1;          /**< \brief Shareable */
        uint32_t ng:1;         /**< \brief Not-global */
        uint32_t _zero:1;      /**< \brief Should be zero. */
        uint32_t ns:1;         /**< \brief Non-secure */
        uint32_t address:12;   /**< \brief Physical base address */
    };
} mmu_l1_section_t;

/* 定义MMU地址范围包含二级页表时的一级描述符 */
typedef union mmu_l1_2nd_des {
    uint32_t u;
    struct {
        uint32_t id:2;          /**< \brief ID */
        uint32_t sbz:3;         /**< \brief 保留 */
        uint32_t domain:4;      /**< \brief Domain */
        uint32_t ap2:1;         /**< \brief Access permissions AP[2] */
        uint32_t address:22;    /**< \brief Physical base address */
    };
} mmu_l1_2nd_des_t;

typedef union mmu_l2_page_entry_4K {
    uint32_t u;
    struct {
        uint32_t xn:1;      /**< \brief Execute-not */
        uint32_t id2:1;     /**< \brief ID */
        uint32_t b:1;       /**< \brief Bufferable */
        uint32_t c:1;       /**< \brief Cacheable */
        uint32_t ap1_0:2;   /**< \brief Access permissions AP[1:0] */
        uint32_t tex:3;     /**< \brief TEX remap */
        uint32_t ap2:1;     /**< \brief Access permissions AP[2] */
        uint32_t s:1;       /**< \brief Shareable */
        uint32_t ng:1;      /**< \brief Not-global */
        uint32_t address:20;/**< \brief Physical base address */
    };
} mmu_l2_page_entry_4K_t;

enum {
    /** \brief ID value for a 1MB section first-level entry. */
    kMMU_L1_Section_ID = 2,  
    kMMU_L1_2ND_DES_ID = 0x1,

    /** \brief Bit offset of the physical base address field. */
    kMMU_L1_Section_Address_Shift = 20,  
    KMMU_l1_2nd_dese_Address_Shift = 10,
    kMMU_L2_Section_ID2 = 0x1,
    KMMU_L2_4K_Section_Shift = 12,
};

/*******************************************************************************
  Externs
*******************************************************************************/


/*******************************************************************************
  Code
*******************************************************************************/




static void mmu_init()
{

}

static void mmu_map_l1_range(uint32_t           pa,
                             uint32_t           va,
                             uint32_t           length,
                             mmu_memory_type_t  memoryType,
                             mmu_shareability_t isShareable,
                             mmu_access_t       access,
                             mmu_executable_t   executable)
{
    register mmu_l1_section_t entry;
    entry.u = 0;

    /* Set constant attributes. */
    entry.id = kMMU_L1_Section_ID;
    entry.xn = 0; /* Allow execution */
    entry.domain = 0; /* Domain 0 */
    entry.ng = 0; /* Global */
    entry.ns = 0; /* Secure */

    /* Set attributes based on the selected memory type. */
    switch (memoryType)
    {
        case kStronglyOrdered:
            entry.c = 0;
            entry.b = 0;
            entry.tex = 0;
            entry.s = 1; /* Ignored */
            break;
        case kDevice:
            if (isShareable)
            {
                entry.c = 0;
                entry.b = 1;
                entry.tex = 0;
                entry.s = 1; /* Ignored */
            }
            else
            {
                entry.c = 0;
                entry.b = 0;
                entry.tex = 2;
                entry.s = 0; /* Ignored */
            }
            break;
        case kOuterInner_WB_NOWA:
            entry.c = 1;
            entry.b = 1;
            entry.tex = 0;
            entry.s = isShareable;
            break;
        case kOuterInner_WT:
            entry.c = 1;
            entry.b = 0;
            entry.tex = 0;
            entry.s = isShareable;
            break;
        case kNoncacheable:
            entry.c = 0;
            entry.b = 0;
            entry.tex = 1;
            entry.s = isShareable;
            break;
    }

    /* Set attributes from specified access mode. */
    switch (access)
    {
        case kNoAccess:
            entry.ap2 = 0;
            entry.ap1_0 = 0;
            break;
        case kROAccess:
            entry.ap2 = 1;
            entry.ap1_0 = 3;
            break;
        case kRWAccess:
            entry.ap2 = 0;
            entry.ap1_0 = 3;
            break;
    }
    entry.xn = executable;

    /* Get the L1 page table base address. */
    uint32_t * table = (uint32_t *)__gp_page_table;

    /* Convert addresses to 12-bit bases. */
    uint32_t vbase = va >> kMMU_L1_Section_Address_Shift;
    uint32_t pbase = pa >> kMMU_L1_Section_Address_Shift;
    uint32_t entries = length >> kMMU_L1_Section_Address_Shift;

    /* Fill in L1 page table entries. */
    for (; entries > 0; ++pbase, ++vbase, --entries)
    {
        entry.address = pbase;
        table[vbase] = entry.u;
    }

    /* Invalidate TLB */
    arch_mmu_tlb_invalidate();
}

static uint32_t mmu_map_l1_range_with_l2(uint32_t va,uint32_t length)
{
    register mmu_l1_2nd_des_t   entry;
    uint32_t                    base = 0;

    entry.u = 0;

    /*确保长度为1M */
    if(1024*1024 != length)
    {
        while(1);
    }

    /* Set constant attributes. */
    entry.id = kMMU_L1_2ND_DES_ID;
    entry.domain = 0; /* Domain 0 */

    /* Get the L1 page table base address. */
    uint32_t * table = (uint32_t *)__gp_page_table;

    /* Convert addresses to 12-bit bases.*/
    uint32_t vbase = va >> kMMU_L1_Section_Address_Shift;

    /* Fill in L1 page table entries. */
    if(__g_used_page_table_size+1024 <= __g_page_table_size) {
        base = ((uint32_t)__gp_page_table) + __g_used_page_table_size;
        __g_used_page_table_size += 1024;
    }
    else {
        while(1);
    }
    base = base >> KMMU_l1_2nd_dese_Address_Shift;
    entry.address = base;
    table[vbase] = entry.u;

    return base;
}

#define FATAL_ERROR() while(1)
static void mmu_map_l2_range(uint32_t                   vabase,
                             const struct sub_mem_desc *pmem_des)
{
    mmu_l1_2nd_des_t           *p_l1_2nd_des;
    uint32_t                    l2base = 0;
    uint32_t                    *table ;
    uint32_t                    *p_l2_base;
    uint32_t                    offset;
    mmu_l2_page_entry_4K_t      entry;
    int                         length = pmem_des->va_end - pmem_des->va +1;


    if( PAGE_SIZE_4K != length ) {
        FATAL_ERROR();
    }

    /*描述符指向的虚拟地址在L1页表的范围之内 */
    if(pmem_des->va < vabase ||
            (pmem_des->va -vabase) >= (1024*1024) ||
            (pmem_des->va - vabase +PAGE_SIZE_4K) > (1024*1024) )
    {
        while(1);
    }
    offset = pmem_des->va - vabase;
    offset /= PAGE_SIZE_4K;

    /*从L1页表中读取L2页表的基地址 */
    vabase = vabase >> kMMU_L1_Section_Address_Shift;
    table  = (uint32_t *)__gp_page_table;
    p_l1_2nd_des =(mmu_l1_2nd_des_t *) &table[vabase];
    l2base = p_l1_2nd_des->address;
    l2base = l2base << KMMU_l1_2nd_dese_Address_Shift;
    p_l2_base = (uint32_t *)l2base;



    /* Set constant attributes. */
    entry.u = 0;
    entry.id2 = kMMU_L2_Section_ID2;
    entry.xn = 0; /* Allow execution */
    entry.ng = 0; /* Global */

    /* Set attributes based on the selected memory type. */
    switch (pmem_des->memoryType)
    {
        case kStronglyOrdered:
            entry.c = 0;
            entry.b = 0;
            entry.tex = 0;
            entry.s = 1; /* Ignored */
            break;
        case kDevice:
            if (pmem_des->isShareable)
            {
                entry.c = 0;
                entry.b = 1;
                entry.tex = 0;
                entry.s = 1; /* Ignored */
            }
            else
            {
                entry.c = 0;
                entry.b = 0;
                entry.tex = 2;
                entry.s = 0; /* Ignored */
            }
            break;
        case kOuterInner_WB_NOWA:
            entry.c = 1;
            entry.b = 1;
            entry.tex = 0;
            entry.s = pmem_des->isShareable;
            break;
        case kOuterInner_WT:
            entry.c = 1;
            entry.b = 0;
            entry.tex = 0;
            entry.s = pmem_des->isShareable;
            break;
        case kNoncacheable:
            entry.c = 0;
            entry.b = 0;
            entry.tex = 1;
            entry.s = pmem_des->isShareable;
            break;
    }

    /* Set attributes from specified access mode. */
    switch (pmem_des->access)
    {
        case kNoAccess:
            entry.ap2 = 0;
            entry.ap1_0 = 0;
            break;
        case kROAccess:
            entry.ap2 = 1;
            entry.ap1_0 = 1;
            break;
        case kRWAccess:
            entry.ap2 = 0;
            entry.ap1_0 = 3;
            break;
    }
    entry.xn = pmem_des->executable;
    entry.address = pmem_des->pa>>KMMU_L2_4K_Section_Shift;
    p_l2_base[offset] = entry.u;

}



/******************************************************************************/
void mmu_dcache_clean (uint32_t buffer, uint32_t size)
{
    unsigned int ptr;

    ptr = buffer & ~(CACHE_LINE_SIZE - 1); /*todo: assert the buffer */

    while (ptr < buffer + size) {
        armv7_mmu_dcache_clean_mva(ptr);
        ptr += CACHE_LINE_SIZE;
    }
    armv7_dsb_cp15();
}

void mmu_dcache_invalidate (uint32_t buffer, uint32_t size)
{
    unsigned int ptr;

    ptr = buffer & ~(CACHE_LINE_SIZE - 1); /*todo: assert the buffer */

    while (ptr < buffer + size) {
        armv7_mmu_dcache_invalidate_mva(ptr);
        ptr += CACHE_LINE_SIZE;
    }
    armv7_dsb_cp15();
}

static int __find_pow_of_2_not_below(unsigned int num)
{
    int             i;
    uint64_t        val;

    for (i = 0; i< 64;i++) {
        val = ((uint64_t)1) << i;
        if (val >= num) {
            break;
        }
    }

    return i;
}


/******************************************************************************/
void aw_hw_mmu_init (const struct mem_desc *p_mdesc,
                     uint32_t               size,
                     void                  *p_page_table,
                     uint32_t               page_table_size)
{
    uint32_t            i,j;

    mmu_init();
    __gp_page_table = p_page_table;
    __g_page_table_size = page_table_size;
    __g_used_page_table_size = MMU_L1_PAGE_TABLE_SIZE;

    arch_mmu_dcache_disable();
    arch_mmu_icache_disable();
    arch_mmu_disable();
    arch_mmu_tlb_invalidate();

    arch_mmu_tt_base_set((uint32_t)__gp_page_table);
    /* Clear the L1 table. */
    memset(__gp_page_table,0, page_table_size);

    for(i = 0; i<size;i++)
    {
        if(NULL == p_mdesc->p_sub_mem_desc) {
            mmu_map_l1_range(p_mdesc->pa,
                         p_mdesc->va,
                         p_mdesc->va_end - p_mdesc->va+1,
                         p_mdesc->memoryType,
                         p_mdesc->isShareable,
                         p_mdesc->access,
                         p_mdesc->executable);
        }
        else {
            mmu_map_l1_range_with_l2(p_mdesc->va,p_mdesc->va_end - p_mdesc->va+1);
            for (j = 0;j<p_mdesc->sub_desc_count;j++) {
                mmu_map_l2_range(p_mdesc->va,&p_mdesc->p_sub_mem_desc[j]);
            }

        }

        p_mdesc ++;
    }

    arch_mmu_enable();
    arch_mmu_icache_invalidate();
    arch_mmu_icache_enable();
    arch_mmu_dcache_enable();

}

/* end of file */
