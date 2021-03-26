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
 * \brief aw_slab
 *
 * \internal
 * \par modification history:
 * - 1.01 17-03-09  imp, add aw_slab_memalign()
 * - 1.00 17-03-03  imp, first implementation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/
#include <string.h>


#include "apollo.h"
#include "aw_assert.h"
#include "aw_errno.h"
#include "aw_common.h"
#include "aw_vdebug.h"
#include "aw_sem.h"
#include "aw_bitops.h"
#include "aw_slab.h"


/* Maybe it would be replaced by the marco form AWorks in the future. */
#if 1
#define __SLAB_DEBUG_LOG(type, message)    \
do                                         \
{                                          \
    if (type)                              \
        AW_LOGF(message);                  \
}                                          \
while (0)
#else
#define __SLAB_DEBUG_LOG(type, message)
#endif

/* Misc contents. */
#define __DEBUG_NOT_IN_INTERRUPT   do{}while (0)
#define __OBJECT_HOOK_CALL(func, p)     \
{                                       \
    if(func != NULL){                   \
        func p;                         \
    }                                   \
}

/* The switch of debug statements. */
#define __DEBUG_SLAB               0

#define rt_base_t                   int

/**
 * \brief slab allocator implementation
 *
 * A slab allocator reserves a ZONE for each chunk size, then lays the
 * chunks out in an array within the zone.  Allocation and deallocation
 * is nearly instantanious, and fragmentation/overhead losses are limited
 * to a fixed worst-case amount.
 *
 * The downside of this slab implementation is in the chunk size
 * multiplied by the number of zones.  ~80 zones * 128K = 10MB of VM per cpu.
 * In a kernel implementation all this memory will be physical so
 * the zone size is adjusted downward on machines with less physical
 * memory.  The upside is that overhead is bounded... this is the *worst*
 * case overhead.
 *
 * Slab management is done on a per-cpu basis and no locking or mutexes
 * are required, only a critical section.  When one cpu frees memory
 * belonging to another cpu's slab manager an asynchronous IPI message
 * will be queued to execute the operation.   In addition, both the
 * high level slab allocator and the low level zone allocator optimize
 * M_ZERO requests, and the slab allocator does not have to pre initialize
 * the linked list of chunks.
 *
 * XXX Balancing is needed between cpus.  Balance will be handled through
 * asynchronous IPIs primarily by reassigning the z_Cpu ownership of chunks.
 *
 * XXX If we have to allocate a new zone and M_USE_RESERVE is set, use of
 * the new zone should be restricted to M_USE_RESERVE requests only.
 *
 *    Alloc Size    Chunking        Number of zones
 *    0-127         8               16
 *    128-255       16              8
 *    256-511       32              8
 *    512-1023      64              8
 *    1024-2047     128             8
 *    2048-4095     256             8
 *    4096-8191     512             8
 *    8192-16383    1024            8
 *    16384-32767   2048            8
 *    (if RT_MM_PAGE_SIZE is 4K the maximum zone allocation is 16383)
 *
 *    Allocations >= zone_limit go directly to kmem.
 *
 *            API REQUIREMENTS AND SIDE EFFECTS
 *
 *    To operate as a drop-in replacement to the FreeBSD-4.x malloc() we
 *    have remained compatible with the following API requirements:
 *
 *    + small power-of-2 sized allocations are power-of-2 aligned (kern_tty)
 *    + all power-of-2 sized allocations are power-of-2 aligned (twe)
 *    + malloc(0) is allowed and returns non-RT_NULL (ahc driver)
 *    + ability to allocate arbitrarily large chunks of memory
 */
#define __ZALLOC_SLAB_MAGIC       0x51ab51ab

/*
 * Misc constants.  Note that allocations that are exact multiples of
 * RT_MM_PAGE_SIZE, or exceed the zone limit, fall through to the kmem module.
 */
#define __MIN_CHUNK_SIZE      8        /* in bytes */
#define __MIN_CHUNK_MASK      (__MIN_CHUNK_SIZE - 1)


#define __btokup(p_this, addr)    (&(*p_this).p_memusage[ \
        ((size_t)(addr) - (*p_this).heap_start) \
        >> (*p_this).page_bits])



/**
 * \brief Chunk structure for free elements
 */
typedef struct __slab_chunk
{
    struct __slab_chunk *c_next;
} __slab_chunk;

/**
 * \brief The IN-BAND zone header is placed at the beginning of each zone.
 */
typedef struct __slab_zone
{
    int32_t   z_magic;        /**< \brief magic number for sanity check */

    uint8_t   page_bits;      /**< \brief 1 << page_bits equals to the page size. */
    uint8_t   nzones;         /**< \brief 1 << page_bits equals to the page size. */

    int32_t   z_nfree;        /**< \brief total free chunks / ualloc space in zone */
    int32_t   z_nmax;         /**< \brief maximum free chunks */

    uint32_t  zone_size;      /**< \brief How large a zone.*/
    uint32_t  zone_limit;     /**< \brief The max size of mem alloced from a zone.*/

    struct __slab_zone *z_next; /**< \brief zoneary[] link if z_nfree non-zero */
    uint8_t *z_baseptr;       /**< \brief pointer to start of chunk array */

    int32_t z_uindex;         /**< \brief current initial allocation index */
    int32_t z_chunksize;      /**< \brief chunk size for validation */

    int32_t z_zoneindex;      /**< \brief zone index */
    __slab_chunk  *z_freechunk; /**< \brief free chunk list */

} __slab_zone_t;

