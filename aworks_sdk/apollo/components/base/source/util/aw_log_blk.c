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
 * @endinternal
 */

#include "apollo.h"
#include "aw_vdebug.h"
#include "aw_mem.h"
#include "string.h"
#include "stdio.h"

#include "aw_log_blk.h"
/******************************************************************************/
#define __LOG_DEBUG
/******************************************************************************/
#if  defined(__LOG_DEBUG) && defined(__VIH_IDENTITY_DEBUG)
#define __kprintf(fmt, ...)   aw_shell_printf(aw_dbg_shell_user(), \
                                              fmt, \
                                              ##__VA_ARGS__)

#define __ERR_DMSG(msg)  \
        __kprintf( "\nLOG MSG: ");\
        __kprintf msg; \
        __kprintf("\n----MSG: at file %s.\n"\
                    "----MSG: at line %d in %s()\n\n", \
                  __FILE__, \
                  __LINE__, \
                  __FUNCTION__) \

#else
#define __kprintf
#define __ERR_DMSG(msg)
#endif


/******************************************************************************/
aw_local void __log_blk_write_cb (void *p_arg);
static int __blk_xfer ( struct aw_blk_dev   *p_bd,
                        const char          *buff,
                        int                  sector,
                        int                  count,
                        aw_bool_t               read);
/******************************************************************************/
aw_local void __delayed_word_cb (void *p_arg)
{
    struct aw_defer_djob    *p_djob = p_arg;

    if (!aw_sys_defer_job_is_usable(AW_SYS_DEFER_TYPE_NOR, (void *)p_djob) ) {
        __kprintf("ERROR: defer job is not usable!\n");
    }

    if (aw_sys_defer_commit(AW_SYS_DEFER_TYPE_NOR, (void *)p_djob) != AW_OK) {
        __kprintf("ERROR: defer job commit failed!\n");
    }
}

static int __defer_job_init (struct aw_defer_djob    *p_djob,
                             struct aw_delayed_work  *p_delay_work,
                             void                   (*pfunc) (void *),
                             void                    *p_arg)
{
    int ret = AW_OK;

    aw_defer_job_init(p_djob, pfunc, p_arg);
    if (!aw_sys_defer_job_is_usable(AW_SYS_DEFER_TYPE_NOR, p_djob) ) {
        ret = -AW_EPERM;
    }
    aw_delayed_work_init(p_delay_work, __delayed_word_cb, (void *)p_djob);

    return ret;
}

/******************************************************************************/
static void __log_blk_dat_clr(aw_log_blk_ctx_t   *p_ctx)
{
    int i;

    /* 将块数据长度都写为0 */
    *p_ctx->head = 0;
    for (i = 0; i < p_ctx->nblks; i++) {
        __blk_xfer(p_ctx->p_bdev, (uint8_t *)p_ctx->head, i, 1, AW_FALSE);
    }
}

aw_local void __log_blk_write_cb (void *p_arg)
{
    aw_log_blk_ctx_t   *p_ctx = p_arg;
    int                  ret;

    if (p_ctx->offset == 0) {
        return;
    }

    ret = __blk_xfer(p_ctx->p_bdev, 
                     (uint8_t *)p_ctx->head,
                     p_ctx->newest_blk, 
                     1, 
                     AW_FALSE);
    if (ret != AW_OK) {
        __ERR_DMSG(("write blk(%d) failed", p_ctx->newest_blk));
    }

    p_ctx->wait_write = AW_FALSE;
}


/******************************************************************************/
static void __blk_xfer_done (struct aw_blk_arp *p_arp)
{
    (void)aw_semc_give((aw_semc_id_t)p_arp->priv);
}

