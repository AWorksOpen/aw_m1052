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
 * \brief debug调试打印输出例程
 *
 * - 实验步骤：
 *   1. 需要在 aw_prj_params.h 中打开AW_COM_CONSOLE和调试串口对应的宏；
 *   2. 串口打印配置在 aw_prj_params.h 中:
 *      #define AW_CFG_CONSOLE_COMID        COM0
 *      #define AW_CFG_CONSOLE_BAUD_RATE    115200
 *   3. 使用串口线分别和开发板的调试串口RX1、TX1相连, 打开串口调试助手,
 *      设置波特率为115200, 1个停止位, 无校验, 无流控。
 *
 * - 实验现象：
 *   1. 查看串口打印
 *
 * - 备注：
 *   1. 打印宏使用前需要定义宏，如下：
 *      AW_INFOF  ->  AW_VDEBUG_INFO
 *      AW_WARNF  ->  AW_VDEBUG_WARN
 *      AW_ERRF   ->  AW_VDEBUG_ERROR
 *   这些宏已在工程配置中配置了
 *
 * \par 源代码
 * \snippet demo_debug.c src_debug
 *
 * \internal
 * \par Modification History
 * - 1.00 17-09-06  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_debug 调试打印输出
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
 * \brief debug 调试打印输出例程入口
 * \return 无
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
