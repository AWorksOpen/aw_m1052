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
 * \brief USB device cdc ���⴮����ʾ����
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
 *   1. ���Գ���һ�����⴮���豸�����豸������˿ڿ��Կ�����Ӧ��COM�ڣ�����Զ�
 *      ��װ����ʧ�ܣ����ֶ���װͨ��CDC����������
 *   2. �򿪴������������ȴ���ȽϾ�
 *   3. �����������������ֵ�����յ���Ӧ������
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
#include "device/class/cdc/aw_usbd_cdc_virtual_serial.h"
#include "aw_demo_config.h"
#include "aw_serial.h"

static struct aw_usbd_cdc_vs    __g_usbd_cdc;

/* ����ȫ��usb�豸ʵ����usb�豸��Ϣ�ṹ�� */
static struct aw_usbd            __g_usbd_obj;
const static struct aw_usbd_info __g_usbd_info = {
    0xEF,
    0x02,
    0x01,
    64,
    0x0100,
    0x6047,
    0x5052,
    "ZLG-VS",
    "AWorks-vs",
    "20160824"
};

/* USB�������
 * p_arg : �������
 */
static void* __serial_task (void *p_arg)
{
    aw_err_t                ret;
    char                   *buff        = "Hello,i am AWorks\r\n";
    char                   *read_buf    = NULL;
    unsigned int            len;
    struct aw_usbd_trans    tx_trans;

    /* ��������� */
    read_buf = aw_usb_mem_alloc(1024);
    if (read_buf == NULL) {
        AW_ERRF(("read_buf alloc failed\r\n"));
        return 0;
    }

    /* ��ʼ��һ��USB�豸 */
    ret = aw_usbd_init(&__g_usbd_obj,
                       &__g_usbd_info,
                       NULL,
                       DE_USB_DEV_NAME);
    if (ret != AW_OK) {
        AW_ERRF(("__g_usbd_obj init failed: %d\r\n", ret));
        return 0;
    }

    /* ����һ�����⴮�� */
    ret = aw_usbd_cdc_vs_create(&__g_usbd_obj,
                                &__g_usbd_cdc,
                                0,
                                "cdc0",
                                1024);
    if (ret != AW_OK) {
        AW_ERRF(("usbd_cdc init failed: %d\r\n", ret));
        return 0;
    }

    /* ���ô���ģʽ */
    //ret = aw_usbd_cdc_vs_set_rx_mode(&__g_usbd_cdc, AW_CDC_VS_TRANFER_SYNC);
    ret = aw_usbd_cdc_vs_set_rx_mode(&__g_usbd_cdc, AW_CDC_VS_TRANFER_ASYNC);
    if (ret != AW_OK) {
        AW_ERRF(("usbd_cdc set rx mode failed: %d\r\n", ret));
        return 0;
    }

    //ret = aw_usbd_cdc_vs_set_tx_mode(&__g_usbd_cdc, AW_CDC_VS_TRANFER_SYNC);
    ret = aw_usbd_cdc_vs_set_tx_mode(&__g_usbd_cdc, AW_CDC_VS_TRANFER_ASYNC);
    if (ret != AW_OK) {
        AW_ERRF(("usbd_cdc set tx mode failed: %d\r\n", ret));
        return 0;
    }

    /* ����USB�豸 */
    aw_usbd_start(&__g_usbd_obj);
    aw_mdelay(10);

    if (__g_usbd_cdc.tx_mode == AW_CDC_VS_TRANFER_ASYNC) {
        memset(&tx_trans, 0, sizeof( struct aw_usbd_trans));
        aw_usbd_fill_trans(&tx_trans, &__g_usbd_cdc.in, buff, 19, 0, NULL, NULL);
        aw_usbd_cdc_vs_async_write(&__g_usbd_cdc, &tx_trans);
    } else if (__g_usbd_cdc.rx_mode == AW_CDC_VS_TRANFER_SYNC) {
        aw_usbd_cdc_vs_poll_write(&__g_usbd_cdc, buff, 19, AW_USB_WAIT_FOREVER);
    }

    for (;;) {
        while (!aw_fun_valid(&__g_usbd_cdc.dfun)) {
            AW_INFOF(("USBD_CDC: wait for connect.\n"));
            aw_usb_sem_take(__g_usbd_cdc.sync_semb, AW_USB_WAIT_FOREVER);
        }

        if (__g_usbd_cdc.rx_mode == AW_CDC_VS_TRANFER_SYNC) {
            memset(read_buf, 0x00, 1024);
            len = aw_usbd_cdc_vs_poll_read(&__g_usbd_cdc, read_buf, 1024, AW_USB_WAIT_FOREVER);
            if (len > 0) {
                aw_usbd_cdc_vs_poll_write(&__g_usbd_cdc, read_buf, len, 5000);
            }
        } else if (__g_usbd_cdc.rx_mode == AW_CDC_VS_TRANFER_ASYNC) {
            memset(read_buf, 0x00, 1024);
            ret = aw_usbd_cdc_vs_async_read(&__g_usbd_cdc, read_buf, 1024, &len);
            if (len > 0) {
                memset(&tx_trans, 0, sizeof( struct aw_usbd_trans));
                aw_usbd_fill_trans(&tx_trans, &__g_usbd_cdc.in, read_buf, len, 0, NULL, NULL);
                aw_usbd_cdc_vs_async_write(&__g_usbd_cdc, &tx_trans);
            }
            aw_mdelay(10);
        }
    }

    return 0;
}

/* USB cdc ���� demo */
void demo_std_usbd_cdc_serial_entry (void)
{
    AW_TASK_DECL_STATIC(task, 2048);

    AW_TASK_INIT(task,          /* ����ʵ�� */
                "task",         /* �������� */
                 6,             /* �������ȼ� */
                 2048,          /* �����ջ��С */
                 __serial_task, /* ������ں��� */
                (void*)0);      /* ������ڲ��� */

    AW_TASK_STARTUP(task);
}

/** [src_std_usbd_cdc] */

/* end of file */
