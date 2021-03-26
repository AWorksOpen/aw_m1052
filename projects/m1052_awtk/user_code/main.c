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
/**
 * \file
 * \brief awtk示例工程
 *
 * - 操作步骤:
 *   1. 接入显示屏
 *   2. 工程默认配置4.3寸显示屏，如果接入7寸显示屏，需在aw_prj_params.h中：
 *      - 打开7寸显示屏
 *      - 关闭4.3寸显示屏
 *
 * - 实验现象:
 *   1. 第一次烧录程序需要进行触摸校验，触摸校验通过后进入gui界面
 *   2. 后续再次烧录程序不需要触摸校验，直接进入gui界面
 *
 */
#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_led.h"
#include "aw_delay.h"
#include "aw_fb.h"

#define LED 0

extern void app_awtk_demo (void);

int aw_main()
{
    aw_kprintf("\r\nApplication Start. \r\n");

    app_awtk_demo();

    while (1) {
        aw_led_toggle(LED);
        aw_mdelay(500);
    }

    return 0;
}


/* end of file*/
