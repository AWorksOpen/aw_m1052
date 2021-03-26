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
 * \file aw_logLib.c
 * \brief log message library
 * \note
 *
 * \internal
 * \par modification history:
 * - 1.00 14-11-2  win&bob, first implementation
 * - 1.10 17-3-15  win, add new features.
 * \endinternal
 */
 
/*******************************************************************************
  headers
*******************************************************************************/
#include "apollo.h"
#include "aw_sem.h"
#include "aw_int.h"
#include "aw_vdebug.h"
#include "aw_loglib.h"
#include "aw_delay.h"

#include <stdlib.h>
#include <string.h>

/*******************************************************************************
  defines
*******************************************************************************/

/* filter buffer size */
#ifndef AW_LOG_FILTER_MAX_LEN
#define AW_LOG_FILTER_MAX_LEN       32
#endif

#ifndef AW_LOG_CODE_LINE_NUM
#define AW_LOG_CODE_LINE_NUM        8
#endif

/* output newline sign */
#ifndef AW_LOG_NEWLINE_SIGN
#define AW_LOG_NEWLINE_SIGN         "\n"
#endif

#define __GOTO_EXIT_IF(cond, ret_val)  if ((cond)) \
                                        {AW_INFOF(("[ ERROR ][%s:%d] " #cond "\n", \
                                         __func__, __LINE__)); \
                                         ret = (ret_val); \
                                         goto exit; }
#define __RETURN_IF(cond, ret_exp)  if ((cond)) \
                                        {AW_INFOF(("[ ERROR ][%s:%d] " #cond "\n", \
                                         __func__, \
                                         __LINE__)); \
                                         return (ret_exp); }
#define __EXIT_IF(cond)  if ((cond)) \
                            {AW_INFOF(("[ ERROR ][%s:%d] " #cond "\n",\
                             __func__, \
                             __LINE__)); \
                             return; }

/*******************************************************************************
  typedefs
*******************************************************************************/

/** \brief output log's filter */
typedef struct __log_filter {
    aw_log_level_t level;                        /* level */
    char    tag[AW_LOG_FILTER_MAX_LEN + 1];      /* tag */
    char    keyword[AW_LOG_FILTER_MAX_LEN + 1];  /* keyword */
} __log_filter_t;

/** \brief log manager */
typedef struct __log_manger { 
    __log_filter_t       filter;
    size_t               enabled_fmt_set[AW_LOG_LVL_TOTAL_NUM];
    AW_MUTEX_DECL(       mutex);
    aw_task_id_t         log_task_id;
    aw_msgq_id_t         msgq_id;
    uint32_t             log_msg_lost;
    struct aw_list_head  list_logger;
    aw_bool_t               inited;
    aw_bool_t               enabled;
    char                *p_logbuf;
    size_t               logbuf_size;

} __log_manger_t;

/** \brief log message */
typedef struct __log_msg {
    aw_task_id_t  id;           /* task id who sent the message */
    char         *p_fmt;        /* message format string */
    int           arg[6];       /* message arguments */
    int           log_id;
} __log_msg_t;

/** \brief extend log message */
typedef struct __log_msg_ex {
    __log_msg_t         msg;          /* format information */

    aw_bool_t              is_raw;    /* raw data flag */
    aw_log_level_t      level;        /* message levle */
    const char          *p_tag;       /* message tag */
    const char          *p_file;      /* message belong to file name */
    const char          *p_func;      /* message call function name */
    long                line;         /* message call code line */
} __log_msg_ex_t;

/** \brief level output info */
static const char *__g_log_lvl_info[] = {
    [AW_LOG_LVL_ASSERT]  = "A/",
    [AW_LOG_LVL_ERROR]   = "E/",
    [AW_LOG_LVL_WARN]    = "W/",
    [AW_LOG_LVL_INFO]    = "I/",
    [AW_LOG_LVL_DEBUG]   = "D/",
    [AW_LOG_LVL_VERBOSE] = "V/",
};

/*******************************************************************************
  locals
*******************************************************************************/

static __log_manger_t   __g_manger; /**< \brief for log manager */

