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
 * \file aw_loglib_cfg.c
 * \brief 日志消息库函数配置文件
 * 
 * \par modification history:
 * - 1.00 14-11-2  win、bob, first implementation
 * - 1.10 17-3-15  win, add new features.
 * \endinternal
 */
 
/*******************************************************************************
  headers
*******************************************************************************/
#include "aworks.h"
#include "aw_errno.h"
#include "aw_sem.h"
#include "aw_task.h"
#include "aw_msgq.h"
#include "aw_int.h"
#include "aw_list.h"
#include "aw_serial.h"
#include "aw_vdebug.h"
#include "aw_time.h"
#include "aw_loglib.h"

#include <stdlib.h>
#include <string.h>

/*******************************************************************************
  Default configure
*******************************************************************************/

/**\brief 指定堆栈大小 */
#ifndef AW_CFG_LOG_TASK_STACKSIZE
#define AW_CFG_LOG_TASK_STACKSIZE   1024*4
#endif

/**\brief 指定消息队列消息个数 */
#ifndef AW_CFG_LOG_MSQ_NUM
#define AW_CFG_LOG_MSQ_NUM          10
#endif

/** \brief 消息队列消息长度（这个值不等于用户消息的长度） */
#ifndef AW_CFG_LOG_MSG_SIZE
#define AW_CFG_LOG_MSG_SIZE         sizeof(int [16])
#endif

/**\brief 指定任务优先级*/
#ifndef AW_CFG_LOG_TASK_PRIORITY
#define AW_CFG_LOG_TASK_PRIORITY    (aw_task_lowest_priority() - 2)
#endif

/** \brief 内部消息缓冲区大小，消息最大不能超过该值，超过部分会被截断 */
#ifndef AW_CFG_LOG_LINE_BUF
#define AW_CFG_LOG_LINE_BUF         512
#endif

/*******************************************************************************
  Import functions
*******************************************************************************/
extern void aw_loglib_task(void *arg);


/*******************************************************************************
  Locals
*******************************************************************************/

/**\brief 声明一个任务 */
AW_TASK_DECL_STATIC(loglib_task, AW_CFG_LOG_TASK_STACKSIZE);

/**\brief 声明一个队列 */
AW_MSGQ_DECL_STATIC(loglib_msgq, AW_CFG_LOG_MSQ_NUM, AW_CFG_LOG_MSG_SIZE);

/** \brief 内部消息缓冲区 */
static char g_loglib_buf[AW_CFG_LOG_LINE_BUF];

/*******************************************************************************
  loglib库配置函数
*******************************************************************************/
aw_err_t aw_loglib_cfg_init (void)
{
    aw_task_id_t log_task_id;
    aw_msgq_id_t msgq_id;
        
    msgq_id = AW_MSGQ_INIT(loglib_msgq,
                           AW_CFG_LOG_MSQ_NUM,
                           AW_CFG_LOG_MSG_SIZE,
                           AW_MSGQ_Q_FIFO);

    if (msgq_id == NULL) {
        return -AW_ENOMEM;
    }
    
    log_task_id = AW_TASK_INIT(loglib_task, 
                               "log-task",
                               AW_CFG_LOG_TASK_PRIORITY, 
                               AW_CFG_LOG_TASK_STACKSIZE,
                               aw_loglib_task,
                               (void *)0);

    if (log_task_id == NULL) {
        aw_msgq_terminate(msgq_id);
        return -AW_ENOMEM;
    }
    
    aw_loglib_init(log_task_id,
                   msgq_id,
                   g_loglib_buf,
                   AW_CFG_LOG_LINE_BUF);
    
    return AW_OK;
}

/**
 * \brief get current time interface
 *
 * \return current time
 */
const char *aw_loglib_time_get (void)
{
#if 1
    static char cur_system_time[32] = { 0 };
    time_t timep;
    struct tm *p;

    aw_time(&timep);
    p = localtime(&timep);
    if (p == NULL) {
        return "";
    }
    aw_snprintf(cur_system_time, 
                sizeof(cur_system_time), 
                "%04d-%02d-%02d %02d:%02d:%02d",
                p->tm_year + 1900, p->tm_mon + 1, p->tm_mday,
                p->tm_hour, p->tm_min, p->tm_sec);

#else

    static char cur_system_time[24] = { 0 };

    aw_snprintf(cur_system_time, 24, "tick:%ld", aw_sys_tick_get());

#endif

    return cur_system_time;
}

/**
 * \brief get current thread name interface
 *
 * \return current task name or task id
 */
const char *aw_loglib_task_info_get (aw_task_id_t id)
{
    static char  cur_thread_info[32] = { 0 };
    const char  *p_task_name;

    /* message is sent from interrupt*/
    if (id == (aw_task_id_t)(-1)) {
        p_task_name = "interrupt";

        return p_task_name;

    /* message is sent from task */
    } else {

        /* print task ID&name */
        if((p_task_name = aw_task_name_get(id)) == NULL) {
            /* 发送消息的任务被回收 */
            p_task_name = "no_name";
        }

        aw_snprintf(cur_thread_info, 32, "task:%s", p_task_name);

        return cur_thread_info;
    }
}

/* end of file */
