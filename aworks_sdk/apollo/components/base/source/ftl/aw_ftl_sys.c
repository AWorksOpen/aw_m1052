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
 * \brief system information
 *
 * \internal
 * \par modification history:
 * - 1.00 17-04-15  vih, first implementation.
 * \endinternal
 */

#include "aworks.h"
#include "string.h"

#include "ftl/aw_ftl_core.h"
#include "ftl/aw_ftl_sys.h"
#include "ftl/aw_ftl_utils.h"
#include "ftl/aw_ftl_map.h"
#include "ftl/aw_ftl_flash.h"

/******************************************************************************/
#define PFX "FTL-sys"
#define __MSG_MASK      AW_FTL_MSG_MASK_SYS
/******************************************************************************/
struct __ftl_sys_tag {
    uint8_t  flag;
    uint8_t  sn;

    uint8_t  type;
#define __FTL_SYS_INFO_TYPE_PBN   1
#define __FTL_SYS_INFO_TYPE_LOG   2
#define __FTL_SYS_INFO_TYPE_LBN   3

};

/******************************************************************************/
/* 将当前存储的系统信息设置为dirty信息 */
int aw_ftl_sys_info_clear (struct aw_ftl_dev *p_ftl)
{
    uint32_t i = 0;
    struct __ftl_sys_tag  sys_tag;
    aw_ftl_sys_info_t *p_sys_info = &p_ftl->sys_info;

    if (!p_sys_info->pbn_valid || p_sys_info->need_update) {

        /* 系统信息正在等待更新时，又重新写入数据则会重新等待更新的时间 */
        if (p_sys_info->time_auto_renew &&
            p_sys_info->waiting_update) {
            aw_ftl_defer_job_stop(p_ftl);
            aw_ftl_defer_job_start(p_ftl, p_sys_info->update_time);
        }
        return AW_OK;
    }

    /* 如果只需要1块系统信息块，则剩余扇区仍然足够写入系统数据的话则还是写入该块 */
    if (p_sys_info->nsys_blk == 1) {

        /* 如果扇区数不足以写入一次数据设置旧的系统信息块为dirty，重新分配新块 */
        if (p_sys_info->sector_use_cnt > p_ftl->sectors_per_blk -
            p_sys_info->need_sectors) {

            AW_FTL_MSG(__MSG_MASK, "system information block(pbn:%d) is full, "
                     "set this block as garbage. ",
                     p_sys_info->array[0]);

            aw_ftl_dirty_node_add(p_ftl,
                                    &p_ftl->pbt[p_sys_info->array[0] - p_ftl->first_pbn]);

            p_sys_info->array[0] = 0;
            p_sys_info->blk_valid[0] = AW_FALSE;
            p_sys_info->pbn_valid = AW_FALSE;

            goto __exit;
        }

        /* 写入新系统信息前，先将保存的旧数据设为garbage */
        for (i = 0;
             i < p_sys_info->need_sectors &&
             p_sys_info->sector_use_cnt >= p_sys_info->need_sectors;
             i ++) {
            sys_tag.flag = AW_FTL_SECTOR_TAG_GARBAGE;
            sys_tag.type = 0xff;
            sys_tag.sn   = 0xff;
            aw_ftl_flash_page_write_with_tag(p_ftl,
                                          p_sys_info->array[0],
                                          p_sys_info->sector_use_cnt - i - 1,
                                          NULL,
                                          &sys_tag,
                                          sizeof(sys_tag));
        }

    /* 
     * 如果需要多个块才能保存一次系统信息，则每次更新都重新分配系统块，
     * 将旧块设置为garbage 
     */
    } else {
        p_sys_info->pbn_valid = AW_FALSE;

        for (i = 0; i < p_sys_info->nsys_blk; i++) {

            aw_ftl_dirty_node_add(p_ftl,
                                    &p_ftl->pbt[p_sys_info->array[i] - p_ftl->first_pbn]);

            p_sys_info->array[i] = 0;
            p_sys_info->blk_valid[i] = AW_FALSE;
        }

        AW_FTL_MSG(__MSG_MASK, "%s : clear all sys blk.", p_ftl->bdev.name);
    }

__exit:

    if (p_sys_info->pbn_valid) {
        AW_FTL_MSG(__MSG_MASK, "clear system information (pbn:%d, use sectors:%d, "
                 "remain sectors:%d). ",
                  p_sys_info->array[0],
                  p_sys_info->need_sectors,
                  p_ftl->sectors_per_blk - p_sys_info->sector_use_cnt);
    }

    p_sys_info->need_update = AW_TRUE;

    if (p_sys_info->update_time) {
        aw_ftl_defer_job_start(p_ftl, p_sys_info->update_time);

        if (p_sys_info->time_auto_renew) {
            p_sys_info->waiting_update = AW_TRUE;
        }
    }

    return AW_OK;
}

