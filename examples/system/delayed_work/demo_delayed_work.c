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
 ******************************************************************************/

/**
 * \file
 * \brief 延迟作业演示例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能AW_COM_CONSOLE；
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象：
 *     串口打印操作信息。
 *
 * \par 源代码
 * \snippet demo_delayed_work.c src_delayed_work
 *
 * \internal
 * \par Modification History
 * - 1.00 17-09-06  sup, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_delayed_work 延迟作业
 * \copydoc demo_delayed_work.c
 */

/** [src_delayed_work] */
#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_delayed_work.h"

 static void work_callback (void *p_arg)
 {
     static uint8_t i = 0;

     i++;
     AW_INFOF(("Process delay work:%d\n", i));

     if (i < 5) {
         AW_INFOF(("Delayed work restart:%d\n", i));
         /* 再次启动work, 延时时间改为1s */
         aw_delayed_work_start((struct aw_delayed_work *)p_arg, 1000);

     } else {
         AW_INFOF(("Delayed work stop\n"));
         /* 停止delayed work */
         aw_delayed_work_stop((struct aw_delayed_work *)p_arg);
     }
 }

/**
 * \brief 延迟作业演示例程入口
 * \return 无
 */
void demo_delayed_work_entry (void)
{
    /* 定义延迟作业对象 */
    static struct aw_delayed_work work;

    /* 初始化delayed work */
    aw_delayed_work_init(&work, work_callback, &work);

    /* 启动delayed work */
    AW_INFOF(("Start delayed work,process work after 3s\n"));
    aw_delayed_work_start(&work, 3000); /* 3s后启动work */
}

/** [src_delayed_work] */

/* end of file */
