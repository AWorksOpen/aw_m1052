/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief USB �Զ����豸��ʾ���̣�USB device �Զ���USB�豸,��ʾ���̣�
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - ��Ӧƽ̨��USBD���USBDЭ��ջ
 *      - AW_COM_CONSOLE
 *   2. �������е�DURX��DUTXͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *   3. ��������USB DEVICE0�ӿ������ʹ��USB������
 *
 * - ʵ������
 *   1. �����豸����������һ��AWorks USB�Զ����豸
 *   2. ��װ�ṩ����������
 *   3. �����׵���λ�����(����NI MAX���)���������շ�����
 *
 */

/** [src_usbd_vendor] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_task.h"
#include "aw_sem.h"
#include "aw_assert.h"
#include "device/aw_usbd.h"
#include "aw_usb_mem.h"
#include "string.h"
#include "aw_demo_config.h"

#define __BUF_SIZE          512
#define __WAIT_TIMEOUT      5000
/* USB�豸����������*/
#define __DISK_NAME         DE_USB_DEV_NAME

/** \brief ����ȫ��usb�豸ʵ����usb�豸��Ϣ�ṹ�� */
static struct aw_usbd            __g_usbd_obj;
const static struct aw_usbd_info __g_usbd_info = {
    0,
    0,
    0,
    64,
    0x0100,
    0x3068,
    0x0003,
    "ZLG",
    "AWorks",
    "20160824"
};

/** \brief ������ûص����� */
static int __custom_alt_set (struct aw_usbd_fun  *p_fun,
                             aw_bool_t            set);

/** \brief �����շ������� */
static void* __custom_task (void *arg);

/** \brief �ӿڶ��� */
static struct aw_usbd_fun   __g_ufun;

/** \brief ��������ܵ����� */
static struct aw_usbd_pipe  __g_in;
static struct aw_usbd_pipe  __g_out;

AW_SEMB_DECL_STATIC(__g_semb);

/** \brief �����շ����� */
AW_TASK_DECL_STATIC(__g_task, 2048);

/** \brief �ӿ���Ϣ */
static const struct aw_usbd_fun_info __g_info = {
    AW_USB_CLASS_VENDOR_SPEC,       /**< \brief �����-�����Զ����豸 */
    0x00,                           /**< \brief ������� */
    0x00,                           /**< \brief Э����� */
    __custom_alt_set,               /**< \brief ������ûص� */
    NULL                            /**< \brief ���ƴ���ص�  */
};

/** \brief IN �ܵ���Ϣ */
static const struct aw_usbd_pipe_info __g_in_info = {
    0,                              /**< \brief �˵��ַ�Զ����� */
    AW_PIPE_DIR_IN,                 /**< \brief IN�˵� */
    AW_PIPE_TYPE_BULK,              /**< \brief ������������ */
    0,                              /**< \brief ����ʱ��Ч */
    512                             /**< \brief ������С512�ֽ� */
};

/** \brief OUT �ܵ���Ϣ */
static const struct aw_usbd_pipe_info __g_out_info = {
    0,                              /**< \brief �˵��ַ�Զ����� */
    AW_PIPE_DIR_OUT,                /**< \brief OUT�˵� */
    AW_PIPE_TYPE_BULK,              /**< \brief ������������ */
    0,                              /**< \brief ����ʱ��Ч */
    512                             /**< \brief ������С512�ֽ� */
};

static int __custom_alt_set (struct aw_usbd_fun  *p_fun,
                             aw_bool_t            set)
{
    if (set) {
        AW_SEMB_GIVE(__g_semb);
    } else {
        aw_usbd_pipe_reset(p_fun->p_obj, &__g_in);
        aw_usbd_pipe_reset(p_fun->p_obj, &__g_out);
    }
    return AW_OK;
}

