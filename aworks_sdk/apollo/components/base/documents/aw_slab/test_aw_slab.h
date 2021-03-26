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
 * ʹ�ñ�������Ҫ����ͷ�ļ� test_aw_slab.h
 *
 * \par ��ʾ��
 * \code
 * 
 *  #include "apollo.h"
 *  #include "aw_mem.h"
 *  #include "test_aw_slab.h"
 *
 *  struct aw_slab foo;
 *    
 *  //��ʼ��aw_slab
 *  aw_slab_init(&foo);
 *
 *  //��ӡaw_slab�ṩ���ڴ�ʹ�����,��Ҫ����AW_SLAB_MEM_STATS                   
 *  test_memory_info(&foo);                 
 *
 *  //����һ���ڴ棬��aw_slab��������16KB����
 *  void *p_trunk = aw_mem_alloc(HEAP_SZ);
 *  test_easy_catenate(&foo, p_trunk, HEAP_SZ);
 *
 *  //��aw_slab����һ���ڴ棬��ӡ�ڴ�ʹ�����
 *  void *p_new = test_malloc(&foo, NEW_SZ);
 *  test_memory_info(&foo);  
 *
 *  //�ͷŸղ�������ڴ棬��ӡ�ڴ����
 *  test_free(&foo, p_new);
 *  test_memory_info(&foo);   
 *
 *  //�˺������ڲ���������ڴ�����������
 *  test_malloc_link(&foo, 200);
 *  test_malloc_link(&foo, 2000);
 *
 *  //�ͷ���test_malloc_link()���������ڴ�
 *  test_free_link(&foo);
 *
 *  //����trunk����ֹ�ڴ�й©
 *  aw_mem_free(p_trunk);
 *
 * \endcode
 *
 *  ������Ҫ�Ƿ�װaw_slab.h�ṩ�Ľӿڣ�����д���Դ���ʱ����������Ľӿڣ�
 *  ������ֱ�ӵ���ԭ�ӿڣ����Ͳ��Դ���Զ��������ԣ���������ʵ����չ��
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


