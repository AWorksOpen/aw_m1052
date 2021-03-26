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

#ifndef __AWBL_USBH_WIRELESS_H
#define __AWBL_USBH_WIRELESS_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


#define AWBL_USBH_WIRELESS_DRV_NAME     "awbl_usbh_wireless_driver"







void *awbl_usbh_wireless_pipe_open (int     pid,
                                    int     vid,
                                    int     inum,
                                    aw_bool_t rngbuf,
                                    int     size,
                                    int     timeout);
void awbl_usbh_wireless_pipe_close (void *pipe);
int awbl_usbh_wireless_pipe_write (void *pipe, void *p_buf, int len, int timeout);
int awbl_usbh_wireless_pipe_read (void *pipe, void *p_buf, int len, int timeout);

void awbl_usbh_wireless_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */


#endif /* __AWBL_USBH_WIRELESS_H */
