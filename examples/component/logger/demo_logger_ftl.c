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
 * \brief ��־���̣��洢��FTL��
 *
 * - �������裺
 *   1. ��Ҫ��aw_prj_params.hͷ�ļ���ʹ�����º꣺
 *      - FLEXSPIʹ�ܺ�
 *      - AW_CFG_FLEXSPI_FLASH_MTD
 *      - SPI1ʹ�ܺ�
 *      - AW_COM_CONSOLE
 *      - AW_COM_FTL
 *      - AW_COM_LOG
 *      - ���Դ���ʹ�ܺ�
 *   2. �������е�RX1��TX1ͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *
 * - ʵ������
 *   1. ���ڴ�ӡ��־��Ϣ��
 *
 * - ��ע��
 *   1. ��������ڳ�ʼ��ʱ������鿴��demo��ʹ�õ���GPIO�Ƿ�������Ÿ��á�
 *
 *
 * \par Դ����
 * \snippet demo_logger_ftl.c src_log_ftl
 *
 * \internal
 * \par Modification history
 * - 1.00 17-08-13  hbt, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_log_ftl ��־(FTL)
 * \copydoc demo_logger_ftl.c
 */

/** [src_log_ftl] */
#include "aworks.h"
#include "aw_log_ftl.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_demo_config.h"

/* ����FTL loggerʵ�� */
static aw_log_ftl_ctx_t __g_ftl_ctx;

/* ������־������ʵ�� */
static aw_logger_t *__g_p_logger;

/* ���徲̬buffer */
static char __g_buffer[128];
static int  __g_buffer_size = 128;

/* ����log ID */
#define AW_LOG_SYS_ID  2

/**
 * \brief ��־���洢��FTL������
 * \return ��
 */
void demo_logger_ftl_entry (void)
{
    int         err = AW_OK;
    uint32_t    log_size = 0;

    /* ������Ϣ��ʽ */
    aw_log_fmt_set(AW_LOG_LVL_INFO,    AW_LOG_FMT_TIME);

    aw_log_fmt_set(AW_LOG_LVL_VERBOSE, AW_LOG_FMT_TIME      |
                                       AW_LOG_FMT_TASK_INFO |
                                       AW_LOG_FMT_DIR       |
                                       AW_LOG_FMT_FUNC      |
                                       AW_LOG_FMT_LINE);

    /* ��ʼ�� ftl ��־�洢 */
    __g_p_logger = aw_log_ftl_ctor(&__g_ftl_ctx, DE_SPI_FLASH, AW_TRUE);
    if (NULL == __g_p_logger) {
        AW_ERRF(("aw_log_fs_ctor err!\n"));
        goto  __log_ftl_task_entry_end;
    }

    /* ע��logger */
    err = aw_logger_register(__g_p_logger,
                             AW_LOG_SYS_ID,
                             1500);
    if (err != AW_OK) {
        AW_ERRF(("aw_logger_register err: %d\n", err));
        goto  __log_ftl_task_entry_end;
    }

    /* д��ϸ��־ */
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

    /* ��ȡ��ǰ��־������  */
    log_size = aw_log_data_size_get(__g_p_logger);

    /*
     * һ���Զ�ȡ�ļ������־������־���ݴ�ŵ�__g_buffer��
     * err: �����Ѷ�ȡ��־��������С
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

    /* ��ӡ��־���� */
    AW_INFOF(("log data: \n"));
    AW_INFOF(("%s\n", __g_buffer));

    memset(__g_buffer, 0, sizeof(__g_buffer));

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

        /* ��ӡ��־���� */
        AW_INFOF(("log data: \n"));
        AW_INFOF(("%s\n", __g_buffer));
    }

__log_ftl_task_entry_end:
    aw_logger_unregister(__g_p_logger); /* ж����־�� */

    return ;
}

/** [src_log_ftl] */

/* end of file */
