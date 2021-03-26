/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.    
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief slab内存分配器例程(观察内存使用情况)
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能AW_COM_CONSOLE和调试串口使能宏；
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象:
 *   1. 打印调用一次 aw_slab_malloc() 之后，内存的使用情况.
 *
 * \par 源代码
 * \snippet demo_slab.c src_slab
 *
 * \internal
 * \par History
 * - 1.00 19-03-19  imp, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_slab
 * \copydoc demo_slab.c
 */

/** [src_slab] */
#include "aworks.h"
#include "aw_slab.h"
#include "aw_vdebug.h"

#define TRUNK_SIZE (30 * 1024)
#define ALLOC_SIZE 1024

static char  __g_trunk[TRUNK_SIZE];

/**
 * \brief slab 示例
 * \return 无
 */
void demo_mem_info_entry (void)
{
    uint32_t total;
    uint32_t used;
    uint32_t peak;
    char    *p_buf;

    struct aw_slab  foo;

    /* 初始化slab */
    aw_slab_init(&foo);

    /* 为slab分配内存空间 */
    aw_slab_easy_catenate(&foo, __g_trunk, TRUNK_SIZE);
 
    /* 使用slab动态分配内存 */
    p_buf = aw_slab_malloc(&foo, ALLOC_SIZE);

    /* 使用分配的内存 */
    if(p_buf) {
        aw_slab_memory_info(&foo, &total, &used, &peak);
        aw_kprintf("Memory Info: Total %d, Used %d, Peak %d.\r\n", 
            total, used, peak);

        /* 释放slab分配的内存 */
        aw_slab_free(&foo, p_buf);

    } else {
        aw_kprintf("Malloc failed.\r\n");
    }
}

/** [src_slab] */

/* end of file */
