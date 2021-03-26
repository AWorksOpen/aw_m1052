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
 * \brief 延时函数演示例程
 *
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能AW_DEV_GPIO_LED。
 *
 * - 实验现象：
 *   1. LED以2s为周期闪烁。
 *
 * \par 源代码
 * \snippet demo_delay.c src_delay
 *
 * \internal
 * \par Modification History
 * - 1.00 17-09-06  sup, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_delay 延时函数
 * \copydoc demo_delay.c
 */

/** [src_delay] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_led.h"
#include "aw_demo_config.h"

#define  LED0      DM_LED

/**
 * \brief 延时函数演示例程入口
 * \return 无
 */
void demo_delay_entry (void)
{
    AW_FOREVER {
        aw_led_on(LED0);
        aw_mdelay(1000);    /* 延时1s */

        aw_led_off(LED0);
        aw_mdelay(1000);    /* 延时1s */
    }
}

/** [src_delay] */

/* end of file */
