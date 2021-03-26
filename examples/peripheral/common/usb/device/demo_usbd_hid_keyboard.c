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
 * \brief USB device hid ������ʾ����
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
 *   1. �����豸����������һ��hid�豸
 *   2. ������괦ÿ��5sΪ��ӡһ����a��
 *
 */

/** [src_std_usbd_hid] */
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
#include "device/class/hid/aw_usbd_hid.h"
#include "aw_input_code.h"

static struct aw_usbd_hid    __g_usbd_hid;

/** \brief ����ȫ��usb�豸ʵ����usb�豸��Ϣ�ṹ�� */
static struct aw_usbd            __g_usbd_obj;
const static struct aw_usbd_info __g_usbd_info = {
    0x00,
    0x00,
    0x00,
    64,
    0x0100,
    0x4035,
    0x5095,
    "ZLG-KB",
    "AWorks-kb",
    "20160824"
};

/**
 * ���ݽ�������
 */
//static void* __task_rev_entry (void *p_arg)
//{
//    int ret;
//    //struct aw_usbd_hid *p_hid = (struct aw_usbd_hid *)p_arg;
//
//    /* ����һ�������ϱ����ݽṹ�ڴ沢��ʼ�� */
//    aw_usbd_keyboard_out_report_t *p_report =
//            aw_usb_mem_alloc(sizeof(aw_usbd_keyboard_out_report_t));
//    memset(p_report, 0x00, sizeof(aw_usbd_keyboard_out_report_t));
//
//    AW_FOREVER {
//
//        /* �ȴ��������� */
//        ret = aw_usbd_hid_keyboard_receive(&__g_usbd_hid,
//                                           p_report,
//                                           AW_SEM_WAIT_FOREVER);
//        if (ret == AW_OK) {
//            AW_INFOF(("KEY %d PRESSED.\n", p_report->led));
//        }
//    }
//    return 0;
//}

/**
 * \brief USB�������
 * \param[in] p_arg : �������
 * \return ��
 */
static void* __keyboard_task (void *p_arg)
{
    int  ret;
//    AW_TASK_DECL_STATIC(task_rev, 1024);
    aw_usbd_keyboard_in_report_t *p_report = NULL;

    /* ��ʼ��һ��USB�豸 */
    ret = aw_usbd_init(&__g_usbd_obj,
                       &__g_usbd_info,
                       NULL,
                       DE_USB_DEV_NAME);
    if (ret != AW_OK) {
        AW_ERRF(("__g_usbd_obj init failed: %d\n", ret));
        return 0;
    }

    /* ����һ��HID �����豸 */
    ret = aw_usbd_hid_keyboard_create(&__g_usbd_obj,
                                      &__g_usbd_hid,
                                      "hid0_keyboard");
    if (ret != AW_OK) {
        AW_ERRF(("usbd_hid init failed: %d\n", ret));
        return 0;
    }

    /* ����USB�豸 */
    ret = aw_usbd_start(&__g_usbd_obj);
    if (ret != AW_OK) {
        AW_ERRF(("__g_usbd_obj start failed: %d\n", ret));
        return 0;
    }

//    AW_TASK_INIT(task_rev,
//                 "usbd_ms_task",
//                 6,
//                 1024,
//                 __task_rev_entry,
//                (void*)(&__g_usbd_hid));
//
//    AW_TASK_STARTUP(task_rev);

    /* ����һ�������ϱ����ݽṹ�ڴ沢��ʼ�� */
    p_report = aw_usb_mem_alloc(sizeof(aw_usbd_keyboard_in_report_t));
    if (p_report == NULL) {
        AW_ERRF(("p_report alloc failed\r\n"));
        return 0;
    }
    memset(p_report, 0x00, sizeof(aw_usbd_keyboard_in_report_t));

    for (;;) {

        /* �൱�ڼ��̵ġ�A������ */
        p_report->key[0] = key_to_code(KEY_A);
        aw_usbd_hid_keyboard_send(&__g_usbd_hid, p_report, 5000);

        /* �ɿ����� */
        p_report->key[0] = 0;
        aw_usbd_hid_keyboard_send(&__g_usbd_hid, p_report, 5000);
        aw_mdelay(5000);
    }

    return 0;
}


/**
 * \brief USB hid�豸 demo
 * \return ��
 */
void demo_std_usbd_hid_keyboard_entry (void)
{
    AW_TASK_DECL_STATIC(task, 2048);

    AW_TASK_INIT(task,              /* ����ʵ�� */
                "task",             /* �������� */
                 6,                 /* �������ȼ� */
                 2048,              /* �����ջ��С */
                 __keyboard_task,   /* ������ں��� */
                (void*)0);          /* ������ڲ��� */

    AW_TASK_STARTUP(task);
}

/** [src_std_usbd_hid] */

/* end of file */