/**
 * \brief Array of descriptors that describe the contents of each page
 */
#define __PAGE_TYPE_FREE      0x00
#define __PAGE_TYPE_SMALL     0x01
#define __PAGE_TYPE_LARGE     0x02
#define __PAGE_TYPE_ALIGN     0x03
struct __memusage
{
    uint32_t type:2 ;     /**< \brief page type */
    uint32_t size:30;     /**< \brief pages allocated or offset from zone */
};


/** \brief page allocator */
struct __page_head
{
    struct __page_head *next;   /**< \brief next valid page */
    size_t page;                /**< \brief number of page  */
};


typedef struct __slab_heap_s{

    /**
     * \brief For freindly completing some methods.
     *        It just a copy of the same attribution in struct aw_slab.
     */
    uint8_t   page_bits;
    /** \brief Threshold of free zones. */
    uint8_t   zone_release_thresh;  

    /** \brief How large a zone.*/
    uint32_t  zone_size;      
    /** \brief The max size of mem alloced from a zone.*/
    uint32_t  zone_limit;     
    /** \brief The number of the pages gived to a zone.*/
    uint32_t  zone_page_cnt;  
    /** \brief The number of the free slab in the zone.*/
    uint32_t  zone_free_cnt;  

    /** \brief The start address of heap after alignment. */
    size_t    heap_start;     
    /** \brief The end address of heap after alignment. */
    size_t    heap_end;       

    /** \brief whole zones that have become free. */
    __slab_zone_t         *p_zone_free;
    /** \brief The position of memory usage. */    
    struct __memusage   *p_memusage;   
    /** \brief The position of heap allocation. */
    struct __page_head  *p_page_list;  

    /** \brief It links with the other heaps. */
    struct aw_list_head  list;  

#ifdef AW_SLAB_MEM_STATS
    /** \brief The simple usage of the memory it manages. */
    size_t used_mem, max_mem;   
#endif

    __slab_zone_t      *(pp_zone_array[AW_SLAB_NZONES_MAX]);

    /** \brief It protects the heap. */
    AW_MUTEX_DECL(heap_sem);

}__slab_heap_t;


/**
 * \brief This function allocates memory, in pages.
 */
static
void *__page_alloc(__slab_heap_t *p_this, size_t npages);


/**
 * \brief This function will release the previously
 *        allocated memory by __page_alloc.
 */
static
void  __page_free (__slab_heap_t *p_this, void *addr, size_t npages);


/**
 * \brief This function will init the page of the heap.
 */
static
void  __page_init (__slab_heap_t *p_this, void *addr, size_t npages);


/**
 * \brief Calculate the zone index for the allocation request size and
 *   set the allocation request size to that particular zone's chunk size.
 */
static inline
int __zoneindex(size_t *bytes);


/**
 * \brief This function will init a new heap, in the form of pages.
 */
static
__slab_heap_t  *__slab_heap(void      *begin_addr,
                            void      *end_addr,
                            uint8_t    page_bits,
                            uint8_t    thresh,
                            uint32_t   zone_min_sz,
                            uint32_t   zone_max_sz,
                            uint32_t   zone_max_limit);
static inline
int __ispowof2(size_t n);


static inline
void *__page_alloc_align(__slab_heap_t *p_this,
                         size_t         npages,
                         size_t         alignment);

/******************************************************************************/
void *__page_alloc(__slab_heap_t *p_this, size_t npages)
{

    struct __page_head *b, *n;
    struct __page_head **prev;

    if(npages == 0) return NULL;

    /* lock heap */
    AW_MUTEX_LOCK((*p_this).heap_sem, AW_SEM_WAIT_FOREVER);

    for (prev = &(*p_this).p_page_list; (b = *prev) != NULL; prev = &(b->next))
    {
        if (b->page > npages)
        {
            /* splite pages */
            n = (struct __page_head *)((char *)b + (npages << (*p_this).page_bits));
            n->next = b->next;
            n->page = b->page - npages;
            *prev = n;
            break;
        }

        if (b->page == npages)
        {
            /* this node fit, remove this node */
            *prev = b->next;
            break;
        }
    }

    /* unlock heap */
    AW_MUTEX_UNLOCK((*p_this).heap_sem);

    return b;
}

/******************************************************************************/
void __page_free(__slab_heap_t *p_this, void *addr, size_t npages)
{
    struct __page_head *b, *n;
    struct __page_head **prev;

    aw_assert(addr != NULL);
    aw_assert((size_t)addr % AW_BIT((*p_this).page_bits) == 0);
    aw_assert(npages != 0);

    n = (struct __page_head *)addr;

    /* lock heap */
    AW_MUTEX_LOCK((*p_this).heap_sem, AW_SEM_WAIT_FOREVER);

    for (prev = &(*p_this).p_page_list; (b = *prev) != NULL; prev = &(b->next))
    {
        aw_assert(b->page > 0);
        aw_assert(b > n || (char *)b + (b->page << (*p_this).page_bits) <= (char*)n);

        if ((char *)b + (b->page << (*p_this).page_bits) == (char*)n)
        {
            b->page += npages;
            if ((char *)b + (b->page << (*p_this).page_bits) == (char*)b->next)
            {
                b->page += b->next->page;
                b->next  = b->next->next;
            }

            goto _return;
        }

        if ((char*)b == (char *)n +(npages << (*p_this).page_bits))
        {
            n->page = b->page + npages;
            n->next = b->next;
            *prev = n;

            goto _return;
        }

        if ((char*)b > (char *)n + (npages << (*p_this).page_bits)) break;
    }

    n->page = npages;
    n->next = b;
    *prev = n;

_return:
    /* unlock heap */
    AW_MUTEX_UNLOCK((*p_this).heap_sem);
}