/*******************************************************************************
  forward declaration
*******************************************************************************/

extern const char *aw_loglib_time_get (void);
extern const char *aw_loglib_task_info_get (aw_task_id_t id);

static void __log_lprintf (__log_msg_t *p_msg);
static void __log_msg_ex_lprintf (__log_msg_ex_t *p_msg_ex);

/*******************************************************************************
  implementation
*******************************************************************************/

/*******************************************************************************/
aw_err_t aw_logger_register(aw_logger_t            *p_logger,
                            int                     id,
                            int                     tim)
{
    int ret = AW_OK;

    if (p_logger == NULL) {
        return -AW_EINVAL;
    }

    if (p_logger->print == NULL) {
       return -AW_ENOTSUP;
    }

    if (!__g_manger.inited ) {
        return -AW_EAGAIN;
    }
    AW_MUTEX_LOCK(__g_manger.mutex, AW_SEM_WAIT_FOREVER); 

    AW_MUTEX_INIT(p_logger->mutex_lock, AW_SEM_Q_PRIORITY);

    AW_MUTEX_LOCK(p_logger->mutex_lock, AW_SEM_WAIT_FOREVER);
    if (p_logger->reg_flag) {
        ret = AW_OK;
        goto exit;
    }

    /* initialize logger */
    p_logger->id        = id;
    p_logger->tim       = tim;
    p_logger->log_size  = 0;
    aw_list_head_init(&p_logger->node);
    
    /* register logger to log manager */
    aw_list_add_tail(&p_logger->node, &__g_manger.list_logger);
    
    if (p_logger->pfn_init ) {
        ret = p_logger->pfn_init(p_logger);
    }

    if (ret == AW_OK) {
        p_logger->reg_flag = AW_TRUE;
    }

exit:
    AW_MUTEX_UNLOCK(p_logger->mutex_lock);
    AW_MUTEX_UNLOCK(__g_manger.mutex);
    return ret;
}

/*******************************************************************************/
aw_err_t aw_logger_unregister(aw_logger_t *p_logger)
{
    int ret = AW_OK;

    if(p_logger == NULL) {
        return -AW_EINVAL;
    }
    
    AW_MUTEX_LOCK(__g_manger.mutex, AW_SEM_WAIT_FOREVER); 
    AW_MUTEX_LOCK(p_logger->mutex_lock, AW_SEM_WAIT_FOREVER);
    if ( !p_logger->reg_flag ) {
        ret = AW_OK;
        goto exit;
    }

    aw_list_del(&p_logger->node);
    
    if (p_logger->pfn_deinit ) {
        ret = p_logger->pfn_deinit(p_logger);
    }

    if (ret == AW_OK) {
        p_logger->reg_flag = AW_FALSE;
        p_logger->log_size = 0;
    }

exit:
    AW_MUTEX_UNLOCK(p_logger->mutex_lock);
    AW_MUTEX_UNLOCK(__g_manger.mutex);
    return ret;
}

/*******************************************************************************/
aw_err_t aw_log_on (void)
{
    AW_MUTEX_LOCK(__g_manger.mutex, AW_SEM_WAIT_FOREVER);
    __g_manger.enabled = AW_TRUE;
    AW_MUTEX_UNLOCK(__g_manger.mutex);

    return AW_OK;
}

/*******************************************************************************/
aw_err_t aw_log_off (void)
{
    AW_MUTEX_LOCK(__g_manger.mutex, AW_SEM_WAIT_FOREVER);
    __g_manger.enabled = AW_FALSE;
    AW_MUTEX_UNLOCK(__g_manger.mutex);

    return AW_OK;
}

/*******************************************************************************/
aw_err_t aw_log_fmt_set (aw_log_level_t level, size_t set)
{
    if (level > AW_LOG_LVL_VERBOSE) {
        return -AW_ENOTSUP;
    }

    AW_MUTEX_LOCK(__g_manger.mutex, AW_SEM_WAIT_FOREVER);
    __g_manger.enabled_fmt_set[level] = set;
    AW_MUTEX_UNLOCK(__g_manger.mutex);

    return AW_OK;
}

