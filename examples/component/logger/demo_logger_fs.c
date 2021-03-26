/*******************************************************************************
*                                 AWorks
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
 * \brief ��־���̣�д��SD����
 *
 * - �������裺
 *   1. ��Ҫ��aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_COM_FS_FATFS
 *      - AW_COM_CONSOLE
 *      - AW_COM_SHELL_SERIAL
 *      - AW_COM_LOG
 *      �Լ����Դ��ڶ�Ӧ�ĺ��SD���ĺ�
 *   2. ���Ӳ���SD��
 *   3. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������
 *   1. ���ڴ�ӡ��־��Ϣ��
 *
 *
 * \par Դ����
 * \snippet demo_logger_fs.c src_loggger_fs
 *
 * \internal
 * \par Modification history
 * - 1.10 18-06-20  lqy, ����demo
 * - 1.00 14-08-13  hbt, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_loggger_fs ��־(�ļ�ϵͳ)
 * \copydoc demo_logger_fs.c
 */

/** [src_logger_fs] */
#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_log_fs.h"
#include "aw_shell.h"
#include "fs/aw_mount.h"
#include "fs/aw_blk_dev.h"

/* ����loggerʵ�� */
aw_local aw_log_fs_ctx_t __g_demo_log_ctx;

/* ������־������ʵ�� */
aw_local aw_logger_t *__g_p_demo_logger = NULL;

/* ���徲̬buffer */
aw_local char __g_buffer[128];
aw_local int  __g_buffer_size = 128;

/* ����log ID */
#define AW_LOG_VFAT_ID         1

/**
 * \brief ��־���洢���ļ�ϵͳ������
 * \return ��
 */
void demo_logger_fs_entry (void)
{
    int         err;
    uint32_t    log_size = 0;

    /* ������Ϣ��ʽ */
    aw_log_fmt_set(AW_LOG_LVL_INFO, AW_LOG_FMT_TIME);

    aw_log_fmt_set(AW_LOG_LVL_VERBOSE, AW_LOG_FMT_TIME      |
                                       AW_LOG_FMT_TASK_INFO |
                                       AW_LOG_FMT_DIR       |
                                       AW_LOG_FMT_FUNC      |
                                       AW_LOG_FMT_LINE);

    /* �ȴ�ϵͳʶ�𵽴洢�豸 */
    if (aw_blk_dev_wait_install("/dev/sd0", 3000)) {
        AW_INFOF(("/dev/sd0 is not exist\r\n"));
        goto __log_fs_task_entry_end;
    }

#if 0
    /*
     *  ͨ��shell�����ʽ��SD��Ϊfat�ļ�ϵͳ��
     *  ע�⣺��ʽ����SD���ļ�����ʧ��������
     */
    err = aw_shell_system("mkfs /dev/sd0 vfat");
    if (err != AW_OK) {
        AW_ERRF(("mkfs /dev/sd0 vfat err: %d\n", err));
        goto  __log_fs_task_entry_end;
    }
#endif

    /* �����ļ�ϵͳ
     * "/dev/sd0" ΪSD�����豸����
     */
    if ((err = aw_mount("/aw_log", "/dev/sd0", "vfat", 0)) != AW_OK) {
        AW_ERRF(("mount failed: %d\n", err));
        goto  __log_fs_task_entry_end;
    } else {
        AW_INFOF(("mount OK\n"));
    }

    /* ʵ������־���� */
    __g_p_demo_logger = aw_log_fs_ctor(&__g_demo_log_ctx,
                                       "/aw_log/aworks-log-demo.txt",
                                       1024,
                                       AW_FALSE);
    if (NULL == __g_p_demo_logger) {
        AW_ERRF(("aw_log_fs_ctor err!\n"));
        goto  __log_fs_task_entry_end;
    }

    /* ע����־�� */
    err = aw_logger_register(__g_p_demo_logger,
                             AW_LOG_VFAT_ID,
                             5000);
    if (err != AW_OK) {
        AW_ERRF(("aw_logger_register err: %d\n", err));
        goto  __log_fs_task_entry_end;
    }

    /* д����־ */
    err = aw_log_msg(AW_LOG_VFAT_ID,
                     "Hello, I'm aw_log_msg %d %s",
                     AW_LOG_VFAT_ID,
                     (int)"log_fs demo!!!",
                     0,
                     0,
                     0,
                     0);
    if (err != AW_OK) {
        AW_ERRF(("aw_log_msg err: %d\n", err));
        goto  __log_fs_task_entry_end;
    }

    aw_mdelay(1000);

    /* д��ϸ��־��ͬ���� */
    err = aw_log_msg_ex_sync(AW_LOG_VFAT_ID,
                             AW_LOG_LVL_VERBOSE,
                             "tag:fs log",
                             __FILE__,
                             __FUNCTION__,
                             __LINE__,
                             "\n%s %d\n",
                             (int)"log_fs demo exist!!!",
                             AW_LOG_VFAT_ID,
                             0,
                             0,
                             0,
                             0);
    if (err != AW_OK) {
        AW_ERRF(("aw_log_msg_ex_sync err: %d\n", err));
        goto  __log_fs_task_entry_end;
    }

    /* ��ȡ��ǰ��־������  */
    log_size = aw_log_data_size_get(__g_p_demo_logger);

    /*
     * һ���Զ�ȡ�ļ������־������־���ݴ�ŵ�__g_buffer��
     * err: �����Ѷ�ȡ��־��������С
     */
    err = aw_log_export(__g_p_demo_logger,
                        0,
                        __g_buffer,
                        (__g_buffer_size > log_size) ?
                            log_size : __g_buffer_size);
    if (err < 0) {
        AW_ERRF(("aw_log_export err: %d\n", err));
        goto  __log_fs_task_entry_end;
    }
    __g_buffer[err - 1] = 0;

    /* ��ӡ��־���� */
    AW_INFOF(("/aw_log/aworks-log-demo.txt: \n"));
    AW_INFOF(("%s\n", __g_buffer));

    /*
     * ѭ����ȡ��־����
     *
     */
    {
        int  read_offer = -log_size;
        int  cur_offset = 0;
        int  per_size   = 31;
        int  size       = 0;
        int  read_size  = (__g_buffer_size > log_size) ?
                              log_size : __g_buffer_size;

        while (1) {

            size = (cur_offset + per_size) > read_size ?
                       (read_size - cur_offset) : per_size;

            if (0 == size) {
                break;
            }

            /* ÿ�ζ�ȡ size ����־���� */
            err = aw_log_export(__g_p_demo_logger,
                                read_offer,
                                __g_buffer+cur_offset,
                                size
                                );
            if (err < 0) {
                AW_ERRF(("aw_log_export err: %d\n", err));
                goto  __log_fs_task_entry_end;
            }

            cur_offset += size;
        }

        __g_buffer[read_size - 1] = 0;

        /* ��ӡ��־���� */
        AW_INFOF(("/aw_log/aworks-log-demo.txt: \n"));
        AW_INFOF(("%s\n", __g_buffer));
    }

__log_fs_task_entry_end:

    /* ж����־�� */
    aw_logger_unregister(__g_p_demo_logger);
    return ;
}

/** [src_logger_fs] */

/* end of file */
