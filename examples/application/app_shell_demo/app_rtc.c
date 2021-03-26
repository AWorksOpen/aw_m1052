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
 * \brief 通过shell命令操作RTC
 *
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能
 *      - 对应平台和对应编号的I2C
 *      - AW_DEV_EXTEND_PCF85063
 *   2. 在shell界面进入".test"子目录后输入"rtc"获取当前时间和日期；
 *   3. 输入"rtc -d 日期"修改日期，如：rtc -d 2018-7-11；
 *   4. 输入"rtc -t 时间"修改时间，如：rtc -t 12:00:00
 *
 * - 备注：
 *   1. 日期之间用断线分割，时间之间用冒号（英文字符）分割；
 *   2. 日期和时间可以一起设置，如：rtc -d 2018-7-11 -t 12:00:00
 *
 * \par 源代码
 * \snippet app_rtc.c app_rtc
 *
 * \internal
 * \par Modification History
 * - 1.00 18-07-11  sdq, first implementation.
 * \endinternal
 */

/** [src_app_rtc] */
#include "aworks.h"                     /* 此头文件必须被首先包含 */
#include "aw_shell.h"
#include "aw_rtc.h"
#include "aw_vdebug.h"
#include "app_config.h"
#include "string.h"
#include "stdlib.h"

#ifdef APP_RTC
/******************************************************************************/
int app_rtc (int argc, char **argv, struct aw_shell_user *p_user)
{
    aw_tm_t   tm;
    aw_bool_t    set = AW_FALSE;
    char     *arg;
    aw_err_t  ret;

    /* 获取一次RTC时间，直接读取RTC设备 */
    ret = aw_rtc_time_get(DE_RTC_ID0, &tm);
    if (ret != AW_OK) {
        aw_kprintf("RTC get failed %d.\n", ret);
        return AW_OK;
    }

    while (1) {
        if (aw_shell_args_get(p_user, "-d", 1) == AW_OK) {
            /* 获取需要设置的日期 */
            arg = aw_shell_str_arg(p_user, 1);
            if (arg) {
                tm.tm_year = atoi(arg) - 1900;
                arg = strstr(arg, "-");
                if (arg) {
                    arg++;
                    tm.tm_mon = atoi(arg) - 1;
                    arg = strstr(arg, "-");
                    if (arg) {
                        arg++;
                        tm.tm_mday = atoi(arg);
                    }
                }
            }
            set = AW_TRUE;
        } else if (aw_shell_args_get(p_user, "-t", 1) == AW_OK) {
            /* 获取需要设置的时间 */
            arg = aw_shell_str_arg(p_user, 1);
            if (arg) {
                tm.tm_hour = atoi(arg);
                arg = strstr(arg, ":");
                if (arg) {
                    arg++;
                    tm.tm_min = atoi(arg);
                    arg = strstr(arg, ":");
                    if (arg) {
                        arg++;
                        tm.tm_sec = atoi(arg);
                    }
                }
            }
            set = AW_TRUE;
        } else {
            break;
        }
    }

    if (set) {
        /* 如果有信息需要设置，则进行RTC更新 */
        ret = aw_rtc_time_set(DE_RTC_ID0, &tm);
        if (ret != AW_OK) {
            aw_kprintf("RTC set failed %d.\n", ret);
            return AW_OK;
        }
    }

    aw_kprintf("date: %d-%d-%d\r\n"
               "time: %02d:%02d:%02d\r\n",
               tm.tm_year + 1900,
               tm.tm_mon + 1,
               tm.tm_mday,
               tm.tm_hour,
               tm.tm_min,
               tm.tm_sec);
    return AW_OK;
}

/** [src_app_rtc] */
#endif /* APP_RTC */
