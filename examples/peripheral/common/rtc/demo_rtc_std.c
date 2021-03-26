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
 * \brief RTC��ʾ���̣�ʹ��aw_rtc_time_set��aw_rtc_time_get�ӿڣ�
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
 *   1. �趨�ó�ʼʱ�䣬ͨ�����ڿ��Թ۲�ʱ������������Ϣ��1�����1�����ݡ�
 *
 * \par Դ����
 * \snippet demo_rtc_std.c src_rtc_std
 *
 * \internal
 * \par Modification History
 * - 1.00 16-03-24  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_std_rtc_std ʵʱʱ��
 * \copydoc demo_rtc_std.c
 */

/** [src_rtc_std] */
#include "aworks.h"     /* ��ͷ�ļ��������ȱ����� */

#include "aw_delay.h"
#include "aw_rtc.h"
#include "aw_vdebug.h"
#include "aw_prj_params.h"
#include "aw_demo_config.h"

#define RTC_ID    DE_RTC_ID0
//#define RTC_ID    DE_RTC_ID1

/**
 * \brief RTC��ʾ�������
 * \return ��
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

    ret = aw_rtc_time_set(RTC_ID, &tm);    /* ������ʼʱ�� */
    if (ret != AW_OK) {
        AW_ERRF(("RTC set error: %d\n", ret));
    }

    while (1) {

        ret = aw_rtc_time_get (RTC_ID, &tm);    /* �õ�ϸ��ʱ�� */

        if (ret != AW_OK) {
            AW_INFOF(("rtc ����ʧ�� %d \r\n", ret));
        } else {

            /* ��ӡϸ��ʱ�� */
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
