/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief USB转串口演示例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - 对应平台的USBH宏和USBH协议栈
 *      - AW_DRV_USBH_CDC_SERIAL
 *      - AW_COM_CONSOLE
 *   2. 将板子中的DURX、DUTX通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *   3. 在底板的USB端口中插入USB转串口线，与电脑端相连，波特率为115200，8位数据长度，1个停止位，无流控
 *   4. 电脑端通过串口助手发送数据，开发板会把数据发回给电脑端
 *   5. 本demo支持测试两种收发模式(异步和同步)，改一下宏就可以切换模式
 *   6. 目前只支持PL2303，CH340，CP210x， FTDI的USB转串口芯片
 *
 * - 实验现象：
 *   1. 串口打印调试信息。
 *
 */

#include "aworks.h"
#include "aw_vdebug.h"
#include <string.h>
#include "aw_delay.h"
#include "aw_demo_config.h"
#include "aw_serial.h"

/* 这个头文件根据需求修改 */
//#include "awbl_usbh_cdc_serial.h"
#include "host/class/cdc/serial/awbl_usbh_cdc_serial.h"

//#define AYSYNC_MODE
#define SYNC_MODE

#if (defined(AYSYNC_MODE) && defined(SYNC_MODE))
#error "AYSYNC_MODE and SYNC_MODE cannot defined at the same time!"
#endif

#if (!defined(AYSYNC_MODE) && !defined(SYNC_MODE))
#error "AYSYNC_MODE and SYNC_MODE must defined one mode"
#endif

aw_local void* __test_usb_serial_task (void *parg)
{
    aw_err_t    ret;
    int         len = 0;
    char        buf[1024];
    struct aw_usb_serial_dcb dcb;
    uint32_t    timeout;
    uint32_t    tranfer_size;
    void       *handle;
    char       *buf_write = "\r\naworks usb to serial\r\n";

    /* 等待打开串口，永远等待 */
    //handle = awbl_usbh_serial_wait_open(0x6001, 0x0403, AW_USB_WAIT_FOREVER);
    //handle = awbl_usbh_serial_wait_open(0x2303, 0x067b, AW_USB_WAIT_FOREVER);
    //handle = awbl_usbh_serial_wait_open(0x7523, 0x1a86, -1, AW_USB_WAIT_FOREVER);
    //handle = awbl_usbh_serial_wait_open(0xea60, 0x10c4, AW_USB_WAIT_FOREVER);
    handle = awbl_usbh_serial_wait_open(default_pid, default_vid, -1, AW_USB_WAIT_FOREVER);
    if (handle == NULL) {
        aw_kprintf("Open usb serial failed\r\n");
        return 0;
    }

    /* 首先获取dcb的默认配置 */
    ret = aw_usb_serial_dcb_get(handle, &dcb);
    if (AW_OK != ret) {
        awbl_usbh_serial_close(handle);
        aw_kprintf("Get usb serial dcb failed\r\n");
        return 0;
    }

    /* 修改 */
    dcb.baud_rate = 115200;
    dcb.byte_size = 8;
    dcb.stop_bits = AW_SERIAL_ONESTOPBIT;

    /* 无校验 */
    dcb.f_parity = AW_FALSE;

#ifdef AYSYNC_MODE
    dcb.read_mode = SERIAL_READ_ASYNC_MODE;
#elif defined (SYNC_MODE)
    dcb.read_mode = SERIAL_READ_SYNC_MODE;
#endif

    /* 设置USB串口dcb */
    ret = aw_usb_serial_dcb_set(handle, &dcb);
    if (AW_OK != ret) {
        awbl_usbh_serial_close(handle);
        aw_kprintf("Set usb serial dcb failed\r\n");
        return 0;
    }
    ret = aw_usb_serial_dcb_get(handle, &dcb);
    if (AW_OK != ret) {
        awbl_usbh_serial_close(handle);
        aw_kprintf("Get usb serial dcb failed\r\n");
        return 0;
    }

    /* 配置USB串口超时 */
    ret = aw_usb_serial_timeout_get(handle, &timeout);
    if (AW_OK != ret) {
        awbl_usbh_serial_close(handle);
        aw_kprintf("Get usb serial timeout failed\r\n");
        return 0;
    }
    aw_kprintf("usb serial timeout :%d(ms)\r\n", timeout);

    /* 读总超时为1s */
    timeout = 1000;
    ret = aw_usb_serial_timeout_set(handle, &timeout);
    if (AW_OK != ret) {
        awbl_usbh_serial_close(handle);
        aw_kprintf("Set usb serial timeout failed\r\n");
        return 0;
    }
    aw_kprintf("usb serial set timeout :%d(ms)\r\n", timeout);

    /* 获取输入传输缓存大小 */
    ret = awbl_usbh_serial_get_tranfersize(handle, USBH_SERIAL_PIPE_IN, &tranfer_size);
    if (AW_OK != ret) {
        awbl_usbh_serial_close(handle);
        aw_kprintf("Get usb serial IN tranfer size failed\r\n");
        return 0;
    }
    aw_kprintf("usb serial IN tranfer size :%d(Bytes)\r\n", tranfer_size);

    /* 设置输入传输缓存大小 */
    tranfer_size = 1024;
    ret = awbl_usbh_serial_set_tranfersize(handle, USBH_SERIAL_PIPE_IN, tranfer_size);
    if (AW_OK != ret) {
        awbl_usbh_serial_close(handle);
        aw_kprintf("Set usb serial IN tranfer size failed\r\n");
        return 0;
    }
    aw_kprintf("usb serial set IN tranfer size :%d(Bytes)\r\n", tranfer_size);

    /* 发送数据*/
#ifdef AYSYNC_MODE
    aw_kprintf("Async mode\r\n");
    aw_usb_serial_write_async(handle, buf_write, strlen(buf_write));
#elif defined (SYNC_MODE)
    aw_kprintf("Sync mode\r\n");
    aw_usb_serial_write_sync(handle, buf_write, strlen(buf_write));
#endif

    /* 收到什么数据就发送什么数据 */
    while (1) {
#ifdef AYSYNC_MODE

    /* 读取数据 */
        len = aw_usb_serial_read_async(handle, buf, sizeof(buf));
        if (len > 0) {
            ret = aw_usb_serial_write_async(handle, buf, len);
            if (ret != AW_OK)
                break;
            }
            aw_mdelay(1);
        }
#elif defined (SYNC_MODE)

        /* 读取数据 */
        len = aw_usb_serial_read_sync(handle, buf, sizeof(buf));
        if (len > 0) {
            len = aw_usb_serial_write_sync(handle, buf, len);
            if (len < 0)
                break;
        }
    }
#endif
    awbl_usbh_serial_close(handle);

    return 0;
}


/**
 * /brief USB串口例程入口函数
 */
void demo_usb_serial_entry (void)
{
    aw_kprintf("USB Serial demo test...\n");

    AW_TASK_DECL_STATIC(usb_serial, 1024 * 4);

    /* 初始化任务 */
    AW_TASK_INIT(usb_serial,            /* 任务实体 */
               "usb_serial",            /* 任务名字 */
               12,                      /* 任务优先级 */
               1024 * 4 ,               /* 任务堆栈大小 */
               __test_usb_serial_task,  /* 任务入口函数 */
               (void *)NULL);           /* 任务入口参数 */

    /* 启动任务 */
    AW_TASK_STARTUP(usb_serial);
}

/* end of file */
