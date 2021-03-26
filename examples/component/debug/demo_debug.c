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
 * \brief debug���Դ�ӡ�������
 *
 * - ʵ�鲽�裺
 *   1. ��Ҫ�� aw_prj_params.h �д�AW_COM_CONSOLE�͵��Դ��ڶ�Ӧ�ĺꣻ
 *   2. ���ڴ�ӡ������ aw_prj_params.h ��:
 *      #define AW_CFG_CONSOLE_COMID        COM0
 *      #define AW_CFG_CONSOLE_BAUD_RATE    115200
 *   3. ʹ�ô����߷ֱ�Ϳ�����ĵ��Դ���RX1��TX1����, �򿪴��ڵ�������,
 *      ���ò�����Ϊ115200, 1��ֹͣλ, ��У��, �����ء�
 *
 * - ʵ������
 *   1. �鿴���ڴ�ӡ
 *
 * - ��ע��
 *   1. ��ӡ��ʹ��ǰ��Ҫ����꣬���£�
 *      AW_INFOF  ->  AW_VDEBUG_INFO
 *      AW_WARNF  ->  AW_VDEBUG_WARN
 *      AW_ERRF   ->  AW_VDEBUG_ERROR
 *   ��Щ�����ڹ���������������
 *
 * \par Դ����
 * \snippet demo_debug.c src_debug
 *
 * \internal
 * \par Modification History
 * - 1.00 17-09-06  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_debug ���Դ�ӡ���
 * \copydoc demo_debug.c
 */

/** [src_debug] */
#include "aworks.h"
#include "aw_vdebug.h"
#include "stdio.h"

aw_local void __my_printf (const char *format, ...)
{
    char  buf[128];

    va_list  vlist;

    va_start(vlist, format);
    aw_vsnprintf(buf, sizeof(buf), format, vlist);
    va_end(vlist);

    aw_kprintf("%s", buf);
}

/**
 * \brief debug ���Դ�ӡ����������
 * \return ��
 */
void demo_debug_entry (void)
{
    char  buf[64] = {0};

    AW_INFOF(("\r\n*Info: Hello World!\r\n"));

    AW_WARNF(("\r\n*Warn: Hello World!\r\n"));

    AW_ERRF(("\r\n*ERR: Hello World!\r\n"));

    aw_kprintf("\r\naw_kprintf: Hello World!\r\n");

    aw_kprintf("\r\n%s, %d, 0x%x, %c, %f, %g\r\n",
               "Hello World",
               123,
               0x12345678,
               'a',
               1.23456789,
               0.012457);

    __my_printf("\r\n%s, %d, 0x%x, %c, %f, %g\r\n",
                "Hello World",
                123,
                0x12345678,
                'a',
                1.23456789,
                0.012457);

    aw_snprintf(buf,
                sizeof(buf),
                "\r\n%s, %d, 0x%x, %c, %f, %g\r\n",
                "Hello World",
                123,
                0x12345678,
                'a',
                1.23456789,
                0.012457);

    aw_kprintf("%s\r\n", buf);
}

/** [src_debug] */

/* end of file */
