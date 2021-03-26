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
 * \brief 日志例程（存储到FTL）
 *
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能以下宏：
 *      - FLEXSPI使能宏
 *      - AW_CFG_FLEXSPI_FLASH_MTD
 *      - SPI1使能宏
 *      - AW_COM_CONSOLE
 *      - AW_COM_FTL
 *      - AW_COM_LOG
 *      - 调试串口使能宏
 *   2. 将板子中的RX1、TX1通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象：
 *   1. 串口打印日志信息。
 *
 * - 备注：
 *   1. 如果工程在初始化时出错，请查看此demo所使用到的GPIO是否存在引脚复用。
 *
 *
 * \par 源代码
 * \snippet demo_logger_ftl.c src_log_ftl
 *
 * \internal
 * \par Modification history
 * - 1.00 17-08-13  hbt, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_log_ftl 日志(FTL)
 * \copydoc demo_logger_ftl.c
 */

/** [src_log_ftl] */
#include "aworks.h"
#include "aw_log_ftl.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_demo_config.h"

/* 定义FTL logger实例 */
static aw_log_ftl_ctx_t __g_ftl_ctx;

/* 定义日志上下文实例 */
static aw_logger_t *__g_p_logger;

/* 定义静态buffer */
static char __g_buffer[128];
static int  __g_buffer_size = 128;

/* 定义log ID */
#define AW_LOG_SYS_ID  2

/**
 * \brief 日志（存储到FTL）范例
 * \return 无
 */
void demo_logger_ftl_entry (void)
{
    int         err = AW_OK;
    uint32_t    log_size = 0;

    /* 设置消息格式 */
    aw_log_fmt_set(AW_LOG_LVL_INFO,    AW_LOG_FMT_TIME);

    aw_log_fmt_set(AW_LOG_LVL_VERBOSE, AW_LOG_FMT_TIME      |
                                       AW_LOG_FMT_TASK_INFO |
                                       AW_LOG_FMT_DIR       |
                                       AW_LOG_FMT_FUNC      |
                                       AW_LOG_FMT_LINE);

    /* 初始化 ftl 日志存储 */
    __g_p_logger = aw_log_ftl_ctor(&__g_ftl_ctx, DE_SPI_FLASH, AW_TRUE);
    if (NULL == __g_p_logger) {
        AW_ERRF(("aw_log_fs_ctor err!\n"));
        goto  __log_ftl_task_entry_end;
    }

    /* 注册logger */
    err = aw_logger_register(__g_p_logger,
                             AW_LOG_SYS_ID,
                             1500);
    if (err != AW_OK) {
        AW_ERRF(("aw_logger_register err: %d\n", err));
        goto  __log_ftl_task_entry_end;
    }

    /* 写详细日志 */
    err = aw_log_msg_ex_sync(AW_LOG_SYS_ID,
                             AW_LOG_LVL_VERBOSE,
                             "tag:flash log ",
                             __FILE__,
                             __FUNCTION__,
                             __LINE__,
                             "\n[ID:%d]%s \n",
                             AW_LOG_SYS_ID,
                             (int)"wrrite std log demo.",
                             0,
                             0,
                             0,
                             0);
    if (err != AW_OK) {
        AW_ERRF(("aw_log_msg_ex_sync err: %d\n", err));
        goto  __log_ftl_task_entry_end;
    }

    /* 获取当前日志数据量  */
    log_size = aw_log_data_size_get(__g_p_logger);

    /*
     * 一次性读取文件里的日志，将日志数据存放到__g_buffer中
     * err: 返回已读取日志数据量大小
     */
    err = aw_log_export(__g_p_logger,
                        0,
                        __g_buffer,
                        (__g_buffer_size > log_size) ?
                            log_size : __g_buffer_size);
    if (err < 0) {
        AW_ERRF(("aw_log_export err: %d\n", err));
        goto  __log_ftl_task_entry_end;
    }
    __g_buffer[err - 1] = 0;

    /* 打印日志数据 */
    AW_INFOF(("log data: \n"));
    AW_INFOF(("%s\n", __g_buffer));

    memset(__g_buffer, 0, sizeof(__g_buffer));

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
            err = aw_log_export(__g_p_logger,
                                read_offer,
                                __g_buffer+cur_offset,
                                size
                                );
            if (err < 0) {
                AW_ERRF(("aw_log_export err: %d\n", err));
                goto  __log_ftl_task_entry_end;
            }
            if (read_offer != 0) {
                read_offer=0;
            }

            cur_offset += size;
        }

        __g_buffer[read_size - 1] = 0;

        /* 打印日志数据 */
        AW_INFOF(("log data: \n"));
        AW_INFOF(("%s\n", __g_buffer));
    }

__log_ftl_task_entry_end:
    aw_logger_unregister(__g_p_logger); /* 卸载日志器 */

    return ;
}

/** [src_log_ftl] */

/* end of file */
