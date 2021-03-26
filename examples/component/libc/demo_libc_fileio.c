/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief ��׼C�⺯��ʹ�����̣��ļ���д��
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_COM_CONSOLE
 *      - AW_COM_FS_FATFS
 *      �Լ��͵��Դ��ڶ�Ӧ�ĺ��ʹ��SD���ĺ�
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ�������أ�
 *   3. ���Ӳ���SD����
 *
 * - ʵ������
 *   1. ���ڴ�ӡ�������ļ���д�ļ����ر��ļ������ļ������ļ����ļ��������Ȳ�����Ϣ��
 *
 *
 * \par Դ����
 * \snippet demo_libc_fileio.c src_libc_fileio
 *
 * \internal
 * \par Modification history
 * - 1.00 17-09-07  mkr, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_libc_fileio �ļ�ϵͳ(��׼C��)
 * \copydoc demo_libc_fileio.c
 */

/** [src_libc_fileio] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "fs/aw_mount.h"
#include "fs/aw_blk_dev.h"
#include <stdio.h>
#include <string.h>

/******************************************************************************/
#define LIBC_IO_FS          "vfat"     /**< \brief ʹ��FAT�ļ�ϵͳ */
#define LIBC_IO_STORER      "/dev/sd0" /**< \brief �洢������ */
/******************************************************************************/
/**
 * \brief �ļ�ϵͳ��ʼ��
 *
 * \return ��
 */
aw_local aw_err_t __fs_init (void)
{
    aw_err_t                ret    = AW_OK;

#if 0
    uint8_t                 agains = 0;
    struct aw_fs_format_arg fmt    = {"awdisk", 4096, 1};

    /* ���ļ�ϵͳ��ʽ���洢�� */
    while ((ret = aw_make_fs(LIBC_IO_STORER, LIBC_IO_FS, &fmt)) != AW_OK) {
        if (-AW_ENODEV == ret) {
            agains++;
            aw_mdelay(1000);
        }
        if (agains > 5) {
            AW_ERRF(("failed: %d\n", ret));
            return ret;
        }
    }
    AW_INFOF(("make %s use %s OK\n", LIBC_IO_STORER, LIBC_IO_FS));
#endif

    /* �ļ�ϵͳ���� */
    ret = aw_mount("/export", LIBC_IO_STORER, LIBC_IO_FS, 0);
    if (ret != AW_OK) {
        AW_ERRF(("/export mount /dev/mtd0 error: %d!\n", ret));
        return ret;
    }
    AW_INFOF(("mount OK\n"));

    return ret;
}

/**
 * \brief libc io ��ں���
 * \return ��
 */
void demo_libc_io_entry (void)
{
    char    *p_filename  = "/export/libc_test.txt";
    char    *p_newname   = "/export/libc_new.txt";
    char    *p_str       = "Demo libc file io testing.";
    uint8_t  wr_buf[256] = {0};
    uint8_t  rd_buf[256] = {0};
    uint16_t i;
    uint16_t len;
    FILE    *fp          = NULL;
    size_t   size;
    aw_err_t ret = 0;

    AW_INFOF(("\r\nLibc IO testing...\n"));

    /* ���SD�����豸�Ƿ���� */
    while (aw_blk_dev_wait_install(LIBC_IO_STORER, 10000) != AW_OK) {
        AW_ERRF(("wait sdcard insert failed, err = %d\r\n", ret));
        aw_mdelay(1000);
    }

    ret = __fs_init(); /* �ļ�ϵͳ��ʼ�� */
    if (AW_OK != ret) {
        AW_ERRF(("fs init failed!\n"));
        return ;
    }

    /* �Զ�д��ʽ�����ļ� */
    fp = fopen(p_filename, "w+");
    if (NULL == fp) {
        AW_ERRF(("fopen(): fopen error!\n"));
        return ;
    } else {
        AW_INFOF(("fopen(): fopen success - creating new file : %s\n", p_filename));
    }

    for (i = 0; i < 256; i++) {
        wr_buf[i] = i;
    }

    /* д��256�ֽ����� */
    size = fwrite(wr_buf, 1, 256, fp);
    if (256 != size) {
        AW_ERRF(("fwrite(): fwrite error!\n"));
    } else {
        AW_INFOF(("fwrite(): fwrite success - write 256 bytes data!\n"));
    }

    /* �ر��ļ� */
    if (0 != fclose(fp)) {
        AW_ERRF(("\nfclose(): flose error!\n"));
    } else {
        AW_INFOF(("\nfclose(): fclose success - close file!\n"));
    }
    fp = NULL;

    /* �ļ����������� */
    ret = rename(p_filename, p_newname);
    if (0 != ret) {
        AW_ERRF(("rename(): rename error!\n"));
    } else {
        AW_INFOF(("rename(): rename success - rename file %s to %s\n",
                  p_filename,
                  p_newname));
    }

    /* ��ֻ����ʽ���ļ� */
    fp = fopen(p_newname, "r");
    if (NULL == fp) {
        AW_ERRF(("fopen(): fopen error!\n"));
        return ;
    } else {
        AW_INFOF(("fopen(): repeat open file - using new filename!\n"));
    }

    /* �����ļ�ƫ��ֵΪ����ʼλ��200�ĵ�ַλ�� */
    ret = fseek(fp, 200, SEEK_SET);
    if (0 != ret) {
        AW_ERRF(("fseek(): fseek error!\n"));
    } else {
        AW_INFOF(("fseek(): setting file pointer to 200!\n"));
    }

    /* ��ȡ�ļ�56������ */
    size = fread(&rd_buf[200], 1, 56, fp);
    if (56 != size) {
        AW_ERRF(("fread(): read error!\n"));
    } else {
        AW_INFOF(("fread(): read 56 bytes data!\n"));
    }

    /* ���������Ч�� */
    for (i = 200; i < 256; i++) {
        if (wr_buf[i] != rd_buf[i]) {
            AW_ERRF(("reading data no validate!\n"));
            break;
        }
    }

    /* �ر��ļ� */
    if (0 != fclose(fp)) {
        AW_ERRF(("\nfclose(): flose error!\n"));
    } else {
        AW_INFOF(("\nfclose(): repeat close file!\n"));
    }
    fp = NULL;

    /* �Ը��ӷ�ʽ���ļ� */
    fp = fopen(p_newname, "a+");
    if (NULL == fp) {
        AW_ERRF(("fopen(): fopen error!\n"));
        return ;
    } else {
        AW_INFOF(("fopen(): repeat open file!\n"));
    }

    /* дһ���ַ������ļ�ĩβ */
    len  = strlen(p_str);
    fputs(p_str, fp);
    AW_INFOF(("fputs(): write a string to the end of file!\n%s\n", p_str));

    /*���ļ�ָ��ָ���ļ���ʼ*/
    rewind(fp);

    ret = fseek(fp, 256, SEEK_SET);

    /* ���ļ��ж�ȡ�ո�д����ַ��� */
    fgets((char *)rd_buf, len + 1, fp);
    if (0 != strcmp((const char *)rd_buf, p_str)) {
        AW_ERRF(("fgets(): fgets error!\n"));
    } else {
        AW_INFOF(("fgets(): read the string that just recently wrote!\n"
                  "the string is valid by checking!\n"));
    }

    /* �ر��ļ� */
    if (0 != fclose(fp)) {
        AW_ERRF(("flose error!\n"));
    } else {
        AW_INFOF(("flose(): close file!\n"));
    }
    fp = NULL;

    return ;
}

/** [src_libc_fileio] */

/* end of file */
