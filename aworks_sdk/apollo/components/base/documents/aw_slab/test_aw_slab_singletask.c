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
 * \brief SingleTask test of aw_slab
 *
 * \internal
 * \par modification history:
 * - 1.00 17-03-03  imp, first implementation
 * \endinternal
 */


#include "apollo.h"
#include "aw_slab.h"
#include "aw_mem.h"

#include "test_aw_slab.h"


/******************************************************************************/    
#define __TEST_HEAD      1

#define __HEAP1_SZ      (1<<14)
#define __HEAP2_SZ      (1<<18)

/******************************************************************************/
void test_aw_slab_simple(void)
{
    __TEST_RUN(__TEST_HEAD);

    struct aw_slab foo;

    aw_slab_init(&foo);

    char *heap1 = aw_mem_alloc(__HEAP1_SZ);
    char *heap2 = aw_mem_alloc(__HEAP2_SZ);

    test_memory_info(&foo);

    test_heap_catenate(&foo, heap1, heap1 + __HEAP1_SZ, 5,2, 32*2*4, 32*2*8, 32*2);
    test_memory_info(&foo);

    test_malloc_link(&foo, 1<<0);
    test_malloc_link(&foo, 1<<1);
    test_malloc_link(&foo, 1<<2);
    test_malloc_link(&foo, 1<<3);
    test_malloc_link(&foo, 1<<4);
    test_malloc_link(&foo, 1<<5);
    test_malloc_link(&foo, 1<<6);
    test_malloc_link(&foo, 1<<7);
    test_malloc_link(&foo, 1<<8);
    test_malloc_link(&foo, 1<<9);
    test_malloc_link(&foo, 1<<10);
    test_malloc_link(&foo, 1<<11);
    test_malloc_link(&foo, 1<<12);
    test_memory_info(&foo);

    test_easy_catenate(&foo, heap2, __HEAP2_SZ);
    test_memory_info(&foo);

    test_malloc_link(&foo, 1<<0);
    test_malloc_link(&foo, 1<<1);
    test_malloc_link(&foo, 1<<2);
    test_malloc_link(&foo, 1<<3);
    test_malloc_link(&foo, 1<<4);
    test_malloc_link(&foo, 1<<5);
    test_malloc_link(&foo, 1<<6);
    test_malloc_link(&foo, 1<<7);
    test_malloc_link(&foo, 1<<8);
    test_malloc_link(&foo, 1<<9);
    test_malloc_link(&foo, 1<<10);
    test_malloc_link(&foo, 1<<11);
    test_malloc_link(&foo, 1<<12);
    test_malloc_link(&foo, 1<<13);
    test_malloc_link(&foo, 1<<14);
    test_malloc_link(&foo, 1<<15);
    test_malloc_link(&foo, 1<<16);
    test_memory_info(&foo);

    test_free_link(&foo);
    test_memory_info(&foo);

    aw_mem_free(heap1);
    aw_mem_free(heap2);

    __TEST_END(__TEST_HEAD);

    return;
}


/******************************************************************************/
static
void  __mhook(void *ptr, size_t size)
{
    __TEST_RUN(__TEST_HEAD);
    __TEST_END(__TEST_HEAD);
}

static
void  __fhook(void *ptr)
{
    __TEST_RUN(__TEST_HEAD);
    __TEST_END(__TEST_HEAD);
}

void test_aw_slab_sethook(void)
{
    __TEST_RUN(__TEST_HEAD);

    struct aw_slab foo;

    aw_slab_init(&foo);

    char *heap1 = aw_mem_alloc(__HEAP2_SZ);
    test_memory_info(&foo);

    test_easy_catenate(&foo, heap1, __HEAP2_SZ);
    test_memory_info(&foo);

    test_malloc_sethook(&foo, __mhook);
    test_free_sethook(&foo, __fhook);

    void *p2 = test_malloc(&foo, 512);
    test_memory_info(&foo);

    test_free(&foo, p2);
    test_memory_info(&foo);

    aw_mem_free(heap1);

    __TEST_END(__TEST_HEAD);

    return;
}


/******************************************************************************/
void test_aw_slab_alloc_api(void)
{
    __TEST_RUN(__TEST_HEAD);

    struct aw_slab foo;

    aw_slab_init(&foo);

    char *heap1 = aw_mem_alloc(__HEAP2_SZ);
    test_memory_info(&foo);

    test_easy_catenate(&foo, heap1, __HEAP2_SZ);
    test_memory_info(&foo);

    void *p1 = test_calloc(&foo, 10, 24);
    test_memory_info(&foo);

    test_free(&foo, p1);
    test_memory_info(&foo);

    void *p2 = test_malloc(&foo, 24);
    test_memory_info(&foo);

    p2 = test_realloc(&foo, p2, 512);
    test_memory_info(&foo);

    test_free(&foo, p2);
    test_memory_info(&foo);

    aw_mem_free(heap1);

    __TEST_END(__TEST_HEAD);

    return;
}


 
#undef  __TEST_HEAD      

