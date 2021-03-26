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
 * \brief USBת������ʾ����
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - ��Ӧƽ̨��USBH���USBHЭ��ջ
 *      - AW_DRV_USBH_CDC_SERIAL
 *      - AW_COM_CONSOLE
 *   2. �������е�DURX��DUTXͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *   3. �ڵװ��USB�˿��в���USBת�����ߣ�����Զ�������������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *   4. ���Զ�ͨ���������ַ������ݣ������������ݷ��ظ����Զ�
 *   5. ��demo֧�ֲ��������շ�ģʽ(�첽��ͬ��)����һ�º�Ϳ����л�ģʽ
 *   6. Ŀǰֻ֧��PL2303��CH340��CP210x�� FTDI��USBת����оƬ
 *
 * - ʵ������
 *   1. ���ڴ�ӡ������Ϣ��
 *
 */

#include "aworks.h"
#include "aw_vdebug.h"
#include <string.h>
#include "aw_delay.h"
#include "aw_demo_config.h"
#include "aw_serial.h"

/* ���ͷ�ļ����������޸� */
//#include "awbl_usbh_cdc_serial.h"
#include "host/class/cdc/serial/awbl_usbh_cdc_serial.h"

//#define AYSYNC_MODE
#define SYNC_MODE

#if (defined(AYSYNC_MODE) && defined(SYNC_MODE))
#error "AYSYNC_MODE and SYNC_MODE cannot defined at the same time!"
#endif

#if (!defined(AYSYNC_MODE) && !defined(SYNC_MODE))
#error "AYSYNC_MODE and SYNC_MODE must defined one mode"
#endif

aw_local void* __test_usb_serial_task (void *parg)
{
    aw_err_t    ret;
    int         len = 0;
    char        buf[1024];
    struct aw_usb_serial_dcb dcb;
    uint32_t    timeout;
    uint32_t    tranfer_size;
    void       *handle;
    char       *buf_write = "\r\naworks usb to serial\r\n";

    /* �ȴ��򿪴��ڣ���Զ�ȴ� */
    //handle = awbl_usbh_serial_wait_open(0x6001, 0x0403, AW_USB_WAIT_FOREVER);
    //handle = awbl_usbh_serial_wait_open(0x2303, 0x067b, AW_USB_WAIT_FOREVER);
    //handle = awbl_usbh_serial_wait_open(0x7523, 0x1a86, -1, AW_USB_WAIT_FOREVER);
    //handle = awbl_usbh_serial_wait_open(0xea60, 0x10c4, AW_USB_WAIT_FOREVER);
    handle = awbl_usbh_serial_wait_open(default_pid, default_vid, -1, AW_USB_WAIT_FOREVER);
    if (handle == NULL) {
        aw_kprintf("Open usb serial failed\r\n");
        return 0;
    }

    /* ���Ȼ�ȡdcb��Ĭ������ */
    ret = aw_usb_serial_dcb_get(handle, &dcb);
    if (AW_OK != ret) {
        awbl_usbh_serial_close(handle);
        aw_kprintf("Get usb serial dcb failed\r\n");
        return 0;
    }

    /* �޸� */
    dcb.baud_rate = 115200;
    dcb.byte_size = 8;
    dcb.stop_bits = AW_SERIAL_ONESTOPBIT;

    /* ��У�� */
    dcb.f_parity = AW_FALSE;

#ifdef AYSYNC_MODE
    dcb.read_mode = SERIAL_READ_ASYNC_MODE;
#elif defined (SYNC_MODE)
    dcb.read_mode = SERIAL_READ_SYNC_MODE;
#endif

    /* ����USB����dcb */
    ret = aw_usb_serial_dcb_set(handle, &dcb);
    if (AW_OK != ret) {
        awbl_usbh_serial_close(handle);
        aw_kprintf("Set usb serial dcb failed\r\n");
        return 0;
    }
    ret = aw_usb_serial_dcb_get(handle, &dcb);
    if (AW_OK != ret) {
        awbl_usbh_serial_close(handle);
        aw_kprintf("Get usb serial dcb failed\r\n");
        return 0;
    }

    /* ����USB���ڳ�ʱ */
    ret = aw_usb_serial_timeout_get(handle, &timeout);
    if (AW_OK != ret) {
        awbl_usbh_serial_close(handle);
        aw_kprintf("Get usb serial timeout failed\r\n");
        return 0;
    }
    aw_kprintf("usb serial timeout :%d(ms)\r\n", timeout);

    /* ���ܳ�ʱΪ1s */
    timeout = 1000;
    ret = aw_usb_serial_timeout_set(handle, &timeout);
    if (AW_OK != ret) {
        awbl_usbh_serial_close(handle);
        aw_kprintf("Set usb serial timeout failed\r\n");
        return 0;
    }
    aw_kprintf("usb serial set timeout :%d(ms)\r\n", timeout);

    /* ��ȡ���봫�仺���С */
    ret = awbl_usbh_serial_get_tranfersize(handle, USBH_SERIAL_PIPE_IN, &tranfer_size);
    if (AW_OK != ret) {
        awbl_usbh_serial_close(handle);
        aw_kprintf("Get usb serial IN tranfer size failed\r\n");
        return 0;
    }
    aw_kprintf("usb serial IN tranfer size :%d(Bytes)\r\n", tranfer_size);

    /* �������봫�仺���С */
    tranfer_size = 1024;
    ret = awbl_usbh_serial_set_tranfersize(handle, USBH_SERIAL_PIPE_IN, tranfer_size);
    if (AW_OK != ret) {
        awbl_usbh_serial_close(handle);
        aw_kprintf("Set usb serial IN tranfer size failed\r\n");
        return 0;
    }
    aw_kprintf("usb serial set IN tranfer size :%d(Bytes)\r\n", tranfer_size);

    /* ��������*/
#ifdef AYSYNC_MODE
    aw_kprintf("Async mode\r\n");
    aw_usb_serial_write_async(handle, buf_write, strlen(buf_write));
#elif defined (SYNC_MODE)
    aw_kprintf("Sync mode\r\n");
    aw_usb_serial_write_sync(handle, buf_write, strlen(buf_write));
#endif

    /* �յ�ʲô���ݾͷ���ʲô���� */
    while (1) {
#ifdef AYSYNC_MODE

    /* ��ȡ���� */
        len = aw_usb_serial_read_async(handle, buf, sizeof(buf));
        if (len > 0) {
            ret = aw_usb_serial_write_async(handle, buf, len);
            if (ret != AW_OK)
                break;
            }
            aw_mdelay(1);
        }
#elif defined (SYNC_MODE)

        /* ��ȡ���� */
        len = aw_usb_serial_read_sync(handle, buf, sizeof(buf));
        if (len > 0) {
            len = aw_usb_serial_write_sync(handle, buf, len);
            if (len < 0)
                break;
        }
    }
#endif
    awbl_usbh_serial_close(handle);

    return 0;
}


/**
 * /brief USB����������ں���
 */
void demo_usb_serial_entry (void)
{
    aw_kprintf("USB Serial demo test...\n");

    AW_TASK_DECL_STATIC(usb_serial, 1024 * 4);

    /* ��ʼ������ */
    AW_TASK_INIT(usb_serial,            /* ����ʵ�� */
               "usb_serial",            /* �������� */
               12,                      /* �������ȼ� */
               1024 * 4 ,               /* �����ջ��С */
               __test_usb_serial_task,  /* ������ں��� */
               (void *)NULL);           /* ������ڲ��� */

    /* �������� */
    AW_TASK_STARTUP(usb_serial);
}

/* end of file */
