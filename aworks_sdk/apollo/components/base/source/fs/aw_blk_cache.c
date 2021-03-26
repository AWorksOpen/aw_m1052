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
 * \brief block device cache.
 *
 * \internal
 * \par modification history:
 * - 170420 deo, first implementation.
 * \endinternal
 */

/*******************************************************************************
    includes
*******************************************************************************/
#include "apollo.h"
#include "aw_list.h"
#include "aw_task.h"
#include "aw_delay.h"
#include "fs/aw_blk_dev.h"
#include "string.h"



#define __LOCK_TIMEOUT          10000
#define __MAX_HASH              2048
#define __SYNC_TASK_STACK_SIZE  (5 * 1024)
#define __SYNC_PERIOD           (5000)


AW_TASK_DECL_STATIC(        __g_sync_task, __SYNC_TASK_STACK_SIZE);
static aw_bool_t            __g_task_init = AW_FALSE;
static struct aw_list_head  __g_sync_list;


/* cache node */
struct __blk_cache_node {
    struct aw_list_head      hash;
    struct aw_list_head      lru;
    struct __blk_cache_node *dirty;
    uint32_t                 blk_no;
    uint8_t                 *p_buf;
};

#define __NODE_IS_VALID(p_node)             (!aw_list_empty(&(p_node)->hash))
#define __NODE_SET_VALID(p_cache, p_node)   __valid_node_add(p_cache, p_node)
#define __NODE_CLR_VALID(p_node)            aw_list_del_init(&(p_node)->hash)

#define __NODE_IS_DIRTY(p_node)             ((p_node)->dirty != p_node)
#define __NODE_SET_DIRTY(p_node)            (p_node)->dirty = NULL
#define __NODE_CLR_DIRTY(p_node)            (p_node)->dirty = p_node



static int __cache_flush (struct aw_blk_cache *p_cache,
                          uint32_t             nblks);





/******************************************************************************/
static void __hw_xfer_cb (struct aw_blk_arp *p_arp)
{
    (void)aw_semc_give((aw_semc_id_t)p_arp->priv);
}




/******************************************************************************/
static int __hw_xfer (struct aw_blk_cache  *p_cache,
                      uint32_t              blk_no,
                      uint32_t              nblks,
                      aw_bool_t                is_read)
{
    struct aw_blk_arp  arp;
    int                ret;
    AW_SEMC_DECL(      bio_sem);
    aw_semc_id_t       semc_id = NULL;
    uint8_t            retry = 3;

    if (nblks > p_cache->b_blks) {
        return -AW_EINVAL;
    }

    memset(&bio_sem,0,sizeof(bio_sem));
    semc_id = AW_SEMC_INIT(bio_sem, 0, AW_SEM_Q_PRIORITY);
    if (semc_id == NULL) {
        return -AW_ENOMEM;
    }

__retry:
    AW_SEMC_TAKE(bio_sem, AW_SEM_NO_WAIT);
    arp.p_dev    = p_cache->p_bd;
    arp.blk_no   = blk_no;
    arp.nbytes   = p_cache->bsize * nblks;
    arp.p_data   = p_cache->p_bbuf;
    arp.resid    = 0;
    arp.error    = AW_OK;
    arp.flags    = is_read ? AW_BD_ARQ_READ : AW_BD_ARQ_WRITE;
    arp.priv     = semc_id;
    arp.pfn_done = __hw_xfer_cb;
    arp.p_next   = NULL;

    ret = p_cache->p_bd->pfn_areq(p_cache->p_bd, &arp);
    AW_SEMC_TAKE(bio_sem, AW_SEM_WAIT_FOREVER);

    if ((ret == AW_OK) && (arp.error != AW_OK)) {
        ret = arp.error;
    }

    if (ret != AW_OK) {
        if ((ret != -AW_ENODEV) && (retry--)) {
            goto __retry;
        }
    }

    AW_SEMC_TERMINATE(bio_sem);

    return ret;
}




/******************************************************************************/
static void __valid_node_add (struct aw_blk_cache     *p_cache,
                              struct __blk_cache_node *p_node)
{
    struct aw_list_head *p_list;

    /* find in HASH table */
    p_list = &p_cache->hash[p_node->blk_no % p_cache->n_hashs];

