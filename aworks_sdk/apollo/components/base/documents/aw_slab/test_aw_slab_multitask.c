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
 * \internal
 * \par modification history:
 * - 1.00 17-03-03  imp, first implementation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "apollo.h"
#include "aw_assert.h"
#include "aw_vdebug.h"
#include "aw_mem.h"
#include "aw_task.h"

#include "test_aw_slab.h"

/*******************************************************************************
* marco
*
* When porting to POSIX,  open the  __USE_POSIX
* When porting to AWorks, close the __USE_POSIX
*
*******************************************************************************/
//#define __USE_POSIX

#ifdef __USE_POSIX
/* When POSIX  */
#define __TASK_ARRAY_DECL_STATIC(task, stack_size, n) \
    static pthread_t task[n]

#else
/* When Apollo  */
#define _TASK_ARRAY_DECL_STATIC_(task, stack_size, n) \
static struct __tasknfo_##task {               \
    struct rtk_task tcb;                 \
    char stack[stack_size];              \
}task[n]

#define __TASK_ARRAY_DECL_STATIC(task, stack_size, n) \
    _TASK_ARRAY_DECL_STATIC_(task, stack_size, n) 
#endif

/*******************************************************************************
  Inner marco
*******************************************************************************/
#define __TEST_DEBUG     1      /**< \brief open the debug log */
#define __TEST_HEAD      1      /**< \brief open the tips  statement */

#define __TASK_SIZE     8196    /**< \brief 任务的栈大小 */ 
#define __TASK_MAX      20      /**< \brief 最大允许建立的任务个数 */ 

#define __HEAP_SZ      (1<<23)  /**< \brief 测试1,2的堆大小,8MB   */
#define __FACTOR        20      /**< \brief 测试2,3的分配因子 */ 

/*******************************************************************************
  inner struct
*******************************************************************************/
/**
 * \brief 传入每个任务的参数,用链表来链接，便于释放占用内存
 */ 
struct test{
    int ctrl;
    int times;
    struct test *next;
}*__gp_head;

/**
 * \brief 所有测试共享的内容
 *        测试1用来传命令行的参数
 *        测试2,3, args用来传休眠时间，argv为NULL
 */ 
struct param{
    int              args;
    char            **argv;
    struct aw_slab  *p_slab;
}__g_parm;


/**
 * \brief 在测试3中用来链接申请的多块内存的地址；
 *        在测试结束后释放，防止内存泄漏
 */ 
struct new {
    void          *ptr;
    struct new    *p_next;
}*__gp_new;

/**
 * \brief 测试1的任务
 */ 
static void *__f1(void *arg);
/**
 * \brief 测试2,3的任务
 */ 
static void *__f2(void *arg);
/**
 * \brief 测试2,3的任务
 */ 
static void *__f3(void *arg);

/**
 * \brief 声明任务数组，因为Apollo没有这个，只能在这里实现一个内部的
 */ 
__TASK_ARRAY_DECL_STATIC(task, __TASK_SIZE, __TASK_MAX);


/*******************************************************************************
  MultiTask Test1
*******************************************************************************/
void *__f1(void *arg)
{
    struct test *p = (struct test*)arg;

    unsigned int sleep = atoi(__g_parm.argv[(*p).ctrl + 1]);
    int block = atoi(__g_parm.argv[(*p).ctrl]);

    void *new = NULL;

    while((*p).times--){
    
        new = test_malloc(__g_parm.p_slab, block);

        aw_assert(new!= NULL);
        memset(new, 0, block);

        __TEST_DEBUG_LOG(__TEST_DEBUG, ("[TASK  %x\t]: malloc %d \t\t\t\t%d \n",
                                       aw_task_id_self(), block, (*p).times));

        if(sleep != 0){
            aw_task_delay(aw_ms_to_ticks(sleep));
        }

        test_free(__g_parm.p_slab, new);

        __TEST_DEBUG_LOG(__TEST_DEBUG,
                        ("[TASK  %x\t]: \t\t\t\tfree %d \t\t%d \n",
                                    aw_task_id_self(), block, (*p).times));

        if(sleep != 0){
             aw_task_delay(aw_ms_to_ticks(sleep));
        }

        fflush(stdout);
    }
    return NULL;
}


