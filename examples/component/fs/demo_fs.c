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
 * \brief AWorks �ļ�ϵͳ����
 *
 * - ������չʾ�� AWorks �ļ�ϵͳ�д�����ӿڲ�������ʹ��FAT�ļ�ϵͳ����SD��Ϊ����
 *
 * - �������裺
 *   1. ��Ҫ��aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_COM_FS_FATFS
 *      - AW_COM_CONSOLE
 *      �Լ����Դ��ڶ�Ӧ�ĺ��SD���ĺ�
 *   2. ���Ӳ���SD��
 *   3. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������ ��ӡ�������ļ���д�ļ����ر��ļ������ļ������ļ����ļ��������Ȳ�����Ϣ��
 *
 *
 * \par Դ����
 * \snippet demo_fs.c src_fs
 *
 * \internal
 * \par Modification History
 * - 1.00 17-09-06  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_fs �ļ�ϵͳ
 * \copydoc demo_fs.c
 */

/** [src_fs] */
#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_time.h"
#include "fs/aw_mount.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "io/aw_dirent.h"
#include "io/sys/aw_stat.h"
#include "aw_delay.h"
#include "fs/aw_blk_dev.h"
#include <string.h>


/*
 * ʹ���ļ�ϵͳǰע�⣺
 *
 * 1. ����洢��֮ǰû�б���ʽ������Ҫ���и�ʽ������ʽ��һ�ξ͹��ˣ�
 *
 * 2. ʹ���ļ�ϵͳ�ӿ�(��д�ļ����ļ��е�)ǰ�������aw_mount��������
 *
 * 3. ����洢����
 *    1) �� SPI FLASH Ϊ������mtd�豸������aw_hwconf_spi_flash0.h�ļ�����ָ����Ϊ"/flexspi_flash0",
 *       �ļ�ϵͳ����ʱ����Ҫ�������
 *    2) �� SD ��Ϊ��������豸����Ϊ"/dev/sd0"
 *    3) �� NAND FLASH Ϊ������mtd�豸�����б���awbl_hwconf_xxx_nandflash.h��ָ�����������������������
 *    ����Ҫ��aw_prj_param.h��ʹ����Ӧ������洢��
 *
 * 4. �ļ�ͬʱ�򿪸������ã���aw_prj_params.h�ļ��У����ú꣺AW_CFG_FATFS_FILES �� AW_CFG_IO_OPEN_FILES_MAX
 *       ����AW_CFG_IO_OPEN_FILES_MAX >= AW_CFG_FATFS_FILES
 *
 * 5. �ļ�ϵͳ�ӿ�������POSIX��׼���ӿڶ��ձ����£�
 *
 * ��/����   aw_open
 *
 * �ر�     aw_close
 *
 * д�ļ�   aw_write
 *
 * ���ļ�   aw_read
 *
 * ������   aw_rename
 *
 * �ı��ļ�ָ��   aw_lseek
 *
 * ��ȡ�ļ�����   aw_stat
 *
 * Ŀ¼����   aw_mkdir
 *
 * ɾ��Ŀ¼   aw_rmdir
 *
 * �����ļ����   aw_opendir, aw_readdir
 *
 * ɾ���ļ�   aw_unlink
 *
 * �ļ��ض�   aw_truncate
 *
 */


/* ��FAT�У������ò�֧�ֳ��ļ���ʱ�����е��ļ������Ǵ�д��������չ������
 * �����ļ����������8.3ԭ�򣬼�8���ļ����ַ���3����չ���ַ�
 *
 * Ŀǰ�����ļ�ϵͳ "vfat", "lffs", "txfs", ������ʹ��vfat����SD��
 */
#define  TEST_FS_NAME           "vfat"
#define  TEST_STORER_NAME       "/dev/sd0"
#define  TEST_MOUNT_POINT       "/sda"
#define  TEST_FILE_NAME         "aworks.txt"
#define  TEST_FILE_NEW_NAME     "zlg.txt"
#define  TEST_DIR_NAME          "dir"


/**
 * \brief ��ʽ������
 *
 * \return -1  ����
 *          0  �ɹ�
 */
