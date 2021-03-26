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
 * \brief log adapt ftl .
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
#include "aw_delay.h"

#include "aw_log_ftl.h"
/******************************************************************************/
#if defined(__VIH_DEBUG)
#include "aw_shell.h"
extern int aw_shell_printf_with_color (aw_shell_user_t *p_user, 
                                       const char      *color,
                                       const char      *func_name, 
                                       const char      *fmt,
                                       ...);
#define __LOG_BASE(mask, color, fmt, ...)   if ((mask)) \
                                            {aw_shell_printf_with_color( \
                                                AW_DBG_SHELL_IO, \
                                                color, \
                                                __func__, \
                                                fmt, \
                                                ##__VA_ARGS__);}
#define __LOG_BASE_ERR   __LOG_BASE
#else

#define __LOG_BASE(mask, color, fmt, ...)
#define __LOG_BASE_ERR(mask, color, fmt, ...)  if (mask) \
                                                {AW_INFOF((fmt, ##__VA_ARGS__)); }
#endif
#define __LOG(mask, fmt, ...)  __LOG_BASE(mask, NULL, fmt, ##__VA_ARGS__)
#define __LOG_PFX(mask, fmt, ...) if ((mask)) \
                                    {__LOG(1, "[%s:%d] " fmt "\n", \
                                        __func__, \
                                        __LINE__, \
                                        ##__VA_ARGS__);}
                                        
#define __LOG_INFO(fmt, ...)  AW_INFOF(("[%s:%d] " fmt "\n", \
                                        __func__, \
                                        __LINE__, \
                                        ##__VA_ARGS__))
                                        
#define __LOG_ERR(fmt, ...)  __LOG_BASE_ERR(__MASK_ERR, \
                                            AW_SHELL_FCOLOR_RED, \
                                            "[err][%s:%d] " fmt "\n", \
                                            __func__, \
                                            __LINE__, \
                                            ##__VA_ARGS__)
                                            
#define __RETURN_IF(cond, ret_exp)  if ((cond)) \
                                    {__LOG_BASE_ERR(__MASK_ERR, \
                                     AW_SHELL_FCOLOR_RED,\
                                     "[err][%s:%d] " #cond "\n", \
                                     __func__, \
                                     __LINE__); \
                                     return (ret_exp); }
                                     
#define __GOTO_EXIT_IF(cond, ret_val)  if ((cond)) \
                                        {__LOG_BASE(__MASK_ERR, \
                                         AW_SHELL_FCOLOR_RED, \
                                         "[err][%s:%d] " #cond "\n", \
                                         __func__, \
                                         __LINE__); \
                                         ret = (ret_val); \
                                         goto exit; }
                                         
#define __WARN_IF(cond)  if ((cond)) \
                            {AW_INFOF(("[ WARN ][%s:%d] " #cond "\n", \
                                       __func__, \
                                       __LINE__));}
/******************************************************************************/
#define __MASK_ERR      1
#define __MASK_DBG      1
/******************************************************************************/
aw_local void __log_ftl_write_cb (void *p_arg);
/******************************************************************************/
aw_local void __delayed_word_cb (void *p_arg)
{
    struct aw_defer_djob    *p_djob = p_arg;

    if (aw_sys_defer_job_is_usable(AW_SYS_DEFER_TYPE_NOR, (void *)p_djob) != AW_TRUE) {
        __LOG_ERR("defer job is not usable!");
    }

    if (aw_sys_defer_commit(AW_SYS_DEFER_TYPE_NOR, (void *)p_djob) != AW_OK) {
        __LOG_ERR("defer job commit failed!");
    }
}

static int __defer_job (struct aw_defer_djob    *p_djob,
                        struct aw_delayed_work  *p_delay_work,
                        void                   (*pfunc) (void *),
                        void                    *p_arg)
{
    int ret = AW_OK;

    aw_defer_job_init(p_djob, pfunc, p_arg);

    if (!aw_sys_defer_job_is_usable(AW_SYS_DEFER_TYPE_NOR, p_djob) ) {
        // 节点不可用
        ret = -AW_EPERM;
    }

    aw_delayed_work_init(p_delay_work, __delayed_word_cb, (void *)p_djob);

    return ret;
}

/******************************************************************************/
aw_local void __log_ftl_write_cb (void *p_arg)
{
    aw_log_ftl_ctx_t   *p_ctx = p_arg;
    int                  ret;

    AW_MUTEX_LOCK(p_ctx->logger.mutex_lock, AW_SEM_WAIT_FOREVER);

    __LOG_PFX(__MASK_DBG, "aw_ftl_sector_write(page:%d)", p_ctx->newest_lsn);
    ret = aw_ftl_sector_write(p_ctx->p_ftl_dev, p_ctx->newest_lsn, (uint8_t *)p_ctx->head);
    if (ret != AW_OK) {
        __LOG_ERR("write sector(%d) failed", p_ctx->newest_lsn);
    }
    p_ctx->wait_write = AW_FALSE;

    AW_MUTEX_UNLOCK(p_ctx->logger.mutex_lock);
}
/******************************************************************************/
/* tag:init */
/**
 * \retval AW_OK     :  成功
 * \retval -EIO      :  读数据失败
 * \retval ELSE      :  操作失败，失败原因可查看errno
 */
static aw_err_t __log_ftl_init (aw_logger_t   *p_logger)
{
    void               *p_ftl_hd = NULL;
    int                 i = 0;
    char               *buf = NULL;
    int                 ret;
    char                ftl_name[64];
    char                fdata[128];
    const char         *p_mtd_name;
    aw_bool_t              find_empty = AW_FALSE;
    aw_bool_t              find_data  = AW_FALSE;
    aw_log_ftl_ctx_t   *p_ctx = AW_CONTAINER_OF(p_logger, aw_log_ftl_ctx_t, logger);

    p_mtd_name = p_ctx->dev_name;

    if (p_ctx == NULL) {
        return -AW_EINVAL;
    }

    if (p_ctx->init_flag ) {
        return AW_OK;
    }

    aw_snprintf(ftl_name, 64, "/ftl%s", p_mtd_name);

    ret = aw_ftl_dev_create (ftl_name, p_mtd_name);
    if (ret != AW_OK) {
        __LOG_ERR("create \"%s\" failed", ftl_name);
        return ret;
    }

    p_ftl_hd = aw_ftl_dev_get(ftl_name);
    if (p_ftl_hd == NULL) {
        __LOG_ERR("get device failed. (ftl-name:%s)", ftl_name);
        __RETURN_IF((ret = aw_ftl_dev_destroy(ftl_name)) != AW_OK, -AW_ENODEV);
    }

    aw_ftl_dev_ioctl(p_ftl_hd, AW_FTL_GET_SECTOR_SIZE,  (void *)&p_ctx->sector_size);
    aw_ftl_dev_ioctl(p_ftl_hd, AW_FTL_GET_SECTOR_COUNT, (void *)&p_ctx->nsectors);

    buf = (char *)aw_mem_alloc(p_ctx->sector_size);
    if (buf == NULL) {
        aw_ftl_dev_put(p_ftl_hd);
        aw_ftl_dev_destroy(ftl_name);
        __LOG_ERR("memory allocate failed");

        return -AW_ENOMEM;
    }

    p_ctx->nsectors = p_ctx->nsectors - 1;  /* 存放格式化信息 */

    p_ctx->sector_threshold = p_ctx->nsectors * 0.05;
    if (p_ctx->sector_threshold < 2) {
        p_ctx->sector_threshold = 2;
    }
    p_ctx->wait_write   = AW_FALSE;
    p_ctx->init_flag    = AW_TRUE;

    p_ctx->p_ftl_dev        = p_ftl_hd;
    p_ctx->sector_data_size = p_ctx->sector_size - sizeof(uint16_t);
    p_ctx->head             = (uint16_t *)buf;
    p_ctx->buf              = buf + sizeof(uint16_t);
    p_ctx->offset           = 0;
    memset((void *)p_ctx->head, 0x00, p_ctx->sector_size);

    find_empty = AW_FALSE;
    find_data  = AW_FALSE;

    /* 检查格式化信息，在最后 一个扇区*/
    {
        aw_snprintf(fdata, sizeof(fdata),
                    "LOG-FTL Format: <name:%s><nsector:%d><sector_size:%d>",
                    ftl_name,
                    p_ctx->nsectors,
                    p_ctx->sector_size);

        memset(buf, 0, p_ctx->sector_size);
        ret = aw_ftl_sector_read(p_ftl_hd, p_ctx->nsectors, (uint8_t *)buf);
        if (ret == AW_OK && memcmp(buf, fdata, strlen(fdata)) == 0) {
            __LOG_PFX(__MASK_DBG, "find the format info: ");
            __LOG(__MASK_DBG, "%s \n", fdata);
        }

        if (p_ctx->format_req ||
            ret != AW_OK ||
            memcmp(buf, fdata, strlen(fdata)) != 0) {

            __LOG_PFX(__MASK_DBG, "Need to format the ftl for log [ WARN ]");

            ret = aw_ftl_dev_ioctl(p_ftl_hd, AW_FTL_DEV_FORMAT, (void *)1);
            __GOTO_EXIT_IF(ret != AW_OK, ret);

            memcpy(buf, fdata, strlen(fdata));
            ret = aw_ftl_sector_write(p_ftl_hd, p_ctx->nsectors, (uint8_t *)buf);
            __GOTO_EXIT_IF(ret != AW_OK, ret);

            memset(buf, 0, p_ctx->sector_size);
            ret = aw_ftl_sector_read(p_ftl_hd, p_ctx->nsectors, (uint8_t *)buf);
            __GOTO_EXIT_IF(ret != AW_OK, ret);
            __GOTO_EXIT_IF(memcmp(buf, fdata, strlen(fdata)) != 0, ret);

            __LOG_PFX(__MASK_DBG, "Format ftl. [ OK ]");
        }
    }

    /* 
     * 扫描所有页获取数据大小;获取最旧和最新的lsn；
     * 0 -> len ：oldest； len -> 0 ：newest
     */
    for (i = 0; i < p_ctx->nsectors; i++) {
        ret = aw_ftl_sector_read(p_ftl_hd, i, (uint8_t *)p_ctx->head);
        if (ret == AW_OK && (*p_ctx->head) > 0) {
            /* 前2字节存放该扇区的数据长度 */
            p_logger->log_size += *p_ctx->head;

            if (find_empty) {
                find_empty        = AW_FALSE;
                p_ctx->oldest_lsn = i;
            }

            find_data = AW_TRUE;
        } else {
            find_empty = AW_TRUE;

            if (find_data) {
                find_data = AW_FALSE;
                p_ctx->newest_lsn = i - 1;
            }
        }

        /* i为最后一页时，再读一次页0 */
        if (i == p_ctx->nsectors - 1) {
            ret = aw_ftl_sector_read(p_ftl_hd, 0, (uint8_t *)p_ctx->head);
            if (ret == AW_OK && (*p_ctx->head) > 0) {
                if (find_empty) {
                    p_ctx->oldest_lsn = 0;
                }
            } else {
                if (find_data) {
                    p_ctx->newest_lsn = i;
                }
            }
        }
    }

    if (p_logger->log_size == 0) {
        p_ctx->oldest_lsn = 0;
        p_ctx->newest_lsn = 0;
        p_ctx->free_sector = p_ctx->nsectors - 1;
    } else {
        if (p_ctx->newest_lsn >= p_ctx->oldest_lsn) {
            p_ctx->free_sector = p_ctx->nsectors - (p_ctx->newest_lsn + 1 - p_ctx->oldest_lsn);
        } else {
            p_ctx->free_sector = p_ctx->oldest_lsn - p_ctx->newest_lsn - 1;
        }
    }

    /* 读最新页的数据到buf中 */
    ret = aw_ftl_sector_read(p_ftl_hd, p_ctx->newest_lsn, (uint8_t *)p_ctx->head);
    if (ret == AW_OK) {
        p_ctx->offset = *p_ctx->head;
        if (p_ctx->offset > p_ctx->sector_data_size) {
            p_ctx->offset = 0;
        }
    } else {
        p_ctx->offset = 0;
    }

    __LOG_PFX(__MASK_DBG, "log size:%d, offset:%d", p_logger->log_size, p_ctx->offset);

    if ( (p_ctx->nsectors - p_ctx->free_sector - 1) * p_ctx->sector_data_size !=
         p_logger->log_size - p_ctx->offset) {

        p_logger->log_size = (p_ctx->nsectors - p_ctx->free_sector - 1) *
                              p_ctx->sector_data_size + p_ctx->offset;

        __LOG_ERR("ftl-log format err, modified :%d!", p_logger->log_size);
    }

    if (!p_ctx->defer_init ) {
        ret = __defer_job(&p_ctx->djob, &p_ctx->delay_work, __log_ftl_write_cb, p_ctx);
        if (ret == AW_OK) {
            p_ctx->defer_init = AW_TRUE;
        } else {
            p_ctx->logger.tim = 0;
        }
    }

    return AW_OK;
exit:
    if (p_ftl_hd) {
        aw_ftl_dev_put(p_ftl_hd);
        aw_ftl_dev_destroy(ftl_name);
    }
    if (buf) {
        aw_mem_free(buf);
    }
    return ret;
}

/* tag:deinit */

static aw_err_t __log_ftl_deinit (aw_logger_t   *p_logger)
{
    aw_log_ftl_ctx_t   *p_ctx = AW_CONTAINER_OF(p_logger, aw_log_ftl_ctx_t, logger);
    char                ftl_name[64];

    if (!p_ctx->init_flag ) {
        return AW_OK;
    }

    p_ctx->init_flag = AW_FALSE;

    if (p_ctx->wait_write) {
        aw_delayed_work_stop(&p_ctx->delay_work);
        __LOG_PFX(__MASK_DBG, "wait to write log data to ftl");
        aw_mdelay(1000);
        __log_ftl_write_cb(p_ctx);
    }

    aw_snprintf(ftl_name, 64, "/ftl%s", p_ctx->dev_name);
    aw_ftl_dev_put(p_ctx->p_ftl_dev);
    aw_ftl_dev_destroy(ftl_name);

    if (p_ctx->head) {
        aw_mem_free(p_ctx->head);
        p_ctx->buf  = NULL;
        p_ctx->head = NULL;
    }

    return AW_OK;
}

/* tag:printf */
static ssize_t __log_ftl_printf (struct aw_logger *p_logger,
                                  const char      *p_buf,
                                  size_t           nbytes)
{
    aw_log_ftl_ctx_t *p_ctx = AW_CONTAINER_OF(p_logger, aw_log_ftl_ctx_t, logger);
    uint32_t  remain;
    uint32_t  write_size;

    if (p_ctx == NULL || (!p_ctx->init_flag) ) {
        return -AW_EPERM;
    }

    if (p_ctx->tim != p_logger->tim) {
        p_ctx->tim = p_logger->tim;
    }

    __LOG_PFX(__MASK_DBG, "record: nbytes:%d", nbytes);

    remain = nbytes;
    while (remain > 0) {
        write_size     = min(remain, p_ctx->sector_data_size - p_ctx->offset);

        memcpy(p_ctx->buf + p_ctx->offset, p_buf, write_size);
        p_buf              += write_size;
        p_ctx->offset      += write_size;
        p_logger->log_size += write_size;
        remain             -= write_size;

        /* 更新扇区数据长度 */
        *p_ctx->head =  p_ctx->offset;

        /* 满一页数据，则写入；写入前先判断是否需要删除旧数据 */
        if (p_ctx->offset >= p_ctx->sector_data_size) {
            if (p_ctx->free_sector > 0) {
                p_ctx->free_sector--;
            }
            if (p_ctx->free_sector <= p_ctx->sector_threshold || \
                p_ctx->free_sector == 0) {
                aw_ftl_dev_ioctl(p_ctx->p_ftl_dev, 
                                 AW_FTL_SECTOR_DAT_DEL, 
                                 (void *)p_ctx->oldest_lsn);
                p_logger->log_size -= p_ctx->sector_data_size;
                if (p_logger->export_pos <= p_ctx->sector_data_size) {
                    p_logger->export_pos = 0;
                } else {
                    p_logger->export_pos -= p_ctx->sector_data_size;
                }

                p_ctx->free_sector++;
                p_ctx->oldest_lsn++;
                if (p_ctx->oldest_lsn >= p_ctx->nsectors) {
                    p_ctx->oldest_lsn = 0;
                }
            }

            __LOG_PFX(__MASK_DBG, "ftl write a full page");
            __log_ftl_write_cb(p_ctx);

            /* 这部分代码不放入  write_cb 中*/
            p_ctx->offset = 0;
            memset(p_ctx->buf, 0, p_ctx->sector_data_size);
            p_ctx->newest_lsn++;
            if (p_ctx->newest_lsn >= p_ctx->nsectors) {
                p_ctx->newest_lsn = 0;
            }
        }
    }

    if (p_ctx->offset == 0) {
        return 0;
    }

    if (p_ctx->tim) {
        aw_delayed_work_start(&p_ctx->delay_work, p_ctx->tim);
        p_ctx->wait_write = AW_TRUE;
    } else {
        __log_ftl_write_cb(p_ctx);
    }

    return 0;
}

/* tag:export */
static int __log_ftl_export  (struct aw_logger  *p_logger,
                              int                offset,
                              void              *p_buffer,
                              uint32_t           buf_size)
{
    aw_log_ftl_ctx_t *p_ctx      = AW_CONTAINER_OF(p_logger, aw_log_ftl_ctx_t, logger);
    uint16_t          lsn;
    uint16_t          lsn_offs;
    int               ret        = AW_OK;
    int               remain;
    int               read_size;
    aw_bool_t         in_buf     = AW_FALSE;
    uint8_t          *p_head;
    uint8_t          *p_dat;
    uint8_t          *p_buf;

    p_buf = (uint8_t *)p_buffer;

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
    if (p_buffer == NULL || buf_size == 0) {
        return AW_OK;    /* 仅偏移，不读数据*/
    }

    p_head = aw_mem_alloc(p_ctx->sector_size);
    if (p_head == NULL) {
        return -AW_ENOMEM;
    }

    p_dat    = p_head + sizeof(uint16_t);
    lsn      = offset / p_ctx->sector_data_size;
    lsn = lsn + p_ctx->oldest_lsn;
    if (lsn >= p_ctx->nsectors) {
        lsn = lsn - p_ctx->nsectors;
    }

    lsn_offs        = offset % p_ctx->sector_data_size;
    remain          = min(p_logger->log_size - offset, buf_size);
    buf_size        = remain;

    while (remain > 0) {
        in_buf = (lsn == p_ctx->newest_lsn) ? AW_TRUE : AW_FALSE;

        if (!in_buf) {
            read_size = min(remain, p_ctx->sector_data_size - lsn_offs);

            ret = aw_ftl_sector_read(p_ctx->p_ftl_dev, lsn, p_head);
            if (ret == AW_OK) {
                memcpy(p_buf, p_dat + lsn_offs, read_size);
            } else {
                memset(p_buf, '0', read_size);
            }

            p_buf      += read_size;
            remain   -= read_size;
            lsn_offs += read_size;
            if (lsn_offs >= p_ctx->sector_data_size) {
                lsn_offs -= lsn_offs;
                lsn++;
                if (lsn == p_ctx->nsectors) {
                    lsn = 0;
                }
            }
        } else {
            read_size = min(remain, p_ctx->offset - lsn_offs);
            memcpy(p_buf, p_ctx->buf + lsn_offs, read_size);
            remain -= read_size;
            ret = AW_OK;
            break;
        }
    }

    aw_mem_free(p_head);

    if (ret != AW_OK) {
        return ret;
    }
    p_logger->export_pos = offset + (buf_size - remain);

    return buf_size - remain;
}

/* tag:data  tag:clear */

static int __log_ftl_data_clear  (struct aw_logger  *p_logger)
{
    int              ret;
    int              i;
    aw_log_ftl_ctx_t *p_ctx  = AW_CONTAINER_OF(p_logger, aw_log_ftl_ctx_t, logger);

    if (p_ctx->wait_write) {
        aw_delayed_work_stop(&p_ctx->delay_work);
        __LOG_PFX(__MASK_DBG, "wait to write log data to ftl");
        aw_mdelay(1000);
        __log_ftl_write_cb(p_ctx);
    }

    for (i = 0; i < p_ctx->nsectors; i++) {
        ret = aw_ftl_dev_ioctl(p_ctx->p_ftl_dev, AW_FTL_SECTOR_DAT_DEL, (void *)i);
        if (ret != AW_OK) {
            __LOG_ERR("ftl delete page(%d) data failed.", i);
        }
    }
    p_logger->log_size   = 0;
    p_logger->export_pos = 0;
    p_ctx->oldest_lsn    = 0;
    p_ctx->newest_lsn    = 0;
    p_ctx->free_sector   = p_ctx->nsectors - 1;
    p_ctx->offset        = 0;
    memset((void *)p_ctx->head, 0x00, p_ctx->sector_size);

    return AW_OK;
}


aw_logger_t * aw_log_ftl_ctor (aw_log_ftl_ctx_t *p_ctx,
                               const char       *p_dev_name,
                               aw_bool_t         format_req)
{
    if (p_ctx == NULL || p_dev_name == NULL) {
        return NULL;
    }
    __LOG_PFX(1, "ctor %s", p_dev_name);

    memset(p_ctx, 0, sizeof(*p_ctx));

    p_ctx->logger.pfn_init       = __log_ftl_init;
    p_ctx->logger.pfn_deinit     = __log_ftl_deinit;
    p_ctx->logger.print          = __log_ftl_printf;
    p_ctx->logger.pfn_export     = __log_ftl_export;
    p_ctx->logger.pfn_data_clear = __log_ftl_data_clear;
    p_ctx->format_req            = format_req;

    aw_snprintf(p_ctx->dev_name, __LOG_FTL_DEV_NAME_MAX, "%s", p_dev_name);

    return &p_ctx->logger;
}