/******************************************************************************/
int test_aw_slab_multitask1(int args, char  *argv[])
{

    __TEST_RUN(__TEST_HEAD);

    if(args < 2 || args %2 != 0){

        aw_kprintf("Usage: ./app  Times malloc[1] Sleep[1]....to N.max= %d\n", 
                    __TASK_MAX);
        return -1;
    }

    struct aw_slab foo;
    aw_slab_init(&foo);
    test_memory_info(&foo);

    char *heap = aw_mem_alloc(__HEAP_SZ);
    test_easy_catenate(&foo, heap, __HEAP_SZ);  

    test_memory_info(&foo);

    aw_kprintf("\t\t\tHere has %d memory \n", __HEAP_SZ);

    __g_parm.args = args;
    __g_parm.argv = argv;
    __g_parm.p_slab = &foo;

    int i=0;
    size_t ctrl = 2;

    __gp_head = NULL;

    while(ctrl < args && i < __TASK_MAX){

        struct test *new = aw_mem_alloc(sizeof(*new));

        new->ctrl = ctrl;
        new->times= atoi(argv[1]);
        new->next = __gp_head;

        __gp_head = new;

        AW_TASK_INIT(task[i], "task", i+20, __TASK_SIZE, __f1, (void*)new);

        ctrl += 2;
        ++i;
    };

    int k;
    for(k = 0; k <i; ++k){
        AW_TASK_STARTUP(task[k]);
    }

    aw_task_delay(aw_sys_clkrate_get() * 10);

    while(i > 0){
        AW_TASK_TERMINATE(task[i-1]);
        --i;
    };

    struct test *tmp;

    while(__gp_head != NULL){
        tmp = __gp_head;
        __gp_head = tmp->next;
        aw_mem_free(tmp);
    }

    aw_mem_free(heap);

    __TEST_END(__TEST_HEAD);

    return 0;
}


/*******************************************************************************
*  MultiTask Test2
*******************************************************************************/
void *__f2(void *arg)
{
    struct test *p = (struct test*)arg;
    unsigned int sleep  = __g_parm.args;

    int factor = (*p).ctrl;

    void *new = NULL;

    int times=0;

    while(times++ < (*p).times){

        int s = factor * times* __FACTOR;
    
        new = test_malloc(__g_parm.p_slab, s);

        aw_assert(new!= NULL);
        memset(new, 0, s);

        __TEST_DEBUG_LOG(__TEST_DEBUG,
                        ("[TASK  %x\t]: malloc %d \t\t\t\t%d \n",
                                    aw_task_id_self(), s, times));

        if(sleep != 0){
            aw_task_delay(aw_ms_to_ticks(sleep));
        }

        test_free(__g_parm.p_slab, new);

        __TEST_DEBUG_LOG(__TEST_DEBUG,
                        ("[TASK  %x\t]: \t\t\t\tfree %d \t\t%d \n",
                                    aw_task_id_self(), s, times));

        if(sleep != 0){
             aw_task_delay(aw_ms_to_ticks(sleep));
        }
        
        fflush(stdout);
    }
    return NULL;
}

/******************************************************************************/
void *__f3(void *arg)
{
    struct test *p = (struct test*)arg;
    unsigned int sleep  = __g_parm.args;

    int factor = (*p).ctrl;

    void *new = NULL;

   int times  = (*p).times;

    while(times-- > 0){

        int s = factor * times* __FACTOR;
    
        new = test_malloc(__g_parm.p_slab, s);

        aw_assert(new!= NULL);
        memset(new, 0, s);

        __TEST_DEBUG_LOG(__TEST_DEBUG,
                        ("[TASK  %x\t]: malloc %d \t\t\t\t%d \n",
                                    aw_task_id_self(), s, times));

        if(sleep != 0){
            aw_task_delay(aw_ms_to_ticks(sleep));
        }

        test_free(__g_parm.p_slab, new);

        __TEST_DEBUG_LOG(__TEST_DEBUG,
                        ("[TASK  %x\t]: \t\t\t\tfree %d \t\t%d \n",
                                    aw_task_id_self(), s, times));

        if(sleep != 0){
             aw_task_delay(aw_ms_to_ticks(sleep));
        }
        
        fflush(stdout);
    }
    return NULL;
}

