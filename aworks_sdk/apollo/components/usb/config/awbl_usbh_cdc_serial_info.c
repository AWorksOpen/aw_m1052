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

/* ��ΪVID*/
#define __HUAWEI_VENDOR_ID             0x12D1
/* ��Ϊ4Gģ��ƥ����Ϣ
 * sc���ӿ��Ӵ���
 * pr���ӿ�Э�����*/
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
/* ����U9300C 4Gģ��*/	
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
	
/* USBת��FTDIƥ����Ϣ
 * sc���ӿ��Ӵ���
 * pr���ӿ�Э�����*/
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

/* ����������GM510 4Gģ��*/
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
		
/* �з� 4Gģ��*/
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

/* CDC ECM������Ϣƥ���*/
const struct awbl_usbh_fundrv_info_tab awbl_usbh_cdc_serial_info_tab[] = {
        __USB_SERIAL_MATCH_INFO(0x0403, 0x6001),    /* USBת�� FTDIϵ��*/
        __USB_SERIAL_MATCH_INFO(0x10c4, 0xea60),    /* USBת�� cp210xϵ��*/
        __USB_SERIAL_MATCH_INFO(0x4348, 0x5523),    /* USBת�� ch34xϵ��*/
        __USB_SERIAL_MATCH_INFO(0x1a86, 0x7523),    /* USBת�� ch34xϵ��*/
        __USB_SERIAL_MATCH_INFO(0x1a86, 0x5523),    /* USBת�� ch34xϵ��*/
        __USB_SERIAL_MATCH_INFO(0x067b, 0x2303),    /* USBת�� pl2303*/


        __HUAWEI_MATCH_INFO(AW_USB_CLASS_VENDOR_SPEC, 0x06, 0x06), /* ��ΪME909s-821 Ctrl�˿�(����eCallҵ��)*/
        __HUAWEI_MATCH_INFO(AW_USB_CLASS_VENDOR_SPEC, 0x06, 0x10), /* ��ΪME909s-821 MODEM�˿�*/
        __HUAWEI_MATCH_INFO(AW_USB_CLASS_VENDOR_SPEC, 0x06, 0x12), /* ��ΪME909s-821 PCUI�˿�(���ں���λ��Ӧ�ó������AT����ͨ��)*/
        __HUAWEI_MATCH_INFO(AW_USB_CLASS_VENDOR_SPEC, 0x06, 0x13), /* ��ΪME909s-821 DIAG�˿�(���ڵ����ռ���־��Ϣ)*/
        __HUAWEI_MATCH_INFO(AW_USB_CLASS_VENDOR_SPEC, 0x06, 0x1B), /* ��ΪME909s-821 Serial B�˿�(���Diag������ģ��log�ռ�)*/

		__QUECTEL_MATCH_INFO(0x2C7C, 0x0125),   /* ��ԶEC20 4Gģ��*/
		
		__NEOWAY_MATCH_INFO(0x2949, 0x7401),    /* �з�N58 4Gģ��*/
		
		__GOSUNCN_MATCH_INFO(0x305a, 0x1415),   /* ����������GM510 4Gģ��*/
		
		__LONGSHANG_MATCH_INFO(0x1C9E, 0x9603),
        __LONGSHANG_MATCH_INFO(0x1C9E, 0x9B05),
        __LONGSHANG_MATCH_INFO(0x1C9E, 0x9B3C),
        __LONGSHANG_MATCH_INFO(0x1C9E, 0x9E00),
		
        AWBL_USBH_FUNDRV_INFO_END
};