/*******************************************************************************/
aw_err_t aw_log_filter_set (aw_log_level_t level, const char *p_tag, const char *p_keyword)
{
    aw_err_t ret;

    ret = aw_log_filter_lvl_set(level);
    if (ret != AW_OK) {
        return ret;
    }

    /* tag 和 keyword为空应该也是可以接受得，不必返回错误 */
    aw_log_filter_tag_set(p_tag);
    aw_log_filter_keyword_set(p_keyword);

    return ret;
}

/*******************************************************************************/
aw_err_t aw_log_filter_lvl_set (aw_log_level_t level)
{
    if (level > AW_LOG_LVL_VERBOSE) {
        return -AW_ENOTSUP;
    }

    AW_MUTEX_LOCK(__g_manger.mutex, AW_SEM_WAIT_FOREVER);
    __g_manger.filter.level = level;
    AW_MUTEX_UNLOCK(__g_manger.mutex);

    return AW_OK;
}

/*******************************************************************************/
aw_err_t aw_log_filter_tag_set (const char *p_tag)
{
    size_t len;

    if (p_tag == NULL) {
        return -AW_EINVAL;
    }

    len = strlen(p_tag);
    if (len > AW_LOG_FILTER_MAX_LEN) {
        len = AW_LOG_FILTER_MAX_LEN;
    }

    AW_MUTEX_LOCK(__g_manger.mutex, AW_SEM_WAIT_FOREVER);
    memset(__g_manger.filter.tag, 0, AW_LOG_FILTER_MAX_LEN);
    strncpy(__g_manger.filter.tag, p_tag, len);
    AW_MUTEX_UNLOCK(__g_manger.mutex);

    return AW_OK;
}

/*******************************************************************************/
aw_err_t aw_log_filter_keyword_set (const char *p_keyword)
{
    size_t len;

    if (p_keyword == NULL) {
        return -AW_EINVAL;
    }

    len = strlen(p_keyword);
    if (len > AW_LOG_FILTER_MAX_LEN) {
        len = AW_LOG_FILTER_MAX_LEN;
    }

    AW_MUTEX_LOCK(__g_manger.mutex, AW_SEM_WAIT_FOREVER);
    memset(__g_manger.filter.keyword, 0, AW_LOG_FILTER_MAX_LEN);
    strncpy(__g_manger.filter.keyword, p_keyword, len);
    AW_MUTEX_UNLOCK(__g_manger.mutex);

    return AW_OK;
}

/*******************************************************************************/
aw_err_t aw_log_msg (int   id,
                     char *p_fmt,
                     int   arg1,
                     int   arg2,
                     int   arg3,
                     int   arg4,
                     int   arg5,
                     int   arg6)
{
    __log_msg_ex_t msg_ex;
    aw_err_t       ret;

    if (!__g_manger.inited) {
        return -AW_EAGAIN;
    }

    AW_MUTEX_LOCK(__g_manger.mutex, AW_SEM_WAIT_FOREVER); 
    if (!__g_manger.enabled) {
        AW_MUTEX_UNLOCK(__g_manger.mutex);
        return -AW_EPERM;
    }

    if (AW_INT_CONTEXT()) {
        /* message from interrupt use id -1 */
        msg_ex.msg.id  = (aw_task_id_t)(-1);
    } else {
        msg_ex.msg.id  = aw_task_id_self();
    }

    msg_ex.msg.p_fmt  = p_fmt;
    msg_ex.msg.arg[0] = arg1;
    msg_ex.msg.arg[1] = arg2;
    msg_ex.msg.arg[2] = arg3;
    msg_ex.msg.arg[3] = arg4;
    msg_ex.msg.arg[4] = arg5;
    msg_ex.msg.arg[5] = arg6;
    msg_ex.msg.log_id = id;

    msg_ex.is_raw = AW_TRUE;

    ret = aw_msgq_send(__g_manger.msgq_id,
                       &msg_ex,
                       sizeof(msg_ex),
                       AW_MSGQ_NO_WAIT,
                       AW_MSGQ_PRI_NORMAL);
    
    if (ret != AW_OK) {
        AW_MUTEX_UNLOCK(__g_manger.mutex);
        ++__g_manger.log_msg_lost;
        return ret;
    }
    AW_MUTEX_UNLOCK(__g_manger.mutex);

    return AW_OK;
}

