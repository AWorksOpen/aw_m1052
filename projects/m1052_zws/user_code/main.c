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

#define LED 0

extern void app_main (void);


int aw_main()
{
    aw_kprintf("\r\nApplication Start. \r\n");

    app_main();

    while (1) {

        aw_mdelay(500);
    }

    return 0;
}


/* end of file*/
