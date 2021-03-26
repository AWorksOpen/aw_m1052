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
 * \brief   LoRaNet Pool Module
 *
 * \internal
 * \par modification history:
 * - 2017-05-23 ebi, first implementation.
 * \endinternal
 */

#ifndef __AM_LRNET_POOL_H
#define __AM_LRNET_POOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "am_lrnet_internal.h"

typedef struct {
    uint8_t  *p_mem;
    uint32_t  size;

    uint32_t  item_size;
    uint32_t  item_idx_max;

    uint32_t  pool_using_mask;
} am_lrnet_pool_t;

int am_lrnet_pool_init (am_lrnet_pool_t *p_pool,
                        uint8_t         *p_buf,
                        int             item_size,
                        int             item_num);

void* am_lrnet_pool_get (am_lrnet_pool_t *p_pool);

void am_lrnet_pool_free (am_lrnet_pool_t *p_pool, void *p_mem);

#ifdef __cplusplus
}
#endif

#endif /* __AM_LRNET_POOL_H */

/* end of file */
