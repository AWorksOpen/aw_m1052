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
 * \brief Fast fixed size memory pool management.
 *
 * \internal
 * \par modification history:
 * - 170923, vih, first implementation.
 * \endinternal
 */

#ifndef _AW_LFFS_POOL_H_
#define _AW_LFFS_POOL_H_


#ifdef __cplusplus
extern "C"{
#endif


#include "aw_lffs_os.h"
#include "aw_lffs_core.h"


/**
 * \struct aw_lffs_pool_tEntrySt
 * \brief Helper type for free buffer entries.
 */
typedef struct aw_lffs_pool_entry {
    struct aw_lffs_pool_entry *next;
} aw_lffs_pool_entry_t;

/**
 * \struct aw_lffs_pool_tSt
 * \brief Memory pool.
 */
struct aw_lffs_pool {
    aw_lffs_pool_entry_t *free_list; /**< \brief linked list of free buffers */
    uint8_t              *mem;       /**< \brief memory pool */
    uint32_t              buf_size;  /**< \brief size of a buffer */
    
    uint32_t             num_bufs;  /**< \brief number of buffers in the pool */

    aw_lffs_sem_t         sem;       /**< \brief buffer lock */
};

aw_err_t aw_lffs_pool_init (struct aw_lffs_pool  *pool,
                            void                 *mem,
                            uint32_t              buf_size,
                            uint32_t              num_bufs,
                            aw_bool_t             lock);
aw_err_t aw_lffs_pool_uninit (aw_lffs_pool_t *pool);

aw_bool_t aw_lffs_pool_verify (aw_lffs_pool_t *pool, void *p);

void *aw_lffs_pool_buf_get (aw_lffs_pool_t *pool);
void *aw_lffs_pool_buf_get_locked (aw_lffs_pool_t *pool);

int aw_lffs_pool_buf_put (aw_lffs_pool_t *pool, void *p);
int aw_lffs_pool_buf_put_locked (aw_lffs_pool_t *pool, void *p);

void *aw_lffs_pool_buf_get_by_index (aw_lffs_pool_t *pool, uint32_t index);
uint32_t aw_lffs_pool_buf_index_get (aw_lffs_pool_t *pool, void *p);
aw_bool_t aw_lffs_pool_buf_is_free (aw_lffs_pool_t *pool, void *p);

void * aw_lffs_pool_next_allocated_buf_find (aw_lffs_pool_t *pool, void *from);

int aw_lffs_pool_get_free_count (aw_lffs_pool_t *pool);
int aw_lffs_pool_all_put (aw_lffs_pool_t *pool);

#ifdef __cplusplus
}
#endif

#endif /* _LFFS_POOL_H_ */

/* end of file */
