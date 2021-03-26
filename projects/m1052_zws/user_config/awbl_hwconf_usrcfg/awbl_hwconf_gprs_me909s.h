/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      aworks.support@zlg.cn
*******************************************************************************/
#ifndef __AWBL_HWCONF_GPRS_ME909S_H
#define __AWBL_HWCONF_GPRS_ME909S_H

#ifdef AW_DEV_GPRS_ME909S

#include "driver/gprs/awbl_gprs_me909s.h"
//#include "awbl_gprs_me909s.h"

/*******************************************************************************
    配置信息
*******************************************************************************/

/** 硬件上电操作 */
aw_local void __gprs_me909s_power_on (void)
{
    /* 根据实际情况添加代码 */
}

/** 硬件掉电操作 */
aw_local void __gprs_me909s_power_off (void)
{
    /* 根据实际情况添加代码 */
}

/** 硬件复位操作 */
aw_local void __gprs_me909s_reset (void)
{
    /* 根据实际情况添加代码 */
}

/** 硬件写使能操作 */
aw_local void __gprs_me909s_write_en (void)
{
    /* 根据实际情况添加代码 */
}

/* GPRS模块的参数信息 */
aw_local struct awbl_gprs_info   __g_me909s_gprs_info = {
    .p_dial_num     = "*99***1#",
    .p_apn          = NULL,
    .p_user         = NULL,
    .p_password     = NULL,
    .p_center_num   = NULL,
    .p_at_err       = "ERROR",

    .pfn_power_on   = __gprs_me909s_power_on,
    .pfn_power_off  = __gprs_me909s_power_off,
    .pfn_reset      = __gprs_me909s_reset,
    .pfn_write_en   = __gprs_me909s_write_en,
};

/* GPRS ME909s 信息结构体 */
aw_local awbl_gprs_usbh_info __g_me909s_usbh_info = {
    .vid        = 0x12D1,
    .pid        = 0x15C1,
    .at_com     = 2,
    .ppp_com    = 0,
    .dm_com     = -1,
    .gps_com    = -1,
};

aw_local char __gprs_me909s_sms_buf[320];
aw_local char __gprs_me909s_num_buf[20];
aw_local char __gprs_me909s_time_buf[50];

/* GPRS 短信结构体 */
aw_local struct aw_gprs_sms_buf  __g_me909s_sms_info = {
    .p_sms_buff     = __gprs_me909s_sms_buf,
    .sms_buff_len   = sizeof(__gprs_me909s_sms_buf) - 1,

    .p_num_buff     = __gprs_me909s_num_buf,
    .num_buff_len   = sizeof(__gprs_me909s_num_buf) - 1,

    .p_time_buff     = __gprs_me909s_time_buf,
    .time_buff_len   = sizeof(__gprs_me909s_time_buf) - 1,
};

aw_local char __g_me909s_msg_buffer[1500];

/** ME909s设备信息 */
aw_local struct  awbl_gprs_me909s_devinfo __g_me909s_devinfo = {
    .id             = AWBL_GPRS_ME909S_ID_BASE,
    .buf            = __g_me909s_msg_buffer,
    .buf_size       = sizeof(__g_me909s_msg_buffer),
    .p_gprs_info    = &__g_me909s_gprs_info,
    .p_sms_info     = &__g_me909s_sms_info,
    .p_usbh_info    = &__g_me909s_usbh_info,
};

aw_local struct awbl_gprs_me909s_dev __g_me909s_dev;

#define AWBL_HWCONF_GPRS_ME909S                           \
    {                                                     \
        AWBL_GPRS_ME909S_NAME,                            \
        0,                                                \
        AWBL_BUSID_PLB,                                   \
        0,                                                \
        (struct awbl_dev *)&__g_me909s_dev.dev,           \
        &__g_me909s_devinfo                               \
    },

#else
#define AWBL_HWCONF_GPRS_ME909S

#endif    /* AW_DEV_GPRS_ME909S */

#endif  /* __AWBL_HWCONF_GPRS_ME909S_H */

/* end of file */
