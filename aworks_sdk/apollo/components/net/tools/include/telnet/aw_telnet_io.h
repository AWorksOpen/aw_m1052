/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * @file
 * @brief telnet io defines
 *
 * @internal
 * @par History
 * - 150430, rnk, First implementation.
 * @endinternal
 */

#ifndef __AW_TELNET_IO_H
#define __AW_TELNET_IO_H

#include "telnet/aw_telnet.h"
#include "aw_task.h"

/**< \brief Э��ѡ�� */
//#define OPTS_UNSET     0
#define OPTS_WW_FLAG   1
#define OPTS_DD_FLAG   2
#define OPTS_SET       3
#define OPTS_UNSET     4

struct aw_telnet_session;
typedef void* (*aw_telnet_open_t)(struct aw_telnet_session *ts);
typedef void (*aw_telnet_close_t)(void* handle);
typedef int (*aw_telnet_write_t)(void* handle, char* buf, int size);
typedef int (*aw_telnet_read_t)(void* handle, char* buf, int size);

typedef struct aw_telnet_io_fn
{
    aw_telnet_open_t  open;
    aw_telnet_close_t close;
    aw_telnet_write_t write;
    aw_telnet_read_t  read;
}aw_telnet_io_fn_t;

typedef struct aw_task_context {
    aw_task_id_t id;
    const char* name;
    int prio;
    char* stack;
    int stack_size;
    void *pfunc;
    void *arg;
}aw_task_context_t;

#define __TASK_INIT(x) rtk_task_init((x).id,                         \
                                     (x).name,                       \
                                     (x).prio,                       \
                                      0,                             \
                                     (x).stack,                      \
                                     (x).stack + (x).stack_size - 1, \
                                     (x).pfunc,                      \
                                     (x).arg,                        \
                                      0);

/* telnet����ʵ�������� */
struct aw_telnet_srv_dev;
typedef struct aw_telnet_session
{
    int session_valid;
    int client_socket;
    char remote_opt[TELNET_MAX_OPTS];
    char local_opt[TELNET_MAX_OPTS];
    void* ts_handle;
    void* user_arg;
    struct aw_telnet_srv_dev *srv;
    aw_task_id_t tsk_id;
    aw_task_context_t tsk_cont;
}aw_telnet_session_t;

/** \brief ����������ʵ�������� */
typedef struct aw_telnet_srv_dev {
    struct aw_telnet_io_fn fn;
    struct aw_telnet_session *ts;
    int ts_count;
    int listen_port;
    char user_name[16];
    char user_psw[16];
    aw_task_id_t tsk_id;
    aw_task_context_t tsk_cont;
}aw_telnet_srv_dev_t;

/* ��telnet�ͻ��˷��ͽ������� */
int aw_telnet_send(struct aw_telnet_session *cc, const char *buf, int size);

/* ����һ��telnet�Ự */
int aw_telnet_session_run(struct aw_telnet_session *ts);

/* ����telnet������ */
int aw_telnet_srv_run(struct aw_telnet_srv_dev *dev);

#endif /* __AW_TELNET_H */

/* end of file */