/******************************************************************************/
/**
 * \brief Initialize the page allocator
 */
static void __page_init(__slab_heap_t *p_this, void *addr, size_t npages)
{
    aw_assert(p_this != NULL);
    aw_assert(addr != NULL);
    aw_assert(npages != 0);

    (*p_this).p_page_list = NULL;

    __page_free(p_this, addr, npages);
}


/******************************************************************************/
/**
 * \brief This function will init system heap
 */
void aw_slab_init (struct aw_slab *p_this)

{
    __DEBUG_NOT_IN_INTERRUPT;

    memset(p_this, 0, sizeof(*p_this));

    aw_list_head_init(&(*p_this).heap);

#ifdef AW_SLAB_MEM_STATS
    (*p_this).used_mem = 0;
    (*p_this).max_mem  = 0;
#endif

#ifdef AW_SLAB_USING_HOOK
    (*p_this).pfn_malloc_hook = NULL;
    (*p_this).pfn_free_hook   = NULL;
#endif
}


/******************************************************************************/
__slab_heap_t  *__slab_heap(void      *begin_addr,
                            void      *end_addr,
                            uint8_t    page_bits,
                            uint8_t    thresh,
                            uint32_t   zone_min_sz,
                            uint32_t   zone_max_sz,
                            uint32_t   zone_max_limit)
{
    uint32_t limsize, npages;
    void   * p_begin = NULL;
    void   * p_end   = NULL;

    __slab_heap_t *p_heap = begin_addr;

    p_begin = (void*)AW_ROUND_UP((size_t)begin_addr + sizeof(*p_heap),
                                  AW_BIT(page_bits));

    p_end   = (void*)AW_ROUND_DOWN((size_t)end_addr,
                                    AW_BIT(page_bits));

    if (p_begin >= p_end)
    {
        aw_kprintf("__slab_heap_init, wrong address[0x%p - 0x%p]\n",
                        begin_addr, end_addr);
        return NULL;
    }

    (*p_heap).heap_start =  (size_t)p_begin;
    (*p_heap).heap_end   =  (size_t)p_end;

    (*p_heap).page_bits  =  page_bits;
    (*p_heap).zone_size  =  zone_min_sz;

    (*p_heap).zone_release_thresh = thresh;
    (*p_heap).zone_free_cnt  = 0;
    (*p_heap).p_zone_free    = NULL;

    int i;
    for (i = 0; i < AW_SLAB_NZONES_MAX; ++i){
        (*p_heap).pp_zone_array[i] = NULL;
    }

#ifdef AW_SLAB_MEM_STATS
    (*p_heap).used_mem = 0;
    (*p_heap).max_mem = 0;
#endif

    /* initialize heap linked-list */
    aw_list_head_init(&(*p_heap).list);

    /* initialize heap semaphore */
    memset(&(*p_heap).heap_sem,0,sizeof((*p_heap).heap_sem));
    AW_MUTEX_INIT((*p_heap).heap_sem,  AW_SEM_Q_PRIORITY);

    limsize = (char *)p_end - (char *)p_begin;
    npages = limsize >> (*p_heap).page_bits;

    __SLAB_DEBUG_LOG(__DEBUG_SLAB,
        ("heap[0x%x - 0x%x], size 0x%x, 0x%x pages\n",
           p_begin, p_end, limsize, npages));

    __SLAB_DEBUG_LOG(__DEBUG_SLAB,
        ("[p_heap]:0x%p \n", p_heap));

    /* init pages */
    __page_init(p_heap, p_begin, npages);

    /* calculate zone size */
    while ( (*p_heap).zone_size < zone_max_sz &&
             ((*p_heap).zone_size << 1) < (limsize>>10))
         (*p_heap).zone_size <<= 1;

     (*p_heap).zone_limit = (*p_heap).zone_size >> 2;

     (*p_heap).zone_limit = min((*p_heap).zone_limit, zone_max_limit);

     (*p_heap).zone_page_cnt = (*p_heap).zone_size >> (*p_heap).page_bits;

     __SLAB_DEBUG_LOG(__DEBUG_SLAB,
        ("page_size 0x%x \n", AW_BIT((*p_heap).page_bits)));

    __SLAB_DEBUG_LOG(__DEBUG_SLAB,
        ("zone size 0x%x, zone page count 0x%x\n", (*p_heap).zone_size,
                                                   (*p_heap).zone_page_cnt));

    /* allocate memusage array */
    limsize = npages * sizeof(struct __memusage);
    limsize = AW_ROUND_UP(limsize, AW_BIT((*p_heap).page_bits));

    (*p_heap).p_memusage = __page_alloc(p_heap, limsize>>(*p_heap).page_bits);

    __SLAB_DEBUG_LOG(__DEBUG_SLAB,
        ("[memusage] 0x%x, size 0x%x\n", (uint32_t)(*p_heap).p_memusage,
                                          limsize));

    return p_heap;
}


