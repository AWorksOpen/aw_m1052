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
 * \brief interrupt例程
 *
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能
 *      - AW_COM_CONSOLE
 *      - 对应平台的串口宏
 *   2. 测试引脚的触发功能时，需用杜邦线将  TEST_IO_INTR 和 TEST_IO_OUTPUT 对应的引脚连接起来。
 *
 * - 实验现象：
 *  1. 串口打印调试信息
 *
 *
 * \par 源代码
 * \snippet demo_interrupt.c src_interrupt
 *
 * \internal
 * \par Modification history
 * - 1.00 18-06-27  lqy, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_interrupt 中断
 * \copydoc demo_interrupt.c
 */

/** [src_interrupt] */
#include "aworks.h"
#include "aw_gpio.h"
#include "aw_sem.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_demo_config.h"
#include "aw_int.h"

/* 平台GPIO，可根据具体平台修改 */
#define TEST_IO_INTR    DM_GPIO_TRG_INTR
#define TEST_IO_OUTPUT  DM_GPIO_TRG_OUTPUT

/**\brief 记录是否产生中断 */
AW_SEMB_DECL_STATIC(__gpio_intr_semb);


/* gpio 测试回调函数 */
static void __test_gpio_trig_isr (void* arg)
{
    AW_SEMB_GIVE(__gpio_intr_semb);
}


void demo_interrupt_entry (void)
{
    aw_err_t    err;
    int         i;

    aw_kprintf("\ninterrupt demo testing...\r\n");

    /* 信号量初始化 */
    AW_SEMB_INIT(__gpio_intr_semb, AW_SEM_EMPTY, AW_SEM_Q_FIFO);

    /* 申请中断引脚并配置引脚 */
    int test_gpios[] = {TEST_IO_INTR, TEST_IO_OUTPUT};
    if (aw_gpio_pin_request("test_gpios",
                             test_gpios,
                             AW_NELEMENTS(test_gpios)) == AW_OK) {

        aw_gpio_pin_cfg(TEST_IO_OUTPUT,
                        AW_GPIO_PUSH_PULL | AW_GPIO_OUTPUT_INIT_HIGH);
        aw_gpio_pin_cfg(TEST_IO_INTR, AW_GPIO_INPUT);
    }

    /* 连接中断回调函数 */
    err = aw_gpio_trigger_connect(TEST_IO_INTR,
                                  __test_gpio_trig_isr,
                                  (void *)AW_GPIO_TRIGGER_FALL);
    if (err != AW_OK) {
        aw_kprintf("gpio trigger connect failed!\n");
        return;
    }

    /* 配置为下降沿触发 */
    err = aw_gpio_trigger_cfg(TEST_IO_INTR, AW_GPIO_TRIGGER_FALL);
    if (err != AW_OK) {
        aw_kprintf("gpio trigger cfg failed!\n");
        return;
    }

    /* 开启引脚的触发 */
    err = aw_gpio_trigger_on(TEST_IO_INTR);
    if (err != AW_OK) {
        aw_kprintf("gpio trigger on failed!\n");
        return;
    }

    for (i = 0; i < 50; i++) {

        /* 设置输出管脚为低电平 */
        aw_gpio_set(TEST_IO_OUTPUT, 0);

        /* 等待中断触发 */
        err = AW_SEMB_TAKE(__gpio_intr_semb, 1000);
        if (err == AW_OK) {
            aw_kprintf("enter gpio interrupt!\n");
        }

        /* 设置输出管脚为高电平 */
        aw_gpio_set(TEST_IO_OUTPUT, 1);

        aw_mdelay(100);
    }

    /* 断开中断连接回调函数 */
    aw_gpio_trigger_disconnect(TEST_IO_INTR,
                               __test_gpio_trig_isr,
                               (void *)AW_GPIO_TRIGGER_FALL);

    /* 关闭引脚的触发 */
    aw_gpio_trigger_off(TEST_IO_INTR);

    /* 释放引脚 */
    aw_gpio_pin_release(test_gpios, AW_NELEMENTS(test_gpios));
    
    /* 终止信号量 */
    AW_SEMB_TERMINATE(__gpio_intr_semb);

    aw_kprintf("interrupt demo exit...\r\n");
}


/** [src_interrupt] */

/* end of file */
