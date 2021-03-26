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
 * \brief USB device hid 键盘演示例程
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
 *   1. 电脑设备管理器出现一个hid设备
 *   2. 在鼠标光标处每隔5s为打印一个“a”
 *
 */

/** [src_std_usbd_hid] */
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
#include "device/class/hid/aw_usbd_hid.h"
#include "aw_input_code.h"

static struct aw_usbd_hid    __g_usbd_hid;

/** \brief 定义全局usb设备实例和usb设备信息结构体 */
static struct aw_usbd            __g_usbd_obj;
const static struct aw_usbd_info __g_usbd_info = {
    0x00,
    0x00,
    0x00,
    64,
    0x0100,
    0x4035,
    0x5095,
    "ZLG-KB",
    "AWorks-kb",
    "20160824"
};

/**
 * 数据接收任务
 */
//static void* __task_rev_entry (void *p_arg)
//{
//    int ret;
//    //struct aw_usbd_hid *p_hid = (struct aw_usbd_hid *)p_arg;
//
//    /* 分配一个键盘上报数据结构内存并初始化 */
//    aw_usbd_keyboard_out_report_t *p_report =
//            aw_usb_mem_alloc(sizeof(aw_usbd_keyboard_out_report_t));
//    memset(p_report, 0x00, sizeof(aw_usbd_keyboard_out_report_t));
//
//    AW_FOREVER {
//
//        /* 等待接收数据 */
//        ret = aw_usbd_hid_keyboard_receive(&__g_usbd_hid,
//                                           p_report,
//                                           AW_SEM_WAIT_FOREVER);
//        if (ret == AW_OK) {
//            AW_INFOF(("KEY %d PRESSED.\n", p_report->led));
//        }
//    }
//    return 0;
//}

/**
 * \brief USB任务入口
 * \param[in] p_arg : 任务参数
 * \return 无
 */
static void* __keyboard_task (void *p_arg)
{
    int  ret;
//    AW_TASK_DECL_STATIC(task_rev, 1024);
    aw_usbd_keyboard_in_report_t *p_report = NULL;

    /* 初始化一个USB设备 */
    ret = aw_usbd_init(&__g_usbd_obj,
                       &__g_usbd_info,
                       NULL,
                       DE_USB_DEV_NAME);
    if (ret != AW_OK) {
        AW_ERRF(("__g_usbd_obj init failed: %d\n", ret));
        return 0;
    }

    /* 创建一个HID 键盘设备 */
    ret = aw_usbd_hid_keyboard_create(&__g_usbd_obj,
                                      &__g_usbd_hid,
                                      "hid0_keyboard");
    if (ret != AW_OK) {
        AW_ERRF(("usbd_hid init failed: %d\n", ret));
        return 0;
    }

    /* 启动USB设备 */
    ret = aw_usbd_start(&__g_usbd_obj);
    if (ret != AW_OK) {
        AW_ERRF(("__g_usbd_obj start failed: %d\n", ret));
        return 0;
    }

//    AW_TASK_INIT(task_rev,
//                 "usbd_ms_task",
//                 6,
//                 1024,
//                 __task_rev_entry,
//                (void*)(&__g_usbd_hid));
//
//    AW_TASK_STARTUP(task_rev);

    /* 分配一个键盘上报数据结构内存并初始化 */
    p_report = aw_usb_mem_alloc(sizeof(aw_usbd_keyboard_in_report_t));
    if (p_report == NULL) {
        AW_ERRF(("p_report alloc failed\r\n"));
        return 0;
    }
    memset(p_report, 0x00, sizeof(aw_usbd_keyboard_in_report_t));

    for (;;) {

        /* 相当于键盘的“A”按键 */
        p_report->key[0] = key_to_code(KEY_A);
        aw_usbd_hid_keyboard_send(&__g_usbd_hid, p_report, 5000);

        /* 松开按键 */
        p_report->key[0] = 0;
        aw_usbd_hid_keyboard_send(&__g_usbd_hid, p_report, 5000);
        aw_mdelay(5000);
    }

    return 0;
}


/**
 * \brief USB hid设备 demo
 * \return 无
 */
void demo_std_usbd_hid_keyboard_entry (void)
{
    AW_TASK_DECL_STATIC(task, 2048);

    AW_TASK_INIT(task,              /* 任务实体 */
                "task",             /* 任务名字 */
                 6,                 /* 任务优先级 */
                 2048,              /* 任务堆栈大小 */
                 __keyboard_task,   /* 任务入口函数 */
                (void*)0);          /* 任务入口参数 */

    AW_TASK_STARTUP(task);
}

/** [src_std_usbd_hid] */

/* end of file */