/******************************************************************************/
int __zoneindex(size_t *bytes)
{
    uint32_t n = (uint32_t)*bytes;    /* unsigned for shift opt */

    if (n < 128)
    {
        *bytes = n = (n + 7) & ~7;
        return(n / 8 - 1);        /* 8 byte chunks, 16 zones */
    }
    if (n < 256)
    {
        *bytes = n = (n + 15) & ~15;
        return(n / 16 + 7);
    }
    if (n < 8192)
    {
        if (n < 512)
        {
            *bytes = n = (n + 31) & ~31;
            return(n / 32 + 15);
        }
        if (n < 1024)
        {
            *bytes = n = (n + 63) & ~63;
            return(n / 64 + 23);
        }
        if (n < 2048)
        {
            *bytes = n = (n + 127) & ~127;
            return(n / 128 + 31);
        }
        if (n < 4096)
        {
            *bytes = n = (n + 255) & ~255;
            return(n / 256 + 39);
        }
        *bytes = n = (n + 511) & ~511;
        return(n / 512 + 47);
    }
    if (n < 16384)
    {
        *bytes = n = (n + 1023) & ~1023;
        return(n / 1024 + 55);
    }

    aw_kprintf("Unexpected byte count %d", n);
    return 0;
}


/******************************************************************************/
aw_err_t  aw_slab_heap_catenate(struct aw_slab *p_this,
                                void           *begin_addr,
                                void           *end_addr,
                                uint8_t         page_bits,
                                uint8_t         thresh,
                                uint32_t        zone_min_sz,
                                uint32_t        zone_max_sz,
                                uint32_t        zone_max_limit)
{

    __DEBUG_NOT_IN_INTERRUPT;

    __slab_heap_t *p_heap = __slab_heap (begin_addr,
                                         end_addr,
                                         page_bits,
                                         thresh,
                                         zone_min_sz,
                                         zone_max_sz,
                                         zone_max_limit);
    if(NULL == p_heap){

        __SLAB_DEBUG_LOG(__DEBUG_SLAB,
                        ("aw_slab_heap_concatenate() failed.\n"));

        return -AW_EINVAL;
    }

    aw_list_add_tail(&(*p_heap).list, &(*p_this).heap);

    return AW_OK;

}


/******************************************************************************/
/**
 * This function will allocate a block from system heap memory.
 */
