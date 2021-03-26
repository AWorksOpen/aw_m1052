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
 * \brief (内存地址) 性能测试
 *
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能 对应平台UART宏。
 *
 * - 实验现象：
 *   1. 串口打印计算信息。
 *
 * \par 源代码
 * \snippet demo_unaligned_access.c src_unaligned_access
 *
 * \internal
 * \par Modification history
 * - 1.00 17-09-09  mex, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_unaligned_access 内存性能测试
 * \copydoc demo_unaligned_access.c
 */

/** [src_unaligned_access] */

#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_cache.h"
#include "string.h"
#include "aw_mem.h"

#pragma pack(push)
#pragma pack(1)
typedef struct {
    int             a;
    char            b;
    volatile int    c;
} test_pack;

#pragma pack(pop)

aw_local void test_unaligned_access_helper (void)
{
    volatile test_pack *p;

    p = (volatile test_pack *)aw_mem_align(sizeof(test_pack), 4);

    if (NULL == p) {
        aw_kprintf("mem assign fail.\r\n");
        return ;
    }

    aw_kprintf("p->a addr:0x%x\r\n", &p->a);
    aw_kprintf("p->b addr:0x%x\r\n", &p->b);
    aw_kprintf("p->c addr:0x%x\r\n", &p->c);

    p->a = 2000;
    p->b = 'b';
    p->c = 1000;

    aw_kprintf("p->a = %d\r\n", p->a);
    aw_kprintf("p->b = %c\r\n", p->b);
    aw_kprintf("p->c = %d\r\n", p->c);

    aw_mem_free((void *)p);

    return ;
}

aw_local void test_memcpy_helper (void)
{
    volatile char *p;

    p = (volatile char *)aw_mem_align(1024, 4);

    aw_kprintf("p addr:0x%x\r\n", p);

    aw_kprintf("p + 1 addr:0x%x\r\n", (p + 1));

    memcpy(p + 1 ,"123456789", 10);

    aw_kprintf("p + 1 data:%s\r\n", p + 1);

    aw_mem_free((void *)p);

    return ;
}

void demo_unaligned_access_entry (void)
{
    aw_kprintf("\n");

    test_unaligned_access_helper();
    test_memcpy_helper();

    return;

}


/** [src_unaligned_access] */

/* end of file */
