/*******************************************************************************
*                                  AWorks
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
 * \brief 标准C库函数使用例程（内存）
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能AW_COM_CONSOLE和调试串口对应的宏；
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象：
 *   1. 串口打印调试信息。
 *
 * \par 源代码
 * \snippet demo_libc_mem.c src_libc_mem
 *
 * \internal
 * \par Modification history
 * - 1.00 17-09-07  mkr, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_libc_mem 内存(标准C库)
 * \copydoc demo_libc_mem.c
 */

/** [src_libc_mem] */
#include "aworks.h"
#include "aw_vdebug.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * \brief libc 内存接口示例入口函数
 *
 * \return 无
 */
void demo_libc_mem_entry (void)
{
    uint8_t  i;
    uint8_t *p_dat = NULL;

    aw_kprintf("\r\nLibc memory interface testing...\r\n");

    p_dat = (uint8_t *)malloc(10 * sizeof(uint8_t));
    if (NULL == p_dat) {
        aw_kprintf("malloc allocate failed!\r\n");
    } else {
        aw_kprintf("malloc allocate success!\r\n");
    }

    for (i = 0; i < 9; i++) {
        p_dat[i] = '0' + i;
    }
    p_dat[i] = '\0';

    aw_kprintf("pdat = %s\r\n", p_dat);

    free(p_dat);
    p_dat = NULL;

    aw_kprintf("memory was freed\r\n");
}

/** [src_libc_mem] */

/* end of file */