void *aw_slab_malloc(struct aw_slab *p_this, size_t size)
{
     __slab_zone_t *z;
    int32_t zi;
    __slab_chunk *chunk;
    struct __memusage *kup;

    uint32_t  page_bits;
    __slab_heap_t *p_pos;

    /* There are no memory within the instance. */
    if(aw_list_empty(&(*p_this).heap)){

        AW_ERRNO_SET(-AW_ENOMEM);
        return NULL;
    }

    /* zero size, is allowed. */
    if (size == 0)
        size = 1;

    aw_list_for_each_entry(p_pos, &(*p_this).heap, __slab_heap_t, list){

        page_bits = (*p_pos).page_bits;

        /*
         * Handle large allocations directly.  There should not be very many of
         * these so performance is not a big issue.
         */
        if (size >= (*p_pos).zone_limit) {

            size = AW_ROUND_UP(size, AW_BIT(page_bits));

            chunk = __page_alloc(p_pos, size >> page_bits);
            if (chunk == NULL) {
                if(aw_list_is_last(&(*p_pos).list, &(*p_this).heap)) {
                    AW_ERRNO_SET(-AW_ENOMEM);
                    return NULL;

                } else {
                    continue ;
                }
            }
            __SLAB_DEBUG_LOG(__DEBUG_SLAB,
                             ("malloc a large memory 0x%x, page cnt %d, kup %d\n",
                             size,
                             size >> page_bits,
                             ((uint32_t)chunk - (*p_pos).heap_start) >> page_bits));

            /* set kup */
            kup = __btokup(p_pos,chunk);

            AW_MUTEX_LOCK((*p_pos).heap_sem, AW_SEM_WAIT_FOREVER);

            kup->type = __PAGE_TYPE_LARGE;
            kup->size = size >> page_bits;

            #ifdef AW_SLAB_MEM_STATS
            (*p_pos).used_mem += size;
            (*p_pos).max_mem = max((*p_pos).max_mem, (*p_pos).used_mem);
            #endif

            goto done;
        }

        /*
         * Attempt to allocate out of an existing zone.  First try the free list,
         * then allocate out of unallocated space.  If we find a good zone move
         * it to the head of the list so later allocations find it quickly
         * (we might have thousands of zones in the list).
         *
         * Note: __zoneindex() will panic of size is too large.
         */
        zi = __zoneindex(&size);
        aw_assert(zi < AW_SLAB_NZONES_MAX);

        __SLAB_DEBUG_LOG(__DEBUG_SLAB, ("try to malloc 0x%x on zone: %d\n", size, zi));

        /* lock heap */
        AW_MUTEX_LOCK((*p_pos).heap_sem, AW_SEM_WAIT_FOREVER);

        if ((z = (*p_pos).pp_zone_array[zi]) != NULL){

            aw_assert(z->z_nfree > 0);

            /* Remove us from the zone_array[] when we become empty */
            if (--z->z_nfree == 0) {

                (*p_pos).pp_zone_array[zi] = z->z_next;
                z->z_next = NULL;
            }

            /*
             * No chunks are available but nfree said we had some memory, so
             * it must be available in the never-before-used-memory area
             * governed by uindex.  The consequences are very serious if our zone
             * got corrupted so we use an explicit rt_kprintf rather then a KASSERT.
             */
            if (z->z_uindex + 1 != z->z_nmax) {

                z->z_uindex = z->z_uindex + 1;
                chunk = (__slab_chunk *)(z->z_baseptr + z->z_uindex * size);

            } else {

                /* find on free chunk list */
                chunk = z->z_freechunk;

                /* remove this chunk from list */
                z->z_freechunk = z->z_freechunk->c_next;
            }

            #ifdef AW_SLAB_MEM_STATS
            (*p_pos).used_mem += z->z_chunksize;
            (*p_pos).max_mem = max((*p_pos).max_mem, (*p_pos).used_mem);
            #endif

            goto done;
        }

        /*
         * If all zones are exhausted we need to allocate a new zone for this
         * index.
         *
         * At least one subsystem, the tty code (see CROUND) expects power-of-2
         * allocations to be power-of-2 aligned.  We maintain compatibility by
         * adjusting the base offset below.
         */
        {
            int32_t off;

            if ((z = (*p_pos).p_zone_free) != NULL) {

                /* remove zone from free zone list */
                (*p_pos).p_zone_free = z->z_next;
                --(*p_pos).zone_free_cnt;

            } else {

                /* unlock heap, since page allocator will think about lock */
                AW_MUTEX_UNLOCK((*p_pos).heap_sem);

                /* allocate a zone from page */
                z = __page_alloc(p_pos, (*p_pos).zone_page_cnt);

                if (z == NULL){
                    if(aw_list_is_last(&(*p_pos).list, &(*p_this).heap)){
                        AW_ERRNO_SET(-AW_ENOMEM);
                        return NULL;

                    } else {
                        continue ;
                    }
                }

                __SLAB_DEBUG_LOG(__DEBUG_SLAB, ("alloc a new zone: 0x%x\n", (uint32_t)z));

                /* lock heap */
                AW_MUTEX_LOCK((*p_pos).heap_sem, AW_SEM_WAIT_FOREVER);

                /* set message usage */
                for (off = 0, kup = __btokup(p_pos,z); off < (*p_pos).zone_page_cnt; off++) {

                    kup->type = __PAGE_TYPE_SMALL;
                    kup->size = off;
                    kup ++;
                }
            }
            /* clear to zero */
            memset(z, 0, sizeof(__slab_zone_t));

            /* offset of slab zone struct in zone */
            off = sizeof(__slab_zone_t);

            /*
             * Guarentee power-of-2 alignment for power-of-2-sized chunks.
             * Otherwise just 8-byte align the data.
             */
            if ((size | (size - 1)) + 1 == (size << 1))
                off = (off + size - 1) & ~(size - 1);
            else
                off = (off + __MIN_CHUNK_MASK) & ~__MIN_CHUNK_MASK;

            z->z_magic     = __ZALLOC_SLAB_MAGIC;
            z->z_zoneindex = zi;
            z->z_nmax      = ((*p_pos).zone_size - off) / size;
            z->z_nfree     = z->z_nmax - 1;
            z->z_baseptr   = (uint8_t *)z + off;
            z->z_uindex    = 0;
            z->z_chunksize = size;

            chunk = (__slab_chunk *)(z->z_baseptr + z->z_uindex * size);

            /* link to zone array */
            z->z_next = (*p_pos).pp_zone_array[zi];
            (*p_pos).pp_zone_array[zi] = z;

            #ifdef AW_SLAB_MEM_STATS
            (*p_pos).used_mem += z->z_chunksize;
            (*p_pos).max_mem = max((*p_pos).used_mem, (*p_pos).max_mem);
            #endif

            goto done;
        }
    }

fail:
    aw_assert(0);
    AW_ERRNO_SET(-AW_ENOMEM);
    return NULL;

done:
    AW_MUTEX_UNLOCK((*p_pos).heap_sem);

#ifdef AW_SLAB_USING_HOOK
    __OBJECT_HOOK_CALL((*p_this).pfn_malloc_hook, ((char *)chunk, size));
#endif

    return chunk;

}


/******************************************************************************/
/**
 * \brief This function will change the size of previously allocated memory block.
 */
