/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief memory management
 *
 * \internal
 * \par modification history:
 * - 1.00 14-06-13  zen, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "aw_mem.h"
#include "aw_memheap.h"
#include "aw_cache.h"
#include "aw_slab.h"
#include "aw_assert.h"

#include <string.h>

/*******************************************************************************
  locals
*******************************************************************************/


#ifdef  AW_SYS_HEAP_USE_SLAB

static struct aw_slab  __g_system_heap;       /* system heap object */
static struct aw_slab  __g_dma_safe_heap;     /* dma-safe heap object */

#else

static struct aw_memheap __g_system_heap;       /* system heap object */
static struct aw_memheap __g_dma_safe_heap;     /* dma-safe heap object */

#endif

/* 判断一个数是否为2的n次方 */
#define     __IS_NOT_SECOND_PARTY(a)    (((a) & ((a)-1)) ? 1 : 0)

/*******************************************************************************
  implementation
*******************************************************************************/

/******************************************************************************/
void aw_bsp_system_heap_init(void)
{
#if defined (__CC_ARM)
    extern  unsigned int const Image$$ER_HEAP_START$$Base;          /* Defined by the linker */
    extern  unsigned int const Image$$ER_HEAP_END$$Base ;           /* Defined by the linker */

    char *heap_start = (char *)&Image$$ER_HEAP_START$$Base;;
    char *heap_end   = (char *)&Image$$ER_HEAP_END$$Base;

#elif  defined (__GNUC__)
    extern int __heap_start__;          /* Defined by the linker */
    extern int __heap_end__;            /* Defined by the linker */

    char *heap_start = (char *)&__heap_start__;
    char *heap_end   = (char *)&__heap_end__;
#endif

#ifdef  AW_SYS_HEAP_USE_SLAB
    aw_err_t  ret = AW_OK;

    aw_slab_init(&__g_system_heap);

    ret = aw_slab_easy_catenate(&__g_system_heap, heap_start, (uint32_t)(heap_end - heap_start));
    aw_assert(ret == AW_OK);

#else
    /* initialize a default heap in the system */
    aw_memheap_init(&__g_system_heap,
                    "system_heap",
                    (void *)heap_start,
                    (uint32_t)heap_end - (uint32_t)heap_start);
#endif


}

/******************************************************************************/
void *aw_mem_align(size_t size, size_t align)
{

#ifdef  AW_SYS_HEAP_USE_SLAB

    int  ret = 0;

    void *ptr = NULL;

    ret = aw_slab_memalign(&__g_system_heap, &ptr, align, size);
    if (ret != AW_OK) {
        return  NULL;
    }

    return  ptr;

#else
    void    *align_ptr;
    void    *ptr;
    size_t   align_size;

    if (__IS_NOT_SECOND_PARTY(align) || (align <= 1)) {
        return NULL;
    }

    /* align the alignment size to 4 byte */
    align = AW_ROUND_UP(align, sizeof(void *));

    /* get total aligned size */
    align_size =  AW_ROUND_UP(size, sizeof(void *)) + align + sizeof(void *);

    /* allocate memory block from heap */
    ptr = aw_memheap_alloc(&__g_system_heap, align_size);
    if (ptr != NULL) {

        /* round up ptr to align */
        align_ptr = (void *)AW_ROUND_UP((uint32_t)ptr + sizeof(void *), align);

        /* set the pointer before alignment pointer to the real pointer */
        *((uint32_t *)((uint32_t)align_ptr - sizeof(void *))) = (uint32_t)ptr;

        ptr = align_ptr;
    }

    return ptr;
#endif

}

/******************************************************************************/
void *aw_mem_alloc(size_t size)
{
#ifdef  AW_SYS_HEAP_USE_SLAB

    return aw_slab_malloc(&__g_system_heap, size);

#else
    /* allocate in the system heap */
    return aw_mem_align(size, 4);
#endif

}

/******************************************************************************/
void *aw_mem_calloc(size_t nelem, size_t size)
{
    void       *ptr;
    size_t      total_size;

    total_size = nelem * size;
    ptr = aw_mem_alloc(total_size);
    if (ptr != NULL) {
        memset(ptr, 0, total_size);
    }

    return ptr;
}

/******************************************************************************/
void aw_mem_free(void *ptr)
{
#ifdef  AW_SYS_HEAP_USE_SLAB

    aw_slab_free(&__g_system_heap, ptr);

#else
    void *real_ptr = NULL;

    if (ptr != NULL) {
        real_ptr = (void *)*(uint32_t *)((uint32_t)ptr - sizeof(void *));
    }
    aw_memheap_free(real_ptr);
#endif
}

