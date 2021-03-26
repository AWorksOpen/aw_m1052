/*
 * app_led.c
 *
 *  Created on: 2019Äê10ÔÂ5ÈÕ
 *      Author: zengqingyuhang
 */

/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_led.h"
#include "aw_task.h"

#define LED     0

void __led_handler(void *parg)
{
    AW_FOREVER {
        aw_led_on(LED);
        aw_mdelay(200);
        aw_led_off(LED);
        aw_mdelay(800);
    }
}


void app_led (void)
{
    aw_task_id_t    tsk;
    tsk = aw_task_create("led_task",
                          10,
                          1024,
                          __led_handler,
                          NULL);
    if (tsk == NULL) {
        aw_kprintf("led task create error!\r\n");
        return ;
    }
    aw_task_startup(tsk);
}


