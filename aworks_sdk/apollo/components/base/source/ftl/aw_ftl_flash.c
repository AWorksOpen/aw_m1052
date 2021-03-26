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
#include "aw_vdebug.h"
#include "aw_delay.h"

#include "ftl/aw_ftl_core.h"
#include "ftl/aw_ftl_utils.h"
#include "ftl/aw_ftl_flash.h"
/******************************************************************************/
#define PFX "FTL-flash"
#define __MSG_MASK      AW_FTL_MSG_MASK_FLASH

#define __WARN_IF(cond)  \
    if ((cond)) {\
        AW_INFOF(("[ WARN ][%s:%d] " #cond "\n", __func__, __LINE__));\
    }

#define __ASSERT(cond)  \
    if (!(cond)) {\
        AW_INFOF(("[ ASSERT Failed ][%s:%d] " #cond "\n", \
                __func__, __LINE__)); \
        aw_mdelay(1000); \
        while(1); \
    }
/******************************************************************************/

/**
 * \brief 向物理页写入整页大小的数据
 *
 * \param[in] p_ftl    : FTL设备结构体
 * \param[in] pbn      : 物理块编号
 * \param[in] page     : 该块的页地址
 * \param[in] data     : 要写入的数据
 * \param[in] tag      : 要写入标签
 * \param[in] tag_size : 标签的大小
 *
 * \retval AW_OK : 操作成功
 * \retval ELSE  : MTD操作失败
 */
static int __flash_page_write_with_tag ( struct aw_ftl_dev *p_ftl,
                                         uint16_t           pbn,
                                         uint16_t           page,
                                         uint8_t           *data,
                                         void              *tag,
                                         uint8_t            tag_size)
{
    int      ret = -1;
    uint32_t retlen, addr;
    struct aw_mtd_oob_ops ops;
    uint32_t  size;

    __ASSERT(pbn < p_ftl->nphy_blocks);
    __ASSERT(page < p_ftl->sectors_per_blk + p_ftl->sectors_per_blk_hdr);

    if (AW_MTD_TYPE_NAND_FLASH == p_ftl->mtd->type) {
        addr = (pbn << p_ftl->log2_blk_size) +
               (page << p_ftl->log2_sector_size);

        ops.mode     = AW_MTD_OOB_MODE_AUTO;

        ops.data_buf = (void *)data;
        ops.len      = (data != NULL ? 1 << p_ftl->log2_sector_size : 0);

        ops.oob_buf  = (void *)tag;
        ops.oob_len  = tag != NULL ? tag_size : 0;

        ops.oob_offs = 0;

        ret = aw_mtd_oob_write(p_ftl->mtd, addr, &ops);
        if (ret != AW_OK) {
            AW_FTL_ERR_MSG(AW_FTL_MSG_MASK_ERR,
                            "aw_mtd_oob_write block(%d) page(%d) failed", pbn, page);
            return -AW_EINVAL;
        }

    } else if (AW_MTD_TYPE_NOR_FLASH == p_ftl->mtd->type) {

        if (data) {

            addr = (pbn << p_ftl->log2_blk_size) +
                   (page << p_ftl->log2_sector_size);

            if (page == 0) {
                size = sizeof(aw_ftl_pb_hdr_t);
            } else {
                size = 1 << p_ftl->log2_sector_size;
            }

            ret = aw_mtd_write(p_ftl->mtd,
                              addr,
                              size,
                              (size_t *)&retlen,
                              (void *)data);

            if (AW_OK != ret ||
                retlen < size) {
                AW_FTL_ERR_MSG(AW_FTL_MSG_MASK_ERR,
                        "aw_mtd_write error: %d, retlen: %d",
                        ret, retlen);
            }
        }

        if (tag) {
            addr = (pbn << p_ftl->log2_blk_size) +
                    sizeof(aw_ftl_pb_hdr_t)    +
                   (tag_size * page);

            ret = aw_mtd_write(p_ftl->mtd,
                               addr,
                               tag_size,
                               (size_t *)&retlen,
                               (void *)tag);

            if (AW_OK != ret ||
                retlen < tag_size) {
                AW_FTL_ERR_MSG(AW_FTL_MSG_MASK_ERR, "nor-flash write tag error!"
                        "(pbn:%d, ret:%d, retlen:%d)",
                        pbn,
                        ret,
                        retlen);

                return ret;
            }
        }
    }

    return ret;
}


/**
 * \brief 物理页数据读取
 *
 * \param[in] p_ftl : FTL设备结构体
 * \param[in] pbn   : 物理块编号
 * \param[in] page  : 页号
 * \param[in] tag   : 读取回来的标签值
 *
 * \retval AW_OK : 操作成功
 * \retval > 0   : 存在ecc错误，但纠正成功
 * \retval ELSE  : MTD操作失败，或ecc纠正失败
 */
static int __flash_page_read_with_tag (struct aw_ftl_dev *p_ftl,
                                       uint16_t           pbn,
                                       uint16_t           page,
                                       uint8_t           *data,
                                       void              *tag,
                                       uint8_t            tag_size)
{
    int ret = AW_OK;
    uint32_t addr, retlen;
    struct aw_mtd_oob_ops ops;
    uint32_t  size;

    __ASSERT(pbn < p_ftl->nphy_blocks);
    __ASSERT(page < p_ftl->sectors_per_blk + p_ftl->sectors_per_blk_hdr);

    if (AW_MTD_TYPE_NAND_FLASH == p_ftl->mtd->type) {

        addr = (pbn << p_ftl->log2_blk_size) + (page << p_ftl->log2_sector_size);

        ops.mode     = AW_MTD_OOB_MODE_AUTO;
        ops.oob_offs = 0;

        ops.oob_buf  = tag;
        ops.oob_len  = tag != NULL ? tag_size : 0;

        ops.data_buf = (void *)data;
        ops.len      = (data != NULL ? (1 << p_ftl->log2_sector_size) : 0);

        ret = aw_mtd_oob_read(p_ftl->mtd, addr, &ops);
        if (ret < 0) {
            AW_FTL_ERR_MSG(AW_FTL_MSG_MASK_ERR,
                    "aw_mtd_oob_read error: %d, retlen: %d", ret, retlen);
        } else if (ret > 0 && p_ftl->need_verify_blk == -1) {
            p_ftl->need_verify_blk = pbn;
        }

    } else if (AW_MTD_TYPE_NOR_FLASH == p_ftl->mtd->type) {

        if (data) {
            addr = (pbn << p_ftl->log2_blk_size) +
                   (page << p_ftl->log2_sector_size);

            if (page == 0) {
                size = sizeof(aw_ftl_pb_hdr_t);
            } else {
                size = 1 << p_ftl->log2_sector_size;
            }

            ret = aw_mtd_read(p_ftl->mtd,
                             addr,
                             size,
                             (size_t *)&retlen,
                             (void *)data);

            if (AW_OK != ret ||
                retlen < size) {
                AW_FTL_ERR_MSG(AW_FTL_MSG_MASK_ERR,
                        "aw_mtd_read error: %d, retlen: %d", ret, retlen);
            }
        }

        if (tag) {
            addr = (pbn << p_ftl->log2_blk_size) +
                    sizeof(aw_ftl_pb_hdr_t)    +
                   (tag_size * page);

            ret = aw_mtd_read(p_ftl->mtd,
                              addr,
                              tag_size,
                              (size_t *)&retlen,
                              tag);

            if (AW_OK != ret ||
                retlen < tag_size) {
                AW_FTL_ERR_MSG(AW_FTL_MSG_MASK_ERR,
                        "nor-flash read tag error: %d, retlen: %d", ret, retlen);
                return ret;
            }
        }
    }

    return ret;
}

/******************************************************************************/
aw_err_t aw_ftl_mtd_oob_read (struct aw_ftl_dev *p_ftl,
                              uint16_t           pbn,
                              uint16_t           page,
                              uint8_t           *data,
                              void              *tag,
                              uint8_t            tag_size)
{
    return __flash_page_read_with_tag(p_ftl, pbn, page, data, tag, tag_size);
}

aw_err_t aw_ftl_mtd_oob_write ( struct aw_ftl_dev *p_ftl,
                                uint16_t           pbn,
                                uint16_t           page,
                                uint8_t           *data,
                                void              *tag,
                                uint8_t            tag_size)
{
    return __flash_page_write_with_tag(p_ftl, pbn, page, data, tag, tag_size);
}

aw_err_t aw_ftl_mtd_block_erase (struct aw_ftl_dev *p_ftl, uint16_t  pbn)
{
    struct aw_mtd_erase_info  ei;
    aw_err_t    ret;

    ei.mtd      = p_ftl->mtd;
    ei.addr     = pbn << p_ftl->log2_blk_size;
    ei.len      = 1 << p_ftl->log2_blk_size;
    ei.retries  = 2;
    ei.callback = NULL;
    ei.priv     = NULL;
    ei.next     = NULL;

    ret = aw_mtd_erase(p_ftl->mtd, &ei);
    if (ret == -AW_EIO) {
        return -AW_EIO;
    } else  if (ret < 0) {
        AW_FTL_MSG(AW_FTL_MSG_MASK_UNDEF, "erase failed, the block(pbn:%d) is a bad block.", pbn);
        return -AW_EPERM;
    }
    return AW_OK;
}
/******************************************************************************/

int aw_ftl_blk_hdr_read (struct aw_ftl_dev     *p_ftl,
                         uint16_t               pbn,
                         struct aw_ftl_pb_hdr  *p_blk_hdr,
                         struct aw_ftl_blk_tag *p_tag)
{
    int ret = AW_OK;
    uint8_t *p_buf;

    if (p_blk_hdr != NULL) {
        bzero(p_ftl->buf, 1 << p_ftl->log2_sector_size);
        p_buf = p_ftl->buf;
    } else {
        p_buf = NULL;
    }

    ret = __flash_page_read_with_tag(p_ftl, pbn, 0, p_buf, p_tag, sizeof(*p_tag));
    if (p_buf != NULL) {
        memcpy(p_blk_hdr, p_buf, sizeof(*p_blk_hdr));
    }

    return ret;
}

int aw_ftl_blk_hdr_write (struct aw_ftl_dev     *p_ftl,
                          uint16_t               pbn,
                          struct aw_ftl_pb_hdr  *p_blk_hdr,
                          struct aw_ftl_blk_tag *p_tag)
{
    int ret = AW_OK;
    uint8_t *p_buf;

    if (p_blk_hdr != NULL) {
        memset(p_ftl->buf, 0, 1 << p_ftl->log2_sector_size);
        p_buf = p_ftl->buf;
        memcpy(p_buf, p_blk_hdr, sizeof(*p_blk_hdr));
    } else {
        p_buf = NULL;
    }

    ret = __flash_page_write_with_tag(p_ftl, pbn, 0, p_buf, p_tag, sizeof(*p_tag));

    return ret;
}

int aw_ftl_flash_page_read_with_tag (struct aw_ftl_dev *p_ftl,
                                     uint16_t           pbn,
                                     uint16_t           idx,
                                     uint8_t           *data,
                                     void              *tag,
                                     uint8_t            tag_size)
{
    return __flash_page_read_with_tag(  p_ftl,
                                        pbn,
                                        idx + p_ftl->sectors_per_blk_hdr,
                                        data,
                                        tag,
                                        tag_size);
}


int aw_ftl_flash_page_write_with_tag (struct aw_ftl_dev *p_ftl,
                                      uint16_t           pbn,
                                      uint16_t           idx,
                                      uint8_t           *data,
                                      void              *tag,
                                      uint8_t            tag_size)
{
    return __flash_page_write_with_tag( p_ftl,
                                        pbn,
                                        idx + p_ftl->sectors_per_blk_hdr,
                                        data,
                                        tag,
                                        tag_size);
}

int aw_ftl_flash_page_read (struct aw_ftl_dev *p_ftl,
                            uint16_t           pbn,
                            uint16_t           idx,
                            uint8_t           *data)
{
    return __flash_page_read_with_tag( p_ftl,
                                        pbn,
                                        idx + p_ftl->sectors_per_blk_hdr,
                                        data,
                                        NULL,
                                        0);
}


int aw_ftl_flash_page_tag_read (struct aw_ftl_dev       *p_ftl,
                                uint16_t                 pbn,
                                uint16_t                 idx,
                                struct aw_ftl_sector_tag *tag)
{
    return __flash_page_read_with_tag( p_ftl,
                                       pbn,
                                       idx + p_ftl->sectors_per_blk_hdr,
                                       NULL,
                                       tag,
                                       sizeof(*tag));
}



/**
 * \brief 扇区标签信息写入
 *
 * \param[in] p_ftl : FTL设备结构体
 * \param[in] pbn : 物理块编号
 * \param[in] idx : 扇区号(所在页索引 - sectors_per_blk_hdr)
 * \param[in] tag : 写入的标签
 *
 * \retval AW_OK : 操作成功
 * \retval ELSE      : MTD操作失败
 */
int aw_ftl_flash_page_tag_write (struct aw_ftl_dev       *p_ftl,
                                 uint16_t                 pbn,
                                 uint16_t                 idx,
                                 struct aw_ftl_sector_tag *tag)
{
    return __flash_page_write_with_tag( p_ftl,
                                        pbn,
                                        idx + p_ftl->sectors_per_blk_hdr,
                                        NULL,
                                        tag,
                                        sizeof(*tag));
}


int aw_ftl_blk_format (struct aw_ftl_dev *p_ftl, uint16_t  pbn)
{
    int ret = AW_OK;
    uint32_t blk = aw_ftl_blk_id_get(p_ftl, pbn);

    struct aw_ftl_pb_hdr blk_hdr;
    struct aw_ftl_pb_hdr blk_hdr_tmp;

    ret = aw_ftl_mtd_block_erase(p_ftl, pbn);
    if (ret == -AW_EPERM) {
        aw_ftl_bad_node_add(p_ftl, pbn);
    } else if (ret != AW_OK) {
        return ret;
    }

    /* 写入物理块信息头 */
    memcpy(blk_hdr.format_pattern, "AWFTL", sizeof("AWFTL"));
    blk_hdr.flags              = 0x0000;

    if (p_ftl->pbt[blk].wear_leveling < p_ftl->lowest_wear) {
        p_ftl->pbt[blk].wear_leveling = p_ftl->lowest_wear;
    }
    blk_hdr.wear_leveling_info = ++p_ftl->pbt[blk].wear_leveling;
    blk_hdr.volumn_size        = (p_ftl->ndata_blocks * p_ftl->sectors_per_blk) <<
                                  p_ftl->log2_sector_size;
    blk_hdr.log2_sector_size   = p_ftl->log2_sector_size;
    blk_hdr.log2_blk_size      = p_ftl->log2_blk_size;
    blk_hdr.first_pbn          = p_ftl->first_pbn;
    blk_hdr.nphy_blocks        = p_ftl->nphy_blocks;
    blk_hdr.ndata_blocks       = p_ftl->ndata_blocks;
    blk_hdr.nlog_blocks        = p_ftl->nlog_blocks;
    blk_hdr.nrsblocks          = p_ftl->nrsblocks;

    ret = aw_ftl_blk_hdr_write(p_ftl, pbn, &blk_hdr, NULL);
    if (ret == AW_OK) {
        ret = aw_ftl_blk_hdr_read(p_ftl, pbn, &blk_hdr_tmp, NULL);
        if (ret == AW_OK && memcmp(&blk_hdr, &blk_hdr_tmp, sizeof(blk_hdr)) == 0) {
            AW_FTL_MSG(AW_FTL_MSG_MASK_FORMAT,
                    "format flash block [ OK ]. (blk:%d,  wear-level:%d )",
                    pbn, blk_hdr.wear_leveling_info);
        } else {
            AW_FTL_ERR_MSG(AW_FTL_MSG_MASK_ERR, "read and verify format info failed.(blk:%d)", pbn);
        }
    } else {
        AW_FTL_ERR_MSG(AW_FTL_MSG_MASK_ERR, 
                       "write format info failed.(blk:%d)", pbn);
    }

    return ret;
}


