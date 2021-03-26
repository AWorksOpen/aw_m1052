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
 * \brief AWorks输入子系统使用例程（按键输入）
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - 调试串口对应的宏
 *      - 对应平台的PWM宏
 *      - AW_COM_CONSOLE
 *      - AW_COM_INPUT_EV
 *      - AW_COM_INPUT_EV_KEY
 *      - AW_DEV_PWM_BUZZER
 *   2. 将开发板DE_KEY_IO管脚连接到用户按键电路；
 *   2. 将板子中的DURX、DUTX通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象：
 *   1. 按键状态变化为1时，蜂鸣器响200ms。
 *
 * \par 源代码
 * \snippet demo_input.c src_input
 *
 * \internal
 * \par Modification history
 * - 1.00 16-04-05  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_input_key 输入子系统
 * \copydoc demo_input_key.c
 */

/** [src_input_key] */
#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_gpio.h"
#include "aw_input.h"
#include "aw_demo_config.h"
#include "aw_task.h"
#include "aw_defer.h"
#include "aw_delay.h"
#include "aw_buzzer.h"

/******************************************************************************/
#define GPIO_KEY   DE_KEY_IO    /**< \brief GPIO按键管脚号 */
#define DELAY_MS   25           /**< \brief 延时时间-ms */
/******************************************************************************/
static int KEY_NUM;             /**< \brief 按键状态flag */
/******************************************************************************/

static void* __task_key (void * p_arg);
static void* __task_input (void * p_arg);

/**
 * \brief GPIO 按键触发回调函数
 * \param[in] p_arg  回调函数参数
 */
aw_local void __gpio_key_callback (void *p_arg)
{
    uint32_t key      = (uint32_t)p_arg;

    uint32_t cur_stat = aw_gpio_get(key);

    /* 上报按键事件 */
    aw_input_report_key(AW_INPUT_EV_KEY, KEY_0, cur_stat);
}

/**
 * \brief 输入子系统回调函数
 * \param[in] p_input_data  输入事件数据
 * \param[in] p_usr_data    用户数据
 */
aw_local void key_proc_cb (struct aw_input_event *p_input_data, void *p_usr_data)
{
    struct aw_input_key_data *p_key_data = NULL;

    if ((p_input_data != NULL) &&
        (p_input_data->ev_type != AW_INPUT_EV_KEY)) {
        return;
    }

    p_key_data = (struct aw_input_key_data *)p_input_data;
    if (p_key_data->key_code == KEY_0) {
        if (!p_key_data->key_state) {
            KEY_NUM = 1;
        }
    }
}

static void* __task_key (void * p_arg)
{
    /* 配置GPIO按键管脚为输入功能 */
    int gpio0_gpios[] = {GPIO_KEY};

    if (aw_gpio_pin_request("gpios",
                             gpio0_gpios,
                             AW_NELEMENTS(gpio0_gpios)) == AW_OK) {
        aw_gpio_pin_cfg(GPIO_KEY, AW_GPIO_INPUT | AW_GPIO_PULL_UP);
    }

    /* 连接回调函数到引脚 */
    aw_gpio_trigger_connect(GPIO_KEY, __gpio_key_callback, (void *)GPIO_KEY);

    /* 设置引脚触发类型，下降沿触发 */
    aw_gpio_trigger_cfg(GPIO_KEY, AW_GPIO_TRIGGER_FALL);

    /* 开启引脚上的触发器  */
    aw_gpio_trigger_on(GPIO_KEY);

    while (1) {
        aw_mdelay(500);
    }

    return 0;
}

static void* __task_input (void * p_arg)
{
    struct aw_input_handler key_input_handler;

    /* 注册按键输入事件到输入子系统中 */
    aw_input_handler_register(&key_input_handler, key_proc_cb, NULL);

    while (1) {
        if (KEY_NUM == 1) {
            aw_buzzer_loud_set(80); /* 设置蜂鸣器鸣叫强度 */
            aw_buzzer_beep(200);    /* 蜂鸣器延时200ms */
            KEY_NUM = 0;
        }
    }

    return 0;
}

/******************************************************************************/
/**
 * \brief demo入口
 */
void demo_input_key_entry (void)
{
    aw_kprintf("\ninput testing...\n");

    aw_task_id_t task_id1 = aw_task_create ("key_task",
                                           8,
                                           1024,
                                           __task_key,
                                           (void *)NULL);
    if (task_id1 == NULL) {
        aw_kprintf("key task startup fail.\r\n");
        return ;
    } else {
        aw_task_startup(task_id1);
        aw_kprintf("key task startup successful.\r\n");
    }

    aw_task_id_t task_id2 = aw_task_create ("key_task",
                                            7,
                                            1024,
                                            __task_input,
                                            (void *)NULL);
    if (task_id2 == NULL) {
        aw_kprintf("input task startup fail.\r\n");
        return ;
    } else {
        aw_task_startup(task_id2);
        aw_kprintf("input task startup successful.\r\n");
    }

    return ;
}

/** [src_input_key] */

/* end of file */