static int __blk_xfer ( struct aw_blk_dev   *p_bd,
                        const char          *buff,
                        int                  sector,
                        int                  count,
                        aw_bool_t            read)
{
    struct aw_blk_arp  arp;
    int                ret;
    AW_SEMC_DECL(      bio_sem);
    aw_semc_id_t       semc_id = NULL;
    uint8_t            retry = 2;
    uint_t             bsize;

    if (p_bd == NULL || buff == NULL) {
        return -AW_EPERM;
    }

    ret = aw_blk_dev_ioctl(p_bd,
                           AW_BD_GET_BSIZE,
                           &bsize);
    if (ret != AW_OK) {
        return ret;
    }

    memset(&bio_sem,0,sizeof(bio_sem));
    semc_id = AW_SEMC_INIT(bio_sem, 0, AW_SEM_Q_PRIORITY);
    if (semc_id == NULL) {
        return -AW_EAGAIN;
    }

__retry:
    AW_SEMC_TAKE(bio_sem, AW_SEM_NO_WAIT);
    arp.p_dev    = p_bd;
    arp.blk_no   = sector;
    arp.nbytes   = bsize * count;
    arp.p_data   = (void *)buff;
    arp.resid    = 0;
    arp.error    = AW_OK;
    arp.flags    = read ? AW_BD_ARQ_READ : AW_BD_ARQ_WRITE;
    arp.priv     = semc_id;
    arp.pfn_done = __blk_xfer_done;
    arp.p_next   = NULL;

    ret = aw_blk_dev_areq(p_bd, &arp);
    AW_SEMC_TAKE(bio_sem, AW_SEM_WAIT_FOREVER);

    if ((ret == AW_OK) && (arp.error != AW_OK)) {
        ret = arp.error;
    }

    if (ret != AW_OK) {
        __ERR_DMSG(("bdev xfer failed:%d, retry %d\n", ret, retry));
        if ((ret != -AW_ENODEV) && (retry--)) {
            goto __retry;
        }
    }

    AW_SEMC_TERMINATE(bio_sem);

    return ret;
}
/******************************************************************************/
static int __log_blk_init (aw_logger_t            *p_logger)
{
    int                 ret;
    int                 i;
    const char         *p_dev_name;
    aw_bool_t              find_empty = AW_FALSE;
    aw_bool_t              find_data  = AW_FALSE;
    aw_log_blk_ctx_t   *p_ctx = AW_CONTAINER_OF(p_logger, aw_log_blk_ctx_t, logger);

    p_dev_name = p_ctx->dev_name;

    if (p_ctx == NULL || p_dev_name == NULL) {
        return -AW_EINVAL;
    }
    if (p_ctx->init_flag ) {
        return AW_OK;
    }

    p_ctx->p_bdev = aw_blk_dev_get(p_dev_name);
    if (p_ctx->p_bdev == NULL) {
        __ERR_DMSG(("get \"%s\" failed", p_dev_name));
        return -AW_ENODEV;
    }
    __kprintf("get \"%s\" success.\n", p_dev_name);

    ret = aw_blk_dev_ioctl(p_ctx->p_bdev, AW_BD_GET_BSIZE, &p_ctx->blk_size);
    if (ret != AW_OK) {
        __ERR_DMSG(("get block size failed(\"%s\")", p_ctx->p_bdev->name));
        return ret;
    }

    ret = aw_blk_dev_ioctl(p_ctx->p_bdev, AW_BD_GET_BLKS,  &p_ctx->nblks);
    if (ret != AW_OK) {
        __ERR_DMSG(("get block count failed(\"%s\")", p_ctx->p_bdev->name));
        return ret;
    }

    p_ctx->head = (uint16_t *)aw_mem_alloc(p_ctx->blk_size);
    if (p_ctx->head == NULL) {
        __ERR_DMSG(("memory allocate failed"));
        return -AW_ENOMEM;
    }


    p_ctx->init_flag     = AW_TRUE;
    p_ctx->wait_write    = AW_FALSE;
    p_ctx->offset        = 0;
    p_ctx->blk_data_size = p_ctx->blk_size - sizeof(uint16_t);
    p_ctx->buf           = (void *)((char *)p_ctx->head + sizeof(uint16_t));
    memset(p_ctx->head, 0x00, p_ctx->blk_size);

    p_ctx->blk_threshold = 0;
    find_empty = AW_FALSE;
    find_data  = AW_FALSE;

    if (p_ctx->format_req) {
        p_ctx->format_req = AW_FALSE;
        __log_blk_dat_clr(p_ctx);
    }

    /*扫描所有blk 获取数据大小 */
    for (i = 0; i < p_ctx->nblks; i++) {
        ret = __blk_xfer(p_ctx->p_bdev, (void *)p_ctx->head, i, 1, AW_TRUE);
        if (ret == AW_OK && (*p_ctx->head > 0)) {
            /* 前2字节存放该扇区的数据长度 */
            p_logger->log_size += *p_ctx->head;

            if (find_empty) {
                find_empty        = AW_FALSE;
                p_ctx->oldest_blk = i;
            }

            find_data = AW_TRUE;
        } else {
            find_empty = AW_TRUE;

            if (find_data) {
                find_data = AW_FALSE;
                p_ctx->newest_blk = i - 1;
            }
        }

        /* i为最后一页时，再读一次页0 */
        if (i == p_ctx->nblks - 1) {
            ret = __blk_xfer(p_ctx->p_bdev, (void *)p_ctx->head, 0, 1, AW_TRUE);
            if (ret == AW_OK && (*p_ctx->head) > 0) {
                if (find_empty) {
                    p_ctx->oldest_blk = 0;
                }
            } else {
                if (find_data) {
                    p_ctx->newest_blk = i;
                }
            }
        }
    }

    if (p_logger->log_size == 0) {
        p_ctx->oldest_blk = 0;
        p_ctx->newest_blk = 0;
        p_ctx->free_blk = p_ctx->nblks - 1;
    } else {
        if (p_ctx->newest_blk >= p_ctx->oldest_blk) {
            p_ctx->free_blk = p_ctx->nblks - (p_ctx->newest_blk + 1 - p_ctx->oldest_blk);
        } else {
            p_ctx->free_blk = p_ctx->oldest_blk - p_ctx->newest_blk - 1;
        }
    }

    /* 读最新页的数据到buf中 */
    __blk_xfer(p_ctx->p_bdev, (void *)p_ctx->head, p_ctx->newest_blk, 1, AW_TRUE);
    p_ctx->offset = *p_ctx->head;

    if ( !p_ctx->defer_init ) {
        ret = __defer_job_init(&p_ctx->djob,
                               &p_ctx->delay_work,
                               __log_blk_write_cb,
                               p_ctx);
        if (ret == AW_OK) {
            p_ctx->defer_init = AW_TRUE;
        } else {
            p_ctx->logger.tim = 0;
        }
    }

    return AW_OK;
}

