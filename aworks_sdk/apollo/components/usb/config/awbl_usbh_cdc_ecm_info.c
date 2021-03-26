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

#include "apollo.h"
#include "host/awbl_usbh.h"

/* 华为VID*/
#define __HUAWEI_VENDOR_ID             0x12D1

/* 华为匹配信息
 * sc：接口子代码
 * pr：接口协议代码*/
#define __HUAWEI_MATCH_INFO(cl, sc, pr) \
        { \
            AWBL_USBH_FUNDRV_MATCH_VENDOR | \
            AWBL_USBH_FUNDRV_MATCH_DEV_CLASS | \
            AWBL_USBH_FUNDRV_MATCH_FUN_CLASS | \
            AWBL_USBH_FUNDRV_MATCH_FUN_SUBCLASS | \
            AWBL_USBH_FUNDRV_MATCH_FUN_PROTOCOL, \
            __HUAWEI_VENDOR_ID, \
            0, \
            0, \
            0, \
            0xFF, \
            cl, \
            sc, \
            pr, \
            0 \
        }
/******************************************************************************/
/* CDC ECM驱动信息匹配表*/
const struct awbl_usbh_fundrv_info_tab awbl_usbh_cdc_ecm_info_tab[] = {
        __HUAWEI_MATCH_INFO(0x02, 0x06, 0),    /* 华为ME909s-821 配置2 NCM通信控制接口*/
        __HUAWEI_MATCH_INFO(0x0A, 0x06, 0),    /* 华为ME909s-821 配置2 NCM数据接口*/
        AWBL_USBH_FUNDRV_INFO_END
};

/* CDC 无线4G模块控制接口驱动信息匹配表*/
const struct awbl_usbh_fundrv_info_tab awbl_usbh_cdc_wireless_control_info_tab[] = {
        __HUAWEI_MATCH_INFO(AW_USB_CLASS_VENDOR_SPEC, 0x06, 0x06), /* 华为ME909s-821 Ctrl端口(用于eCall业务)*/
        __HUAWEI_MATCH_INFO(AW_USB_CLASS_VENDOR_SPEC, 0x06, 0x10), /* 华为ME909s-821 MODEM端口*/
        __HUAWEI_MATCH_INFO(AW_USB_CLASS_VENDOR_SPEC, 0x06, 0x12), /* 华为ME909s-821 PCUI端口(用于和上位机应用程序进行AT命令通信)*/
        __HUAWEI_MATCH_INFO(AW_USB_CLASS_VENDOR_SPEC, 0x06, 0x13), /* 华为ME909s-821 DIAG端口(用于调试收集日志信息)*/
        __HUAWEI_MATCH_INFO(AW_USB_CLASS_VENDOR_SPEC, 0x06, 0x1B), /* 华为ME909s-821 Serial B端口(配合Diag口用于模块log收集)*/
        AWBL_USBH_FUNDRV_INFO_END
};