/* 将逻辑块配置信息存储到flash中 */
aw_local int __ftl_sys_lbn_info_update (struct aw_ftl_dev *p_ftl)
{
    int ret;
    uint8_t  sn;
    uint8_t  blk_no = 0, sector = 0;
    uint16_t tmp, bytes;
    uint32_t i, cnt;
    struct __ftl_sys_tag  sys_tag;
    aw_ftl_sys_info_t *p_sys_info = &p_ftl->sys_info;

    tmp   = (p_ftl->sectors_per_blk + 7) / 8;
    bytes = (sizeof(uint8_t) << 1) + (tmp << 1);

    memset(p_ftl->buf, 0, p_ftl->sector_size);

    blk_no = p_sys_info->sector_use_cnt / p_ftl->sectors_per_blk;

    for (i = 0, cnt = 0, sn = 0;
         i < p_ftl->ndata_blocks;
         i++) {

        memcpy(p_ftl->buf + cnt, &p_ftl->logic_tbl[i].sector_use_cnt, sizeof(uint8_t));
        cnt += sizeof(uint8_t);

        memcpy(p_ftl->buf + cnt, &p_ftl->logic_tbl[i].sector_dirty_cnt, sizeof(uint8_t));
        cnt += sizeof(uint8_t);

        memcpy(p_ftl->buf + cnt, p_ftl->logic_tbl[i].sector_be_used, tmp);
        cnt += tmp;

        memcpy(p_ftl->buf + cnt, p_ftl->logic_tbl[i].sector_is_dirty, tmp);
        cnt += tmp;

        /* 满一个扇区的数据则写入flash */
        if (cnt > p_ftl->sector_size - bytes ||
            (cnt > 0 && i == p_ftl->ndata_blocks - 1)) {

            if (!(p_sys_info->sector_use_cnt % p_ftl->sectors_per_blk) &&
                p_sys_info->sector_use_cnt) {

                blk_no = p_sys_info->sector_use_cnt / p_ftl->sectors_per_blk;

                if (blk_no >= p_sys_info->nsys_blk) {
                    AW_FTL_ERR_MSG(AW_FTL_MSG_MASK_ERR, "system information block not enough");
                    return -AW_ENOMEM;
                }
            }

            sys_tag.flag = AW_FTL_SECTOR_TAG_DATA;
            sys_tag.type = __FTL_SYS_INFO_TYPE_LBN;
            sys_tag.sn   = sn;

            sector = (p_sys_info->sector_use_cnt % p_ftl->sectors_per_blk);

            ret = aw_ftl_flash_page_write_with_tag(p_ftl,
                                                p_sys_info->array[blk_no],
                                                sector,
                                                p_ftl->buf,
                                                &sys_tag,
                                                sizeof(sys_tag));

            memset(p_ftl->buf, 0, p_ftl->sector_size);

            cnt = 0;
            p_sys_info->sector_use_cnt++;
            sn++;
        }
    }

    return ret;
}

