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
 * \brief 按键演示例程
 *
 * - 操作步骤：
 *   1. 在aw_prj_params.h头文件里使能
 *      - 调试串口对应的宏
 *      - AW_COM_CONSOLE
 *      - AW_DEV_GPIO_KEY
 *      - AW_COM_INPUT_EV_ABS
 *
 *   2. 使用串口线分别和开发板的调试串口DURX、DUTX相连，打开串口调试助手，
 *      设置波特率为115200，1个停止位，无校验，无流控。
 *
 *   3.将  KEY1 对应的管脚连接到用户按键电路，本例程按键 KEY1 所使用GPIO连接可在用户配置文件
 *    awbl_hwconf_gpio_key.h 中设置。
 *
 *   4. 配置awbl_hwconf_gpio_key.h文件，设置按键按下为低电平。
 *
 * - 实验现象：
 *   1. 串口打印出按键按下/松开信息，以及模拟按键、模拟坐标信息。
 *
 * \par 源代码
 * \snippet demo_key.c src_key
 *
 * \internal
 * \par Modification History
 * - 1.00 16-03-25  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_key 按键演示
 * \copydoc demo_key.c
 */

/** [src_key] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_gpio.h"
#include "aw_input.h"
#include "aw_input_code.h"

/******************************************************************************/
#define SIMULATE_KEY_EVENT       (0xF0) /**< \brief 模拟按键事件 */
#define SIMULATE_KEY_DOWN        (1)    /**< \brief 模拟按键按下 */
#define SIMULATE_KEY_UP          (0)    /**< \brief 模拟按键松开 */

#define SIMULATE_ABS_COORD_EVENT (0xF1) /**< \brief 模拟绝对坐标事件 */
#define SIMULATE_ABS_COORD_X     (3)    /**< \brief 模拟绝对X坐标值 */
#define SIMULATE_ABS_COORD_Y     (4)    /**< \brief 模拟绝对Y坐标值 */
#define SIMULATE_ABS_COORD_Z     (5)    /**< \brief 模拟绝对Z坐标值 */

/******************************************************************************/
/**
 * \brief 按键回调函数
 * \param[in] p_input_data  按键/指针输入事件数据
 * \param[in] p_usr_data    用户自定义数据
 */
aw_local void __key_proc_cb (aw_input_event_t *p_input_data, void *p_usr_data)
{
    struct aw_input_key_data *p_key_data =
                             (struct aw_input_key_data *)p_input_data;
    struct aw_input_ptr_data *p_ptr_data =
                             (struct aw_input_ptr_data *)p_input_data;

    if ((p_input_data != NULL) &&
        (AW_INPUT_EV_KEY != p_input_data->ev_type) &&
        (AW_INPUT_EV_ABS != p_input_data->ev_type)) {
        return;
    }

    switch (p_key_data->key_code) {

    case KEY_1 :
        if (p_key_data->key_state) {
            AW_INFOF(("key_proc_cb : KEY1 is down : %d. \n",
                      p_key_data->key_state));
        } else {
            AW_INFOF(("key_proc_cb : KEY1 is release : %d. \n",
                      p_key_data->key_state));
        }
        break;

    case SIMULATE_KEY_EVENT:
        if (p_key_data->key_state) {
            AW_INFOF(("key_proc_cb : SIMULATE_KEY is down : %d. \n",
                      p_key_data->key_state));
        } else {
            AW_INFOF(("key_proc_cb : SIMULATE_KEY is release : %d. \n",
                      p_key_data->key_state));
        }
        break;

    case SIMULATE_ABS_COORD_EVENT:
        AW_INFOF(("abs coordinate: x = %d, y = %d, z = %d\n",
                  p_ptr_data->pos.x,
                  p_ptr_data->pos.y,
                  p_ptr_data->pos.z));
        break;

    default:
        AW_INFOF(("Unknow Key Code!\r\n"));
        break;
    }
}

/******************************************************************************/
/**
 * \brief 按键 demo
 * \return 无
 */
aw_local void* __task_handle (void *arg)
{
    int         times   = 0;

    struct aw_input_handler key_hdlr;

    aw_kprintf("\nKey demo testing...\n");

    /* 注册一个按键输入句柄到输入子系统 */
    aw_input_handler_register(&key_hdlr, __key_proc_cb, NULL);

    AW_FOREVER {
        aw_kprintf("\nYou can press the key1 ~ key4 for watching the phenomenon!\n");
        aw_mdelay(2000); /* 延时2s */

        /* 模拟上报一个按键事件-按下 */
        aw_input_report_key(AW_INPUT_EV_KEY,
                            SIMULATE_KEY_EVENT,
                            SIMULATE_KEY_DOWN);

        aw_mdelay(2000); /* 延时2s */

        /* 模拟上报一个按键事件-松开 */
        aw_input_report_key(AW_INPUT_EV_KEY,
                            SIMULATE_KEY_EVENT,
                            SIMULATE_KEY_UP);

        aw_mdelay(2000); /* 延时2s */

        /* 模拟上报一个绝对坐标事件 */
        aw_input_report_coord(AW_INPUT_EV_ABS,
                              SIMULATE_ABS_COORD_EVENT,
                              SIMULATE_ABS_COORD_X,
                              SIMULATE_ABS_COORD_Y,
                              SIMULATE_ABS_COORD_Z);

        times++;

        if (times == 10) {
            break;
        }
    }
    /* 从输入子系统中注销按键输入句柄 */
    aw_input_handler_unregister(&key_hdlr);

    return NULL;
}

/******************************************************************************/
void demo_key_entry (void)
{
    aw_task_id_t    tsk;

    tsk = aw_task_create("Key demo",
                         12,
                         1024,
                         __task_handle,
                         (void *)NULL);
    if (tsk == NULL) {
        aw_kprintf("Key demo task create failed\r\n");
        return;
    }

    aw_task_startup(tsk);
}

/** [src_key] */

/* end of file */
