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
 * \brief U����ʾ���̣�ʹ���ļ�ϵͳ���ж�д��
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - ��Ӧƽ̨��USBH���USBHЭ��ջ
 *      - AW_COM_FS_FATFS
 *      - AW_COM_CONSOLE
 *   2. �������е�DURX��DUTXͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *   3. �ڵװ��USB�˿��в���U�̣�U�̿��豸��Ҫ���ݲ����USB�˿ڶ��ı�
 *
 * - ʵ������
 *   1. ���ڴ�ӡ������Ϣ��
 *
 */

/** [src_udisk_fs] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "fs/aw_mount.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "io/sys/aw_stat.h"
#include "io/sys/aw_statvfs.h"
#include "fs/aw_blk_dev.h"
#include "string.h"
#include "aw_demo_config.h"

/*
 * U�̿��豸����/dev/hx-dy-z
 * 1. x: ���� USB host�ڱ�ţ�
 *    �����USB1, ��ôx==0
 *    �����USB2, ��ôx==1
 * 2. y: �����豸����host��ַ
 * 3. z�������߼���Ԫ��
 * 4. �����ȷ���豸�������Բ鿴U�̲���󴮿ڴ�ӡ��Ϣ���磺
      USB-HUB:new device (VID&048d_PID&1172).
      Manufacturer: Generic
      Product: USB Mass Storage Device
      Serial: 5EE8D32C07043B44
      USBH-MS: luns0's capacity is: 7800 MB
      USBH-MS: /dev/h1-d1-0.
      ���Կ������豸��Ϊ��/dev/h1-d1-0
 */
#define     __BLK_NAME          DE_USB_BLK_NAME /* USB�豸�� */

#define     __FORMAT_ENABLE     0               /* ��ʽ��ʹ�ܣ���ʽ��һ�μ��� */

aw_local void __disk_evt_cb (const char *name, int event, void *p_arg)
{
    /* ͨ�� U�� �Ŀ��豸�����ж��Ƿ�Ϊ U�� ��� */
    if (strcmp(name, __BLK_NAME)) {
        return;
    }

    if (event == AW_BD_EVT_INJECT) {
        AW_INFOF(("udisk insert...\r\n"));
    }

    if (event == AW_BD_EVT_EJECT) {
        AW_INFOF(("udisk remove...\r\n"));
    }
}


aw_local int __fs_file_rw (void)
{
    int         i = 0;
    int         handle;
    char       *p_file_name = "/u/aworks_udisk_test.txt";
    uint8_t     str_buf[256] = {0};

    /*
     * д�ļ����ԣ������������ļ�д�������رղ�����
     */
    /* �������ļ� */
    handle = aw_open(p_file_name, O_RDWR | O_CREAT, 0777);
    if (handle < 0) {
        AW_ERRF(("* Creat file Fail: %d\n", handle));
        return -1;
    }
    AW_INFOF(("* Creat file %s OK\n", p_file_name));

    for (i = 0; i < sizeof(str_buf); i++) {
        str_buf[i] = (uint8_t)i;
    }

    /* д�ļ� */
    if (aw_write(handle, str_buf, sizeof(str_buf)) != sizeof(str_buf)) {
        aw_close(handle);
        AW_ERRF(("* Write file Fail\n"));
        return -1;
    }
    AW_INFOF(("* Write file %s OK\n", p_file_name));

    /* �ر��ļ� */
    aw_close(handle);
    AW_INFOF(("* Close file %s OK\n", p_file_name));

    /*
     * ���ļ����ԣ������򿪣��ļ����������رղ�����
     */
    /* ���ļ� */
    handle = aw_open(p_file_name, O_RDONLY, 0777);
    if (handle < 0) {
        AW_ERRF(("* Open file Fail: %d\n", handle));
        return -1;
    }
    AW_INFOF(("* Open file %s OK\n", p_file_name));

    memset(str_buf, 0, sizeof(str_buf));

    /* ��ȡ�ļ� */
    if (aw_read(handle, str_buf, sizeof(str_buf)) != sizeof(str_buf)) {
        aw_close(handle);
        AW_ERRF(("* Read file Fail!\n"));
        return -1;
    }
    AW_INFOF(("* Read file %s OK\n", p_file_name));

    /* �ر��ļ� */
    aw_close(handle);
    AW_INFOF(("* Close file %s OK\n", p_file_name));

    /* ���������Ƿ���ȷ */
    for (i = 0; i < sizeof(str_buf); i++) {
        if ((uint8_t)i != str_buf[i]) {
            AW_ERRF(("* File data Fail!\n"));
            aw_close(handle);
            return -1;
        }
    }
    AW_INFOF(("* File %s data check OK\n", p_file_name));
    return AW_OK;
}

/**
 * \brief U���ļ�ϵͳ�������
 * \return ��
 *
 */
int demo_udisk_fs_entry (void)
{
    aw_err_t    ret,result;
    aw_bool_t   regist_ok   = AW_TRUE;
    int         event_index = 0;
    struct aw_statvfs fs_info;
    uint32_t total_size, remain_size;
    /*
     * ��ӿ��豸����¼����, ����U�̲��ʱ, ��ص�__disk_evt_cb
     */
    ret = aw_blk_dev_event_register(__disk_evt_cb, NULL, &event_index);
    if (ret != AW_OK) {
        AW_ERRF(("block event register error: %d!\n", ret));
        regist_ok = AW_FALSE;
    }

    /* ���U�̿��豸�Ƿ���� */
    ret = aw_blk_dev_wait_install(__BLK_NAME, 10000);
    if (ret != AW_OK) {
        AW_ERRF(("wait udisk insert failed, err = %d\r\n", ret));
        goto __task_udisk_fs_end;
    }


#if __FORMAT_ENABLE

    /* ����Ϊ"awdisk"�� ���СΪ4k */
    struct aw_fs_format_arg fmt = {"awdisk", 1024 * 4, 0};

    /* �����ļ�ϵͳ �����洢������Ϊ"vfat"���͵��ļ�ϵͳ */
    if ((ret = aw_make_fs(__BLK_NAME, "vfat", &fmt)) != AW_OK) {
        AW_ERRF(("failed: %d\n", ret));
        goto __task_udisk_fs_end;
    }
    AW_INFOF(("make fs OK\n"));

#endif

    /* �ļ�ϵͳ���ص� "/u" ��� */
    ret = aw_mount("/u", __BLK_NAME, "vfat", 0);
    if (ret != AW_OK) {
        AW_ERRF(("* /u mount FATFS Fail: %d!\n", ret));
        goto __task_udisk_fs_end;
    }
    AW_INFOF(("* USB Disk Mount OK\n"));

    /* ��ȡU����Ϣ*/
    ret = aw_statvfs("/u", &fs_info);
    if(ret == AW_OK){
        total_size = fs_info.f_bsize * fs_info.f_blocks / (1024 * 1024);
        aw_kprintf("udisk total  size: %d MB(s)\r\n", total_size);
        remain_size = fs_info.f_bsize * fs_info.f_bfree / (1024 * 1024);
        aw_kprintf("udisk remain size: %d MB(s)\r\n", remain_size);
    }

    /* U�̶�д���� */
    ret = __fs_file_rw();
    if(ret != AW_OK)
        return ret;

__task_udisk_fs_end:
    /* ע���¼� */
    if (regist_ok) {
        result = aw_blk_dev_event_unregister(event_index);
        if (result != AW_OK) {
            AW_ERRF(("block event unregister error: %d!\n", ret));
            return result;
        }
    }

    return ret;
}

/** [src_udisk_fs] */

/* end of file */
