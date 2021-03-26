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
#include "aw_led.h"
#include "aw_delay.h"
#include "aw_errno.h"
#define LED 0


int aw_main()
{
    aw_kprintf("\r\nApplication Start. \r\n");


    while (1) {
        aw_led_toggle(LED);
        aw_mdelay(500);
    }

    return 0;
}

/* end of file*/