/*******************************************************************************/
aw_err_t aw_log_msg_ex (int                  id,
                        aw_log_level_t       level,
                        const char          *p_tag,
                        const char          *p_file,
                        const char          *p_func,
                        const long           line,
                        char                *p_fmt,
                        int                  arg1,
                        int                  arg2,
                        int                  arg3,
                        int                  arg4,
                        int                  arg5,
                        int                  arg6)
{
    __log_msg_ex_t msg_ex;
    aw_err_t       ret;

    if (!__g_manger.inited) {
        return -AW_EAGAIN;
    }

    AW_MUTEX_LOCK(__g_manger.mutex, AW_SEM_WAIT_FOREVER); 
    if (!__g_manger.enabled) {
        AW_MUTEX_UNLOCK(__g_manger.mutex);
        return -AW_EPERM;
    }
    
    if (level > __g_manger.filter.level) {
        AW_MUTEX_UNLOCK(__g_manger.mutex);
        return -AW_EPERM;
    }


//    if (!p_tag || !strstr(p_tag, __g_manger.filter.tag)) {
//        AW_MUTEX_UNLOCK(__g_manger.mutex);
//        return -AW_EPERM;
//    }

    if (__g_manger.filter.tag[0] != 0 && strstr(p_tag, __g_manger.filter.tag) == NULL) {
        AW_MUTEX_UNLOCK(__g_manger.mutex);
        return -AW_EPERM;
    }

    msg_ex.is_raw = AW_FALSE;
    msg_ex.level  = level;
    msg_ex.p_tag  = p_tag;
    msg_ex.p_file = p_file;
    msg_ex.p_func = p_func;
    msg_ex.line   = line;

    if (AW_INT_CONTEXT()) {
        /* message from interrupt use id -1 */
        msg_ex.msg.id  = (aw_task_id_t)(-1);
    } else {
        msg_ex.msg.id  = aw_task_id_self();
    }

    msg_ex.msg.p_fmt  = p_fmt;
    msg_ex.msg.arg[0] = arg1;
    msg_ex.msg.arg[1] = arg2;
    msg_ex.msg.arg[2] = arg3;
    msg_ex.msg.arg[3] = arg4;
    msg_ex.msg.arg[4] = arg5;
    msg_ex.msg.arg[5] = arg6;
    msg_ex.msg.log_id = id;

    ret = aw_msgq_send(__g_manger.msgq_id,
                       &msg_ex,
                       sizeof(msg_ex),
                       AW_MSGQ_NO_WAIT,
                       AW_MSGQ_PRI_NORMAL);

    if (ret != AW_OK) {
        AW_MUTEX_UNLOCK(__g_manger.mutex);
        ++__g_manger.log_msg_lost;
        return ret;
    }
    AW_MUTEX_UNLOCK(__g_manger.mutex);

    return AW_OK;
}

