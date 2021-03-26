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
 * \brief time ����
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_COM_CONSOLE
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������
 *   1. ����ÿ�� 1s + 2ms�� ��ӡһ������ʱ���AW_FOREVER�д���ִ�к�ʱʱ��(2ms)�������Ϣ��
 *
 * \par Դ����
 * \snippet demo_time.c src_time
 */

/**
 * \addtogroup demo_if_time ����ʱ��
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
 * \brief timer����
 */
void demo_time_entry (void)
{
    aw_err_t  ret = AW_OK;

    aw_tm_t       tm;            /* ϸ��ʱ�� */
    aw_timespec_t timespec;      /* ����ʱ�� */

    /* ���õ�ǰʱ�� */
    tm.tm_sec    = 30;           /* ������ */
    tm.tm_min    = 46;           /* ���÷� */
    tm.tm_hour   = 14;           /* ����Сʱ */
    tm.tm_mday   = 8;            /* ������ */
    tm.tm_mon    = 9 - 1;        /* �����£�ʵ���·�Ҫ��һ */
    tm.tm_year   = 2017 - 1900;  /* �����꣬��1900������ */
    tm.tm_isdst  = -1;           /* ����ʱ������ */

    ret = aw_tm_set(&tm);        /* ��������ʱ�� */
    if (ret != AW_OK) {
        AW_ERRF(("aw_tm_set err: %d\n", ret));
    }
    memset(&tm, 0, sizeof(tm));

    ret = aw_tm_get(&tm);        /* �õ���ǰʱ�� */
    if (ret != AW_OK) {
        AW_ERRF(("aw_tm_get err: %d\n", ret));
    }

    /* ��ӡʱ�� */
    aw_kprintf("%d-%d-%d %d:%d:%d wday:%d yday:%d\n",
               tm.tm_year + 1900,
               tm.tm_mon + 1,
               tm.tm_mday,
               tm.tm_hour,
               tm.tm_min,
               tm.tm_sec,
               tm.tm_wday,
               tm.tm_yday);

    /* �õ�����ʱ�� */
    ret = aw_timespec_get(&timespec);
    if (ret != AW_OK) {
        AW_ERRF(("aw_timespec_get err: %d\n", ret));
    }
    aw_kprintf("%s", ctime((const time_t *)&timespec.tv_sec));

    /* ���õ�ǰʱ�� */
    tm.tm_sec    = 0;            /* ������ */
    tm.tm_min    = 0;            /* ���÷� */
    tm.tm_hour   = 15;           /* ����Сʱ */
    tm.tm_mday   = 8;            /* ������ */
    tm.tm_mon    = 9 - 1;        /* �����£�ʵ���·�Ҫ��һ */
    tm.tm_year   = 2017 - 1900;  /* �����꣬��1900������ */
    tm.tm_isdst  = -1;           /* ����ʱ������ */

    aw_tm_to_time(&tm, &timespec.tv_sec);   /* ת��Ϊ����ʱ�� */
    timespec.tv_nsec = 0;                   /* ���벿����0 */

    ret = aw_timespec_set(&timespec);       /* ��������ʱ�� */
    if (ret != AW_OK) {
        AW_ERRF(("aw_timespec_set err: %d\n", ret));
    }
    memset(&timespec, 0, sizeof(timespec));

    AW_FOREVER {

        ret = aw_timespec_get(&timespec);       /* �õ�����ʱ�� */
        if (ret != AW_OK) {
            AW_ERRF(("aw_timespec_get err: %d\n", ret));
        }
        aw_kprintf("%s", ctime((const time_t *)&timespec.tv_sec));  /* ��ӡ����ʱ�� */
        aw_kprintf(" %d ms \n", timespec.tv_nsec / 1000000);        /* ��ӡAW_FOREVER�г�������ʱ��(����) */

        aw_mdelay(1000);    /* ��ʱ 1s */
    }
}

/** [src_time] */

/* end of file */