void *aw_slab_realloc(struct aw_slab *p_this, void *ptr, size_t size)
{
    void *nptr;
    __slab_zone_t *z;
    struct __memusage *kup;

    if (ptr == NULL) return aw_slab_malloc(p_this, size);
    if (size == 0)
    {
        aw_slab_free(p_this, ptr);
        return NULL;
    }

    __slab_heap_t *p_pos;
    size_t  tmp = 0;

    aw_list_for_each_entry(p_pos, &(*p_this).heap, __slab_heap_t, list){

        if((void*)p_pos > ptr){
            continue ;

        } else {
            tmp = max(tmp, (size_t)p_pos);
        }
    }
    aw_assert(tmp);

    p_pos = (void*)tmp;

    uint8_t page_bits = (*p_pos).page_bits;

    /*
     * Get the original allocation's zone.  If the new request winds up
     * using the same chunk size we do not have to do anything.
     */
    kup = __btokup(p_pos,((size_t)ptr & ~AW_BITS_MASK(page_bits)));

    if (kup->type == __PAGE_TYPE_LARGE || kup->type == __PAGE_TYPE_ALIGN){

        size_t osize;

        osize = kup->size << page_bits;

        if ((nptr = aw_slab_malloc(p_this, size)) == NULL){
            AW_ERRNO_SET(-AW_ENOMEM);
            return NULL;
        }
        memcpy(nptr, ptr, size > osize ? osize : size);
        aw_slab_free(p_this, ptr);

        return nptr;

    } else if (kup->type == __PAGE_TYPE_SMALL) {

        z = (__slab_zone_t *)(((size_t)ptr & ~AW_BITS_MASK(page_bits))
          - (kup->size << page_bits));

        aw_assert(z->z_magic == __ZALLOC_SLAB_MAGIC);

        __zoneindex(&size);
        if (z->z_chunksize == size) return(ptr); /* same chunk */

        /*
         * Allocate memory for the new request size.  Note that __zoneindex has
         * already adjusted the request size to the appropriate chunk size, which
         * should optimize our bcopy().  Then copy and return the new pointer.
         */
        if ((nptr = aw_slab_malloc(p_this, size)) == NULL) {
            AW_ERRNO_SET(-AW_ENOMEM);
            return NULL;
        }

        memcpy(nptr, ptr, size > z->z_chunksize ? z->z_chunksize : size);
        aw_slab_free(p_this, ptr);

        return nptr;
    }

    AW_ERRNO_SET(-AW_ENOMEM);
    return NULL;
}


/******************************************************************************/
/**
 * This function will contiguously allocate enough space for count objects
 * that are size bytes of memory each and returns a pointer to the allocated
 * memory.
 */
void *aw_slab_calloc(struct aw_slab *p_this, size_t count, size_t size)
{
    if(0 == count || 0 == size){
        return NULL;
    }

    void *p;

    /* allocate 'count' objects of size 'size' */
    p = aw_slab_malloc(p_this, count * size);

    /* zero the memory */
    if (p != NULL){
        memset(p, 0, count * size);
    } else {
        AW_ERRNO_SET(-AW_ENOMEM);
    }

    return p;
}


/******************************************************************************/
/**
 * This function will release the previously allocated memory block by malloc.
 * The released memory block is taken back to system heap.
 */
void aw_slab_free(struct aw_slab *p_this, void *ptr)
{

    __slab_zone_t *z;
    __slab_chunk *chunk;
    struct __memusage *kup;

    /* free a NULL pointer */
    if (ptr == NULL) return ;

#ifdef AW_SLAB_USING_HOOK
    __OBJECT_HOOK_CALL((*p_this).pfn_free_hook, (ptr));
#endif

    aw_assert(!aw_list_empty(&(*p_this).heap));

    __slab_heap_t *p_pos;
    size_t  tmp = 0;

    aw_list_for_each_entry(p_pos, &(*p_this).heap, __slab_heap_t, list){

        if((void*)p_pos > ptr){
            continue ;

        } else {
            tmp = max(tmp, (size_t)p_pos);
        }
    }
    aw_assert(tmp);

    p_pos = (void*)tmp;

    uint8_t page_bits = (*p_pos).page_bits;


    /* get memory usage */
#if __DEBUG_SLAB
    {
        uint32_t addr = ((uint32_t)ptr & ~AW_BITS_MASK(page_bits));
        __SLAB_DEBUG_LOG(__DEBUG_SLAB,
            ("free a memory 0x%x and align to 0x%x, kup index %d\n",
            (uint32_t)ptr,
            (uint32_t)addr,
            ((uint32_t)(addr) - (*p_pos).heap_start) >> page_bits));
    }
#endif

    kup = __btokup(p_pos,(size_t)ptr & ~AW_BITS_MASK(page_bits));
    /* release large allocation */
    if (kup->type == __PAGE_TYPE_LARGE || kup->type == __PAGE_TYPE_ALIGN)
    {
        uint32_t size;

        /* lock heap */
        AW_MUTEX_LOCK((*p_pos).heap_sem, AW_SEM_WAIT_FOREVER);

        /* clear page counter */
        size = kup->size;
        kup->size = 0;

#ifdef AW_SLAB_MEM_STATS
        (*p_pos).used_mem -= size << page_bits;
#endif

        AW_MUTEX_UNLOCK((*p_pos).heap_sem);

        __SLAB_DEBUG_LOG(__DEBUG_SLAB,
            ("free large memory block 0x%x, page count %d\n", (uint32_t)ptr, size));

        /* free this page */
        __page_free(p_pos, ptr, size);
        return;
    }

    /* lock heap */
    AW_MUTEX_LOCK((*p_pos).heap_sem, AW_SEM_WAIT_FOREVER);

    /* zone case. get out zone. */
    z = (__slab_zone_t *)(((size_t)ptr & ~AW_BITS_MASK(page_bits))
                     - (kup->size << page_bits));

    aw_assert(z->z_magic == __ZALLOC_SLAB_MAGIC);

    chunk = (__slab_chunk *)ptr;
    chunk->c_next = z->z_freechunk;
    z->z_freechunk = chunk;

#ifdef AW_SLAB_MEM_STATS
    (*p_pos).used_mem -= z->z_chunksize;
#endif

    /*
     * Bump the number of free chunks.  If it becomes non-zero the zone
     * must be added back onto the appropriate list.
     */
    if (z->z_nfree++ == 0)
    {
        z->z_next = (*p_pos).pp_zone_array[z->z_zoneindex];
        (*p_pos).pp_zone_array[z->z_zoneindex] = z;
    }

    /*
     * If the zone becomes totally free, and there are other zones we
     * can allocate from, move this zone to the FreeZones list.  Since
     * this code can be called from an IPI callback, do *NOT* try to mess
     * with kernel_map here.  Hysteresis will be performed at malloc() time.
     */
    if (z->z_nfree == z->z_nmax &&
         (z->z_next ||
            (*p_pos).pp_zone_array[z->z_zoneindex] != z))
    {
        __slab_zone_t **pz;

        __SLAB_DEBUG_LOG(__DEBUG_SLAB,
                ("free zone 0x%x, index %x\n", (uint32_t)z, z->z_zoneindex));

        /* remove zone from zone array list */
        for (pz = &(*p_pos).pp_zone_array[z->z_zoneindex];
             z != *pz;
             pz = &(*pz)->z_next);
        *pz = z->z_next;

        /* reset zone */
        z->z_magic = -1;

        /* insert to free zone list */
        z->z_next = (*p_pos).p_zone_free;
        (*p_pos).p_zone_free = z;

        ++ (*p_pos).zone_free_cnt;

        /* release zone to page allocator */
        if ((*p_pos).zone_free_cnt > (*p_pos).zone_release_thresh)
        {
            register rt_base_t i;

            z = (*p_pos).p_zone_free;
            (*p_pos).p_zone_free = z->z_next;
            -- (*p_pos).zone_free_cnt;

            /* set message usage */
            for (i = 0, kup = __btokup(p_pos,z);
                 i < (*p_pos).zone_page_cnt;
                 i ++)
            {
                kup->type = __PAGE_TYPE_FREE;
                kup->size = 0;
                kup ++;
            }
            /* unlock heap */
            AW_MUTEX_UNLOCK((*p_pos).heap_sem);

            /* release pages */
            __page_free(p_pos, z, (*p_pos).zone_page_cnt);
            return;
        }
    }
    /* unlock heap */
    AW_MUTEX_UNLOCK((*p_pos).heap_sem);
}




