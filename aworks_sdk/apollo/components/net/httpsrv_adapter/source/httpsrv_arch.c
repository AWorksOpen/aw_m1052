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
 * \brief HTTPSRV "OS support" emulation layer
 *
 * \internal
 * \par History
 * - 17-08-11, sdy, First implementation.
 * \endinternal
 */

#undef AW_VDEBUG
#undef AW_VDEBUG_DEBUG
#undef AW_VDEBUG_INFO
#undef AW_VDEBUG_WARN
#undef AW_VDEBUG_ERROR

#include "aw_vdebug.h"

/******************************************************************************/

#include "lwip/sys.h"
#include "lwip/opt.h"
#include "lwip/stats.h"
#include "arch/sys_arch.h"

#include "string.h"
#include "aw_assert.h"
#include "aw_sem.h"
#include "aw_mem.h"
#include "aw_msgq.h"
#include "aw_pool.h"
#include "aw_task.h"
#include "aw_system.h"
#include "aw_delay.h"                   /* 延时服务 */
#include "httpsrv_config.h"
#include "httpsrv_arch.h"
#include "aw_task2.h"
/*******************************************************************************
  defines
*******************************************************************************/

#define __HTTPSRV_THREAD_NAME                  "HTTP server"
#define __HTTPSRV_SESSION_THREAD_NAME          "HTTP server session"
#define __HTTPSSRV_SESSION_THREAD_NAME         "HTTPS server session"

//typedef struct httpsrv_session {
//    sys_thread_t id;        /* 任务id */
//    uint32_t     num;       /* 结构体编号 */
//    uint8_t      state;     /* 任务状态 */
//}httpsrv_session_t;


/*******************************************************************************
  public variables
*******************************************************************************/

#if 0 
extern sys_thread_t httpsrv_session_thread_new_start (int             num,
                                                      lwip_thread_fn  thread,
                                                      void           *arg,
                                                      int             prio);
#endif 
/*******************************************************************************
  local variables
*******************************************************************************/

struct httpsrv_info {
    uint32_t thread_http_session_num;    /**< \brief HTTP连接任务最大数 */
    uint32_t thread_https_session_num;   /**< \brief HTTPS连接任务最大数 */
    uint32_t thread_prio;                /**< \brief 任务优先级 */

    uint32_t socket_send_timeout;
    uint32_t socket_receive_timeout;

    uint32_t ses_timeout;                /**< \brief 会话超时时间 */

    uint32_t keepalive_timeout;          /**< \brief 长连接超时时间 */

    uint32_t url_len;                    /**< \brief 允许URL最大长度 */
    uint32_t ses_buffer_size;            /**< \brief 会话缓存大小 */
    aw_bool_t   keepalive;               /**< \brief 长连接使能开关 */
};

static struct httpsrv_info __g_info;

/* static httpsrv_session_t *__gp_httpsrv_session = NULL; */

/*******************************************************************************
  functions
*******************************************************************************/

void httpsrv_thread_http_session_num_init(int num)
{
    if(num > 0) {
        __g_info.thread_http_session_num = num;
    }
}

int httpsrv_thread_http_session_num_get(void)
{
    return __g_info.thread_http_session_num;
}

void httpsrv_thread_https_session_num_init(int num)
{
    if(num > 0) {
        __g_info.thread_https_session_num = num;
    }
}

int httpsrv_thread_https_session_num_get(void)
{
    return __g_info.thread_https_session_num;
}

void httpsrv_thread_prio_init(int prio)
{
    __g_info.thread_prio = prio;
}

int httpsrv_thread_prio_get(void)
{
    return __g_info.thread_prio;
}

void httpsrv_keepalive_init(aw_bool_t keep_switch, uint32_t overtime)
{
    __g_info.keepalive = keep_switch;
    __g_info.keepalive_timeout = overtime;
}

aw_bool_t httpsrv_keepalive_switch_get(void)
{
    return __g_info.keepalive;
}

uint32_t httpsrv_keepalive_overtime_get(void)
{
    return __g_info.keepalive_timeout;
}

void httpsrv_socket_opt_timeout_init(uint32_t send_timeout,
                                     uint32_t receive_timeout)
{
    __g_info.socket_send_timeout    = send_timeout;
    __g_info.socket_receive_timeout = receive_timeout;
}

uint32_t httpsrv_socket_send_timeout_get(void)
{
    return __g_info.socket_send_timeout;
}

uint32_t httpsrv_socket_receive_timeout_get(void)
{
    return __g_info.socket_receive_timeout;
}

