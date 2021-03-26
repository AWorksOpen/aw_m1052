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
 * \brief cache function for armv8m arch.
 *
 * \internal
 * \par History
 * - 1.10 2019-08-27, A.J, armv8-m cache implementation.
 * - 1.00 17-10-11  sni, first implementation.
 * \endinternal
 */

#ifndef __ARMV8M_CACHE_H /* { */
#define __ARMV8M_CACHE_H

extern void armv8m_icache_enable (void);
extern void armv8m_icache_disable (void);

extern void armv8m_dcache_enable (void);
extern void armv8m_dcache_disable (void);

void armv8m_dcache_clean (uint32_t buffer, uint32_t size);
void armv8m_dcache_invalidate (uint32_t buffer, uint32_t size);

uint32_t armv8m_dcache_get_line_size(void);

#endif /* } __ARMV8M_CACHE_H */

/* end of file */
