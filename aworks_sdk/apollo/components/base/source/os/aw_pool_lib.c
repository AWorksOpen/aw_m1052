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
 * \brief pool manager library implementation
 *
 * \internal
 * \par modification history:
 * - 1.00 13-09-11  zen, first implementation
 * \endinternal
 */

#include "apollo.h"
#include "aw_pool.h"
#include "aw_spinlock.h"

#include "aw_assert.h"
#include "aw_vdebug.h"

/** \brief tructure representing a free block in the Native Memory Pool */
typedef struct __free_item {
    struct __free_item *p_next;
} __free_item_t;

/******************************************************************************/
aw_pool_id_t aw_pool_init(aw_pool_t *p_pool,
                          void      *p_pool_mem,
                          size_t     pool_size,
                          size_t     item_size)
{
    __free_item_t  *fb;
    size_t          corr;
    size_t          nblocks;

    /* The memory block must be valid
     * and the poolSize must fit at least one free block
     * and the blockSize must not be too close to the top of the dynamic range
     */
    if (!((p_pool_mem != NULL) &&
          (pool_size >= (uint32_t)sizeof(__free_item_t)) &&
          (item_size + sizeof(__free_item_t) > item_size))) {

        return NULL;
    }

    corr = (size_t)p_pool_mem & (sizeof(__free_item_t) - 1);
    if (corr != 0) {                        /* alignment needed? */
        corr = sizeof(__free_item_t) - corr;/*amount to align poolSto*/
        pool_size -= corr;                  /* reduce the available pool size */
    }

    p_pool->p_free = (void *)((uint8_t *)p_pool_mem + corr);

    /* round up the blockSize to fit an integer # free blocks, no division */
    /* start with just one */
    p_pool->item_size= sizeof(__free_item_t); 
    /* # free blocks that fit in one memory block */      
    nblocks = 1;           
    while (p_pool->item_size < item_size) {
        p_pool->item_size += sizeof(__free_item_t);
        ++nblocks;
    }
    item_size= p_pool->item_size;  /* use the rounded-up value from now on */

    /* the pool buffer must fit at least one rounded-up block */
    if (pool_size < item_size) {
        return NULL;
    }

    /* chain all blocks together in a free-list... */
    pool_size -= item_size;         /* don't count the last block */
    p_pool->item_count= 1;          /* the last block already in the pool */
    fb = (__free_item_t *)p_pool->p_free;/*start at the head of the free list */
    while (pool_size>= item_size) {
        fb->p_next = &fb[nblocks];  /* point the next link to the next block */
        fb = fb->p_next;                    /* advance to the next block */
        pool_size -= (uint32_t)item_size;   /* reduce the available pool size */
        ++p_pool->item_count;       /* increment the number of blocks so far */
    }

    fb->p_next  = NULL;                 /* the last link points to NULL */
    p_pool->nfree = p_pool->item_count; /* all blocks are free */
    p_pool->nmin  = p_pool->item_count; /* the minimum number of free blocks */
    p_pool->p_start = p_pool_mem;       /* the original start this pool buffer */
    p_pool->p_end   = fb;               /* the last block in this pool */

    return p_pool;
}

/******************************************************************************/
void *aw_pool_item_get (aw_pool_id_t pool)
{
    aw_pool_t *p_pool = (aw_pool_t *)pool;
    __free_item_t *fb;
    aw_spinlock_isr_t lock = AW_SPINLOCK_ISR_UNLOCK;

    aw_spinlock_isr_take(&lock);

    fb = (__free_item_t *)p_pool->p_free; /* get a free block or NULL */
    if (fb != NULL) {                   /* free block available? */
        p_pool->p_free = fb->p_next;    /* adjust list head to the next free block */
        --p_pool->nfree;                /* one less free block */
        if (p_pool->nmin > p_pool->nfree) {
            p_pool->nmin = p_pool->nfree;            /* remember the minimum so far */
        }
    }

    aw_spinlock_isr_give(&lock);

    return fb;            /* return the block or NULL pointer to the caller */
}

/******************************************************************************/
aw_err_t aw_pool_item_return (aw_pool_id_t pool, void *p_item)
{
    aw_spinlock_isr_t lock = AW_SPINLOCK_ISR_UNLOCK;
    aw_pool_t *p_pool = (aw_pool_t *)pool;

    aw_spinlock_isr_take(&lock);
    /*  must be in range */
    if (!((p_pool->p_start <= p_item) && (p_item <= p_pool->p_end)
              && (p_pool->nfree <= p_pool->item_count))) { 

        aw_spinlock_isr_give(&lock);
        return -AW_EFAULT;
    }

    /* link into free list */
    ((__free_item_t *)p_item)->p_next = (__free_item_t *)p_pool->p_free;
    p_pool->p_free = p_item;       /* set as new head of the free list */
    ++p_pool->nfree;               /* one more free block in this pool */

    aw_spinlock_isr_give(&lock);

    return AW_OK;
}

/******************************************************************************/
size_t aw_pool_margin_get (aw_pool_id_t pool)
{
    aw_pool_t *p_pool = (aw_pool_t *)pool;
    aw_spinlock_isr_t lock = AW_SPINLOCK_ISR_UNLOCK;
    size_t margin;

    aw_spinlock_isr_take(&lock);
    margin = p_pool->nmin;
    aw_spinlock_isr_give(&lock);

    return margin;
}


/* end of file */

