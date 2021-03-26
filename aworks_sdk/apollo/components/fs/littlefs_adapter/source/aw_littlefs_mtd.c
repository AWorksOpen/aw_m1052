/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

#include "AWorks.h"
#include "aw_vdebug.h"
#include "aw_mem.h"
#include "mtd/aw_mtd.h"
#include "aw_littlefs.h"
#include <string.h>

aw_local int __littlefs_mtd_read(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, void *buffer, lfs_size_t size)
{
    struct aw_mtd_info *mtd = c->context;
    aw_err_t ret;
    size_t retlen = 0;
    uint32_t offset = mtd->erase_size * block + off;

    if (buffer && (size > 0)) {
        ret = aw_mtd_read(mtd, offset, size, &retlen, buffer);
        if ((ret < 0) || (retlen < size)) {
            ret = LFS_ERR_CORRUPT;
        }
    }

    return ret;
}

aw_local int __littlefs_mtd_prog(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, const void *buffer, lfs_size_t size)
{
    struct aw_mtd_info *mtd = c->context;
    aw_err_t ret;
    size_t retlen = 0;
    uint32_t offset = mtd->erase_size * block + off;

    if (buffer && (size > 0)) {
        ret = aw_mtd_write(mtd, offset, size, &retlen, buffer);
        if ((ret < 0) || (retlen < size)) {
            ret = LFS_ERR_CORRUPT;
        }
    }
//    aw_kprintf ("w %d %d\n", block, off);

    return ret;
}

aw_local int __littlefs_mtd_erase(const struct lfs_config *c, lfs_block_t block)
{
    int ret;
    struct aw_mtd_info *mtd = c->context;
    struct aw_mtd_erase_info ei;

    ei.mtd       = mtd;
    ei.addr      = mtd->erase_size * block;
    ei.len       = mtd->erase_size;
    ei.retries   = 2;
    ei.callback  = NULL;
    ei.priv      = NULL;
    ei.next      = NULL;

    if ((ret = aw_mtd_erase(mtd, &ei)) < 0) {
        ret = LFS_ERR_IO;
    } else {
        switch (ei.state) {
        case AW_MTD_ERASE_FAILED:
            ret = LFS_ERR_CORRUPT;
            break;
        case AW_MTD_ERASE_DONE:
            break;
        default:
            ret = LFS_ERR_IO;
            break;
        }
    }
//    aw_kprintf ("e %d\n", block);

    return ret;
}

aw_local int __littlefs_mtd_sync(const struct lfs_config *c)
{
    struct aw_mtd_info *mtd = c->context;

    aw_mtd_sync(mtd);

    return LFS_ERR_OK;
}


void aw_littlefs_mtd_destory(struct aw_littlefs_volume *p_vol)
{
    aw_mem_free(p_vol->config.read_buffer);
    aw_mem_free(p_vol->config.prog_buffer);
    aw_mem_free(p_vol->config.lookahead_buffer);

}

void aw_littlefs_mtd_init(struct aw_littlefs_volume *p_vol)
{
    struct aw_mtd_info *p_mtd = p_vol->blk_dev;

    memset(&p_vol->config, 0, sizeof(struct lfs_config));

    p_vol->config.context = p_mtd;

    p_vol->config.read    = __littlefs_mtd_read;
    p_vol->config.prog    = __littlefs_mtd_prog;
    p_vol->config.erase   = __littlefs_mtd_erase;
    p_vol->config.sync    = __littlefs_mtd_sync;

    p_vol->config.read_size   = p_mtd->write_size;
    p_vol->config.prog_size   = p_mtd->write_size;
    p_vol->config.block_size  = p_mtd->erase_size;
    p_vol->config.block_count = p_mtd->size / p_mtd->erase_size;

    p_vol->config.block_cycles   = -1;
    p_vol->config.cache_size     = p_mtd->write_size * 2;
    p_vol->config.lookahead_size = 8 * 32 ;   //have to muti by 32

    p_vol->config.name_max = LFS_NAME_MAX;
    p_vol->config.file_max = LFS_FILE_MAX;
    p_vol->config.attr_max = LFS_ATTR_MAX;

    p_vol->config.read_buffer      = aw_mem_alloc(p_vol->config.cache_size);
    p_vol->config.prog_buffer      = aw_mem_alloc(p_vol->config.cache_size);
    p_vol->config.lookahead_buffer = aw_mem_align(p_vol->config.lookahead_size, 32);

#if 0
    p_vol->config.read_buffer      = NULL;
    p_vol->config.prog_buffer      = NULL;
    p_vol->config.lookahead_buffer = NULL;
#endif
}