/* 将日志块配置信息存储到flash中 */
aw_local int __ftl_sys_log_info_update (struct aw_ftl_dev *p_ftl)
{
    int ret;
    uint32_t cnt;
    uint32_t i;
    uint8_t sn;
    uint16_t bytes;
    uint16_t tmp, tmp1;
    struct __ftl_sys_tag  sys_tag;
    uint8_t  blk_no = 0, sector = 0;
    aw_ftl_sys_info_t *p_sys_info = &p_ftl->sys_info;

    tmp   = (p_ftl->sectors_per_blk + 7) / 8;

    bytes = (sizeof(uint16_t) << 1) * p_ftl->sectors_per_blk +
            (sizeof(uint8_t)  << 1) + (tmp << 1);

    /* 保存日志块信息 */
    memset(p_ftl->buf, 0, p_ftl->sector_size);

    blk_no = p_sys_info->sector_use_cnt / p_ftl->sectors_per_blk;

    for (i = 0, cnt = 0, sn = 0;
         i < p_ftl->nlog_blocks;
         i++) {

        memcpy(p_ftl->buf + cnt, &p_ftl->log_tbl[i].sector_use_cnt, sizeof(uint8_t));
        cnt += sizeof(uint8_t);

        memcpy(p_ftl->buf + cnt, &p_ftl->log_tbl[i].sector_dirty_cnt, sizeof(uint8_t));
        cnt += sizeof(uint8_t);

        memcpy(p_ftl->buf + cnt, p_ftl->log_tbl[i].sector_be_used, tmp);
        cnt += tmp;

        memcpy(p_ftl->buf + cnt, p_ftl->log_tbl[i].sector_is_dirty, tmp);
        cnt += tmp;

        for (tmp1 = 0; tmp1 < p_ftl->sectors_per_blk; tmp1++) {
            memcpy(p_ftl->buf + cnt, &p_ftl->log_tbl[i].map[tmp1].lbn, sizeof(uint16_t));
            cnt += sizeof(uint16_t);

            memcpy(p_ftl->buf + cnt, &p_ftl->log_tbl[i].map[tmp1].sn, sizeof(uint16_t));
            cnt += sizeof(uint16_t);
        }

        /* 满一个扇区的数据则写入flash */
        if (cnt > p_ftl->sector_size - bytes ||
            (cnt > 0 && i == p_ftl->nlog_blocks - 1)) {

            if (!(p_sys_info->sector_use_cnt % p_ftl->sectors_per_blk) &&
                p_sys_info->sector_use_cnt) {

                blk_no = p_sys_info->sector_use_cnt / p_ftl->sectors_per_blk;

                if (blk_no >= p_sys_info->nsys_blk) {
                    AW_FTL_ERR_MSG(AW_FTL_MSG_MASK_ERR, "system information block not enough");
                    return -AW_ENOMEM;
                }
            }

            sys_tag.flag = AW_FTL_SECTOR_TAG_DATA;
            sys_tag.type = __FTL_SYS_INFO_TYPE_LOG;
            sys_tag.sn   = sn;

            sector = (p_sys_info->sector_use_cnt % p_ftl->sectors_per_blk);

            ret = aw_ftl_flash_page_write_with_tag(p_ftl,
                                                p_sys_info->array[blk_no],
                                                sector,
                                                p_ftl->buf,
                                                &sys_tag,
                                                sizeof(sys_tag));

            memset(p_ftl->buf, 0, p_ftl->sector_size);

            cnt = 0;
            p_sys_info->sector_use_cnt++;
            sn++;
        }
    }

    return ret;
}

/* 将系统配置信息存储到flash中 */
int aw_ftl_sys_info_update (struct aw_ftl_dev *p_ftl)
{
    int      ret;
    uint32_t i;

    aw_ftl_sys_info_t *p_sys_info = &p_ftl->sys_info;

    if (!p_sys_info->need_update) {
        return AW_OK;
    }

    /* 如果自动重置等待时间标志为真，则清除正在等待更新的标志 */
    if (p_sys_info->time_auto_renew) {
        p_sys_info->waiting_update = AW_FALSE;
    }

    /* 如果系统信息块无效则分配一个新块作为系统信息块，该系统块占用的是保留块 */
    if (!p_sys_info->pbn_valid) {
        aw_ftl_blk_info_t   *p_pb;
        struct aw_ftl_blk_tag blk_tag;

        p_sys_info->sector_use_cnt = 0;

        /* 分配系统信息块，块标签的log写入系统块号 */
        for (i = 0; i < p_sys_info->nsys_blk; i++) {
            p_pb = aw_ftl_free_node_get(p_ftl);

            p_sys_info->pbn_valid = AW_TRUE;
            p_sys_info->array[i]        = p_pb->pbn;

            blk_tag.flag = AW_FTL_BLOCK_TAG_SYS_INFO;
            blk_tag.lbn  = 0xffff;
            blk_tag.log  = i;

            ret = aw_ftl_blk_hdr_write(p_ftl, p_pb->pbn, NULL, &blk_tag);

            if (ret == AW_OK) {
                AW_FTL_MSG(__MSG_MASK,
                        "get a new block(pbn:%d) as system information block.",
                         p_sys_info->array[i]);
            } else {
                return ret;
            }
        }
    }

    ret = __ftl_sys_lbn_info_update(p_ftl);
    if (ret != AW_OK) {
        return ret;
    }

    ret = __ftl_sys_log_info_update(p_ftl);
    if (ret != AW_OK) {
        return ret;
    }

    AW_FTL_MSG(__MSG_MASK,
            "update new system information and map table(per %dmS). ",
             p_sys_info->update_time);

    /* 更新完毕 */
    p_sys_info->need_update = AW_FALSE;

    return ret;
}


