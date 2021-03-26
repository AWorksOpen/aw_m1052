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
 * \brief FTP Server configure file.
 *
 * \internal
 * \par modification history:
 *
 * - 150812 axn, first implementation.
 * \endinternal
 */

#include "apollo.h"
#include "aw_task.h"

#include "ftpd/aw_ftpd.h"

#include "aw_vdebug.h"

/******************************************************************************/
#ifndef FTPD_MAX_SESSIONS
#define FTPD_MAX_SESSIONS 2
#endif
#ifndef FTPD_LISTEN_TASK_PRIO
#define FTPD_LISTEN_TASK_PRIO 6
#endif
#ifndef FTPD_LISTEN_TASK_STK_SIZE
#define FTPD_LISTEN_TASK_STK_SIZE 4096
#endif

#ifndef FTPD_CMD_BUF_SIZE
#define FTPD_CMD_BUF_SIZE 2920
#endif

/** \brief 同时支持的最多会话数  */
#define __FTPD_MAX_SESSIONS           FTPD_MAX_SESSIONS 

/** \brief 支持的路径最大长度      */
#define __FTPD_MAX_PATH_SIZE          256    

/** \brief 支持的最大用户名长度 */
#define __FTPD_MAX_USER_NAME_LEN      32      

/** \brief 运行缓存，应大于__FTPD_MAX_PATH_SIZE */
#define __FTPD_CMD_BUF_SIZE           FTPD_CMD_BUF_SIZE    
#if __FTPD_CMD_BUF_SIZE < __FTPD_MAX_PATH_SIZE
#error CFG ERROR: FTPD_CMD_BUF_SIZE too small
#endif

#define __FTPD_IDLE_TIMEOUT       60      /**< \brief idle timeout: 1 minutes */
#define __FTPD_RECV_TIMEOUT       60      /**< \brief recv timeout: 1 minutes */

#define __FTPD_TCP_KEEPIDLE      10    /**< \brief Idle time, 10 seconds */
#define __FTPD_TCP_KEEPINTVL     10    /**< \brief Probe interval, 10 seconds */
#define __FTPD_TCP_KEEPCNT       3     /**< \brief Probe count */

#define __FTPD_LISTEN_TASK_STK_SIZE   FTPD_LISTEN_TASK_STK_SIZE
#define __FTPD_LISTEN_TASK_PRIO       FTPD_LISTEN_TASK_PRIO

/******************************************************************************/
AW_TASK_DECL_STATIC(__g_ftpd_task_listen, __FTPD_LISTEN_TASK_STK_SIZE);

/**
 * allocation memory
 */
aw_local char  __ftpd_user_name_buf[__FTPD_MAX_SESSIONS][__FTPD_MAX_USER_NAME_LEN];
aw_local char  __ftpd_path_buf[__FTPD_MAX_SESSIONS][__FTPD_MAX_PATH_SIZE];
aw_local char  __ftpd_cmd_buf[__FTPD_MAX_SESSIONS][__FTPD_CMD_BUF_SIZE];

/* 中间缓存空间  */
aw_local char   __ftpd_temp_path_buf[__FTPD_MAX_SESSIONS][__FTPD_MAX_PATH_SIZE];
aw_local char   __ftpd_temp_cmd_buf[__FTPD_MAX_SESSIONS][__FTPD_CMD_BUF_SIZE];

aw_local aw_ftpd_session_t __ftpd_session[__FTPD_MAX_SESSIONS];
aw_local aw_ftpd_t         __g_ftpd;

aw_const aw_local aw_ftpd_config_t __ftpd_cfg_info = {

    __FTPD_MAX_SESSIONS,
    __FTPD_MAX_PATH_SIZE,
    __FTPD_MAX_USER_NAME_LEN,
    __FTPD_CMD_BUF_SIZE,

    (char *)__ftpd_user_name_buf[0],
    (char *)__ftpd_path_buf[0],
    (char *)__ftpd_cmd_buf[0],

    (char *)__ftpd_temp_path_buf[0],
    (char *)__ftpd_temp_cmd_buf[0],

    __FTPD_IDLE_TIMEOUT,
    __FTPD_RECV_TIMEOUT,

    __FTPD_TCP_KEEPIDLE,
    __FTPD_TCP_KEEPINTVL,
    __FTPD_TCP_KEEPCNT,

    &__ftpd_session[0]
};



int aw_ftpd_init (const char *root_path,
                  int         listen_port,
                  int         anonymous,
                  aw_bool_t    (*pfn_check_user) (const char *user_name,
                                                  const char *pass_word))
{
    AW_INFOF(("ftpd: root_path=%s, listening_port=%d, anonymous=%s\n",
              root_path, listen_port, anonymous ? "yes" : "no"));

    aw_ftpd_param_init(&__g_ftpd,
                       &__ftpd_cfg_info,
                        root_path, 
                        listen_port,
                        anonymous, 
                        pfn_check_user);

    __g_ftpd.sessions_stack_size = __FTPD_LISTEN_TASK_STK_SIZE;
    __g_ftpd.max_sessions  = __FTPD_MAX_SESSIONS;
    AW_TASK_INIT(__g_ftpd_task_listen,
                 "ftpd task",
                 AW_TASK_SYS_PRIORITY(__FTPD_LISTEN_TASK_PRIO),
                 __FTPD_LISTEN_TASK_STK_SIZE,
                 aw_ftpd_task_listen,
                 (void *)&__g_ftpd);

    AW_TASK_STARTUP(__g_ftpd_task_listen);

    return 0;
}

/* @end of file */

