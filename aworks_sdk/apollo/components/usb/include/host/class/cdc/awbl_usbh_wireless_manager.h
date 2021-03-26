/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

#ifndef __AWBL_USBH_WIRELESS_MANAGER_H
#define __AWBL_USBH_WIRELESS_MANAGER_H
#include "aw_vdebug.h"

/* USB无线设备驱动打印函数*/
#define __USB_WIRELESS_TRACE(info) \
            do { \
                AW_INFOF(("USB-WIRELESS:"));\
                AW_INFOF(info);\
            } while (0)

enum{
    PPP_MODE = 1,
    ECM_MODE = 2,
};


/* 无线设备管道结构体*/
struct __wireless_pipe {
    struct awbl_usbh_function       *p_fun;        /* 相应的功能设备结构体*/
    AW_MUTEX_DECL(                   lock);        /* 管道互斥锁*/
    struct aw_list_head              node;         /* 管道节点*/
    struct aw_refcnt                 ref;          /* 管道引用计数*/

    struct awbl_usbh_endpoint       *p_in;         /* 设备输入端点*/
    void                            *p_in_buf;     /* 设备输入端点数据缓存*/

    struct awbl_usbh_endpoint       *p_out;        /* 设备输出端点*/
    void                            *p_out_buf;    /* 设备输出端点数据缓存*/
    int                              size;         /* 一次传输的数据大小*/

    aw_bool_t                        removed;      /* 移除状态*/
};

/* 打开无线设备管道*/
void *awbl_usbh_wireless_pipe_open (int     pid,
                                    int     vid,
                                    int     inum,
                                    aw_bool_t rngbuf,
                                    int     size,
                                    int     timeout);
/* 关闭无线设备管道*/
void awbl_usbh_wireless_pipe_close (void *pipe);
/* 无线设备管道写*/
int awbl_usbh_wireless_pipe_write (void *pipe, void *p_buf, int len, int timeout);
/* 无线设备管道读*/
int awbl_usbh_wireless_pipe_read (void *pipe, void *p_buf, int len, int timeout);
/* 无线设备管理驱动注册*/
void awbl_usbh_wireless_manager_register (int mode);

#endif

