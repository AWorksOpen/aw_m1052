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

#include "aworks.h"
#include "aw_task.h"
#include "aw_sem.h"
#include "aw_cache.h"
#include "aw_assert.h"
#include "aw_mem.h"
#include "aw_usb_common.h"
#include "aw_usb_mem.h"
#include "host/awbl_usbh.h"

#include "aw_usb_mem.c"
#include "aw_usb_os.c"
#include "awbl_usbh_dev_spec_config_info.c"

/* ECM设备信息表*/
#ifdef AW_DRV_USBH_CDC_ECM
#include "awbl_usbh_cdc_ecm_info.c"
#endif
/* USB转串口设备信息表*/
#ifdef AW_DRV_USBH_CDC_SERIAL
#include "awbl_usbh_cdc_serial_info.c"
#endif

#ifdef AW_DRV_USBH_WIRELESS
#include "awbl_usbh_wireless_info.c"
#endif

#if (defined(AW_COM_USBH) || \
     defined(AW_COM_USBD) || \
     defined(AW_COM_AWBL_USBH) || \
     defined(AW_COM_AWBL_USBD))
#ifndef AW_CFG_USB_MEM_SIZE
#define AW_CFG_USB_MEM_SIZE (32 * 1024)
#endif

#ifdef AW_COM_USB_MEM_STATIC
aw_local uint32_t __g_usb_mem[(AW_CFG_USB_MEM_SIZE + sizeof(uint32_t) - 1) / sizeof(uint32_t)];
#else
aw_local uint32_t *__g_usb_mem = NULL;
#endif

#ifndef AW_CFG_USB_HUB_STACK_SIZE
#define AW_CFG_USB_HUB_STACK_SIZE (10 * 1024)
#endif

#ifndef AW_CFG_USB_HUB_TASK_PRIO
#define AW_CFG_USB_HUB_TASK_PRIO 15
#endif

#endif



void awbl_usb_cfg (void)
{
    int size = AW_ROUND_UP(AW_CFG_USB_MEM_SIZE, sizeof(uint32_t));
#if (defined(AW_COM_USBH) || \
     defined(AW_COM_AWBL_USBH))
    aw_err_t ret;
#endif
    
#if (defined(AW_COM_USBH) || \
     defined(AW_COM_USBD) || \
     defined(AW_COM_AWBL_USBH) || \
     defined(AW_COM_AWBL_USBD))

#ifndef AW_COM_USB_MEM_STATIC
    __g_usb_mem = aw_mem_alloc(size);
    aw_assert(__g_usb_mem != NULL);
#endif
    aw_usb_mem_lib_init(__g_usb_mem, size);
#endif

#if (defined(AW_COM_USBH) || \
     defined(AW_COM_AWBL_USBH))
    ret = awbl_usbh_lib_init(AW_CFG_USB_HUB_STACK_SIZE, AW_CFG_USB_HUB_TASK_PRIO);
    aw_assert(ret == AW_OK);
#endif
}




