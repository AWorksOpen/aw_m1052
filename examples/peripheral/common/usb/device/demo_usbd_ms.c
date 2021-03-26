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
 * \brief U����ʾ���̣�USB device �������洢����ʾ��
 *
 * - �������裺
 *   1. ���������� aw_prj_params.h ͷ�ļ���ʹ��
 *      - ��Ӧƽ̨��USBD���USBDЭ��ջ
 *      - AW_COM_FS_FATFS
 *      - AW_COM_CONSOLE
 *   2. �������е�DURX��DUTXͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 *   3. �������к󽫿�����USB DEVICE0�ӿ������ʹ��USB������
 *
 * - ʵ������
 *   1. �����豸����������һ������Ϊ "AWorks" �Ŀ��ƶ��洢�豸
 *
 * - ��ע��
 *   1. ������ڴ治������´���ʧ�ܣ��볢�ԹرշǱ�Ҫ������޸Ĳ�����С��Ҫ��
 *      �ռ��С��
 */

/** [src_usbd_ms] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "device/aw_usbd.h"
#include "device/class/ms/aw_usbd_ms.h"
#include "fs/aw_ram_disk.h"
#include "fs/aw_blk_dev.h"
#include "fs/aw_mount.h"
#include "string.h"
#include "aw_demo_config.h"

#define __MS_SDCARD     0
#define __MS_RAM_DISK   1

/** \brief ʹ��SD������RAM_DISKģ��U��
 * ��ʹ��SD������Ҫ��aw_prj_params.h��ʹ��SD���豸
 * ���Ұ��� SD0 ��Ҫ����SD��
 */
#define __MS_USE_TYPE   __MS_RAM_DISK
//#define __MS_USE_TYPE   __MS_SDCARD

#define __USB_DEV_NAME  DE_USB_DEV_NAME

#if __MS_USE_TYPE == __MS_SDCARD
#include "aw_sdcard.h"

#define __DISK_NAME     DE_SD_DEV_NAME
#endif

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
    "ZLG-MS",
    "AWorks-ms",
    "20160824"
};


/** \brief �������洢��ʵ������ */
static struct aw_usbd_ms      __g_usbd_ms;

#if __MS_USE_TYPE == __MS_RAM_DISK

/** \brief ram_diskʵ������ */
static struct aw_ram_disk     __g_ramdisk;

/** \briefram_disk�ռ��С����U�̿ռ��С�� */
static char                   __g_ram[100 * 1024];
#endif

/**
 * \brief USB device �������洢�� demo
 * \return ��
 */
void demo_usbd_ms_entry (void)
{
    int         ret;
    const char *disk;
#if __MS_USE_TYPE == __MS_SDCARD
    disk = __DISK_NAME;
    aw_blk_dev_wait_install(disk, AW_WAIT_FOREVER);
#endif

    /*
     * USB device ��ʼ��
     * "/dev/usbd0" �� awbl_hwconf_xxxxxx_usbd.h �ж���
     */
    ret = aw_usbd_init(&__g_usbd_obj,
                       &__g_usbd_info,
                       NULL,
                       DE_USB_DEV_NAME);
    if (ret != AW_OK) {
        AW_ERRF(("__g_usbd_obj init failed: %d\n", ret));
        return ;
    }

#if __MS_USE_TYPE == __MS_RAM_DISK
    disk = __USB_DEV_NAME;

    /* �洢������Ŀ�����������192�� */
    ret = aw_ram_disk_create(&__g_ramdisk,
                             disk,
                             512,                /* һ����Ĵ�С */
                             __g_ram,
                             sizeof(__g_ram),    /* �洢���ܴ�С */
                             NULL);
    if (ret != AW_OK) {
        AW_ERRF(("disk create failed: %d\n", ret));
        return;
    }

    /* �� fat ��ʽ�� ram disk */
    ret = aw_make_fs(disk, "vfat", NULL);
    if (ret != AW_OK) {
        AW_ERRF(("disk make_fs failed: %d\n", ret));
        return;
    }

#endif

    ret = aw_usbd_ms_create(&__g_usbd_obj,
                            &__g_usbd_ms,
                            "UDISK",
                            NULL,
                            20 * 1024);
    if (ret != AW_OK) {
        AW_ERRF(("aw_usbd_ms_create failed: %d\n", ret));
        return;
    }

    aw_usbd_ms_lun_add(&__g_usbd_ms, disk);
    if (ret != AW_OK) {
        AW_ERRF(("__g_usbd_ms_lun init failed: %d\n", ret));
        return;
    }

    ret = aw_usbd_start(&__g_usbd_obj);
    if (ret != AW_OK) {
        AW_ERRF(("__g_usbd_obj start failed: %d\n", ret));
    }

    return ;
}

/** [src_usbd_ms] */

/* end of file */