aw_err_t aw_log_msg_ex_sync (int             id,
                             aw_log_level_t  level,
                             const char     *p_tag,
                             const char     *p_file,
                             const char     *p_func,
                             const long      line,
                             char           *p_fmt,
                             int             arg1,
                             int             arg2,
                             int             arg3,
                             int             arg4,
                             int             arg5,
                             int             arg6)
{
    __log_msg_ex_t  msg_ex;
    aw_err_t        ret;
    static int      s_old_msgs_lost = 0;
    int             new_msgs_lost   = 0;

    if (!__g_manger.inited) {
        return -AW_EAGAIN;
    }

    AW_MUTEX_LOCK(__g_manger.mutex, AW_SEM_WAIT_FOREVER); 
    if (!__g_manger.enabled) {
        AW_MUTEX_UNLOCK(__g_manger.mutex);
        return -AW_EPERM;
    }

    if (level > __g_manger.filter.level) {
        AW_MUTEX_UNLOCK(__g_manger.mutex);
        return -AW_EPERM;
    }

    if (__g_manger.filter.tag[0] != 0 && strstr(p_tag, __g_manger.filter.tag) == NULL) {
        AW_MUTEX_UNLOCK(__g_manger.mutex);
        return -AW_EPERM;
    }

    msg_ex.is_raw = AW_FALSE;
    msg_ex.level  = level;
    msg_ex.p_tag  = p_tag;
    msg_ex.p_file = p_file;
    msg_ex.p_func = p_func;
    msg_ex.line   = line;

    if (AW_INT_CONTEXT()) {
        /* message from interrupt use id -1 */
        msg_ex.msg.id  = (aw_task_id_t)(-1);
    } else {
        msg_ex.msg.id  = aw_task_id_self();
    }

    msg_ex.msg.p_fmt  = p_fmt;
    msg_ex.msg.arg[0] = arg1;
    msg_ex.msg.arg[1] = arg2;
    msg_ex.msg.arg[2] = arg3;
    msg_ex.msg.arg[3] = arg4;
    msg_ex.msg.arg[4] = arg5;
    msg_ex.msg.arg[5] = arg6;
    msg_ex.msg.log_id = id;

    if (!msg_ex.is_raw) {
        __log_msg_ex_lprintf(&msg_ex);
    } else {
        __log_lprintf(&msg_ex.msg);
    }

    new_msgs_lost = __g_manger.log_msg_lost;
    /* 打印丢失消息的个数 */
    if (new_msgs_lost != s_old_msgs_lost) {
        msg_ex.msg.p_fmt  = "logTask: %d log messages lost.";
        msg_ex.msg.arg[0] = new_msgs_lost - s_old_msgs_lost;
        __log_lprintf(&msg_ex.msg);
        s_old_msgs_lost = new_msgs_lost;
    }
    AW_MUTEX_UNLOCK(__g_manger.mutex);
    return AW_OK;
}
/*******************************************************************************/
void aw_loglib_task (void *p_arg)
{
    static int      s_old_msgs_lost = 0;
    int             new_msgs_lost   = 0;
    __log_msg_ex_t  msg_ex;
    aw_err_t        ret;
  
    AW_FOREVER{
      
        /* waiting for log messages */
        ret = aw_msgq_receive(__g_manger.msgq_id,
                              &msg_ex,
                              sizeof(msg_ex),
                              AW_MSGQ_WAIT_FOREVER);

        AW_MUTEX_LOCK(__g_manger.mutex, AW_SEM_WAIT_FOREVER);

        /* message receive failed */
        if (ret != AW_OK) {
            msg_ex.msg.p_fmt = "logTask: error reading log messages.";
            __log_lprintf(&msg_ex.msg);

        /* message received */
        } else {

            if (!msg_ex.is_raw) {
                __log_msg_ex_lprintf(&msg_ex);
            } else {
                __log_lprintf(&msg_ex.msg);
            }
        }  

        new_msgs_lost = __g_manger.log_msg_lost;
        /* 打印丢失消息的个数 */
        if (new_msgs_lost != s_old_msgs_lost) {
            msg_ex.msg.p_fmt  = "logTask: %d log messages lost.";
            msg_ex.msg.arg[0] = new_msgs_lost - s_old_msgs_lost;
            __log_lprintf(&msg_ex.msg);
            s_old_msgs_lost = new_msgs_lost;
        }

        AW_MUTEX_UNLOCK(__g_manger.mutex);
    }
}

/**
 * \brief another copy string function
 *
 * \param[in] cur_len : current copied log length, max size is ELOG_LINE_BUF_SIZE
 * \param[in] dst     : destination
 * \param[in] src     : source
 *
 * \return copied length
 */
static size_t __log_strcpy (size_t cur_len, char *p_dst, const char *p_src) {
    const char *p_src_old = p_src;

    if (p_dst == NULL || p_src == NULL) {
        return 0;
    }

    while (*p_src != 0) {
        /* make sure destination has enough space */
        if (cur_len++ < __g_manger.logbuf_size) {
            *p_dst++ = *p_src++;
        } else {
            break;
        }
    }
    return p_src - p_src_old;
}

