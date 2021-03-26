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

/** \brief wireless device vendor id and product id */
/** \brief Anydata */
#define __ANYDATA_VENDOR_ID             0x16d5
#define __ANYDATA_PRODUCT_ADU_620UW     0x6202
#define __ANYDATA_PRODUCT_ADU_E100A     0x6501
#define __ANYDATA_PRODUCT_ADU_500A      0x6502
#define __ANYDATA_PRODUCT_DTL718_W      0x6604

#define __ANYDATA_MATCH_INFO(pid) \
        { \
            AWBL_USBH_FUNDRV_MATCH_VENDOR | \
            AWBL_USBH_FUNDRV_MATCH_PRODUCT | \
            AWBL_USBH_FUNDRV_MATCH_DEV_CLASS, \
            __ANYDATA_VENDOR_ID, \
            pid, \
            0, \
            0, \
            0, \
            AW_USB_CLASS_VENDOR_SPEC, \
            0, \
            0, \
            0 \
        }

/** \brief HUAWEI */
#define __HUAWEI_VENDOR_ID             0x12D1

#define __HUAWEI_MATCH_INFO(sc, pr) \
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
            AW_USB_CLASS_VENDOR_SPEC, \
            sc, \
            pr, \
            0 \
        }

/** \brief LongShang */
#define __LONGSHANG_VENDOR_ID             0x1C9E

#define __LONGSHANG_MATCH_INFO(pid) \
        { \
            AWBL_USBH_FUNDRV_MATCH_VENDOR | \
            AWBL_USBH_FUNDRV_MATCH_PRODUCT | \
            AWBL_USBH_FUNDRV_MATCH_FUN_CLASS, \
            __LONGSHANG_VENDOR_ID, \
            pid, \
            0, \
            0, \
            0, \
            AW_USB_CLASS_VENDOR_SPEC, \
            0, \
            0, \
            0, \
        }

/** \brief LongShang */
#define __LONGSHANG_EX_VENDOR_ID             0x12D1
#define __LONGSHANG_EX_PRODUCT_ID            0x1506

#define __LONGSHANG_EX_MATCH_INFO(sc, pr) \
        { \
            AWBL_USBH_FUNDRV_MATCH_VENDOR | \
            AWBL_USBH_FUNDRV_MATCH_PRODUCT | \
            AWBL_USBH_FUNDRV_MATCH_FUN_CLASS | \
            AWBL_USBH_FUNDRV_MATCH_FUN_SUBCLASS | \
            AWBL_USBH_FUNDRV_MATCH_FUN_PROTOCOL, \
            __LONGSHANG_EX_VENDOR_ID, \
            __LONGSHANG_EX_PRODUCT_ID, \
            0, \
            0, \
            0, \
            AW_USB_CLASS_VENDOR_SPEC, \
            sc, \
            pr, \
            0, \
        }

#define __FT232_INFO() \
        { \
            AWBL_USBH_FUNDRV_MATCH_VENDOR | \
            AWBL_USBH_FUNDRV_MATCH_PRODUCT, \
            0x0403, \
            0x6001, \
            0, \
            0, \
            0, \
            AW_USB_CLASS_VENDOR_SPEC, \
            0, \
            0, \
            0, \
        }

/** \brief QUECTEL */

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
/******************************************************************************/
const struct awbl_usbh_fundrv_info_tab awbl_usbh_wireless_info_tab[] = {
        __ANYDATA_MATCH_INFO(__ANYDATA_PRODUCT_ADU_620UW),
        __ANYDATA_MATCH_INFO(__ANYDATA_PRODUCT_ADU_E100A),
        __ANYDATA_MATCH_INFO(__ANYDATA_PRODUCT_ADU_500A),
        __ANYDATA_MATCH_INFO(__ANYDATA_PRODUCT_DTL718_W),
        __HUAWEI_MATCH_INFO(0x06, 0x06),
        __HUAWEI_MATCH_INFO(0x06, 0x10),
        __HUAWEI_MATCH_INFO(0x06, 0x12),
        __HUAWEI_MATCH_INFO(0x06, 0x13),
        __HUAWEI_MATCH_INFO(0x06, 0x16),
        __HUAWEI_MATCH_INFO(0x06, 0x1B),

        __LONGSHANG_MATCH_INFO(0x9603),
        __LONGSHANG_MATCH_INFO(0x9B05),
        __LONGSHANG_MATCH_INFO(0x9B3C),
        __LONGSHANG_MATCH_INFO(0x9E00),

        __QUECTEL_MATCH_INFO(0x05C6, 0x9215),
        __QUECTEL_MATCH_INFO(0x05C6, 0x9090),
        __QUECTEL_MATCH_INFO(0x05C6, 0x9003),
        __QUECTEL_MATCH_INFO(0x2C7C, 0x0125),
        __QUECTEL_MATCH_INFO(0x2C7C, 0x0121),
        __QUECTEL_MATCH_INFO(0x2C7C, 0x0191),
        __QUECTEL_MATCH_INFO(0x2C7C, 0x0195),

        __LONGSHANG_EX_MATCH_INFO(0x02, 0x12),
        __LONGSHANG_EX_MATCH_INFO(0x02, 0x14),
        __LONGSHANG_EX_MATCH_INFO(0x02, 0x13),
        __LONGSHANG_EX_MATCH_INFO(0x02, 0x03),
        __LONGSHANG_EX_MATCH_INFO(0x02, 0x0A),
        __LONGSHANG_EX_MATCH_INFO(0x02, 0x05),
        __LONGSHANG_EX_MATCH_INFO(0x02, 0x01),
        __FT232_INFO(),
        AWBL_USBH_FUNDRV_INFO_END
};


