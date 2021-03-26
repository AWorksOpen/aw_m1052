/*******************************************************************************
*                                 AWorks
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
 * \brief ��ftl�豸ע��ɿ��豸����fat�ļ�ϵͳ������
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - NANDFLASH�����
 *      - AW_COM_FTL
 *      - AW_COM_FS_FATFS
 *      - AW_COM_CONSOLE
 *   2. �������е�DURX��DUTXͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������:
 *   1. ���Ȼᴴ��һ��FTL�豸������FatFs��ʽ��
 *   2. ͨ���ļ�ϵͳ�ӿڶ�д
 *
 * \par Դ����
 * \snippet demo_ftl_fs.c src_ftl_fs
 */

/**
 * \addtogroup demo_if_ftl_fs FTL(�ļ�ϵͳ)
 * \copydoc demo_ftl_fs.c
 */

/** [src_ftl_fs] */

#include "aworks.h"
#include "aw_vdebug.h"
#include "string.h"
#include "fs/aw_mount.h"
#include "io/sys/aw_stat.h"
#include "aw_ftl.h"
#include "aw_delay.h"
#include "io/aw_unistd.h"
#include "io/aw_fcntl.h"

/* mtd��������nandflash��Ӳ������(awbl_hwconf_xxx_nandflash.h)����� */
#define __MTD_NAME  "/dev/mtd1"

/**
 * \brief �ļ���д����
 * \return ��
 */
static void __fs_file_rw (char *p_mnt)
{
    int             i = 0;
    int             handle;
    static uint8_t  str_buf[256]    = {0};
    static char     p_file_name[64] = {0};

    strncpy(p_file_name, p_mnt, sizeof(p_file_name));
    strncat(p_file_name, "/aworks_sd_test.txt", strlen("/aworks_sd_test.txt"));

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
    if (aw_close(handle)) {
        AW_INFOF(("close file %s ok\n", p_file_name));
        return;
    }

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
    aw_close(handle);
    AW_INFOF(("file %s data check ok\n", p_file_name));
}

/**
 * \brief �������
 * \param[in] p_arg : �������
 * \return ��
 */
void demo_ftl_fs_entry (void)
{
    int    err;

    /*
     * ����һ��ftl�豸
     * note: mtd��������nandflash��Ӳ������(awbl_hwconf_xxx_nandflash.h)�����
     */
    err = aw_ftl_dev_create ("/dev/ftl", __MTD_NAME);
    if (err != AW_OK) {
        AW_ERRF(("aw_ftl_dev_create err: %d\n", err));
        goto __task_entry_end;
    }

    /* �����豸ע��ɿ��豸  */
    err = aw_ftl_blk_register("/dev/ftl");
    if (err != AW_OK) {
        AW_ERRF(("aw_ftl_blk_register err: %d\n", err));
        goto __task_entry_end;
    }

    /* ��ʽ���洢������ʽ��һ�ξ͹��ˣ�����ʽ���������ļ���ɾ�������Խ�������  */
#if 1
    {
        /* ����Ϊ"awdisk"�� ���СΪ2K��
         *
         */
        struct aw_fs_format_arg fmt = {"awdisk", 2048, 0};

        /* �� fat �ļ�ϵͳ��ʽ���ÿ��豸 */
        if ((err = aw_make_fs("/dev/ftl", "vfat", &fmt)) != AW_OK) {
            AW_ERRF(("make %s use %s failed: %d\n", "/dev/ftl", "vfat", err));
            goto  __task_entry_end;
        }
        AW_INFOF(("make %s use %s OK\n", "/dev/ftl", "vfat"));
    }
#endif

    /* �� fat ���ظÿ��豸 */
    if ((err = aw_mount("/fat_mnt", "/dev/ftl", "vfat", 0)) != AW_OK) {
        AW_ERRF(("aw_mount err: %d\n", err));
        goto __task_entry_end;
    } else {
        AW_INFOF(("mount OK\n"));
    }

    /*
     * �����ļ���д����
     */
    __fs_file_rw("/fat_mnt");

    /*
     * ж�ظù��ص�
     */
    if ((err = aw_umount("/fat_mnt", 0)) == AW_OK) {
        AW_INFOF(("umount OK\n"));
    }

    /* ע�� ftl �Ŀ��豸 */
    aw_ftl_blk_unregister("/dev/ftl");

__task_entry_end:
    for (;;) {
        aw_mdelay(1000);
    }
}

/** [src_ftl_fs] */

/* end of file */
