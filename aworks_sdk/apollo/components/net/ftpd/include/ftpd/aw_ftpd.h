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
 * \brief FTP server
 *
 * \internal
 * \par modification history:
 * - 1.01 15-08-12  axn, 使得FTP可配置，方便编译库后维护
 * - 1.00 13-06-27  orz, first implementation.
 * \endinternal
 */

#ifndef __AW_FTPD_H
#define __AW_FTPD_H

/**
 * \addtogroup grp_aw_if_ftpd
 * \copydoc aw_ftpd.h
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "time.h"
#include "ctype.h"
#include "aw_types.h"
#include "lwip/sockets.h"

struct aw_ftpd;

/******************************************************************************/
typedef enum aw_ftp_trans_mode {

    AW_FTP_TRANS_MODE_UNKNOWN = 0,
    AW_FTP_TRANS_MODE_ASCII   = 1,
    AW_FTP_TRANS_MODE_EBCDIC  = 2,
    AW_FTP_TRANS_MODE_BIN     = 3,
    AW_FTP_TRANS_MODE_LOCAL   = 4

} aw_ftp_trans_mode_t;


/******************************************************************************/
typedef struct aw_ftpd_session {

    aw_bool_t              login;
    aw_bool_t              use_default;
    aw_ftp_trans_mode_t trans_mode;
    struct sockaddr_in  saddrin_ctrl;
    struct sockaddr_in  saddrin_def;
    struct sockaddr_in  saddrin_data;
    struct in_addr      inaddr_remote;
    int                 sock_ctrl;
    int                 sock_pasv;
    int                 sock_data;
    time_t              time_start;
    char               *root_path; /**< \brief user root path */
    char               *path_end;  /**< \brief user path ending position */

    int                 max_path_size;
    int                 max_user_name_len;
    int                 cmd_buf_size;

    char               *user_name_buf;
    char               *path_buf;
    char               *cmd_buf;

    char               *temp_path_buf;
    char               *temp_cmd_buf;

    /**
     * TCP keep alive
     */
    int                 tcp_keepidle_s;
    int                 tcp_keepintvl_s;
    int                 tcp_keepcnt;

    struct aw_ftpd     *ftpd;

} aw_ftpd_session_t;

/******************************************************************************/
typedef struct aw_ftpd {

    int                max_sessions;
    aw_bool_t         (*check_user) (const char *name, const char *passwd);
    const char        *root_path;  /**< \brief root path of ftp server */
    aw_bool_t          anonymous;  /**< \brief anonymous user enabled or not */
    uint16_t           port;       /**< \brief server listen port */
    uint16_t           sess_cnt;   /**< \brief session count */
    struct timeval     idle_timeout;
    struct timeval     recv_timeout;

    /** \brief FTPD session pool */
    int sessions_stack_size;
    aw_ftpd_session_t   *sessions;

} aw_ftpd_t;

typedef struct aw_ftpd_config {

    int     max_sessions;

/*  int     max_file_name_len; */
    int     max_path_size;
    int     max_user_name_len;
    int     cmd_buf_size;

    char   *p_user_name_buf;
    char   *p_path_buf;
    char   *p_cmd_buf;

    char   *temp_path_buf;
    char   *temp_cmd_buf;

    int     idel_timeout_s;
    int     recv_timeout_s;

    /**
     * TCP keep alive
     */
    int     tcp_keepidle_s;
    int     tcp_keepintvl_s;
    int     tcp_keepcnt;

    aw_ftpd_session_t *sessions;

} aw_ftpd_config_t;

void aw_ftpd_param_init (
         aw_ftpd_t              *ftpd,
         const aw_ftpd_config_t *p_cfg_info,
         const char             *root_path,
         int                     listen_port,
         int                     anonymous,
         aw_bool_t             (*pfn_check_user) (const char *user_name,
                                                  const char *pass_word));
void aw_ftpd_task_listen (void *arg);

/**
 * \brief    ftpd初始化
 *
 * \param[in]    root_path      ：  ftpd路径
 * \param[in]   listen_port     ：  监听端口
 * \param[in]   anonymous       ：  是否匿名 1：匿名   0：不匿名
 * \param[in]   pfn_check_user  ：  用户检测回调函数指针
 *
 * \retval AM_OK 初始化成功
 */
int  aw_ftpd_init (const char *root_path,
                   int         listen_port,
                   int         anonymous,
                   aw_bool_t   (*pfn_check_user) (const char *user_name,
                                                  const char *pass_word));

#ifdef __cplusplus
}
#endif

/** @} grp_aw_if_tpd */

#endif /* } __AW_FTPD_H */

/* end of file */

