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
 * \brief USB 自定义设备演示例程（USB device 自定义USB设备,演示例程）
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - 对应平台的USBD宏和USBD协议栈
 *      - AW_COM_CONSOLE
 *   2. 将板子中的DURX、DUTX通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *   3. 将开发板USB DEVICE0接口与电脑使用USB线连接
 *
 * - 实验现象：
 *   1. 电脑设备管理器出现一个AWorks USB自定义设备
 *   2. 安装提供的配套驱动
 *   3. 打开配套的上位机软件(可用NI MAX替代)进行数据收发测试
 *
 */

/** [src_usbd_vendor] */
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

#define __BUF_SIZE          512
#define __WAIT_TIMEOUT      5000
/* USB设备控制器名字*/
#define __DISK_NAME         DE_USB_DEV_NAME

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
    "ZLG",
    "AWorks",
    "20160824"
};

/** \brief 替代设置回调函数 */
static int __custom_alt_set (struct aw_usbd_fun  *p_fun,
                             aw_bool_t            set);

/** \brief 数据收发主任务 */
static void* __custom_task (void *arg);

/** \brief 接口定义 */
static struct aw_usbd_fun   __g_ufun;

/** \brief 输入输出管道定义 */
static struct aw_usbd_pipe  __g_in;
static struct aw_usbd_pipe  __g_out;

AW_SEMB_DECL_STATIC(__g_semb);

/** \brief 数据收发任务 */
AW_TASK_DECL_STATIC(__g_task, 2048);

/** \brief 接口信息 */
static const struct aw_usbd_fun_info __g_info = {
    AW_USB_CLASS_VENDOR_SPEC,       /**< \brief 类代码-厂商自定义设备 */
    0x00,                           /**< \brief 子类代码 */
    0x00,                           /**< \brief 协议代码 */
    __custom_alt_set,               /**< \brief 替代设置回调 */
    NULL                            /**< \brief 控制传输回调  */
};

/** \brief IN 管道信息 */
static const struct aw_usbd_pipe_info __g_in_info = {
    0,                              /**< \brief 端点地址自动分配 */
    AW_PIPE_DIR_IN,                 /**< \brief IN端点 */
    AW_PIPE_TYPE_BULK,              /**< \brief 批量传输类型 */
    0,                              /**< \brief 批量时无效 */
    512                             /**< \brief 最大包大小512字节 */
};

/** \brief OUT 管道信息 */
static const struct aw_usbd_pipe_info __g_out_info = {
    0,                              /**< \brief 端点地址自动分配 */
    AW_PIPE_DIR_OUT,                /**< \brief OUT端点 */
    AW_PIPE_TYPE_BULK,              /**< \brief 批量传输类型 */
    0,                              /**< \brief 批量时无效 */
    512                             /**< \brief 最大包大小512字节 */
};

static int __custom_alt_set (struct aw_usbd_fun  *p_fun,
                             aw_bool_t            set)
{
    if (set) {
        AW_SEMB_GIVE(__g_semb);
    } else {
        aw_usbd_pipe_reset(p_fun->p_obj, &__g_in);
        aw_usbd_pipe_reset(p_fun->p_obj, &__g_out);
    }
    return AW_OK;
}

/* USB收发任务 */
/* arg：任务参数(这里是USB接口结构体) */
static void* __custom_task (void *arg)
{
    int   ret;
    void *p_buf;

    /* 申请数据缓存 */
    p_buf = aw_usb_mem_alloc(__BUF_SIZE);
    if (p_buf == NULL) {
        aw_kprintf("aw_usb_mem_alloc error!");
        return 0;
    }

    AW_FOREVER {

        /* 等待USB设备连接电脑 */
        while (!aw_fun_valid(&__g_ufun)) {
            AW_INFOF(("custom usb wait for connect.\n"));
            AW_SEMB_TAKE(__g_semb, AW_SEM_WAIT_FOREVER);
        }

        /* 数据缓存清0 */
        memset(p_buf, 0, __BUF_SIZE);

        /* 接收数据(阻塞) */
        ret = aw_usbd_trans_sync(__g_ufun.p_obj,
                                 &__g_out,
                                 p_buf,
                                 __BUF_SIZE,
                                 0,
                                 AW_SEM_WAIT_FOREVER);
        if (ret > 0) {
            aw_kprintf("[recv]:%s\n", (char *)p_buf);
        }
        aw_kprintf("[recv_len]:%d\n", ret);

        /* 把数据发送回去 */
        ret = aw_usbd_trans_sync(__g_ufun.p_obj,
                                 &__g_in,
                                 p_buf,
                                 ret,
                                 AWBL_USBD_ZERO_PACKET,
                                 __WAIT_TIMEOUT);
        aw_kprintf("[tranfer]:%d\n", ret);
    }

    return 0;
}

/**
 * \brief 自定义类例程入口
 */
aw_local int __demo_usbd_vendor (void)
{
    int   ret;

    /* 初始化接口功能 */
    ret = aw_usbd_fun_init(&__g_ufun,
                           "AWORKS-USB",
                           &__g_info);
    if (ret != AW_OK) {
        return ret;
    }

    /* 创建IN管道 */
    ret = aw_usbd_pipe_create(&__g_ufun, &__g_in, &__g_in_info);
    if (ret != AW_OK) {
        return ret;
    }

    /* 创建OUT管道 */
    ret = aw_usbd_pipe_create(&__g_ufun, &__g_out, &__g_out_info);
    if (ret != AW_OK) {
        return ret;
    }

    /* 添加接口功能到USB设备对象 */
    ret = aw_usbd_fun_add(&__g_usbd_obj, 0, &__g_ufun);
    if (ret != AW_OK) {
        return ret;
    }

    /* 初始化信号量 */
    AW_SEMB_INIT(__g_semb, 0, AW_SEM_Q_PRIORITY);

    /* 初始化一个任务 */
    AW_TASK_INIT(__g_task,
                 "CUSTOM-USB",
                 10,
                 2048,
                 __custom_task,
                 &__g_ufun);

    /* 启动任务 */
    AW_TASK_STARTUP(__g_task);

    return AW_OK;
}

/**
 * \brief 自定义USB设备 demo
 * \return 无
 */
void demo_usbd_vendor_entry (void)
{
    int  ret;

    /*
     * USB device 初始化
     * "/dev/usbd" 在awbl_hwconf_xxxxxx_usbd.h 中定义
     */
    ret = aw_usbd_init(&__g_usbd_obj,
                       &__g_usbd_info,
                       NULL,
                       __DISK_NAME);
    if (ret != AW_OK) {
        AW_ERRF(("__g_usbd_obj init failed: %d\n", ret));
        return;
    }

    /* 自定义类例程入口 */
    __demo_usbd_vendor();

    /* 启动USB设备 */
    ret = aw_usbd_start(&__g_usbd_obj);
    if (ret != AW_OK) {
        AW_ERRF(("__g_usbd_obj start failed: %d\n", ret));
        return;
    }

    return ;
}

/** [src_usbd_vendor] */

/* end of file */
