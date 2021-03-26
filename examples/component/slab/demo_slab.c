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
 * \brief slab内存分配器例程(避免内存碎片)
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能AW_COM_CONSOLE和调试串口使能宏；
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象:
 *   1. 打印一块动态内存的地址
 *
 * \par 源代码
 * \snippet demo_slab.c src_slab
 *
 * \internal
 * \par History
 * - 1.00 16-03-25  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_slab slab内存分配器
 * \copydoc demo_slab.c
 */

/** [src_slab] */
#include "aworks.h"
#include "aw_slab.h"
#include "aw_vdebug.h"

#include <string.h>

#define TRUNK_SIZE (100 * 1024)
#define ALLOC_SIZE 1024


/**
 * \brief slab 示例
 * \return 无
 */
void demo_slab_entry (void)
{
    static char     slab_buf[TRUNK_SIZE] = {0};
    char           *p_buf = NULL;

    struct aw_slab  foo;

    /* 初始化slab */
    aw_slab_init(&foo);

    /* 为slab分配内存空间 */
    aw_slab_easy_catenate(&foo, slab_buf, TRUNK_SIZE);
 
    /* 使用slab动态分配内存 */
    p_buf = aw_slab_malloc(&foo, ALLOC_SIZE);

    /* 使用分配的内存 */
    if (p_buf) {
        memset(p_buf, 0, ALLOC_SIZE);
        aw_kprintf("Got an addr 0x%x.\r\n", p_buf);

        /* 释放slab分配的内存 */
        aw_slab_free(&foo, p_buf);

    } else {
        aw_kprintf("Had nothing to be alloced.\r\n");
    }
}

/** [src_slab] */

/* end of file */
