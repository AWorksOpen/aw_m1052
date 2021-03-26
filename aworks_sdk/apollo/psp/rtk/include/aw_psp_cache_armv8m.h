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
 * \brief cache management
 *
 * \internal
 * \par modification history:
 * - 1.00 2019-08-29, A.J, first implementation.
 * \endinternal
 */

#ifndef __AW_PSP_CACHE_ARMV8M_H
#define __AW_PSP_CACHE_ARMV8M_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

#include "apollo.h"
#include "arch/arm/armv8-m/armv8m_cache.h"

/******************************************************************************/
#define AW_PSP_CACHE_LINE_SIZE      (armv8m_dcache_get_line_size())

/******************************************************************************/
aw_static_inline void aw_cache_invalidate (void *address, size_t nbytes)
{
    armv8m_dcache_invalidate((uint32_t)address, (uint32_t)nbytes);
}

/******************************************************************************/
aw_static_inline void aw_cache_flush (void *address, size_t nbytes)
{
    armv8m_dcache_clean((uint32_t)address, (uint32_t)nbytes);
}

#ifdef __cplusplus
}
#endif /* __cplusplus     */

#endif /* __AW_PSP_CACHE_ARMV8M_H */

/* end of file */
