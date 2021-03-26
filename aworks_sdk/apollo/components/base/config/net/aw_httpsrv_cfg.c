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

/******************************************************************************/

#include "lwip/sys.h"
#include "lwip/opt.h"
#include "lwip/stats.h"
#include "arch/sys_arch.h"

#include "aw_task.h"
#include "aw_system.h"
#include "httpsrv_config.h"
#include "aw_prj_params.h"
/*******************************************************************************
  defines
*******************************************************************************/

#ifndef AW_HTTPSRV_DBGF_ENABNLE
#define AW_HTTPSRV_DBGF_ENABNLE      0
#endif

#if AW_HTTPSRV_DBGF_ENABNLE
#define HTTPSRV_DBGF(...)   (void)aw_kprintf(__VA_ARGS__)
#else
#define HTTPSRV_DBGF(...)
#endif
#undef AW_HTTPSRV_DBGF_ENABNLE

/** \brief HTTP 监听任务最大个数 */
#ifndef  AW_HTTPSRV_CFG_HTTP_DEFAULT_SRV_CNT
#define AW_HTTPSRV_CFG_HTTP_DEFAULT_SRV_CNT    1
#endif

#if HTTPSRV_CFG_WOLFSSL_ENABLE || HTTPSRV_CFG_MBEDTLS_ENABLE
    #ifndef AW_HTTPSRV_CFG_HTTPS_DEFAULT_SRV_CNT
    /** \brief HTTPS 监听任务最大个数 */
    #define AW_HTTPSRV_CFG_HTTPS_DEFAULT_SRV_CNT   1
    #endif /* AW_HTTPSRV_CFG_HTTPS_DEFAULT_SRV_CNT */
#else
/** \brief HTTPS 监听任务最大个数 */
#undef AW_HTTPSRV_CFG_HTTPS_DEFAULT_SRV_CNT
#define AW_HTTPSRV_CFG_HTTPS_DEFAULT_SRV_CNT   0
#endif
/** \brief 监听任务最大个数 */
#define __HTTPSRV_CFG_DEFAULT_SRV_CNT   (AW_HTTPSRV_CFG_HTTP_DEFAULT_SRV_CNT + \
                                         AW_HTTPSRV_CFG_HTTPS_DEFAULT_SRV_CNT)

/** \brief HTTP会话任务最大个数  */
#ifndef AW_HTTPSRV_CFG_HTTP_DEFAULT_SES_CNT
#define AW_HTTPSRV_CFG_HTTP_DEFAULT_SES_CNT    5
#endif /* AW_HTTPSRV_CFG_HTTP_DEFAULT_SES_CNT */

/** \brief HTTPS会话任务最大个数  */
#ifndef AW_HTTPSRV_CFG_HTTPS_DEFAULT_SES_CNT
#define AW_HTTPSRV_CFG_HTTPS_DEFAULT_SES_CNT   1
#endif /* AW_HTTPSRV_CFG_HTTPS_DEFAULT_SES_CNT */

/** \brief 监听任务堆栈大小 */
#ifndef AW_HTTPSRV_CFG_SERVER_STACK_SIZE
#define AW_HTTPSRV_CFG_SERVER_STACK_SIZE         (3500)
#endif /* AW_HTTPSRV_CFG_SERVER_STACK_SIZE */

/** \brief HTTP会话任务堆栈大小 */
#ifndef AW_HTTPSRV_CFG_HTTP_SESSION_STACK_SIZE
#define AW_HTTPSRV_CFG_HTTP_SESSION_STACK_SIZE   (1024 * 3)
#endif /* AW_HTTPSRV_CFG_HTTP_SESSION_STACK_SIZE */

/** \brief HTTPS会话任务堆栈大小 */
#ifndef AW_HTTPSRV_CFG_HTTPS_SESSION_STACK_SIZE
#define AW_HTTPSRV_CFG_HTTPS_SESSION_STACK_SIZE  (28000)
#endif /* AW_HTTPSRV_CFG_HTTPS_SESSION_STACK_SIZE */

/** \brief 任务优先级 */
#ifndef AW_HTTPSRV_CFG_DEFAULT_PRIORITY
#define AW_HTTPSRV_CFG_DEFAULT_PRIORITY           3
#endif /* AW_HTTPSRV_CFG_DEFAULT_PRIORITY */

