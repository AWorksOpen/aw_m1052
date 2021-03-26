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
 * \brief USB �ļ�ϵͳ������ʾ���̣�ʹ���ļ�ϵͳ���ж�д��
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
#include "aw_mem.h"
#include "aw_vdebug.h"
#include "fs/aw_mount.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "io/sys/aw_stat.h"
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

/* ��ȡ��Сֵ */
uint32_t get_average_min(uint32_t *data, int num)
{
    int      j          = 0;
    uint32_t temp_min   = 0;;

    temp_min = data[0];
    for (j = 0; j < num; j++) {
        if (temp_min > data[j]) {
            temp_min = data[j];
        }
    }
    return temp_min;
}

/* ��ȡ���ֵ */
uint32_t get_average_max(uint32_t *data, int num)
{
    int      j          = 0;
    uint32_t temp_max   = 0;;

    temp_max = data[0];
    for (j = 0; j < num; j++) {
        if (temp_max < data[j]) {
            temp_max = data[j];
        }
    }
    return temp_max;
}


aw_local int __fs_speed_test (void)
{
    int         i = 0 , j = 0;
    int         handle, len;
    char       *p_file_name = "/u/aworks_udisk_test.txt";
    uint8_t    *str_buf = NULL;
    uint32_t    str_buf_size = 0;
    uint32_t    loop_num = 0;
    aw_tick_t   start_ticks;
    aw_tick_t   end_ticks;
    uint32_t    timeout_ms[10] = {0};
    uint32_t    average_timeout_ms = 0;
    uint32_t    timeout_ms_min = 0;
    uint32_t    timeout_ms_max = 0;
    float       result = 0;

    /* ����ռ�*/
    str_buf = aw_mem_alloc(1 * 1024 * 1024);
    if(str_buf == NULL){
        str_buf = aw_mem_alloc(4 * 1024);
        if(str_buf == NULL){
            return -AW_ENOMEM;
        }
        else{
            str_buf_size = 4 * 1024;
            loop_num = 256;
        }
    }
    else{
        str_buf_size = 1024 * 1024;
        loop_num = 1;
    }

    /* д���� */
    for (i = 0; i < str_buf_size; i++) {
        str_buf[i] = (uint8_t)i;
    }
    /*
     * д�ļ����ԣ������������ļ�д�������رղ�����
     */
    /* �������ļ� */
    for (j = 0; j < 10; j++) {
        handle = aw_open(p_file_name, O_RDWR | O_CREAT, 0777);
        if (handle < 0) {
            AW_ERRF(("* Creat file Fail: %d\n", handle));
            aw_mem_free(str_buf);
            return -1;
        }

        /* д�ļ�(1M) */
        start_ticks = aw_sys_tick_get();
        for (i = 0;i < loop_num; i++) {
            len = aw_write(handle, str_buf, str_buf_size);
            if (len != str_buf_size) {
                aw_close(handle);
                AW_ERRF(("* Write file Fail\n"));
                aw_mem_free(str_buf);
                return -1;
            }
        }
        end_ticks  = aw_sys_tick_get();
        timeout_ms[j] = aw_ticks_to_ms(end_ticks - start_ticks);
        AW_INFOF(("* Write 1M data success,use time %d(ms)\r\n", timeout_ms[j]));

        /* �ر��ļ� */
        aw_close(handle);
    }

    timeout_ms_min = get_average_min(timeout_ms, 10);
    timeout_ms_max = get_average_max(timeout_ms, 10);

    /* ʱ��ȡƽ��ֵ */
    for (j = 0; j < 10; j++) {
        average_timeout_ms += timeout_ms[j];
    }
    average_timeout_ms -= (timeout_ms_min + timeout_ms_max);
    average_timeout_ms /= 8;
    result = 1024*1000/(float)average_timeout_ms;

    /* ��ӡ�ٶ� */
    AW_INFOF(("* Write average speed : %.2f KB/s\r\n", result));

    average_timeout_ms = 0;

    /*
     * ���ļ����ԣ������򿪣��ļ����������رղ�����
     */
    /* ���ļ� */
    for (j = 0; j < 10; j++) {
        handle = aw_open(p_file_name, O_RDONLY, 0777);
        if (handle < 0) {
            AW_ERRF(("* Open file Fail: %d\n", handle));
            aw_mem_free(str_buf);
            return -1;
        }

        memset(str_buf, 0, str_buf_size);

        /* ���ļ�(1M) */
        start_ticks = aw_sys_tick_get();
        for (i = 0;i < loop_num; i++) {
            if (aw_read(handle, str_buf, str_buf_size) != str_buf_size) {
                aw_close(handle);
                AW_ERRF(("* Read file Fail!\n"));
                aw_mem_free(str_buf);
                return -1;
            }
        }
        end_ticks  = aw_sys_tick_get();
        timeout_ms[j] = aw_ticks_to_ms(end_ticks - start_ticks);
        AW_INFOF(("* Read 1M data success,use time %d(ms)\r\n", timeout_ms[j]));

        /* �ر��ļ� */
        aw_close(handle);
    }

    /* ʱ��ȡƽ��ֵ */
    timeout_ms_min = get_average_min(timeout_ms, 10);
    timeout_ms_max = get_average_max(timeout_ms, 10);

    /* ʱ��ȡƽ��ֵ */
    for (j = 0; j < 10; j++) {
        average_timeout_ms += timeout_ms[j];
    }
    average_timeout_ms -= (timeout_ms_min + timeout_ms_max);
    average_timeout_ms /= 8;
    result = 1024*1000/(float)average_timeout_ms;

    /* ��ӡ�ٶ� */
    AW_INFOF(("* Read average speed : %.2f KB/s\r\n", result));

    aw_mem_free(str_buf);

    return AW_OK;
}

/**
 * \brief U���ļ�ϵͳ�������
 * \return ��
 * 
 */
int demo_udisk_fs_speedtest_entry (void)
{
    aw_err_t    ret,result;
    aw_bool_t   regist_ok   = AW_TRUE;
    int         event_index = 0;

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

    /* �����ļ�ϵͳ�����洢������Ϊ"vfat"���͵��ļ�ϵͳ */
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

    /* U�̲��ٲ��� */
    ret = __fs_speed_test();
    if (ret != AW_OK)
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

/* end of file */
