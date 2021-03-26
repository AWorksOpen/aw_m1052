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
 * \brief FLASH translation layer
 *
 * \internal
 * \par modification history:
 * - 1.00 17-04-15   vih, first implementation.
 * \endinternal
 */

#ifndef __AW_FTL_CORE_H
#define __AW_FTL_CORE_H

/**
 * \addtogroup grp_aw_if_ftl_core
 * \copydoc aw_ftl_core.h
 * @{
 */

#include "fs/aw_blk_dev.h"
#include "mtd/aw_mtd.h"
#include "aw_list.h"
#include "aw_sem.h"
#include "aw_delayed_work.h"
#include "aw_sys_defer.h"
#include "aw_refcnt.h"

#include "aw_ftl.h"
#include "aw_ftl_sys.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * \brief 物理块信息头
 */
typedef struct aw_ftl_pb_hdr {

    /** \brief AWFTL */
    uint8_t  format_pattern[6];

    /** \brief 标志，0x0000表示该物理块描述结构体有效 */
    uint16_t  flags;

    /** \brief 物理块擦除次数 */
    uint32_t wear_leveling_info;

    /** \brief 整个已格式化的大小 */
    uint32_t volumn_size;

    /** \brief log2(SECTOR_SIZE)，SECTOR_SIZE就是一个扇区的大小 */
    uint8_t  log2_sector_size;

    /** \brief log2(UNIT_SIZE)，UNIT_SIZE就是一个擦除单元的大小 */
    uint8_t  log2_blk_size;

    /** \brief 第一个物理块编号 */
    uint16_t first_pbn;

    /** \brief 格式化物理块个数 */
    uint16_t nphy_blocks;

    /** \brief 数据块单元个数 */
    uint16_t ndata_blocks;

    /** \brief 日志块个数 */
    uint16_t nlog_blocks;

    /** \brief 保留物理块个数 */
    uint16_t nrsblocks;

} aw_ftl_pb_hdr_t;

/**
 * \brief 物理块信息
 */
typedef struct aw_ftl_blk_info {

    /** \brief 链表节点 */
    struct aw_list_head node;

    /** \brief 均衡信息 */
    uint32_t wear_leveling;

    /** \brief 逻辑块编号 */
    uint16_t lbn;

    /** \brief 物理块编号 */
    uint16_t pbn;

} aw_ftl_blk_info_t;


/** \brief 前置声明 */
struct aw_ftl_logic_blk_info;
struct aw_ftl_log_blk_info;

/**
 * \brief FTL设备结构体
 */
typedef struct aw_ftl_dev {

    /** \brief BLK设备 */
    struct aw_blk_dev        bdev;

    /** \brief MTD设备 */
    struct aw_mtd_info      *mtd;

    struct aw_ftl_sys_info   sys_info;

    struct aw_refcnt         ref;

    /** \brief 坏块链 */
    struct aw_list_head      bad_blk_list;

    /** \brief 空闲块链 */
    struct aw_list_head      free_blk_list;

    /** \brief 脏块链 */
    struct aw_list_head      dirty_blk_list;

    /** \brief 链表节点 */
    struct aw_list_head      node;

    /** \brief 定时清除数据(data)工作  */
    struct aw_delayed_work   delay_work;

    struct aw_defer_djob     defer_job;

    /** \brief 用于读、写、垃圾回收的互斥锁 */
    AW_MUTEX_DECL(op_mutex);

#define __FTL_NAME_MAX     32
    char               name[__FTL_NAME_MAX];

    /** \brief 物理块表，供链表使用 */
    aw_ftl_blk_info_t            *pbt;

    /** \brief 逻辑块信息数组 */
    struct aw_ftl_logic_blk_info *logic_tbl;

    /** \brief 日志块信息数组 */
    struct aw_ftl_log_blk_info   *log_tbl;

    /** \brief 临时缓存，大小为页大小 */
    uint8_t *buf;

    /** \brief 最低的磨损均衡，当块信息丢失时使用该磨损均衡   */
    uint32_t lowest_wear;

    /** \brief 当前FTL中扇区的个数 */
    uint32_t sector_total;

    /** \brief 数据扇区的总数 */
    uint32_t data_sector_total;

    /** \brief 可用空闲数据扇区数 */
    uint32_t free_sector_total;

    /** \brief 一个扇区的大小 */
    uint32_t sector_size;

    /** \brief 擦除单元的大小 */
    uint32_t blk_size;

    /** \brief 读写单元的大小 */
    uint32_t write_size;

    uint32_t malloc_cnt;

    /** \brief 逻辑块清理的阀值，当逻辑块的dirty扇区数量达到该阀值时，会合并该块数据 */
    float    logic_clear_threshold;

    /** \brief 日志块清理的阀值，当所有日志块的扇区使用数量达到该阀值时，会合并所有日志块数据 */
    float    log_clear_threshold;

    /** \brief 第一个物理块单元编号 */
    uint16_t first_pbn;

    /** \brief 逻辑块中可用的扇区数量 */
    uint16_t sectors_per_blk;

    /** \brief 物理块单元个数 */
    uint16_t nphy_blocks;

    /** \brief 数据块单元个数 */
    uint16_t ndata_blocks;

    /** \brief 日志块单元个数 */
    uint16_t nlog_blocks;

    /** \brief 保留物理块个数 */
    uint16_t nrsblocks;

    /** \brief 出现了不正常掉电的情况，需要恢复的逻辑块 */
    uint16_t need_recovery_lbn;

    /** \brief 占用资源数量   */
    int16_t  resource;

    /** \brief 出现了ecc错误，但是能纠正的块，需要进行坏块检查   */
    int16_t  need_verify_blk;

    /** \brief physical block header占用的扇区数量 */
    uint8_t  sectors_per_blk_hdr;

    /** \brief log2(sector_size), sector_size就是一个扇区的大小 */
    uint8_t  log2_sector_size;

    /** \brief log2(blk_size)，blk_size就是一个擦除单元的大小 */
    uint8_t  log2_blk_size;

    /** \brief log2(write_size)，wirte_size就是一个读写单元的大小 */
    uint8_t  log2_write_size;

    /** \brief 逻辑块需要恢复标志，为true表示出现了不正常掉电的情况 */
    aw_bool_t   lbn_need_recovery;

    /** \brief blk 注册标记 */
    aw_bool_t   blk_reg;

} aw_ftl_dev_t;


/**
 * \brief FTL对象池初始化
 *
 * \param[in] pool : 指向池内存
 * \param[in] size : 池大小
 *
 * \return 操作成功返回AW_OK, 否则返回aw_err_t的错误类型
 *
 * \note 该函数仅在aw_ftl_cfg.c中被aw_ftl_lib_init函数所调用
 */
aw_err_t aw_ftl_pool_init (struct aw_ftl_dev *pool, unsigned int size);


/** @}  grp_aw_if_ftl_core */

#ifdef __cplusplus
}
#endif

#endif /* __AW_FTL_CORE_H */



/* end of file */
