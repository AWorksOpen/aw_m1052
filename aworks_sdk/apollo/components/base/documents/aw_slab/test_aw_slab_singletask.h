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
 * ʹ�ñ�������Ҫ����ͷ�ļ� test_aw_slab_singletask.h
 *
 * \par ��ʾ��
 * \code
 *
 *  #include "test_aw_slab_singletask.h"
 *
 *  test_aw_slab_simple();
 *
 * \endcode
 *
 *  ����������aw_slab�Ľӿ�:
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

  ������Լ򵥲���һ�½ӿڵĹ���,�ڵ�����

*******************************************************************************/
extern 
void test_aw_slab_simple(void);


/*******************************************************************************
  Hook Test

  ������Լ򵥲���һ�¹��Ӻ����Ƿ����������ú͵��ã�
  �۲�������Ҫ��test_aw_slab.h�ĺ�__TEST_DEBUG_LOG��
  ��aw_slab.h�ĺ�AW_SLAB_USING_HOOK��

*******************************************************************************/
extern
void test_aw_slab_sethook(void);


/*******************************************************************************
  Allocation Test

  �����������������aw_slab���ṩ�������ڴ����Ľӿڣ�����
  aw_slab_malloc(), aw_slab_calloc(), aw_slab_realloc()��aw_slab_free()��

*******************************************************************************/
extern
void test_aw_slab_alloc_api(void);


#ifdef __cplusplus
}
#endif

#endif
