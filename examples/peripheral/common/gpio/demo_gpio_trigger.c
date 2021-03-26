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
 * \brief gpio触发中断例程
 *
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能
 *      - AW_COM_CONSOLE
 *      - AW_DEV_GPIO_LED
 *      - 对应平台的串口宏
 *   2. 测试引脚的触发功能时，需用杜邦线将  TEST_GPIO_0 和 TEST_GPIO_1 对应的引脚连接起来。
 *   3. 屏蔽其他使用到LED灯的地方
 *
 * - 实验现象：
 *  1. 进入中断后，LED灯闪烁（电平触发模式LED可能不会闪烁）；
 *  2. 串口打印调试信息。
 *
 *
 * \par 源代码
 * \snippet demo_gpio_trigger.c src_gpio_trigger
 *
 * \internal
 * \par Modification history
 * - 1.00 18-06-27  lqy, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_gpio_trigger
 * \copydoc demo_gpio_trigger.c
 */

/** [src_gpio_trigger] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_gpio.h"
#include "aw_led.h"
#include "aw_vdebug.h"
#include "aw_demo_config.h"
#include "aw_sem.h"

/* 平台GPIO，可根据具体平台修改 */
#define TEST_GPIO_0     DM_GPIO_TRG_OUTPUT
#define TEST_GPIO_1     DM_GPIO_TRG_INTR

// 定义二进制信号量 sem
AW_MUTEX_DECL_STATIC(mutex);

/**\brief 记录中断进入的次数 */
aw_local uint8_t volatile __g_test_count = 0;

/* gpio 测试回调函数 */
void test_gpio_trig_isr (void* arg);

void demo_gpio_trigger_entry (void)
{
    // 初始化信号量
    AW_MUTEX_INIT(mutex, AW_SEM_Q_PRIORITY);

    uint32_t i   = 0;
    uint32_t ret = 0;

    aw_kprintf("\nGPIO trigger demo testing...\r\n");

    /* 申请中断引脚并配置引脚输出 */
    int test_gpios[] = {TEST_GPIO_0, TEST_GPIO_1};
    if (aw_gpio_pin_request("test_gpios",
                             test_gpios,
                             AW_NELEMENTS(test_gpios)) == AW_OK) {
        aw_gpio_pin_cfg(TEST_GPIO_0,
                        AW_GPIO_PUSH_PULL | AW_GPIO_OUTPUT_INIT_HIGH);
        aw_gpio_pin_cfg(TEST_GPIO_1, AW_GPIO_INPUT);
    }

    /* 连接中断回调函数 */
    ret = aw_gpio_trigger_connect(TEST_GPIO_1, test_gpio_trig_isr, 0);
    if (ret != AW_OK) {
        aw_kprintf("gpio trigger connect failed!\n");
        return;
    }

//#define TRIGGER_FLAG    AW_GPIO_TRIGGER_LOW         /* 配置为低电平触发 */
//#define TRIGGER_FLAG    AW_GPIO_TRIGGER_HIGH        /* 配置为高电平触发 */
#define TRIGGER_FLAG    AW_GPIO_TRIGGER_BOTH_EDGES  /* 配置为双边沿触发 */
//#define TRIGGER_FLAG    AW_GPIO_TRIGGER_FALL        /* 配置为下降沿触发 */
//#define TRIGGER_FLAG    AW_GPIO_TRIGGER_RISE        /* 配置为上升沿触发 */

    ret = aw_gpio_trigger_cfg(TEST_GPIO_1, TRIGGER_FLAG);
    if (ret != AW_OK) {
        aw_kprintf("gpio trigger cfg failed!\n");
        return;
    }

    /* 启用触发中断 */
    ret = aw_gpio_trigger_on(TEST_GPIO_1);
    if (ret != AW_OK) {
        aw_kprintf("gpio trigger on failed!\n");
        return;
    }

    for (i = 0; i < 19; i++) {
#if TRIGGER_FLAG == AW_GPIO_TRIGGER_LOW || TRIGGER_FLAG == AW_GPIO_TRIGGER_HIGH
        /* 打开在回调函数中关闭的触发中断 */
        ret = aw_gpio_trigger_on(TEST_GPIO_1);
        if (ret != AW_OK) {
            aw_kprintf("gpio trigger on failed!\n");
            return;
        }
#endif
        aw_task_delay(500);
        aw_gpio_toggle(TEST_GPIO_0);
    }

    AW_MUTEX_LOCK(mutex,AW_SEM_WAIT_FOREVER);

    /* 打印进入中断次数 */
    AW_INFOF(("entry gpio int times: %d\r\n", __g_test_count));

    AW_MUTEX_UNLOCK(mutex);

    /* 断开中断回调函数 */
    aw_gpio_trigger_disconnect(TEST_GPIO_1, test_gpio_trig_isr, 0);
    /* 关闭触发中断 */
    aw_gpio_trigger_off(TEST_GPIO_1);

    /* 释放引脚 */
    aw_gpio_pin_release(test_gpios, AW_NELEMENTS(test_gpios));
    
    aw_kprintf("GPIO trigger demo exit...\r\n");
}

/* gpio 测试回调函数 */
void test_gpio_trig_isr (void* arg)
{
#if TRIGGER_FLAG == AW_GPIO_TRIGGER_LOW || TRIGGER_FLAG == AW_GPIO_TRIGGER_HIGH
    /* 关闭触发中断，避免电平触发时不停地进中断导致程序无法继续运行 */
    aw_gpio_trigger_off(TEST_GPIO_1);
#endif
    aw_led_toggle(0);

    AW_MUTEX_LOCK(mutex,AW_SEM_WAIT_FOREVER);

    __g_test_count++;

    AW_MUTEX_UNLOCK(mutex);


}

/** [src_gpio_trigger] */

/* end of file */
