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
 * \brief lua ��ʾ���̣�SD����
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *    - AW_COM_SDCARD�豸
 *    - ��Ӧƽ̨��SD����
 *    - AW_COM_FS_FATFS
 *    - AW_COM_SHELL_LUA
 *   2. �������е�DURX��DUTXͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *   3. ׼��һ��SD�������뿪�����SD������
 *   4. ��shell������ִ��"lua /sd/test.lua"���
 *
 * - ʵ������
 *   1. ִ��shell����󣬴��ڴ�ӡHello world!
 *
 * - ��ע��
 *   ��ʱֻ֧��eclipse�汾����ΪluaԴ������g++����ѡ�����ǰ�汾armcc��������g++������
 *   Ĭ�ϲ���ʽ��SD���������ʽ��SD�����뽫���ζδ����
 *
 * \par Դ����
 * \snippet demo_lua.c src_demo_lua
 *
 * \internal
 * \par Modification History
 * - 1.00 19-04-25  vih, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_lua
 * \copydoc demo_lua.c
 */

/** [src_demo_lua] */

#include "aworks.h"
#include "aw_vdebug.h"
#include "fs/aw_mount.h"
#include "aw_delay.h"
#include <string.h>
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "aw_demo_config.h"
#include "fs/aw_fs_type.h"
#include "fs/aw_blk_dev.h"

#define __MOUNT_NAME        "/sd"
#define __FAT_NAME          "vfat"
#define __DISK_NAME         DE_SD_DEV_NAME

#define __LUA               "lua"
#define __FILE_NAME         "/sd/test.lua"

#define __MAKE_FS           1

/**
 * \brief ����lua�ű�  demo
 * \return ��
 */
void demo_lua_entry (void)
{
#if __MAKE_FS
    struct aw_fs_format_arg fmt = {"awdisk", 512, 0};
#endif
    int err;


    /* ���SD�����豸�Ƿ���� */
    while (aw_blk_dev_wait_install(__DISK_NAME, 10000) != AW_OK) {
        AW_ERRF(("sdcard is not exist\r\n"));
        aw_mdelay(1000);
    }

    /*
     * ��ָ�����豸�ϸ�ʽ��  vfat�ļ�ϵͳ��
     * note: ��sd��û�б���ʽ������ô��Ҫ��ʽ��������ʽ�������Ͳ���Ҫ�˲���
     */
#if __MAKE_FS
    /* �����ļ�ϵͳ �����洢����Ϊ "/dev/sd0"����Ϊ"vfat"���͵��ļ�ϵͳ  */
    if ((err = aw_make_fs(__DISK_NAME, __FAT_NAME, &fmt)) != AW_OK) {
        AW_INFOF(("failed: %d\n", err));
    } else {
        AW_INFOF(("OK\n"));
    }
#endif

    /*
     * ��ָ����sd���豸���ص�/sd���ļ�ϵͳ����Ϊvfat��
     * ���سɹ�֮��ſ��Խ����������ļ�ϵͳ��д��
     */
    int fd;
    if ((err = aw_mount(__MOUNT_NAME, __DISK_NAME, __FAT_NAME, AW_OK)) == AW_OK) {

        AW_INFOF(("mount OK\n"));

        fd = aw_open (__FILE_NAME,O_RDWR | O_CREAT, 0777);
        if (fd > 0) {
            const char * strbuf = "print(\"Hello world!\")";
            ssize_t len = aw_write (fd, strbuf, strlen(strbuf));
            if (len == strlen(strbuf)) {
                aw_kprintf("write ok.\r\n");
            }
            aw_close(fd);
        }
    }

    return ;
}

/** [src_demo_lua] */

/* end of file */