/**
 * \brief 格式字符串输出到每个设备和文件
 * \param[in] p_msg  format message(raw)
 * \retval    无
 */
static void __log_lprintf (__log_msg_t *p_msg)
{
    aw_logger_t  *p_logger = NULL;
    int           str_len  = 0;
    size_t        new_line_len = strlen(AW_LOG_NEWLINE_SIGN);

    /* 清空之前的缓存消息 */
    memset(__g_manger.p_logbuf, 0, __g_manger.logbuf_size);

    if (p_msg->p_fmt == NULL) {
        p_msg->p_fmt = "<null \"fmt\" parameter>";
    }

    str_len = aw_snprintf(__g_manger.p_logbuf,
                          __g_manger.logbuf_size,
                          p_msg->p_fmt,
                          p_msg->arg[0],
                          p_msg->arg[1],
                          p_msg->arg[2],
                          p_msg->arg[3],
                          p_msg->arg[4],
                          p_msg->arg[5]);

    /* package newline sign */
    if ((str_len > -1) && ((str_len + new_line_len) <= __g_manger.logbuf_size)) {
        str_len += __log_strcpy(str_len, 
                                __g_manger.p_logbuf + str_len, 
                                AW_LOG_NEWLINE_SIGN);
    } else {
        str_len = __g_manger.logbuf_size;

        /* copy newline sign */
        strncpy(__g_manger.p_logbuf + str_len - new_line_len, 
                AW_LOG_NEWLINE_SIGN, 
                new_line_len);
    }

    aw_list_for_each_entry(p_logger, &(__g_manger.list_logger), aw_logger_t, node) {
        if (p_msg->log_id == p_logger->id) {
            AW_MUTEX_LOCK(p_logger->mutex_lock, AW_SEM_WAIT_FOREVER);
            p_logger->print(p_logger, __g_manger.p_logbuf, str_len);
            AW_MUTEX_UNLOCK(p_logger->mutex_lock);
        }
    }

}

/**
 * \brief get format enabled
 *
 * \param[in] level level
 * \param[in] set format set
 *
 * \return enable or disable
 */
static aw_bool_t __log_fmt_enabled_get (aw_log_level_t level, size_t set) {

    if (__g_manger.enabled_fmt_set[level] & set) {
        return AW_TRUE;
    } else {
        return AW_FALSE;
    }
}

/**
 * \brief 扩展格式字符串输出到每个设备和文件
 * \param[in] p_msg  extend format message
 * \retval    无
 */
