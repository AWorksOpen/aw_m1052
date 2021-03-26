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

/* ��Ϊƥ����Ϣ
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
/******************************************************************************/
/* CDC ECM������Ϣƥ���*/
const struct awbl_usbh_fundrv_info_tab awbl_usbh_cdc_ecm_info_tab[] = {
        __HUAWEI_MATCH_INFO(0x02, 0x06, 0),    /* ��ΪME909s-821 ����2 NCMͨ�ſ��ƽӿ�*/
        __HUAWEI_MATCH_INFO(0x0A, 0x06, 0),    /* ��ΪME909s-821 ����2 NCM���ݽӿ�*/
        AWBL_USBH_FUNDRV_INFO_END
};

/* CDC ����4Gģ����ƽӿ�������Ϣƥ���*/
const struct awbl_usbh_fundrv_info_tab awbl_usbh_cdc_wireless_control_info_tab[] = {
        __HUAWEI_MATCH_INFO(AW_USB_CLASS_VENDOR_SPEC, 0x06, 0x06), /* ��ΪME909s-821 Ctrl�˿�(����eCallҵ��)*/
        __HUAWEI_MATCH_INFO(AW_USB_CLASS_VENDOR_SPEC, 0x06, 0x10), /* ��ΪME909s-821 MODEM�˿�*/
        __HUAWEI_MATCH_INFO(AW_USB_CLASS_VENDOR_SPEC, 0x06, 0x12), /* ��ΪME909s-821 PCUI�˿�(���ں���λ��Ӧ�ó������AT����ͨ��)*/
        __HUAWEI_MATCH_INFO(AW_USB_CLASS_VENDOR_SPEC, 0x06, 0x13), /* ��ΪME909s-821 DIAG�˿�(���ڵ����ռ���־��Ϣ)*/
        __HUAWEI_MATCH_INFO(AW_USB_CLASS_VENDOR_SPEC, 0x06, 0x1B), /* ��ΪME909s-821 Serial B�˿�(���Diag������ģ��log�ռ�)*/
        AWBL_USBH_FUNDRV_INFO_END
};

