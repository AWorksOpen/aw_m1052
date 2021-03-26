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
#ifndef __AWBL_USBH_CDC_ECM_H
#define __AWBL_USBH_CDC_ECM_H

/* USB ECM´òÓ¡º¯Êý*/
#define __USB_ECM_TRACE(info, ...) \
            do { \
                AW_INFOF(("USBH-ECM:"info, ##__VA_ARGS__)); \
            } while (0)


#ifdef USB_ECM_DEBUG
    #define __USB_ECM_TRACE_DEBUG  __USB_ECM_TRACE
    #define usb_ecm_printf  aw_kprintf
#else
    #define __USB_ECM_TRACE_DEBUG(info) \
            do { \
            } while (0)
#endif


#endif