/* 从flahs中下载系统配置信息 */
int aw_ftl_sys_info_download (struct aw_ftl_dev *p_ftl)
{
    int ret = AW_ERROR;
    uint32_t i = 0;
    uint32_t cnt;
    uint8_t  lbn_sn = 0, log_sn = 0;
    uint16_t tmp, tmp1, bytes;

    uint8_t  blk_no = 0;
    uint8_t  sectors;
    aw_ftl_sys_info_t *p_sys_info = &p_ftl->sys_info;
    struct __ftl_sys_tag sys_tag;

    for (blk_no = 0; blk_no < p_sys_info->nsys_blk; blk_no++) {

        if (!p_sys_info->blk_valid[blk_no]) {
            return -AW_EINVAL;
        }
    }

    for (blk_no = 0; blk_no < p_sys_info->nsys_blk; blk_no++) {

        for (sectors = 0; sectors < p_ftl->sectors_per_blk; sectors++) {

            /* 获取物理块信息 */
            memset(p_ftl->buf, 0, p_ftl->sector_size);
            aw_ftl_flash_page_read_with_tag(p_ftl,
                                         p_sys_info->array[blk_no],
                                         sectors,
                                         p_ftl->buf,
                                         &sys_tag,
                                         sizeof(sys_tag));

            if (sys_tag.flag == AW_FTL_SECTOR_TAG_GARBAGE) {
                p_sys_info->sector_use_cnt++;
                ret = -AW_EINVAL;

                continue;
            } else if (sys_tag.flag == AW_FTL_SECTOR_TAG_FREE) {

                if (blk_no != p_sys_info->nsys_blk - 1) {
                    AW_FTL_ERR_MSG(AW_FTL_MSG_MASK_ERR, "system information block invalid");
                    return -AW_EINVAL;
                }

                break;

            } else if (sys_tag.flag == AW_FTL_SECTOR_TAG_DATA) {
                p_sys_info->sector_use_cnt++;

                 if (sys_tag.type == __FTL_SYS_INFO_TYPE_LBN) {

                    if (lbn_sn == 0 && sys_tag.sn == 0) {
                        i = 0;    /* i表示逻辑块号 */
                    } else if (lbn_sn != sys_tag.sn) {
                        AW_FTL_ERR_MSG(AW_FTL_MSG_MASK_ERR, "system information block invalid");
                        return -AW_EINVAL;
                    }

                    /* 保存逻辑块信息 */
                    tmp   = (p_ftl->sectors_per_blk + 7) >> 3;
                    bytes = (sizeof(uint8_t) << 1) + (tmp << 1);

                    for (cnt = 0;
                         i < p_ftl->ndata_blocks;
                         i++) {

                        memcpy(&p_ftl->logic_tbl[i].sector_use_cnt, 
                                p_ftl->buf + cnt, 
                                sizeof(uint8_t));
                        cnt += sizeof(uint8_t);

                        memcpy(&p_ftl->logic_tbl[i].sector_dirty_cnt, 
                                p_ftl->buf + cnt, 
                                sizeof(uint8_t));
                        cnt += sizeof(uint8_t);

                        memcpy(p_ftl->logic_tbl[i].sector_be_used, 
                               p_ftl->buf + cnt, 
                               tmp);
                        cnt += tmp;

                        memcpy(p_ftl->logic_tbl[i].sector_is_dirty, 
                               p_ftl->buf + cnt, 
                               tmp);
                        cnt += tmp;

                        if (cnt > p_ftl->sector_size - bytes ||
                            (cnt > 0 && i == p_ftl->ndata_blocks - 1)) {

                            lbn_sn++;
                            i++;

                            ret = AW_OK;
                            break;
                        }
                    }
                } else if (sys_tag.type == __FTL_SYS_INFO_TYPE_LOG) {

                    if (log_sn == 0 && sys_tag.sn == 0) {
                        i = 0;
                    } else if (log_sn != sys_tag.sn) {
                        AW_FTL_ERR_MSG(AW_FTL_MSG_MASK_ERR, "system information block invalid");
                        return -AW_EINVAL;
                    }

                    /* 保存日志块信息 */
                    tmp   = (p_ftl->sectors_per_blk + 7) >> 3;

                    bytes = (sizeof(uint16_t) << 1) * p_ftl->sectors_per_blk +
                            (sizeof(uint8_t)  << 1) + (tmp << 1);

                    for (cnt = 0;
                         i < p_ftl->nlog_blocks;
                         i++) {

                        memcpy(&p_ftl->log_tbl[i].sector_use_cnt, 
                                p_ftl->buf + cnt, 
                                sizeof(uint8_t));
                        cnt += sizeof(uint8_t);

                        memcpy(&p_ftl->log_tbl[i].sector_dirty_cnt, 
                                p_ftl->buf + cnt,
                                sizeof(uint8_t));
                        cnt += sizeof(uint8_t);

                        memcpy(p_ftl->log_tbl[i].sector_be_used, 
                               p_ftl->buf + cnt,
                               tmp);
                        cnt += tmp;

                        memcpy(p_ftl->log_tbl[i].sector_is_dirty, p_ftl->buf + cnt, tmp);
                        cnt += tmp;

                        for (tmp1 = 0; tmp1 < p_ftl->sectors_per_blk; tmp1++) {
                            memcpy(&p_ftl->log_tbl[i].map[tmp1].lbn, 
                                    p_ftl->buf + cnt, 
                                    sizeof(uint16_t));
                            cnt += sizeof(uint16_t);

                            memcpy(&p_ftl->log_tbl[i].map[tmp1].sn, 
                                    p_ftl->buf + cnt, 
                                    sizeof(uint16_t));
                            cnt += sizeof(uint16_t);
                        }

                        if (cnt > p_ftl->sector_size - bytes ||
                            (cnt > 0 && i == p_ftl->nlog_blocks - 1)) {

                            log_sn++;

                            ret = AW_OK;
                            break;
                        }
                    }
                }
            }
        }
    }

    return ret;
}