    aw_list_add(&p_node->hash, p_list);
}




/******************************************************************************/
static void __dirty_node_add (struct __blk_cache_node **p_dirty,
                              struct __blk_cache_node  *p_node)
{
    struct __blk_cache_node *p_tmp, *p_pre;

    if (*p_dirty == NULL) {
        *p_dirty = p_node;
        return;
    }

    p_tmp = *p_dirty;
    p_pre = p_tmp;

    while (p_tmp) {
        if (p_node->blk_no < p_tmp->blk_no) {
            if (p_pre == p_tmp) {
                p_node->dirty = p_tmp;
                *p_dirty = p_node;
            } else {
                p_pre->dirty  = p_node;
                p_node->dirty = p_tmp;
            }
            return;
        }

        p_pre = p_tmp;
        p_tmp = p_tmp->dirty;
    }

    p_pre->dirty  = p_node;
    p_node->dirty = NULL;
}




/******************************************************************************/
static struct __blk_cache_node *__node_alloc (struct aw_blk_cache *p_cache)
{
    struct __blk_cache_node *p_tmp = NULL;
    int                      ret;

    p_tmp = aw_list_entry(p_cache->lru.prev,
                          struct __blk_cache_node,
                          lru);
    if (__NODE_IS_DIRTY(p_tmp)) {
        return NULL;
    }

    return p_tmp;
}




/******************************************************************************/
static struct __blk_cache_node *__node_find (struct aw_blk_cache *p_cache,
                                             uint32_t             blk_no)
{
    struct aw_list_head     *p_list;
    struct __blk_cache_node *p_node;

    /* find in HASH table */
    p_list = &p_cache->hash[blk_no % p_cache->n_hashs];
    aw_list_for_each_entry (p_node,
                            p_list,
                            struct __blk_cache_node,
                            hash) {
        if (p_node->blk_no == blk_no) {
            /* get it */
            return p_node;
        }
    }

    return NULL;
}




/******************************************************************************/
static int __nodes_load (struct aw_blk_cache      *p_cache,
                         uint32_t                  blk_no,
                         struct __blk_cache_node **p_node)
{
    struct __blk_cache_node *p_tmp;
    uint32_t                 nblks, blk;
    int                      ret;
    uint8_t                 *ptr;

    /* get the nodes counts */
    nblks = min((p_cache->nblks - blk_no), p_cache->b_blks);

    /* nblks nodes must been clear at least */
    ret = __cache_flush(p_cache, nblks);
    if (ret != AW_OK) {
        return ret;
    }

    /* read data */
    ret = __hw_xfer(p_cache, blk_no, nblks, AW_TRUE);
    if (ret != AW_OK) {
        return ret;
    }

    ptr = (uint8_t*)p_cache->p_bbuf;
    blk = blk_no;

    ret = -AW_ENOSPC;
    while (nblks) {
        /* check if node already existed */
        p_tmp = __node_find(p_cache, blk);
        if (p_tmp == NULL) {
            /* allocate a new */
            p_tmp = __node_alloc(p_cache);
            if (p_tmp == NULL) {

                break;
            }

            memcpy(p_tmp->p_buf, ptr, p_cache->bsize);
            /* refresh LRU */
            aw_list_del(&p_tmp->lru);
            aw_list_add(&p_tmp->lru, &p_cache->lru);

            if (__NODE_IS_VALID(p_tmp)) {
                __NODE_CLR_VALID(p_tmp);
            }

            p_tmp->blk_no = blk;
            __NODE_SET_VALID(p_cache, p_tmp);
        }

        if (blk == blk_no) {
            *p_node = p_tmp;
            ret = AW_OK;
        }

        nblks--;
        blk++;
        ptr += p_cache->bsize;
    }

    return ret;
}




/******************************************************************************/
static int __node_get (struct aw_blk_cache      *p_cache,
                       uint32_t                  blk_no,
                       aw_bool_t                    is_read,
                       struct __blk_cache_node **p_node)
{
    struct __blk_cache_node *p_tmp;
    int                      ret;