/** \brief keep-alive 使能开关 */
#ifndef AW_HTTPSRV_CFG_KEEPALIVE_ENABLE
#define AW_HTTPSRV_CFG_KEEPALIVE_ENABLE (0)
#endif /* AW_HTTPSRV_CFG_KEEPALIVE_ENABLE */

/** \brief keep-alive 会话超时时间（毫秒） */
#ifndef AW_HTTPSRV_CFG_KEEPALIVE_TIMEOUT
#define AW_HTTPSRV_CFG_KEEPALIVE_TIMEOUT (200)
#endif /* AW_HTTPSRV_CFG_KEEPALIVE_TIMEOUT */

/** \brief 会话超时时间（毫秒） */
#ifndef AW_HTTPSRV_CFG_SES_TIMEOUT
#define AW_HTTPSRV_CFG_SES_TIMEOUT (20000)
#endif /* AW_HTTPSRV_CFG_SES_TIMEOUT */

/** \brief Socket OPT_SEND_TIMEOUT 选项值  */
#ifndef AW_HTTPSRV_CFG_SEND_TIMEOUT
#define AW_HTTPSRV_CFG_SEND_TIMEOUT (500)
#endif /* AW_HTTPSRV_CFG_SEND_TIMEOUT */

/** \brief Socket OPT_RECEIVE_TIMEOUT 选项值  */
#ifndef AW_HTTPSRV_CFG_RECEIVE_TIMEOUT
#define AW_HTTPSRV_CFG_RECEIVE_TIMEOUT (50)
#endif /* AW_HTTPSRV_CFG_RECEIVE_TIMEOUT */

/** \brief Maximal URL length. 允许URL最大长度  */
#ifndef AW_HTTPSRV_CFG_DEFAULT_URL_LEN
#define AW_HTTPSRV_CFG_DEFAULT_URL_LEN (128)
#endif /* AW_HTTPSRV_CFG_DEFAULT_URL_LEN */

/** \brief Session buffer size */
#ifndef AW_HTTPSRV_CFG_SES_BUFFER_SIZE
#define AW_HTTPSRV_CFG_SES_BUFFER_SIZE (1360)
#endif /* AW_HTTPSRV_CFG_SES_BUFFER_SIZE */

#if AW_HTTPSRV_CFG_SES_BUFFER_SIZE < 512
#undef AW_HTTPSRV_CFG_SES_BUFFER_SIZE
#define AW_HTTPSRV_CFG_SES_BUFFER_SIZE (512)
#endif
/******************************************************************************/
#define __TASK_TYPE_NAME(name) \
    __aw_httpsrv_##name##_task_t

#define __TASK_TASK_NAME(name) \
    arch_httpsrv_##name

#define __TASK_OBJ_NAME(name) \
    arch_httpsrv_##name##_thread

#define __HTTPSRV_TASK_DECL(name, task, stack_size, count) \
    struct __TASK_TYPE_NAME(name) { \
        aw_task_id_t id; \
        AW_TASK_DECL(__TASK_TASK_NAME(name), (stack_size)); \
    } __TASK_OBJ_NAME(name)[count];

/******************************************************************************/

#define AW_HTTPSRV_THREAD_NAME                  "HTTP server"
#define AW_HTTPSSRV_THREAD_NAME                 "HTTPS server"
#define AW_HTTPSRV_THREAD_SERVER_STACKSIZE       AW_HTTPSRV_CFG_SERVER_STACK_SIZE

#define AW_HTTPSRV_SESSION_THREAD_NAME          "HTTP session"
#define AW_HTTPSSRV_SESSION_THREAD_NAME         "HTTPS session"

/*******************************************************************************
  public variables
*******************************************************************************/
extern void httpsrv_thread_http_session_num_init(int num);
extern void httpsrv_thread_https_session_num_init(int num);
extern void httpsrv_thread_prio_init(int prio);
extern void httpsrv_keepalive_init(aw_bool_t keep_switch, uint32_t overtime);
extern void httpsrv_ses_timeout_init(uint32_t ses_timeout);
extern void httpsrv_socket_opt_timeout_init(uint32_t send_timeout, uint32_t receive_timeout);
extern void httpsrv_url_len_init(int url_len);
extern void httpsrv_ses_buffer_size_init(int size);

