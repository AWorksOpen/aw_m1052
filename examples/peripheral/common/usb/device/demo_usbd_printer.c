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
*******************************************************************************/

/**
 * \file
 * \brief USB �����ӡ����ʾ����
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - ��Ӧƽ̨��USBD���USBDЭ��ջ
 *      - AW_COM_CONSOLE
 *   2. �������е�DURX��DUTXͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *   3. ��������USB DEVICE�ӿ������ʹ��USB������
 *
 * - ʵ������
 *   1. ���ѡ��һ���ĵ��������ӡ���ڴ�ӡ����ѡ����Ӵ�ӡ����
 *    . �����ʹ�����еĶ˿ڡ���ѡ��USB�����ӡ���˿ڡ�
 *    . ����ѡ��Generic������ӡ��ѡ��Generic/Text Only��
 *    . �����ʹ�õ�ǰ�Ѱ�װ����������
 *   2. ��ӡ����ӳɹ��󣬵����ӡ�����ڻ��յ���ƪ�ĵ�������
 *
 */

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
#include "device/class/vprinter/aw_usbd_vprinter.h"

#define __BUF_SIZE  (4 * 1024)

static aw_usbd_vp_t __g_usbd_printer;

/** \brief ����ȫ��usb�豸ʵ����usb�豸��Ϣ�ṹ�� */
static struct aw_usbd            __g_usbd_obj;
const static struct aw_usbd_info __g_usbd_info = {
    0x00,
    0x00,
    0x00,
    64,
    0x0100,
    0x4033,
    0x5093,
    "ZLG-VP",
    "AWorks-vp",
    "20160824"
};

/**
 * \brief USB�������
 * \param[in] p_arg  �������
 * \retval  ��
 */
static void* __printer_task (void *p_arg)
{
    aw_err_t    ret;
    void       *p_buf;

    /* ��ʼ��һ��USB�豸 */
    ret = aw_usbd_init(&__g_usbd_obj,
                       &__g_usbd_info,
                       NULL,
                       DE_USB_DEV_NAME);
    if (ret != AW_OK) {
        AW_ERRF(("__g_usbd_obj init failed: %d\n", ret));
        return 0;
    }

    /* ����һ����ӡ���豸 */
    ret = aw_usbd_printer_create(&__g_usbd_obj,
                                 &__g_usbd_printer,
                                 "usb_printer");
    if (ret != AW_OK) {
        AW_ERRF(("usbd_printer init failed: %d\n", ret));
        return 0;
    }

    /* ����USB�豸 */
    ret = aw_usbd_start(&__g_usbd_obj);
    if (ret != AW_OK) {
        AW_ERRF(("__g_usbd_obj start failed: %d\n", ret));
    }

    /* �������ݻ��� */
    p_buf = aw_usb_mem_alloc(__BUF_SIZE);
    if (p_buf == NULL) {
        AW_ERRF(("aw_usb_mem_alloc error!"));
        return 0;
    }

    while (1) {

        /* �ȴ�USB�豸���ӵ��� */
        while (!aw_fun_valid(&__g_usbd_printer.fun)) {
            AW_INFOF(("custom usb wait for connect.\n"));
            aw_usb_sem_take(__g_usbd_printer.semb, AW_SEM_WAIT_FOREVER);
        }

        /* ��������(����) */
        ret = aw_usbd_cdc_vp_read_sync(&__g_usbd_printer,
                                       p_buf,
                                       __BUF_SIZE,
                                       AW_SEM_WAIT_FOREVER);
        if (ret > 0) {
            memset((char *)p_buf + ret, 0, __BUF_SIZE - ret);
            AW_INFOF(("[recv]:%s\n", (char *)p_buf));
        }
    }

    return 0;
}

/**
 * \brief USB ��ӡ���豸 demo
 * \retval ��
 */
void demo_usbd_printer_entry (void)
{
    AW_TASK_DECL_STATIC(task, 2048);

    AW_TASK_INIT(task,              /* ����ʵ�� */
                "task",             /* �������� */
                 6,                 /* �������ȼ� */
                 2048,              /* �����ջ��С */
                 __printer_task,    /* ������ں��� */
                (void*)0);          /* ������ڲ��� */

    AW_TASK_STARTUP(task);
}









