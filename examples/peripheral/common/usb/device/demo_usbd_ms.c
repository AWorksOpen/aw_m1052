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
 * \brief U盘演示例程（USB device 大容量存储器演示）
 *
 * - 操作步骤：
 *   1. 本例程需在 aw_prj_params.h 头文件里使能
 *      - 对应平台的USBD宏和USBD协议栈
 *      - AW_COM_FS_FATFS
 *      - AW_COM_CONSOLE
 *   2. 将板子中的DURX、DUTX通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 *   3. 程序运行后将开发板USB DEVICE0接口与电脑使用USB线连接
 *
 * - 实验现象：
 *   1. 电脑设备管理器出现一个名称为 "AWorks" 的可移动存储设备
 *
 * - 备注：
 *   1. 如果因内存不足而导致创建失败，请尝试关闭非必要组件、修改参数减小需要的
 *      空间大小。
 */

/** [src_usbd_ms] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "device/aw_usbd.h"
#include "device/class/ms/aw_usbd_ms.h"
#include "fs/aw_ram_disk.h"
#include "fs/aw_blk_dev.h"
#include "fs/aw_mount.h"
#include "string.h"
#include "aw_demo_config.h"

#define __MS_SDCARD     0
#define __MS_RAM_DISK   1

/** \brief 使用SD卡还是RAM_DISK模拟U盘
 * 若使用SD卡，需要在aw_prj_params.h中使能SD宏设备
 * 并且板子 SD0 需要插入SD卡
 */
#define __MS_USE_TYPE   __MS_RAM_DISK
//#define __MS_USE_TYPE   __MS_SDCARD

#define __USB_DEV_NAME  DE_USB_DEV_NAME

#if __MS_USE_TYPE == __MS_SDCARD
#include "aw_sdcard.h"

#define __DISK_NAME     DE_SD_DEV_NAME
#endif

/** \brief 定义全局usb设备实例和usb设备信息结构体 */
static struct aw_usbd            __g_usbd_obj;
const static struct aw_usbd_info __g_usbd_info = {
    0,
    0,
    0,
    64,
    0x0100,
    0x3068,
    0x0003,
    "ZLG-MS",
    "AWorks-ms",
    "20160824"
};


/** \brief 大容量存储器实例定义 */
static struct aw_usbd_ms      __g_usbd_ms;

#if __MS_USE_TYPE == __MS_RAM_DISK

/** \brief ram_disk实例定义 */
static struct aw_ram_disk     __g_ramdisk;

/** \briefram_disk空间大小（即U盘空间大小） */
static char                   __g_ram[100 * 1024];
#endif

/**
 * \brief USB device 大容量存储器 demo
 * \return 无
 */
void demo_usbd_ms_entry (void)
{
    int         ret;
    const char *disk;
#if __MS_USE_TYPE == __MS_SDCARD
    disk = __DISK_NAME;
    aw_blk_dev_wait_install(disk, AW_WAIT_FOREVER);
#endif

    /*
     * USB device 初始化
     * "/dev/usbd0" 在 awbl_hwconf_xxxxxx_usbd.h 中定义
     */
    ret = aw_usbd_init(&__g_usbd_obj,
                       &__g_usbd_info,
                       NULL,
                       DE_USB_DEV_NAME);
    if (ret != AW_OK) {
        AW_ERRF(("__g_usbd_obj init failed: %d\n", ret));
        return ;
    }

#if __MS_USE_TYPE == __MS_RAM_DISK
    disk = __USB_DEV_NAME;

    /* 存储器分配的块数不能少于192个 */
    ret = aw_ram_disk_create(&__g_ramdisk,
                             disk,
                             512,                /* 一个块的大小 */
                             __g_ram,
                             sizeof(__g_ram),    /* 存储器总大小 */
                             NULL);
    if (ret != AW_OK) {
        AW_ERRF(("disk create failed: %d\n", ret));
        return;
    }

    /* 用 fat 格式化 ram disk */
    ret = aw_make_fs(disk, "vfat", NULL);
    if (ret != AW_OK) {
        AW_ERRF(("disk make_fs failed: %d\n", ret));
        return;
    }

#endif

    ret = aw_usbd_ms_create(&__g_usbd_obj,
                            &__g_usbd_ms,
                            "UDISK",
                            NULL,
                            20 * 1024);
    if (ret != AW_OK) {
        AW_ERRF(("aw_usbd_ms_create failed: %d\n", ret));
        return;
    }

    aw_usbd_ms_lun_add(&__g_usbd_ms, disk);
    if (ret != AW_OK) {
        AW_ERRF(("__g_usbd_ms_lun init failed: %d\n", ret));
        return;
    }

    ret = aw_usbd_start(&__g_usbd_obj);
    if (ret != AW_OK) {
        AW_ERRF(("__g_usbd_obj start failed: %d\n", ret));
    }

    return ;
}

/** [src_usbd_ms] */

/* end of file */
