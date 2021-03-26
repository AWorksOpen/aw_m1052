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
 * \brief USB device hid mouse演示例程
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
 *   2. 鼠标一直在移动
 *
 */

/**
 * \addtogroup demo_if_std_usbd_hid
 * \copydoc demo_std_usbd_hid.c
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

static struct aw_usbd_hid    __g_usbd_hid;

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
    "ZLG-MOUSE",
    "AWorks-mouse",
    "20160824"
};

/**
 * \brief USB任务入口
 * \param[in] p_arg : 任务参数
 * \return 无
 */
static void* __mouse_task (void *p_arg)
{
    int  ret;

    /* 分配一个鼠标输入结构体内存空间并初始化 */
    aw_usbd_mouse_in_report_t *p_report =
            aw_usb_mem_alloc(sizeof(aw_usbd_mouse_in_report_t));
    memset(p_report, 0x00, sizeof(aw_usbd_mouse_in_report_t));

    /* 初始化一个USB设备 */
    ret = aw_usbd_init(&__g_usbd_obj,
                       &__g_usbd_info,
                       NULL,
                       DE_USB_DEV_NAME);
    if (ret != AW_OK) {
        AW_ERRF(("__g_usbd_obj init failed: %d\n", ret));
        goto __out;
    }
    /* 创建一个HID 鼠标设备 */
    ret = aw_usbd_hid_mouse_create(&__g_usbd_obj,
                                   &__g_usbd_hid,
                                   "hid0_mouse");
    if (ret != AW_OK) {
        AW_ERRF(("usbd_hid init failed: %d\n", ret));
        goto __out;
    }
    //aw_usbd_hid_set_rev_cb(&__g_usbd_hid, NULL, &__g_usbd_hid);

    /* 启动USB设备 */
    ret = aw_usbd_start(&__g_usbd_obj);
    if (ret != AW_OK) {
        AW_ERRF(("__g_usbd_obj start failed: %d\n", ret));
    }

__out:
    for (;;) {

        /* 等待USB设备连接电脑 */
        while (!aw_fun_valid(&__g_usbd_hid.fun)) {
            AW_INFOF(("custom usb wait for connect.\n"));
            aw_usb_sem_take(__g_usbd_hid.semb, AW_SEM_WAIT_FOREVER);
        }
        aw_mdelay(500);

        /* 模拟鼠标坐标数据 */
        p_report->x = 10;
        p_report->y = 10;

        /* 把数据上报到主机 */
        aw_usbd_hid_mouse_send(&__g_usbd_hid, p_report, AW_SEM_WAIT_FOREVER);
    }

    return 0;
}

/**
 * \brief USB hid设备 demo
 * \return 无
 */
void demo_std_usbd_hid_mouse_entry (void)
{
    AW_TASK_DECL_STATIC(task, 2048);

    AW_TASK_INIT(task,          /* 任务实体 */
                "task",         /* 任务名字 */
                 6,             /* 任务优先级 */
                 2048,          /* 任务堆栈大小 */
                 __mouse_task,  /* 任务入口函数 */
                (void*)0);      /* 任务入口参数 */

    AW_TASK_STARTUP(task);
}

/** [src_std_usbd_hid] */

/* end of file */