/* defer job 的回调函数 */
aw_local void __ftl_sys_info_update_job_call (void *p_arg)
{
    struct aw_ftl_dev *p_ftl = p_arg;

    AW_MUTEX_LOCK(p_ftl->op_mutex, AW_SEM_WAIT_FOREVER);
    aw_ftl_sys_info_update(p_ftl);
    AW_MUTEX_UNLOCK(p_ftl->op_mutex);
}

aw_err_t aw_ftl_sys_info_init (struct aw_ftl_dev *p_ftl)
{
    aw_ftl_sys_info_t   *p_sys_info = &p_ftl->sys_info;
    uint16_t             size = 0;
    int                  ret;
    uint32_t             lbn_need_bytes;
    uint32_t             log_need_bytes;
    uint8_t              need_sectors = 0;

    lbn_need_bytes = (sizeof(uint8_t)  * 2 +
                     (p_ftl->sectors_per_blk + 7) / 8 * 2) *
                      p_ftl->ndata_blocks;

    need_sectors = lbn_need_bytes / p_ftl->sector_size +
                  (lbn_need_bytes % p_ftl->sector_size ? 1 : 0);

    log_need_bytes = (sizeof(uint8_t)  * 2 +
                     (p_ftl->sectors_per_blk + 7) / 8 * 2 +
                      sizeof(uint16_t) * 2 * p_ftl->sectors_per_blk) *
                      p_ftl->nlog_blocks;

    need_sectors += log_need_bytes / p_ftl->sector_size +
                   (log_need_bytes % p_ftl->sector_size ? 1 : 0);

    p_sys_info->need_sectors = need_sectors;
    p_sys_info->nsys_blk     = (need_sectors + (p_ftl->sectors_per_blk - 1)) /
                                p_ftl->sectors_per_blk;

    if (NULL == p_sys_info->blk_valid) {
        size = sizeof(aw_bool_t) * p_sys_info->nsys_blk;

        p_sys_info->blk_valid = (aw_bool_t *)aw_ftl_mem_alloc(p_ftl, size);
        if (NULL == p_sys_info->blk_valid) {
            goto __err_exit;
        }
    }

    if (NULL == p_sys_info->array) {
        size = sizeof(uint16_t) * p_sys_info->nsys_blk;

        p_sys_info->array = (uint16_t *)aw_ftl_mem_alloc(p_ftl, size);
        if (NULL == p_sys_info->array) {
            goto __err_exit;
        }
    }

    p_sys_info->pbn_valid   = AW_FALSE;
    p_sys_info->need_update = AW_TRUE;

    /* 默认值，关闭自动更新，关闭自动重置，阀值均为85% */
    p_sys_info->update_time     = 0;
    p_sys_info->time_auto_renew = 0;

    ret = aw_ftl_defer_job_init(p_ftl, __ftl_sys_info_update_job_call);

    return ret;

__err_exit:
    aw_ftl_mem_free(p_ftl, (void **)&p_sys_info->blk_valid);
    aw_ftl_mem_free(p_ftl, (void **)&p_sys_info->array);

    return -AW_ENOMEM;
}

aw_err_t aw_ftl_sys_info_deinit (struct aw_ftl_dev *p_ftl)
{
    aw_ftl_sys_info_t *p_sys_info = &p_ftl->sys_info;

    aw_ftl_mem_free(p_ftl, (void **)&p_sys_info->blk_valid);
    aw_ftl_mem_free(p_ftl, (void **)&p_sys_info->array);

    return AW_OK;
}

