aw_local int __fs_format (void)
{
    aw_err_t ret;

    /* ����Ϊ"awdisk"�� ���СΪ4k  */
    struct aw_fs_format_arg fmt = {"awdisk", 1024 * 4, 0};

    /* �����ļ�ϵͳ �����洢����Ϊ TEST_STORER_NAME ����Ϊ TEST_FS_NAME ���͵��ļ�ϵͳ  */
    ret = aw_make_fs(TEST_STORER_NAME, TEST_FS_NAME, &fmt);
    if (ret != AW_OK) {
        aw_kprintf("make %s by %s failed %d\n", TEST_STORER_NAME, TEST_FS_NAME, ret);
        return -1;
    }
    aw_kprintf("make %s by %s OK\n", TEST_STORER_NAME, TEST_FS_NAME);
    return 0;
}

 /**
  * \brief ���ش���
  *
  * \return -1  ����
  *          0  �ɹ�
  */
aw_local int __fs_mount (void)
{
    aw_err_t ret;

    /* �ļ�ϵͳ���� */
    ret = aw_mount(TEST_MOUNT_POINT, TEST_STORER_NAME, TEST_FS_NAME, 0);
    if (ret != AW_OK) {
        aw_kprintf("mount %s to %s failed %d!\n", TEST_STORER_NAME, TEST_MOUNT_POINT, ret);
        return -1;
    }
    aw_kprintf("mount OK.\n");
    return 0;
}

/**
 * \brief д����
 *
 * \return -1  ����
 *          0  �ɹ�
 */
aw_local int __file_write (void)
{
    int        fd;
    const char str[] = "this is aworks file test.";
    char       path[255];
    int        len, ret;

    /* �����ļ�����·�� */
    sprintf(path, "%s/%s", TEST_MOUNT_POINT, TEST_FILE_NAME);

     /* �������ļ� */
    fd = aw_open(path, O_RDWR | O_CREAT, 0777);
    if (fd < 0) {
        aw_kprintf("create file failed %d\n", fd);
        return -1;
    }
    aw_kprintf("create file %s ok.\n", TEST_FILE_NAME);

    len = strlen(str);

    /* д������ */
    ret = aw_write(fd, str, len);
    if (ret != len) {
        aw_kprintf("write file %s failed.\n", TEST_FILE_NAME);
        ret = -1;
    } else {
        aw_kprintf("write file %s ok.\n", TEST_FILE_NAME);
        ret = 0;
    }

    /* �ر��ļ����������� */
    aw_close(fd);

    return ret;
}

 /**
  * \brief ������
  *
  * \return -1  ����
  *          0  �ɹ�
  */
 aw_local int __file_read (void)
 {
     int        fd;
     char       buf[32];
     char       path[255];
     int        ret;

     /* �����ļ�����·�� */
     sprintf(path, "%s/%s", TEST_MOUNT_POINT, TEST_FILE_NAME);

      /* ���ļ� */
     fd = aw_open(path, O_RDWR, 0777);
     if (fd < 0) {
         aw_kprintf("open file failed %d\n", fd);
         return -1;
     }
     aw_kprintf("open file %s ok.\n", TEST_FILE_NAME);

     /* ��ȡ���� */
     ret = aw_read(fd, buf, sizeof(buf));
     if (ret < 0) {
         aw_kprintf("read file %s failed.\n", TEST_FILE_NAME);
         ret = -1;
     } else {
         buf[ret] = '\0';
         aw_kprintf("read file %s ok.\n", TEST_FILE_NAME);
         aw_kprintf("read data: %s\n", buf);
         ret = 0;
     }

     /* �ر��ļ����������� */
     aw_close(fd);

     return ret;
 }

 /**
  * \brief ������
  *
  * \return -1  ����
  *          0  �ɹ�
  */
 aw_local int __file_rename (void)
 {
     char       old[255], new[255];
     int        ret;

     /* �����ļ�����·�� */
     sprintf(old, "%s/%s", TEST_MOUNT_POINT, TEST_FILE_NAME);
     sprintf(new, "%s/%s", TEST_MOUNT_POINT, TEST_FILE_NEW_NAME);

     ret = aw_rename(old, new);
     if (ret != AW_OK) {
         aw_kprintf("%s rename to %s failed.\n", TEST_FILE_NAME, TEST_FILE_NEW_NAME);
         return -1;
     }

     aw_kprintf("%s rename to %s ok.\n", TEST_FILE_NAME, TEST_FILE_NEW_NAME);
     return 0;
 }

