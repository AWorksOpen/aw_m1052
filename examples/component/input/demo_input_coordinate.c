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
 * \brief AWorks输入子系统使用例程（模拟坐标输入）
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - 调试串口对应的宏
 *      - AW_COM_CONSOLE
 *      - AW_COM_INPUT_EV
 *      - AW_COM_INPUT_EV_ABS
 *   2. 将板子中的DURX、DUTX通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象：
 *   1. 串口循环打印坐标值。
 *
 * \par 源代码
 * \snippet demo_input_coordinate.c src_input_coordinate
 *
 * \internal
 * \par Modification history
 * - 1.00 16-04-05  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_input_coordinate 输入子系统
 * \copydoc demo_input_coordinate.c
 */

/** [src_input_coordinate] */
#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_input.h"
#include "aw_demo_config.h"
#include "aw_task.h"
#include "aw_delay.h"


static void* __input_task (void *p_arg);
static void __input_callback (aw_input_event_t *p_input_data, void *p_usr_data);


static void __input_callback (aw_input_event_t *p_input_data, void *p_usr_data)
{
    struct aw_input_ptr_data *p_data;

    p_data = (struct aw_input_ptr_data *)p_input_data;

    if (p_data->input_ev.ev_type != AW_INPUT_EV_ABS) {
        aw_kprintf("this is not coordinate input event.\r\n");
    } else {
        aw_kprintf("coordinate(x = %d, y = %d, z = %d)\r\n",
                   p_data->pos.x,
                   p_data->pos.y,
                   p_data->pos.z);
    }

    return ;
}

static void* __input_task (void * p_arg)
{
    aw_input_handler_t input_handler;
    int x, y, z;
    int ret;

    /* 注册按键输入事件到输入子系统中 */
    aw_input_handler_register(&input_handler, __input_callback, NULL);

    while (1) {

        /*模拟产生随机坐标*/
        x = rand() % 100;
        y = rand() % 100;
        z = rand() % 100;

        /*每隔一秒上报一个坐标输入事件*/
        ret = aw_input_report_coord(AW_INPUT_EV_ABS, ABS_RESERVED, x, y, z);
        if (ret != AW_OK) {
            aw_kprintf("aw_input_report_coord error:%d\r\n", ret);
        }

        aw_mdelay(1000);
    }

    return 0;
}

/******************************************************************************/
/**
 * \brief 任务入口函数
 * \param[in] p_arg  任务入口参数
 */
void demo_input_coordinate_entry (void)
{
    aw_kprintf("\ninput testing...\n");

    aw_task_id_t task_id = aw_task_create("input task",
                                          8,
                                          1024,
                                          __input_task,
                                          NULL);
    if (task_id == NULL) {
        aw_kprintf("input task startup fail.\r\n");
    } else {
        aw_kprintf("input task startup successful.\r\n");
        aw_task_startup(task_id);
    }

    return ;
}

/** [src_input_coordinate] */

/* end of file */
