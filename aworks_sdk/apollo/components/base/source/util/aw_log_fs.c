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
 * \brief log adapt fs .
 *
 * \internal
 * \par History
 * - 1.00 17-07-07, vih, First implementation.
 * \endinternal
 */

#include "apollo.h"
#include "aw_vdebug.h"
#include "aw_mem.h"
#include "string.h"
#include "stdio.h"
#include "io/aw_io_lib.h"

#include "aw_log_fs.h"
/*----------------------------------------------------------------------------*/
#if defined(__VIH_DEBUG)
#include "aw_shell.h"
extern int aw_shell_printf_with_color (aw_shell_user_t *p_user,
                                       const char *color, 
                                       const char *func_name,
                                       const char *fmt, ...);
#define __LOG_BASE(mask, color, fmt, ...)   \
    if ((mask)) {\
        aw_shell_printf_with_color(AW_DBG_SHELL_IO, \
                                   color, \
                                   __func__, \
                                   fmt, \
                                   ##__VA_ARGS__);\
    }
#define __LOG_BASE_ERR   __LOG_BASE
#else
#define __LOG_BASE(mask, color, fmt, ...)
#define __LOG_BASE_ERR(mask, color, fmt, ...)  \
    if (mask) {\
        AW_INFOF((fmt, ##__VA_ARGS__));\
    }

#endif
#define __LOG(mask, fmt, ...)  __LOG_BASE(mask, NULL, fmt, ##__VA_ARGS__)
#define __LOG_PFX(mask, fmt, ...) if ((mask)) {__LOG(1, \
                                                    "[%s:%d] " fmt "\n", \
                                                    __func__, \
                                                    __LINE__, \
                                                    ##__VA_ARGS__);}
#define __LOG_ERR(fmt, ...)  __LOG_BASE_ERR(__MASK_ERR, \
                                            AW_SHELL_FCOLOR_RED, \
                                            "[err][%s:%d] " fmt "\n", \
                                            __func__, \
                                            __LINE__, \
                                            ##__VA_ARGS__)
#define __LOG_RES(is_true, mask, fmt, ...) \
    if (is_true) {\
        __LOG_BASE(mask, NULL, "\x1B[38;5;40m[ OK ]%s " fmt  "\n", \
                aw_shell_color_get_by_name(__func__), ##__VA_ARGS__);\
    } else {\
        __LOG_BASE_ERR(1, \
                       AW_SHELL_FCOLOR_RED,   \
                       "[ ERROR ] [%s:%d] " fmt "\n", \
                       __func__, \
                       __LINE__ , \
                       ##__VA_ARGS__);\
    }

#define __WARN_IF(cond)  if ((cond)) {AW_INFOF(("[ WARN ][%s:%d] " #cond "\n",\
                                                __func__, \
                                                __LINE__));}
#define __GOTO_EXIT_IF(cond, ret_val)  if ((cond)) \
                                        {AW_INFOF(("[ ERROR ][%s:%d] " #cond "\n", \
                                        __func__, \
                                        __LINE__)); \
                                        ret = (ret_val); \
                                        goto exit; }

#define __RETURN_IF(cond, ret_exp)  if ((cond)) \
                                        {AW_INFOF(("[ ERROR ][%s:%d] " #cond "\n", \
                                         __func__, \
                                         __LINE__)); \
                                         return (ret_exp); }
#define __EXIT_IF(cond)  if ((cond)) {AW_INFOF(("[ ERROR ][%s:%d] " #cond "\n", \
                                                __func__, \
                                                __LINE__)); \
                                                return; }
/******************************************************************************/
#define __MASK_ERR      1
#define __MASK_DBG      1
#define __MASK_FS       1
/******************************************************************************/
/* 不允许等于0 */
#define __LOG_FS_BUF_SIZE   (1024*4)
/******************************************************************************/
aw_local void __log_fs_write_cb (void *p_arg);
/******************************************************************************/
aw_local void __delayed_word_cb (void *p_arg)
{
    struct aw_defer_djob    *p_defer_job = p_arg;

    if (aw_sys_defer_job_is_usable(AW_SYS_DEFER_TYPE_NOR, 
                                   (void *)p_defer_job) != AW_TRUE) {
        __LOG_PFX(__MASK_DBG, "ERROR: defer job is not usable!\n");
    }

    if (aw_sys_defer_commit(AW_SYS_DEFER_TYPE_NOR, (void *)p_defer_job) != AW_OK) {
        __LOG_ERR("ERROR: defer job commit failed!\n");
    }
}

