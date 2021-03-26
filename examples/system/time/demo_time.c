/*******************************************************************************
*                                 AWorks
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
 * \brief time 例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - AW_COM_CONSOLE
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象：
 *   1. 串口每隔 1s + 2ms， 打印一次日历时间和AW_FOREVER中代码执行耗时时间(2ms)等相关信息。
 *
 * \par 源代码
 * \snippet demo_time.c src_time
 */

/**
 * \addtogroup demo_if_time 日历时间
 * \copydoc demo_time.c
 */

/** [src_time] */
#include "aworks.h"
#include "aw_time.h"
#include "aw_vdebug.h"
#include "aw_task.h"
#include "aw_delay.h"

#include <string.h>

/**
 * \brief timer例程
 */
void demo_time_entry (void)
{
    aw_err_t  ret = AW_OK;

    aw_tm_t       tm;            /* 细分时间 */
    aw_timespec_t timespec;      /* 日历时间 */

    /* 设置当前时间 */
    tm.tm_sec    = 30;           /* 设置秒 */
    tm.tm_min    = 46;           /* 设置分 */
    tm.tm_hour   = 14;           /* 设置小时 */
    tm.tm_mday   = 8;            /* 设置日 */
    tm.tm_mon    = 9 - 1;        /* 设置月，实际月份要减一 */
    tm.tm_year   = 2017 - 1900;  /* 设置年，距1900的年数 */
    tm.tm_isdst  = -1;           /* 夏令时不可用 */

    ret = aw_tm_set(&tm);        /* 设置日历时间 */
    if (ret != AW_OK) {
        AW_ERRF(("aw_tm_set err: %d\n", ret));
    }
    memset(&tm, 0, sizeof(tm));

    ret = aw_tm_get(&tm);        /* 得到当前时间 */
    if (ret != AW_OK) {
        AW_ERRF(("aw_tm_get err: %d\n", ret));
    }

    /* 打印时间 */
    aw_kprintf("%d-%d-%d %d:%d:%d wday:%d yday:%d\n",
               tm.tm_year + 1900,
               tm.tm_mon + 1,
               tm.tm_mday,
               tm.tm_hour,
               tm.tm_min,
               tm.tm_sec,
               tm.tm_wday,
               tm.tm_yday);

    /* 得到日历时间 */
    ret = aw_timespec_get(&timespec);
    if (ret != AW_OK) {
        AW_ERRF(("aw_timespec_get err: %d\n", ret));
    }
    aw_kprintf("%s", ctime((const time_t *)&timespec.tv_sec));

    /* 设置当前时间 */
    tm.tm_sec    = 0;            /* 设置秒 */
    tm.tm_min    = 0;            /* 设置分 */
    tm.tm_hour   = 15;           /* 设置小时 */
    tm.tm_mday   = 8;            /* 设置日 */
    tm.tm_mon    = 9 - 1;        /* 设置月，实际月份要减一 */
    tm.tm_year   = 2017 - 1900;  /* 设置年，距1900的年数 */
    tm.tm_isdst  = -1;           /* 夏令时不可用 */

    aw_tm_to_time(&tm, &timespec.tv_sec);   /* 转换为日历时间 */
    timespec.tv_nsec = 0;                   /* 纳秒部分置0 */

    ret = aw_timespec_set(&timespec);       /* 设置日历时间 */
    if (ret != AW_OK) {
        AW_ERRF(("aw_timespec_set err: %d\n", ret));
    }
    memset(&timespec, 0, sizeof(timespec));

    AW_FOREVER {

        ret = aw_timespec_get(&timespec);       /* 得到日历时间 */
        if (ret != AW_OK) {
            AW_ERRF(("aw_timespec_get err: %d\n", ret));
        }
        aw_kprintf("%s", ctime((const time_t *)&timespec.tv_sec));  /* 打印日历时间 */
        aw_kprintf(" %d ms \n", timespec.tv_nsec / 1000000);        /* 打印AW_FOREVER中程序消耗时间(毫秒) */

        aw_mdelay(1000);    /* 延时 1s */
    }
}

/** [src_time] */

/* end of file */