/******************************************************************************/
int test_aw_slab_multitask2(int nthread, int size, int stime)
{

    __TEST_RUN(__TEST_HEAD);

    if(nthread < 0 || stime < 0 || size < 0){

        aw_kprintf("Usage: ./app thread_number size sleep_times \n");
        return -1;
    }

    struct aw_slab foo;

    aw_slab_init(&foo);
    test_memory_info(&foo);

    char *heap = aw_mem_alloc(__HEAP_SZ);
    test_easy_catenate(&foo, heap, __HEAP_SZ);  

    test_memory_info(&foo);

    __g_parm.args = stime;
    __g_parm.argv = NULL;
    __g_parm.p_slab = &foo;

    int i    = 0;
    int ctrl = 0;

    __gp_head = NULL;

    while(ctrl < nthread && i < __TASK_MAX){

        struct test *new = aw_mem_alloc(sizeof(*new));

        new->ctrl = ctrl + 1;
        new->times= size;
        new->next = __gp_head;

        __gp_head = new;

        AW_TASK_INIT(task[i], "task", i+20, __TASK_SIZE, __f2, (void*)new);

        ++ctrl;
        ++i;
    };

    int k;
    for(k = 0; k <i; ++k){
        AW_TASK_STARTUP(task[k]);
    }

    aw_task_delay(aw_sys_clkrate_get() * 10);

    while(i > 0){

       AW_TASK_TERMINATE(task[i-1]);
        --i;
    };

    struct test *tmp;
    
    while(__gp_head != NULL){
        tmp = __gp_head;
        __gp_head = tmp->next;
        aw_mem_free(tmp);
    }

    aw_mem_free(heap);

    __TEST_END(__TEST_HEAD);

    return 0;
}


/*******************************************************************************
*  MultiTask Test3
*******************************************************************************/

#define __FIRST_SZ   (1<<14)    /* 第一块链接的内存大小  */
#define __DIRECT     <<         /* 内存增长方向  */
#define __CAT_STEP   2          /* 内存增长梯度 */
#define __CAT_BLOCKS 5          /* 链接块数 */


#define __CAL_SIZE(x)   __FIRST_SZ __DIRECT (__CAT_STEP*(x))

#define __GET_HEAP_SIZE(cnt, size)                  \
        for(cnt=0, size=0;                          \
            cnt < __CAT_BLOCKS && (size=__CAL_SIZE(cnt),1); \
            ++cnt)

/******************************************************************************/
int test_aw_slab_multitask3(int nthread, int size, int stime)
{
    __TEST_RUN(__TEST_HEAD);

    if(nthread < 0 || stime < 0 || size < 0){
        aw_kprintf("Usage: ./app thread_number size sleep_times \n");
        return -1;
    }

    struct aw_slab foo;
    struct new *p_tmp;

    aw_slab_init(&foo);
    test_memory_info(&foo);

    char *heap = NULL;
    int i, block;

    __gp_new = NULL;

    __GET_HEAP_SIZE(i, block){

        p_tmp = aw_mem_alloc(sizeof(*p_tmp));

        heap = aw_mem_alloc(block);

        if(heap == NULL){
            aw_kprintf("NO Memory.\n");
            aw_assert(0);
        }

        /* 将内存连接起来，最后释放 */
        p_tmp->ptr = heap;
        p_tmp->p_next = __gp_new;
        __gp_new = p_tmp;
        
        /* 将内存放入aw_slab管理 */
        test_easy_catenate(&foo, heap, block);
        test_memory_info(&foo);
    }

    __g_parm.args = stime;
    __g_parm.argv = NULL;
    __g_parm.p_slab = &foo;

    int ctrl = 0;
    i = 0;
    __gp_head = NULL;

    while(ctrl < nthread && i < __TASK_MAX){

        struct test *new = aw_mem_alloc(sizeof(*new));

        new->ctrl = ctrl + 1;
        new->times= size;
        new->next = __gp_head;

        __gp_head = new;

        AW_TASK_INIT(task[i], 
                    "task", 
                    i + 20, 
                    __TASK_SIZE, 
                    __f2, 
                    (void*)new);

        ++ctrl;
        ++i;
    };

    int k;
    for(k = 0; k <i; ++k){
        AW_TASK_STARTUP(task[k]);
    }

    aw_task_delay(aw_sys_clkrate_get() * 60);

    while(i > 0){

       AW_TASK_TERMINATE(task[i-1]);
        --i;
    };

    struct test *tmp;
    
    while(__gp_head != NULL){
        tmp = __gp_head;
        __gp_head = tmp->next;

        aw_mem_free(tmp);
    }

    while(__gp_new != NULL){

        p_tmp    = __gp_new;
        __gp_new = p_tmp->p_next;

        aw_mem_free(p_tmp->ptr);
    }
    

    __TEST_END(__TEST_HEAD);

    return 0;
}


#undef  __FIRST_SZ   
#undef  __DIRECT     
#undef  __CAT_STEP   
#undef  __CAT_BLOCKS  

#undef  __TEST_DEBUG     
#undef  __TEST_HEAD      