static int __defer_job_init (aw_log_fs_ctx_t         *p_this,
                             void                   (*pfunc) (void *))
{
    int ret = AW_OK;
    struct aw_defer_djob    *p_defer_job  = &p_this->defer_job;
    struct aw_delayed_work  *p_delay_work = &p_this->delay_work;

    aw_defer_job_init(p_defer_job, pfunc, p_this);
    if ( !aw_sys_defer_job_is_usable(AW_SYS_DEFER_TYPE_NOR, p_defer_job) ) {
        // 节点不可用
        ret = -AW_EPERM;
    }

    aw_delayed_work_init(p_delay_work, __delayed_word_cb, (void *)p_defer_job);

    return ret;
}

static aw_err_t __defer_job_start (aw_log_fs_ctx_t         *p_this, uint32_t ms_tim)
{
    if (aw_sys_defer_job_is_usable(AW_SYS_DEFER_TYPE_NOR, &p_this->defer_job) != AW_TRUE) {
        __LOG_ERR("ERROR: defer job is not usable!\n");
        return -AW_EPERM;
    }

    if (ms_tim > 0) {
        aw_delayed_work_start(&p_this->delay_work, ms_tim);
    }
    return AW_OK;
}

static void __defer_job_stop (aw_log_fs_ctx_t         *p_this)
{
    aw_sys_defer_abort(AW_SYS_DEFER_TYPE_NOR, &p_this->defer_job);
    aw_delayed_work_stop(&p_this->delay_work);
}

/******************************************************************************/
/* tag:cb */

aw_local void __log_fs_write_cb (void *p_arg)
{
    aw_log_fs_ctx_t *p_ctx = p_arg;
    int              len;
    int              ret;

    AW_MUTEX_LOCK(p_ctx->logger.mutex_lock, AW_SEM_WAIT_FOREVER);

    if (p_ctx->offset == 0) {
        goto exit;
    }

    len = aw_write(p_ctx->handle, p_ctx->buf, p_ctx->offset);
    if (len != p_ctx->offset) {
        __LOG_ERR("write log failed(len=%d, offset=%d)", len, p_ctx->offset);
        p_ctx->err = -AW_ENOMEM;
    } else {
        __LOG_PFX(__MASK_DBG, "\"%s\" log write success.\n", p_ctx->file_name);
    }
    memset(p_ctx->buf, 0,    __LOG_FS_BUF_SIZE);
    p_ctx->offset = 0;

    ret = aw_fsync(p_ctx->handle);
    if (ret != AW_OK) {
        __LOG_ERR("file synchronize failed. (file-name:%s, ret:%d)", p_ctx->file_name, ret);
    }

exit:
    AW_MUTEX_UNLOCK(p_ctx->logger.mutex_lock);
}


/******************************************************************************/
/* tag:init  tag:open */

static aw_err_t __log_fs_open (aw_logger_t   *p_logger)
{
    char               *buf;
    int                 ret;
    aw_log_fs_ctx_t    *p_ctx = AW_CONTAINER_OF(p_logger, aw_log_fs_ctx_t, logger);
    struct aw_stat      stat_buf;

    if (p_ctx == NULL || p_ctx->file_name == NULL) {
        return -AW_EINVAL;
    }
    if (p_ctx->init_flag ) {
        return AW_OK;
    }

    /* 读stat，获取文件的大小 */
    aw_snprintf(p_ctx->file_name_old, sizeof(p_ctx->file_name_old), "%s.old", p_ctx->file_name);
    if (aw_stat(p_ctx->file_name_old, &stat_buf) == AW_OK) {
        p_logger->log_size = stat_buf.st_size;

    } else {
        p_logger->log_size = 0;
    }

    /* 读stat，获取文件的大小 */
    if (aw_stat(p_ctx->file_name, &stat_buf) == AW_OK) {
        p_logger->log_size += stat_buf.st_size;
        p_ctx->file_size    = stat_buf.st_size;

        if (p_ctx->format_req) {
            ret = aw_unlink(p_ctx->file_name);
            if (ret != AW_OK) {
                __LOG_ERR("delete file failed. (file path:%s)", p_ctx->file_name);
            }
            p_logger->log_size = 0;
        }
    } else {
        p_logger->log_size = 0;
        p_ctx->file_size   = 0;
    }
    __LOG_PFX(__MASK_DBG, "log size:%d ", p_logger->log_size);

    p_ctx->format_req = AW_FALSE;

    buf = (char *)aw_mem_alloc(__LOG_FS_BUF_SIZE);
    if (buf == NULL) {
        __LOG_ERR("memory allocate failed");
        return -AW_EINVAL;
    }
    p_ctx->resource_cnt++;

    p_ctx->buf        = buf;
    p_ctx->offset     = 0;

    memset(p_ctx->buf, 0x00, __LOG_FS_BUF_SIZE);

    ret = __defer_job_init(p_ctx, __log_fs_write_cb);
    if (ret == AW_OK && p_ctx->logger.tim > 0) {
        p_ctx->resource_cnt += 2;
    }

    p_ctx->handle_old = aw_open(p_ctx->file_name_old, O_RDONLY , 0777);

    p_ctx->handle = aw_open(p_ctx->file_name, O_RDWR | O_CREAT | O_APPEND, 0777);
    if (p_ctx->handle < 0) {
        __LOG_ERR("open file failed. (file:%s, ret:%d)", p_ctx->file_name, p_ctx->handle);
        ret = p_ctx->handle;
        goto failed;
    }
    p_ctx->init_flag  = AW_TRUE;

    return AW_OK;

failed:
    if (buf) {
        aw_mem_free(buf);
    }

    if (p_ctx->handle) {
        aw_close(p_ctx->handle);
    }
    if (p_ctx->handle_old) {
        aw_close(p_ctx->handle_old);
    }
    return ret;
}

