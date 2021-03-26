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
 * \brief ftl flash interface
 *
 * \internal
 * \par modification history:
 * - 1.00 17-04-15   vih, first implementation.
 * \endinternal
 */

#ifndef __AW_FTL_FLASH_H
#define __AW_FTL_FLASH_H

/**
 * \addtogroup grp_aw_if_ftl_flash
 * \copydoc aw_ftl_flash.h
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

struct aw_ftl_blk_tag {
    uint8_t  flag;
    uint8_t  log;
    uint16_t lbn;
};

struct aw_ftl_sector_tag {
    uint8_t  flag;
    uint8_t  sn;
    uint16_t lbn;
};

/**
 * \brief 物理块头信息和标签读取
 *
 * \param[in] p_ftl     : FTL设备结构体
 * \param[in] pbn       : 物理块编号
 * \param[in] p_blk_hdr : 指向物理块头信息结构体，可为NULL
 * \param[in] p_tag     : 页0的标签，可为NULL
 *
 * \retval AW_OK : 读取成功
 * \retval ELSE      : MTD读取失败
 */
int aw_ftl_blk_hdr_read (struct aw_ftl_dev     *p_ftl,
                         uint16_t               pbn,
                         struct aw_ftl_pb_hdr  *p_blk_hdr,
                         struct aw_ftl_blk_tag *p_tag);

/**
 * \brief 物理块头信息和标签写入
 *
 * \param[in] p_ftl     : FTL设备结构体
 * \param[in] pbn       : 物理块编号
 * \param[in] p_blk_hdr : 指向物理块头信息结构体，可为NULL
 * \param[in] p_tag     : 页0的标签，可为NULL
 *
 * \retval AW_OK : 写入成功
 * \retval ELSE  : MTD写入失败
 */
int aw_ftl_blk_hdr_write (struct aw_ftl_dev     *p_ftl,
                          uint16_t               pbn,
                          struct aw_ftl_pb_hdr  *p_blk_hdr,
                          struct aw_ftl_blk_tag *p_tag);

/**
 * \brief 物理页数据读取
 *
 * \param[in] p_ftl : FTL设备结构体
 * \param[in] pbn   : 物理块编号
 * \param[in] idx   : 所在页索引
 * \param[in] tag   : 读取回来的标签值
 *
 * \retval AW_OK : 操作成功
 * \retval ELSE  : MTD操作失败
 */
int aw_ftl_flash_page_read_with_tag (struct aw_ftl_dev *p_ftl,
                                     uint16_t           pbn,
                                     uint16_t           idx,
                                     uint8_t           *data,
                                     void              *tag,
                                     uint8_t            tag_size);

/**
 * \brief 向物理页写入整页大小的数据
 *
 * \param[in] p_ftl    : FTL设备结构体
 * \param[in] pbn      : 物理块编号
 * \param[in] idx      : 扇区数据位置索引
 * \param[in] data     : 要写入的数据
 * \param[in] tag      : 要写入标签
 * \param[in] tag_size : 标签的大小
 *
 * \retval AW_OK : 操作成功
 * \retval ELSE  : MTD操作失败
 */
int aw_ftl_flash_page_write_with_tag (struct aw_ftl_dev *p_ftl,
                                      uint16_t           pbn,
                                      uint16_t           idx,
                                      uint8_t           *data,
                                      void              *tag,
                                      uint8_t            tag_size);

/**
 * \brief 从物理块读取一个扇区数据
 *
 * \param[in] p_ftl  : FTL设备结构体
 * \param[in] pbn    : 物理块编号
 * \param[in] idx    : 扇区数据位置索引
 * \param[in] data   : 读取回来的数据
 *
 * \retval AW_OK : 操作成功
 * \retval ELSE  : MTD操作失败
 */
int aw_ftl_flash_page_read (struct aw_ftl_dev *p_ftl,
                            uint16_t           pbn,
                            uint16_t           idx,
                            uint8_t           *data);

/**
 * \brief 扇区标签信息读取
 *
 * \param[in] p_ftl : FTL设备结构体
 * \param[in] pbn   : 物理块编号
 * \param[in] idx   : 扇区号(所在页索引 - sectors_per_blk_hdr)
 * \param[in] tag   : 读取回来的标签值
 *
 * \retval AW_OK : 操作成功
 * \retval ELSE  : MTD操作失败
 */
int aw_ftl_flash_page_tag_read (struct aw_ftl_dev        *p_ftl,
                                uint16_t                  pbn,
                                uint16_t                  idx,
                                struct aw_ftl_sector_tag *tag);

/**
 * \brief 扇区标签信息写入
 *
 * \param[in] p_ftl : FTL设备结构体
 * \param[in] pbn   : 物理块编号
 * \param[in] idx   : 扇区号(所在页索引 - sectors_per_blk_hdr)
 * \param[in] tag   : 写入的标签
 *
 * \retval AW_OK : 操作成功
 * \retval ELSE  : MTD操作失败
 */
int aw_ftl_flash_page_tag_write (struct aw_ftl_dev        *p_ftl,
                                 uint16_t                  pbn,
                                 uint16_t                  idx,
                                 struct aw_ftl_sector_tag *tag);

/**
 * \brief 物理块格式化
 *
 * \param[in] p_ftl     : FTL设备结构体
 * \param[in] pbn       : 物理块编号
 *
 * \retval AW_OK  : 操作成功
 * \retval -EPERM : 出现坏块
 * \retval -EBUSY : 擦除失败
 * \retval -EIO   : MTD操作失败
 */
int aw_ftl_blk_format (struct aw_ftl_dev *p_ftl, uint16_t  pbn);

/**
 * \brief 物理块擦除
 *
 * \param[in] p_ftl  : FTL设备结构体
 * \param[in] pbn    : 物理块编号
 *
 * \retval AW_OK     : 操作成功
 * \retval -AW_EPERM : 出现坏块
 * \retval -AW_EIO   : IO错误
 */
aw_err_t aw_ftl_mtd_block_erase (struct aw_ftl_dev *p_ftl, uint16_t  pbn);

aw_err_t aw_ftl_mtd_oob_read (struct aw_ftl_dev *p_ftl,
                              uint16_t           pbn,
                              uint16_t           page,
                              uint8_t           *data,
                              void              *tag,
                              uint8_t            tag_size);

aw_err_t aw_ftl_mtd_oob_write ( struct aw_ftl_dev *p_ftl,
                                uint16_t           pbn,
                                uint16_t           page,
                                uint8_t           *data,
                                void              *tag,
                                uint8_t            tag_size);

/** @}  grp_aw_if_ftl_flash */

#ifdef __cplusplus
}
#endif

#endif /* __AW_FTL_FLASH_H */



/* end of file */
