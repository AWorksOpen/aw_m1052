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
/* 华为4G模块匹配信息
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

#define __QUECTEL_MATCH_INFO(vid, pid) \
        { \
            AWBL_USBH_FUNDRV_MATCH_VENDOR | \
            AWBL_USBH_FUNDRV_MATCH_PRODUCT | \
            AWBL_USBH_FUNDRV_MATCH_FUN_CLASS, \
            vid, \
            pid, \
            0, \
            0, \
            0, \
            AW_USB_CLASS_VENDOR_SPEC, \
            0, \
            0, \
            0, \
        }	
/* 龙尚U9300C 4G模块*/	
#define __LONGSHANG_MATCH_INFO(vid, pid) \
        { \
            AWBL_USBH_FUNDRV_MATCH_VENDOR | \
            AWBL_USBH_FUNDRV_MATCH_PRODUCT | \
            AWBL_USBH_FUNDRV_MATCH_FUN_CLASS, \
            vid, \
            pid, \
            0, \
            0, \
            0, \
            AW_USB_CLASS_VENDOR_SPEC, \
            0, \
            0, \
            0, \
        }	
	
/* USB转串FTDI匹配信息
 * sc：接口子代码
 * pr：接口协议代码*/
#define __USB_SERIAL_MATCH_INFO(vid, pid) \
        { \
            AWBL_USBH_FUNDRV_MATCH_VENDOR | \
            AWBL_USBH_FUNDRV_MATCH_PRODUCT, \
            vid, \
            pid, \
            0, \
            0, \
            0, \
            0, \
            0, \
            0, \
            0 \
        }

/* 高新兴物联GM510 4G模块*/
#define __GOSUNCN_MATCH_INFO(vid, pid) \
        { \
            AWBL_USBH_FUNDRV_MATCH_VENDOR | \
            AWBL_USBH_FUNDRV_MATCH_PRODUCT | \
			AWBL_USBH_FUNDRV_MATCH_FUN_CLASS, \
            vid, \
            pid, \
            0, \
            0, \
            0, \
            AW_USB_CLASS_VENDOR_SPEC, \
            0, \
            0, \
            0, \
        }
		
/* 有方 4G模块*/
#define __NEOWAY_MATCH_INFO(vid, pid) \
        { \
            AWBL_USBH_FUNDRV_MATCH_VENDOR | \
            AWBL_USBH_FUNDRV_MATCH_PRODUCT | \
            AWBL_USBH_FUNDRV_MATCH_FUN_CLASS, \
            vid, \
            pid, \
            0, \
            0, \
            0, \
            AW_USB_CLASS_VENDOR_SPEC, \
            0, \
            0, \
            0, \
        }

/* CDC ECM驱动信息匹配表*/
const struct awbl_usbh_fundrv_info_tab awbl_usbh_cdc_serial_info_tab[] = {
        __USB_SERIAL_MATCH_INFO(0x0403, 0x6001),    /* USB转串 FTDI系列*/
        __USB_SERIAL_MATCH_INFO(0x10c4, 0xea60),    /* USB转串 cp210x系列*/
        __USB_SERIAL_MATCH_INFO(0x4348, 0x5523),    /* USB转串 ch34x系列*/
        __USB_SERIAL_MATCH_INFO(0x1a86, 0x7523),    /* USB转串 ch34x系列*/
        __USB_SERIAL_MATCH_INFO(0x1a86, 0x5523),    /* USB转串 ch34x系列*/
        __USB_SERIAL_MATCH_INFO(0x067b, 0x2303),    /* USB转串 pl2303*/


        __HUAWEI_MATCH_INFO(AW_USB_CLASS_VENDOR_SPEC, 0x06, 0x06), /* 华为ME909s-821 Ctrl端口(用于eCall业务)*/
        __HUAWEI_MATCH_INFO(AW_USB_CLASS_VENDOR_SPEC, 0x06, 0x10), /* 华为ME909s-821 MODEM端口*/
        __HUAWEI_MATCH_INFO(AW_USB_CLASS_VENDOR_SPEC, 0x06, 0x12), /* 华为ME909s-821 PCUI端口(用于和上位机应用程序进行AT命令通信)*/
        __HUAWEI_MATCH_INFO(AW_USB_CLASS_VENDOR_SPEC, 0x06, 0x13), /* 华为ME909s-821 DIAG端口(用于调试收集日志信息)*/
        __HUAWEI_MATCH_INFO(AW_USB_CLASS_VENDOR_SPEC, 0x06, 0x1B), /* 华为ME909s-821 Serial B端口(配合Diag口用于模块log收集)*/

		__QUECTEL_MATCH_INFO(0x2C7C, 0x0125),   /* 移远EC20 4G模块*/
		
		__NEOWAY_MATCH_INFO(0x2949, 0x7401),    /* 有方N58 4G模块*/
		
		__GOSUNCN_MATCH_INFO(0x305a, 0x1415),   /* 高新兴物联GM510 4G模块*/
		
		__LONGSHANG_MATCH_INFO(0x1C9E, 0x9603),
        __LONGSHANG_MATCH_INFO(0x1C9E, 0x9B05),
        __LONGSHANG_MATCH_INFO(0x1C9E, 0x9B3C),
        __LONGSHANG_MATCH_INFO(0x1C9E, 0x9E00),
		
        AWBL_USBH_FUNDRV_INFO_END
};
