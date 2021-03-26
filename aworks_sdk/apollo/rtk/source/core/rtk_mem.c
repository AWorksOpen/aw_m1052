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
 * \file  rtk_mem.c
 * \brief rtk is light weigh and powerfull real time scheduler.
 *
 * \internal
 * \par modification history:
 * - 2.02.1110 18-04-16  sni, first implention
 * \endinternal
 */
#include "rtk.h"
#include "rtk_mem.h"
#include "aw_memheap.h"

#if CONFIG_RTK_OBJECT_CREATE_EN && CONFIG_RTK_USERMODE
static struct aw_memheap __g_sys_heap;          /* system heap object */
static struct aw_memheap __g_usr_heap;          /* system heap object */

void *aw_bsp_get_sys_heap_start(void);
void *aw_bsp_get_sys_heap_end(void);

void *aw_bsp_get_usr_heap_start(void);
void *aw_bsp_get_usr_heap_end(void);

void rtk_mem_init(void)
{
    void               *heap_start;
    void               *heap_end ;
    /* 初始化系统堆 */
    heap_start = aw_bsp_get_sys_heap_start();
    heap_end = aw_bsp_get_sys_heap_end();

    aw_memheap_init(&__g_sys_heap,
                    "rtk_sys_head",
                    (void *)heap_start,
                    (uint32_t)heap_end - (uint32_t)heap_start);

    /* 初始化用户空间堆 */
    heap_start = aw_bsp_get_usr_heap_start();
    heap_end = aw_bsp_get_usr_heap_end();

    aw_memheap_init(&__g_usr_heap,
                    "rtk_usr_head",
                    (void *)heap_start,
                    (uint32_t)heap_end - (uint32_t)heap_start);

}

/******************************************************************************/
static void * __rtk_mem_align(struct aw_memheap *p_heap,size_t size, size_t align)
{
    void    *align_ptr;
    void    *ptr;
    size_t   align_size;

    /* check align is pow of 2 */
    if ((align & (align -1)) != 0 ) {
        return NULL;
    }

    /* align the alignment size to 4 byte */
    align = AW_ROUND_UP(align, sizeof(void *));

    /* get total aligned size */
    align_size =  AW_ROUND_UP(size, sizeof(void *)) + align + sizeof(void *);

    /* allocate memory block from heap */
    ptr = aw_memheap_alloc(p_heap, align_size);
    if (ptr != NULL) {

        /* round up ptr to align */
        align_ptr = (void *)AW_ROUND_UP((uint32_t)ptr + sizeof(void *), align);

        /* set the pointer before alignment pointer to the real pointer */
        *((uint32_t *)((uint32_t)align_ptr - sizeof(void *))) = (uint32_t)ptr;

        ptr = align_ptr;
    }

    return ptr;
}

/******************************************************************************/
void *rtk_sys_mem_align(size_t size, size_t align)
{
    return __rtk_mem_align(&__g_sys_heap,size,align);
}

/******************************************************************************/
void *rtk_sys_mem_alloc(size_t size)
{
    /* allocate in the system heap */
    return rtk_sys_mem_align(size, 4);
}


/******************************************************************************/
void rtk_sys_mem_free(void *ptr)
{
    void *real_ptr = NULL;

    if (ptr != NULL) {
        real_ptr = (void *)*(uint32_t *)((uint32_t)ptr - sizeof(void *));
    }
    aw_memheap_free(real_ptr);
}


void *rtk_usr_mem_align(size_t size, size_t align)
{
    return __rtk_mem_align(&__g_usr_heap,size,align);
}

/******************************************************************************/
void *rtk_usr_mem_alloc(size_t size)
{
    /* allocate in the system heap */
    return rtk_usr_mem_align(size, 4);
}


/******************************************************************************/
void rtk_usr_mem_free(void *ptr)
{
    rtk_sys_mem_free(ptr);
}

size_t rtk_sys_mem_get_free_size(void)
{
    return __g_sys_heap.available_size;
}

size_t rtk_usr_mem_get_free_size(void)
{
    return __g_usr_heap.available_size;
}

#elif CONFIG_RTK_OBJECT_CREATE_EN

void rtk_mem_init(void)
{

}

#else
void rtk_mem_init(void)
{

}
#endif
