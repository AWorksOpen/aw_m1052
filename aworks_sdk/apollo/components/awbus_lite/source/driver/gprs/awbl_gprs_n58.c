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
#include "awbl_gprs.h"
#include <stdio.h>
#include <string.h>

#include "host/class/cdc/serial/awbl_usbh_cdc_serial.h"
#include "driver/gprs/awbl_gprs_n58.h"

typedef void   (*aw_gprs_log_t) (void         *p_color,
                                 const char   *func_name,
                                 int           line,
                                 const char   *fmt, ...);

aw_bool_t is_gprs_log_set = AW_FALSE;

/* N58��ӡ����*/
#define __N58_TRACE(info, ...) \
            do { \
                if(is_gprs_log_set == AW_FALSE){           \
                    AW_INFOF(("N58:"info, ##__VA_ARGS__)); \
                }                                          \
            } while (0)
/* ��ȡN58�豸�ṹ��*/
#define __N58_DEV_DECL(p_this, p_dev) \
        struct awbl_gprs_n58_dev *p_this = AW_CONTAINER_OF(p_dev, \
                                                           struct awbl_gprs_n58_dev, \
                                                           dev)
/* ��ȡN58�豸��Ϣ*/
#define __N58_DEVINFO_DECL(p_devinfo, p_dev) \
        struct awbl_gprs_n58_devinfo *p_devinfo = \
                     ((struct awbl_gprs_n58_devinfo *)AWBL_DEVINFO_GET(p_dev))
/* ��GPRS�豸�л�ȡN58�豸�ṹ��*/
#define __N58_DECL_FROM_GPRS(p_this, p_gprs) \
        struct awbl_gprs_n58_dev *p_this = \
                     (struct awbl_gprs_n58_dev *)p_gprs->p_gprs_dev
#define __LOG_FCOLOR_RED        "\x1B[31m"
#define __LOG_FCOLOR_GREEN      "\x1B[32m"

/* ��ȡ��ӡ����*/
#define __LOG_DEV_DEF(p_gprs)  \
    aw_gprs_log_t _pfn_log_out_ = \
            (p_gprs ? ((struct awbl_gprs_dev *)p_gprs)->pfn_log_out : NULL )
/* ����ɫ��ӡ*/
#define __LOG_BASE(mask, color, fmt, ...)   \
    if ((mask) && _pfn_log_out_) {          \
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

/** \brief �� usbh �����豸�ܵ�   */
#define __USBH_WIRELESS_OPEN(com) \
    __usbh_wireless_open(p_this->p_devinfo->p_usbh_info->pid,\
                         p_this->p_devinfo->p_usbh_info->vid,\
                         com, AW_TRUE, 512, 15000);

aw_local aw_const struct awbl_gprs_service __g_gprs_servfunc;

/** �� p_str �е� p_substr ������ߵ��ַ�����ȥ�� */
aw_local aw_inline char *__lstrip (char *p_str, char *p_substr)
{
    char *p_begin;

    p_begin = strstr(p_str, p_substr);
    if (p_begin != NULL) {
        return p_begin + strlen(p_substr);
    }
    return  NULL;
}

///** ��ȡsocket*/
//static int __socket_get (struct awbl_gprs_n58_dev *p_this)
//{
//    uint8_t *p_socket       = p_this->socket;
//    uint8_t *p_socket_used  = p_this->socket_used;
//    uint8_t  i = 0;
//
//    for (i = 0; i < __INTERNAL_SOCKET_NUM; i++) {
//        if (p_socket_used[i] == 0) {
//            if (p_socket_used[i] < 255) {
//                p_socket_used[i]++;
//            }
//            return p_socket[i];
//        }
//    }
//    return -1;
//}
//
///** �ͷ�socket*/
//static void __socket_put (struct awbl_gprs_n58_dev *p_this,
//                          uint8_t                    socket)
//{
//    uint8_t *p_socket       = p_this->socket;
//    uint8_t *p_socket_used  = p_this->socket_used;
//    uint8_t  i = 0;
//
//    for (i = 0; i < __INTERNAL_SOCKET_NUM; i++) {
//        if (p_socket[i] == socket) {
//            if (p_socket_used[i] > 0) {
//                p_socket_used[i]--;
//                return ;
//            }
//        }
//    }
//}

/** ��ȡGPRSģʽ*/
aw_local awbl_gprs_ll_type_t
__usbh_wireless_ll_type_get (struct awbl_gprs_dev *p_gprs)
{
    return AWBL_GPRS_LL_TYPE_PPP;
}

/** ���� ll_type �Զ���ȡ at ����ʹ�õĹܵ�*/
aw_local uint8_t __at_com_num_get (struct awbl_gprs_dev *p_gprs)
{
    __N58_DECL_FROM_GPRS(p_this, p_gprs);
    if (p_gprs->ll_type == AWBL_GPRS_LL_TYPE_ETH) {
        /* ��̫��ģʽAT�����ʹ�õ� AT�˿�*/
        return p_this->p_devinfo->p_usbh_info->at_com;
    } else {
        /* PPPģʽAT�����ʹ�õ� AT�˿�*/
        return p_this->p_devinfo->p_usbh_info->at_com;
    }
}

/** �˿ڴ򿪺���*/
aw_local void *__usbh_wireless_open (int         pid,
                                     int         vid,
                                     int         inum,
                                     aw_bool_t   rngbuf,
                                     int         size,
                                     int         timeout)
{
    void    *p;
    (void)rngbuf;
    (void)size;
    (void)timeout;
    p = awbl_usbh_serial_open(pid, vid, inum);

    return p;
}

/** �˿�д����*/
aw_local u32_t __usbh_wireless_write (void *fd, u8_t *data, u32_t len)
{
    return aw_usb_serial_write_sync(fd, data, len);
}

/** �˿ڶ�����*/
aw_local u32_t __usbh_wireless_read (void *fd, u8_t *data, u32_t len)
{
    return aw_usb_serial_read_sync(fd, data, len);
}

/** �˿ڹرպ���*/
aw_local void __usbh_wireless_close (void *fd)
{
    awbl_usbh_serial_close(fd);
}

/** AT����ͺ���*/
aw_local ssize_t __gprs_at_cmd_send (struct awbl_gprs_dev *p_gprs, char *p_buf)
{
    __N58_DECL_FROM_GPRS(p_this, p_gprs);
    if (p_this->p_pipe == NULL) {
        return 0;
    }
    return aw_usb_serial_write_sync(p_this->p_pipe, p_buf, strlen(p_buf));
}

/** AT������պ���*/
aw_local ssize_t __gprs_at_cmd_recv (struct awbl_gprs_dev   *p_gprs,
                                     char                   *p_buf,
                                     size_t                  buf_size)
{
    __N58_DECL_FROM_GPRS(p_this, p_gprs);
    if (p_this->p_pipe == NULL) {
        return 0;
    }
    return aw_usb_serial_read_sync(p_this->p_pipe, p_buf, buf_size);
}

/** �ȴ����ջظ�*/
aw_local int __at_cmd_result_wait (struct awbl_gprs_n58_dev *p_this,
                                   char *p_str,
                                   uint32_t ms_timeout)
{
    char *arg[1];

    arg[0] = p_str;
    /* �Ϸ��Լ��*/
    if (p_this->p_pipe == NULL) {
        __N58_TRACE("AT pipe is NULL\r\n");
        return -1;
    }

    return awbl_gprs_at_recv(&p_this->gprs,
                             p_this->p_devinfo->buf,
                             p_this->p_devinfo->buf_size,
                             arg,
                             1,
                             ms_timeout);
}

///** ��ȡN58 ״̬*/
//aw_local int n58_status_is(struct awbl_gprs_n58_dev *p_this, int status)
//{
//    if (p_this == NULL) {
//        return -AW_EFAULT;
//    }
//    return ((p_this->status & status) == status);
//}

/** ���N58 �豸״̬*/
aw_local void n58_status_clr(struct awbl_gprs_n58_dev *p_this, int status)
{

    if (p_this == NULL) {
        __N58_TRACE("dev is NULL!\r\n");
        return ;
    }
    if (status == 0) {
        p_this->status = 0;
    } else {
        p_this->status &= ~status;
    }
}

/** ����N58 �豸״̬*/
aw_local void n58_status_set(struct awbl_gprs_n58_dev *p_this, int status)
{
    if (p_this == NULL) {
        return;
    }

    p_this->status |= status;
}

/* ���N58 �豸�Ƿ���Ӧ��*/
aw_local aw_err_t __n58_dev_check (struct awbl_gprs_n58_dev *p_this)
{
    aw_err_t ret;

    if(p_this->p_pipe == NULL){
        return -AW_EINVAL;
    }

    __at_cmd_send(p_this, "AT\r");
    ret = __at_cmd_result_wait(p_this, "OK", 1000);
    return ret;
}

/** ��ȡN58����汾*/
aw_local aw_err_t __n58_get_version(struct awbl_gprs_n58_dev *p_this, char* p_buf){
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

    __at_cmd_send(p_this, "AT+GMR\r");
    /* �ȴ���Ӧ���С�N58��*/
    if (__at_cmd_result_wait(p_this, "\r\nOK\r\n", 10000) == AW_OK) {
        char *p_begin = NULL;
        char *p_end   = NULL;
        /* �ƶ����ؼ��ֵط�*/
        p_begin = strstr(p_this->p_devinfo->buf, "N58");
        if(p_begin){
            /* ��������û�С�OK�����еĻ�ȥ��*/
            p_end = strstr(p_begin, "\r\nOK\r\n");
            if(p_end != NULL){
                *p_end = '\0';
            }
            memcpy(p_buf, p_begin, strlen(p_begin));

            return AW_OK;
        }
    }
    return -AW_ECANCELED;
}

/** ��ȡN58 IMSI*/
aw_local aw_err_t __n58_get_IMSI(struct awbl_gprs_n58_dev *p_this, char* p_buf){
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
    if (__at_cmd_result_wait(p_this, "\r\nOK\r\n", 10000) == AW_OK) {
        char *p_begin = NULL;
        char *p_end   = NULL;

        p_begin = strstr(p_this->p_devinfo->buf, "+CIMI:");
        if(p_begin == NULL){
            return -AW_ECANCELED;
        }
        p_end = strstr(p_begin, "\r\nOK\r\n");
        if(p_end != NULL){
            *p_end = '\0';
        }
        memcpy(p_buf, p_begin + 6, strlen(p_begin) - 6);

        return AW_OK;
    }
    return -AW_ECANCELED;
}

/** ��ȡN58 IMEI*/
aw_local aw_err_t __n58_get_IMEI(struct awbl_gprs_n58_dev *p_this, char* p_buf){
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
    __at_cmd_send(p_this, "AT+CGSN\r");
    if (__at_cmd_result_wait(p_this, "\r\nOK\r\n", 10000) == AW_OK) {
        char *p_begin = NULL;
        char *p_end   = NULL;

        p_begin = strstr(p_this->p_devinfo->buf, "+CGSN:");
        if(p_begin == NULL){
            return -AW_ECANCELED;
        }

        p_end = strstr(p_begin, "\r\nOK\r\n");
        if(p_end != NULL){
            *p_end = '\0';
        }

        memcpy(p_buf, p_begin + 6, strlen(p_begin) - 6);

        return AW_OK;
    }
    return -AW_ECANCELED;
}

/** ��ȡN58 �������ĺ���*/
aw_local aw_err_t __n58_get_SCA(struct awbl_gprs_n58_dev *p_this, char* p_buf){
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
    if (__at_cmd_result_wait(p_this, "\r\nOK\r\n", 10000) == AW_OK) {
        char *p_begin = NULL;
        char *p_end   = NULL;

        p_begin = strstr(p_this->p_devinfo->buf, "+CSCA:");
        if(p_begin){
            /* ��������û�С�OK�����еĻ�ȥ��*/
            p_end = strstr(p_begin, "\r\nOK\r\n");
            if(p_end != NULL){
                *p_end = '\0';
            }
            memcpy(p_buf, p_begin + 6, strlen(p_begin) - 6);
            return AW_OK;
        }
    }
    return -AW_ECANCELED;
}

/** ��ȡN58 SIM��ICCID*/
aw_local aw_err_t __n58_get_ICCID(struct awbl_gprs_n58_dev *p_this, char* p_buf){
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

    __at_cmd_send(p_this, "AT+CCID\r");
    if (__at_cmd_result_wait(p_this, "\r\nOK\r\n", 10000) == AW_OK) {
        char *p_begin = NULL;
        char *p_end   = NULL;

        p_begin = strstr(p_this->p_devinfo->buf, "+CCID:");
        if(p_begin){
            /* ��������û�С�OK�����еĻ�ȥ��*/
            p_end = strstr(p_begin, "\r\nOK\r\n");
            if(p_end != NULL){
                *p_end = '\0';
            }
            memcpy(p_buf, p_begin + 6, strlen(p_begin) - 6);
            return AW_OK;
        }
    }
    return -AW_ECANCELED;
}

/** ����ź�ǿ�� */
aw_local aw_err_t __n58_dev_csq_check (struct awbl_gprs_dev *p_gprs, int *p_csq)
{
    __N58_DECL_FROM_GPRS(p_this, p_gprs);

    char *buf     = p_this->p_devinfo->buf;

    __at_cmd_send(p_this, "AT+CSQ\r");
    if (__at_cmd_result_wait(p_this, "+CSQ:", 5000) != AW_OK) {
        return -AW_ENOMSG;
    }
    /* ��ȡ�ź�ֵ */
    *p_csq = (uint8_t)strtol(strstr(buf, "+CSQ:") + 5, 0, 0);
    /* ������� +CSQ: 99 ���ʾû���ź� */
    if (*p_csq == 99 || *p_csq < 9) {
        return -AW_EAGAIN;
    }
    else{
        __N58_TRACE("CSQ:%d\r\n", *p_csq);
    }
    return AW_OK;
}

/** N58 AT�����*/
aw_local aw_err_t __n58_at_trans(struct awbl_gprs_n58_dev *p_this, aw_gprs_ioctl_args_t *p_arg){
    int i;
    uint32_t timeout, timeout_temp;

    if((p_arg == NULL) || (p_arg->at_trans.p_at_cmd == NULL)){
        __N58_TRACE("invalid p_arg\r\n");
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

/**
 * ���͸�λ��AT����
 *
 * @param  this     �豸�ṹ�����
 * @param  opt      opt=0:�����λ ; opt=1��Ӳ����λ
 *
 * @return �ɹ�����AW_OK
 */
aw_local aw_err_t __n58_dev_reset (struct awbl_gprs_dev *p_gprs,
                                     uint32_t              opt)
{
    __N58_DECL_FROM_GPRS(p_this, p_gprs);
    int ret;

    if (opt == 0) {
        if (p_this == NULL) {
            return -AW_EFAULT;
        }
        __N58_TRACE("soft reset\r\n");
        __at_cmd_send(p_this, "AT+CFUN=1,1\r");
        __usbh_wireless_close(p_this->p_pipe);
        aw_mdelay(30000);
    } else {
        if (p_this->p_devinfo->p_gprs_info->pfn_reset) {
            p_this->p_devinfo->p_gprs_info->pfn_reset();
        }
    }
    /* ���״̬*/
    n58_status_clr(p_this, 0);

    /* ���´�AT�ܵ�*/
    p_this->p_pipe = __USBH_WIRELESS_OPEN(__at_com_num_get(p_gprs));
    if (p_this->p_pipe == NULL) {
        ret = -AW_ENODEV;
        goto failed1;
    }
    /* ���´�PPP�ܵ�*/
    p_this->gprs.ppp.sio_mgn.fd =
            __USBH_WIRELESS_OPEN(p_this->p_devinfo->p_usbh_info->ppp_com);
    if (p_this->gprs.ppp.sio_mgn.fd == NULL) {
        ret = -AW_ENOMEM;
        goto failed2;
    }
    /* ����Ƿ���Ӧ*/
    __ITERATE(ret = __n58_dev_check(p_this), ret == AW_OK, 5, 0);
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
 * AT����Ļ�������
 *
 * @param  this    �豸�ṹ�����
 * @param  ate     0���ػ��ԣ�  1��������
 * @param  atv     0�����������ͣ�  1�������ַ���
 *
 * @return �ɹ�����AW_OK
 */
aw_local aw_err_t __n58_dev_at_disp_set (struct awbl_gprs_n58_dev *p_this,
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

/** ��ѯ�����ƶ�̨�豸��ʶ IMSI */
aw_local aw_err_t __cimi_check (struct awbl_gprs_n58_dev *p_this)
{
    char *p_res_dat[16];
    int   ret;

    /* �Ϸ��Լ��*/
    if(p_this->p_pipe == NULL){
        return -AW_EINVAL;
    }

    __at_cmd_send(p_this, "AT+CIMI\r");
    p_res_dat[0] = "46000";                 /* �ƶ� GSM */
    p_res_dat[1] = "46002";                 /* �ƶ� TD-S */
    p_res_dat[2] = "46007";                 /* �ƶ� TD-S */
    p_res_dat[8] = "46004";                 /* �ƶ� TD-S */

    p_res_dat[3] = "46001";                 /* ��ͨ GSM */
    p_res_dat[4] = "46006";                 /* ��ͨ WCDMA */

    p_res_dat[5] = "46003";                 /* ���� CDMA */
    p_res_dat[6] = "46005";                 /* ���� CDMA */
    p_res_dat[7] = "46011";                 /* ���� FDD-LTE */

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

/** ��ѯ������Ӫ�̣�����0���ƶ��� ����1����ͨ */
aw_local aw_err_t __cops_check (struct awbl_gprs_n58_dev *p_this)
{
    char *p_res_dat[4];
    int   ret;

    if(p_this->p_pipe == NULL){
        return -AW_EINVAL;
    }

    __at_cmd_send(p_this, "AT+COPS=3,0\r");
    ret = __at_cmd_result_wait(p_this, "OK", 2000);

    __at_cmd_send(p_this, "AT+COPS?\r");
    p_res_dat[0] = "MOBILE";                /* �ƶ� */
    p_res_dat[1] = "UNICOM";                /* ��ͨ */
    p_res_dat[2] = "CHINA TELECOM";         /* ���� */
    p_res_dat[3] = "CHN-CT";                /* ���� */

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

/** ����PDP */
aw_local aw_err_t __n58_dev_ppp_pdp_set (struct awbl_gprs_dev *p_gprs)
{
    __N58_DECL_FROM_GPRS(p_this, p_gprs);
    aw_err_t   ret;
    void *p_pipe_bp = p_this->p_pipe;
    char buf[256] = {0};

    /* ���PDP״̬*/
    n58_status_clr(p_this, AWBL_GPRS_N58_GPRS_PDP_OK);
    p_this->p_pipe = p_this->gprs.ppp.sio_mgn.fd;

    __N58_TRACE("set PDP...\r\n");

    aw_mdelay(1000);
    __at_cmd_send(p_this, "+++\r");
    aw_mdelay(1000);
    __at_cmd_send(p_this, "ATH\r");

    /* ���APN*/
    if (p_this->p_devinfo->p_gprs_info->p_apn == NULL) {
        __ITERATE(ret = __cimi_check(p_this), ret == AW_OK, 3, 0);
        if (ret < 0) {
            __ITERATE(ret = __cops_check(p_this), ret == AW_OK, 1, 0);
            if (ret != AW_OK) {
                __N58_TRACE("detect APN failed\r\n");
                goto _err_exit;
            }
        }
    }
    if (p_this->p_devinfo->p_gprs_info->p_apn) {
        /* ����PDP������*/
        sprintf(buf, "AT+CGDCONT=1,\"IP\",\"%s\"\r", p_this->p_devinfo->p_gprs_info->p_apn);
        __at_cmd_send(p_this, buf);
        if (__at_cmd_result_wait(p_this, "OK", 2000) != AW_OK) {
            /* ʧ�����ٷ�һ��*/
            aw_mdelay(1000);
            sprintf(buf, "AT+CGDCONT=1,\"IP\",\"%s\"\r", p_this->p_devinfo->p_gprs_info->p_apn);
            __at_cmd_send(p_this, buf);
            if (__at_cmd_result_wait(p_this, "OK", 3000) != AW_OK) {
                ret = -AW_ENOMSG;
                __N58_TRACE("set PDP failed\r\n");
                goto _err_exit;
            }
        }
        /* MT����GPRSҵ��*/
        __at_cmd_send(p_this, "AT+CGATT=1\r");
        if (__at_cmd_result_wait(p_this, "OK", 3000) != AW_OK) {
            ret = -AW_ENOMSG;
            goto _err_exit;
        }
    }
    /* ��ѯ����ע��״̬*/
    __at_cmd_send(p_this, "AT+CREG?\r");
    if (__at_cmd_result_wait(p_this, "0,1", 3000) != AW_OK) {
        ret = -AW_ENOMSG;
        __N58_TRACE("net register failed\r\n");
        goto _err_exit;
    }
    __at_cmd_result_wait(p_this, "OK", 500);
    /* ����*/
    __at_cmd_send(p_this, "ATD*99#\r");
    if (__at_cmd_result_wait(p_this, "CONNECT", 5000) != AW_OK) {
        ret = -AW_ENOMSG;
        __N58_TRACE("connect failed\r\n");
        goto _err_exit;
    }
    /* ����PDP���óɹ�״̬*/
    n58_status_set(p_this, AWBL_GPRS_N58_GPRS_PDP_OK);
    __N58_TRACE("set PDP success\r\n");
    ret = AW_OK;

_err_exit:
    p_this->p_pipe = p_pipe_bp;
    return ret;
}

/** ���й������ݣ�   */
aw_local aw_err_t __n58_dev_ppp_dial_down (struct awbl_gprs_dev *p_gprs)
{
    __N58_DECL_FROM_GPRS(p_this, p_gprs);
    void *p_pipe_bp = p_this->p_pipe;

    p_this->p_pipe = p_this->gprs.ppp.sio_mgn.fd;
    aw_mdelay(1000);
    __at_cmd_send(p_this, "+++");
    aw_mdelay(1000);

    p_this->p_pipe = p_pipe_bp;
    /* ��ָ����ֹ��ǰ��������ͨ�� */
    __at_cmd_send(p_this, "ATH\r");
    if (__at_cmd_result_wait(p_this, "OK", 1000) != AW_OK) {
        return -AW_EBUSY;
    }
    return AW_OK;
}

/**
 * N58 ж�غ���
 *
 * @param p_this N58�豸�ṹ��
 *
 * @return �ɹ�����AW_OK
 */
aw_local aw_err_t __gprs_n58_dev_uninit (struct awbl_gprs_n58_dev *p_this)
{
    if (p_this == NULL) {
        return -AW_EINVAL;
    }

    /* �ر�PPP�ܵ�*/
    if (p_this->gprs.ll_type == AWBL_GPRS_LL_TYPE_PPP &&
            p_this->gprs.ppp.sio_mgn.fd) {
        __usbh_wireless_close(p_this->gprs.ppp.sio_mgn.fd);
        p_this->gprs.ppp.sio_mgn.fd = NULL;
    }

    /* �ر�AT�ܵ�*/
    if (p_this->p_pipe) {
        __usbh_wireless_close(p_this->p_pipe);
        p_this->p_pipe = NULL;
    }
    /* �����ʼ����ɱ�־*/
    if(p_this->init_step > 0){
        p_this->init_step = 1;
    }
    return AW_OK;
}

/**
 * N58 ��ʼ������һ
 *
 * @param p_this N58�豸�ṹ��
 *
 * @return �ɹ�����AW_OK
 */
aw_local aw_err_t __n58_init_step1 (struct awbl_gprs_n58_dev *p_this){
    __N58_DEVINFO_DECL(p_devinfo, &p_this->dev);
    struct awbl_gprs_dev    *p_gprs = NULL;
    awbl_gprs_usbh_info     *p_usbh_info = p_this->p_devinfo->p_usbh_info;
    __LOG_DEV_DEF(p_gprs);

    /* �Ϸ��Լ��*/
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

    /* ��ȡGPRS��Ϣ��SMS��Ϣ */
    p_gprs->p_info      = p_devinfo->p_gprs_info;
    p_gprs->p_sms       = p_devinfo->p_sms_info;
    /* ��ȡGPRSģʽ*/
    p_gprs->ll_type = __usbh_wireless_ll_type_get(p_gprs);

#if AWBL_GPRS_LL_TYPE_ETH_ENABLE
    //todo
#endif
    /* PPPģʽ*/
    if (p_gprs->ll_type == AWBL_GPRS_LL_TYPE_PPP) {
        p_gprs->ppp.pfn_dial_up     = __n58_dev_ppp_pdp_set;
        p_gprs->ppp.pfn_dial_down   = __n58_dev_ppp_dial_down;

        /* ���ùܵ���д����*/
        p_gprs->ppp.sio_mgn.pfn_sio_read  = __usbh_wireless_read;
        p_gprs->ppp.sio_mgn.pfn_sio_write = __usbh_wireless_write;
    }

    p_this->net_mode = 0;

    /* ��ʼ�����ͷ��ź��� */
    aw_assert(p_this->init_step < 1);
    AW_SEMB_INIT(p_this->lock, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);

    p_this->init_step = 1;
    return AW_OK;
}

/**
 * N58 ��ʼ��������
 *
 * @param p_this N58�豸�ṹ��
 *
 * @return �ɹ�����AW_OK
 */
aw_local aw_err_t __n58_init_step2 (struct awbl_gprs_n58_dev *p_this)
{
    aw_err_t ret;
    uint8_t socket[__INTERNAL_SOCKET_NUM] = __INTERNAL_SOCKET_VAL;

    /* ���N58״̬*/
    n58_status_clr(p_this, 0);

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
 * N58 ��ʼ��������
 *
 * @param p_this N58�豸�ṹ��
 *
 * @return �ɹ�����AW_OK
 */
aw_local aw_err_t __n58_init_step3 (struct awbl_gprs_n58_dev *p_this)
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
    /* PPPģʽ���ݽ������ýӿ�4�����Դ򿪹ܵ�4*/
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

    __ITERATE(ret = __n58_dev_check(p_this), ret == AW_OK, 5, 0);
    __ITERATE(ret = __n58_dev_check(p_this), ret == AW_OK, 5, 0);
    __ITERATE(ret = __n58_dev_check(p_this), ret == AW_OK, 5, 0);
    __LOG_RES(ret, 1, "detect AT cmd");
    if (ret != AW_OK) {
        __N58_TRACE("detect AT cmd failed %d\r\n",ret);
        __gprs_n58_dev_uninit(p_this);
        return -AW_EPERM;
    }

    /* at����Ļ������� */
    __n58_dev_at_disp_set(p_this, 0);
    /* �鿴4Gģ��汾*/
    __at_cmd_send(p_this, "AT+GMR\r");
    if (__at_cmd_result_wait(p_this, "OK", 10000) == AW_OK) {
        char *p_begin = NULL;

        p_begin = strstr(p_this->p_devinfo->buf, "\r\nOK\r\n");
        if(p_begin){
            *p_begin = '\0';
        }
        __N58_TRACE("version:%s\r\n",p_this->p_devinfo->buf + 7);
    }
#if 0
    /* ������С����*/
    __at_cmd_send(p_this, "AT+CFUN=0\r");
    if (__at_cmd_result_wait(p_this, "OK", 10000) != AW_OK) {
        __LOG_RES(ret, 1, "AT+CFUN=0 [ ERROR ]");
        __GM510_TRACE("AT+CFUN=0 [ ERROR ] %d\r\n",ret);
        return -AW_EPERM;
    }
    /* ����ģ����Ƶ���շ��书��*/
    __at_cmd_send(p_this, "AT+CFUN=4\r");
    if (__at_cmd_result_wait(p_this, "OK", 10000) != AW_OK) {
        __LOG_RES(ret, 1, "AT+CFUN=4 [ ERROR ]");
        __GM510_TRACE("AT+CFUN=4 [ ERROR ] %d\r\n",ret);
        return -AW_EPERM;
    }
    /* ����ȫ����*/
    __at_cmd_send(p_this, "AT+CFUN=1\r");
    if (__at_cmd_result_wait(p_this, "OK", 10000) != AW_OK) {
        __LOG_RES(ret, 1, "AT+CFUN=1 [ ERROR ]");
        __GM510_TRACE("AT+CFUN=1 [ ERROR ] %d\r\n",ret);
        return -AW_EPERM;
    }
#endif
    /* ��ѯ�����ƶ�̨�豸��ʶ IMSI */
    __ITERATE(ret = __cimi_check(p_this), ret == AW_OK, 5, 500);
    if (ret < 0) {
        /* ��ѯ������Ӫ��*/
        __ITERATE(ret = __cops_check(p_this), ret == AW_OK, 5, 500);
        if (ret != AW_OK) {
            __LOG_RES(ret, 1, "detect APN");
            __N58_TRACE("detect APN %d\r\n",ret);
            p_this->p_devinfo->p_gprs_info->p_apn = NULL;
        }
    }
    if (p_this->p_devinfo->p_gprs_info->p_apn != NULL) {
        __LOG_PFX(1, "detect APN:\"%s\" \n",
                      p_this->p_devinfo->p_gprs_info->p_apn);
        __N58_TRACE("detect APN:\"%s\" \r\n",
                p_this->p_devinfo->p_gprs_info->p_apn);
    } else {
        p_this->p_devinfo->p_gprs_info->p_apn = default_apn;
        __LOG_PFX(1, "set default APN:\"%s\" [ WARN ] \n",
                       p_this->p_devinfo->p_gprs_info->p_apn);
        __N58_TRACE("set default APN:\"%s\" [ WARN ] \r\n",
                p_this->p_devinfo->p_gprs_info->p_apn);
    }

    return AW_OK;
failed2:
    __usbh_wireless_close(p_this->p_pipe);
failed1:
    return ret;
}

/**
 * N58 �ϵ纯��
 *
 * @param p_gprs GPRS�豸�ṹ��
 *
 * @return �ɹ�����AW_OK
 */
aw_local aw_err_t __gprs_power_on (struct awbl_gprs_dev *p_gprs)
{
    __N58_DECL_FROM_GPRS(p_this, p_gprs);
    aw_err_t ret;
    __LOG_DEV_DEF(p_gprs);

    /* ��ʼ����һ��*/
    if (p_this->init_step == 0) {
        ret = __n58_init_step1(p_this);
        __LOG_RES(ret, 1, "__n58_init_step1");
        if (ret != AW_OK) {
            __N58_TRACE("__n58_init_step1 failed %d\r\n",ret);
            return ret;
        }
    }
    /* ��ʼ���ڶ���*/
    if (p_this->init_step == 1) {
        ret = __n58_init_step2(p_this);
        __LOG_RES(ret, 1, "__n58_init_step2");
        if (ret != AW_OK) {
            __N58_TRACE("__n58_init_step2 failed %d\r\n",ret);
            return ret;
        }
    }
    /* ��ʼ��������*/
    if (p_this->init_step == 2) {
        ret = __n58_init_step3(p_this);
        __LOG_RES(ret, 1, "__n58_init_step3");
        if (ret != AW_OK) {
            __N58_TRACE("__n58_init_step3 failed %d\r\n",ret);
            return ret;
        }
    }

    return AW_OK;
}

/**
 * N58 ̽�⺯��
 *
 * @param p_gprs GPRS�豸�ṹ��
 *
 * @return �ɹ�����AW_TRUE
 */
aw_local aw_bool_t __n58_dev_probe (struct awbl_gprs_dev *p_gprs)
{
    void *ptr;
    __N58_DECL_FROM_GPRS(p_this, p_gprs);

    /* �Ϸ��Լ��*/
    if((p_gprs == NULL) ||
       (p_this == NULL) ||
       (p_this->p_devinfo == NULL) ||
       (p_this->p_devinfo->p_usbh_info == NULL)){
        return AW_FALSE;
    }

    /* ���GPRS�Ƿ��Ѿ��ϵ磬�ҵ���GPRS�ϵ纯��*/
    if (!p_gprs->power_on && p_gprs->p_info->pfn_power_on) {
        p_gprs->p_info->pfn_power_on();
    }
    /* ����ܲ��ܴ�AT�ܵ�*/
    ptr = __USBH_WIRELESS_OPEN(__at_com_num_get(p_gprs));
    if (ptr) {
        __usbh_wireless_close(ptr);
        return AW_TRUE;
    }

    return AW_FALSE;
}

/**
 * N58 ���纯��
 *
 * @param p_gprs GPRS�豸�ṹ��
 *
 * @return �ɹ�����AW_OK
 */
aw_local aw_err_t __gprs_power_off (struct awbl_gprs_dev *p_gprs)
{
    aw_err_t ret;
    __N58_DECL_FROM_GPRS(p_this, p_gprs);
    ret = __gprs_n58_dev_uninit(p_this);

    return ret;
}


/** N58 ���ƺ���*/
aw_local aw_err_t __gprs_ioctl (struct awbl_gprs_dev *p_gprs,
                                aw_gprs_ioctl_req_t   req,
                                aw_gprs_ioctl_args_t *p_arg){
    __N58_DECL_FROM_GPRS(p_this, p_gprs);
    aw_err_t ret = -AW_ENOTSUP;
    int      i   = 0;

    if(p_this->p_pipe == NULL){
        return -AW_EINVAL;
    }

    switch (req) {
    case AW_GPRS_REQ_RESET:
        /* �ر�PPP�ܵ�*/
        if (p_this->gprs.ppp.sio_mgn.fd) {
            __usbh_wireless_close(p_this->gprs.ppp.sio_mgn.fd);
            p_this->gprs.ppp.sio_mgn.fd = NULL;
        }
        /* ��λ*/
        ret = __n58_dev_reset(p_gprs, (uint32_t)p_arg);
        break;
    /* ����ź�ǿ��*/
    case AW_GPRS_REQ_CSQ_GET:
        while (i++ < 3) {
            aw_mdelay(2000);
            ret = __n58_dev_csq_check(p_gprs, (int *)p_arg);
            if (ret == AW_OK) {
                break;
            }
            if (ret != -AW_EAGAIN) {
                break;
            }
        }
        break;
    /* ��ȡAPN*/
    case AW_GPRS_REQ_APN_GET:{
        char *p_res_dat[8];
        __at_cmd_send(p_this, "AT+CIMI\r");
        p_res_dat[0] = "46000";                 /* �ƶ� GSM */
        p_res_dat[1] = "46002";                 /* �ƶ� TD-S */
        p_res_dat[2] = "46007";                 /* �ƶ� TD-S */

        p_res_dat[3] = "46001";                 /* ��ͨ GSM */
        p_res_dat[4] = "46006";                 /* ��ͨ WCDMA */

        p_res_dat[5] = "46003";                 /* ���� CDMA */
        p_res_dat[6] = "46005";                 /* ���� CDMA */
        p_res_dat[7] = "46011";                 /* ���� FDD-LTE */

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
    /* ��ѯ�豸״̬*/
    case AW_GPRS_REQ_STS:{
        aw_gprs_sts_t sts = AW_GPRS_STS_OK;

        /* �Ƿ���Ҫ����PIN��*/
        __at_cmd_send(p_this, "AT+CPIN?\r");
        if (__at_cmd_result_wait(p_this, "READY", 4000) != AW_OK) {
            sts = AW_GPRS_STS_PIN_ERROR;
        }
        __at_cmd_result_wait(p_this, "OK", 1000);
        /* GPRS�Ƿ���*/
        __at_cmd_send(p_this, "AT+CGATT=1\r");
        if (__at_cmd_result_wait(p_this, "OK", 5000) != AW_OK) {
            sts = AW_GPRS_STS_CEREG_ERROR;
        }
        __at_cmd_result_wait(p_this, "OK", 1000);
        /* �Ƿ�ע������*/
        __at_cmd_send(p_this, "AT+CREG?\r");
        if (__at_cmd_result_wait(p_this, "0,1", 4000) != AW_OK) {
            sts = AW_GPRS_STS_CEREG_ERROR;
        }
        __at_cmd_result_wait(p_this, "OK", 1000);
        /* �Ƿ�EPSע������*/
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
        /* �����AT����ܵ������ٽ��������շ� */
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
    /* ��ѯ�����������*/
    case AW_GPRS_REQ_NET_MODE_GET:
        break;
    /* ��ѯ��վ*/
    case AW_GPRS_REQ_BS_GET:
        break;
    /* ��ȡSIM����ICCID*/
    case AW_GPRS_REQ_CCID_GET:
        if(p_arg == NULL){
            __N58_TRACE("invalid p_arg\r\n");
            return -AW_EINVAL;
        }
        memset(p_arg->ccid, 0, sizeof(p_arg->ccid));
        ret = __n58_get_ICCID(p_this, p_arg->ccid);
        break;
    case AW_GPRS_REQ_IP_GET:
    /* �鿴4Gģ��汾*/
    case AW_GPRS_REQ_VERSION_GET:
        if(p_arg == NULL){
            __N58_TRACE("invalid p_arg\r\n");
            return -AW_EINVAL;
        }
        memset(p_arg->version, 0, sizeof(p_arg->version));
        ret = __n58_get_version(p_this, p_arg->version);
        break;
    /* ��ȡIMEI*/
    case AW_GPRS_REQ_IMEI_GET:
        if(p_arg == NULL){
            __N58_TRACE("invalid p_arg\r\n");
            return -AW_EINVAL;
        }
        memset(p_arg->imei, 0, sizeof(p_arg->imei));
        ret = __n58_get_IMEI(p_this, p_arg->imei);
        break;
    /* ��ȡIMSI*/
    case AW_GPRS_REQ_IMSI_GET:
        if(p_arg == NULL){
            __N58_TRACE("invalid p_arg\r\n");
            return -AW_EINVAL;
        }
        memset(p_arg->imsi, 0, sizeof(p_arg->imsi));
        ret = __n58_get_IMSI(p_this, p_arg->imsi);
        break;
    /* ��ѯ�������ĺ���*/
    case AW_GPRS_REQ_SCA_GET:
        if(p_arg == NULL){
            __N58_TRACE("invalid p_arg\r\n");
            return -AW_EINVAL;
        }
        memset(p_arg->sca, 0, sizeof(p_arg->sca));
        ret = __n58_get_SCA(p_this, p_arg->sca);
        break;
    /* AT�����*/
    case AW_GPRS_REQ_AT_TRANS:
        ret = __n58_at_trans(p_this, p_arg);
        break;
    /* ���ô�ӡ����*/
    case AW_GPRS_LOG_SET:
        is_gprs_log_set = AW_TRUE;
        break;
    default:
        return -AW_ENOTSUP;
    }
    return ret;
}

/** ��ʼ��PDU SMS*/
aw_local aw_err_t __n58_dev_init_pdu_sms (struct awbl_gprs_n58_dev *p_this)
{
    int   err = 0;
    int   csq = 0;
    /* ����ź�*/
    err = __n58_dev_csq_check(&p_this->gprs, &csq);
    if (err != AW_OK) {
        return err;
    }

    /* ����Ϊ pdu ��ʽ */
    __at_cmd_send(p_this, "AT+CMGF=0\r");
    if (__at_cmd_result_wait(p_this, "OK", 5000) != AW_OK) {
        return -AW_ENOMSG;
    }
    /* ����TE�ַ�����UCS2*/
    __at_cmd_send(p_this, "AT+CSCS=\"UCS2\"\r");
    if (__at_cmd_result_wait(p_this, "OK", 5000) != AW_OK) {
        return -AW_ENOMSG;
    }
    return AW_OK;
}



/** ���Ͷ���*/
aw_local aw_err_t __pdu_sms_send (struct awbl_gprs_dev *p_gprs,
                                  uint8_t              *p_sms,
                                  uint8_t               len,
                                  char                 *p_num,
                                  uint8_t               code)
{
    __N58_DECL_FROM_GPRS(p_this, p_gprs);
    char     buf[370] = { 0 };
    int      err  = 0;

    /*  ���Ž��� */
    aw_const char GPRS_MSG_END[]={0x1A,0x00};

    /* ��ʼ��PDU SMS*/
    err = __n58_dev_init_pdu_sms(p_this);
    if (err != AW_OK) {
        __N58_TRACE("init sms pdu failed\r\n");
        return err;
    }
    /*  һ�����Ķ����Ϊ70�������ַ�  */
    if (len >= 70) {
        return -AW_EMSGSIZE;
    }

    err = awbl_gprs_pdu_pack(buf, sizeof(buf), NULL, p_sms, len, p_num, code);

    /* ��������Ƿ�Ӧ��Ϊ�������ݳ��ȣ�������Ϣ���ĺ��� Service Center Address��*/
    sprintf(p_this->p_devinfo->buf, "AT+CMGS=%d\r", strlen(buf)/2 - 1);
    __at_cmd_send(p_this, p_this->p_devinfo->buf);
    if (__at_cmd_result_wait(p_this, "\r\n>", 5000) != AW_OK) {
        __N58_TRACE("enter sms lengths failed\r\n");
        return -AW_ENOMSG;
    }
    /* ���Ͷ��Ž���*/
    strcat(buf, GPRS_MSG_END);
    __at_cmd_send(p_this, buf);
    if (__at_cmd_result_wait(p_this, "OK", 20000) != AW_OK) {
        n58_status_clr(p_this, AWBL_GPRS_N58_SIM_OK);
        __N58_TRACE("send message failed\r\n");
        return -AW_ENOMSG;
    }
    /* ���ö��ųɹ�״̬*/
    n58_status_set(p_this, AWBL_GPRS_N58_SIM_OK);
    __N58_TRACE("send message success\r\n");
    return AW_OK;
}

/**
 * ���ն���(�Զ���ʼ��)
 *
 * @param p_gprs  ���GPRS�豸
 * @param p_sms   ���ն��ŵ����ݵ�ָ��
 * @param timeout ��ʱʱ��
 *
 * @return �ɹ�����AW_OK
 */
aw_local aw_err_t __gprs_sms_recv(struct awbl_gprs_dev      *p_gprs,
                                  struct aw_gprs_sms_buf    *p_sms,
                                  uint32_t                   timeout){
    __N58_DECL_FROM_GPRS(p_this, p_gprs);
    aw_err_t ret;
    uint16_t index;
    char    *buf;
    char    *buf_tmp;
    char utf8[256] = {0};
    uint32_t rx_timeout, rx_timeout_temp;
    int i;

    /* �Ϸ��Լ��*/
    if((p_this->p_devinfo->p_sms_info->p_sms_buff == NULL) ||
            (p_this->p_devinfo->p_sms_info->sms_buff_len == 0) ||
            (p_this->p_devinfo->p_sms_info->p_num_buff == NULL) ||
            (p_this->p_devinfo->p_sms_info->num_buff_len == 0) ||
            (p_sms == NULL) || (p_sms->p_sms_buff == NULL) ||
            (p_sms->p_num_buff == NULL)){
        __N58_TRACE("invalid p_sms_info\r\n");
        return -AW_EINVAL;
    }

    /* ��ʼ��PDU*/
    ret = __n58_dev_init_pdu_sms(p_this);
    if (ret != AW_OK) {
        __N58_TRACE("init pdu sms failed\r\n");
        return ret;
    }

    /* ����0����*/
    memset(utf8, 0, sizeof(utf8));
    memset(p_this->p_devinfo->buf, 0, p_this->p_devinfo->buf_size);
    /* �������������߰��������*/
    rx_timeout_temp = 500;
    aw_usb_serial_timeout_get(p_this->p_pipe, &rx_timeout);
    aw_usb_serial_timeout_set(p_this->p_pipe, &rx_timeout_temp);
    for(i = 0;i < 3;i++){
        __gprs_at_cmd_recv(&p_this->gprs, p_this->p_devinfo->buf, p_this->p_devinfo->buf_size);
    }
    aw_usb_serial_timeout_set(p_this->p_pipe, &rx_timeout);
#ifdef SMS_DEL
    /* ����������ȡ����*/
    __at_cmd_send(p_this, "AT+CMGD=1,4\r");
    if (__at_cmd_result_wait(p_this, "\r\nOK\r\n", 5000) == AW_OK) {
        return AW_OK;
    }
#endif

    /* ��ȡ���ж��ţ�Ȼ�������һ�����ţ�����ȡ���������ŵ�index��
     * ����indexɾ����������*/
    __at_cmd_send(p_this, "AT+CMGL=4\r");
    if (__at_cmd_result_wait(p_this, "\r\nOK\r\n", 10000) == AW_OK) {
        /* ��ȡ����index */
        buf = __lstrip(p_this->p_devinfo->buf, "+CMGL:");
        if (buf == NULL) {
            __N58_TRACE("get sms index failed\r\n");
            return -AW_ENOMSG;
        }
        index = atoi(buf);
        __at_cmd_result_wait(p_this, "\r\nOK\r\n", 1000);

        /* ��ȡ��һ����������*/
        aw_snprintf(p_this->p_devinfo->buf, p_this->p_devinfo->buf_size,
                    "AT+CMGR=%d\r", index);
        __at_cmd_send(p_this, p_this->p_devinfo->buf);
        if (__at_cmd_result_wait(p_this, "\r\nOK\r\n", 5000) == AW_OK) {
            buf = __lstrip(p_this->p_devinfo->buf, "+CMGR:");
            if (buf == NULL) {
                __N58_TRACE("get sms index failed\r\n");
                return -AW_ENOMSG;
            }

            buf = __lstrip(buf, "\r\n\r\n");
            if (buf == NULL) {
                __N58_TRACE("get sms content failed\r\n");
                return -AW_ENOMSG;
            }
            /* ���ý����� */
            buf_tmp = strstr(buf, "\r\n");
            aw_assert(buf_tmp);
            buf_tmp[0] = 0;
            goto __unpack;
        }
    }

    return -AW_EPERM;
__unpack:
    /* ��������*/
    ret = awbl_gprs_pdu_unpack(buf, p_this->p_devinfo->p_sms_info);
    if (ret == AW_OK) {
        if (p_this->p_devinfo->p_sms_info->sms_code == AW_GPRS_SMS_CODE_UCS2) {
            /* �Ѷ�������ת����utf-8*/
            ret = ucs2_to_utf8(p_this->p_devinfo->p_sms_info->p_sms_buff,
                    p_this->p_devinfo->p_sms_info->sms_buff_len, utf8, sizeof(utf8));
            if(ret == 0){
                __N58_TRACE("ucs2 to utf8 failed\r\n");
                return -AW_EBADMSG;
            }
            utf8[ret] = 0;
            strcpy(p_this->p_devinfo->p_sms_info->p_sms_buff, utf8);
        }

        memcpy(p_sms, p_this->p_devinfo->p_sms_info, sizeof(*p_sms));
        __N58_TRACE("recv sms %d : %s\r\n", index, p_sms->p_num_buff);

        /* ɾ�������Ķ��� */
        sprintf(p_this->p_devinfo->buf, "AT+CMGD=%d\r", index);
        __at_cmd_send(p_this, p_this->p_devinfo->buf);
        if (__at_cmd_result_wait(p_this, "OK", 3000) != AW_OK) {
            __N58_TRACE("delete sms failed\r\n");
            return -AW_ENOMSG;
        }

    } else if (-AW_ENOTSUP == ret) {
        /* ��֧�ֵ����ͣ� ɾ�� */
        sprintf(p_this->p_devinfo->buf, "AT+CMGD=%d\r", index);
        __at_cmd_send(p_this, p_this->p_devinfo->buf);
        if (__at_cmd_result_wait(p_this, "OK", 3000) != AW_OK) {
            __N58_TRACE("delete sms failed\r\n");
            return -AW_ENOMSG;
        }
        return -AW_ENOTSUP;
    } else {
        return -AW_EBADMSG;
    }

    return AW_OK;
}


/**
 * ���Ͷ���(�Զ���ʼ��)
 *
 * @param p_gprs ���GPRS�豸
 * @param p_sms  ���Ͷ��ŵ�����
 * @param len    �������ݵĳ���
 * @param p_num  ���ͺ���
 *
 * @return �ɹ�����AW_OK
 */
aw_local aw_err_t __gprs_sms_send (struct awbl_gprs_dev *p_gprs,
                                   uint8_t              *p_sms,
                                   uint8_t               len,
                                   char                 *p_num)
{
    return __pdu_sms_send(p_gprs, p_sms, len, p_num, AW_GPRS_SMS_CODE_UTF8);
}

/* GPRS��������*/
aw_local aw_const struct awbl_gprs_service __g_gprs_servfunc = {
    .pfn_gprs_power_on      = __gprs_power_on,
    .pfn_gprs_power_off     = __gprs_power_off,
    .pfn_gprs_ioctl         = __gprs_ioctl,
    .pfn_gprs_sms_send      = __gprs_sms_send,
    .pfn_gprs_sms_recv      = __gprs_sms_recv,
//    .pfn_gprs_start         = __gm510_dev_ip_start,
//    .pfn_gprs_close         = __gm510_dev_ip_close,
//    .pfn_gprs_send          = __gm510_dev_ip_send,
    .pfn_probe              = __n58_dev_probe,
    .pfn_at_cmd_send        = __gprs_at_cmd_send,
    .pfn_at_cmd_recv        = __gprs_at_cmd_recv,
};

/** ��ӵ�GPRS���� */
aw_local aw_err_t __n58_dev_get (struct awbl_dev *p_dev, void *p_arg)
{
    __N58_DEV_DECL(p_this, p_dev);
    __N58_DEVINFO_DECL(p_devinfo, p_dev);

    struct awbl_gprs_dev *p_gprs = NULL;
    /* �Ϸ��Լ�� */
    if (!p_dev) {
        return -AW_EINVAL;
    }
    /* ��ȡGPRS�ṹ�� */
    p_gprs = &p_this->gprs;
    /* ��ʼ�������� */
    AW_MUTEX_INIT(p_gprs->mutex, AW_SEM_Q_PRIORITY);
    /* ��ʼ��GPRS�ṹ���Ա */
    p_gprs->p_serv      = &__g_gprs_servfunc;
    p_gprs->p_gprs_dev  = (void *)p_this;
    p_gprs->p_info      = p_devinfo->p_gprs_info;
    p_gprs->dev_id      = p_devinfo->id;
    p_this->p_devinfo   = p_devinfo;
    /* �ѳ�ʼ�����GPRS�ṹ�巵�ظ�GPRS���� */
    *(struct awbl_gprs_dev **)p_arg = p_gprs;

    __N58_TRACE("n58 driver register\r\n");

    return AW_OK;
}

/** �ڶ��׶γ�ʼ�� */
aw_local void __drv_init2 (struct awbl_dev *p_dev)
{
    __N58_DEVINFO_DECL(p_devinfo, p_dev);
    if (p_devinfo->pfn_init) {
        p_devinfo->pfn_init();
    }
}

/** �����׶γ�ʼ�� */
aw_local void __drv_connect(struct awbl_dev *p_dev){
    __N58_DEV_DECL(p_this, p_dev);
    struct awbl_gprs_dev *p_gprs = &p_this->gprs;

    if(p_gprs->p_info){
        /* �û��ϵ纯�� */
        if(p_gprs->p_info->pfn_power_on){
            p_gprs->p_info->pfn_power_on();
        }
    }
}

/** ������ʼ������ */
aw_local aw_const struct awbl_drvfuncs __g_drvfuncs = {
    NULL,
    __drv_init2,
    __drv_connect,
};

/* GPRS����METHOD */
AWBL_METHOD_IMPORT(awbl_gprs_dev_get);

aw_local aw_const struct awbl_dev_method __g_gprs_n58_methods[] = {
    AWBL_METHOD(awbl_gprs_dev_get, __n58_dev_get),
    AWBL_METHOD_END
};

/* ������Ϣ */
aw_local aw_const awbl_plb_drvinfo_t __g_n58_drv_registration = {
    {
        AWBL_VER_1,                         /* awb_ver */
        AWBL_BUSID_PLB,                     /* bus_id */
        AWBL_GPRS_N58_NAME,                 /* p_drvname */
        &__g_drvfuncs,                      /* p_busfuncs */
        &__g_gprs_n58_methods[0],           /* p_methods */
        NULL                                /* pfunc_drv_probe */
    }
};

/** N58 4G����ע�ắ�� */
void awbl_gprs_n58_drv_register (void)
{
    int ret;
    ret = awbl_drv_register((struct awbl_drvinfo *)&__g_n58_drv_registration);
    if (ret != AW_OK) {
        return;
    }
}





