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
 * \brief ARMv7-M cache
 *
 * \internal
 * \par modification history:
 * - 1.00 17-04-08  zen, first implementation
 * \endinternal
 */

#include "arch/arm/armv7-m/armv7m_mpu.h"
#include "string.h"
#include "aw_bitops.h"


/* 定义系统控制空间中关于cache 操作的寄存器*/
typedef struct {
    volatile uint32_t ICIALLU;  /**< \brief address 0xE000EF50,I-cache invalidate all to PoU*/
    volatile uint32_t Reserved; /**< \brief address 0xE000EF54 */
    volatile uint32_t ICIMVAU;  /**< \brief address 0xE000EF58,I-cache invalidate by MVA to PoU*/
    volatile uint32_t DCIMVAC;  /**< \brief address 0xE000EF5c,D-cache invalidate by MVA to PoC*/
    volatile uint32_t DCISW;    /**< \brief address 0xE000EF60,D-cache invalidate by set-way*/
    volatile uint32_t DCCMVAU;  /**< \brief address 0xE000EF64,D-cache clean by MVA to PoU*/
    volatile uint32_t DCCMVAC;  /**< \brief address 0xE000EF68,D-cache clean by MVA to PoC*/
    volatile uint32_t DCCSW;    /**< \brief address 0xE000EF6c,D-cache clean by set-way*/
    volatile uint32_t DCCIMVAC; /**< \brief address 0xE000EF70,D-cache clean and invalidate by MVA to PoC*/
    volatile uint32_t DCCISW;   /**< \brief address 0xE000EF74,D-cache clean and invalidate by set-way*/
    volatile uint32_t BPIALL;   /**< \brief address 0xE000EF78,Branch predictor invalidate all*/
}scs_cache_regs_t;

typedef struct {
    volatile uint32_t CLIDR;    /**< \brief address 0xE000ED78,Cache Level ID*/
    volatile uint32_t CTR;      /**< \brief address 0xE000ED7C,Cache Type Register*/
    volatile uint32_t CCSIDR;   /**< \brief address 0xE000ED80,Cache Size ID*/
    volatile uint32_t CSSELR;   /**< \brief address 0xE000ED84,Cache Size Selection*/
}scs_cache_id_regs_t;



#define SCS_CACHE       ((scs_cache_regs_t *)0xE000EF50)
#define SCS_CACHE_ID    ((scs_cache_id_regs_t *)0xE000ED78)

void armv7m_dcache_clean_by_mva(uint32_t mva);
void armv7m_dcache_invalidate_by_mva(uint32_t mva);
int armv7m_cache_get_cache_level_count(void);
int armv7m_dcache_get_line_size(void);

void armv7m_dcache_clean (uint32_t buffer, uint32_t size)
{

    unsigned int    ptr;
    uint32_t        cache_line_size;

    if (0 == armv7m_cache_get_cache_level_count()) {
        return;
    }

    cache_line_size = armv7m_dcache_get_line_size();

    ptr = buffer & ~(cache_line_size - 1); /* todo: assert the buffer */

    while (ptr < buffer + size) {
        armv7m_dcache_clean_by_mva(ptr);
        ptr += cache_line_size;
    }

}

void armv7m_dcache_invalidate (uint32_t buffer, uint32_t size)
{

    unsigned int    ptr;
    uint32_t        cache_line_size;

    if (0 == armv7m_cache_get_cache_level_count()) {
        return;
    }

    cache_line_size = armv7m_dcache_get_line_size();

    ptr = buffer & ~(cache_line_size - 1); /* todo: assert the buffer */

    while (ptr < buffer + size) {
        armv7m_dcache_invalidate_by_mva(ptr);
        ptr += cache_line_size;
    }

}