void httpsrv_ses_timeout_init(uint32_t ses_timeout)
{
    __g_info.ses_timeout    = ses_timeout;
}

uint32_t httpsrv_ses_timeout_get(void)
{
    return __g_info.ses_timeout;
}

void httpsrv_url_len_init(int url_len)
{
    __g_info.url_len = url_len;
}

int httpsrv_url_len_get(void)
{
    return __g_info.url_len;
}

void httpsrv_ses_buffer_size_init(int size)
{
    __g_info.ses_buffer_size = size;
}

int httpsrv_ses_buffer_size_get(void)
{
    return __g_info.ses_buffer_size;
}
/*******************************************************************************
  task manage
*******************************************************************************/

sys_thread_t
httpsrv_thread_new (enum httpsrv_ses_thread_type  type,
                    uint32_t                     *p_id,
                    lwip_thread_fn                thread,
                    void                         *arg,
                    int                           prio)
{
     aw_task_id_t task_id = AW_TASK_ID_INVALID;

     switch (type) {
     case HTTPSRV_SES_THREAD_TYPE_HTTP: {

         extern sys_thread_t httpsr_http_thread_new_start (
                    uint32_t       *p_id,
                    lwip_thread_fn  thread,
                    void           *arg,
                    int             prio);
         task_id = httpsr_http_thread_new_start (p_id, thread, arg, prio);
     }
         break;
     case HTTPSRV_SES_THREAD_TYPE_HTTPS: {
         extern sys_thread_t httpsr_https_thread_new_start (
                    uint32_t       *p_id,
                    lwip_thread_fn  thread,
                    void           *arg,
                    int             prio);
         task_id = httpsr_https_thread_new_start (p_id, thread, arg, prio);
     }
         break;
     default:
         break;
     }


    return task_id;
}

sys_thread_t
httpsrv_http_ses_thread_new (uint32_t        id,
                             lwip_thread_fn  thread,
                             void           *arg,
                             int             prio)
{
    aw_task_id_t task_id = AW_TASK_ID_INVALID;

    extern sys_thread_t
    httpsrv_http_ses_thread_new_start (uint32_t        id,
                                       lwip_thread_fn  thread,
                                       void           *arg,
                                       int             prio);

    task_id = httpsrv_http_ses_thread_new_start (id,
                                                 thread,
                                                 arg,
                                                 prio);

    return task_id;
}

sys_thread_t
httpsrv_https_ses_thread_new (uint32_t       id,
                             lwip_thread_fn  thread,
                             void           *arg,
                             int             prio)
{
    aw_task_id_t task_id = AW_TASK_ID_INVALID;

    extern sys_thread_t
    httpsrv_https_ses_thread_new_start (uint32_t        id,
                                        lwip_thread_fn  thread,
                                        void           *arg,
                                        int             prio);

    task_id = httpsrv_https_ses_thread_new_start (id,
                                                  thread,
                                                  arg,
                                                  prio);

    return task_id;
}

/** 删掉线程  */
void httpsrv_thread_delete(void *id)
{
    if (id) {
        AW_DBGF(("httpsrv_thread_delete(%p)\r\n\r\n", id));
        extern void httpsrv_thread_del(sys_thread_t id);
        httpsrv_thread_del((sys_thread_t)id);
        return;
    } else {
        id = aw_task_id_self();
        AW_DBGF(("httpsrv_thread_delete current task %p\r\n\r\n", id));
        extern void httpsrv_http_ses_thread_del(sys_thread_t id);
        extern void httpsrv_https_ses_thread_del(sys_thread_t id);
        httpsrv_http_ses_thread_del(id);
        httpsrv_https_ses_thread_del(id);
        return;
    }
}

//#undef AW_DBGF
//#define AW_DBGF(msg)    (void)aw_kprintf msg

void *httpsrv_mem_alloc (size_t size)
{
    void *ptr = aw_mem_alloc(size);
    AW_DBGF(("httpsrv_malloc(%u):%p\r\n", size, ptr));
    return ptr;
}

void httpsrv_mem_free (void *ptr)
{
    AW_DBGF(("httpsrv_free(%p)\r\n", ptr));
    aw_mem_free(ptr);
}

void *httpsrv_mem_alloc_zero (size_t size)
{
    void *ptr = aw_mem_calloc(1, size);
    AW_DBGF(("httpsrv_calloc(%u):%p\r\n", size, ptr));
    return ptr;
}

/* end of file */
