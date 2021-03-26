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
 * \brief cache function for armv7m arch.
 *
 * \internal
 * \par History
 * - 1.00 17-10-11  sni, first implementation.
 * \endinternal
 */

#ifndef __ARMV7M_CACHE_H /* { */
#define __ARMV7M_CACHE_H

extern void armv7m_icache_enable (void);
extern void armv7m_icache_disable (void);

extern void armv7m_dcache_enable (void);
extern void armv7m_dcache_disable (void);

void armv7m_dcache_clean (uint32_t buffer, uint32_t size);
void armv7m_dcache_invalidate (uint32_t buffer, uint32_t size);

uint32_t armv7m_dcache_get_line_size(void);

#endif /* } __ARMV4_MMU_H */

/* end of file */
