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
 * \brief 堆内存操作例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - AW_COM_CONSOLE
 *      - 对应平台的串口宏
 *
 * - 实验现象：
 *   1. 串口打印内存分配成功时使用的字节对齐数，可以看到只有对齐长度是4x2^n，分配内存才会成功。
 *
 * \par 源代码
 * \snippet demo_mem_align.c src_mem_align
 *
 * \internal
 * \par Modification History
 * - 1.00 18-06-27  lqy, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_mem_align 堆内存操作
 * \copydoc demo_mem_align.c
 */

/** [src_mem_align] */
#include "aworks.h"
#include "aw_mem.h"
#include "aw_cache.h"
#include "aw_vdebug.h"

#define     BUF_SIZE    1024
#define     ALIGN(i)    ((0) + (i))

void demo_mem_align_entry (void)
{
    uint8_t    *p_temp = NULL;
    uint32_t    i      = 0;

    aw_kprintf(" aw_cache_dam_align: \n");
    for (i = 0; i < 500; i++) {

       /* ALIGN(i)对齐长度是4x2^n，分配内存才会成功 */
        p_temp = (uint8_t *)aw_cache_dma_align(BUF_SIZE, ALIGN(i));
        if (NULL != p_temp) {
            aw_kprintf(" address: 0x%X ", p_temp);
            if (((uint32_t)p_temp % ALIGN(i)) != 0) {
                aw_kprintf(" misaligned %d \n", ALIGN(i));
            } else {
                aw_kprintf(" address: 0x%X align %d \n", p_temp, ALIGN(i));
            }
            aw_cache_dma_free(p_temp);
        }
    }

    aw_kprintf(" aw_mem_align: \n");
    for (i = 0; i < 500; i++) {

        /* ALIGN(i)对齐长度是4x2^n，分配内存才会成功 */
        p_temp = (uint8_t *)aw_mem_align(BUF_SIZE, ALIGN(i));
        if (NULL != p_temp) {
            aw_kprintf(" address: 0x%X ", p_temp);
            if (((uint32_t)p_temp % ALIGN(i)) != 0) {
                aw_kprintf(" misaligned %d \n", ALIGN(i));
            } else {
                aw_kprintf(" align %d \n", ALIGN(i));
            }
            aw_mem_free(p_temp);
        }
    }
}

/** [src_mem_align] */

/* end of file */