    p_tmp = __node_find(p_cache, blk_no);
    if (p_tmp == NULL) {
        if (is_read) {
            ret = __nodes_load(p_cache, blk_no, &p_tmp);
            if (ret < 0) {
                return ret;
            }
        } else {
            do {
                p_tmp = __node_alloc(p_cache);
                if (p_tmp == NULL) {
                    ret = __cache_flush(p_cache, p_cache->b_blks);
                    if (ret != AW_OK) {
                        return ret;
                    }
                }
            } while (p_tmp == NULL);

            p_tmp->blk_no = blk_no;
            if (__NODE_IS_VALID(p_tmp)) {
                __NODE_CLR_VALID(p_tmp);
                __NODE_SET_VALID(p_cache, p_tmp);
            }
        }
    }

    /* refresh LRU */
    aw_list_del(&p_tmp->lru);
    aw_list_add(&p_tmp->lru, &p_cache->lru);

    *p_node = p_tmp;

    return AW_OK;
}




/******************************************************************************/
static int __cache_flush (struct aw_blk_cache *p_cache,
                          uint32_t             nblks)
{
    struct __blk_cache_node *p_node, *p_tmp, *p_next, *p_dirty;
    uint32_t                 b_blks = 0, blk_no = 0xFFFFFFFF;
    uint8_t                 *ptr = (uint8_t*)p_cache->p_bbuf;
    int                      ret;

    if (nblks == 0) {
        return AW_OK;
    }

    p_dirty = NULL;

    /* get dirty nodes */
    aw_list_for_each_entry_reverse (p_node,
                                    &p_cache->lru,
                                    struct __blk_cache_node,
                                    lru) {
        if (__NODE_IS_DIRTY(p_node)) {
            __dirty_node_add(&p_dirty, p_node);
            nblks--;
        }

        if (nblks == 0) {
            break;
        }
    }

    if (p_dirty) {
        p_node = p_dirty;
        while (p_node) {
            /* copy data to buffer */
            memcpy(ptr, p_node->p_buf, p_cache->bsize);

            b_blks++;
            ptr += p_cache->bsize;

            if (blk_no == 0xFFFFFFFF) {
                /* block number */
                blk_no = p_node->blk_no;
            }

            p_next = p_node->dirty;
            if ((p_next == NULL) ||                           /* if last node */
                ((p_node->blk_no + 1) != p_next->blk_no) ||   /* if continuous node */
                (b_blks >= p_cache->b_blks)                   /* MAX blocks */) {
                /* write data */
                ret = __hw_xfer(p_cache, blk_no, b_blks, AW_FALSE);
                if (ret != AW_OK) {
                    return ret;
                }
                ptr    = (uint8_t*)p_cache->p_bbuf;
                b_blks = 0;
                blk_no = 0xFFFFFFFF;

                /* clear dirty node */
                while (p_dirty != p_next) {
                    p_tmp   = p_dirty;
                    p_dirty = p_dirty->dirty;
                    __NODE_CLR_DIRTY(p_tmp);
                }
            }
            p_node = p_next;
        }
    }

    return AW_OK;
}




/******************************************************************************/
static int __write (struct aw_blk_cache *p_cache,
                    uint32_t             blk_no,
                    void                *p_data,
                    size_t               nbytes,
                    size_t              *residual)
{
    struct __blk_cache_node *p_node;
    int                      ret;
    uint32_t                 len;
    uint8_t                 *ptr;

    ptr = (uint8_t *)p_data;

    while (nbytes) {
        ret = __node_get(p_cache,
                         blk_no,
                         (nbytes < p_cache->bsize) ? AW_TRUE : AW_FALSE,
                         &p_node);
        if (ret < 0) {
            return ret;
        }

        /* copy data */
        len = min(p_cache->bsize, nbytes);
        memcpy(p_node->p_buf, ptr, len);
        ptr    += len;
        nbytes -= len;
        blk_no++;

        if (!__NODE_IS_DIRTY(p_node)) {
            __NODE_SET_DIRTY(p_node);
        }

        if (!__NODE_IS_VALID(p_node)) {
            __NODE_SET_VALID(p_cache, p_node);
        }
    }

    *residual = nbytes;
    return AW_OK;
}

