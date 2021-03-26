/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief 标准C库函数使用例程（时间）
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能AW_COM_CONSOLE和调试串口对应的宏。
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象：
 *   1. 串口循环5次打印时间信息。
 *
 * \par 源代码
 * \snippet demo_libc_time.c src_libc_time
 *
 * \internal
 * \par Modification history
 * - 1.00 17-09-07  mkr, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_libc_time 时间(标准C库)
 * \copydoc demo_libc_time.c
 */

/** [src_libc_time] */
#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_time.h"
#include <time.h>

/**
 * \brief 设置系统日历时间
 *
 * \return 无
 */
aw_local void __calendar_time_set (void)
{
    aw_tm_t       tm;            /* 细分时间 */
    aw_timespec_t timespec;      /* 日历时间 */

    tm.tm_sec    = 6;
    tm.tm_min    = 6;
    tm.tm_hour   = 6;
    tm.tm_mday   = 7;
    tm.tm_mon    = 9 - 1;        /* 实际月份要减1 */
    tm.tm_year   = 2017 - 1900;  /* 距1900的年数 */
    tm.tm_isdst  = -1;           /* 夏令时不可用 */

    aw_tm_to_time(&tm, &timespec.tv_sec);    /* 转换为日历时间 */
    timespec.tv_nsec = 0;                    /* 纳秒部分置0 */

    aw_timespec_set(&timespec);              /* 设置日历时间 */
}

/******************************************************************************/
/**
 * \brief libc time 接口示例入口函数
 *
 * \return 无
 */
void demo_libc_time_entry (void)
{
    uint8_t  i;
    char     str[80];
    char     time_str[] = "Tue May 21 13:46:22 1991\n";
    time_t   cal_time;   /* 日历时间 */
    struct   tm bd_time; /* 细分时间 */

    aw_kprintf("\r\nLibc time interface testing...\r\n");

    __calendar_time_set();

    aw_kprintf("CLOCKS_PER_SEC : %d\n", CLOCKS_PER_SEC);

    for (i = 0; i < 5; i++) {

        /* 获取当前日历时间 */
        cal_time = time(NULL);
        aw_kprintf("The current calendar time : %d\r\n", cal_time);

        /* 将日历时间转换成标准字符串形式 */
        aw_kprintf("is equal to \r\n%s\r\n", ctime_r(&cal_time, time_str));

        /* 将日历时间转换成细分时间 */
        localtime_r(&cal_time, &bd_time);

        /* 将细分时间转换成标准字符串形式 */
        aw_kprintf("broken-down time to string: \r\n%s\r\n",
                   asctime_r(&bd_time, time_str));

        /* 将细分时间转换成指定格式输出 */
        strftime(str,
                 sizeof(str),
                 "Date:\r\n%Y-%m-%d\r\nTime:\r\n%I:%M:%S\r\n",
                 &bd_time);
        aw_kprintf("%s\n", str);

        aw_mdelay(1000);
    }
}

/** [src_libc_time] */

/* end of file */