/******************************************************************************/
void *aw_mem_realloc(void *ptr, size_t newsize)
{
#ifdef  AW_SYS_HEAP_USE_SLAB

    return  aw_slab_realloc(&__g_system_heap, ptr, newsize);

#else
    void  *real_ptr     = NULL;
    void  *new_ptr      = NULL;

    if (ptr != NULL) {
        /* get real pointer */
        real_ptr = (void *)*(uint32_t *)((uint32_t)ptr - sizeof(void *));
    } else {
        /* equal malloc when ptr==NULL */
        return aw_mem_alloc(newsize);
    }

    /* allocate newsize plus one more pointer, used for save real pointer */
    real_ptr = aw_memheap_realloc(&__g_system_heap,
                                  real_ptr,
                                  newsize + sizeof(void *));

    if (real_ptr != NULL) {
        *(uint32_t *)real_ptr = (uint32_t)real_ptr;
        new_ptr               = (void *)((size_t)real_ptr + sizeof(void *));
    } else {
        new_ptr = NULL;
    }

    return new_ptr;
#endif

}

/******************************************************************************/
void *aw_memp_alloc(aw_heap_id_t heap_id,size_t size)
{

#ifdef  AW_SYS_HEAP_USE_SLAB

    struct aw_slab *heap = (struct aw_slab *)heap_id;

    /* allocate memory from system heap */
    if (heap == NULL) {
        return aw_mem_alloc(size);
    }

    /* allocate memory from specify heap */
    return aw_slab_malloc(heap, size);

#else
    aw_memheap_t *heap = (aw_memheap_t *)heap_id;

    /* allocate memory from system heap */
    if (heap == NULL) {
        return aw_mem_alloc(size);
    }

    /* allocate memory from specify heap */
    return aw_memheap_alloc(heap, size);
#endif

}

/******************************************************************************/
void aw_bsp_dma_safe_heap_init(void)
{
#if defined (__CC_ARM)
    extern  unsigned int const Image$$ER_DMA_HEAP_START$$Base;      /* Defined by the linker */
    extern  unsigned int const Image$$ER_DMA_HEAP_END$$Base ;       /* Defined by the linker */

    char *heap_start = (char *)&Image$$ER_DMA_HEAP_START$$Base;;
    char *heap_end   = (char *)&Image$$ER_DMA_HEAP_END$$Base;

#elif  defined (__GNUC__)
    extern char __heap_dma_start__;     /* Defined by the linker */
    extern char __heap_dma_end__;       /* Defined by the linker */

    char *heap_start = &__heap_dma_start__;
    char *heap_end   = &__heap_dma_end__;
#endif

#ifdef  AW_SYS_HEAP_USE_SLAB

    aw_err_t  ret = AW_OK;

    aw_slab_init(&__g_dma_safe_heap);
    ret = aw_slab_easy_catenate(&__g_dma_safe_heap, heap_start, (uint32_t)(heap_end - heap_start));
    aw_assert(ret == AW_OK);

#else

    /* initialize a default heap in the system */
    aw_memheap_init(&__g_dma_safe_heap,
                    "dma_safe_heap",
                    (void *)heap_start,
                    (uint32_t)heap_end - (uint32_t)heap_start);
#endif


}


/******************************************************************************/
void *aw_cache_dma_align (size_t nbytes, size_t align)
{

#ifdef  AW_SYS_HEAP_USE_SLAB

    int  ret = 0;

    void *ptr = NULL;

    ret = aw_slab_memalign(&__g_dma_safe_heap, &ptr, align, nbytes);
    if (ret != AW_OK) {
        return  NULL;
    }

    return  ptr;

#else

    size_t  align_size;
    void   *ptr;
    void   *aligned_ptr;

    if (__IS_NOT_SECOND_PARTY(align) || (align <= 1)) {
        return NULL;
    }

    /* align the alignment size to cache line size */
    align  = AW_ROUND_UP(align, AW_CACHE_LINE_SIZE);

    /* align allocate size to cache line size */
    nbytes = AW_ROUND_UP(nbytes, AW_CACHE_LINE_SIZE);

    /* get total align size */
    align_size = nbytes + align;

    /* allocate in the dma-safe heap */
    ptr = aw_memheap_alloc(&__g_dma_safe_heap, align_size);
    if (ptr != NULL) {

        /* the allocated memory block is aligned */
        if (AW_ALIGNED((uint32_t)ptr, align)) {
            aligned_ptr = (void *)((uint32_t)ptr + align);
        } else {
            aligned_ptr = (void *)(AW_ROUND_UP((uint32_t)ptr, align));
        }

        /* set the pointer before alignment pointer to the real pointer */
        *((uint32_t *)((uint32_t)aligned_ptr - sizeof(void *))) = (uint32_t)ptr;

        ptr = aligned_ptr;
    }

    return ptr;
#endif

}

/******************************************************************************/
void *aw_cache_dma_malloc (size_t nbytes)
{
    return aw_cache_dma_align(nbytes, AW_CACHE_LINE_SIZE);
}

/******************************************************************************/
void aw_cache_dma_free (void *ptr)
{

#ifdef  AW_SYS_HEAP_USE_SLAB

    aw_slab_free(&__g_dma_safe_heap, ptr);

#else
    void *real_ptr = NULL;

    if (ptr != NULL) {
        real_ptr = (void *)*(uint32_t *)((uint32_t)ptr - sizeof(void *));
    }
    aw_memheap_free(real_ptr);
#endif
}


/* end of file */
