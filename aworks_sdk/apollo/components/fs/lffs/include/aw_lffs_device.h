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
 * \brief lffs device structures definition
 *
 * \internal
 * \par modification history:
 * - 170923, vih, first implementation.
 * \endinternal
 */


#ifndef _AW_LFFS_DEVICE_H
#define _AW_LFFS_DEVICE_H

#ifdef __cplusplus
extern "C"{
#endif

/**
 * \addtogroup grp_aw_if_lffs_device
 * \copydoc aw_lffs_device.h
 * @{
 */

#include "aw_lffs_core.h"
#include "aw_lffs_tree.h"
#include "aw_lffs_mem.h"
#include "aw_lffs_os.h"

#if (AW_LFFS_CONFIG_ENABLE_INFO_TABLE == 1)
#include "aw_delayed_work.h"
#include "aw_sys_defer.h"
#endif

/**
 * \def AW_LFFS_MAX_DIRTY_BUF_GROUPS
 */
#define AW_LFFS_MAX_DIRTY_BUF_GROUPS    3

/**
 * \struct lffs_BlockInfoCacheSt
 * \brief block information structure, used to manager block information caches
 */
typedef struct aw_lffs_block_info_cache {
    
    /** \brief buffer list of block info(spares) */
    struct aw_list_head      list;              
    
    /** \brief internal memory pool, used for release whole buffer */
    void                    *mem_pool;            
} aw_lffs_block_info_cache_t;

/**
 * \struct lffs_PartitionSt
 * \brief partition basic information
 */
struct aw_lffs_partition {
    uint16_t start;        /**< \brief start block number of partition */
    uint16_t end;          /**< \brief end block number of partition */
};

/**
 * \struct lffs_LockSt
 * \brief lock stlffs
 */
struct aw_lffs_lock {
    aw_lffs_sem_t sem;
    int task_id;
    int counter;
};

/**
 * \struct lffs_DirtyGroupSt
 * \brief manager dirty page buffers
 */
typedef struct aw_lffs_dirty_group {
    int                  count;  /**< \brief dirty buffers count */
    
    /** \brief dirty group lock (0: unlocked, >0: locked) */
    int                  lock;   
    struct aw_list_head  dirty_list;
} aw_lffs_dirty_group_t;

/**
 * \struct lffs_PageBufDescSt
 * \brief lffs page buffers descriptor
 */
typedef struct aw_lffs_page_buf {
    struct aw_list_head     list;
    struct aw_list_head     clone_list;
    
    /** \brief dirty buffer groups */
    aw_lffs_dirty_group_t   dirtyGroup[AW_LFFS_MAX_DIRTY_BUF_GROUPS];    
    int            buf_max;          /**< \brief maximum buffers */
    int            dirty_buf_max;    /**< \brief maximum dirty buffer allowed */
    void          *pool;             /**< \brief memory pool for buffers */
} aw_lffs_page_buf_t;


/**
 * \struct lffs_PageCommInfoSt
 * \brief common data for device, should be initialised at early
 * \note it is possible that pg_size is smaller than physical page size, 
 *  but normally they are the same.
 * \note page data layout: [HEADER] + [DATA]
 */
typedef struct aw_lffs_page_info {
    uint16_t pg_data_size;           /**< \brief page data size */
    uint16_t header_size;            /**< \brief header size */
    uint16_t pg_size;                /**< \brief page size */
} aw_lffs_page_info_t;

/**
 * \struct lffs_FlashStatSt
 * \typedef lffs_FlashStat
 * \brief statistic data of flash read/write/erase activities
 */
typedef struct aw_lffs_flash_stat {
    int block_erase_count;
    int page_write_count;
    int page_read_count;
    int page_header_read_count;
    int spare_write_count;
    int spare_read_count;
    unsigned long io_read;
    unsigned long io_write;
} aw_lffs_flash_stat_t;


/**
 * \struct lffs_ConfigSt
 * \typedef lffs_Config
 * \brief lffs config parameters
 */
typedef struct aw_lffs_config {
    int bc_caches;
    int page_buffers;
    int dirty_pages;
    int dirty_groups;
    int reserved_free_blocks;
} aw_lffs_config_t;


/** pending block mark definitions */

/** \brief require block recovery and mark bad block */
#define AW_LFFS_PENDING_BLK_RECOVER    0        

/** \brief require refresh the block (erase and re-use it) */
#define AW_LFFS_PENDING_BLK_REFRESH    1        

/*
 * \brief require block cleanup (due to interrupted write, erase and re-use it) 
 */
#define AW_LFFS_PENDING_BLK_CLEANUP    2        

/**
 * \struct lffs_PendingBlockSt
 * \typedef lffs_PendingBlock
 * \brief Pending block descriptor
 */
typedef struct aw_lffs_pending_block {
    uint16_t block;           /**< \brief pending block number */
    uint8_t  mark;            /**< \brief pending block mark */
} aw_lffs_pending_block_t;

/**
 * \struct lffs_PendingListSt
 * \brief Pending block list
 */
typedef struct aw_lffs_pending_list {
    
    /** \brief pending block counter */
    int                     count;                
    
    /** \brief pending block list */
    aw_lffs_pending_block_t list[AW_LFFS_CONFIG_MAX_PENDING_BLOCKS];    
    
    /** \brief pending block being recovered */
    uint16_t                block_in_recovery;         
} aw_lffs_pending_list_t;


#if (AW_LFFS_CONFIG_ENABLE_INFO_TABLE == 1)
/**
 * \brief 文件信息表
 */
typedef struct aw_lffs_info_table {
    struct aw_delayed_work  delay_work;
    struct aw_defer_djob    defer_job;
    
    /** \brief 当前使用的节点  */
    struct aw_lffs_tree_node *p_node;   
    
    /** \brief 当前物理块的扇区使用记数  */
    uint16_t sector_use_cnt;        
    
    /** \brief   */
    uint16_t sector_valid_begin;    
    
    /** \brief   */
    uint16_t sector_valid_cnt;      
    
    /** \brief 表中的数据是否有效  */
    aw_bool_t   is_valid;

    /** \brief  是否使能该功能，当一个块无法存下一次 infotbl 时，文件系统会关闭该功能  */
    aw_bool_t   is_enable;

    aw_err_t err;
} aw_lffs_info_table_t;
#endif

/**
 * \brief The core data structure of LFFS, all information needed by manipulate 
 *        LFFS object
 * \note one partition corresponding one lffs device.
 */
struct aw_lffs_device {
    
    /** \brief low level initialisation */
    aw_err_t (*Init)   (struct aw_lffs_device *dev);            
    
    /** \brief low level release */
    aw_err_t (*Release)(struct aw_lffs_device *dev);            
    
    
    void      *_private;                  /**< \brief private data for device */

    struct aw_lffs_storage_attr      *attr;     /**< \brief storage attribute */
    
    /** \brief partition information */
    struct aw_lffs_partition          par;      
    struct aw_lffs_flash_ops         *ops;      /**< \brief flash operations */
    struct aw_lffs_block_info_cache   bc;       /**< \brief block info cache */
    
    /** \brief lock data structure */
    struct aw_lffs_lock               lock;     
    struct aw_lffs_page_buf           buf;      /**< \brief page buffers */
    
    /** \brief common information */
    struct aw_lffs_page_info          page_info;
    
    /** \brief tree list of block */
    struct aw_lffs_tree               tree;     
    
    /** \brief pending block list, to be recover/mark 'bad'/refresh */
    struct aw_lffs_pending_list       pending;  
    
    /** \brief statistic (counters) */
    struct aw_lffs_flash_stat         st;       
    
    /** \brief lffs memory allocator */
    struct aw_lffs_mem_allocator      mem;      
    struct aw_lffs_config             cfg;      /**< \brief lffs config */
    struct aw_lffs_info_table         info_tbl; /**< \brief lffs info table */
    
    
    uint32_t    ref_count;       /**< \brief device reference count */
    int            dev_num;      /**< \brief device number (partition number) */
    uint32_t    malloc_cnt;
};


/** create the lock for lffs device */
void aw_lffs_devicelock_init(aw_lffs_device_t *dev);

/** delete the lock of lffs device */
void aw_lffs_devicelock_release(aw_lffs_device_t *dev);

/** lock lffs device */
void aw_lffs_devicelock_lock(aw_lffs_device_t *dev);

/** unlock lffs device */
void aw_lffs_devicelock_unlock(aw_lffs_device_t *dev);


/** @}  grp_aw_if_lffs_device */

#ifdef __cplusplus
}
#endif


#endif

/* end of file */
