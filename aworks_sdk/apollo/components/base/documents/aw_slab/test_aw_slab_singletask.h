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
 * 使用本服务需要包含头文件 test_aw_slab_singletask.h
 *
 * \par 简单示例
 * \code
 *
 *  #include "test_aw_slab_singletask.h"
 *
 *  test_aw_slab_simple();
 *
 * \endcode
 *
 *  基本覆盖了aw_slab的接口:
 *  test_aw_slab_simple():
 *      aw_slab_init();
 *      aw_slab_catenate();
 *      aw_slab_easy_catenate();
 *      aw_slab_malloc();
 *      aw_slab_free();
 *      aw_slab_memory_info();
 *
 *  test_aw_slab_alloc_api():
 *      aw_slab_malloc();
 *      aw_slab_free();
 *      aw_slab_calloc();
 *      aw_slab_realloc();
 *
 *  test_aw_slab_sethook():
 *      aw_slab_malloc_sethook();
 *      aw_slab_free_sethook();
 *
 * \internal
 * \par modification history:
 * - 1.00 17-03-03  imp, first implementation
 * \endinternal
 */


#ifndef __TEST_AW_SLAB_SINGLETASK_H
#define __TEST_AW_SLAB_SINGLETASK_H


#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
  Simple Test

  这个测试简单测试一下接口的功能,在单任务。

*******************************************************************************/
extern 
void test_aw_slab_simple(void);


/*******************************************************************************
  Hook Test

  这个测试简单测试一下钩子函数是否能正常设置和调用，
  观察现象需要打开test_aw_slab.h的宏__TEST_DEBUG_LOG，
  和aw_slab.h的宏AW_SLAB_USING_HOOK。

*******************************************************************************/
extern
void test_aw_slab_sethook(void);


/*******************************************************************************
  Allocation Test

  这个测试用例测试了aw_slab所提供的所有内存分配的接口，包括
  aw_slab_malloc(), aw_slab_calloc(), aw_slab_realloc()和aw_slab_free()。

*******************************************************************************/
extern
void test_aw_slab_alloc_api(void);


#ifdef __cplusplus
}
#endif

#endif