/******************************************************************************/
/**
 * \brief The origin of size_t must be decorated by unsigned.
 * n should greater than 0
 */
int __ispowof2(size_t n)
{
    return ((n > 0) && ((n & (n-1)) == 0));
}


void *__page_alloc_align(__slab_heap_t *p_this, size_t npages, size_t alignment)
{
    struct __page_head *b, *n;
    struct __page_head **prev;

    if(npages == 0) return NULL;

    size_t    align_pages;
    uint32_t  page_bits;

    page_bits   = (*p_this).page_bits;
    align_pages = 0;

    if(AW_BIT(page_bits) < alignment ){

        align_pages = alignment >> page_bits;
        aw_assert(alignment % AW_BIT(page_bits) == 0);
    }

    /* lock heap */
    AW_MUTEX_LOCK((*p_this).heap_sem, AW_SEM_WAIT_FOREVER);

    for (prev = &(*p_this).p_page_list;
        (b = *prev) != NULL;
        prev = &(b->next)){

        size_t mod;

        mod = (size_t)b % alignment;

        if(0 == mod){

            if (b->page > npages){

                /* splite pages */
                n = (struct __page_head *)((char *)b
                    + (npages << page_bits));

                n->next = b->next;
                n->page = b->page - npages;
                *prev = n;
                break;
            }

            if (b->page == npages){

                /* this node fit, remove this node */
                *prev = b->next;
                break;
            }

        } else {

            mod >>= page_bits;

            aw_assert(mod != 0);
            aw_assert(align_pages != 0);
            aw_assert(align_pages > mod);

            mod = align_pages - mod;

            if (b->page > npages + mod){

                /* splite pages */
                n = (struct __page_head *)((char *)b
                  + ((npages + mod) << page_bits));

                n->next = b->next;
                n->page = b->page - (npages + mod);

                b->next = n;
                b->page = mod;
                *prev = b;

                b = (struct __page_head *)((char *)b
                  + (mod << page_bits));
                break;
            }

            if (b->page == npages + mod){

                b->page -= npages;

                b = (struct __page_head *)((char *)b
                  + (mod << page_bits));

                break;
            }
        }
    }

    /* unlock heap */
    AW_MUTEX_UNLOCK((*p_this).heap_sem);

    return b;
}


aw_err_t aw_slab_memalign(struct aw_slab *p_this,
                          void          **pp_memptr,
                          size_t          alignment,
                          size_t          size)
{
    if(!__ispowof2(alignment) || (alignment % sizeof(void*) != 0)){
        return -AW_EINVAL;
    }

    if (size == 0){
        *pp_memptr = NULL;
        return AW_OK;
    }

    /* There are no memory within the instance. */
    if(aw_list_empty(&(*p_this).heap)){
        return -AW_ENOMEM;
    }

    uint32_t  page_bits;

    __slab_chunk        *chunk;
    __slab_heap_t       *p_pos;
    struct __memusage   *kup;

    aw_list_for_each_entry(p_pos, &(*p_this).heap, __slab_heap_t, list){

        page_bits = (*p_pos).page_bits;

        size = AW_ROUND_UP(size, AW_BIT(page_bits));

        chunk = __page_alloc_align(p_pos, size >> page_bits, alignment);

        if (chunk == NULL) {
            if(aw_list_is_last(&(*p_pos).list, &(*p_this).heap)) {
                return -AW_ENOMEM;

            } else {
                continue ;
            }
        }
        __SLAB_DEBUG_LOG(__DEBUG_SLAB, ("malloc a aligned memory 0x%x, page cnt %d, kup %d\n",
                size,
                size >> page_bits,
                ((uint32_t)chunk - (*p_pos).heap_start) >> page_bits));

        /* set kup */
        kup = __btokup(p_pos,chunk);

        AW_MUTEX_LOCK((*p_pos).heap_sem, AW_SEM_WAIT_FOREVER);

        kup->type = __PAGE_TYPE_ALIGN;
        kup->size = size >> page_bits;

#ifdef AW_SLAB_MEM_STATS
        (*p_pos).used_mem += size;
        (*p_pos).max_mem   = max((*p_pos).max_mem, (*p_pos).used_mem);
 #endif
        goto done;
    }

done:
    AW_MUTEX_UNLOCK((*p_pos).heap_sem);

    *pp_memptr = chunk;

    return AW_OK;
}


