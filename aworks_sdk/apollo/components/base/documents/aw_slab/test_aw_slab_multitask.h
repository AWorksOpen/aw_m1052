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
 * 使用本服务需要包含头文件 test_aw_slab_multitask.h
 *
 * \par 简单示例
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
 *  当前的测试的思路是，建立多个线程(1...n)，线程T申请 Tb 个字节的内存，
 *  然后往内存写零，休眠 Tm 毫秒，释放内存，休眠 Tm 毫秒，并输出出相关信息。
 *
 *  测试1,2,3都是基于这种思路来写，只是参数的控制方式不一样。
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

  这个测试本来是在PC上运行，从命令行传入参数的，
  但是考虑到在AWorks上不太好传参，所以建了个命令表test_aw_slab_multitask_cmd。
  
  test_aw_slab_multitask_cmd[1]控制执行申请，清零，释放的次数

  test_aw_slab_multitask_cmd[i],test_aw_slab_multitask_cmd[i+1](i是除0外的偶数)
  代表(任务申请内存大小，休眠时间)
  
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

  这个测试，建立 N个任务，申请、清零、释放的内存大小是
  
  size = Tn*s*F, Tn是该任务传入的参数，s属于[1, SIZE], F是内部常量

  举例子，假设N=555, SIZE=777, F=8：
  主任务建立第一个任务 T1， Tn传入1，该任务执行SIZE次动作，每次的内存大小是：
  1*1*8, 1*2*8, 1*3*8, 1*4*8, 1*5*8, ...., 1*777*8

  则第N个任务是
  555*1*8, 555*2*8, 555*3*8, 555*4*8, 555*5*8, ...., 555*777*8

  这样控制是为了测试内存从小到大分配。

*******************************************************************************/
#define TEST_AW_SLAB_NTASK      10     /**< \brief 任务的个数 */
#define TEST_AW_SLAB_SIZE       30     /**< \brief 分配的控制参数，也是循环次数 */
#define TEST_AW_SLAB_SLEEP_MS   0      /**< \brief 休眠的时间 */

extern 
int test_aw_slab_multitask2(int ntask, int size, int stime);

#define test_aw_slab_multitask2_void() \
        test_aw_slab_multitask2(TEST_AW_SLAB_NTASK,     \
                                TEST_AW_SLAB_SIZE,      \
                                TEST_AW_SLAB_SLEEP_MS);

/*******************************************************************************
  MultiTask 3

  这个测试时基于Test 2的，只是在建立线程前，会链接复数块内存
  用来测试 aw_slab 用于管理多块内存。

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