/**
* \brief ��ȡ�ļ�����
*
* \return -1  ����
*          0  �ɹ�
*/
aw_local int __file_stat (void)
{
    struct aw_stat  sta;
    char            buf[255];
    int             ret;
    struct aw_tm    tm;

    /* �����ļ�����·�� */
    sprintf(buf, "%s/%s", TEST_MOUNT_POINT, TEST_FILE_NAME);

    ret = aw_stat(buf, &sta);
    if (ret != AW_OK) {
        aw_kprintf("get %s stat failed.\n", TEST_FILE_NAME);
        return -1;
    }

    /* ��ȡ�ļ�����ʱ�� */
    aw_time_to_tm(&sta.st_mtim.tv_sec, &tm);
    if (difftime(aw_time(NULL), sta.st_mtim.tv_sec) > (365 * 24 * 60 * 60 / 2 /* 6 months */)) {
        strftime(buf, sizeof(buf), "%b %d  %Y", (struct tm*)&tm);
    } else {
        strftime(buf, sizeof(buf), "%b %d  %H:%M", (struct tm*)&tm);
    }

    /* ����ļ���Ϣ */
    aw_kprintf("file %s stat:\n", TEST_FILE_NAME);

    aw_kprintf("%c%c%c%c%c%c%c%c%c%c %2d user AWorks %9u %s %s\n",
               (S_ISLNK(sta.st_mode) ? ('l') : (S_ISDIR(sta.st_mode) ? ('d') : ('-'))),
               (sta.st_mode & S_IRUSR) ? ('r') : ('-'),
               (sta.st_mode & S_IWUSR) ? ('w') : ('-'),
               (sta.st_mode & S_IXUSR) ? ('x') : ('-'),
               (sta.st_mode & S_IRGRP) ? ('r') : ('-'),
               (sta.st_mode & S_IWGRP) ? ('w') : ('-'),
               (sta.st_mode & S_IXGRP) ? ('x') : ('-'),
               (sta.st_mode & S_IROTH) ? ('r') : ('-'),
               (sta.st_mode & S_IWOTH) ? ('w') : ('-'),
               (sta.st_mode & S_IXOTH) ? ('x') : ('-'),
               sta.st_nlink,
               (int)sta.st_size,
               buf,
               TEST_FILE_NAME);

    return 0;
}

/**
* \brief �ļ�ָ������
*
* \return -1  ����
*          0  �ɹ�
*/
aw_local int __file_seek (void)
{
    int        fd = -1;
    char       buf[32];
    char       path[255];
    off_t      offset;
    int        ret;

    /* �����ļ�����·�� */
    sprintf(path, "%s/%s", TEST_MOUNT_POINT, TEST_FILE_NAME);

     /* ���ļ� */
    fd = aw_open(path, O_RDWR, 0777);
    if (fd < 0) {
        goto __failed;
    }
    aw_kprintf("open file %s ok.\n", TEST_FILE_NAME);

    /* �����ļ�ָ�뵽ƫ��λ��5 */
    offset = aw_lseek(fd, 5, SEEK_SET);
    if (offset < 0) {
        goto __failed;
    }
    aw_kprintf("file data seek ok.\n", TEST_FILE_NAME);

    /* ��ȡ���� */
    ret = aw_read(fd, buf, sizeof(buf));
    if (ret < 0) {
        goto __failed;
    } else {
        buf[ret] = '\0';
        aw_kprintf("read data: %s\n", buf);
        ret = 0;
    }

    /* �ر��ļ����������� */
    aw_close(fd);
    return 0;

__failed:
    if (fd >= 0) {
        aw_close(fd);
    }
    aw_kprintf("file data seek failed.\n");
    return -1;
}

