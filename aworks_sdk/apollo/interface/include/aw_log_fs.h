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
 * @brief log adapt fs.
 *
 * @internal
 * @par History
 * - 170707, vih, First implementation.
 * @endinternal
 */


#ifndef __AW_LOG_FS_H__
#define __AW_LOG_FS_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_log_fs
 * \copydoc aw_log_fs.h
 * @{
 */

#include "aworks.h"
#include "aw_delayed_work.h"
#include "aw_sys_defer.h"
#include "aw_loglib.h"

#include "io/aw_fcntl.h"
#include "io/aw_unistd.h"
#include "io/aw_dirent.h"
#include "io/sys/aw_stat.h"

/** \brief fs日志存储的上下文 */
typedef struct aw_log_fs_ctx {

#define _FILE_PATH_MAX_LEN   128
    
    /** \brief 文件名 */
    char                        file_name[_FILE_PATH_MAX_LEN];  
    char                        file_name_old[_FILE_PATH_MAX_LEN];

    aw_logger_t                 logger;       /**< \brief 日志器实例 */

    struct aw_delayed_work      delay_work;   /**< \brief 用于定时  */
    struct aw_defer_djob        defer_job;    /**< \brief 延时工作  */
    aw_bool_t                   defer_init;   /**< \brief 延时工作的初始化标志  */
    uint32_t                    tim;          /**< \brief 定时ms  */
    
    int                         handle;       /**< \brief 文件句柄  */
    int                         handle_old;   /**< \brief 文件句柄  */
    int                         file_size;    /**< \brief handle 文件大小  */

    int                         err;          /**< \brief 错误值  */
    int                         offset;       /**< \brief 数据偏移  */
    int16_t                     resource_cnt; /**< \brief 使用资源记数  */
    char                       *buf;          /**< \brief 缓存  */
    aw_bool_t                   init_flag;    /**< \brief 初始化标志  */
    aw_bool_t                   format_req;   /**< \brief 格式化磁盘请求  */

    /** \brief log 文件可写入的最大数据量  */
    uint32_t                    storage_size; 

} aw_log_fs_ctx_t;

/**
 * \brief 初始化一个log_fs实例
 *
 * \param[in]       p_ctx       实例指针
 * \param[in]       p_file_path 文件名
 * \param[in]       storage_size 文件最大数据量
 * \param[in]       format_req  格式化请求, TRUE:请求格式化  FALSE:不格式化
 *
 * \return 成功返回aw_logger_t *类型的指针，失败返回NULL
 *
 */
aw_logger_t * aw_log_fs_ctor (aw_log_fs_ctx_t *p_ctx,
                              const char      *p_file_path,
                              uint32_t         storage_size,
                              aw_bool_t        format_req);

/** @} grp_aw_log_fs */

#ifdef __cplusplus
}
#endif

#endif /* __AW_LOG_FS_H__ */

/* end of file */
