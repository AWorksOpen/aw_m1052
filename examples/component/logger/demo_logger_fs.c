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
 * \brief 日志例程（写入SD卡）
 *
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能
 *      - AW_COM_FS_FATFS
 *      - AW_COM_CONSOLE
 *      - AW_COM_SHELL_SERIAL
 *      - AW_COM_LOG
 *      以及调试串口对应的宏和SD卡的宏
 *   2. 板子插上SD卡
 *   3. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象：
 *   1. 串口打印日志信息。
 *
 *
 * \par 源代码
 * \snippet demo_logger_fs.c src_loggger_fs
 *
 * \internal
 * \par Modification history
 * - 1.10 18-06-20  lqy, 更新demo
 * - 1.00 14-08-13  hbt, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_loggger_fs 日志(文件系统)
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

/* 定义logger实例 */
aw_local aw_log_fs_ctx_t __g_demo_log_ctx;

/* 定义日志上下文实例 */
aw_local aw_logger_t *__g_p_demo_logger = NULL;

/* 定义静态buffer */
aw_local char __g_buffer[128];
aw_local int  __g_buffer_size = 128;

/* 定义log ID */
#define AW_LOG_VFAT_ID         1

/**
 * \brief 日志（存储到文件系统）范例
 * \return 无
 */
void demo_logger_fs_entry (void)
{
    int         err;
    uint32_t    log_size = 0;

    /* 设置消息格式 */
    aw_log_fmt_set(AW_LOG_LVL_INFO, AW_LOG_FMT_TIME);

    aw_log_fmt_set(AW_LOG_LVL_VERBOSE, AW_LOG_FMT_TIME      |
                                       AW_LOG_FMT_TASK_INFO |
                                       AW_LOG_FMT_DIR       |
                                       AW_LOG_FMT_FUNC      |
                                       AW_LOG_FMT_LINE);

    /* 等待系统识别到存储设备 */
    if (aw_blk_dev_wait_install("/dev/sd0", 3000)) {
        AW_INFOF(("/dev/sd0 is not exist\r\n"));
        goto __log_fs_task_entry_end;
    }

#if 0
    /*
     *  通过shell命令格式化SD卡为fat文件系统，
     *  注意：格式化后SD卡文件将丢失，谨慎打开
     */
    err = aw_shell_system("mkfs /dev/sd0 vfat");
    if (err != AW_OK) {
        AW_ERRF(("mkfs /dev/sd0 vfat err: %d\n", err));
        goto  __log_fs_task_entry_end;
    }
#endif

    /* 挂载文件系统
     * "/dev/sd0" 为SD卡块设备名字
     */
    if ((err = aw_mount("/aw_log", "/dev/sd0", "vfat", 0)) != AW_OK) {
        AW_ERRF(("mount failed: %d\n", err));
        goto  __log_fs_task_entry_end;
    } else {
        AW_INFOF(("mount OK\n"));
    }

    /* 实例化日志对象 */
    __g_p_demo_logger = aw_log_fs_ctor(&__g_demo_log_ctx,
                                       "/aw_log/aworks-log-demo.txt",
                                       1024,
                                       AW_FALSE);
    if (NULL == __g_p_demo_logger) {
        AW_ERRF(("aw_log_fs_ctor err!\n"));
        goto  __log_fs_task_entry_end;
    }

    /* 注册日志器 */
    err = aw_logger_register(__g_p_demo_logger,
                             AW_LOG_VFAT_ID,
                             5000);
    if (err != AW_OK) {
        AW_ERRF(("aw_logger_register err: %d\n", err));
        goto  __log_fs_task_entry_end;
    }

    /* 写简单日志 */
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

    /* 写详细日志（同步） */
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

    /* 获取当前日志数据量  */
    log_size = aw_log_data_size_get(__g_p_demo_logger);

    /*
     * 一次性读取文件里的日志，将日志数据存放到__g_buffer中
     * err: 返回已读取日志数据量大小
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

    /* 打印日志数据 */
    AW_INFOF(("/aw_log/aworks-log-demo.txt: \n"));
    AW_INFOF(("%s\n", __g_buffer));

    /*
     * 循环读取日志数据
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

            /* 每次读取 size 个日志数据 */
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

        /* 打印日志数据 */
        AW_INFOF(("/aw_log/aworks-log-demo.txt: \n"));
        AW_INFOF(("%s\n", __g_buffer));
    }

__log_fs_task_entry_end:

    /* 卸载日志器 */
    aw_logger_unregister(__g_p_demo_logger);
    return ;
}

/** [src_logger_fs] */

/* end of file */