/**
* \brief �ļ�ɾ��
*
* \return -1  ����
*          0  �ɹ�
*/
aw_local int __file_delete (void)
{
    char       path[255];
    int        ret;

    /* �����ļ�����·�� */
    sprintf(path, "%s/%s", TEST_MOUNT_POINT, TEST_FILE_NEW_NAME);

    ret = aw_unlink(path);
    if (ret != AW_OK) {
        aw_kprintf("file delete failed.\n");
        return -1;
    }

    aw_kprintf("file delete ok.\n");
    return 0;
}

/**
* \brief �½��ļ���
*
* \return -1  ����
*          0  �ɹ�
*/
aw_local int __dir_create (void)
{
    char       path[255];
    int        ret;

    /* �����ļ�����·�� */
    sprintf(path, "%s/%s", TEST_MOUNT_POINT, TEST_DIR_NAME);

    ret = aw_mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);
    if (ret != AW_OK) {
        aw_kprintf("directory create failed.\n");
        return -1;
    }

    aw_kprintf("directory create ok.\n");
    return 0;
}

/**
* \brief ɾ���ļ���
*
* \return -1  ����
*          0  �ɹ�
*/
aw_local int __dir_delete (void)
{
    char       path[255];
    int        ret;

    /* �����ļ�����·�� */
    sprintf(path, "%s/%s", TEST_MOUNT_POINT, TEST_DIR_NAME);

    ret = aw_rmdir(path);
    if (ret != AW_OK) {
        aw_kprintf("directory delete failed.\n");
        return -1;
    }

    aw_kprintf("directory delete ok.\n");
    return 0;
}

/**
* \brief ��ѯ�ļ���
*
* \return -1  ����
*          0  �ɹ�
*/
aw_local int __dir_list (void)
{
    char                path[255];
    struct aw_dir      *p_dir;
    struct aw_dirent   *p_entry;

    /* �����ļ�����·�� */
    sprintf(path, "%s/%s", TEST_MOUNT_POINT, TEST_DIR_NAME);

    /* ��Ŀ¼*/
    p_dir = aw_opendir(path);
    if (p_dir == NULL) {
        aw_kprintf("open directory %s failed.\n", TEST_DIR_NAME);
        return -1;
    }

    /* �г������ļ� */
    while((p_entry = aw_readdir(p_dir)) != NULL) {
        aw_kprintf("%s\n", p_entry->d_name);
    }

    /* �ر�Ŀ¼ */
    aw_closedir(p_dir);

    return 0;
}

/**
 * \brief �ļ�ϵͳ demo���
 *
 * \param[in]  format   ��ʽ��ѡ���0����и�ʽ��������
 *
 * \return ��
 */
void  demo_fs_entry (aw_bool_t format)
{
    aw_kprintf("\n\n\n**********************************************\n");
    aw_kprintf(" test file system function:\n");

    /* �ȴ�ϵͳʶ�𵽴洢�豸 */
    if (aw_blk_dev_wait_install(TEST_STORER_NAME, 3000)) {
        aw_kprintf("%s is not exist.\r\n", TEST_STORER_NAME);
        return;
    }

    /* ��ʽ���洢������������ */
    if (format) {
        if (__fs_format() == -1) {
            return;
        }
    }

    /* ���ش��̣�����PC�Ͻ�U�̱��һ��G�̡�I�̵�һ�� */
    if (__fs_mount() == -1) {
        return;
    }

    /* д������ */
    if (__file_write() == -1) {
        goto __exit;
    }

    /* ��ȡ���� */
    if (__file_read() == -1) {
        goto __exit;
    }

    /* �����ļ����� */
    if (__file_seek() == -1) {
        goto __exit;
    }

    /* ��ȡ�ļ����� */
    if (__file_stat() == -1) {
        goto __exit;
    }

    /* ������ */
    if (__file_rename() == -1) {
        goto __exit;
    }

    /* ɾ���ļ� */
    if (__file_delete() == -1) {
        goto __exit;
    }


    /* �½��ļ��� */
    if (__dir_create() == -1) {
        goto __exit;
    }

    /* ��ѯ�ļ��� */
    if (__dir_list() == -1) {
        goto __exit;
    }

    /* ɾ���ļ��� */
    if (__dir_delete() == -1) {
        goto __exit;
    }

__exit:
    aw_umount(TEST_MOUNT_POINT, 0);
}

/** [src_fs] */

/* end of file */
