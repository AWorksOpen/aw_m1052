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
 * \brief 看门狗演示例程
 *
 * - 操作步骤:
 *   1. 需要在aw_prj_params.h中使能
 *      - 对应平台的内部看门狗(AW_DEV_xxx_WDT) 或 外部看门狗(AW_DEV_GPIO_WDT)；
 *      - 对应平台的串口宏
 *      - AW_COM_CONSOLE
 *   2. 这里演示的是AWorks软件看门狗用法，实际上只要使能看门狗设备宏，硬件看门狗系统会定时
 *      喂，但如果软件看门狗没有及时喂，那么系统就会停止喂硬件看门狗，从而导致系统被复位。
 *   3. 调试WDT程序时，不能打断点调试，否则会因为系统没有喂狗，导致复位。
 *   4. 禁能其它地方对LED的操作
 *   5. 程序需下载到flash之后，拔掉开发板WDT跳线帽，重新启动板子
 *
 * - 实验现象:
 *   1. 程序启动，串口打印"Add WDT device!"
 *   2. 每喂狗一次，打印"WDT Feed!"
 *   3. 若喂狗超时，程序复位，重新打印出"Add WDT device!".
 *
 * - 备注：
 *   1. 外部看门狗使用后，为避免硬件复位，请屏蔽掉外部看门狗的宏。
 *   2. WDT复位后，内部boot重启只能使用OCRAM启动。
 *   3. 在程序启动后bsp中的代码将内部RAM空间全部改为了DTCM。WDT复位时，内部RAM boot只能
 *      使用OCRAM启动，由于WDT复位时不会重新复位RAM分配寄存器， 导致内部boot无法正常复位成功，
 *      若要其正常复位，可修改链接文件和RAM配置，将RAM分配为OCRAM和DTCM。
 *
 * \par 源代码
 * \snippet demo_wdt.c src_wdt
 *
 * \internal
 * \par History
 * - 1.00 18-06-25  lqy, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_wdt 看门狗
 * \copydoc demo_wdt.c
 */

/** [src_wdt] */
#include "aworks.h"     /* 此头文件必须被首先包含 */

/* 本程序用到了以下服务 */
#include "aw_delay.h"
#include "aw_wdt.h"
#include "aw_vdebug.h"
#include "aw_led.h"
#include "aw_demo_config.h"

#define WDT_TIMEOUT_MS     1000   /* 看门狗时间 */
#define FEED_TIME_MS       600    /* 喂狗周期 */

#define LED      DM_LED

/******************************************************************************/
static struct awbl_wdt     wdt_dev;

void demo_wdt_entry (void)
{
    aw_err_t    ret     = AW_OK;
    int         count   = 10;

    /* 设置看门狗时间 */
    ret = aw_wdt_add(&wdt_dev, WDT_TIMEOUT_MS);
    if (ret != AW_OK) {
        AW_ERRF(("*ERR: WDT add error %d\n", ret));
        return;
    }

    AW_INFOF(("Add WDT device!\r\n"));
    AW_INFOF(("WDT timeout is %d ms!\r\n", WDT_TIMEOUT_MS));

    /* 喂狗一定次数，以证明喂狗有效 */
    /* LED 间隔闪烁 */
    while (count) {
        aw_wdt_feed(&wdt_dev);  /* 喂狗 */
        aw_mdelay(FEED_TIME_MS);
        aw_led_toggle(LED);
        AW_INFOF(("WDT Feed!\r\n"));
        count--;
    }
    aw_wdt_feed(&wdt_dev);      /* 喂狗 */

    /* 现在停止喂狗，系统应该复位 */
    AW_INFOF(("now stop feed watch dog!\r\n"));
    AW_INFOF(("the system will reset!\r\n"));

    while (1) {
        AW_INFOF(("the system is still alive!\r\n"));
        aw_task_delay(1000);
    }
}

/** [src_wdt] */

/* end of file */
