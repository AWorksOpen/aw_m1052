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
 * \file
 * \brief Compiler defines for lwip.
 *
 * \internal
 * \par History
 * - 130604, orz, modify for anywhere platform.
 * - 110506, orz, First implementation.
 * \endinternal
 */

#ifndef __HTTPSRV_ARCH_H__
#define __HTTPSRV_ARCH_H__

#include "arch/cc.h"
#include "aworks.h"
#include "aw_task.h"
#include "arch/sys_arch.h"

#define bool  aw_bool_t
#define false AW_FALSE
#define true  AW_TRUE

#define taskYIELD()       aw_task_yield()
#define vTaskDelete(a)    httpsrv_thread_delete(a)

enum httpsrv_ses_thread_type {
      HTTPSRV_SES_THREAD_TYPE_HTTP,
      HTTPSRV_SES_THREAD_TYPE_HTTPS,
};

int httpsrv_thread_http_session_num_get(void);
int httpsrv_thread_https_session_num_get(void);
int httpsrv_thread_prio_get(void);
aw_bool_t httpsrv_keepalive_switch_get(void);
uint32_t httpsrv_keepalive_overtime_get(void);
uint32_t httpsrv_socket_send_timeout_get(void);
uint32_t httpsrv_socket_receive_timeout_get(void);
uint32_t httpsrv_ses_timeout_get(void);
int httpsrv_url_len_get(void);

void *httpsrv_memp_ws_handshake_alloc(void);

aw_err_t httpsrv_memp_ws_handshake_free(void *p_item);

void *httpsrv_memp_server_alloc(void);

aw_err_t httpsrv_memp_server_free(void *p_item);

void *httpsrv_memp_session_alloc(void);

aw_err_t httpsrv_memp_session_free(void *p_item);

void *httpsrv_memp_ses_param_alloc(void);

aw_err_t httpsrv_memp_ses_param_free(void *p_item);

void *httpsrv_memp_request_path_alloc(void);

aw_err_t httpsrv_memp_request_path_free(void *p_item);

void *httpsrv_memp_buffer_data_alloc(void);


aw_err_t httpsrv_memp_buffer_data_free(void *p_item);

sys_thread_t
httpsrv_thread_new (enum httpsrv_ses_thread_type  type,
                    uint32_t                     *p_id,
                    lwip_thread_fn                thread,
                    void                         *arg,
                    int                           prio);

sys_thread_t
httpsrv_http_ses_thread_new (uint32_t        id,
                             lwip_thread_fn  thread,
                             void           *arg,
                             int             prio);

sys_thread_t
httpsrv_https_ses_thread_new (uint32_t       id,
                             lwip_thread_fn  thread,
                             void           *arg,
                             int             prio);

void httpsrv_thread_delete(void *id);

#endif /* __HTTPSRV_ARCH_H__ */

/* end of file */