/******************************************************************************/
#ifdef AW_SLAB_MEM_STATS
aw_err_t aw_slab_memory_info (struct aw_slab *p_this,
                              uint32_t       *p_total,
                              uint32_t       *p_used,
                              uint32_t       *p_max_used)
{
    uint32_t tmp0 = 0;
    uint32_t tmp1 = 0;
    uint32_t tmp2 = 0;
    __slab_heap_t *p_pos;

    /* There are no memory within the instance. */
    if(aw_list_empty(&(*p_this).heap)){
        return -AW_ENOMEM;
    }

    aw_list_for_each_entry(p_pos, &(*p_this).heap, __slab_heap_t, list){
            tmp0 += (*p_pos).heap_end - (*p_pos).heap_start;
            tmp1 += (*p_pos).used_mem;
            tmp2 += max((*p_pos).max_mem, tmp2);
    }

    if (p_total != NULL) {
        *p_total = tmp0;
    }
    if (p_used  != NULL) {
        *p_used = tmp1;
    }
    if (p_max_used != NULL){
        *p_max_used = tmp2;
    }

    return AW_OK;
}

#endif


/******************************************************************************/
#ifdef AW_SLAB_USING_HOOK
void aw_slab_malloc_sethook(struct aw_slab *p_this,
                            void          (*pfn_hook)(void *ptr, size_t size))
{
    (*p_this).pfn_malloc_hook = pfn_hook;
}

void aw_slab_free_sethook(struct aw_slab *p_this,
                          void          (*pfn_hook)(void *ptr))
{
    (*p_this).pfn_free_hook = pfn_hook;
}

#endif

/******************************************************************************/
#define __1K          (1UL<<10)
#define __2K          (1UL<<11)
#define __4K          (1UL<<12)
#define __8K          (1UL<<13)
#define __16K         (1UL<<14)
#define __32K         (1UL<<15)
#define __64K         (1UL<<16)
#define __128K        (1UL<<17)
#define __256K        (1UL<<18)
#define __512K        (1UL<<19)

#define __1M          (1UL<<20)
#define __2M          (1UL<<21)
#define __4M          (1UL<<22)
#define __16M         (1UL<<24)
#define __64M         (1UL<<26)
#define __256M        (1UL<<28)
#define __1G          (1UL<<30)

#define __MAX_ZONE_LIMIT   __16K    /* Conclusion from __zoneindex() */

struct __easy_cfg{
    int         mem_size;
    uint8_t     bits;
    uint8_t     thresh;
    uint32_t    zone_min_sz;
    uint32_t    zone_max_sz;
    uint32_t    zone_max_limit;
};

// Modify
static const
struct __easy_cfg __g_slab_cfg[] = {
    {__8K,   5,  2, 256,    512,    64},                /**< \brief 8KB */
    {__16K,  5,  2, 512,    __1K,   128},               /**< \brief 16KB */
    {__32K,  6,  2, __1K,   __4K,   256},               /**< \brief 32KB */
    {__256K, 7,  2, __2K,   __4K,   512},               /**< \brief 256KB */
    {__1M,   8,  2, __8K,   __32K,  __1K},              /**< \brief 1MB */
    {__16M,  10, 2, __64K,  __128K, __8K},              /**< \brief 16MB */
    {__64M,  12, 4, __128K, __2M, __MAX_ZONE_LIMIT},    /**< \brief 64MB */
};

/******************************************************************************/
aw_err_t  aw_slab_easy_catenate(struct aw_slab *p_this,
                                char           *p_mem,
                                uint32_t        mem_size)
{
    if(NULL == p_mem || NULL == p_this
    || mem_size < __g_slab_cfg[0].mem_size ){
        return  -AW_EINVAL;
    }

    int index;

    for(index = 0; index < AW_NELEMENTS(__g_slab_cfg); index++){
        if(mem_size >= __g_slab_cfg[index].mem_size){
            continue;

        } else {
            index--;
            break;
        }
    }

    if (index > AW_NELEMENTS(__g_slab_cfg) - 1) {
        index = AW_NELEMENTS(__g_slab_cfg) - 1;
    }

    aw_assert(index >= 0);

    aw_slab_heap_catenate (p_this,
                           p_mem,
                           p_mem + mem_size,
                         __g_slab_cfg[index].bits,
                         __g_slab_cfg[index].thresh,
                         __g_slab_cfg[index].zone_min_sz,
                         __g_slab_cfg[index].zone_max_sz,
                         __g_slab_cfg[index].zone_max_limit);

    return AW_OK;
}

/*@}*/
/* end of file*/
