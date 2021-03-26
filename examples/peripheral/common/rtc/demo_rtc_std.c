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
 * \brief RTC演示例程（使用aw_rtc_time_set和aw_rtc_time_get接口）
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - RTC外设宏
 *      - AW_COM_CONSOLE
 *      注意：如果初始化失败，查看配置文件是否是引脚配置冲突，如果是，在aw_prj_params.h中关闭不使用的宏
 *   2. 将板子中的DURX、DUTX通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *   3. 由于系统中RTC为单实例，因而只能开启一个RTC外设
 *
 * - 实验现象：
 *   1. 设定好初始时间，通过串口可以观察时间和日期相关信息，1秒更新1次数据。
 *
 * \par 源代码
 * \snippet demo_rtc_std.c src_rtc_std
 *
 * \internal
 * \par Modification History
 * - 1.00 16-03-24  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_std_rtc_std 实时时钟
 * \copydoc demo_rtc_std.c
 */

/** [src_rtc_std] */
#include "aworks.h"     /* 该头文件必须首先被包含 */

#include "aw_delay.h"
#include "aw_rtc.h"
#include "aw_vdebug.h"
#include "aw_prj_params.h"
#include "aw_demo_config.h"

#define RTC_ID    DE_RTC_ID0
//#define RTC_ID    DE_RTC_ID1

/**
 * \brief RTC演示例程入口
 * \return 无
 */
aw_local void* __task_handle (void *parg)
{
    aw_err_t ret = AW_OK;
    aw_tm_t tm;

    tm.tm_year  = 2018 - 1900;   /* year 2018    */
    tm.tm_mon   = 11 - 1;        /* month 11     */
    tm.tm_mday  = 20;            /* day 20       */
    tm.tm_hour  = 9;             /* hour 9       */
    tm.tm_min   = 40;            /* minute 40    */
    tm.tm_sec   = 50;            /* second1 50   */

    ret = aw_rtc_time_set(RTC_ID, &tm);    /* 设置起始时间 */
    if (ret != AW_OK) {
        AW_ERRF(("RTC set error: %d\n", ret));
    }

    while (1) {

        ret = aw_rtc_time_get (RTC_ID, &tm);    /* 得到细分时间 */

        if (ret != AW_OK) {
            AW_INFOF(("rtc 操作失败 %d \r\n", ret));
        } else {

            /* 打印细分时间 */
            AW_INFOF(("current time: %s\n",
                      asctime((const struct tm *)&tm)));
        }

        aw_mdelay(1000);
    }

    return 0;
}

/******************************************************************************/
void demo_rtc_std (void)
{
    aw_task_id_t    tsk;

    tsk = aw_task_create("RTC std demo",
                         12,
                         2048,
                         __task_handle,
                         (void *)NULL);
    if (tsk == NULL) {
        aw_kprintf("RTC std demo task create failed\r\n");
        return;
    }

    if (aw_task_startup(tsk) != AW_OK) {
        aw_task_delete(tsk);
    }
}

/** [src_rtc_std] */

/* end of file */
