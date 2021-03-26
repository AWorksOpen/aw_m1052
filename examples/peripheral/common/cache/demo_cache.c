/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief no-cache内存管理例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - 对应平台的串口宏
 *      - AW_COM_CONSOLE
 *   2. 将板子中的DURX、DUTX通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象：
 *   1. 串口打印调试信息。
 *
 *
 * \par 源代码
 * \snippet demo_cache.c src_cache
 */

/**
 * \addtogroup demo_if_cache cache例程
 * \copydoc demo_cache.c
 */

/** [src_cache] */

#include "aworks.h"
#include "aw_cache.h"
#include "aw_mem.h"
#include "aw_vdebug.h"
#include <string.h>

/**
 * \brief cache demo
 * \return 无
 */
void demo_cache_entry (void)
{
    int   i         = 0;
    char *p_mem     = NULL;
    char *test_str  = "test cache";

    /* 分配500字节大小的 cache-safe 的缓冲区 */
    p_mem = (char *)aw_cache_dma_malloc(500);
    if (p_mem == NULL) {
        aw_kprintf("p_mem is NULL");
        return ;
    }

    memset(p_mem, '\0', 500);
    for (i = 0; i < strlen(test_str) + 1; i++) {
        writeb(*(test_str + i), p_mem + i);
    }

    AW_INFOF(("p_mem addr : %x \n", p_mem));
    AW_INFOF(("p_mem data : %s \n", p_mem));

    /* 释放 cache-safe 的缓冲区 */
    aw_cache_dma_free(p_mem);

    /* 分配指定对齐长度的 cache-safe 的缓冲区 */
    p_mem = aw_cache_dma_align(1000, 16);

    memset(p_mem, '\0', 1000);
    for (i = 0; i < strlen(test_str) + 1; i++) {
        writeb(*(test_str + i), p_mem + i);
    }

    AW_INFOF(("p_mem addr : %x \n", p_mem));
    AW_INFOF(("p_mem data : %s \n", p_mem));

    /* 释放 cache-safe 的缓冲区 */
    aw_cache_dma_free(p_mem);

    uint8_t *buf = NULL;
    buf = aw_mem_alloc(128);

    for (i = 0; i < 128; i++) {
        buf[i] = (uint8_t)i;
    }

    /* 刷新buf内存中的数据  */
    aw_cache_flush(buf, sizeof(buf));

    memset(buf, 0, 128);
    aw_cache_flush(buf, 128);

    /* 使buf缓存无效，重新从内存中读取数据  */
    aw_cache_invalidate(buf, 128);

    /* 可以使用buf */
    // todo

    return ;
}

/** [src_cache] */

/* end of file */
