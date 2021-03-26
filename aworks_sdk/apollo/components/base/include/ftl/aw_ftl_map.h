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
 * \brief 地址映射功能
 *
 * \internal
 * \par modification history:
 * - 1.00 2017-4-15   vih, first implementation.
 * \endinternal
 */

#ifndef __AW_FTL_MAP_H
#define __AW_FTL_MAP_H

/**
 * \addtogroup grp_aw_if_ftl_map
 * \copydoc aw_ftl_map.h
 * @{
 */

#include "aworks.h"


#ifdef __cplusplus
extern "C" {
#endif


/** \brief NOR FLASH类型扇区大小固定为512 */
#define AW_FTL_NOR_SECTOR_SIZE        512

/** \brief 未分配物理块编号 */
#define AW_FTL_UNASSIGN_BLK_NO        0xFFFF

/** \brief 未分配扇区编号 */
#define AW_FTL_UNASSIGN_SECTOR_NO     0xFFFFFFFF

/**
 * \brief FTL物理块标签
 * \anchor aw_ftl_block_tag
 * @ {
 */

/** \brief 物理块空闲类型标签 */
#define AW_FTL_BLOCK_TAG_FREE         0xFF

/** \brief 系统信息块类型标签 */
#define AW_FTL_BLOCK_TAG_SYS_INFO     0xAA

/** \brief 日志块类型标签 */
#define AW_FTL_BLOCK_TAG_LOG          0x77

/** \brief 数据块类型标签 */
#define AW_FTL_BLOCK_TAG_DATA         0x55

/** 
 * \brief 第一阶段，数据块正在从旧块写入数据，
 *        完成后写入AW_FTL_BLOCK_TAG_DATA_IN_LOG 
 */
#define AW_FTL_BLOCK_TAG_DATA_IN_BLK  0x5F

/** 
 * \brief 第二阶段，数据块正在从日志块写入数据，
 *        完成后写入AW_FTL_BLOCK_TAG_DATA 
 */
#define AW_FTL_BLOCK_TAG_DATA_IN_LOG  0x57

/** \brief 块数据需要清理标志 */
#define AW_FTL_BLOCK_TAG_GARBAGE      0x00

/**
 * @} aw_ftl_block_tag
 */


/**
 * \brief FTL扇区标签
 * \anchor aw_ftl_sector_tag
 * @ {
 */

/** \brief 扇区空闲标志 */
#define AW_FTL_SECTOR_TAG_FREE        0xFF

/** \brief 扇区数据无效标志 */
#define AW_FTL_SECTOR_TAG_GARBAGE     0x00

/** \brief 扇区存储数据的标志 */
#define AW_FTL_SECTOR_TAG_DATA        0x44

/**
 * @} aw_ftl_sector_tag
 */


/**
 * \brief FTL逻辑块信息
 */
struct aw_ftl_logic_blk_info {

    /** \brief 物理块信息 */
    struct aw_ftl_blk_info *p_pyh_blk;

    /** \brief 扇区的使用标志 */
    uint8_t *sector_be_used;

    /** \brief 扇区的脏标志 */
    uint8_t *sector_is_dirty;

    /** \brief 被使用的扇区数量 */
    uint8_t sector_use_cnt;

    /** \brief 脏扇区的数量 */
    uint8_t sector_dirty_cnt;
};

struct aw_ftl_log_map_info {
    uint16_t lbn;
    uint16_t sn;
};

/**
 * \brief FTL日志块信息
 */
struct aw_ftl_log_blk_info {

    /** \brief 日志块的映射信息 */
    struct aw_ftl_log_map_info *map;

    /** \brief 物理块信息 */
    struct aw_ftl_blk_info *p_pyh_blk;

    /** \brief 该日志块扇区的使用标志 */
    uint8_t *sector_be_used;

    /** \brief 该日志块扇区的脏标志 */
    uint8_t *sector_is_dirty;

    /** \brief 该日志块被使用的扇区数量 */
    uint8_t sector_use_cnt;

    /** \brief 该日志块脏扇区的数量 */
    uint8_t sector_dirty_cnt;
};

struct aw_ftl_dev;
struct aw_ftl_sector_tag;

aw_err_t aw_ftl_map_table_init (struct aw_ftl_dev *p_ftl);
void aw_ftl_map_table_deinit (struct aw_ftl_dev *p_ftl);

aw_bool_t aw_ftl_logic_blk_is_empty (struct aw_ftl_dev *p_ftl, uint16_t lbn);

aw_bool_t aw_ftl_logic_sector_be_used (struct aw_ftl_dev *p_ftl,
                                       uint16_t           lbn,
                                       uint8_t            sector);

aw_bool_t aw_ftl_logic_sector_is_dirty (struct aw_ftl_dev *p_ftl,
                                        uint16_t           lbn,
                                        uint8_t            sector);

aw_bool_t aw_ftl_log_sector_is_dirty (struct aw_ftl_dev *p_ftl,
                                      uint16_t           log,
                                      uint8_t            sector);


/**
 * \brief 获取一个新的逻辑块
 *
 * \param[in] p_ftl : FTL设备结构体
 * \param[in] lbn   : 逻辑块编号
 *
 * \retval AW_OK    : 操作成功
 * \retval -ENODEV  : 没有空闲的物理块
 * \retval ELSE     : MTD操作失败
 */
int aw_ftl_new_logic_blk_get (struct aw_ftl_dev *p_ftl, uint16_t lbn);

/**
 * \brief 向逻辑块写入一个扇区数据和标签
 *
 * \param[in] p_ftl  : FTL设备结构体
 * \param[in] lbn    : 逻辑编号
 * \param[in] idx    : 扇区数据位置索引
 * \param[in] data   : 要写入的数据
 * \param[in] tag    : 要写入的标签，为空则不写标签
 *
 * \retval AW_OK : 操作成功
 * \retval ELSE  : MTD操作失败
 */
int aw_ftl_logic_sector_write_with_tag (struct aw_ftl_dev         *p_ftl,
                                        uint16_t                   lbn,
                                        uint16_t                   idx,
                                        uint8_t                   *data,
                                        struct aw_ftl_sector_tag  *tag);

/**
 * \brief 将脏标签写入扇区
 *
 * \param[in] p_ftl : FTL设备结构体
 * \param[in] lbn   : 逻辑块编号
 * \param[in] idx   : 扇区号(所在页索引 - sectors_per_blk_hdr)
 *
 * \retval AW_OK : 操作成功
 * \retval ELSE  : MTD操作失败
 */
int aw_ftl_logic_sector_dirty_tag_write (struct aw_ftl_dev *p_ftl,
                                         uint16_t           lbn,
                                         uint16_t           idx);

aw_err_t aw_ftl_logic_sector_read (struct aw_ftl_dev *p_ftl,
                                   uint16_t           lbn,
                                   uint16_t           idx,
                                   uint8_t           *data);

/**
 * \brief 将逻辑块数据写到日志块中,同时也会写tag
 *
 * \param[in] p_ftl: FTL设备结构体
 * \param[in] lbn  : 逻辑块编号
 * \param[in] idx  : 扇区数据位置索引
 * \param[in] data : 数据
 *
 * \retval AW_OK : 操作成功
 * \retval ELSE  : MTD操作失败
 */
int aw_ftl_log_sector_write (struct aw_ftl_dev *p_ftl,
                             uint16_t           lbn,
                             uint16_t           idx,
                             uint8_t           *data);

/**
 * \brief 从日志块重写指定逻辑块扇区数据,同时也会写tag，会将旧数据标记为dirty
 *
 * \param[in] p_ftl  : FTL设备结构体
 * \param[in] lbn    : 逻辑块编号
 * \param[in] idx    : 扇区数据位置索引
 * \param[in] data   : 数据
 *
 * \retval AW_OK : 操作成功
 * \retval ELSE  : MTD操作失败
 */
int aw_ftl_log_sector_rewrite (struct aw_ftl_dev   *p_ftl,
                               uint16_t             lbn,
                               uint16_t             idx,
                               uint8_t             *data);

/**
 * \brief 日志扇区标签信息写脏信息
 *
 * \param[in] p_ftl   : FTL设备结构体
 * \param[in] log_blk : 日志块号
 * \param[in] sector  : 扇区号(所在页索引 - sectors_per_blk_hdr)
 *
 * \retval AW_OK : 操作成功
 * \retval ELSE  : MTD操作失败
 */
int aw_ftl_log_sector_dirty_tag_write (struct aw_ftl_dev *p_ftl,
                                       uint16_t           log_blk,
                                       uint16_t           sector);

/**
 * \brief 从日志块读取制定逻辑块扇区数据
 *
 * \param[in] p_ftl  : FTL设备结构体
 * \param[in] lbn    : 逻辑块编号
 * \param[in] idx    : 扇区数据位置索引
 * \param[in] data   : 读取回来的数据
 *
 * \retval AW_OK : 操作成功
 * \retval ELSE  : MTD操作失败
 */
int aw_ftl_log_sector_read (struct aw_ftl_dev *p_ftl,
                            uint16_t           lbn,
                            uint16_t           idx,
                            uint8_t           *data);

/**
 * \brief 合并指定的逻辑的日志块数据
 *
 * \param[in]  p_ftl  : FTL设备结构体
 * \param[in]  lbn    : 逻辑块
 *
 * \retval AW_OK : 操作成功
 * \retval ELSE  : MTD操作失败
 */
int aw_ftl_log_blk_merge (struct aw_ftl_dev *p_ftl, uint16_t lbn);

/**
 * \brief 合并所有日志块数据
 *
 * \param[in]  p_ftl  : FTL设备结构体
 *
 * \retval AW_OK : 操作成功
 * \retval ELSE  : MTD操作失败
 */
int aw_ftl_log_blk_merge_all (struct aw_ftl_dev *p_ftl);

/**
 * \brief 获取逻辑块的具体信息 
 *
 * \param[in]   p_ftl  : FTL设备结构体
 * \param[out]  lbn    : 逻辑块
 *
 * \retval AW_OK : 操作成功
 * \retval ELSE  : MTD操作失败
 */
int aw_ftl_logic_info_get (struct aw_ftl_dev *p_ftl, uint16_t lbn);

/**
 * \brief 获取日志块的具体信息 
 *
 * \param[in]  p_ftl  : FTL设备结构体
 * \param[in]  log    : 日志块
 *
 * \retval AW_OK : 操作成功
 * \retval ELSE  : MTD操作失败
 */
int aw_ftl_log_info_get (struct aw_ftl_dev *p_ftl, uint16_t log);


/** @}  grp_aw_if_ftl_map */

#ifdef __cplusplus
}
#endif

#endif /* __AW_FTL_MAP_H */



/* end of file */
