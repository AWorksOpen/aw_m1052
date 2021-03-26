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


#ifndef __AWBL_SERIAL_USBH_WIRELESS_H
#define __AWBL_SERIAL_USBH_WIRELESS_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


#define AWBL_USBH_SERIAL_WIRELESS_DRV_NAME     "awbl_usbh_serial_wireless_driver"


/** \brief register USB wireless driver */
void awbl_usbh_serial_wireless_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_SERIAL_USBH_WIRELESS_H */