static void __log_msg_ex_lprintf (__log_msg_ex_t *p_msg_ex)
{
    aw_logger_t  *p_logger = NULL;
    size_t        log_len  = 0;
    int           fmt_len  = 0;
    const char   *p_tmp;
    char          line_num[AW_LOG_CODE_LINE_NUM + 1] = {0};

    size_t        new_line_len = strlen(AW_LOG_NEWLINE_SIGN);

    /* 清空之前的缓存消息 */
    memset(__g_manger.p_logbuf, 0, __g_manger.logbuf_size);

    if (__log_fmt_enabled_get(p_msg_ex->level, AW_LOG_FMT_LVL)) {
        log_len += __log_strcpy(log_len, 
                                __g_manger.p_logbuf + log_len, 
                                __g_log_lvl_info[p_msg_ex->level]);
    }

    if (__log_fmt_enabled_get(p_msg_ex->level, AW_LOG_FMT_TAG)) {
        log_len += __log_strcpy(log_len, 
                                __g_manger.p_logbuf + log_len, 
                                p_msg_ex->p_tag);
        log_len += __log_strcpy(log_len, 
                                __g_manger.p_logbuf + log_len,
                                " ");
    }

    if (__log_fmt_enabled_get(p_msg_ex->level, 
                              AW_LOG_FMT_TIME | AW_LOG_FMT_TASK_INFO)) {
        log_len += __log_strcpy(log_len, __g_manger.p_logbuf + log_len, "[");

        /* time info */
        if (__log_fmt_enabled_get(p_msg_ex->level, AW_LOG_FMT_TIME)) {

            /* 获取时间的函数在库外实现，库内引用 */
            p_tmp = aw_loglib_time_get();
            log_len += __log_strcpy(log_len, 
                                    __g_manger.p_logbuf + log_len, 
                                    p_tmp);

            if (__log_fmt_enabled_get(p_msg_ex->level, AW_LOG_FMT_TASK_INFO)) {
                log_len += __log_strcpy(log_len, __g_manger.p_logbuf + log_len, " ");
            }
        }

        if (__log_fmt_enabled_get(p_msg_ex->level, AW_LOG_FMT_TASK_INFO)) {

            /* 获取任务信息的函数在库外实现，库内引用 */
            p_tmp = aw_loglib_task_info_get(p_msg_ex->msg.id);
            log_len += __log_strcpy(log_len, __g_manger.p_logbuf + log_len, p_tmp);
        }

        log_len += __log_strcpy(log_len, __g_manger.p_logbuf + log_len, "]");
    }

    /* package file directory and name, function name and line number info */
    if (__log_fmt_enabled_get(p_msg_ex->level, 
                              AW_LOG_FMT_DIR | AW_LOG_FMT_FUNC | AW_LOG_FMT_LINE)) {

        log_len += __log_strcpy(log_len, __g_manger.p_logbuf + log_len, "(");

        /* package directory info */
        if (__log_fmt_enabled_get(p_msg_ex->level, AW_LOG_FMT_DIR)) {

            log_len += __log_strcpy(log_len, 
                                    __g_manger.p_logbuf + log_len, 
                                    p_msg_ex->p_file);

            if (__log_fmt_enabled_get(p_msg_ex->level, AW_LOG_FMT_FUNC)) {
                log_len += __log_strcpy(log_len, __g_manger.p_logbuf + log_len, " ");
            } else if (__log_fmt_enabled_get(p_msg_ex->level, AW_LOG_FMT_LINE)) {
                log_len += __log_strcpy(log_len, __g_manger.p_logbuf + log_len, ":");
            }
        }

        /* package function info */
        if (__log_fmt_enabled_get(p_msg_ex->level, AW_LOG_FMT_FUNC)) {
            log_len += __log_strcpy(log_len, 
                                    __g_manger.p_logbuf + log_len, 
                                    p_msg_ex->p_func);
            if (__log_fmt_enabled_get(p_msg_ex->level, AW_LOG_FMT_LINE)) {
                log_len += __log_strcpy(log_len, __g_manger.p_logbuf + log_len, ":");
            }
        }

        /* package line info */
        if (__log_fmt_enabled_get(p_msg_ex->level, AW_LOG_FMT_LINE)) {
            aw_snprintf(line_num, AW_LOG_CODE_LINE_NUM, "%ld", p_msg_ex->line);
            log_len += __log_strcpy(log_len, 
                                    __g_manger.p_logbuf + log_len, 
                                    line_num);
        }

        log_len += __log_strcpy(log_len, __g_manger.p_logbuf + log_len, ")");
    }

    log_len += __log_strcpy(log_len, __g_manger.p_logbuf + log_len, ":");

    if (p_msg_ex->msg.p_fmt == NULL) {
        p_msg_ex->msg.p_fmt = "<null \"fmt\" parameter>";
    }

    /**
     * log_len的长度最大为__g_manger.logbuf_size
     */
    fmt_len = aw_snprintf(__g_manger.p_logbuf + log_len,
                          __g_manger.logbuf_size - log_len,
                          p_msg_ex->msg.p_fmt,
                          p_msg_ex->msg.arg[0],
                          p_msg_ex->msg.arg[1],
                          p_msg_ex->msg.arg[2],
                          p_msg_ex->msg.arg[3],
                          p_msg_ex->msg.arg[4],
                          p_msg_ex->msg.arg[5]);

    /* keyword filter */
    if (!strstr(__g_manger.p_logbuf, __g_manger.filter.keyword)) {
        return;
    }

    /* package newline sign */
    if ((fmt_len > -1) && ((fmt_len + log_len + new_line_len) <= __g_manger.logbuf_size)) {
        log_len += fmt_len;
        log_len += __log_strcpy(log_len, 
                                __g_manger.p_logbuf + log_len, 
                                AW_LOG_NEWLINE_SIGN);
    } else {
        log_len = __g_manger.logbuf_size;

        /* copy newline sign */
        strncpy(__g_manger.p_logbuf + log_len - new_line_len, 
                AW_LOG_NEWLINE_SIGN, 
                new_line_len);
    }

    aw_list_for_each_entry(p_logger, 
                           &(__g_manger.list_logger), 
                           aw_logger_t, node) {

        if (p_msg_ex->msg.log_id == p_logger->id) {
            AW_MUTEX_LOCK(p_logger->mutex_lock, AW_SEM_WAIT_FOREVER);
            p_logger->print(p_logger, __g_manger.p_logbuf, log_len);
            AW_MUTEX_UNLOCK(p_logger->mutex_lock);
        }
    }
}