/*******************************************************************************
  local variables
*******************************************************************************/
aw_local
__HTTPSRV_TASK_DECL(server,
                    http_srv_thread,
                    AW_HTTPSRV_THREAD_SERVER_STACKSIZE,
                    __HTTPSRV_CFG_DEFAULT_SRV_CNT);

#if AW_HTTPSRV_CFG_HTTP_DEFAULT_SRV_CNT
__HTTPSRV_TASK_DECL(http_ses,
                    http_ses_thread,
                    AW_HTTPSRV_CFG_HTTP_SESSION_STACK_SIZE,
                    (AW_HTTPSRV_CFG_HTTP_DEFAULT_SRV_CNT * \
                     AW_HTTPSRV_CFG_HTTP_DEFAULT_SES_CNT));
#endif

#if AW_HTTPSRV_CFG_HTTPS_DEFAULT_SRV_CNT
__HTTPSRV_TASK_DECL(https_ses,
                    https_ses_thread,
                    AW_HTTPSRV_CFG_HTTPS_SESSION_STACK_SIZE,
                    (AW_HTTPSRV_CFG_HTTPS_DEFAULT_SRV_CNT * \
                     AW_HTTPSRV_CFG_HTTPS_DEFAULT_SES_CNT));
#endif

/*******************************************************************************
  functions
*******************************************************************************/

/**
 * \brief   初始化一个HTTP监听任务并启动任务
 *
 * \param[in]  thread     ： 任务入口函数
 * \param[in]  arg        ： 任务入口函数参数
 * \param[in]  prio       ： 任务优先级
 * \param[out] id         ： 监听任务序列号
 *
 * \return 任务id
 */
sys_thread_t httpsr_http_thread_new_start (uint32_t       *p_id,
                                           lwip_thread_fn  thread,
                                           void           *arg,
                                           int             prio)
{

    int i;
    aw_task_id_t task_id = AW_TASK_ID_INVALID;

    for (i = 0; i < (AW_HTTPSRV_CFG_HTTP_DEFAULT_SRV_CNT); i++) {
        aw_task_id_t id = __TASK_OBJ_NAME(server)[i].id;
        if (AW_TASK_ID_INVALID == id) {
            task_id = AW_TASK_INIT(__TASK_OBJ_NAME(server)[i].__TASK_TASK_NAME(server),
                                   AW_HTTPSRV_THREAD_NAME,
                                   prio,
                                   AW_HTTPSRV_THREAD_SERVER_STACKSIZE,
                                   thread,
                                   arg);
            HTTPSRV_DBGF("http_new_server(%d, %p, %d)\n", i, task_id, prio);
            __TASK_OBJ_NAME(server)[i].id = task_id;
            *p_id = i;
            AW_TASK_STARTUP(__TASK_OBJ_NAME(server)[i].__TASK_TASK_NAME(server));
            break;
        }
    }
    return task_id;
}

/**
 * \brief   初始化一个HTTPS监听任务并启动任务
 *
 * \param[in]  thread     ： 任务入口函数
 * \param[in]  arg        ： 任务入口函数参数
 * \param[in]  prio       ： 任务优先级
 * \param[out] id         ： 监听任务序列号
 *
 * \return 任务id
 */
sys_thread_t httpsr_https_thread_new_start (uint32_t       *p_id,
                                            lwip_thread_fn  thread,
                                            void           *arg,
                                            int             prio)
{

    int i;
    aw_task_id_t task_id = AW_TASK_ID_INVALID;
    uint32_t offset = AW_HTTPSRV_CFG_HTTP_DEFAULT_SRV_CNT;


    for (i = 0; i < (AW_HTTPSRV_CFG_HTTPS_DEFAULT_SRV_CNT); i++) {
        aw_task_id_t id = __TASK_OBJ_NAME(server)[i + offset].id;
        if (AW_TASK_ID_INVALID == id) {
            task_id = AW_TASK_INIT(__TASK_OBJ_NAME(server)[i + offset].__TASK_TASK_NAME(server),
                                   AW_HTTPSSRV_THREAD_NAME,
                                   prio,
                                   AW_HTTPSRV_THREAD_SERVER_STACKSIZE,
                                   thread,
                                   arg);
            HTTPSRV_DBGF("https_new_server(%d, %p, %d)\n", 
                         i + offset, task_id, prio);
            __TASK_OBJ_NAME(server)[i + offset].id = task_id;
            *p_id = i;
            AW_TASK_STARTUP(__TASK_OBJ_NAME(server)[i + offset].__TASK_TASK_NAME(server));
            break;
        }
    }
    return task_id;
}

