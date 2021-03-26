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
*******************************************************************************/

/**
 * \file
 * \brief USB 虚拟打印机演示例程
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
 *   1. 随便选择一个文档，点击打印，在打印界面选择“添加打印机”
 *    . 点击“使用现有的端口”，选择“USB虚拟打印机端口”
 *    . 产商选择“Generic”，打印机选择“Generic/Text Only”
 *    . 点击“使用当前已安装的驱动程序”
 *   2. 打印机添加成功后，点击打印，串口会收到整篇文档的内容
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
#include "aw_demo_config.h"
#include "device/class/vprinter/aw_usbd_vprinter.h"

#define __BUF_SIZE  (4 * 1024)

static aw_usbd_vp_t __g_usbd_printer;

/** \brief 定义全局usb设备实例和usb设备信息结构体 */
static struct aw_usbd            __g_usbd_obj;
const static struct aw_usbd_info __g_usbd_info = {
    0x00,
    0x00,
    0x00,
    64,
    0x0100,
    0x4033,
    0x5093,
    "ZLG-VP",
    "AWorks-vp",
    "20160824"
};

/**
 * \brief USB任务入口
 * \param[in] p_arg  任务参数
 * \retval  无
 */
static void* __printer_task (void *p_arg)
{
    aw_err_t    ret;
    void       *p_buf;

    /* 初始化一个USB设备 */
    ret = aw_usbd_init(&__g_usbd_obj,
                       &__g_usbd_info,
                       NULL,
                       DE_USB_DEV_NAME);
    if (ret != AW_OK) {
        AW_ERRF(("__g_usbd_obj init failed: %d\n", ret));
        return 0;
    }

    /* 创建一个打印机设备 */
    ret = aw_usbd_printer_create(&__g_usbd_obj,
                                 &__g_usbd_printer,
                                 "usb_printer");
    if (ret != AW_OK) {
        AW_ERRF(("usbd_printer init failed: %d\n", ret));
        return 0;
    }

    /* 启动USB设备 */
    ret = aw_usbd_start(&__g_usbd_obj);
    if (ret != AW_OK) {
        AW_ERRF(("__g_usbd_obj start failed: %d\n", ret));
    }

    /* 申请数据缓存 */
    p_buf = aw_usb_mem_alloc(__BUF_SIZE);
    if (p_buf == NULL) {
        AW_ERRF(("aw_usb_mem_alloc error!"));
        return 0;
    }

    while (1) {

        /* 等待USB设备连接电脑 */
        while (!aw_fun_valid(&__g_usbd_printer.fun)) {
            AW_INFOF(("custom usb wait for connect.\n"));
            aw_usb_sem_take(__g_usbd_printer.semb, AW_SEM_WAIT_FOREVER);
        }

        /* 接收数据(阻塞) */
        ret = aw_usbd_cdc_vp_read_sync(&__g_usbd_printer,
                                       p_buf,
                                       __BUF_SIZE,
                                       AW_SEM_WAIT_FOREVER);
        if (ret > 0) {
            memset((char *)p_buf + ret, 0, __BUF_SIZE - ret);
            AW_INFOF(("[recv]:%s\n", (char *)p_buf));
        }
    }

    return 0;
}

/**
 * \brief USB 打印机设备 demo
 * \retval 无
 */
void demo_usbd_printer_entry (void)
{
    AW_TASK_DECL_STATIC(task, 2048);

    AW_TASK_INIT(task,              /* 任务实体 */
                "task",             /* 任务名字 */
                 6,                 /* 任务优先级 */
                 2048,              /* 任务堆栈大小 */
                 __printer_task,    /* 任务入口函数 */
                (void*)0);          /* 任务入口参数 */

    AW_TASK_STARTUP(task);
}









