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
 * \brief ͨ��shell�������RTC
 *
 * - �������裺
 *   1. ��Ҫ��aw_prj_params.hͷ�ļ���ʹ��
 *      - ��Ӧƽ̨�Ͷ�Ӧ��ŵ�I2C
 *      - AW_DEV_EXTEND_PCF85063
 *   2. ��shell�������".test"��Ŀ¼������"rtc"��ȡ��ǰʱ������ڣ�
 *   3. ����"rtc -d ����"�޸����ڣ��磺rtc -d 2018-7-11��
 *   4. ����"rtc -t ʱ��"�޸�ʱ�䣬�磺rtc -t 12:00:00
 *
 * - ��ע��
 *   1. ����֮���ö��߷ָʱ��֮����ð�ţ�Ӣ���ַ����ָ
 *   2. ���ں�ʱ�����һ�����ã��磺rtc -d 2018-7-11 -t 12:00:00
 *
 * \par Դ����
 * \snippet app_rtc.c app_rtc
 *
 * \internal
 * \par Modification History
 * - 1.00 18-07-11  sdq, first implementation.
 * \endinternal
 */

/** [src_app_rtc] */
#include "aworks.h"                     /* ��ͷ�ļ����뱻���Ȱ��� */
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

    /* ��ȡһ��RTCʱ�䣬ֱ�Ӷ�ȡRTC�豸 */
    ret = aw_rtc_time_get(DE_RTC_ID0, &tm);
    if (ret != AW_OK) {
        aw_kprintf("RTC get failed %d.\n", ret);
        return AW_OK;
    }

    while (1) {
        if (aw_shell_args_get(p_user, "-d", 1) == AW_OK) {
            /* ��ȡ��Ҫ���õ����� */
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
            /* ��ȡ��Ҫ���õ�ʱ�� */
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
        /* �������Ϣ��Ҫ���ã������RTC���� */
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
