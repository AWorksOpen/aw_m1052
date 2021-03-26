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
 * \brief Test package of aw_slab
 *
 * \internal
 * \par modification history:
 * - 1.00 17-03-03  imp, first implementation
 * \endinternal
 */

#include <string.h>

#include "apollo.h"
#include "aw_vdebug.h"
#include "aw_assert.h"
#include "aw_mem.h" 
#include "aw_slab.h"

#include "test_aw_slab.h"


/******************************************************************************/
#define __TEST_DEBUG     0
#define __TEST_HEAD      0

/******************************************************************************/
struct __typ{
    void *p;
    struct __typ *next;

}__typ = {NULL, NULL};

uint32_t cnt = 0;

/******************************************************************************/
void  test_heap_catenate(struct aw_slab *p, 
                         void           *begin_addr, 
                         void           *end_addr,
                         uint8_t         page_bits,
                         uint8_t         thresh,
                         uint32_t        zone_min_sz,
                         uint32_t        zone_max_sz,
                         uint32_t        zone_max_limit)
{
    __TEST_RUN(__TEST_HEAD);

    aw_slab_heap_catenate  (p, 
                            begin_addr, 
                            end_addr, 
                            page_bits, 
                            thresh, 
                            zone_min_sz, 
                            zone_max_sz, 
                            zone_max_limit);

    __TEST_END(__TEST_HEAD);
}

/******************************************************************************/
void  test_easy_catenate(struct aw_slab *p, void *heap, size_t sz)
{
    __TEST_RUN(__TEST_HEAD);

    aw_slab_easy_catenate(p, heap, sz);

    __TEST_END(__TEST_HEAD);
}

/******************************************************************************/
void test_malloc_link(struct aw_slab *p, size_t sz)
{
    __TEST_RUN(__TEST_HEAD);

    struct __typ *n = aw_mem_alloc(sizeof(*n));

    if(!n){
        aw_assert(0);
        return;
    }
    cnt++;

    n->p = test_malloc(p, sz);

    if(!n->p){
    	aw_assert(0);
        return;
    }

    n->next = __typ.next;
    __typ.next = n;

    __TEST_END(__TEST_HEAD);
}

/******************************************************************************/
void test_free_link(struct aw_slab *p)
{
    __TEST_RUN(__TEST_HEAD);

    struct __typ *n;
    
    n = __typ.next;

    while(n){

        test_free(p, n->p);

        __typ.next = n->next;

        aw_mem_free(n);
        cnt--;

        n = __typ.next;
    }
    __TEST_END(__TEST_HEAD);
}

/******************************************************************************/
void *test_malloc(struct aw_slab *p, size_t n)
{
    __TEST_RUN(__TEST_HEAD);

    char *ret = aw_slab_malloc(p, n);

    if(!ret){

        __TEST_DEBUG_LOG(__TEST_DEBUG,
                        ("Failed\n"));
        return NULL;
    }

    __TEST_DEBUG_LOG(__TEST_DEBUG,
                    ("Addr 0x%x\tSize 0x%x\n", ret, n));

    __TEST_END(__TEST_HEAD);

    return ret;
}

/******************************************************************************/
void *test_calloc(struct aw_slab *p, size_t count, size_t n)
{
    __TEST_RUN(__TEST_HEAD);

    char *ret = aw_slab_calloc(p, count, n);

    if(!ret){

        __TEST_DEBUG_LOG(__TEST_DEBUG,
                        ("[Failed\n"));
        return NULL;
    }

    __TEST_DEBUG_LOG(__TEST_DEBUG,
                    ("Addr 0x%x\tSize 0x%x\n", ret, n));

    __TEST_END(__TEST_HEAD);

    return ret;
}

/******************************************************************************/
void *test_realloc(struct aw_slab *p, void *ptr, size_t size)
{
    __TEST_RUN(__TEST_HEAD);


    char *ret = aw_slab_realloc(p, ptr, size);

    if(!ret){

        __TEST_DEBUG_LOG(__TEST_DEBUG,
                        ("Failed\n"));
        return NULL;
    }

    __TEST_DEBUG_LOG(__TEST_DEBUG,
                    ("Addr 0x%x\tSize 0x%x\n", ret, size));

    __TEST_END(__TEST_HEAD);

    return ret;
}

/******************************************************************************/
void test_free(struct aw_slab *p, void *f)
{
    __TEST_RUN(__TEST_HEAD);

    aw_slab_free(p, f);

    __TEST_DEBUG_LOG(__TEST_DEBUG,
                    ("Addr 0x%x\n", f));

    __TEST_END(__TEST_HEAD);
}

/******************************************************************************/
void test_memory_info(struct aw_slab *p)
{
    __TEST_RUN(__TEST_HEAD);

    #ifdef AW_SLAB_MEM_STATS

    uint32_t total, used, max_used;

    aw_slab_memory_info(p, &total, &used, &max_used);

    __TEST_DEBUG_LOG(1, 
                ("total\t\tused\t\tmax_used\n"));

    __TEST_DEBUG_LOG(1, 
                ("%u\t\t%u\t\t%u\n", total, used, max_used));

    #endif

    __TEST_END(__TEST_HEAD);
}


#undef  __TEST_DEBUG     
#undef  __TEST_HEAD      