/*******************************************************************************/
aw_err_t aw_loglib_init (aw_task_id_t  task_id,
                         aw_msgq_id_t  msgq_id,
                         char         *p_logbuf,
                         size_t        logbuf_size)
{
    aw_mutex_id_t sem_id;

    if (__g_manger.inited ) {
        return AW_OK;
    }

    /* save task id & msgq id */
    __g_manger.log_task_id = task_id;
    __g_manger.msgq_id     = msgq_id;

    /* initialize mutex used by log manager */
    sem_id = AW_MUTEX_INIT(__g_manger.mutex, AW_SEM_Q_PRIORITY);
    if (NULL == sem_id) {
        aw_msgq_terminate(__g_manger.msgq_id);
        aw_task_terminate(__g_manger.log_task_id);
        return -AW_ENOMEM;
    }

    __g_manger.p_logbuf    = p_logbuf;
    __g_manger.logbuf_size = logbuf_size;

    /* initialize logger list */
    aw_list_head_init(&(__g_manger.list_logger));

    /* loglib initialization done */
    __g_manger.inited = AW_TRUE;

    /* we can startup logger task now */
    aw_task_startup(__g_manger.log_task_id);

    /* 设置等级为详细 */
    aw_log_filter_lvl_set(AW_LOG_LVL_VERBOSE);

    /* 打开消息日志 */
    aw_log_on();

    return AW_OK;
}



aw_err_t aw_log_export (aw_logger_t       *p_logger,
                        int                offset,
                        void              *buf,
                        uint32_t           buf_size)
{
    aw_err_t ret;
    if (p_logger == NULL) {
        return -AW_EINVAL;
    }

    if (p_logger->pfn_export == NULL || (!p_logger->reg_flag) ) {
        return -AW_ENOTSUP;
    }

#if 0
    if (offset >= p_logger->log_size) {
        return -AW_EFAULT;
    }
#endif

    AW_MUTEX_LOCK(p_logger->mutex_lock, AW_SEM_WAIT_FOREVER);
    ret = p_logger->pfn_export(p_logger, offset, buf, buf_size);
    AW_MUTEX_UNLOCK(p_logger->mutex_lock);

    return ret;
}

uint32_t aw_log_data_size_get (aw_logger_t       *p_logger)
{
    uint32_t size;
    if (p_logger == NULL || (!p_logger->reg_flag) ) {
        return 0;
    }
    AW_MUTEX_LOCK(p_logger->mutex_lock, AW_SEM_WAIT_FOREVER);
    size = p_logger->log_size;
    AW_MUTEX_UNLOCK(p_logger->mutex_lock);

    return size;
}

aw_err_t aw_log_data_clear (aw_logger_t       *p_logger)
{
    aw_err_t ret;

    __RETURN_IF(p_logger == NULL, -AW_EINVAL);
    __RETURN_IF(!p_logger->reg_flag, -AW_EPERM);
    __RETURN_IF(p_logger->pfn_data_clear == NULL, -AW_ENOTSUP);

    AW_MUTEX_LOCK(p_logger->mutex_lock, AW_SEM_WAIT_FOREVER);
    ret = p_logger->pfn_data_clear(p_logger);
    AW_MUTEX_UNLOCK(p_logger->mutex_lock);

    return ret;
}

/* end of file */

