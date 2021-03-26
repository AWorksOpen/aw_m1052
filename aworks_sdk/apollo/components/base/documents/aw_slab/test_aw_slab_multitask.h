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
 * \brief MultiTask test of aw_slab
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ� test_aw_slab_multitask.h
 *
 * \par ��ʾ��
 * \code
 *
 *  #include "test_aw_slab_multitask.h"
 *
 *  test_aw_slab_multitask1_void();
 *  test_aw_slab_multitask2_void();
 *  test_aw_slab_multitask3_void();
 *
 * \endcode
 *
 *  ��ǰ�Ĳ��Ե�˼·�ǣ���������߳�(1...n)���߳�T���� Tb ���ֽڵ��ڴ棬
 *  Ȼ�����ڴ�д�㣬���� Tm ���룬�ͷ��ڴ棬���� Tm ���룬������������Ϣ��
 *
 *  ����1,2,3���ǻ�������˼·��д��ֻ�ǲ����Ŀ��Ʒ�ʽ��һ����
 *
 * \internal
 * \par modification history:
 * - 1.00 17-03-03  imp, first implementation
 * \endinternal
 */


#ifndef __TEST_AW_SLAB_MULTITASK_H
#define __TEST_AW_SLAB_MULTITASK_H

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************

  MultiTask Test1

  ������Ա�������PC�����У��������д�������ģ�
  ���ǿ��ǵ���AWorks�ϲ�̫�ô��Σ����Խ��˸������test_aw_slab_multitask_cmd��
  
  test_aw_slab_multitask_cmd[1]����ִ�����룬���㣬�ͷŵĴ���

  test_aw_slab_multitask_cmd[i],test_aw_slab_multitask_cmd[i+1](i�ǳ�0���ż��)
  ����(���������ڴ��С������ʱ��)
  
*******************************************************************************/
static
char *test_aw_slab_multitask_cmd[] = {
    "./App",    "50",
    "500",      "0",
    "5500",     "0",
    "50",       "0",
    "58600",    "0",
    "2000",     "0",
    "5",        "0",
    "202",      "0",
    "123456",   "0",
    "6542",     "0",
    "120",      "0",
};

extern
int test_aw_slab_multitask1(int args, char  *argv[]);

#define test_aw_slab_multitask1_void()  \
        test_aw_slab_multitask1(sizeof(test_aw_slab_multitask_cmd)/     \
                                sizeof(test_aw_slab_multitask_cmd[0]),  \
                                test_aw_slab_multitask_cmd)


/*******************************************************************************
  MultiTask 2

  ������ԣ����� N���������롢���㡢�ͷŵ��ڴ��С��
  
  size = Tn*s*F, Tn�Ǹ�������Ĳ�����s����[1, SIZE], F���ڲ�����

  �����ӣ�����N=555, SIZE=777, F=8��
  ����������һ������ T1�� Tn����1��������ִ��SIZE�ζ�����ÿ�ε��ڴ��С�ǣ�
  1*1*8, 1*2*8, 1*3*8, 1*4*8, 1*5*8, ...., 1*777*8

  ���N��������
  555*1*8, 555*2*8, 555*3*8, 555*4*8, 555*5*8, ...., 555*777*8

  ����������Ϊ�˲����ڴ��С������䡣

*******************************************************************************/
#define TEST_AW_SLAB_NTASK      10     /**< \brief ����ĸ��� */
#define TEST_AW_SLAB_SIZE       30     /**< \brief ����Ŀ��Ʋ�����Ҳ��ѭ������ */
#define TEST_AW_SLAB_SLEEP_MS   0      /**< \brief ���ߵ�ʱ�� */

extern 
int test_aw_slab_multitask2(int ntask, int size, int stime);

#define test_aw_slab_multitask2_void() \
        test_aw_slab_multitask2(TEST_AW_SLAB_NTASK,     \
                                TEST_AW_SLAB_SIZE,      \
                                TEST_AW_SLAB_SLEEP_MS);

/*******************************************************************************
  MultiTask 3

  �������ʱ����Test 2�ģ�ֻ���ڽ����߳�ǰ�������Ӹ������ڴ�
  �������� aw_slab ���ڹ������ڴ档

*******************************************************************************/
extern 
int test_aw_slab_multitask3(int ntask, int size, int stime);

#define test_aw_slab_multitask3_void() \
        test_aw_slab_multitask3(TEST_AW_SLAB_NTASK,  \
                                TEST_AW_SLAB_SIZE,   \
                                TEST_AW_SLAB_SLEEP_MS);


#ifdef __cplusplus
}
#endif

#endif
