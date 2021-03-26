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
 * \brief 内存池演示例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能AW_COM_CONSOLE和调试串口使能宏；
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象:
 *      串口打印 内存池 操作结果信息。
 *
 * \par 源代码
 * \snippet demo_pool.c src_pool
 *
 * \internal
 * \par History
 * - 1.00 17-09-12  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_pool 内存池
 * \copydoc demo_pool.c
 */

/** [src_pool] */
#include "aworks.h"
#include "aw_pool.h"
#include "aw_vdebug.h"

/**
 * \brief 内存池例程入口
 *
 */
void demo_pool_entry (void)
{
#define  TESN_POOL_CNT  10

    aw_pool_t       my_pool;                /* 内存池对象 */
    aw_pool_id_t    my_pool_id;             /* 内存池句柄 */
    static int      bulk[TESN_POOL_CNT];    /* 内存块 */

    int             i;
    int            *p_item[TESN_POOL_CNT] = {0};

    /* 内存池初始化 */
    my_pool_id = aw_pool_init(&my_pool,
                              bulk,
                              sizeof(bulk),
                              sizeof(bulk[0]));
    if (NULL == my_pool_id) {
        AW_ERRF(("aw_pool_init err!\n"));
        return;
    }

    /* 测试从内存池获取内存 */
    for (i = 0; i < TESN_POOL_CNT; i++) {

        /* 从内存池分配内存 */
        p_item[i]  = (int *)aw_pool_item_get(my_pool_id);
        if (NULL == p_item[i]) {
            AW_ERRF(("pool have no mem!\n"));
            break;
        }

        /* 使用内存 */
        *p_item[i] = i;
    }

    /* 释放从内存池分配的内存 */
    for (i = 0; i < TESN_POOL_CNT; i++) {
        if (p_item[i] != NULL) {
            aw_pool_item_return(my_pool_id, p_item[i]);
        }
    }

    AW_INFOF(("pool demo done.\n"));
}

/** [src_pool] */

/* end of file */
