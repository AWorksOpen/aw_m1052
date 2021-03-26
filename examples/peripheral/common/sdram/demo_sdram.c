/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief sdram堆内存操作例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - AW_COM_CONSOLE
 *      - 对应平台的串口宏
 *
 * - 实验现象：
 *   1. 串口打印操作信息。
 *
 * - 备注：
 *   1. 用户可在对应平台的sdram连接脚本中查看sdram的地址范围，本例程在sdram堆中分配了1M的内存，进行操作。
 *
 * \par 源代码
 * \snippet demo_sdram.c src_sdram
 *
 * \internal
 * \par Modification History
 * - 1.00 18-06-27  lqy, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_sdram SDRAM操作
 * \copydoc demo_sdram.c
 */

/** [src_sdram] */

#include "aworks.h"                   /* 此头文件必须被首先包含 */
#include "aw_vdebug.h"
#include "aw_shell.h"
#include "aw_mem.h"
#include "aw_psp_task.h"

/* 分配 1M 字节的SDRAM内存 */
#define SDRAM_SIZE                  (1 * 1024 * 1024)

/* 分配 1M 字节的SDRAM内存,4字节对齐 */
#define SDRAM_START_ADDR            aw_mem_align(SDRAM_SIZE, 4)

/* 缓存分配的sdram起始地址 */
static uint32_t *__g_sdram_start_addr;

static uint32_t test_mask = 0x0;

static void __test_sdram_write_init (void)
{
    uint32_t   *p     = (uint32_t *)SDRAM_START_ADDR;
    uint32_t    count = SDRAM_SIZE / sizeof(uint32_t);
    uint32_t    i;
    aw_tick_t   t1, t2;
    double      rate;
    uint32_t    mask;

    __g_sdram_start_addr = p;

    test_mask = ~test_mask;
    mask = test_mask;

    /* 将sdram内存中写入数据 */
    t1 = aw_sys_tick_get();
    for (i = 0; i < count; i++) {
        *p = i ^ mask;
        p++;
    }
    t2 = aw_sys_tick_get();

    /* 计算写速度 */
    rate  = SDRAM_SIZE;
    rate /= (t2 - t1);
    rate *= 1000;
    rate /= (1024 * 1024);
    aw_kprintf("%s:%.2f MB/s\n", "test_sdram_write", rate);

    return ;
}

static int __test_sdram_read_verfy (void)
{
    uint32_t  *p = __g_sdram_start_addr;
    uint32_t   count = SDRAM_SIZE / sizeof(uint32_t);
    uint32_t   i;
    int        ret;
    aw_tick_t  t1, t2;
    double     rate;
    uint32_t   mask;
    uint32_t   temp;

    ret  = 0;
    mask = test_mask;

    /* 从sdram内存中读取数据，并校验 */
    t1 = aw_sys_tick_get();
    for (i = 0; i < count; i++) {
        temp = *p;
        p++;
        if ((i ^ mask) != temp) {
            ret++;
        }
    }
    t2 = aw_sys_tick_get();

    /* 计算读数据速度 */
    rate  = SDRAM_SIZE;
    rate /= (t2 - t1);
    rate *= 1000;
    rate /= (1024 * 1024);
    aw_kprintf("%s:%.2f MB/s\n", "test_sdram_read", rate);

    return ret;
}

void demo_sdram_entry (void)
{
    aw_kprintf("demo_sdram test...\n");

    int r;
    int count = 5;

    /* 将sdram的堆内存中写入数据 */
    __test_sdram_write_init();

    count = 5;
    while (count --) {
        aw_task_delay(1000);

        /* 从sdram的堆内存中读取数据并校验 */
        r = __test_sdram_read_verfy();
        if (0 == r) {
            aw_kprintf("test sdram success\n");
        } else {
            aw_kprintf("test sdram failed:r= %d\n", r);
        }
    }

    /* 释放分配的sdram堆内存 */
    aw_mem_free(__g_sdram_start_addr);
    return ;
}

/** [src_sdram] */

/* end of file */
