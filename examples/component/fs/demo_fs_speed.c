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
 * \brief ͨ��shell��������ļ�ϵͳ��д�ٶ�����
 *
 * - �������裺
 *   1. ��Ҫ��aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_COM_SHELL_SERIAL
 *      - AW_COM_CONSOLE
 *      - AW_COM_SDCARD
 *      - AW_COM_FS_FATFS
 *      �Լ����Դ��ڶ�Ӧ�ĺ��SD���ĺ�
 *   �ȣ�
 *
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 *   3. ���г��������Ӳ�����SD��������ͨ��"mount /sd /dev/sd0 vfat"�������sd����
 *      Ȼ�����ͨ��������ϵͳ��������"fspeed /sd/12 1234567"�����ļ�ϵͳ�Ķ�д�ٶȡ�
 *
 * - ʵ������
 *   1. ��������°��Ӵ��ڻ��ӡ������Ϣ����д�ٶ�Ҫ������SD�ͺŵȣ���
 *      - "/sd/12": 1234567B(1205KB) read and write speed test
 *      - Write file speed: tick 590, 2092000 B/s (2042 KB/s)
 *      - Read file speed: tick 285, 4331000 B/s (4229 KB/s)
 *      - Verify data ok.
 *
 * - ��ע��
 *   1. �����ļ�ϵͳ��洢��Ҳͬ������;
 *
 * \par Դ����
 * \snippet demo_fs_speed.c src_fs_speed
 *
 * \internal
 * \par Modification History
 * - 1.00 17-09-14  may, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_fs_speed �ļ�ϵͳ(����)
 * \copydoc demo_fs_speed.c
 */

/** [src_fs_speed] */
#include "aworks.h"
#include "aw_mem.h"
#include "aw_shell.h"
#include "aw_vdebug.h"
#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "io/sys/aw_stat.h"
#include <string.h>

static int __file_rw_speed_test (int                    argc,
                                 char                  *argv[],
                                 struct aw_shell_user  *p_user)
{
#define __DEFAULT_SIZE  (1024 * 32) /* 100 KB */
#define __RW_CHAR       'A'

    char       *rw_buf;
    int         rw_size, speed, cnt = 0;
    int         fd, i, len, err;
    aw_tick_t   ticks;
    aw_err_t    ret = AW_OK;

    (void)p_user;

    if (argc > 1) {
        if ((rw_size = atoi(argv[1])) < 1024) {
            rw_size = 1024;
        }
    } else {
        rw_size = __DEFAULT_SIZE;
    }
    AW_INFOF(("\"%s\": %dB(%uKB) read and write speed test\n",
              argv[0], rw_size, rw_size / 1024));

    if ((rw_buf = aw_mem_alloc(__DEFAULT_SIZE)) == NULL) {
        AW_ERRF(("alloc RW buffer failed\n"));
        return  -AW_ENOMEM;
    }

    cnt = max(1, rw_size / __DEFAULT_SIZE);

    /*
     * open (create if not exist) and truncate the file size to zero
     *
     */
    if ((fd = aw_open(argv[0], O_RDWR | O_CREAT | O_TRUNC, 0777)) < 0) {
        AW_ERRF(("Open to write failed\n"));
        ret = -AW_ENOENT;
        goto __exit;
    }

    (void)memset(rw_buf, __RW_CHAR, __DEFAULT_SIZE);

    /*
     *    write  test...
     */
    ticks = aw_sys_tick_get();

    for (i = 0; i < cnt; i++) {
        if ((len = aw_write(fd, rw_buf, __DEFAULT_SIZE)) != __DEFAULT_SIZE) {
            AW_ERRF(("Write failed\n"));
            aw_close(fd);
            ret = -AW_EIO;
            goto __exit;
        }
    }

    if ((err = aw_close(fd)) != AW_OK) {
        AW_ERRF(("Close write failed\n"));
        ret = err;
        goto __exit;
    }

    ticks = aw_sys_tick_get() - ticks;
    speed = rw_size / aw_ticks_to_ms(ticks) * 1000;
    AW_INFOF(("Write file speed: tick %d, %d B/s (%u KB/s)\n", ticks, speed, speed / 1024));


    /*
     *    read  test...
     */
    (void)memset(rw_buf, 0, __DEFAULT_SIZE);

    if ((fd = aw_open(argv[0], O_RDONLY, 0777)) < 0) {
        AW_ERRF(("Open to read failed\n"));
        ret = -AW_ENOENT;
        goto __exit;
    }

    ticks = aw_sys_tick_get();

    for (i = 0; i < cnt; i++) {
        if ((len = aw_read(fd, rw_buf, __DEFAULT_SIZE)) != __DEFAULT_SIZE) {
            AW_ERRF(("Read failed\n"));
            aw_close(fd);
            ret = -AW_EIO;
            goto __exit;
        }
    }

    if ((err = aw_close(fd)) != AW_OK) {
        AW_ERRF(("Close read failed\n"));
        ret = err;
        goto __exit;
    }

    ticks = aw_sys_tick_get() - ticks;
    speed = rw_size / aw_ticks_to_ms(ticks) * 1000;
    AW_INFOF(("Read file speed: tick %d, %d B/s (%u KB/s)\n", ticks, speed, speed / 1024));


    /* verify */
    for (i = 0; i < cnt; i++) {
        for (len = 0; len < __DEFAULT_SIZE; len++) {
            if (rw_buf[len] != __RW_CHAR) {
                AW_ERRF(("Verify failed at %i\n", i * len));
                goto  __exit;
            }
        }
    }

    AW_INFOF(("Verify data ok.\n"));

__exit:
    aw_mem_free(rw_buf);
    return  ret;
}

static const struct aw_shell_cmd __g_fs_test_cmds[] = {
    {__file_rw_speed_test,  "fspeed",  "<path> [size]  - file RW speed test"},
};

/**
 * \brief �����ļ�ϵͳ��д�ٶ� demo
 *
 * \return ��
 */
void demo_fs_fspeed_entry (void)
{
    static struct aw_shell_cmd_list cl;

    (void)AW_SHELL_REGISTER_CMDS(&cl, __g_fs_test_cmds);
}

/** [src_fs_speed] */

/* end of file */
