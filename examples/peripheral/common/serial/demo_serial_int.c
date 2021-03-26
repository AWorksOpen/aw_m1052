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
 * \brief 串口演示例程（中断模式）
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - 对应平台的串口宏（如：AW_DEV_xxx_LPUARTx）
 *      - AW_COM_CONSOLE
 *   2. 使用本例程时，若开启shell 宏 (AW_COM_SHELL_SERIAL)，则本例程的
 *      TEST_SERIAL_NUM 所使用不能为 COM0 (因为shell使用COM0，会读取串口数据)
 *   3. 将板子中的RX、TX通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象：
 *   1. 串口进入中断读写模式（非阻塞），上位机发送数据，串口将接收到的数据返回
 *      给上位机。
 *
 * - 备注：
 *   1. COMx与串口的对应关系在aw_prj_params.h文件中查看。
 *
 * \par 源代码
 * \snippet demo_serial_int.c src_serial_int
 *
 * \internal
 * \par Modification History
 * - 1.00 16-03-24  cod, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_serial_int 串口(中断模式)
 * \copydoc demo_serial_int.c
 */

/** [src_serial_int] */
#include "aworks.h"

#include "aw_delay.h"
#include "aw_serial.h"
#include "aw_ioctl.h"

#define  TEST_SERIAL_NUM   COM1

/**
 * \brief 串口 demo
 * \return 无
 */
aw_local void* __task_handle (void *parg)
{
    char                      buf[32];
    int                       len = 0;
    struct  aw_serial_dcb     dcb;
    aw_err_t                  ret;
    struct aw_serial_timeout  timeout;

    /* 获取dcb的默认配置 */
    ret = aw_serial_dcb_get(TEST_SERIAL_NUM, &dcb);
    if (AW_OK != ret) {
        return 0;
    }

    /* 配置串口波特率为115200，8个数据位，1位停止位，无奇偶校验 */
    dcb.baud_rate = 115200;
    dcb.byte_size = 8;
    dcb.stop_bits = AW_SERIAL_ONESTOPBIT;
    dcb.f_parity = AW_FALSE;
    ret = aw_serial_dcb_set(TEST_SERIAL_NUM, &dcb);
    if (AW_OK != ret) {
        return 0;
    }

    /* 配置串口超时 */
    ret = aw_serial_timeout_get(TEST_SERIAL_NUM, &timeout);
    if (AW_OK != ret) {
        return 0;
    }

    timeout.rd_timeout = 1000;          /* 读总超时为1s */
    timeout.rd_interval_timeout = 50;   /* 码间超时为50ms */
    ret = aw_serial_timeout_set(TEST_SERIAL_NUM, &timeout);
    if (AW_OK != ret) {
        return 0;
    }


    /* 收到什么数据就发送什么数据 */
    AW_FOREVER {

        /* 读取数据 */
        len = aw_serial_read(TEST_SERIAL_NUM, buf, sizeof(buf));
        if (len > 0) {
            aw_serial_write(TEST_SERIAL_NUM, buf, len);
        }
    }

    return 0;
}

/******************************************************************************/
void demo_serial_int_entry (void)
{
    aw_task_id_t    tsk;

    tsk = aw_task_create("Serial int demo",
                         12,
                         1024,
                         __task_handle,
                         (void *)NULL);
    if (tsk == NULL) {
        aw_kprintf("Serial int demo task create failed\r\n");
        return;
    }

    if (aw_task_startup(tsk) != AW_OK) {
        aw_task_delete(tsk);
    }
}

/** [src_serial_int] */

/* end of file */
