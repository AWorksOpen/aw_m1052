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
 * \brief LED例程，通过标准接口实现
 *
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能AW_DEV_GPIO_LED
 *   2. 具体LED硬件配置需查看awbl_hwconf_gpio_led.h
 *   3. 禁能其它地方对LED的操作
 *
 * - 实验现象：
 *   1. LED以1Hz的频率闪烁。
 *
 * \par 源代码
 * \snippet demo_led.c src_led
 *
 * \internal
 * \par Modification history
 * - 1.00 17-09-09  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_led LED
 * \copydoc demo_led.c
 */

/** [src_led] */
#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_led.h"
#include "aw_demo_config.h"

#define  LED  DM_LED

/**
 * \brief LED demo
 * \return 无
 */
aw_local void* __task_handle (void *arg)
{
    aw_kprintf("\nled demo testing...\n");

    while(1) {
        aw_led_on(LED);             /* 点亮LED灯 */
        aw_mdelay(500);             /* 延时0.5s */
        aw_led_off(LED);
        aw_mdelay(500);

        aw_led_set(LED, AW_TRUE);   /* 点亮LED灯 */
        aw_mdelay(500);
        aw_led_set(LED, AW_FALSE);  /* 熄灭LED灯 */
        aw_mdelay(500);

        aw_led_toggle(LED);         /* LED状态翻转 */
        aw_mdelay(500);
        aw_led_toggle(LED);         /* LED状态翻转 */
        aw_mdelay(500);
    }

    return 0;
}

/******************************************************************************/
void demo_led_entry (void)
{
    aw_task_id_t    tsk;

    tsk = aw_task_create("Led demo",
                         12,
                         1024,
                         __task_handle,
                         (void *)NULL);
    if (tsk == NULL) {
        aw_kprintf("Led demo task create failed\r\n");
        return;
    }

    if (aw_task_startup(tsk) != AW_OK) {
        aw_task_delete(tsk);
    }
}

/** [src_led] */

/* end of file */
