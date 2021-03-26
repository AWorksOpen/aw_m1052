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
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_gprs.h"

#include <stdio.h>
#include <string.h>

#include "host/class/cdc/serial/awbl_usbh_cdc_serial.h"
#include "driver/gprs/awbl_gprs_gm510.h"


typedef void   (*aw_gprs_log_t) (void         *p_color,
                                 const char   *func_name,
                                 int           line,
                                 const char   *fmt, ...);

#define GM510_TRACE
#ifdef GM510_TRACE
/** GM510打印函数*/
#define __GM510_TRACE(info, ...) \
            do { \
                AW_INFOF(("GM510:"info, ##__VA_ARGS__)); \
            } while (0)
#else
/** GM510打印函数*/
#define __GM510_TRACE(info, ...) \
            do { \
            } while (0)
#endif
/** 获取GM510设备结构体*/
#define __GM510_DEV_DECL(p_this, p_dev) \
        struct awbl_gprs_gm510_dev *p_this = AW_CONTAINER_OF(p_dev, \
                                                           struct awbl_gprs_gm510_dev, \
                                                           dev)
/** 获取GM510设备信息*/
#define __GM510_DEVINFO_DECL(p_devinfo, p_dev) \
        struct awbl_gprs_gm510_devinfo *p_devinfo = \
                     ((struct awbl_gprs_gm510_devinfo *)AWBL_DEVINFO_GET(p_dev))
/** 从GPRS设备中获取GM510设备结构体*/
#define __GM510_DECL_FROM_GPRS(p_this, p_gprs) \
        struct awbl_gprs_gm510_dev *p_this = \
                     (struct awbl_gprs_gm510_dev *)p_gprs->p_gprs_dev

#define __LOG_FCOLOR_RED        "\x1B[31m"
#define __LOG_FCOLOR_GREEN      "\x1B[32m"

/* 获取打印函数*/
#define __LOG_DEV_DEF(p_gprs)  \
    aw_gprs_log_t _pfn_log_out_ = \
            (p_gprs ? ((struct awbl_gprs_dev *)p_gprs)->pfn_log_out : NULL )
/* 带颜色打印*/
#define __LOG_BASE(mask, color, fmt, ...)   \
    if ((mask) && _pfn_log_out_) {\
         _pfn_log_out_(color, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__);\
    }
#define __LOG_RES(is_true, mask, fmt, ...) \
    if (is_true) {\
        __LOG_BASE(mask, __LOG_FCOLOR_GREEN, "[ OK ] " fmt "\n", \
                     ##__VA_ARGS__);\
    } else {\
        __LOG_BASE(1, __LOG_FCOLOR_RED,\
                     "[ ER ] [%s:%d] " fmt "\n", \
                     __func__, __LINE__, ##__VA_ARGS__);\
    }
#define __LOG_PFX(mask, fmt, ...) \
    __LOG_BASE(mask, NULL, "[%s:%d] " fmt "\n", \
               __func__, __LINE__, ##__VA_ARGS__)

#define __ITERATE(EXP, COND, ntrys, ms_delay)   \
{\
    uint8_t __ntrys = ntrys;\
    while (__ntrys) {\
        EXP;\
        if (COND) {\
            break;\
        } else {\
            __ntrys--;\
            aw_mdelay(ms_delay);\
        }\
    }\
}

#define __at_cmd_send(OBJ, MSG)  if (OBJ->p_pipe) {awbl_gprs_at_send(&OBJ->gprs, MSG);}

/** \brief 打开 usbh 无线设备管道   */
#define __USBH_WIRELESS_OPEN(com) \
    __usbh_wireless_open(p_this->p_devinfo->p_usbh_info->pid,\
                         p_this->p_devinfo->p_usbh_info->vid,\
                         com, AW_TRUE, 512, 15000);

aw_local aw_const struct awbl_gprs_service __g_gprs_servfunc;

extern uint32_t utf8_to_ucs2 (const char   *utf8,
                               int           utf8_length,
                               char         *ucs2,
                               int           ucs2_length);
extern uint32_t ucs2_to_utf8 (const char   *ucs2,
                               int           ucs2_length,
                               char         *utf8,
                               int           utf8_length);

/* 检查字符串中是否含有中文*/
aw_local aw_bool_t __is_chinese (uint8_t *p_str)
{
    while (*p_str != '\0') {
        if (*p_str >= 0x80) {
            return AW_TRUE;
        } else {
            p_str++;
        }
    }
    return AW_FALSE;
}

/* 将 p_str 中的 p_substr 及其左边的字符串都去掉 */
aw_local aw_inline char *__lstrip (char *p_str, char *p_substr)
{
    char *p_begin;

    p_begin = strstr(p_str, p_substr);
    if (p_begin != NULL) {
        return p_begin + strlen(p_substr);
    }
    return  NULL;
}

/** 获取socket*/
static int __socket_get (struct awbl_gprs_gm510_dev *p_this)
{
    uint8_t *p_socket       = p_this->socket;
    uint8_t *p_socket_used  = p_this->socket_used;
    uint8_t  i = 0;

    for (i = 0; i < __INTERNAL_SOCKET_NUM; i++) {
        if (p_socket_used[i] == 0) {
            if (p_socket_used[i] < 255) {
                p_socket_used[i]++;
            }
            return p_socket[i];
        }
    }
    return -1;
}

/** 释放socket*/
static void __socket_put (struct awbl_gprs_gm510_dev *p_this,
                          uint8_t                    socket)
{
    uint8_t *p_socket       = p_this->socket;
    uint8_t *p_socket_used  = p_this->socket_used;
    uint8_t  i = 0;

    for (i = 0; i < __INTERNAL_SOCKET_NUM; i++) {
        if (p_socket[i] == socket) {
            if (p_socket_used[i] > 0) {
                p_socket_used[i]--;
                return ;
            }
        }
    }
}

/**
 * 获取GPRS模式
 */
aw_local awbl_gprs_ll_type_t
__usbh_wireless_ll_type_get (struct awbl_gprs_dev *p_gprs)
{
    return AWBL_GPRS_LL_TYPE_PPP;
}

/**
 *
 * \brief 根据 ll_type 自动获取 at 命令使用的管道
 */
aw_local uint8_t __at_com_num_get (struct awbl_gprs_dev *p_gprs)
{
    __GM510_DECL_FROM_GPRS(p_this, p_gprs);
    if (p_gprs->ll_type == AWBL_GPRS_LL_TYPE_ETH) {
        /* 以太网模式AT命令交互使用的 AT端口*/
        return p_this->p_devinfo->p_usbh_info->at_com;
    } else {
        /* PPP模式AT命令交互使用的 AT端口*/
        return p_this->p_devinfo->p_usbh_info->at_com;
    }
}

/**
 * 端口打开函数
 */
aw_local void *__usbh_wireless_open (int         pid,
                                     int         vid,
                                     int         inum,
                                     aw_bool_t   rngbuf,
                                     int         size,
                                     int         timeout)
{
    void    *p;
    uint32_t tm;
    (void)rngbuf;
    (void)size;
    (void)timeout;
    p = awbl_usbh_serial_open(pid, vid, inum);

//    tm = 50;
//    aw_usb_serial_timeout_set(p, &tm);

    return p;
}

/**
 * 端口写函数
 */
aw_local u32_t __usbh_wireless_write (void *fd, u8_t *data, u32_t len)
{
    return aw_usb_serial_write_sync(fd, data, len);
}

/**
 * 端口读函数
 */
aw_local u32_t __usbh_wireless_read (void *fd, u8_t *data, u32_t len)
{
    return aw_usb_serial_read_sync(fd, data, len);
}

/**
 * 端口关闭函数
 */
aw_local void __usbh_wireless_close (void *fd)
{
    awbl_usbh_serial_close(fd);
}

/** AT命令发送函数*/
aw_local ssize_t __gprs_at_cmd_send (struct awbl_gprs_dev *p_gprs, char *p_buf)
{
    __GM510_DECL_FROM_GPRS(p_this, p_gprs);
    if (p_this->p_pipe == NULL) {
        return 0;
    }
    return aw_usb_serial_write_sync(p_this->p_pipe, p_buf, strlen(p_buf));
}

/** AT命令接收函数*/
aw_local ssize_t __gprs_at_cmd_recv (struct awbl_gprs_dev   *p_gprs,
                                     char                   *p_buf,
                                     size_t                  buf_size)
{
    __GM510_DECL_FROM_GPRS(p_this, p_gprs);
    if (p_this->p_pipe == NULL) {
        return 0;
    }
    return aw_usb_serial_read_sync(p_this->p_pipe, p_buf, buf_size);
}

/** 等待接收回复*/
aw_local int __at_cmd_result_wait (struct awbl_gprs_gm510_dev *p_this,
                                   char *p_str,
                                   uint32_t ms_timeout)
{
    char *arg[1];

    arg[0] = p_str;
    /* 合法性检查*/
    if (p_this->p_pipe == NULL) {
        __GM510_TRACE("AT pipe is NULL\r\n");
        return -1;
    }

    return awbl_gprs_at_recv(&p_this->gprs,
                             p_this->p_devinfo->buf,
                             p_this->p_devinfo->buf_size,
                             arg,
                             1,
                             ms_timeout);
}

/* 检查GM510 设备是否有应答*/
aw_local aw_err_t __gm510_dev_check (struct awbl_gprs_gm510_dev *p_this)
{
    aw_err_t ret;

    if(p_this->p_pipe == NULL){
        return -AW_EINVAL;
    }

    __at_cmd_send(p_this, "AT\r");
    ret = __at_cmd_result_wait(p_this, "OK", 1000);
    return ret;
}

/** 获取GM510软件版本*/
aw_local aw_err_t __gm510_get_version(struct awbl_gprs_gm510_dev *p_this, char* p_buf){

    if((p_this == NULL) || (p_buf == NULL)){
        return -AW_EINVAL;
    }

    __at_cmd_send(p_this, "AT+ZVN\r");
    /* 等待回应带有“GM510”*/
    if (__at_cmd_result_wait(p_this, "\r\n\r\nOK\r\n", 10000) == AW_OK) {
        char *p_begin = NULL;
        char *p_end   = NULL;
        /* 移动到关键字地方*/
        p_begin = strstr(p_this->p_devinfo->buf, "GM510");
        if(p_begin){
            /* 检查后面有没有“OK”，有的话去掉*/
            p_end = strstr(p_begin, "\r\n\r\nOK\r\n");
            if(p_end != NULL){
                *p_end = '\0';
            }
            memcpy(p_buf, p_begin, strlen(p_begin));

            return AW_OK;
        }
    }
    return -AW_ECANCELED;
}

/** 获取GM510 SIM卡ICCID*/
aw_local aw_err_t __gm510_get_ICCID(struct awbl_gprs_gm510_dev *p_this, char* p_buf){
    int i;
    uint32_t timeout, timeout_temp;

    if((p_this == NULL) || (p_buf == NULL)){
        return -AW_EINVAL;
    }

    timeout_temp = 500;
    aw_usb_serial_timeout_get(p_this->p_pipe, &timeout);
    aw_usb_serial_timeout_set(p_this->p_pipe, &timeout_temp);
    for(i = 0;i < 3;i++){
        __gprs_at_cmd_recv(&p_this->gprs, p_this->p_devinfo->buf, p_this->p_devinfo->buf_size);
    }
    aw_usb_serial_timeout_set(p_this->p_pipe, &timeout);
    __at_cmd_send(p_this, "AT+ZGETICCID\r");
    if (__at_cmd_result_wait(p_this, "\r\n\r\nOK\r\n", 10000) == AW_OK) {
        char *p_begin = NULL;
        char *p_end   = NULL;

        p_begin = strstr(p_this->p_devinfo->buf, "+ZGETICCID:");
        if(p_begin){
            /* 检查后面有没有“OK”，有的话去掉*/
            p_end = strstr(p_begin, "\r\n\r\nOK\r\n");
            if(p_end != NULL){
                *p_end = '\0';
            }
            memcpy(p_buf, p_begin + 11, strlen(p_begin) - 11);
            return AW_OK;
        }
    }
    return -AW_ECANCELED;
}

/** 获取GM510 IMEI*/
aw_local aw_err_t __gm510_get_IMEI(struct awbl_gprs_gm510_dev *p_this, char* p_buf){
    int i;
    uint32_t timeout, timeout_temp;

    if((p_this == NULL) || (p_buf == NULL)){
        return -AW_EINVAL;
    }
    timeout_temp = 500;
    aw_usb_serial_timeout_get(p_this->p_pipe, &timeout);
    aw_usb_serial_timeout_set(p_this->p_pipe, &timeout_temp);
    for(i = 0;i < 3;i++){
        __gprs_at_cmd_recv(&p_this->gprs, p_this->p_devinfo->buf, p_this->p_devinfo->buf_size);
    }
    aw_usb_serial_timeout_set(p_this->p_pipe, &timeout);
    __at_cmd_send(p_this, "AT+GSN\r");
    if (__at_cmd_result_wait(p_this, "\r\n\r\nOK\r\n", 10000) == AW_OK) {
        char *p_begin = NULL;
        char *p_end   = NULL;
        int j = 0;

        while(p_this->p_devinfo->buf[j] != '\0'){
            if((p_this->p_devinfo->buf[j] != '\r') && (p_this->p_devinfo->buf[j] != '\n') &&
                    (p_this->p_devinfo->buf[j] != 'O')  && (p_this->p_devinfo->buf[j] != 'K')){
                p_begin = &p_this->p_devinfo->buf[j];
                goto __find_imei;
            }
            j++;
        }
        return -AW_ECANCELED;
__find_imei:
        p_end = strstr(p_begin, "\r\n\r\nOK\r\n");
        if(p_end != NULL){
            *p_end = '\0';
        }

        memcpy(p_buf, p_begin, strlen(p_begin));

        return AW_OK;
    }
    return -AW_ECANCELED;
}

/** 获取GM510 IMSI*/
aw_local aw_err_t __gm510_get_IMSI(struct awbl_gprs_gm510_dev *p_this, char* p_buf){
    int i;
    uint32_t timeout, timeout_temp;

    if((p_this == NULL) || (p_buf == NULL)){
        return -AW_EINVAL;
    }

    timeout_temp = 500;
    aw_usb_serial_timeout_get(p_this->p_pipe, &timeout);
    aw_usb_serial_timeout_set(p_this->p_pipe, &timeout_temp);
    for(i = 0;i < 3;i++){
        __gprs_at_cmd_recv(&p_this->gprs, p_this->p_devinfo->buf, p_this->p_devinfo->buf_size);
    }
    aw_usb_serial_timeout_set(p_this->p_pipe, &timeout);
    __at_cmd_send(p_this, "AT+CIMI\r");
    if (__at_cmd_result_wait(p_this, "\r\n\r\nOK\r\n", 10000) == AW_OK) {
        char *p_begin = NULL;
        char *p_end   = NULL;
        int j = 0;

        while(p_this->p_devinfo->buf[j] != '\0'){
            if((p_this->p_devinfo->buf[j] != '\r') && (p_this->p_devinfo->buf[j] != '\n') &&
                    (p_this->p_devinfo->buf[j] != 'O')  && (p_this->p_devinfo->buf[j] != 'K')){
                p_begin = &p_this->p_devinfo->buf[j];
                goto __find_imsi;
            }
            j++;
        }
        return -AW_ECANCELED;
__find_imsi:
        p_end = strstr(p_begin, "\r\n\r\nOK\r\n");
        if(p_end != NULL){
            *p_end = '\0';
        }
        memcpy(p_buf, p_begin, strlen(p_begin));

        return AW_OK;
    }
    return -AW_ECANCELED;
}

/** 获取GM510 短信中心号码*/
aw_local aw_err_t __gm510_get_SCA(struct awbl_gprs_gm510_dev *p_this, char* p_buf){
    int i;
    uint32_t timeout, timeout_temp;

    if((p_this == NULL) || (p_buf == NULL)){
        return -AW_EINVAL;
    }

    timeout_temp = 500;
    aw_usb_serial_timeout_get(p_this->p_pipe, &timeout);
    aw_usb_serial_timeout_set(p_this->p_pipe, &timeout_temp);
    for(i = 0;i < 3;i++){
        __gprs_at_cmd_recv(&p_this->gprs, p_this->p_devinfo->buf, p_this->p_devinfo->buf_size);
    }
    aw_usb_serial_timeout_set(p_this->p_pipe, &timeout);

    __at_cmd_send(p_this, "AT+CSCA?\r");
    if (__at_cmd_result_wait(p_this, "\r\n\r\nOK\r\n", 10000) == AW_OK) {
        char *p_begin = NULL;
        char *p_end   = NULL;

        p_begin = strstr(p_this->p_devinfo->buf, "+CSCA:");
        if(p_begin){
            /* 检查后面有没有“OK”，有的话去掉*/
            p_end = strstr(p_begin, "\r\n\r\nOK\r\n");
            if(p_end != NULL){
                *p_end = '\0';
            }
            memcpy(p_buf, p_begin + 6, strlen(p_begin) - 6);
            return AW_OK;
        }
    }
    return -AW_ECANCELED;
}

/** GM510 AT命令传输*/
aw_local aw_err_t __gm510_at_trans(struct awbl_gprs_gm510_dev *p_this, aw_gprs_ioctl_args_t *p_arg){
    int i;
    uint32_t timeout, timeout_temp;

    if((p_arg == NULL) || (p_arg->at_trans.p_at_cmd == NULL)){
        __GM510_TRACE("invalid p_arg\r\n");
        return -AW_EINVAL;
    }

    timeout_temp = 500;
    aw_usb_serial_timeout_get(p_this->p_pipe, &timeout);
    aw_usb_serial_timeout_set(p_this->p_pipe, &timeout_temp);
    for(i = 0;i < 3;i++){
        __gprs_at_cmd_recv(&p_this->gprs, p_this->p_devinfo->buf, p_this->p_devinfo->buf_size);
    }
    aw_usb_serial_timeout_set(p_this->p_pipe, &timeout);

    __at_cmd_send(p_this, p_arg->at_trans.p_at_cmd);
    if((p_arg->at_trans.p_recv_expect != NULL) &&
            (p_arg->at_trans.p_recv_buf != NULL) &&
            (p_arg->at_trans.recv_tm > 0)){
        memset(p_arg->at_trans.p_recv_buf, 0, p_arg->at_trans.bufsize);
        if (__at_cmd_result_wait(p_this, p_arg->at_trans.p_recv_expect,
                p_arg->at_trans.recv_tm) == AW_OK){
            memcpy(p_arg->at_trans.p_recv_buf, p_this->p_devinfo->buf, p_arg->at_trans.bufsize);
            return AW_OK;
        }
        return -AW_ECANCELED;
    }
    return AW_OK;
}

/** 查询国际移动台设备标识 IMSI */
aw_local aw_err_t __cimi_check (struct awbl_gprs_gm510_dev *p_this)
{
    char *p_res_dat[16];
    int   ret;

    /* 合法性检查*/
    if(p_this->p_pipe == NULL){
        return -AW_EINVAL;
    }

    __at_cmd_send(p_this, "AT+CIMI\r");
    p_res_dat[0] = "46000";                 /* 移动 GSM */
    p_res_dat[1] = "46002";                 /* 移动 TD-S */
    p_res_dat[2] = "46007";                 /* 移动 TD-S */
    p_res_dat[8] = "46004";                 /* 移动 TD-S */

    p_res_dat[3] = "46001";                 /* 联通 GSM */
    p_res_dat[4] = "46006";                 /* 联通 WCDMA */

    p_res_dat[5] = "46003";                 /* 电信 CDMA */
    p_res_dat[6] = "46005";                 /* 电信 CDMA */
    p_res_dat[7] = "46011";                 /* 电信 FDD-LTE */

    ret = awbl_gprs_at_recv(&p_this->gprs, p_this->p_devinfo->buf,
                            p_this->p_devinfo->buf_size, p_res_dat, 9, 5000);
    if (ret == 0 || ret == 1 || ret == 2 || ret == 8) {
        p_this->p_devinfo->p_gprs_info->p_apn = "CMIOT";
    } else if (ret == 3 || ret == 4) {
        p_this->p_devinfo->p_gprs_info->p_apn = "3GNET";
    } else if (ret == 5 || ret == 6 || ret == 7) {
        p_this->p_devinfo->p_gprs_info->p_apn = "CTNET";
    } else {
        return -AW_ENODEV;
    }

    return AW_OK;
}

/** 查询网络运营商，返回0是移动， 返回1是联通 */
aw_local aw_err_t __cops_check (struct awbl_gprs_gm510_dev *p_this)
{
    char *p_res_dat[4];
    int   ret;

    if(p_this->p_pipe == NULL){
        return -AW_EINVAL;
    }

    __at_cmd_send(p_this, "AT+COPS=3,0\r");
    ret = __at_cmd_result_wait(p_this, "OK", 2000);

    __at_cmd_send(p_this, "AT+COPS?\r");
    p_res_dat[0] = "MOBILE";                /* 移动 */
    p_res_dat[1] = "UNICOM";                /* 联通 */
    p_res_dat[2] = "CHINA TELECOM";         /* 电信 */
    p_res_dat[3] = "CHN-CT";                /* 电信 */

    ret = awbl_gprs_at_recv(&p_this->gprs, p_this->p_devinfo->buf,
                            p_this->p_devinfo->buf_size, p_res_dat, 4, 2000);
    if (ret == 0) {
        p_this->p_devinfo->p_gprs_info->p_apn = "CMIOT";
    } else if (ret == 1) {
        p_this->p_devinfo->p_gprs_info->p_apn = "3GNET";
    } else if (ret == 2 || ret == 3) {
        p_this->p_devinfo->p_gprs_info->p_apn = "CTNET";
    } else {
        return -AW_ENODEV;
    }

    return AW_OK;
}

/** 获取GM510 状态*/
aw_local int gm510_status_is(struct awbl_gprs_gm510_dev *p_this, int status)
{
    if (p_this == NULL) {
        return -AW_EFAULT;
    }
    return ((p_this->status & status) == status);
}

/** 清除GM510 设备状态*/
aw_local void gm510_status_clr(struct awbl_gprs_gm510_dev *p_this, int status)
{

    if (p_this == NULL) {
        __GM510_TRACE("dev is NULL!\r\n");
        return ;
    }
    if (status == 0) {
        p_this->status = 0;
    } else {
        p_this->status &= ~status;
    }
}

/** 设置GM510 设备状态*/
aw_local void gm510_status_set(struct awbl_gprs_gm510_dev *p_this, int status)
{
    if (p_this == NULL) {
        return;
    }

    p_this->status |= status;
}

#if 0
/**
 * 使用内部协议栈进行PPP连接
 *
 * @param this 设备结构体变量
 *
 * @return aw_err_t的错误类型
 */
aw_local aw_err_t __set_mipcall (struct awbl_gprs_gm510_dev *p_this)
{
    char *buf     = p_this->p_devinfo->buf;
    u32_t buf_len = p_this->p_devinfo->buf_size;
    int      err = -1;
    char    *p_res_dat[2];

    if (p_this == NULL) {
        return -AW_EFAULT;
    }

    if (gm510_status_is(p_this, AWBL_GPRS_GM510_GPRS_PPP_OK) == AW_TRUE) {
        return AW_OK;
    }

    memset(buf, 0, buf_len);

    /*
     * "AT+ZSNT?\r": 查询当前网络模式，不是自动则设置搜网模式为自动
     *   0  AUTO模式（LTE-TDSCDMA-GSM）增加搜网顺序
     *   1  GSM ONLY，只注册GSM网
     *   2  WCDMA_ONLY
     *   3  TDSCDMA_ONLY(暂不支持)
     *   4  CDMA ONLY(暂不支持)
     *   5  HDR ONLY(暂不支持)
     *   6  LTE ONLY，只注册LTE网络
     *   7  WCDMA AND LTE ONLY(暂不支持)
     *   8  GSM WCDMA AND LTE ONLY(暂不支持)
     *   9  CDMA AND HDR ONLY(暂不支持)
     *   10 WCDMA_LTE ONLY(暂不支持)
     */
    if (p_this->net_mode != 2) {
        __at_cmd_send(p_this, "AT+ZSNT=0\r");
        if (__at_cmd_result_wait(p_this, "OK", 5000) != AW_OK) {
            return -AW_ENOMSG;
        }
        p_this->net_mode = 2;
    }
    /* 配置IP CALL*/
    if (p_this->p_devinfo->p_gprs_info->p_apn) {
        sprintf(buf, "AT+ZIPCFG=\"%s\"\r", p_this->p_devinfo->p_gprs_info->p_apn);
        __at_cmd_send(p_this, buf);
        if (__at_cmd_result_wait(p_this, "OK", 3000) != AW_OK) {
            return -AW_ENOMSG;
        }
    }
    /* 检查PPP状态*/
    __at_cmd_send(p_this, "AT+ZIPCALL?\r");
    p_res_dat[0] = "+ZIPCALL:0";
    p_res_dat[1] = "+ZIPCALL:1";

    err = awbl_gprs_at_recv(&p_this->gprs, p_this->p_devinfo->buf,
            p_this->p_devinfo->buf_size, p_res_dat, 2, 5000);
    /* res = 0 :PPP处于断开状态，则建立PPP连接   res = 1: ppp已连接*/
    if (err == 0) {
        __at_cmd_send(p_this, "AT+ZIPCALL=1\r");
        if (__at_cmd_result_wait(p_this, "+ZIPCALL:1", 5000) != AW_OK) {
            return -AW_ENOMSG;
        }
        /* 提取ppp_ip */
        buf = __lstrip(p_this->p_devinfo->buf, "+ZIPCALL:1,");
#if AWBL_GPRS_USE_LWIP_1_4
        inet_aton(buf, &(p_this->gprs.ppp.p_pppaddr->our_ipaddr));
#endif /* AWBL_GPRS_USE_LWIP_1_4 */

    } else if (err <= -1) {
        return -AW_ENOMSG;
    }

    gm510_status_set(p_this, AWBL_GPRS_GM510_GPRS_PPP_OK);
    return AW_OK;
}

#endif

///**
// * 使用at命令进行ping测试
// *
// * @param this     设备结构体变量
// * @param p_dns    目标域名
// *
// * @return aw_err_t的错误类型
// */
//aw_local aw_err_t __gm510_dev_gprs_ping (struct awbl_gprs_gm510_dev *p_this,
//                                         char                       *p_dns)
//{
//    if (p_this == NULL) {
//        return -AW_EFAULT;
//    }
//
//    char *buf     = p_this->p_devinfo->buf;
//    int      err = AW_OK;
//
//    err = __set_mipcall(p_this);
//    /* 初始化GPRS的功能 */
//    if (err != AW_OK) {
//        return err;
//    }
//
//    if (p_dns[0] == '\"') {      /* 参数错误 */
//        return -AW_EINVAL;
//    }
//    sprintf(buf, "AT+MPING=\"%s\"\r", p_dns);
//    __at_cmd_send(p_this, buf);
//    /* 返回OK 接着返回4次+MPING的结果 最后返回+MPINGSTATE状态值 */
//    if (__at_cmd_result_wait(p_this, "+MPINGSTATE:0,4,4", 30000) != AW_OK) {
//        err = -AW_ENOMSG;
//    }
//
//    return err;
//}

/* 获取网络服务类型*/
aw_local aw_err_t __gm510_dev_net_mode_get (struct awbl_gprs_gm510_dev *p_this,
                                            char                       *p_to)
{
    if (p_this == NULL) {
        return -AW_EFAULT;
    }

    char *buf     = p_this->p_devinfo->buf;

    __at_cmd_send(p_this, "AT+ZPAS?\r");     /* 查询网络服务类型*/
    if (__at_cmd_result_wait(p_this, "OK", 3000) == AW_OK) {
        strncpy(p_to, buf, 16);
        return AW_OK;
    }

    return -AW_ENOMSG;
}

/* 获取基站id */
aw_local aw_err_t __gm510_dev_base_station_query (
                                            struct awbl_gprs_gm510_dev *p_this,
                                            uint32_t    *p_dat)
{
    u32_t                    cnt = 0;
    struct awbl_gprs_dev    *p_gprs = &p_this->gprs;
    __LOG_DEV_DEF(p_gprs);

    if (p_this == NULL) {
        return -AW_EFAULT;
    }

    char *buf     = p_this->p_devinfo->buf;
    char *p_addr = 0;

    /* socket正在连接，不允许切换搜网模式*/
    if (gm510_status_is(p_this, AWBL_GPRS_GM510_GPRS_PPP_OK) ||
            gm510_status_is(p_this, AWBL_GPRS_GM510_GPRS_PDP_OK)) {
        __GM510_TRACE("socket is conneting\r\n");
        return -AW_EPERM;
    }

    if (p_this->net_mode != 3) {
        /* 设置搜网模式为2G网 */
        __at_cmd_send(p_this, "AT+ZSNT=1\r");
        if (__at_cmd_result_wait(p_this, "OK", 5000) != AW_OK) {
            return -AW_ENOMSG;
        }
        p_this->net_mode = 3;
    }
    /* 使能提示*/
    __at_cmd_send(p_this, "AT+CREG=2\r");
    if (__at_cmd_result_wait(p_this, "OK", 5000) != AW_OK) {
        return -AW_ENOMSG;
    }
    /* 查询网络注册状态*/
    do {
        __at_cmd_send(p_this, "AT+CREG?\r");
        if (__at_cmd_result_wait(p_this, "OK", 5000) != AW_OK) {
            return -AW_ENOMSG;
        }
        /* 获取基站IP地址*/
        p_addr = __lstrip(buf, "+CREG: 2,1,\"");
        if (p_addr == NULL) {
            return -AW_ENODATA;
        }
        p_dat[0] = strtol(p_addr, 0, 16);
        aw_mdelay(1000);
    } while ((p_dat[0] == 0xfffe || p_dat[0] == 0) && ++cnt < 8 );

    p_addr = __lstrip(p_addr, ",\"");
    if (p_addr == NULL) {
        return -AW_ENODATA;
    }
    p_dat[1] = strtol(p_addr, 0, 16);

    return AW_OK;
}

/** 设置PDP */
aw_local aw_err_t __gm510_dev_ppp_pdp_set (struct awbl_gprs_dev *p_gprs)
{
    __GM510_DECL_FROM_GPRS(p_this, p_gprs);
    aw_err_t   ret;
    void *p_pipe_bp = p_this->p_pipe;
    char buf[256] = {0};
    aw_bool_t is_out = AW_FALSE;
    aw_bool_t is_set_pdp = AW_TRUE;
    aw_bool_t is_dailup = AW_FALSE;
    __LOG_DEV_DEF(p_gprs);
    int i;
    uint32_t timeout, timeout_temp;

    p_this->p_pipe = p_this->gprs.ppp.sio_mgn.fd;
    /* 清除PDP状态*/
    gm510_status_clr(p_this, AWBL_GPRS_GM510_GPRS_PDP_OK);

    __GM510_TRACE("connect...\r\n");

//    aw_mdelay(1000);
//    __at_cmd_send(p_this, "+++\r");
//    aw_mdelay(1000);
//    __at_cmd_send(p_this, "ATH\r");

    /* 检测APN*/
    if (p_this->p_devinfo->p_gprs_info->p_apn == NULL) {
        __ITERATE(ret = __cimi_check(p_this), ret == AW_OK, 3, 0);
        if (ret < 0) {
            __ITERATE(ret = __cops_check(p_this), ret == AW_OK, 1, 0);
            if (ret != AW_OK) {
                __GM510_TRACE("detect APN failed\r\n");
                goto _err_exit;
            }
        }
    }

    timeout_temp = 500;
    aw_usb_serial_timeout_get(p_this->p_pipe, &timeout);
    aw_usb_serial_timeout_set(p_this->p_pipe, &timeout_temp);
    /* 查询网络注册状态*/
    __at_cmd_send(p_this, "AT+CREG?\r");
    if (__at_cmd_result_wait(p_this, "0,1", 3000) != AW_OK) {
        __LOG_RES(0, 1, "net register failed");
        is_out = AW_TRUE;
    }
    for(i = 0;i < 3;i++){
        __gprs_at_cmd_recv(&p_this->gprs, p_this->p_devinfo->buf, p_this->p_devinfo->buf_size);
    }
    /* 查询MT附着GPRS业务状态*/
    __at_cmd_send(p_this, "AT+CGATT?\r");
    if (__at_cmd_result_wait(p_this, "OK", 3000) == AW_OK) {
        char *p_begin = NULL;

        p_begin = __lstrip(p_this->p_devinfo->buf, "+CGATT: ");
        if(p_begin != NULL){
            if(p_begin[0] == '1'){
                __LOG_RES(1, 1, "CGATT 1");
                is_dailup = AW_TRUE;
            }
            else if(p_begin[0] == '0'){
                __LOG_RES(1, 1, "CGATT 0");
            }
        }
    }
    else{
        __LOG_RES(1, 1, "AT+CGATT? no response");
    }
    __gprs_at_cmd_recv(&p_this->gprs, p_this->p_devinfo->buf, p_this->p_devinfo->buf_size);
    /* 查询PDP激活状态*/
    __at_cmd_send(p_this, "AT+CGACT?\r");
    if (__at_cmd_result_wait(p_this, "OK", 3000) == AW_OK) {
        char *p_begin = NULL;

        p_begin = __lstrip(p_this->p_devinfo->buf, "+CGACT: ");
        if(p_begin != NULL){
            if((strncmp(p_begin, "1,1", 3) == 0) && (is_out == 0)){
                __LOG_RES(1, 1, "PDP already active");
                is_set_pdp = AW_FALSE;
            }
            else if(strncmp(p_begin, "0,1", 3) == 0){
                __LOG_RES(1, 1, "PDP inactive");
            }
            else{
                __LOG_RES(1, 1, "PDP cid 1 not exist");
            }
        }
    }
    else{
        __LOG_RES(1, 1, "AT+CGACT? no response");
    }
    /* 网络注册失败，直接退出*/
    if(is_out == AW_TRUE){
        aw_usb_serial_timeout_set(p_this->p_pipe, &timeout);
        p_this->p_pipe = p_pipe_bp;
        return -AW_ENOMSG;
    }
    /* MT附着GPRS业务，直接拨号*/
    if((is_dailup == AW_TRUE) && (is_set_pdp == AW_FALSE)){
        goto __dailup;
    }

    aw_usb_serial_timeout_set(p_this->p_pipe, &timeout);
    if (p_this->p_devinfo->p_gprs_info->p_apn) {
        if(is_set_pdp == AW_TRUE){
            /* 定义PDP上下文*/
            sprintf(buf, "AT+CGDCONT=1,\"IP\",\"%s\"\r", p_this->p_devinfo->p_gprs_info->p_apn);
            __at_cmd_send(p_this, buf);
            if (__at_cmd_result_wait(p_this, "OK", 2000) != AW_OK) {
                /* 失败则再发一次*/
                aw_mdelay(1000);
                sprintf(buf, "AT+CGDCONT=1,\"IP\",\"%s\"\r", p_this->p_devinfo->p_gprs_info->p_apn);
                __at_cmd_send(p_this, buf);
                if (__at_cmd_result_wait(p_this, "OK", 3000) != AW_OK) {
                    ret = -AW_ENOMSG;
                    __GM510_TRACE("PDP set failed\r\n");
                    goto _err_exit;
                }
            }
        }
        /* MT附着GPRS业务*/
        __at_cmd_send(p_this, "AT+CGATT=1\r");
        if (__at_cmd_result_wait(p_this, "OK", 120000) != AW_OK) {
            ret = -AW_ENOMSG;
            goto _err_exit;
        }
    }
    /* 查询网络注册状态*/
    __at_cmd_send(p_this, "AT+CREG?\r");
    if (__at_cmd_result_wait(p_this, "0,1", 3000) != AW_OK) {
        ret = -AW_ENOMSG;
        __GM510_TRACE("net register failed\r\n");
        goto _err_exit;
    }
    __at_cmd_result_wait(p_this, "OK", 500);
__dailup:
    /* 拨号*/
    __at_cmd_send(p_this, "ATD*99#\r");
    if (__at_cmd_result_wait(p_this, "CONNECT", 5000) != AW_OK) {
        ret = -AW_ENOMSG;
        __GM510_TRACE("connect failed\r\n");
        goto _err_exit;
    }
    /* 设置PDP设置成功状态*/
    gm510_status_set(p_this, AWBL_GPRS_GM510_GPRS_PDP_OK);
    __GM510_TRACE("PDP set success\r\n");
    ret = AW_OK;

_err_exit:
    p_this->p_pipe = p_pipe_bp;
    return ret;
}

/** 呼叫挂起（数据）   */
aw_local aw_err_t __gm510_dev_ppp_dial_down (struct awbl_gprs_dev *p_gprs)
{
    __GM510_DECL_FROM_GPRS(p_this, p_gprs);
    void *p_pipe_bp = p_this->p_pipe;

    p_this->p_pipe = p_this->gprs.ppp.sio_mgn.fd;
    aw_mdelay(1000);
    __at_cmd_send(p_this, "+++");
    aw_mdelay(1000);

    p_this->p_pipe = p_pipe_bp;
    /* 该指令终止当前所有数据通话 */
    __at_cmd_send(p_this, "ATH\r");
    if (__at_cmd_result_wait(p_this, "OK", 1000) != AW_OK) {
        return -AW_EBUSY;
    }
    return AW_OK;
}

/**
 * AT命令的回显设置
 *
 * @param  this    设备结构体变量
 * @param  ate     0：关回显；  1：开回显
 * @param  atv     0：返回数字型；  1：返回字符型
 *
 * @return 成功返回AW_OK
 */
aw_local aw_err_t __gm510_dev_at_disp_set (struct awbl_gprs_gm510_dev *p_this,
                                          char                       ate)
{
    aw_err_t ret;

    if ((p_this == NULL) || (p_this->p_pipe == NULL)) {
        return -AW_EFAULT;
    }

    char *buf = p_this->p_devinfo->buf;

    sprintf(buf, "ATE%d\r", ate);
    __at_cmd_send(p_this, buf);
    ret = __at_cmd_result_wait(p_this, "OK", 3000);
    return ret;
}

/** 检测信号强度 */
aw_local aw_err_t __gm510_dev_csq_check (struct awbl_gprs_dev *p_gprs, int *p_csq)
{
    __GM510_DECL_FROM_GPRS(p_this, p_gprs);

    char *buf     = p_this->p_devinfo->buf;

    __at_cmd_send(p_this, "AT+CSQ\r");
    if (__at_cmd_result_wait(p_this, "+CSQ:", 5000) != AW_OK) {
        return -AW_ENOMSG;
    }
    /* 获取信号值 */
    *p_csq = (uint8_t)strtol(strstr(buf, "+CSQ:") + 5, 0, 0);
    /* 如果遇到 +CSQ: 99 则表示没有信号 */
    if (*p_csq == 99 || *p_csq < p_gprs->csq_min) {
        return -AW_EAGAIN;
    }
    else{
        __GM510_TRACE("CSQ:%d\r\n", *p_csq);
    }
    return AW_OK;
}

/**
 * 发送复位的AT命令
 *
 * @param  this     设备结构体变量
 * @param  opt      opt=0:软件复位 ; opt=1：硬件复位
 *
 * @return 成功返回AW_OK
 */
aw_local aw_err_t __gm510_dev_reset (struct awbl_gprs_dev *p_gprs,
                                     uint32_t              opt)
{
    __GM510_DECL_FROM_GPRS(p_this, p_gprs);
    int ret;

    if (opt == 0) {
        if (p_this == NULL) {
            return -AW_EFAULT;
        }
        __GM510_TRACE("soft reset\r\n");
        __at_cmd_send(p_this, "AT+CFUN=1,1\r");
        __usbh_wireless_close(p_this->p_pipe);
        aw_mdelay(30000);
    } else {
        if (p_this->p_devinfo->p_gprs_info->pfn_reset) {
            p_this->p_devinfo->p_gprs_info->pfn_reset();
        }
    }
    /* 清除状态*/
    gm510_status_clr(p_this, 0);

    /* 重新打开AT管道*/
    p_this->p_pipe = __USBH_WIRELESS_OPEN(__at_com_num_get(p_gprs));
    if (p_this->p_pipe == NULL) {
        ret = -AW_ENODEV;
        goto failed1;
    }
    /* 重新打开PPP管道*/
    p_this->gprs.ppp.sio_mgn.fd =
            __USBH_WIRELESS_OPEN(p_this->p_devinfo->p_usbh_info->ppp_com);
    if (p_this->gprs.ppp.sio_mgn.fd == NULL) {
        ret = -AW_ENOMEM;
        goto failed2;
    }
    /* 检查是否响应*/
    __ITERATE(ret = __gm510_dev_check(p_this), ret == AW_OK, 5, 0);
    if (ret != AW_OK) {
        goto failed2;
    }

    return AW_OK;

failed2:
    if (p_this->gprs.ppp.sio_mgn.fd) {
        __usbh_wireless_close(p_this->gprs.ppp.sio_mgn.fd);
    }
    __usbh_wireless_close(p_this->p_pipe);
failed1:
    return ret;
}

/**
 * GM510 探测函数
 *
 * @param p_gprs GPRS设备结构体
 *
 * @return 成功返回AW_TRUE
 */
aw_local aw_bool_t __gm510_dev_probe (struct awbl_gprs_dev *p_gprs)
{
    void *ptr;
    __GM510_DECL_FROM_GPRS(p_this, p_gprs);

    /* 合法性检查*/
    if((p_gprs == NULL) ||
       (p_this == NULL) ||
       (p_this->p_devinfo == NULL) ||
       (p_this->p_devinfo->p_usbh_info == NULL)){
        return AW_FALSE;
    }

    /* 检查GPRS是否已经上电，且调用GPRS上电函数*/
    if (!p_gprs->power_on && p_gprs->p_info->pfn_power_on) {
        p_gprs->p_info->pfn_power_on();
    }
    /* 检查能不能打开AT管道*/
    ptr = __USBH_WIRELESS_OPEN(__at_com_num_get(p_gprs));
    if (ptr) {
        __usbh_wireless_close(ptr);
        return AW_TRUE;
    }

    return AW_FALSE;
}

/** 初始化PDU SMS*/
aw_local aw_err_t __gm510_dev_init_pdu_sms (struct awbl_gprs_gm510_dev *p_this)
{
    int   err = 0;

    /* 设置为 pdu 格式 */
    __at_cmd_send(p_this, "AT+CMGF=0\r");
    if (__at_cmd_result_wait(p_this, "OK", 5000) != AW_OK) {
        return -AW_ENOMSG;
    }
    /* 设置TE字符集：UCS2*/
    __at_cmd_send(p_this, "AT+CSCS=\"UCS2\"\r");
    if (__at_cmd_result_wait(p_this, "OK", 5000) != AW_OK) {
        return -AW_ENOMSG;
    }
    return AW_OK;
}

/**
 * 初始化设置发送TEXT短信功能
 *
 * @param  this     设备结构体变量
 *
 * @return 成功返回AW_OK
 */
aw_local aw_err_t __gm510_dev_init_text_sms (struct awbl_gprs_gm510_dev *p_this)
{
    if (p_this == NULL) {
        return -AW_EINVAL;
    }

    int   err = 0;
    int   csq = 0;

    /* 设置为TEXT模式*/
    __at_cmd_send(p_this, "AT+CMGF=1\r");
    if (__at_cmd_result_wait(p_this, "OK", 5000) != AW_OK) {
        return -AW_ENOMSG;
    }
    /* 设置TE字符集：GSM缺省符号集*/
    awbl_gprs_at_send(&p_this->gprs, "AT+CSCS=\"GSM\"\r");
    if (__at_cmd_result_wait(p_this, "OK", 5000) != AW_OK) {
        return -AW_ENOMSG;
    }

    return AW_OK;
}

/** 发送短信*/
aw_local aw_err_t __pdu_sms_send (struct awbl_gprs_dev *p_gprs,
                                  uint8_t              *p_sms,
                                  uint8_t               len,
                                  char                 *p_num,
                                  uint8_t               code)
{
    __GM510_DECL_FROM_GPRS(p_this, p_gprs);
    char     buf[370] = { 0 };
    int      err  = 0;
    char ucs2_temp[256] = {0};
    uint32_t ucs2_len = 0;
    /*  短信结束 */
    aw_const char GPRS_MSG_END[]={0x1A,0x00};

    /* 初始化PDU SMS*/
    err = __gm510_dev_init_pdu_sms(p_this);
    if (err != AW_OK) {
        __GM510_TRACE("init sms pdu failed\r\n");
        return err;
    }
    /*  一条中文短信最长为70个中文字符  */
    if (len >= 70) {
        return -AW_EMSGSIZE;
    }
#if 0
    if (code == AW_GPRS_SMS_CODE_UTF8) {
        /* 把utf8编码的短信内容转换成ucs2*/
        ucs2_len = utf8_to_ucs2((char *)p_sms, len, ucs2_temp, sizeof(ucs2_temp));
        if(ucs2_len == 0){
            __GM510_TRACE("utf8 to ucs2 failed\r\n");
            return -AW_EBADMSG;
        }
        if (err != AW_OK) {
            return -AW_EBADMSG;
        }
        p_sms  = (uint8_t *)ucs2_temp;
        len    = ucs2_len;
        code   = AW_GPRS_SMS_CODE_UCS2;
    }
#endif
    err = awbl_gprs_pdu_pack(buf, sizeof(buf), NULL, p_sms, len, p_num, code);

    /* 这个长度是否应该为短信内容长度（除短信息中心号码 Service Center Address）*/
    sprintf(p_this->p_devinfo->buf, "AT+CMGS=%d\r", strlen(buf)/2 - 1);
    __at_cmd_send(p_this, p_this->p_devinfo->buf);
    if (__at_cmd_result_wait(p_this, "\r\n>", 5000) != AW_OK) {
        __GM510_TRACE("enter sms lengths failed\r\n");
        return -AW_ENOMSG;
    }
    /* 发送短信结束*/
    strcat(buf, GPRS_MSG_END);
    __at_cmd_send(p_this, buf);
    if (__at_cmd_result_wait(p_this, "OK", 20000) != AW_OK) {
        gm510_status_clr(p_this, AWBL_GPRS_GM510_SIM_OK);
        __GM510_TRACE("send message failed\r\n");
        return -AW_ENOMSG;
    }
    /* 设置短信成功状态*/
    gm510_status_set(p_this, AWBL_GPRS_GM510_SIM_OK);
    __GM510_TRACE("send message success\r\n");
    return AW_OK;
}

/** 以 text 格式发送英文短信 */
aw_local aw_err_t __en_text_sms_send (struct awbl_gprs_dev *p_gprs,
                                      uint8_t              *p_sms,
                                      uint8_t               len,
                                      char                 *p_num)
{
    __GM510_DECL_FROM_GPRS(p_this, p_gprs);
    aw_err_t    err;
    int         retry = 0;
    char        buf[370] = { 0 };

    /* 检查网络是否注册*/
    for(retry = 0;retry < 3;retry++){
        __at_cmd_send(p_this, "AT+CREG?\r");
        if (__at_cmd_result_wait(p_this, "+CREG: 0,1", 5000) == AW_OK) {
            break;
        }
    }
    if(retry == 3){
        __GM510_TRACE("net register failed\r\n");
        return -AW_ENOMSG;
    }
    /* 检查是否已登录网络*/
    for(retry = 0;retry < 3;retry++){
        __at_cmd_send(p_this, "AT+CEREG?\r");
        if (__at_cmd_result_wait(p_this, "+CEREG: 0,1", 5000) == AW_OK) {
            break;
        }
    }
    if(retry == 3){
        __GM510_TRACE("net login failed\r\n");
        return -AW_ENOMSG;
    }
    /* 初始化文本短信*/
    err = __gm510_dev_init_text_sms(p_this);
    if (err != AW_OK) {
        __GM510_TRACE("init text sms failed\r\n");
        return err;
    }
    /* 英文短信最大长度 */
    if (len >= 160) {
        return -AW_EMSGSIZE;
    }
    __GM510_TRACE("sending message...\r\n");
    aw_mdelay(1000);
    /* 输入电话号码*/
    sprintf(buf, "AT+CMGS=\"%s\"\r", p_num);
    __at_cmd_send(p_this, buf);
    if (__at_cmd_result_wait(p_this, "\r\n>", 5000) != AW_OK) {
        __GM510_TRACE("enter phone numbers failed\r\n");
        return -AW_ENOMSG;
    }
    /* 输入短信内容 */
    __at_cmd_send(p_this, (char *)p_sms);
    aw_mdelay(1000);
    /* 结束符 ctrl+z */
    buf[0] = 0x1a;
    buf[1] = 0;
    __at_cmd_send(p_this, buf);
    if (__at_cmd_result_wait(p_this, "OK", 20000) != AW_OK) {
        /* 在这一步失败，可能欠费 */
        gm510_status_clr(p_this, AWBL_GPRS_GM510_SIM_OK);
        __GM510_TRACE("send message failed\r\n");
        return -AW_ENOMSG;
    }
    /* 设置短信成功状态*/
    gm510_status_set(p_this, AWBL_GPRS_GM510_SIM_OK);
    __GM510_TRACE("send message success\r\n");
    return AW_OK;
}

/**
 * 发送短信(自动初始化)
 *
 * @param p_gprs 相关GPRS设备
 * @param p_sms  发送短信的内容
 * @param len    发送内容的长度
 * @param p_num  发送号码
 *
 * @return 成功返回AW_OK
 */
aw_local aw_err_t __gprs_sms_send (struct awbl_gprs_dev *p_gprs,
                                   uint8_t              *p_sms,
                                   uint8_t               len,
                                   char                 *p_num)
{
    return __pdu_sms_send(p_gprs, p_sms, len, p_num, AW_GPRS_SMS_CODE_UTF8);
}

//#define SMS_DEL

/**
 * 接收短信(自动初始化)
 *
 * @param p_gprs  相关GPRS设备
 * @param p_sms   接收短信的内容的指针
 * @param timeout 超时时间
 *
 * @return 成功返回AW_OK
 */
aw_local aw_err_t __gprs_sms_recv(struct awbl_gprs_dev      *p_gprs,
                                  struct aw_gprs_sms_buf    *p_sms,
                                  uint32_t                   timeout){
    __GM510_DECL_FROM_GPRS(p_this, p_gprs);
    aw_err_t ret;
    uint16_t index;
    char    *buf;
    char    *buf_tmp;
    char utf8[256] = {0};
    uint32_t rx_timeout, rx_timeout_temp;
    int i;

    /* 合法性检查*/
    if((p_this->p_devinfo->p_sms_info->p_sms_buff == NULL) ||
            (p_this->p_devinfo->p_sms_info->sms_buff_len == 0) ||
            (p_this->p_devinfo->p_sms_info->p_num_buff == NULL) ||
            (p_this->p_devinfo->p_sms_info->num_buff_len == 0) ||
            (p_sms == NULL) || (p_sms->p_sms_buff == NULL) ||
            (p_sms->p_num_buff == NULL)){
        __GM510_TRACE("invalid p_sms_info\r\n");
        return -AW_EINVAL;
    }

    /* 初始化PDU*/
    ret = __gm510_dev_init_pdu_sms(p_this);
    if (ret != AW_OK) {
        __GM510_TRACE("init pdu sms failed\r\n");
        return ret;
    }

    /* 先清0缓存*/
    memset(utf8, 0, sizeof(utf8));
    memset(p_this->p_devinfo->buf, 0, p_this->p_devinfo->buf_size);
    /* 先收了其他乱七八糟的数据*/
    rx_timeout_temp = 500;
    aw_usb_serial_timeout_get(p_this->p_pipe, &rx_timeout);
    aw_usb_serial_timeout_set(p_this->p_pipe, &rx_timeout_temp);
    for(i = 0;i < 3;i++){
        __gprs_at_cmd_recv(&p_this->gprs, p_this->p_devinfo->buf, p_this->p_devinfo->buf_size);
    }
    aw_usb_serial_timeout_set(p_this->p_pipe, &rx_timeout);
#ifdef SMS_DEL
    /* 根据索引读取短信*/
    __at_cmd_send(p_this, "AT+CMGD=1,4\r");
    if (__at_cmd_result_wait(p_this, "\r\nOK\r\n", 5000) == AW_OK) {
        return AW_OK;
    }
#endif

    /* 读取所有短信，然后解析第一条短信，并获取到这条短信的index，
     * 再用index删除这条短信*/
    __at_cmd_send(p_this, "AT+CMGL=4\r");
    if (__at_cmd_result_wait(p_this, "\r\n+CMGL:", 5000) == AW_OK) {
        /* 获取短信index */
        buf = __lstrip(p_this->p_devinfo->buf, "+CMGL:");
        if (buf == NULL) {
            __GM510_TRACE("get sms index failed\r\n");
            return -AW_ENOMSG;
        }
        index = atoi(buf);

        i = 5;
        while (i--) {
            ret = __at_cmd_result_wait(p_this, "\r\nOK\r\n", 1000);
            if (ret == AW_OK) {
                break;
            }
        }

        /* 获取第一条短信内容*/
        aw_snprintf(p_this->p_devinfo->buf, p_this->p_devinfo->buf_size,
                    "AT+CMGR=%d\r", index);
        __at_cmd_send(p_this, p_this->p_devinfo->buf);
        if (__at_cmd_result_wait(p_this, "\r\nOK\r\n", 5000) == AW_OK) {
            buf = __lstrip(p_this->p_devinfo->buf, "+CMGR:");
            if (buf == NULL) {
                __GM510_TRACE("get sms index failed\r\n");
                return -AW_ENOMSG;
            }

            buf = __lstrip(buf, "\r\n");
            if (buf == NULL) {
                __GM510_TRACE("get sms content failed\r\n");
                return -AW_ENOMSG;
            }
            /* 设置结束符 */
            buf_tmp = strstr(buf, "\r\n");
            aw_assert(buf_tmp);
            buf_tmp[0] = 0;
            goto __unpack;
        }
    }

    return -AW_EPERM;
__unpack:
    /* 解析短信*/
    ret = awbl_gprs_pdu_unpack(buf, p_this->p_devinfo->p_sms_info);
    if (ret == AW_OK) {
        if (p_this->p_devinfo->p_sms_info->sms_code == AW_GPRS_SMS_CODE_UCS2) {
            /* 把短信内容转换成utf-8*/
            ret = ucs2_to_utf8(p_this->p_devinfo->p_sms_info->p_sms_buff,
                    p_this->p_devinfo->p_sms_info->sms_buff_len, utf8, sizeof(utf8));
            if(ret == 0){
                __GM510_TRACE("ucs2 to utf8 failed\r\n");
                return -AW_EBADMSG;
            }
            utf8[ret] = 0;
            strcpy(p_this->p_devinfo->p_sms_info->p_sms_buff, utf8);
        }

        memcpy(p_sms, p_this->p_devinfo->p_sms_info, sizeof(*p_sms));
        __GM510_TRACE("recv sms %d : %s\r\n", index, p_sms->p_num_buff);

        /* 删除读出的短信 */
        sprintf(p_this->p_devinfo->buf, "AT+CMGD=%d\r", index);
        __at_cmd_send(p_this, p_this->p_devinfo->buf);
        if (__at_cmd_result_wait(p_this, "OK", 3000) != AW_OK) {
            __GM510_TRACE("delete sms failed\r\n");
            return -AW_ENOMSG;
        }

    } else if (-AW_ENOTSUP == ret) {
        /* 不支持的类型， 删除 */
        sprintf(p_this->p_devinfo->buf, "AT+CMGD=%d\r", index);
        __at_cmd_send(p_this, p_this->p_devinfo->buf);
        if (__at_cmd_result_wait(p_this, "OK", 3000) != AW_OK) {
            __GM510_TRACE("delete sms failed\r\n");
            return -AW_ENOMSG;
        }
        return -AW_ENOTSUP;
    } else {
        __GM510_TRACE("unpack sms content failed\r\n");
        return -AW_EBADMSG;
    }

    return AW_OK;
}

/**
 * GM510 卸载函数
 *
 * @param p_this GM510设备结构体
 *
 * @return 成功返回AW_OK
 */
aw_local aw_err_t __gprs_gm510_dev_uninit (struct awbl_gprs_gm510_dev *p_this)
{
    if (p_this == NULL) {
        return -AW_EINVAL;
    }

    /* 关闭PPP管道*/
    if (p_this->gprs.ll_type == AWBL_GPRS_LL_TYPE_PPP &&
            p_this->gprs.ppp.sio_mgn.fd) {
        __usbh_wireless_close(p_this->gprs.ppp.sio_mgn.fd);
        p_this->gprs.ppp.sio_mgn.fd = NULL;
    }

    /* 关闭AT管道*/
    if (p_this->p_pipe) {
        __usbh_wireless_close(p_this->p_pipe);
        p_this->p_pipe = NULL;
    }
    /* 清除初始化完成标志*/
    if(p_this->init_step > 0){
        p_this->init_step = 1;
    }
    return AW_OK;
}

/**
 * GM510 初始化函数一
 *
 * @param p_this GM510设备结构体
 *
 * @return 成功返回AW_OK
 */
aw_local aw_err_t __gm510_init_step1 (struct awbl_gprs_gm510_dev *p_this){
    __GM510_DEVINFO_DECL(p_devinfo, &p_this->dev);
    struct awbl_gprs_dev    *p_gprs = NULL;
    awbl_gprs_usbh_info     *p_usbh_info = p_this->p_devinfo->p_usbh_info;
    __LOG_DEV_DEF(p_gprs);

    /* 合法性检查*/
    if (p_this == NULL || p_devinfo == NULL) {
        return -AW_EINVAL;
    }

    if ((p_usbh_info == NULL) ||
            (p_usbh_info->at_com < 0) ||
            (p_usbh_info->ppp_com < 0)){
        return -AW_EINVAL;
    }

    p_this->p_devinfo = p_devinfo;
    p_gprs            = &p_this->gprs;

    /* 获取GPRS信息和SMS信息 */
    p_gprs->p_info      = p_devinfo->p_gprs_info;
    p_gprs->p_sms       = p_devinfo->p_sms_info;
    /* 获取GPRS模式*/
    p_gprs->ll_type = __usbh_wireless_ll_type_get(p_gprs);

#if AWBL_GPRS_LL_TYPE_ETH_ENABLE
    //todo
#endif
    /* PPP模式*/
    if (p_gprs->ll_type == AWBL_GPRS_LL_TYPE_PPP) {
        p_gprs->ppp.pfn_dial_up     = __gm510_dev_ppp_pdp_set;
        p_gprs->ppp.pfn_dial_down   = __gm510_dev_ppp_dial_down;

        /* 设置管道读写函数*/
        p_gprs->ppp.sio_mgn.pfn_sio_read  = __usbh_wireless_read;
        p_gprs->ppp.sio_mgn.pfn_sio_write = __usbh_wireless_write;
    }

    p_this->net_mode = 0;

    /* 初始化和释放信号量 */
    aw_assert(p_this->init_step < 1);
    AW_SEMB_INIT(p_this->lock, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);

    p_this->init_step = 1;
    return AW_OK;
}

/**
 * GM510 初始化函数二
 *
 * @param p_this GM510设备结构体
 *
 * @return 成功返回AW_OK
 */
aw_local aw_err_t __gm510_init_step2 (struct awbl_gprs_gm510_dev *p_this)
{
    aw_err_t ret;
    uint8_t socket[__INTERNAL_SOCKET_NUM] = __INTERNAL_SOCKET_VAL;

    /* 清楚GM510状态*/
    gm510_status_clr(p_this, 0);

    memcpy(p_this->socket, socket,
           __INTERNAL_SOCKET_NUM * sizeof(p_this->socket[0]));
    memset(p_this->socket_used,
           0, __INTERNAL_SOCKET_NUM * sizeof(p_this->socket_used[0]));

    ret = awbl_gprs_init_tail(&p_this->gprs);
    if(ret == AW_OK){
        p_this->init_step = 2;
    }
    return ret;
}

/**
 * GM510 初始化函数三
 *
 * @param p_this GM510设备结构体
 *
 * @return 成功返回AW_OK
 */
aw_local aw_err_t __gm510_init_step3 (struct awbl_gprs_gm510_dev *p_this)
{
    aw_local char   *default_apn = NULL;
    aw_err_t         ret;
    __LOG_DEV_DEF(&p_this->gprs);
    struct awbl_gprs_dev *p_gprs = NULL;
    awbl_gprs_usbh_info  *p_usbh_info = p_this->p_devinfo->p_usbh_info;

    p_gprs = &p_this->gprs;
    p_this->p_pipe = __USBH_WIRELESS_OPEN(__at_com_num_get(p_gprs));
    __LOG_RES(p_this->p_pipe == NULL, 1,
                "__usbh_wireless_open for AT");
    if (p_this->p_pipe == NULL) {
        ret = -AW_ENODEV;
        goto failed1;
    }
    /* PPP模式数据交互是用接口0，所以打开管道0*/
    p_gprs->ppp.sio_mgn.fd = __USBH_WIRELESS_OPEN(p_usbh_info->ppp_com);
    __LOG_RES(p_this->gprs.ppp.sio_mgn.fd == NULL, 1,
                "__usbh_wireless_open for ppp");
    if (p_this->gprs.ppp.sio_mgn.fd == NULL) {
        ret = -AW_ENOMEM;
        goto failed2;
    }

    if (default_apn == NULL) {
        default_apn = p_this->p_devinfo->p_gprs_info->p_apn;
        p_this->p_devinfo->p_gprs_info->p_apn = NULL;
    }

    __ITERATE(ret = __gm510_dev_check(p_this), ret == AW_OK, 5, 0);
    __ITERATE(ret = __gm510_dev_check(p_this), ret == AW_OK, 5, 0);
    __ITERATE(ret = __gm510_dev_check(p_this), ret == AW_OK, 5, 0);
    __LOG_RES(ret, 1, "detect AT cmd");
    if (ret != AW_OK) {
        __GM510_TRACE("detect AT cmd failed %d\r\n",ret);
        __gprs_gm510_dev_uninit(p_this);
        return -AW_EPERM;
    }

    /* at命令的回显设置 */
    __gm510_dev_at_disp_set(p_this, 0);
    /* 查看4G模块版本*/
    __at_cmd_send(p_this, "AT+ZVN\r");
    if (__at_cmd_result_wait(p_this, "OK", 10000) == AW_OK) {
        char *p_begin = NULL;

        p_begin = strstr(p_this->p_devinfo->buf, "\r\nOK\r\n");
        if(p_begin){
            *p_begin = '\0';
        }
        __GM510_TRACE("version:%s",p_this->p_devinfo->buf + 2);
    }
#if 0
    /* 设置最小功能*/
    __at_cmd_send(p_this, "AT+CFUN=0\r");
    if (__at_cmd_result_wait(p_this, "OK", 10000) != AW_OK) {
        __LOG_RES(ret, 1, "AT+CFUN=0 [ ERROR ]");
        __GM510_TRACE("AT+CFUN=0 [ ERROR ] %d\r\n",ret);
        return -AW_EPERM;
    }
    /* 禁用模组射频接收发射功能*/
    __at_cmd_send(p_this, "AT+CFUN=4\r");
    if (__at_cmd_result_wait(p_this, "OK", 10000) != AW_OK) {
        __LOG_RES(ret, 1, "AT+CFUN=4 [ ERROR ]");
        __GM510_TRACE("AT+CFUN=4 [ ERROR ] %d\r\n",ret);
        return -AW_EPERM;
    }
    /* 设置全功能*/
    __at_cmd_send(p_this, "AT+CFUN=1\r");
    if (__at_cmd_result_wait(p_this, "OK", 10000) != AW_OK) {
        __LOG_RES(ret, 1, "AT+CFUN=1 [ ERROR ]");
        __GM510_TRACE("AT+CFUN=1 [ ERROR ] %d\r\n",ret);
        return -AW_EPERM;
    }
#endif
    /* 查询国际移动台设备标识 IMSI */
    __ITERATE(ret = __cimi_check(p_this), ret == AW_OK, 5, 500);
    if (ret < 0) {
        /* 查询网络运营商*/
        __ITERATE(ret = __cops_check(p_this), ret == AW_OK, 5, 500);
        if (ret != AW_OK) {
            __LOG_RES(ret, 1, "detect APN");
            __GM510_TRACE("detect APN %d\r\n",ret);
            p_this->p_devinfo->p_gprs_info->p_apn = NULL;
        }
    }
    if (p_this->p_devinfo->p_gprs_info->p_apn != NULL) {
        __LOG_PFX(1, "detect APN:\"%s\" \n",
                      p_this->p_devinfo->p_gprs_info->p_apn);
        __GM510_TRACE("detect APN:\"%s\" \r\n",
                p_this->p_devinfo->p_gprs_info->p_apn);
    } else {
        p_this->p_devinfo->p_gprs_info->p_apn = default_apn;
        __LOG_PFX(1, "set default APN:\"%s\" [ WARN ] \n",
                       p_this->p_devinfo->p_gprs_info->p_apn);
        __GM510_TRACE("set default APN:\"%s\" [ WARN ] \r\n",
                p_this->p_devinfo->p_gprs_info->p_apn);
    }
//    /* PDP环境去除激活*/
//    __at_cmd_send(p_this, "AT+CGATT=0\r");
//    if (__at_cmd_result_wait(p_this, "OK", 3000) != AW_OK) {
//        ret = -AW_ENOMSG;
//        return ret;
//    }

    return AW_OK;
failed2:
    __usbh_wireless_close(p_this->p_pipe);
failed1:
    return ret;
}

/**
 * GM510 掉电函数
 *
 * @param p_gprs GPRS设备结构体
 *
 * @return 成功返回AW_OK
 */
aw_local aw_err_t __gprs_power_off (struct awbl_gprs_dev *p_gprs)
{
    aw_err_t ret;
    __GM510_DECL_FROM_GPRS(p_this, p_gprs);
    ret = __gprs_gm510_dev_uninit(p_this);

    return ret;
}

/** GM510 控制函数*/
aw_local aw_err_t __gprs_ioctl (struct awbl_gprs_dev *p_gprs,
                                aw_gprs_ioctl_req_t   req,
                                aw_gprs_ioctl_args_t *p_arg){
    __GM510_DECL_FROM_GPRS(p_this, p_gprs);
    aw_err_t ret = -AW_ENOTSUP;
    int      i   = 0;

    if(p_this->p_pipe == NULL){
        return -AW_EINVAL;
    }

    switch (req) {
    case AW_GPRS_REQ_RESET:
        /* 关闭PPP管道*/
        if (p_this->gprs.ppp.sio_mgn.fd) {
            __usbh_wireless_close(p_this->gprs.ppp.sio_mgn.fd);
            p_this->gprs.ppp.sio_mgn.fd = NULL;
        }
        /* 软复位*/
        ret = __gm510_dev_reset(p_gprs, (uint32_t)p_arg);
        break;
    /* 检查信号强度*/
    case AW_GPRS_REQ_CSQ_GET:
        while (i++ < 3) {
            aw_mdelay(2000);
            ret = __gm510_dev_csq_check(p_gprs, (int *)p_arg);
            if (ret == AW_OK) {
                break;
            }
            if (ret != -AW_EAGAIN) {
                break;
            }
        }
        break;
    /* 获取APN*/
    case AW_GPRS_REQ_APN_GET:{
        char *p_res_dat[8];
        __at_cmd_send(p_this, "AT+CIMI\r");
        p_res_dat[0] = "46000";                 /* 移动 GSM */
        p_res_dat[1] = "46002";                 /* 移动 TD-S */
        p_res_dat[2] = "46007";                 /* 移动 TD-S */

        p_res_dat[3] = "46001";                 /* 联通 GSM */
        p_res_dat[4] = "46006";                 /* 联通 WCDMA */

        p_res_dat[5] = "46003";                 /* 电信 CDMA */
        p_res_dat[6] = "46005";                 /* 电信 CDMA */
        p_res_dat[7] = "46011";                 /* 电信 FDD-LTE */

        ret = awbl_gprs_at_recv(&p_this->gprs, p_this->p_devinfo->buf,
                    p_this->p_devinfo->buf_size, p_res_dat, 8, 2000);
        if (ret == 0 || ret == 1 || ret == 2) {
            p_this->p_devinfo->p_gprs_info->p_apn = "CMNET";
        } else if (ret == 3 || ret == 4) {
            p_this->p_devinfo->p_gprs_info->p_apn = "3GNET";
        } else if (ret == 5 || ret == 6 || ret == 7) {
            p_this->p_devinfo->p_gprs_info->p_apn = "CTNET";
        } else {
            return -AW_ENODEV;
        }
        p_arg->p_apn = p_this->p_devinfo->p_gprs_info->p_apn;
        ret = AW_OK;
    }
    break;
    /* 查询设备状态*/
    case AW_GPRS_REQ_STS:{
        aw_gprs_sts_t sts = AW_GPRS_STS_OK;

        __at_cmd_send(p_this, "AT+CPIN?\r");
        if (__at_cmd_result_wait(p_this, "READY", 4000) != AW_OK) {
            sts = AW_GPRS_STS_PIN_ERROR;
        }
        __at_cmd_result_wait(p_this, "OK", 1000);

        __at_cmd_send(p_this, "AT+CGATT=1\r");
        if (__at_cmd_result_wait(p_this, "OK", 5000) != AW_OK) {
            sts = AW_GPRS_STS_CEREG_ERROR;
        }

        __at_cmd_send(p_this, "AT+CGREG?\r");
        if (__at_cmd_result_wait(p_this, "0,1", 4000) != AW_OK) {
            sts = AW_GPRS_STS_CEREG_ERROR;
        }
        __at_cmd_result_wait(p_this, "OK", 1000);

        __at_cmd_send(p_this, "AT+CREG?\r");
        if (__at_cmd_result_wait(p_this, "0,1", 4000) != AW_OK) {
            sts = AW_GPRS_STS_CEREG_ERROR;
        }
        __at_cmd_result_wait(p_this, "OK", 1000);

        __at_cmd_send(p_this, "AT+CEREG?\r");
        if (__at_cmd_result_wait(p_this, "0,1", 4000) != AW_OK) {
            sts = AW_GPRS_STS_CEREG_ERROR;
        }
        __at_cmd_result_wait(p_this, "OK", 1000);

        p_arg->sts = sts;
        ret = AW_OK;
    }
    break;
    case AW_GPRS_EXCEPTION:
        /* 清除这AT命令管道，不再进行数据收发 */
        if (p_this->p_pipe) {
            __usbh_wireless_close(p_this->p_pipe);
            p_this->p_pipe = NULL;
        }
        if (p_this->gprs.ll_type == AWBL_GPRS_LL_TYPE_PPP &&
                p_this->gprs.ppp.sio_mgn.fd) {
            __usbh_wireless_close(p_this->gprs.ppp.sio_mgn.fd);
            p_this->gprs.ppp.sio_mgn.fd = NULL;
        }
        ret = AW_OK;
        break;
//    case AW_GPRS_REQ_PING:
//        ret = __ec20_dev_gprs_ping(p_this, p_arg->p_ping_dst);
//        __LOG_RES(ret, 1, "Ping \"%s\"", p_arg->p_ping_dst);
//        break;
    /* 查询网络服务类型*/
    case AW_GPRS_REQ_NET_MODE_GET:
        ret = __gm510_dev_net_mode_get(p_this, p_arg->net_mode);
        break;
    /* 查询基站*/
    case AW_GPRS_REQ_BS_GET:
        ret = __gm510_dev_base_station_query(p_this, p_arg->bs_info);
        break;
    /* 读取SIM卡的ICCID*/
    case AW_GPRS_REQ_CCID_GET:
        if(p_arg == NULL){
            __GM510_TRACE("invalid p_arg\r\n");
            return -AW_EINVAL;
        }
        memset(p_arg->ccid, 0, sizeof(p_arg->ccid));
        ret = __gm510_get_ICCID(p_this, p_arg->ccid);
        break;
    case AW_GPRS_REQ_IP_GET:
    /* 查看4G模块版本*/
    case AW_GPRS_REQ_VERSION_GET:
        if(p_arg == NULL){
            __GM510_TRACE("invalid p_arg\r\n");
            return -AW_EINVAL;
        }
        memset(p_arg->version, 0, sizeof(p_arg->version));
        ret = __gm510_get_version(p_this, p_arg->version);
        break;
    /* 获取IMEI*/
    case AW_GPRS_REQ_IMEI_GET:
        if(p_arg == NULL){
            __GM510_TRACE("invalid p_arg\r\n");
            return -AW_EINVAL;
        }
        memset(p_arg->imei, 0, sizeof(p_arg->imei));
        ret = __gm510_get_IMEI(p_this, p_arg->imei);
        break;
    /* 获取IMSI*/
    case AW_GPRS_REQ_IMSI_GET:
        if(p_arg == NULL){
            __GM510_TRACE("invalid p_arg\r\n");
            return -AW_EINVAL;
        }
        memset(p_arg->imsi, 0, sizeof(p_arg->imsi));
        ret = __gm510_get_IMSI(p_this, p_arg->imsi);
        break;
    /* 查询短信中心号码*/
    case AW_GPRS_REQ_SCA_GET:
        if(p_arg == NULL){
            __GM510_TRACE("invalid p_arg\r\n");
            return -AW_EINVAL;
        }
        memset(p_arg->sca, 0, sizeof(p_arg->sca));
        ret = __gm510_get_SCA(p_this, p_arg->sca);
        break;
    /* AT命令传输*/
    case AW_GPRS_REQ_AT_TRANS:
        ret = __gm510_at_trans(p_this, p_arg);
        break;
    default:
        return -AW_ENOTSUP;
    }
    return ret;
}

/**
 * GM510 上电函数
 *
 * @param p_gprs GPRS设备结构体
 *
 * @return 成功返回AW_OK
 */
aw_local aw_err_t __gprs_power_on (struct awbl_gprs_dev *p_gprs)
{
    __GM510_DECL_FROM_GPRS(p_this, p_gprs);
    aw_err_t ret;
    __LOG_DEV_DEF(p_gprs);

    /* 初始化第一步*/
    if (p_this->init_step == 0) {
        ret = __gm510_init_step1(p_this);
        __LOG_RES(ret, 1, "__gm510_init_step1");
        if (ret != AW_OK) {
            __GM510_TRACE("__gm510_init_step1 failed %d\r\n",ret);
            return ret;
        }
    }
    /* 初始化第二步*/
    if (p_this->init_step == 1) {
        ret = __gm510_init_step2(p_this);
        __LOG_RES(ret, 1, "__gm510_init_step2");
        if (ret != AW_OK) {
            __GM510_TRACE("__gm510_init_step2 failed %d\r\n",ret);
            return ret;
        }
    }
    /* 初始化第三步*/
    if (p_this->init_step == 2) {
        ret = __gm510_init_step3(p_this);
        __LOG_RES(ret, 1, "__gm510_init_step3");
        if (ret != AW_OK) {
            __GM510_TRACE("__gm510_init_step3 failed %d\r\n",ret);
            return ret;
        }
    }

    return AW_OK;
}

/**
 * 建立GPRS连接
 *
 * @param  p_gprs_dev     gprs设备结构体变量
 * @param  mode           0：tcp   1：udp
 * @param  p_ip           目的地ip
 * @param  port           目标端口
 *
 * @return 成功返回AW_OK
 */
aw_local int __gm510_dev_ip_start (struct awbl_gprs_dev *p_gprs,
                                  uint8_t               mode,
                                  char                 *p_ip,
                                  uint16_t              port,
                                  uint32_t              timeout)
{
    __GM510_DECL_FROM_GPRS(p_this, p_gprs);
    char       *buf     = p_this->p_devinfo->buf;
    int         ret     = -1;
    int         hd     = -1;

    if(p_this->p_pipe == NULL){
        return -AW_EINVAL;
    }
    /* 检查socket数量*/
    if (p_gprs->socket_num >= 255) {
        __GM510_TRACE("socket too many( num >= 255)\r\n");
        return -AW_ENODEV;
    }
    /* 获取socket*/
    hd = __socket_get(p_this);
    if (hd < 0) {
        return hd;
    }
    /* 配置IP所需要的APN，用户名和密码*/
    sprintf(buf, "AT+ZIPCFG=\"%s\"\r", p_this->gprs.p_info->p_apn);
    __at_cmd_send(p_this, buf);
    if (__at_cmd_result_wait(p_this, "OK", 5000) != AW_OK) {
        /* 查看是否配置成功*/
        __at_cmd_send(p_this, "AT+ZIPCFG?\r");
        if (__at_cmd_result_wait(p_this, "OK", 5000) != AW_OK) {
            ret = -AW_ENOMSG;
            goto failed;
        }
    }
    /* 关闭TCP/UDP连接*/
    sprintf(buf, "AT+ZIPCLOSE=%d\r", hd);
    __at_cmd_send(p_this, buf);
    if (__at_cmd_result_wait(p_this, "OK", 5000) != AW_OK) {
        ret = -AW_ENOMSG;
        goto failed;
    }
    /* 建立TCP/UDP连接*/
    sprintf(buf, "AT+ZIPOPEN=%d,\"%s\",\"%s\",%d\r",
            hd, (mode == 0 ? "TCP":"UDP"), p_ip, port);
    __at_cmd_send(p_this, buf);
    if (__at_cmd_result_wait(p_this, "OK", 5000) != AW_OK) {
        ret = -AW_ENOMSG;
        goto failed;
    }
    /* socket数量增加*/
    p_gprs->socket_num++;
    return hd;

failed:
    __socket_put(p_this, hd);
    return ret;
}

/**
 * 关闭GPRS连接
 *
 * @param p_gprs GPRS设备
 * @param hd     套接字标识号
 *
 * @return 成功返回AW_OK
 */
aw_local aw_err_t __gm510_dev_ip_close (struct awbl_gprs_dev *p_gprs, int hd)
{
    __GM510_DECL_FROM_GPRS(p_this, p_gprs);
    char *buf     = p_this->p_devinfo->buf;

    if(p_this->p_pipe == NULL){
        return -AW_EINVAL;
    }
    /* 检查socket号*/
    if (hd < 1 || hd > 5) {
        return -AW_ENODEV;
    }
    /* 关闭TCP/UDP连接*/
    sprintf(buf, "AT+ZIPCLOSE=%d\r", hd);
    __at_cmd_send(p_this, buf);
    if (__at_cmd_result_wait(p_this, "OK", 5000) != AW_OK) {
        return -AW_ENOMSG;
    }
    /* socket数量减1*/
    if (p_gprs->socket_num > 0) {
        p_gprs->socket_num--;
    }
    __socket_put(p_this, hd);

    return AW_OK;
}

/**
 * 使用at命令发送gprs数据，注意发送的数据里不能有回车符'\r'，
 * 如果有责强制转换成空格符
 *
 * @param  p_gprs_dev     gprs设备结构体变量
 * @param  p_dat          要发送的数据
 *
 * @return 成功返回AW_OK
 */
aw_local ssize_t __gm510_dev_ip_send (struct awbl_gprs_dev *p_gprs,
                                     int                   hd,
                                     void                 *p_dat,
                                     size_t                len,
                                     uint32_t              timeout)
{
    __GM510_DECL_FROM_GPRS(p_this, p_gprs);
    char        *buf = p_this->p_devinfo->buf;

    if(p_this->p_pipe == NULL){
        return -AW_EINVAL;
    }
    /* 发送数据*/
    sprintf(buf, "AT+ZIPSEND=%d,\"%s\"\r", hd, (char *)p_dat);
    __at_cmd_send(p_this, buf);
    if (__at_cmd_result_wait(p_this, "OK", 10000) != AW_OK) {
        return -AW_ENOMSG;
    }

    return len;
}

/** GPRS服务函数集*/
aw_local aw_const struct awbl_gprs_service __g_gprs_servfunc = {
    .pfn_gprs_power_on      = __gprs_power_on,
    .pfn_gprs_power_off     = __gprs_power_off,
    .pfn_gprs_ioctl         = __gprs_ioctl,
    .pfn_gprs_sms_send      = __gprs_sms_send,
    .pfn_gprs_sms_recv      = __gprs_sms_recv,
//    .pfn_gprs_sms_hex_send  = __gprs_sms_hex_send,
    .pfn_gprs_start         = __gm510_dev_ip_start,
    .pfn_gprs_close         = __gm510_dev_ip_close,
    .pfn_gprs_send          = __gm510_dev_ip_send,
    .pfn_probe              = __gm510_dev_probe,
    .pfn_at_cmd_send        = __gprs_at_cmd_send,
    .pfn_at_cmd_recv        = __gprs_at_cmd_recv,
};


/** 添加到GPRS服务*/
aw_local aw_err_t __gm510_dev_get (struct awbl_dev *p_dev, void *p_arg)
{
    __GM510_DEV_DECL(p_this, p_dev);
    __GM510_DEVINFO_DECL(p_devinfo, p_dev);

    struct awbl_gprs_dev *p_gprs = NULL;
    /* 合法性检查*/
    if (!p_dev) {
        return -AW_EINVAL;
    }
    /* 获取GPRS结构体*/
    p_gprs = &p_this->gprs;
    /* 初始化互斥锁*/
    AW_MUTEX_INIT(p_gprs->mutex, AW_SEM_Q_PRIORITY);
    /* 初始化GPRS结构体成员*/
    p_gprs->p_serv      = &__g_gprs_servfunc;
    p_gprs->p_gprs_dev  = (void *)p_this;
    p_gprs->p_info      = p_devinfo->p_gprs_info;
    p_gprs->dev_id      = p_devinfo->id;
    p_this->p_devinfo   = p_devinfo;
    /* 把初始化完的GPRS结构体返回给GPRS服务*/
    *(struct awbl_gprs_dev **)p_arg = p_gprs;

    return AW_OK;
}

/** 第二阶段初始化*/
aw_local void __drv_init2 (struct awbl_dev *p_dev)
{
    __GM510_DEVINFO_DECL(p_devinfo, p_dev);
    if (p_devinfo->pfn_init) {
        p_devinfo->pfn_init();
    }
}

/** 第三阶段初始化*/
aw_local void __drv_connect(struct awbl_dev *p_dev){
    __GM510_DEV_DECL(p_this, p_dev);
    struct awbl_gprs_dev *p_gprs = &p_this->gprs;

    if(p_gprs->p_info){
        /* 用户上电函数*/
        if(p_gprs->p_info->pfn_power_on){
            p_gprs->p_info->pfn_power_on();
        }
    }
}

/** 驱动初始化函数*/
aw_local aw_const struct awbl_drvfuncs __g_drvfuncs = {
    NULL,
    __drv_init2,
    __drv_connect,
};

/** GPRS服务METHOD*/
AWBL_METHOD_IMPORT(awbl_gprs_dev_get);

aw_local aw_const struct awbl_dev_method __g_gprs_gm510_methods[] = {
    AWBL_METHOD(awbl_gprs_dev_get, __gm510_dev_get),
    AWBL_METHOD_END
};

/** 驱动信息*/
aw_local aw_const awbl_plb_drvinfo_t __g_gm510_drv_registration = {
    {
        AWBL_VER_1,                         /* awb_ver */
        AWBL_BUSID_PLB,                     /* bus_id */
        AWBL_GPRS_GM510_NAME,               /* p_drvname */
        &__g_drvfuncs,                      /* p_busfuncs */
        &__g_gprs_gm510_methods[0],         /* p_methods */
        NULL                                /* pfunc_drv_probe */
    }
};


/** GM510 4G驱动注册函数*/
void awbl_gprs_gm510_drv_register (void)
{
    int ret;
    ret = awbl_drv_register((struct awbl_drvinfo *)&__g_gm510_drv_registration);
    if (ret != AW_OK) {
        return;
    }
}
