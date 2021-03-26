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
 * \brief RTC��ʾ���̣�ͨ��shell��������ʱ��
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - RTC�����
 *      - AW_COM_CONSOLE
 *      ע�⣺�����ʼ��ʧ�ܣ��鿴�����ļ��Ƿ����������ó�ͻ������ǣ���aw_prj_params.h�йرղ�ʹ�õĺ�
 *   2. �������е�DURX��DUTXͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *   3. ����ϵͳ��RTCΪ��ʵ�������ֻ�ܿ���һ��RTC����
 *
 * - ʵ������
 *   1. ִ��shell����"test_rtc":��������rtc��ʱ5��
 *   2. ִ��shell����"test_rtc 2018-11-13 15:05:00":����ָ��ʱ�䣬��������rtc��ʱ5�Ρ�
 *
 * \par Դ����
 * \snippet demo_rtc_time_set.c src_rtc_time_set
 *
 * \internal
 * \par Modification History
 * - 1.00 16-03-24  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_std_rtc_time_set ʵʱʱ��(Shell����)
 * \copydoc demo_rtc_time_set.c
 */

/** [src_rtc_time_set] */
#include "aworks.h"     /* ��ͷ�ļ��������ȱ����� */

#include "aw_delay.h"
#include "aw_rtc.h"
#include "aw_vdebug.h"
#include "aw_prj_params.h"
#include "aw_shell.h"
#include "stdio.h"
#include "string.h"
#include "aw_demo_config.h"

#define RTC_ID    DE_RTC_ID0
//#define RTC_ID    DE_RTC_ID1

static aw_tm_t      __g_old_time = {0};

/**
 * \brief �ж�����ʱ���Ƿ���Ч
 *
 * \param[in] hour      ʱ
 * \param[in] minute    ��
 * \param[in] second    ��
 *
 * \return �жϽ��
 */
aw_local int __is_time_invalid (int hour, int minute, int second)
{
    int ret = -AW_EINVAL;

    if( hour < 0 || hour > 23) {
        goto cleanup;
    }
    if (minute < 0 || minute > 59) {
        goto cleanup;
    }
    if (second < 0 || second > 59) {
        goto cleanup;
    }

    ret = 0;

cleanup:
    return ret;
}


/**
 * \brief �ж����������Ƿ���Ч
 *
 * \param[in] year      ��
 * \param[in] month     ��
 * \param[in] date      ��
 *
 * \return �жϽ��
 */
aw_local int __is_date_invalid (int year, int month, int date)
{
    static const int    month_days[12] = {31,29,31,30,31,30,31,31,30,31,30,31};
    int                 ret = -AW_EINVAL;

    if (year <2000 || year >2050) {
        goto __cleanup;
    }
    if (month <=0 || month > 12) {
        goto __cleanup;
    }
    if (date <= 0 || date > month_days[month-1]) {
        goto __cleanup;
    }

    /* ��������� */
    if (((year%4)==0 && (year%100)!=0 ) || (year%400) == 0) {
        ;
    } else {
        if (2 == month) {
            if (date >= 29) {
                goto __cleanup;
            }
        }
    }

    ret = 0;

__cleanup:
    return ret;
}

static void __rtc_time_run (void)
{
    int                 i;
    aw_tm_t             tm;
    aw_time_t           t1,t2;
    int                 ret;
    char                time_str[30];

    ret = aw_tm_to_time(&__g_old_time, &t1);
    if (ret != AW_OK) {
        goto __exit ;
    }

    for (i = 0; i < 5; i++) {
        ret = aw_rtc_time_get(RTC_ID, &tm);
        if (ret != AW_OK) {
            goto __exit ;
        }

        ret = aw_tm_to_time(&tm, &t2);

        /* ���������ʱ����Ϣת�����ַ�����ʽ */
        asctime_r((const struct tm *)&tm, time_str);
        aw_kprintf("%s", time_str);
        if (ret != AW_OK) {
            goto __exit ;
        }

        aw_mdelay(1000);
    }

__exit:
    return ;
}

aw_local aw_const char rtc_help_info[] = {
    "Command:test_rtc [date] [time]\n\n"
    "Description:Set RTC time\n\n"
    "Examples:\n"
        "    1.test_rtc\n"
        "    2.test_rtc 2018-1-19 09:00:00\n"
};

int __rtc_cmd (int argc, char *argv[], struct aw_shell_user *sh)
{
    const char             *sz_date = NULL;
    const char             *sz_time = NULL;
    int                     ret = 0;
    int                     t1,t2,t3;

    if (argc == 1 && !strcmp(argv[0],"--help")) {
        aw_shell_printf(sh, rtc_help_info);
        return AW_OK;
    }

    if (argc == 0) {
        goto __run;
    }

    ret = aw_rtc_time_get(RTC_ID, &__g_old_time);
    if (ret != AW_OK) {
        aw_shell_printf(sh, "get old time failed\n");
        goto __cleanup ;
    }

    if (argc >= 1) {
        sz_date = argv[0];
    }
    if (argc == 2) {
        sz_time = argv[1];
    } else {
        goto __set_date;
    }

    ret = sscanf(sz_time, "%d:%d:%d", &t1, &t2, &t3);
    if (ret != 3) {
        aw_shell_printf(sh, "invalid time string\n");
        ret = -AW_EINVAL;
        goto __cleanup;
    }
    ret = __is_time_invalid(t1, t2, t3);
    if (ret != 0) {
        aw_shell_printf(sh, "invalid time \n");
        ret = -AW_EINVAL;
        goto __cleanup;
    }

    __g_old_time.tm_hour = t1;
    __g_old_time.tm_min  = t2;
    __g_old_time.tm_sec  = t3;

__set_date:

    ret = sscanf(sz_date, "%d-%d-%d", &t1, &t2, &t3);
    if (ret != 3) {
        aw_shell_printf(sh, "invalid date string\n");
        ret = -AW_EINVAL;
        goto __cleanup;
    }
    ret = __is_date_invalid(t1, t2, t3);
    if (ret != 0) {
        aw_shell_printf(sh, "invalid date \n");
        ret = -AW_EINVAL;
        goto __cleanup;
    }
    __g_old_time.tm_year = t1 -1900;
    __g_old_time.tm_mon  = t2 -1;
    __g_old_time.tm_mday = t3;

    /* ����ʱ�� */
    ret = aw_rtc_time_set(RTC_ID, &__g_old_time);
    if (ret != 0) {
        aw_shell_printf(sh, "invalid aw_rtc_time_set failed \n");
        ret = -AW_EINVAL;
        goto __cleanup;
    }

__run:
    __rtc_time_run();

__cleanup:
    return ret;
}

static const struct aw_shell_cmd __g_rtc_test_cmds[] = {
    {__rtc_cmd,  "test_rtc",  "Set RTC time"},
};

/**
 * \brief RTC��ʾ�������
 * \return ��
 */
void demo_rtc_time_set_entry (void)
{
    static struct aw_shell_cmd_list cl;
    (void)AW_SHELL_REGISTER_CMDS(&cl, __g_rtc_test_cmds);
}

/** [src_rtc_time_set] */

/* end of file */