static int __log_blk_deinit (aw_logger_t   *p_logger)
{
    aw_log_blk_ctx_t   *p_ctx = AW_CONTAINER_OF(p_logger, aw_log_blk_ctx_t, logger);

    if (!p_ctx->init_flag ) {
        return AW_OK;
    }

    p_ctx->init_flag = AW_FALSE;

    if (p_ctx->wait_write) {
        aw_delayed_work_stop(&p_ctx->delay_work);
        __log_blk_write_cb(p_ctx);
    }

    aw_blk_dev_put(p_ctx->p_bdev);

    if (p_ctx->head) {
        aw_mem_free(p_ctx->head);
        p_ctx->buf = NULL;
        p_ctx->head = NULL;
    }

    return AW_OK;
}

static ssize_t __log_blk_printf (struct aw_logger *p_logger,
                               const char       *p_buf,
                               size_t            nbytes)
{
    aw_log_blk_ctx_t *p_ctx = AW_CONTAINER_OF(p_logger, aw_log_blk_ctx_t, logger);
    int  remain;
    int  write_size;
    int  tmp;

    if (p_ctx == NULL || !(p_ctx->init_flag) ) {
        return -AW_EPERM;
    }

    if (p_ctx->tim != p_logger->tim) {
        p_ctx->tim = p_logger->tim;
    }

    remain = nbytes;
    while (remain > 0) {
        write_size     = min(remain, p_ctx->blk_data_size - p_ctx->offset);

        memcpy(p_ctx->buf + p_ctx->offset, p_buf, write_size);
        p_buf              += write_size;
        p_ctx->offset      += write_size;
        p_logger->log_size += write_size;
        remain             -= write_size;

        /* 更新扇区数据长度 */
        *p_ctx->head =  p_ctx->offset;

        /* 满一页数据，则写入；写入前先判断是否需要删除旧数据 */
        if (p_ctx->offset >= p_ctx->blk_data_size) {
            if (p_ctx->free_blk > 0) {
                p_ctx->free_blk--;
            }
            if (p_ctx->free_blk < p_ctx->blk_threshold || p_ctx->free_blk == 0) {
                tmp = *p_ctx->head;
                *p_ctx->head = 0;
                /* 将块数据长度都写为0 */
                __blk_xfer(p_ctx->p_bdev, (uint8_t *)p_ctx->head, p_ctx->oldest_blk, 1, AW_FALSE);
                *p_ctx->head = tmp;

                p_logger->log_size -= p_ctx->blk_data_size;
                p_ctx->free_blk++;
                p_ctx->oldest_blk++;
                if (p_ctx->oldest_blk >= p_ctx->nblks) {
                    p_ctx->oldest_blk = 0;
                }
            }

            __log_blk_write_cb(p_ctx);
            p_ctx->offset = 0;
            bzero(p_ctx->buf, p_ctx->blk_data_size);
            p_ctx->newest_blk++;
            if (p_ctx->newest_blk >= p_ctx->nblks) {
                p_ctx->newest_blk = 0;
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
        __log_blk_write_cb(p_ctx);
        return p_ctx->err;
    }

    return 0;
}

static int __log_blk_export  (struct aw_logger  *p_logger,
                               uint32_t           offset,
                               void              *p_buf,
                               uint32_t           buf_size)
{
    aw_log_blk_ctx_t *p_ctx = AW_CONTAINER_OF(p_logger, aw_log_blk_ctx_t, logger);
    uint16_t lsn, lsn_offs;
    int      ret;
    int      remain;
    int      read_size;
    aw_bool_t   in_buf = AW_FALSE;
    uint8_t *p_head;
    uint8_t *p_dat;

    if (offset == -0xFFFFFFFF ||
        offset + offset + p_logger->export_pos < 0) {
        offset = 0;
    } else if (offset == 0xFFFFFFFF ||
               offset + p_logger->export_pos > p_logger->log_size) {
        offset = p_logger->log_size;
    } else {
        offset += p_logger->export_pos;
    }

    p_head = aw_mem_alloc(p_ctx->blk_size);
    if (p_head == NULL) {
        return -AW_ENOMEM;
    }

    p_dat    = p_head + sizeof(uint16_t);
    lsn      = offset / p_ctx->blk_data_size;
    lsn = lsn + p_ctx->oldest_blk;
    if (lsn >= p_ctx->nblks) {
        lsn = lsn - p_ctx->nblks;
    }

    lsn_offs  = offset % p_ctx->blk_data_size;
    remain    = min(p_logger->log_size - offset, buf_size);
    buf_size  = remain;

    while (remain > 0) {
        in_buf = (lsn == p_ctx->newest_blk) ? AW_TRUE : AW_FALSE;

        if (!in_buf) {
            read_size = min(remain, p_ctx->blk_data_size - lsn_offs);

            ret = __blk_xfer(p_ctx->p_bdev, p_head, lsn, 1, AW_TRUE);
            if (ret == AW_OK) {
                memcpy(p_buf, p_dat + lsn_offs, read_size);
            } else {
                memset(p_buf, '0', read_size);
            }

            p_buf = (void *)((char *)p_buf + read_size);
            remain   -= read_size;
            lsn_offs += read_size;
            if (lsn_offs >= p_ctx->blk_data_size) {
                lsn_offs -= lsn_offs;
                lsn++;
                if (lsn == p_ctx->nblks) {
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

aw_logger_t * aw_log_blk_ctor (aw_log_blk_ctx_t *p_ctx,
                               const char       *p_dev_name,
                               aw_bool_t            format_req)
{
    if (p_ctx == NULL || p_dev_name == NULL) {
        return NULL;
    }

    bzero(p_ctx, sizeof(*p_ctx));

    p_ctx->logger.pfn_init   = __log_blk_init;
    p_ctx->logger.pfn_deinit = __log_blk_deinit;
    p_ctx->logger.print      = __log_blk_printf;
    p_ctx->logger.pfn_export = __log_blk_export;
    p_ctx->format_req        = format_req;

    snprintf(p_ctx->dev_name, __LOG_BLK_DEV_NAME_MAX, "%s", p_dev_name);

    return &p_ctx->logger;
}