/* USB�շ����� */
/* arg���������(������USB�ӿڽṹ��) */
static void* __custom_task (void *arg)
{
    int   ret;
    void *p_buf;

    /* �������ݻ��� */
    p_buf = aw_usb_mem_alloc(__BUF_SIZE);
    if (p_buf == NULL) {
        aw_kprintf("aw_usb_mem_alloc error!");
        return 0;
    }

    AW_FOREVER {

        /* �ȴ�USB�豸���ӵ��� */
        while (!aw_fun_valid(&__g_ufun)) {
            AW_INFOF(("custom usb wait for connect.\n"));
            AW_SEMB_TAKE(__g_semb, AW_SEM_WAIT_FOREVER);
        }

        /* ���ݻ�����0 */
        memset(p_buf, 0, __BUF_SIZE);

        /* ��������(����) */
        ret = aw_usbd_trans_sync(__g_ufun.p_obj,
                                 &__g_out,
                                 p_buf,
                                 __BUF_SIZE,
                                 0,
                                 AW_SEM_WAIT_FOREVER);
        if (ret > 0) {
            aw_kprintf("[recv]:%s\n", (char *)p_buf);
        }
        aw_kprintf("[recv_len]:%d\n", ret);

        /* �����ݷ��ͻ�ȥ */
        ret = aw_usbd_trans_sync(__g_ufun.p_obj,
                                 &__g_in,
                                 p_buf,
                                 ret,
                                 AWBL_USBD_ZERO_PACKET,
                                 __WAIT_TIMEOUT);
        aw_kprintf("[tranfer]:%d\n", ret);
    }

    return 0;
}

/**
 * \brief �Զ������������
 */
aw_local int __demo_usbd_vendor (void)
{
    int   ret;

    /* ��ʼ���ӿڹ��� */
    ret = aw_usbd_fun_init(&__g_ufun,
                           "AWORKS-USB",
                           &__g_info);
    if (ret != AW_OK) {
        return ret;
    }

    /* ����IN�ܵ� */
    ret = aw_usbd_pipe_create(&__g_ufun, &__g_in, &__g_in_info);
    if (ret != AW_OK) {
        return ret;
    }

    /* ����OUT�ܵ� */
    ret = aw_usbd_pipe_create(&__g_ufun, &__g_out, &__g_out_info);
    if (ret != AW_OK) {
        return ret;
    }

    /* ��ӽӿڹ��ܵ�USB�豸���� */
    ret = aw_usbd_fun_add(&__g_usbd_obj, 0, &__g_ufun);
    if (ret != AW_OK) {
        return ret;
    }

    /* ��ʼ���ź��� */
    AW_SEMB_INIT(__g_semb, 0, AW_SEM_Q_PRIORITY);

    /* ��ʼ��һ������ */
    AW_TASK_INIT(__g_task,
                 "CUSTOM-USB",
                 10,
                 2048,
                 __custom_task,
                 &__g_ufun);

    /* �������� */
    AW_TASK_STARTUP(__g_task);

    return AW_OK;
}

/**
 * \brief �Զ���USB�豸 demo
 * \return ��
 */
void demo_usbd_vendor_entry (void)
{
    int  ret;

    /*
     * USB device ��ʼ��
     * "/dev/usbd" ��awbl_hwconf_xxxxxx_usbd.h �ж���
     */
    ret = aw_usbd_init(&__g_usbd_obj,
                       &__g_usbd_info,
                       NULL,
                       __DISK_NAME);
    if (ret != AW_OK) {
        AW_ERRF(("__g_usbd_obj init failed: %d\n", ret));
        return;
    }

    /* �Զ������������ */
    __demo_usbd_vendor();

    /* ����USB�豸 */
    ret = aw_usbd_start(&__g_usbd_obj);
    if (ret != AW_OK) {
        AW_ERRF(("__g_usbd_obj start failed: %d\n", ret));
        return;
    }

    return ;
}

/** [src_usbd_vendor] */

/* end of file */