/* tag:close */
static aw_err_t __log_fs_close (aw_logger_t   *p_logger)
{
    aw_log_fs_ctx_t   *p_ctx = AW_CONTAINER_OF(p_logger, aw_log_fs_ctx_t, logger);

    if (p_ctx == NULL || !p_ctx->init_flag ) {
        return -AW_EPERM;
    }
    p_ctx->init_flag = AW_FALSE;

    if (p_logger->tim) {
        __defer_job_stop(p_ctx);
        p_ctx->resource_cnt -= 2;
    }

    if (p_ctx->offset) {
        __log_fs_write_cb(p_ctx);
    }

    if (p_ctx->buf) {
        aw_mem_free(p_ctx->buf);
        p_ctx->buf = NULL;
        p_ctx->resource_cnt--;
    }

    if (p_ctx->resource_cnt != 0) {
        __LOG_ERR("deinit err: resource remain:%d.\n", p_ctx->resource_cnt);
    }

    aw_close(p_ctx->handle);

    if (p_ctx->handle_old >= 0) {
        aw_close(p_ctx->handle_old);
    }

    return AW_OK;
}

/* tag:printf */

static ssize_t __log_fs_printf (struct aw_logger *p_logger,
                                const char       *p_buf,
                                size_t            nbytes)
{
    aw_log_fs_ctx_t *p_ctx              = AW_CONTAINER_OF(p_logger, aw_log_fs_ctx_t, logger);
    int             remain;
    int             write_size;
    char            need_new_file       = AW_FALSE;
    int             new_file_write_size = 0;
    int             old_file_write_size = 0;
    int             ret;

    if (p_ctx == NULL || (!p_ctx->init_flag) ) {
        return -AW_EPERM;
    }

    if (p_ctx->tim != p_logger->tim) {
        p_ctx->tim = p_logger->tim;
    }
    if (p_ctx->tim) {
        __defer_job_stop(p_ctx);
    }

    remain = nbytes;
    while (remain > 0) {
        write_size     = min(remain, __LOG_FS_BUF_SIZE - p_ctx->offset);

        if (p_ctx->file_size + write_size <= (p_ctx->storage_size >> 1)) {

            p_ctx->file_size   += write_size;
            memcpy(p_ctx->buf + p_ctx->offset, p_buf, write_size);
            p_buf              += write_size;
            p_ctx->offset      += write_size;
            p_logger->log_size += write_size;
            remain             -= write_size;
        } else {
            need_new_file       = AW_TRUE;

            new_file_write_size = p_ctx->file_size + write_size - (p_ctx->storage_size >> 1);
            old_file_write_size = write_size - new_file_write_size;

            memcpy(p_ctx->buf + p_ctx->offset, p_buf, old_file_write_size);
            p_buf              += old_file_write_size;
            p_ctx->offset      += old_file_write_size;
            p_logger->log_size += old_file_write_size;
            remain             -= old_file_write_size;
        }

        if (need_new_file) {
            need_new_file       = AW_FALSE;

            __LOG_PFX(__MASK_DBG, 
                      "save \"%s\" as \"%s\"", 
                      p_ctx->file_name, 
                      p_ctx->file_name_old);

            /*
             * todo
             * - 将 file 写满 (storage_size 的一半大小)
             * - 已有 file.old 文件则删除
             * - 将 file 改名为 file.old
             * - 新建 file
             * - 将剩余数据写入 buf
             */
            __log_fs_write_cb(p_ctx);

            if (p_ctx->handle_old >= 0) {

                ret = aw_close(p_ctx->handle_old);
                p_ctx->handle_old = 1;
                __LOG_RES(ret == AW_OK, __MASK_FS, "close \"%s\".", p_ctx->file_name_old);

                ret = aw_delete(p_ctx->file_name_old);
                __LOG_RES(ret == AW_OK, __MASK_FS, "delete \"%s\".", p_ctx->file_name_old);

                p_logger->log_size -= (p_ctx->storage_size >> 1);

                if (p_logger->export_pos <= (p_ctx->storage_size >> 1)) {
                    p_logger->export_pos = 0;
                } else {
                    p_logger->export_pos -= (p_ctx->storage_size >> 1);
                }
            }

            ret = aw_close(p_ctx->handle);
            p_ctx->handle = 1;
            __LOG_RES(ret == AW_OK, __MASK_FS, "close \"%s\".", p_ctx->file_name);

            ret = aw_rename(p_ctx->file_name, p_ctx->file_name_old);
            __LOG_RES(ret == AW_OK, __MASK_FS, "rename \"%s\".", p_ctx->file_name);


            p_ctx->handle_old = aw_open(p_ctx->file_name_old, O_RDONLY , 0777);
            __LOG_RES(ret == AW_OK, __MASK_FS, "open \"%s\".", p_ctx->file_name_old);

            p_ctx->handle = aw_open(p_ctx->file_name, O_RDWR | O_CREAT | O_APPEND, 0777);
            __LOG_RES(ret == AW_OK, __MASK_FS, "create \"%s\".", p_ctx->file_name);

            if (new_file_write_size > 0) {
                memcpy(p_ctx->buf + p_ctx->offset, p_buf, new_file_write_size);
                p_buf              += new_file_write_size;
                p_ctx->offset      += new_file_write_size;
                p_logger->log_size += new_file_write_size;
                remain             -= new_file_write_size;
                p_ctx->file_size    = new_file_write_size;
            }
        }

        if (p_ctx->offset >= __LOG_FS_BUF_SIZE) {
            __log_fs_write_cb(p_ctx);
        }
    }

    if (p_ctx->offset == 0) {
        return 0;
    }

    if (p_ctx->tim) {
        ret = __defer_job_start(p_ctx, p_ctx->tim);
        if (ret != AW_OK) {
            __log_fs_write_cb(p_ctx);
            return p_ctx->err;
        }
    } else {
        __log_fs_write_cb(p_ctx);
        return p_ctx->err;
    }

    return 0;
}

