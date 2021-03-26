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

#ifndef __AWBL_USBH_WIRELESS_MANAGER_H
#define __AWBL_USBH_WIRELESS_MANAGER_H
#include "aw_vdebug.h"

/* USB�����豸������ӡ����*/
#define __USB_WIRELESS_TRACE(info) \
            do { \
                AW_INFOF(("USB-WIRELESS:"));\
                AW_INFOF(info);\
            } while (0)

enum{
    PPP_MODE = 1,
    ECM_MODE = 2,
};


/* �����豸�ܵ��ṹ��*/
struct __wireless_pipe {
    struct awbl_usbh_function       *p_fun;        /* ��Ӧ�Ĺ����豸�ṹ��*/
    AW_MUTEX_DECL(                   lock);        /* �ܵ�������*/
    struct aw_list_head              node;         /* �ܵ��ڵ�*/
    struct aw_refcnt                 ref;          /* �ܵ����ü���*/

    struct awbl_usbh_endpoint       *p_in;         /* �豸����˵�*/
    void                            *p_in_buf;     /* �豸����˵����ݻ���*/

    struct awbl_usbh_endpoint       *p_out;        /* �豸����˵�*/
    void                            *p_out_buf;    /* �豸����˵����ݻ���*/
    int                              size;         /* һ�δ�������ݴ�С*/

    aw_bool_t                        removed;      /* �Ƴ�״̬*/
};

/* �������豸�ܵ�*/
void *awbl_usbh_wireless_pipe_open (int     pid,
                                    int     vid,
                                    int     inum,
                                    aw_bool_t rngbuf,
                                    int     size,
                                    int     timeout);
/* �ر������豸�ܵ�*/
void awbl_usbh_wireless_pipe_close (void *pipe);
/* �����豸�ܵ�д*/
int awbl_usbh_wireless_pipe_write (void *pipe, void *p_buf, int len, int timeout);
/* �����豸�ܵ���*/
int awbl_usbh_wireless_pipe_read (void *pipe, void *p_buf, int len, int timeout);
/* �����豸��������ע��*/
void awbl_usbh_wireless_manager_register (int mode);

#endif