/**
 * \brief    初始化一个HTTP会话任务并启动任务
 *
 * \param[in]  id         ： 监听任务序列号
 * \param[in]  thread     ： 任务入口函数
 * \param[in]  arg        ： 任务入口函数参数
 * \param[in]  prio       ： 任务优先级
 * \param[out]
 *
 * \return 任务id
 */
sys_thread_t httpsrv_http_ses_thread_new_start (uint32_t        id,
                                                lwip_thread_fn  thread,
                                                void           *arg,
                                                int             prio)
{
#if AW_HTTPSRV_CFG_HTTP_DEFAULT_SRV_CNT
    int i;
    aw_task_id_t task_id = AW_TASK_ID_INVALID;

    uint32_t offset = AW_HTTPSRV_CFG_HTTP_DEFAULT_SES_CNT * id;

    for (i = 0; i < (AW_HTTPSRV_CFG_HTTP_DEFAULT_SES_CNT); i++) {
        aw_task_id_t id = __TASK_OBJ_NAME(http_ses)[i + offset].id;
        if (AW_TASK_ID_INVALID == id) {
            task_id = AW_TASK_INIT(__TASK_OBJ_NAME(http_ses)[i + offset].__TASK_TASK_NAME(http_ses),
                                   AW_HTTPSRV_SESSION_THREAD_NAME,
                                   prio,
                                   AW_HTTPSRV_SESSION_THREAD_STACKSIZE,
                                   thread,
                                   arg);
            HTTPSRV_DBGF("http_new_http_ses(%d, %p, %d)\n", i, task_id, prio);
            __TASK_OBJ_NAME(http_ses)[i + offset].id = task_id;
            AW_TASK_STARTUP(__TASK_OBJ_NAME(http_ses)[i + offset].__TASK_TASK_NAME(http_ses));

            break;
        }
    }

    return task_id;
#else
    return AW_TASK_ID_INVALID;
#endif
}

/**
 * \brief   初始化一个HTTPS会话任务并启动任务
 *
 * \param[in]  id         ： 监听任务序列号
 * \param[in]  thread     ： 任务入口函数
 * \param[in]  arg        ： 任务入口函数参数
 * \param[in]  prio       ： 任务优先级
 * \param[out]
 *
 * \return 任务id
 */
sys_thread_t httpsrv_https_ses_thread_new_start (uint32_t        id,
                                                 lwip_thread_fn  thread,
                                                 void           *arg,
                                                 int             prio)
{

#if AW_HTTPSRV_CFG_HTTPS_DEFAULT_SRV_CNT
    aw_task_id_t task_id = AW_TASK_ID_INVALID;

    int i;
    uint32_t offset = AW_HTTPSRV_CFG_HTTPS_DEFAULT_SES_CNT * id;

    for (i = 0; i < (AW_HTTPSRV_CFG_HTTPS_DEFAULT_SES_CNT); i++) {
        aw_task_id_t id = __TASK_OBJ_NAME(https_ses)[i + offset].id;
        if (AW_TASK_ID_INVALID == id) {
            task_id = AW_TASK_INIT(__TASK_OBJ_NAME(https_ses)[i + offset].__TASK_TASK_NAME(https_ses),
                                   AW_HTTPSSRV_SESSION_THREAD_NAME,
                                   prio,
                                   AW_HTTPSRV_CFG_HTTPS_SESSION_STACK_SIZE,
                                   thread,
                                   arg);
            HTTPSRV_DBGF("http_new_https_ses(%d, %p, %d)\n", i, task_id, prio);
            AW_TASK_STARTUP(__TASK_OBJ_NAME(https_ses)[i + offset].__TASK_TASK_NAME(https_ses));
            __TASK_OBJ_NAME(https_ses)[i + offset].id = task_id;
            break;
        }
    }

    return task_id;
#else
    return AW_TASK_ID_INVALID;
#endif
}

void httpsrv_thread_del(sys_thread_t id)
{
    uint8_t i;

    if (id == 0) {
        return;
    }

    for (i = 0; i < (__HTTPSRV_CFG_DEFAULT_SRV_CNT); i++) {
        if (__TASK_OBJ_NAME(server)[i].id == id) {
            HTTPSRV_DBGF("del thread %p, i = %d\n", id, i);
            __TASK_OBJ_NAME(server)[i].id = AW_TASK_ID_INVALID;
            aw_task_terminate(id);
            break;
        }
    }
}