/* tag:export */

static int __log_fs_export  (struct aw_logger  *p_logger,
                             int                offset,
                             void              *p_buf,
                             uint32_t           read_size)
{
    aw_log_fs_ctx_t *p_ctx      = AW_CONTAINER_OF(p_logger, aw_log_fs_ctx_t, logger);
    int             ret;
    int             remain      = 0;
    int             will_export = 0;
    int             seek_pos    = 0;
    uint32_t        export_size = 0;

    if (p_ctx == NULL || (!p_ctx->init_flag) ) {
        return -AW_EAGAIN;
    }

    if (offset == -0xFFFFFFFF ||
        offset + offset + p_logger->export_pos < 0) {
        offset = 0;
    } else if (offset == 0xFFFFFFFF ||
               offset + p_logger->export_pos > p_logger->log_size) {
        offset = p_logger->log_size;
    } else {
        offset += p_logger->export_pos;
    }
    __LOG_PFX(__MASK_DBG & 0, "offset:%d", offset);

    p_logger->export_pos = offset;
    if (p_buf == NULL || read_size == 0) {
        return AW_OK;    // 仅偏移，不读数据
    }

    /* 如果 buf 里有数据未写入文件，则关闭defer并立即写入文件  */
    if (p_ctx->tim > 0 && p_ctx->offset > 0) {
        __defer_job_stop(p_ctx);
        __log_fs_write_cb(p_ctx);
    }

    /* 有 .old 文件 */
    if (p_ctx->handle_old >= 0 && offset < (p_ctx->storage_size >> 1)) {

        ret = aw_lseek(p_ctx->handle_old, offset, SEEK_SET);
        if (ret != offset) {
            __LOG_ERR("file seek to offset:%d  failed. (file:%s, ret:%d)",
                      offset, p_ctx->file_name_old, ret);
            return -AW_EAGAIN;
        }

        remain      = min((p_ctx->storage_size >> 1) - offset, read_size);
        will_export = remain;
        if (remain > 0) {
            ret = aw_read(p_ctx->handle_old, p_buf, remain);
            if (ret != remain) {
                __LOG_ERR("read log failed. (retlen=%d, remain=%d)", ret, remain);
            }

            remain -= ret;
        }

        export_size     = will_export - remain;
        p_buf           = (void *)(((uint8_t *)p_buf) + export_size);
        offset          = 0;
        read_size      -= export_size;
    }

    if (offset >= (p_ctx->storage_size >> 1)) {
        offset -= (p_ctx->storage_size >> 1);
    }

    if (read_size) {
        ret = aw_lseek(p_ctx->handle, offset, SEEK_SET);
        if (ret != offset) {
            __LOG_ERR("file seek to address:%d  failed. (file:%d, ret:%d)", 
                      offset, 
                      p_ctx->file_name, 
                      ret);
            return -AW_EAGAIN;
        }

        remain      = min(p_ctx->file_size - offset, read_size);
        will_export = remain;
        if (remain > 0) {
            ret = aw_read(p_ctx->handle, p_buf, remain);
            if (ret != remain) {
                __LOG_ERR("read log failed. (retlen=%d, remain=%d)", ret, remain);
            }

            remain -= ret;
        }

        export_size += will_export - remain;
    }

    /* 检查文件大小*/
    if (p_ctx->handle_old >= 0) {
        seek_pos  = aw_lseek(p_ctx->handle_old, 0, SEEK_END);
    }
    seek_pos += aw_lseek(p_ctx->handle, 0, SEEK_END);
    if (seek_pos !=  p_logger->log_size) {
        __LOG_ERR("file seek to end address failed. (file:%d, ret:%d)",
                    p_ctx->file_name, seek_pos);
        ret =  -AW_EAGAIN;
    }

    if (ret < AW_OK) {
        return ret;
    }
    p_logger->export_pos += export_size;

    return export_size;
}