/******************************************************************************/
static int __read (struct aw_blk_cache *p_cache,
                   uint32_t             blk_no,
                   void                *p_data,
                   size_t               nbytes,
                   size_t              *residual)
{
    struct __blk_cache_node *p_node;
    int                      ret = AW_OK;
    uint32_t                 len;
    uint8_t                 *ptr;

    ptr = (uint8_t*)p_data;

    while (nbytes) {
        ret = __node_get(p_cache,
                         blk_no,
                         AW_TRUE,
                         &p_node);
        if (ret < 0) {
            return ret;
        }

        /* copy data */
        len = min(p_cache->bsize, nbytes);
        memcpy(ptr, p_node->p_buf, len);
        ptr    += len;
        nbytes -= len;
        blk_no++;
    }

    *residual = nbytes;

    return ret;
}

/******************************************************************************/
static void __sync_work (void *p_arg)
{
    struct aw_blk_cache     *p_cache;
    AW_INT_CPU_LOCK_DECL(    key);

    AW_FOREVER {
        AW_INT_CPU_LOCK(key);
        aw_list_for_each_entry (p_cache,
                                &__g_sync_list,
                                struct aw_blk_cache,
                                sync) {
            AW_INT_CPU_UNLOCK(key);

            if (AW_MUTEX_LOCK(p_cache->lock, __LOCK_TIMEOUT) == AW_OK) {
                __cache_flush(p_cache, p_cache->b_blks);
                AW_MUTEX_UNLOCK(p_cache->lock);
            }

            AW_INT_CPU_LOCK(key);
        }
        AW_INT_CPU_UNLOCK(key);

        aw_mdelay(__SYNC_PERIOD);
    }
}




/******************************************************************************/
static void __sync_add (struct aw_blk_cache *p_cache)
{
    AW_INT_CPU_LOCK_DECL(key);

    AW_INT_CPU_LOCK(key);
    aw_list_add(&p_cache->sync, &__g_sync_list);
    AW_INT_CPU_UNLOCK(key);
}




/******************************************************************************/
static void __sync_del (struct aw_blk_cache *p_cache)
{
    AW_INT_CPU_LOCK_DECL(key);

    AW_INT_CPU_LOCK(key);
    aw_list_del(&p_cache->sync);
    AW_INT_CPU_UNLOCK(key);
}

/******************************************************************************/
static int __sync_init (void)
{
    AW_INT_CPU_LOCK_DECL(key);

    AW_INT_CPU_LOCK(key);
    if (__g_task_init) {
        AW_INT_CPU_UNLOCK(key);
        return AW_OK;
    }
    aw_list_head_init(&__g_sync_list);
    __g_task_init = AW_TRUE;
    AW_INT_CPU_UNLOCK(key);


    if (AW_TASK_INIT(__g_sync_task,
                     "block cache sync",
                     aw_task_lowest_priority() - 1,
                     __SYNC_TASK_STACK_SIZE,
                     __sync_work,
                     NULL) == NULL) {
        return -AW_ENOMEM;
    }

    AW_TASK_STARTUP(__g_sync_task);

    return AW_OK;
}




