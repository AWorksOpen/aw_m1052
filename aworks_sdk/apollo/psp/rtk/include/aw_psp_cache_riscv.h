/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief cache management
 *
 * \internal
 * \par modification history:
 * \endinternal
 */

#ifndef __AW_PSP_CACHE_RISCV_H
#define __AW_PSP_CACHE_RISCV_H

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus	*/

#include "apollo.h"

/******************************************************************************/
#define AW_PSP_CACHE_LINE_SIZE  4

/******************************************************************************/
aw_static_inline void aw_cache_invalidate (void *address, size_t nbytes)
{
    return;
}

/******************************************************************************/
aw_static_inline void aw_cache_flush (void *address, size_t nbytes)
{
    return;
}

#ifdef __cplusplus
}
#endif	/* __cplusplus 	*/

#endif	/* __AW_PSP_CACHE_RISCV_H*/

/* end of file */
