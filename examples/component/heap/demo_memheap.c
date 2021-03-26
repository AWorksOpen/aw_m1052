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
 * \brief 堆管理器演示例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能 AW_COM_CONSOLE和调试串口对应的宏；
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象:
 *   1. 打印一块从堆内存分配出来的内存的地址
 *
 * \par 源代码
 * \snippet demo_memheap.c src_memheap
 *
 * \internal
 * \par History
 * - 1.00 16-03-25  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_memheap 堆管理器
 * \copydoc demo_memheap.c
 */

/** [src_memheap] */
#include "aworks.h"
#include "aw_memheap.h"
#include "aw_vdebug.h"

#define  HEAP_SIZE 1024

static char         __g_heap_buf[HEAP_SIZE];    /* 堆管理器使用的内存 */
static aw_memheap_t __g_my_heap;                /* 堆管理器对象 */

/**
 * \brief 堆管理器例程
 *
 * \return 无
 */
void demo_memheap_entry (void)
{
    aw_err_t  ret = AW_OK;
    int       i   = 0;

    char     *ptr = NULL;

    /* 堆管理器初始化 */
    ret = aw_memheap_init(&__g_my_heap,     // 堆管理器
                          "my_heap",        // 内存堆的名字
                          __g_heap_buf,     // 可用的地址空间
                          HEAP_SIZE);       // 地址空间大小
    if (ret != AW_OK) {
        AW_ERRF(("aw_memheap_init err: %d\n", ret));
        return;
    }

    /* 从堆管理器中分配内存 */
    ptr = (char *)aw_memheap_alloc(&__g_my_heap, 32);
    if (NULL == ptr) {
        AW_ERRF(("aw_memheap_alloc err!\n"));
        return;
    }

    /* 测试分配的内存 */
    for (i = 0; i < 32; i++) {
        ptr[i] = (char)i;
    }

    /* 释放分配的内存 */
    if (ptr) {
        aw_kprintf("Got an addr 0x%x.\r\n", ptr);
        aw_memheap_free(ptr);

    } else {
        aw_kprintf("Had nothing got.\r\n");
    } 
}

/** [src_memheap] */

/* end of file */
