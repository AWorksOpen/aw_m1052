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
 * \brief 
 *
 * \internal
 * \par History
 * - 1.00 19-07-4  xgg, first implementation
 * \endinternal
 */
 
#include <apollo.h>
#include <string.h>
#include "aw_task.h"
#include "aw_sem.h"
#include "aw_bitops.h"
#include "add_test_cmd.h"
#include <Unity/unity.h>

#if CONFIG_USE_TICKLESS_IDLE
#define TEST_STACK_SIZE     1024

AW_TASK_DECL_STATIC(_test_task1,TEST_STACK_SIZE);
AW_TASK_DECL_STATIC(_test_task2,TEST_STACK_SIZE);

static volatile int         _g_continue_test;
static volatile uint64_t    _g_test_count1;
static volatile uint64_t    _g_test_count2;
static void * _test_task_func1(void * ret)
{
    aw_task_delay(2);
    while(_g_continue_test) {
        _g_test_count1 ++;
    }

    return ret;
}

static void * _test_task_func2(void * ret)
{
    aw_task_delay(2);
    while(_g_continue_test) {
        _g_test_count2 ++;
    }
}


static void _test_task_exec(void)
{
    aw_task_id_t        cur_task_id;
    int                 cur_pri;
    uint64_t            data;


    cur_task_id = aw_task_id_self();
    cur_pri = aw_task_priority_get(cur_task_id);

    aw_task_delay(1000);

    _g_continue_test = 1;
    _g_test_count1 = 0;
    _g_test_count2 = 0;

    /* 首先启动一个测试任务*/
    AW_TASK_INIT(_test_task1,
            "_test_task1",
            cur_pri + 1,
            TEST_STACK_SIZE,
            (void *)_test_task_func1,
            ((void *)-123489));
    AW_TASK_STARTUP(_test_task1);

    /* 启动另一个任务*/
    AW_TASK_INIT(_test_task2,
            "_test_task2",
            cur_pri + 1,
            TEST_STACK_SIZE,
            (void *)_test_task_func2,
            ((void *)-123489));
    AW_TASK_STARTUP(_test_task2);

    aw_task_delay(1000);
    /* 结束掉_test_task2*/
    _g_continue_test = 0;
    AW_TASK_JOIN(_test_task1,NULL);
    AW_TASK_JOIN(_test_task2,NULL);

    TEST_ASSERT(_g_test_count1 > 0);
    TEST_ASSERT(_g_test_count2 > 0);

    if (_g_test_count2 < _g_test_count1) {
        data = _g_test_count1 - _g_test_count2;
    }
    else {
        data = _g_test_count2 - _g_test_count1;
    }

    TEST_ASSERT(data < 600000);

}

static void _test_task_delay(void)
{
    aw_tick_t       tick1,tick2;

    tick1 = aw_sys_tick_get();
    aw_task_delay(1000);
    tick2 = aw_sys_tick_get() - tick1;

    TEST_ASSERT(1000 == tick2|| 1001 == tick2);
}

#define DWT_CYCCNT_REG      ((volatile uint32_t *)0xE0001004)
#define DWT_CTL_REG         ((volatile uint32_t *)0xE0001000)
#define CPU_FREQ        600000000

static void out_put_uint64_t(uint64_t t)
{
    char            sz[32] = {0};
    int             count = 0;

    do {
        sz[count] = (uint32_t)(t % 10) + '0';
        t = t/10;
        count ++;
    } while(t != 0);

    count --;
    while(count >= 0 ) {
        char        t[2] = {0};
        t [0] = sz[count];
        aw_kprintf(t);
        count --;
    }

}

extern volatile uint32_t g_supress_ticks_count;

static void test_tickless_drift(void)
{
    volatile uint64_t       cyccnt_old,cyccnt_now,t64,diff;
    volatile uint32_t       cyccnt_old32,t;
    volatile uint32_t       tick_old,cyccnt_overflow_count = 0;
    volatile uint32_t       tick_now;
    volatile uint32_t       i;
    volatile uint64_t       first_time = 0;
    volatile uint32_t       old_supress_ticks_count,now_supress_ticks_count;

    /* 启动CYCCNT寄存器 */
    AW_REG_BIT_SET32(DWT_CTL_REG,0);
    aw_task_delay(1);
    tick_old = aw_sys_tick_get();
    cyccnt_old32 = *DWT_CYCCNT_REG;
    old_supress_ticks_count =g_supress_ticks_count;
    cyccnt_old = cyccnt_old32;

    while(1) {
        aw_task_delay(1000);
        tick_now = aw_sys_tick_get();
        t = *DWT_CYCCNT_REG;
        now_supress_ticks_count = g_supress_ticks_count;
        cyccnt_now = t;
        if (t < cyccnt_old32) {
            cyccnt_overflow_count ++;
        }
        i = cyccnt_overflow_count;
        while( i > 0) {
            cyccnt_now += 0xFFFFFFFF;
            cyccnt_now += 1;
            i --;
        }
        cyccnt_old32 = t;

        /* 比较差异*/
        t64 = cyccnt_now - cyccnt_old;
        if (first_time == 0) {
            first_time = t64;
            old_supress_ticks_count =g_supress_ticks_count;
        }

        diff = t64/(CPU_FREQ/1000/1000);
        cyccnt_now = (tick_now - tick_old);
        cyccnt_now *= 1000;

        if (diff > cyccnt_now) {
            diff = diff - cyccnt_now;
            t = (uint32_t)diff;
        }
        else {
            diff = cyccnt_now - diff;
            t = - (uint32_t)diff;
        }

        if (diff >= 1000) {
            aw_kprintf("after %d ticks,the shift is %d\n",
                    (tick_now - tick_old),t);
            aw_kprintf("(");
            out_put_uint64_t(t64);
            aw_kprintf(",");
            out_put_uint64_t(first_time);
            aw_kprintf(")\n");
            aw_kprintf("supress:%u,%d\n",now_supress_ticks_count,old_supress_ticks_count);

        }
        TEST_ASSERT(diff < 1000);


    }
}

#else



#endif

void rtk_enable_tickless_idle(int enable);

static void test_task_tickless()
{
#if CONFIG_USE_TICKLESS_IDLE
    rtk_enable_tickless_idle(1);
    UnityBegin(__FILE__);
    /* RUN_TEST(_test_task_delay);*/
    /* RUN_TEST(_test_task_exec);*/
    RUN_TEST(test_tickless_drift);
    UnityEnd();
    rtk_enable_tickless_idle(0);
#endif

}

ADD_TEST_CMD(test_task_tickless);