/******************************************************************************/
int aw_blk_cache_init (struct aw_blk_cache *p_cache,
                       struct aw_blk_dev   *p_bd,
                       void                *p_buf,
                       uint32_t             size,
                       int                  b_blks)
{
    struct __blk_cache_node *p_node;
    int                      ret, i;
    uint32_t                 n_nodes;
    uint8_t                 *ptr;

    if ((p_cache == NULL) ||
        (p_bd == NULL) ||
        (p_buf == NULL) ||
        (size == 0) ||
        (b_blks == 0)) {
        return -AW_EINVAL;
    }

    __sync_init();

    memset(p_cache, 0, sizeof(*p_cache));

    /* get block size */
    ret = aw_blk_dev_ioctl(p_bd, AW_BD_GET_BSIZE, &p_cache->bsize);
    if (ret != AW_OK) {
        return ret;
    }

    /* block counts */
    ret = aw_blk_dev_ioctl(p_bd, AW_BD_GET_BLKS, &p_cache->nblks);
    if (ret != AW_OK) {
        return ret;
    }

    aw_list_head_init(&p_cache->lru);

    p_cache->p_bd    = p_bd;
    p_cache->b_blks  = b_blks;
    p_cache->p_bbuf  = (uint8_t *)p_buf;

    /* must more than burst size */
    if (size < (b_blks * p_cache->bsize)) {
        return -AW_ENOSPC;
    }

    ptr   = ((uint8_t *)p_buf) + (b_blks * p_cache->bsize);
    size -= (b_blks * p_cache->bsize);

    /* must more than one hash */
    if (size < sizeof(*p_cache->hash)) {
        return -AW_ENOSPC;
    }

    /* calculate node count and hash count */
    for (i = 0; ; i++) {
       n_nodes = (size - (i * sizeof(*p_cache->hash))) / (sizeof(*p_node) + p_cache->bsize);
       if (n_nodes < b_blks) {
           if (i) {
               i--;
           }
           break;
       }

       if (((i * 2) > n_nodes) || (i >= __MAX_HASH)) {
           break;
       }
    }

    size -= (i * sizeof(*p_cache->hash));

    p_cache->hash    = (struct aw_list_head *)ptr;
    p_cache->n_hashs = i;
    p_cache->n_nodes = size / (sizeof(*p_node) + p_cache->bsize);
    if (n_nodes < b_blks) {
        /* not enough memory for cache */
        return -AW_ENOSPC;
    }

    /* initialize nodes */
    ptr += (p_cache->n_hashs * sizeof(*p_cache->hash));
    for (i = 0; i < n_nodes; i++) {
        p_node          = (struct __blk_cache_node *)ptr;
        memset(p_node, 0, sizeof(*p_node));
        ptr            += sizeof(*p_node);
        p_node->p_buf   = ptr;
        ptr            += p_cache->bsize;
        p_node->blk_no  = 0xFFFFFFFF;
        __NODE_CLR_DIRTY(p_node);
        aw_list_head_init(&p_node->hash);
        aw_list_add_tail(&p_node->lru, &p_cache->lru);
    }

    /* hash table */
    for (i = 0; i < p_cache->n_hashs; i++) {
        aw_list_head_init(&p_cache->hash[i]);
    }

    p_bd->p_cache = p_cache;

    memset(&p_cache->lock,0,sizeof(p_cache->lock) );
    if (AW_MUTEX_INIT(p_cache->lock, AW_SEM_Q_PRIORITY) == NULL) {
        return -AW_ENOMEM;
    }

    __sync_add(p_cache);

    return AW_OK;
}




/******************************************************************************/
void aw_blk_cache_uninit (struct aw_blk_cache *p_cache)
{
    __sync_del(p_cache);
    AW_MUTEX_TERMINATE(p_cache->lock);
    p_cache->p_bd->p_cache = NULL;
    return;
}




/******************************************************************************/
int aw_blk_cache_ioctl (struct aw_blk_cache *p_cache,
                        int                  cmd,
                        void                *p_arg)
{
    int  ret;
    switch (cmd) {
    case AW_BD_SYNC:
        ret = AW_MUTEX_LOCK(p_cache->lock, __LOCK_TIMEOUT);
        if (ret != AW_OK) {
            return ret;
        }

        ret = __cache_flush(p_cache, 0xFFFFFFFF);

        AW_MUTEX_UNLOCK(p_cache->lock);
        return ret;
    default:
        return -AW_ENOTSUP;
    }
}




/******************************************************************************/
int aw_blk_cache_areq (struct aw_blk_cache *p_cache,
                       struct aw_blk_arp   *p_arp)
{
    int       ret;
    uint32_t  nblks;
    uint8_t  *ptr;
    uint32_t  blk_no;

    ret = AW_MUTEX_LOCK(p_cache->lock, __LOCK_TIMEOUT);
    if (ret != AW_OK) {
        return ret;
    }

    for (; p_arp != NULL; p_arp = p_arp->p_next) {
        if (p_arp->flags & AW_BD_ARQ_READ) {
            ret = __read(p_cache,
                         p_arp->blk_no,
                         p_arp->p_data,
                         p_arp->nbytes,
                         &p_arp->resid);
        } else {
            ret = __write(p_cache,
                          p_arp->blk_no,
                          p_arp->p_data,
                          p_arp->nbytes,
                          &p_arp->resid);
        }
    }

    AW_MUTEX_UNLOCK(p_cache->lock);
    return ret;
}

