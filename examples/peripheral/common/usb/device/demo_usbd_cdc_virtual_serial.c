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
 * \brief USB device cdc 虚拟串口演示例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - 对应平台的USBD宏和USBD协议栈
 *      - AW_COM_CONSOLE
 *   2. 将板子中的DURX、DUTX通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *   3. 将开发板USB DEVICE接口与电脑使用USB线连接
 *
 * - 实验现象：
 *   1. 电脑出现一个虚拟串口设备，在设备管理其端口可以看到对应的COM口（如果自动
 *      安装驱动失败，请手动安装通用CDC串口驱动）
 *   2. 打开串口软件，这里等待会比较久
 *   3. 往串口软件键入数据值，会收到相应的数据
 *
 */

#include "aworks.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_task.h"
#include "aw_sem.h"
#include "aw_assert.h"
#include "device/aw_usbd.h"
#include "aw_usb_mem.h"
#include "string.h"
#include "device/class/cdc/aw_usbd_cdc_virtual_serial.h"
#include "aw_demo_config.h"
#include "aw_serial.h"

static struct aw_usbd_cdc_vs    __g_usbd_cdc;

/* 定义全局usb设备实例和usb设备信息结构体 */
static struct aw_usbd            __g_usbd_obj;
const static struct aw_usbd_info __g_usbd_info = {
    0xEF,
    0x02,
    0x01,
    64,
    0x0100,
    0x6047,
    0x5052,
    "ZLG-VS",
    "AWorks-vs",
    "20160824"
};

/* USB任务入口
 * p_arg : 任务参数
 */
static void* __serial_task (void *p_arg)
{
    aw_err_t                ret;
    char                   *buff        = "Hello,i am AWorks\r\n";
    char                   *read_buf    = NULL;
    unsigned int            len;
    struct aw_usbd_trans    tx_trans;

    /* 申请读缓存 */
    read_buf = aw_usb_mem_alloc(1024);
    if (read_buf == NULL) {
        AW_ERRF(("read_buf alloc failed\r\n"));
        return 0;
    }

    /* 初始化一个USB设备 */
    ret = aw_usbd_init(&__g_usbd_obj,
                       &__g_usbd_info,
                       NULL,
                       DE_USB_DEV_NAME);
    if (ret != AW_OK) {
        AW_ERRF(("__g_usbd_obj init failed: %d\r\n", ret));
        return 0;
    }

    /* 创建一个虚拟串口 */
    ret = aw_usbd_cdc_vs_create(&__g_usbd_obj,
                                &__g_usbd_cdc,
                                0,
                                "cdc0",
                                1024);
    if (ret != AW_OK) {
        AW_ERRF(("usbd_cdc init failed: %d\r\n", ret));
        return 0;
    }

    /* 设置传输模式 */
    //ret = aw_usbd_cdc_vs_set_rx_mode(&__g_usbd_cdc, AW_CDC_VS_TRANFER_SYNC);
    ret = aw_usbd_cdc_vs_set_rx_mode(&__g_usbd_cdc, AW_CDC_VS_TRANFER_ASYNC);
    if (ret != AW_OK) {
        AW_ERRF(("usbd_cdc set rx mode failed: %d\r\n", ret));
        return 0;
    }

    //ret = aw_usbd_cdc_vs_set_tx_mode(&__g_usbd_cdc, AW_CDC_VS_TRANFER_SYNC);
    ret = aw_usbd_cdc_vs_set_tx_mode(&__g_usbd_cdc, AW_CDC_VS_TRANFER_ASYNC);
    if (ret != AW_OK) {
        AW_ERRF(("usbd_cdc set tx mode failed: %d\r\n", ret));
        return 0;
    }

    /* 启动USB设备 */
    aw_usbd_start(&__g_usbd_obj);
    aw_mdelay(10);

    if (__g_usbd_cdc.tx_mode == AW_CDC_VS_TRANFER_ASYNC) {
        memset(&tx_trans, 0, sizeof( struct aw_usbd_trans));
        aw_usbd_fill_trans(&tx_trans, &__g_usbd_cdc.in, buff, 19, 0, NULL, NULL);
        aw_usbd_cdc_vs_async_write(&__g_usbd_cdc, &tx_trans);
    } else if (__g_usbd_cdc.rx_mode == AW_CDC_VS_TRANFER_SYNC) {
        aw_usbd_cdc_vs_poll_write(&__g_usbd_cdc, buff, 19, AW_USB_WAIT_FOREVER);
    }

    for (;;) {
        while (!aw_fun_valid(&__g_usbd_cdc.dfun)) {
            AW_INFOF(("USBD_CDC: wait for connect.\n"));
            aw_usb_sem_take(__g_usbd_cdc.sync_semb, AW_USB_WAIT_FOREVER);
        }

        if (__g_usbd_cdc.rx_mode == AW_CDC_VS_TRANFER_SYNC) {
            memset(read_buf, 0x00, 1024);
            len = aw_usbd_cdc_vs_poll_read(&__g_usbd_cdc, read_buf, 1024, AW_USB_WAIT_FOREVER);
            if (len > 0) {
                aw_usbd_cdc_vs_poll_write(&__g_usbd_cdc, read_buf, len, 5000);
            }
        } else if (__g_usbd_cdc.rx_mode == AW_CDC_VS_TRANFER_ASYNC) {
            memset(read_buf, 0x00, 1024);
            ret = aw_usbd_cdc_vs_async_read(&__g_usbd_cdc, read_buf, 1024, &len);
            if (len > 0) {
                memset(&tx_trans, 0, sizeof( struct aw_usbd_trans));
                aw_usbd_fill_trans(&tx_trans, &__g_usbd_cdc.in, read_buf, len, 0, NULL, NULL);
                aw_usbd_cdc_vs_async_write(&__g_usbd_cdc, &tx_trans);
            }
            aw_mdelay(10);
        }
    }

    return 0;
}

/* USB cdc 串口 demo */
void demo_std_usbd_cdc_serial_entry (void)
{
    AW_TASK_DECL_STATIC(task, 2048);

    AW_TASK_INIT(task,          /* 任务实体 */
                "task",         /* 任务名字 */
                 6,             /* 任务优先级 */
                 2048,          /* 任务堆栈大小 */
                 __serial_task, /* 任务入口函数 */
                (void*)0);      /* 任务入口参数 */

    AW_TASK_STARTUP(task);
}

/** [src_std_usbd_cdc] */

/* end of file */
