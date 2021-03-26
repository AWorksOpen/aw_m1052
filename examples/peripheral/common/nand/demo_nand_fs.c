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
 * \brief nandflash ʹ���ļ�ϵͳ��д��ʾ����
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - NANDFLASH�豸��
 *      - AW_COM_FS_LFFS
 *      - AW_COM_CONSOLE
 *      - AW_COM_SHELL_SERIAL
 *   2. �������е�DURX��DUTXͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������
 *   1. ����ʹ���ļ�ϵͳ��ʽ�� nand
 *   2. �� nand �������ļ�ϵͳ��
 *   3. ͨ���ļ�ϵͳ�ӿ�ȥ��д nand
 *   4. ͨ��"ls"����,���Կ���mntĿ¼��ͨ��"cd mnt"�����л���/mnt·���£�"ls"�鿴�½����ļ�"aworks_nand_test.txt"
 *
 * \par Դ����
 * \snippet demo_nand_fs.c src_demo_nand_fs
 *
 * \internal
 * \par Modification History
 * - 1.00 17-09-05  vih, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_nand_fs nandflash(�ļ�ϵͳ)
 * \copydoc demo_nand_fs.c
 */

/** [src_demo_nand_fs] */

#include "aworks.h"
#include "aw_vdebug.h"
#include "fs/aw_mount.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "io/sys/aw_stat.h"
#include "aw_delay.h"
#include "aw_demo_config.h"
#include <string.h>

#define __FS_NAME        DE_NAMD_FS_NAME
#define __DISK_NAME      DE_DISK_NAME

/**
 * \brief �ļ���д����
 * \return ��
 */
static void __fs_file_rw (char  *p_mnt)
{
    int         i = 0;
    int         handle;
    uint8_t     str_buf[256] = {0};
    char        p_file_name[64] = {0};

    strncpy(p_file_name, p_mnt, sizeof(p_file_name));
    strncat(p_file_name, "/aworks_nand_test.txt", strlen("/aworks_nand_test.txt"));

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

    /* �ر��ļ� */
    aw_close(handle);
    AW_INFOF(("file %s data check ok\n", p_file_name));
}

/**
 * \brief nand ʹ�� fs ���� demo
 * \return ��
 */
void demo_nand_fs_entry (void)
{
    struct aw_fs_format_arg fmt = {"awdisk", 512, 0};
    int     err;

    /*
     * ��ָ����mtd�豸�ϸ�ʽ�� lffs �ļ�ϵͳ��
     * note: mtd������(DE_DISK_NAME)�� nandflash ��Ӳ������(awbl_hwconf_xxxxxx_nandflash.h)
     *       �����
     * note: ��nandû�б���ʽ������ô��Ҫ��ʽ��������ʽ�������Ͳ���Ҫ�˲���
     */
    if ((err = aw_make_fs(__DISK_NAME, __FS_NAME, &fmt)) != AW_OK) {
        AW_INFOF(("failed: %d\n", err));
    } else {
        AW_INFOF(("OK\n"));
    }

    /*
     * ��ָ����mtd�豸���ص�/mnt���ļ�ϵͳ����Ϊlffs��
     * ���سɹ�֮��ſ��Խ����������ļ�ϵͳ��д��
     */
    if ((err = aw_mount("/mnt", __DISK_NAME, __FS_NAME, AW_OK)) == AW_OK) {
        AW_INFOF(("OK\n"));
    }

    /*
     * �����ļ���д����
     */
    __fs_file_rw("/mnt");

#if 0
    /*ж���ļ�ϵͳ*/
    if ((err = aw_umount("/mnt",AW_OK)) == AW_OK) {
        AW_INFOF(("OK\n"));
    }
#endif

}

/** [src_demo_nand_fs] */

/* end of file */