void httpsrv_http_ses_thread_del(sys_thread_t id)
{
#if AW_HTTPSRV_CFG_HTTP_DEFAULT_SRV_CNT
    uint8_t i;

    if (id == 0) {
        return;
    }

    for (i = 0; i < (AW_HTTPSRV_CFG_HTTP_DEFAULT_SES_CNT); i++) {
        if (__TASK_OBJ_NAME(http_ses)[i].id == id) {
            HTTPSRV_DBGF("del thread %p, i = %d\n", id, i);
            __TASK_OBJ_NAME(http_ses)[i].id = AW_TASK_ID_INVALID;
            aw_task_terminate(id);
            break;
        }
    }
#endif
}

void httpsrv_https_ses_thread_del(sys_thread_t id)
{
#if AW_HTTPSRV_CFG_HTTPS_DEFAULT_SRV_CNT
    uint8_t i;

    if (id == 0) {
        return;
    }

    for (i = 0; i < (AW_HTTPSRV_CFG_HTTPS_DEFAULT_SES_CNT); i++) {
        if (__TASK_OBJ_NAME(https_ses)[i].id == id) {
            HTTPSRV_DBGF("del thread %p, i = %d\n", id, i);
            __TASK_OBJ_NAME(https_ses)[i].id = AW_TASK_ID_INVALID;
            aw_task_terminate(id);
            break;
        }
    }
#endif
}




void aw_httpsrv_lib_init(void)
{
    httpsrv_thread_http_session_num_init(AW_HTTPSRV_CFG_HTTP_DEFAULT_SES_CNT);
    httpsrv_thread_https_session_num_init(AW_HTTPSRV_CFG_HTTPS_DEFAULT_SES_CNT);
    httpsrv_thread_prio_init(AW_HTTPSRV_CFG_DEFAULT_PRIORITY);
    httpsrv_keepalive_init(AW_HTTPSRV_CFG_KEEPALIVE_ENABLE,
                           AW_HTTPSRV_CFG_KEEPALIVE_TIMEOUT);
    httpsrv_socket_opt_timeout_init(AW_HTTPSRV_CFG_SEND_TIMEOUT,
                                    AW_HTTPSRV_CFG_RECEIVE_TIMEOUT);
    httpsrv_ses_timeout_init(AW_HTTPSRV_CFG_SES_TIMEOUT);
    httpsrv_url_len_init(AW_HTTPSRV_CFG_DEFAULT_URL_LEN);
    httpsrv_ses_buffer_size_init(AW_HTTPSRV_CFG_SES_BUFFER_SIZE);
}

/** \brief 取消相关配置宏定义  */
#undef AW_HTTPSRV_CFG_HTTP_DEFAULT_SRV_CNT
#undef AW_HTTPSRV_CFG_HTTPS_DEFAULT_SRV_CNT
#undef AW_HTTPSRV_CFG_DEFAULT_SRV_CNT
#undef AW_HTTPSRV_CFG_DEFAULT_SES_CNT
#undef AW_HTTPSRV_CFG_SERVER_STACK_SIZE
#undef AW_HTTPSRV_CFG_HTTP_SESSION_STACK_SIZE
#undef AW_HTTPSRV_CFG_HTTPS_SESSION_STACK_SIZE
#undef AW_HTTPSRV_CFG_DEFAULT_PRIORITY
#undef AW_HTTPSRV_CFG_KEEPALIVE_ENABLED
#undef AW_HTTPSRV_CFG_KEEPALIVE_TIMEOUT
#undef AW_HTTPSRV_CFG_SES_TIMEOUT
#undef AW_HTTPSRV_CFG_SEND_TIMEOUT
#undef AW_HTTPSRV_CFG_RECEIVE_TIMEOUT
#undef AW_HTTPSRV_CFG_DEFAULT_URL_LEN
#undef AW_HTTPSRV_CFG_SES_BUFFER_SIZE
#undef __TASK_TYPE_NAME
#undef __TASK_TASK_NAME
#undef __TASK_OBJ_NAME
#undef __HTTPSRV_TASK_DECL

#undef HTTPSRV_DBGF
/* end of file */
