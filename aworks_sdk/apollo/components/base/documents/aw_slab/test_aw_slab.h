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
 * 使用本服务需要包含头文件 test_aw_slab.h
 *
 * \par 简单示例
 * \code
 * 
 *  #include "apollo.h"
 *  #include "aw_mem.h"
 *  #include "test_aw_slab.h"
 *
 *  struct aw_slab foo;
 *    
 *  //初始化aw_slab
 *  aw_slab_init(&foo);
 *
 *  //打印aw_slab提供的内存使用情况,需要开启AW_SLAB_MEM_STATS                   
 *  test_memory_info(&foo);                 
 *
 *  //分配一块内存，给aw_slab管理，建议16KB以上
 *  void *p_trunk = aw_mem_alloc(HEAP_SZ);
 *  test_easy_catenate(&foo, p_trunk, HEAP_SZ);
 *
 *  //从aw_slab申请一块内存，打印内存使用情况
 *  void *p_new = test_malloc(&foo, NEW_SZ);
 *  test_memory_info(&foo);  
 *
 *  //释放刚才申请的内存，打印内存情况
 *  test_free(&foo, p_new);
 *  test_memory_info(&foo);   
 *
 *  //此函数在内部将分配的内存链起来保存
 *  test_malloc_link(&foo, 200);
 *  test_malloc_link(&foo, 2000);
 *
 *  //释放用test_malloc_link()链起来的内存
 *  test_free_link(&foo);
 *
 *  //回收trunk，防止内存泄漏
 *  aw_mem_free(p_trunk);
 *
 * \endcode
 *
 *  这里主要是封装aw_slab.h提供的接口，建议写测试代码时，调用这里的接口，
 *  而不是直接调用原接口，减低测试代码对对象的耦合性，而且容易实现扩展。
 * 
 *
 * \internal
 * \par modification history:
 * - 1.00 17-03-03  imp, first implementation
 * \endinternal
 */

#include "apollo.h"
#include "aw_vdebug.h"
#include "aw_types.h"
#include "aw_slab.h"

#ifndef __TEST_AW_SLAB_H
#define __TEST_AW_SLAB_H

#ifdef __cplusplus
extern "C" {
#endif

#if 1
#define __TEST_DEBUG_LOG(type, message)    \
do                                         \
{                                          \
    if (type)                              \
        AW_LOGF(message);                  \
}                                          \
while (0)
#else
#define __TEST_DEBUG_LOG(type, message)
#endif

#define __RUN  "[RUN       ]: %s\n", __FUNCTION__
#define __END  "[       END]: %s\n", __FUNCTION__


#define __TEST_RUN(type)   __TEST_DEBUG_LOG(type, (__RUN))
#define __TEST_END(type)   __TEST_DEBUG_LOG(type, (__END))


extern
void test_malloc_link(struct aw_slab *p, size_t sz);

extern
void test_free_link(struct aw_slab *p);

extern
void *test_malloc(struct aw_slab *p, size_t n);

extern
void *test_calloc(struct aw_slab *p, size_t count, size_t size);

extern
void *test_realloc(struct aw_slab *p, void *ptr, size_t size);

extern
void test_free(struct aw_slab *p, void *f);

extern
void test_memory_info(struct aw_slab *p);

extern
void  test_easy_catenate(struct aw_slab *p, void *heap, size_t sz);

extern
void  test_heap_catenate(struct aw_slab *p, 
                         void           *begin_addr, 
                         void           *end_addr,
                         uint8_t         page_bits,
                         uint8_t         thresh,
                         uint32_t        zone_min_sz,
                         uint32_t        zone_max_sz,
                         uint32_t        zone_max_limit);

#ifdef AW_SLAB_USING_HOOK

#define test_malloc_sethook(x,y) aw_slab_malloc_sethook(x,y)
#define test_free_sethook(x,y)   aw_slab_free_sethook(x,y)

#else 

#define test_malloc_sethook(x,y) 
#define test_free_sethook(x,y)   

#endif




#ifdef __cplusplus
}
#endif


#endif      /* __TEST_AW_SLAB_H */

/* end of file */


