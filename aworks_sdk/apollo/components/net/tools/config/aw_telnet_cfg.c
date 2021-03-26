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
 * @brief shell config
 *
 * @internal
 * @par History
 * - 150428, rnk, First implementation.
 * @endinternal
 */

#include "telnet/aw_telnet_shell.h"

#ifdef  AXIO_AWORKS
#include "aw_net_tools_autoconf.h"
#endif

/**< \brief telent shell 线程优先级  */
#ifdef CONFIG_TELNET_TASK_PRIO
#define AW_CFG_TELNET_TASK_PRIO        CONFIG_TELNET_TASK_PRIO
#else
#ifndef AW_CFG_TELNET_TASK_PRIO
#define AW_CFG_TELNET_TASK_PRIO        1   
#endif
#endif

/**< \brief telent shell 线程堆栈大小 */
#ifdef CONFIG_TELNET_TASK_STACK
#define AW_CFG_TELNET_TASK_STACK_SIZE        CONFIG_TELNET_TASK_STACK
#else
#ifndef AW_CFG_TELNET_TASK_STACK_SIZE
#define AW_CFG_TELNET_TASK_STACK_SIZE        8192   
#endif
#endif

/**< \brief telent shell 最大同时连接数目 */
#ifdef CONFIG_TELNET_MAX_SESSION
#define AW_CFG_TELNET_MAX_SESSION        CONFIG_TELNET_MAX_SESSION
#else
#ifndef AW_CFG_TELNET_MAX_SESSION
#define AW_CFG_TELNET_MAX_SESSION        2
#endif
#endif


/**< \brief telent shell 网络端口号 */
#ifdef CONFIG_TELNET_LISTEN_PORT
#define AW_CFG_TELNET_LISTEN_PORT        CONFIG_TELNET_LISTEN_PORT
#else
#ifndef AW_CFG_TELNET_LISTEN_PORT
#define AW_CFG_TELNET_LISTEN_PORT        23
#endif
#endif

/**< \brief telent shell 用户登入名 */
#ifdef CONFIG_TELNET_USER_NAME
#define AW_CFG_TELNET_USER_NAME        CONFIG_TELNET_USER_NAME
#else
#ifndef AW_CFG_TELNET_USER_NAME
#define AW_CFG_TELNET_USER_NAME        "admin"   
#endif
#endif

/**< \brief telent shell 用户登入密码 */
#ifdef CONFIG_TELNET_USER_PSW
#define AW_CFG_TELNET_USER_PSW        CONFIG_TELNET_USER_PSW
#else
#ifndef AW_CFG_TELNET_USER_PSW
#define AW_CFG_TELNET_USER_PSW        "123456"   
#endif
#endif


static struct aw_telnet_srv_dev __telnet_srv;
static struct aw_telnet_session __telnet_session[AW_CFG_TELNET_MAX_SESSION];
static struct aw_telnet_shell_dev __shell_dev[AW_CFG_TELNET_MAX_SESSION];

static char __g_his_mem[AW_CFG_TELNET_MAX_SESSION][80 * 100];

static struct aw_shell_opt __g_tel_shell_opt[AW_CFG_TELNET_MAX_SESSION];

AW_TASK_DECL(srv_task, AW_CFG_TELNET_TASK_STACK_SIZE);
AW_TASK_DECL(session_task, AW_CFG_TELNET_TASK_STACK_SIZE)[AW_CFG_TELNET_MAX_SESSION];
AW_TASK_DECL(ts_task, AW_CFG_TELNET_TASK_STACK_SIZE)[AW_CFG_TELNET_MAX_SESSION];

static void _init_task(aw_task_context_t *tsk,
        aw_task_id_t id,
        const char* name,
        int prio,
        char* stack,
        int stack_size,
        void *pfunc,
        void *arg)
{
    tsk->id = id;
    tsk->name = name;
    tsk->prio = prio;
    tsk->stack = stack;
    tsk->stack_size = stack_size;
    tsk->pfunc = pfunc;
    tsk->arg = arg;
}

aw_err_t aw_telnet_shell_cfg()
{
    int i =0;
    struct aw_shell_opt shell_opt = {
        .use_chart      = 1,
        .use_color      = 1,
        .use_history    = 1,
        .use_escape_seq = 0,
        .use_cmdset     = 1,
        .use_cursor     = 1,
        .use_complete   = 1,

        .p_endchar      = NULL,
        .p_desc         = "telnet->> ",
        .p_desc_color   = NULL,
        .p_his_buf      = NULL,
        .his_buf_size   = sizeof(__g_his_mem[0]),
    };

    _init_task(&__telnet_srv.tsk_cont, 
               &srv_task.tcb, 
               "telnet srv task",
               AW_CFG_TELNET_TASK_PRIO, 
               srv_task.stack, 
               AW_CFG_TELNET_TASK_STACK_SIZE,
               aw_telnet_srv_run, 
               &__telnet_srv);
    for (i = 0; i < AW_CFG_TELNET_MAX_SESSION; i++) {
        _init_task(&__telnet_session[i].tsk_cont, 
                   &session_task[i].tcb,
                   "telnet session task",
                   AW_CFG_TELNET_TASK_PRIO, 
                   session_task[i].stack, 
                   AW_CFG_TELNET_TASK_STACK_SIZE,
                   aw_telnet_session_run, 
                   &__telnet_session[i]);
        _init_task(&__shell_dev[i].tsk_cont,
                   &ts_task[i].tcb,
                   "telnet shell task",
                   AW_CFG_TELNET_TASK_PRIO,
                   ts_task[i].stack, 
                   AW_CFG_TELNET_TASK_STACK_SIZE,
                   aw_telnet_shell_run,
                   &__shell_dev[i]);

        shell_opt.p_his_buf     = __g_his_mem[i];
        __g_tel_shell_opt[i]    = shell_opt;
    }

    return aw_telnet_shell_init(&__telnet_srv, 
                                 __telnet_session,
                                 __shell_dev,
                                 AW_CFG_TELNET_MAX_SESSION, 
                                 AW_CFG_TELNET_LISTEN_PORT,
                                 AW_CFG_TELNET_USER_NAME,
                                 AW_CFG_TELNET_USER_PSW,
                                 __g_tel_shell_opt);
}

/* end of file */
