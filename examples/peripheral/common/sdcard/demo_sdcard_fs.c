/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief SDCARD��ʾ���̣��ļ�ϵͳ��
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_COM_SDCARD
 *      - ��Ӧƽ̨��SD�꣨�磺AW_DEV_xxx_USDHCx��
 *      - AW_COM_FS_FATFS
 *      - AW_COM_CONSOLE
 *   2. �������е�DURX��DUTXͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *   3. �ڿ�����SD�����в���SD��.
 *
 * - ʵ������
 *   1. ���ڴ�ӡ������Ϣ��
 *
 * \par Դ����
 * \snippet demo_sdcard_fs.c src_sdcard_fs
 *
 * \internal
 * \par Modification History
 * - 1.00 16-03-25  cod, first implementation.
 * - 1.01 17-08-31  xdn, modify some demo code.
 * \endinternal
 */

/**
 * \addtogroup demo_if_sdcard_fs SDCARD(�ļ�ϵͳ)
 * \copydoc demo_sdcard_fs.c
 */

/** [src_sdcard_fs] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "fs/aw_mount.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "io/sys/aw_stat.h"
#include "aw_sdcard.h"
#include "fs/aw_blk_dev.h"
#include "aw_demo_config.h"

#define __BLK_NAME      DE_SD_DEV_NAME

aw_local void __disk_evt_cb (const char *name, int event, void *p_arg)
{
    /* ͨ�� SDcard �Ŀ��豸�����ж��Ƿ�Ϊ SDcard ��� */
    if (strcmp(name, __BLK_NAME)) {
        return;
    }

    if (event == AW_BD_EVT_INJECT) {
        AW_INFOF(("sdcard insert...\r\n"));
    }

    if (event == AW_BD_EVT_EJECT) {
        AW_INFOF(("sdcard remove...\r\n"));
    }
}

aw_local void __fs_file_rw (void)
{
    int         i = 0;
    int         handle;
    char       *p_file_name = "/sd/aworks_sd_test.txt";
    uint8_t     str_buf[256] = {0};

    /*
     * д�ļ����ԣ������������ļ�д�������رղ�����
     */
    /* �������ļ� */
    handle = aw_open(p_file_name, O_RDWR | O_CREAT, 0777);
    if (handle < 0) {
        AW_ERRF(("creat file error: %d\n", handle));
        return;
    }
    AW_INFOF(("creat file %s ok\n", p_file_name));

    for (i = 0; i < sizeof(str_buf); i++) {
        str_buf[i] = (uint8_t)i;
    }

    /* д�ļ� */
    if (aw_write(handle, str_buf, sizeof(str_buf)) != sizeof(str_buf)) {
        aw_close(handle);
        AW_ERRF(("write file error\n"));
        return;
    }
    AW_INFOF(("write file %s ok\n", p_file_name));

    /* �ر��ļ� */
    aw_close(handle);
    AW_INFOF(("close file %s ok\n", p_file_name));

    /*
     * ���ļ����ԣ������򿪣��ļ����������رղ�����
     */
    /* ���ļ� */
    handle = aw_open(p_file_name, O_RDONLY, 0777);
    if (handle < 0) {
        AW_ERRF(("open file error: %d\n", handle));
        return;
    }
    AW_INFOF(("open file %s ok\n", p_file_name));

    memset(str_buf, 0, sizeof(str_buf));

    /* ��ȡ�ļ� */
    if (aw_read(handle, str_buf, sizeof(str_buf)) != sizeof(str_buf)) {
        aw_close(handle);
        AW_ERRF(("read file error!\n"));
        return;
    }
    AW_INFOF(("read file %s ok\n", p_file_name));

    /* ���������Ƿ���ȷ */
    for (i = 0; i < sizeof(str_buf); i++) {
        if ((uint8_t)i != str_buf[i]) {
            AW_ERRF(("file data error!\n"));
            aw_close(handle);
            return;
        }
    }

    AW_INFOF(("file %s data check ok\n", p_file_name));
}

/**
 * \brief ʹ���ļ�ϵͳ��дSD�� demo
 * \return ��
 *
 */
void demo_sdcard_fs_entry (void)
{
    aw_err_t    ret;
    aw_bool_t   regist_ok   = AW_TRUE;
    int         event_index = 0;

    /*
     * ��ӿ��豸����¼����, ����SDcard���ʱ, ��ص�__disk_evt_cb
     */
    ret = aw_blk_dev_event_register(__disk_evt_cb, NULL, &event_index);
    if (ret != AW_OK) {
        AW_ERRF(("block event register error: %d!\n", ret));
        regist_ok = AW_FALSE;
    }

    /* ���SD�����豸�Ƿ���� */
    ret = aw_blk_dev_wait_install(__BLK_NAME, 10000);
    if (ret != AW_OK) {
        AW_ERRF(("wait sdcard insert failed, err = %d\r\n", ret));
        goto __sd_fs_end;
    }


#if 0  /* ��ʽ��һ�μ���  */

    /* ����Ϊ"awdisk"�� ���СΪ4k  */
    struct aw_fs_format_arg fmt = {"awdisk", 1024 * 4, 0};
    /* �����ļ�ϵͳ �����洢����Ϊ "/dev/sd0"����Ϊ"vfat"���͵��ļ�ϵͳ  */
    ret = aw_make_fs(__BLK_NAME, "vfat", &fmt);
    if (ret != AW_OK) {
        AW_ERRF(("failed: %d\n", ret));
        goto __sd_fs_end;
    }
    AW_INFOF(("make fs OK\n"));

#endif

    /* �ļ�ϵͳ���ص�"/sd"���  */
    ret = aw_mount("/sd", __BLK_NAME, "vfat", 0);
    if (ret != AW_OK) {
        AW_ERRF(("/sd mount FATFS error: %d!\n", ret));
        goto __sd_fs_end;
    }
    AW_INFOF(("mount OK\n"));

    /* SD ����д����  */
    __fs_file_rw();

__sd_fs_end:
    /* ע���¼� */
    if (regist_ok) {
        ret = aw_blk_dev_event_unregister(event_index);
        if (ret != AW_OK) {
            AW_ERRF(("block event unregister error: %d!\n", ret));
        }
    }

    return ;
}

/** [src_sdcard_fs] */

/* end of file */
