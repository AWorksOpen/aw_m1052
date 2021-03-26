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
 * \brief ��׼C�⺯��ʹ�����̣�ʱ�䣩
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��AW_COM_CONSOLE�͵��Դ��ڶ�Ӧ�ĺꡣ
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������
 *   1. ����ѭ��5�δ�ӡʱ����Ϣ��
 *
 * \par Դ����
 * \snippet demo_libc_time.c src_libc_time
 *
 * \internal
 * \par Modification history
 * - 1.00 17-09-07  mkr, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_libc_time ʱ��(��׼C��)
 * \copydoc demo_libc_time.c
 */

/** [src_libc_time] */
#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_time.h"
#include <time.h>

/**
 * \brief ����ϵͳ����ʱ��
 *
 * \return ��
 */
aw_local void __calendar_time_set (void)
{
    aw_tm_t       tm;            /* ϸ��ʱ�� */
    aw_timespec_t timespec;      /* ����ʱ�� */

    tm.tm_sec    = 6;
    tm.tm_min    = 6;
    tm.tm_hour   = 6;
    tm.tm_mday   = 7;
    tm.tm_mon    = 9 - 1;        /* ʵ���·�Ҫ��1 */
    tm.tm_year   = 2017 - 1900;  /* ��1900������ */
    tm.tm_isdst  = -1;           /* ����ʱ������ */

    aw_tm_to_time(&tm, &timespec.tv_sec);    /* ת��Ϊ����ʱ�� */
    timespec.tv_nsec = 0;                    /* ���벿����0 */

    aw_timespec_set(&timespec);              /* ��������ʱ�� */
}

/******************************************************************************/
/**
 * \brief libc time �ӿ�ʾ����ں���
 *
 * \return ��
 */
void demo_libc_time_entry (void)
{
    uint8_t  i;
    char     str[80];
    char     time_str[] = "Tue May 21 13:46:22 1991\n";
    time_t   cal_time;   /* ����ʱ�� */
    struct   tm bd_time; /* ϸ��ʱ�� */

    aw_kprintf("\r\nLibc time interface testing...\r\n");

    __calendar_time_set();

    aw_kprintf("CLOCKS_PER_SEC : %d\n", CLOCKS_PER_SEC);

    for (i = 0; i < 5; i++) {

        /* ��ȡ��ǰ����ʱ�� */
        cal_time = time(NULL);
        aw_kprintf("The current calendar time : %d\r\n", cal_time);

        /* ������ʱ��ת���ɱ�׼�ַ�����ʽ */
        aw_kprintf("is equal to \r\n%s\r\n", ctime_r(&cal_time, time_str));

        /* ������ʱ��ת����ϸ��ʱ�� */
        localtime_r(&cal_time, &bd_time);

        /* ��ϸ��ʱ��ת���ɱ�׼�ַ�����ʽ */
        aw_kprintf("broken-down time to string: \r\n%s\r\n",
                   asctime_r(&bd_time, time_str));

        /* ��ϸ��ʱ��ת����ָ����ʽ��� */
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