/* tag:data  tag:clear */

static int __log_fs_data_clear  (struct aw_logger  *p_logger)
{
    int             ret;
    aw_log_fs_ctx_t *p_ctx  = AW_CONTAINER_OF(p_logger, aw_log_fs_ctx_t, logger);
    struct          aw_stat stat_buf;

    p_logger->export_pos = 0;
    /* 先 close 再删除文件，重新 open */

    __RETURN_IF((ret = __log_fs_close(p_logger)) != AW_OK, ret);

    if (aw_stat(p_ctx->file_name, &stat_buf) == AW_OK) {
        __WARN_IF((ret = aw_unlink(p_ctx->file_name)) != AW_OK);
    }

    if (aw_stat(p_ctx->file_name_old, &stat_buf) == AW_OK) {
        __WARN_IF((ret = aw_unlink(p_ctx->file_name_old)) != AW_OK);
    }
    __RETURN_IF((ret = __log_fs_open(p_logger)) != AW_OK, ret);
    return AW_OK;
}
/******************************************************************************/
aw_logger_t * aw_log_fs_ctor (aw_log_fs_ctx_t *p_ctx,
                              const char      *p_file_path,
                              uint32_t         storage_size,
                              aw_bool_t        format_req)
{
    if (p_ctx == NULL || p_file_path == NULL) {
        return NULL;
    }

    memset(p_ctx,0, sizeof(aw_log_fs_ctx_t));
    p_ctx->logger.pfn_init       = __log_fs_open;
    p_ctx->logger.pfn_deinit     = __log_fs_close;
    p_ctx->logger.pfn_export     = __log_fs_export;
    p_ctx->logger.print          = __log_fs_printf;
    p_ctx->logger.pfn_data_clear = __log_fs_data_clear;
    p_ctx->format_req            = format_req;
    p_ctx->storage_size          = storage_size;

    aw_snprintf(p_ctx->file_name,     _FILE_PATH_MAX_LEN, "%s",     p_file_path);
    aw_snprintf(p_ctx->file_name_old, _FILE_PATH_MAX_LEN, "%s.old", p_file_path);

    return &p_ctx->logger;
}


