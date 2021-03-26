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
 * \brief basic configuration of lffs
 *
 * \internal
 * \par modification history:
 * - 2017-9-18   vih, first implementation.
 * \endinternal
 */

#ifndef __AW_LFFS_CONFIG_H
#define __AW_LFFS_CONFIG_H

#include "apollo.h"
#include "aw_errno.h"
#include "aw_list.h"
#include "io/aw_fcntl.h"
#include "io/sys/aw_stat.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief maximum page size AW_LFFS support
 */
#define AW_LFFS_MAX_PAGE_SIZE      2048

/**
 * \brief AW_LFFS_MAX_SPARE_SIZE
 */
#define AW_LFFS_MAX_SPARE_SIZE     ((AW_LFFS_MAX_PAGE_SIZE / 256) * 8)

/**
 * \brief AW_LFFS_MAX_ECC_SIZE
 */
#define AW_LFFS_MAX_ECC_SIZE       ((AW_LFFS_MAX_PAGE_SIZE / 256) * 5)

/**
 * \brief AW_LFFS_MAX_CACHED_BLOCK_INFO
 * \note lffs cache the block info for opened directories and files,
 *       a practical value is 5 ~ AW_LFFS_MAX_OBJECT_HANDLE
 */
#define AW_LFFS_MAX_CACHED_BLOCK_INFO   50

/**
 * \brief AW_LFFS_MAX_PAGE_BUFFERS
 * \note the bigger value will bring better read/write performance.
 *       but few writing performance will be improved when this
 *       value is become larger than 'max pages per block'
 */
#define AW_LFFS_MAX_PAGE_BUFFERS        64


/**
 * \brief AW_LFFS_CLONE_BUFFER_THRESHOLD
 * \note reserve buffers for clone.
 *       1 or 2 (when AW_LFFS_CONFIG_PAGE_WRITE_VERIFY is enabled).
 */
#define AW_LFFS_CLONE_BUFFERS_THRESHOLD 2

/**
 * \brief AW_LFFS_MAX_SPARE_BUFFERS
 * \note spare buffers are used for lower level flash operations,
 *       5 should be enough.
 */
#define AW_LFFS_MAX_SPARE_BUFFERS       5


/**
 * \brief AW_LFFS_CONFIG_MAX_PENDING_BLOCKS
 * \note When a new bad block or ECC error is discovered during reading flash,
 *       the block will be put in a 'pending' list and will be processed later.
 *       This config the maximum pending blocks before being processed, 4 should
 *       be enough.
 */
#define AW_LFFS_CONFIG_MAX_PENDING_BLOCKS   4

/**
 * \brief 使能保存文件信息到表中，可以加快mount的速度，每个块需要保存12字节的数据，
 *        因此设置保存时间  AW_LFFS_CONFIG_INFO_TABLE_DELAY_MS 应尽可能大
 */
#define AW_LFFS_CONFIG_ENABLE_INFO_TABLE    1

#if (AW_LFFS_CONFIG_ENABLE_INFO_TABLE == 1)
#define AW_LFFS_CONFIG_INFO_TABLE_DELAY_MS  60000
#endif /* AW_LFFS_CONFIG_ENABLE_INFO_TABLE */

/**
 * \brief AW_LFFS_MAX_DIRTY_PAGES_IN_A_BLOCK
 * \note this value should be between '2' and the lesser of
 *       'max pages per block' and
 *       (AW_LFFS_MAX_PAGE_BUFFERS - AW_LFFS_CLONE_BUFFERS_THRESHOLD - 1).
 *
 *       the smaller the value the frequently the buffer will be flushed.
 */
#define AW_LFFS_MAX_DIRTY_PAGES_IN_A_BLOCK  60

/**
 * \brief AW_LFFS_CONFIG_ENABLE_LFFS_DEBUG_MSG
 * \note Enable debug message output. You must call lffs_InitDebugMessageOutput()
 *      to initialize debug apart from enable debug feature.
 */
#define AW_LFFS_CONFIG_ENABLE_LFFS_DEBUG_MSG

/**
 * \brief AW_LFFS_CONFIG_USE_GLOBAL_FS_LOCK
 * \note use global lock instead of per-device lock.
 *       this is required if you use fd APIs in multi-thread environment.
 */
#define AW_LFFS_CONFIG_USE_GLOBAL_FS_LOCK


/**
 * \brief AW_LFFS_CONFIG_USE_PER_DEVICE_LOCK
 * \note use per-device lock.
 *       this is required if you use fs APIs in multi-thread environment.
 */
//#define AW_LFFS_CONFIG_USE_PER_DEVICE_LOCK

#ifdef AW_LFFS_CONFIG_USE_PER_DEVICE_LOCK
#define AW_LFFS_CONFIG_PER_DEVICE_LOCK
#endif


/**
 * \brief AW_LFFS_CONFIG_USE_STATIC_MEMORY_ALLOCATOR
 * \note lffs will use static memory allocator if this is defined.
 *       to use static memory allocator, you need to provide memory
 *       buffer when creating lffs_Device.
 *
 *       use AW_LFFS_STATIC_BUFF_SIZE() to calculate memory buffer size.
 */
#define AW_LFFS_CONFIG_USE_STATIC_MEMORY_ALLOCATOR 0

/**
 * \brief AW_LFFS_CONFIG_USE_SYSTEM_MEMORY_ALLOCATOR
 * \note  using system platform's 'malloc' and 'free'.
 */
#define AW_LFFS_CONFIG_USE_SYSTEM_MEMORY_ALLOCATOR 1



/**
 * \brief AW_LFFS_CONFIG_FLUSH_BUF_AFTER_WRITE
 * \note AW_LFFS will write all data directly into flash in
 *       each 'write' call if you enable this option.
 *       (which means lesser data lost when power failure but
 *       poorer writing performance).
 *       It's not recommended to enable this for normal applications.
 */
//#define AW_LFFS_CONFIG_FLUSH_BUF_AFTER_WRITE


/**
 * \brief AW_LFFS_CONFIG_LFFS_AUTO_LAYOUT_MTD_COMP
 * \note Use Linux MTD compatiable spare placement for AW_LFFS_LAYOUT_AUTO,
 *       only valid for page data size 512 or 2048.
 */
//#define AW_LFFS_CONFIG_LFFS_AUTO_LAYOUT_USE_MTD_SCHEME


/**
 * \brief AW_LFFS_MAX_OBJECT_HANDLE
 * maximum number of object handle
 */
#define AW_LFFS_MAX_OBJECT_HANDLE   50
#define AW_LFFS_FD_SIGNATURE_SHIFT  6


/**
 * \brief AW_LFFS_MAX_DIR_HANDLE
 * maximum number of lffs_DIR
 */
#define AW_LFFS_MAX_DIR_HANDLE      10

/**
 * \brief AW_LFFS_MINIMUN_ERASED_BLOCK
 *  AW_LFFS will not allow appending or creating new files when the free/erased
 *  block is lower then AW_LFFS_MINIMUN_ERASED_BLOCK.
 */
#define AW_LFFS_MINIMUN_ERASED_BLOCK 2

/**
 * \brief AW_LFFS_CONFIG_CHANGE_MODIFY_TIME
 * \note If defined, closing a file which is opened for writing/appending will
 *       update the file's modify time as well.
 *       It's not recommended to enable this for most application.
 */
//#define AW_LFFS_CONFIG_CHANGE_MODIFY_TIME


/**
 * \brief AW_LFFS_CONFIG_ENABLE_BAD_BLOCK_VERIFY
 * \note Allow erase and verify block marked as 'bad' when format AW_LFFS partition.
 *      It's not recommended for most NAND flash.
 */
//#define AW_LFFS_CONFIG_ENABLE_BAD_BLOCK_VERIFY

/**
 * \brief AW_LFFS_CONFIG_ERASE_BLOCK_BEFORE_MARK_BAD
 * \note Erase block before mark bad block.
 *       This is required if a dedicated 'MarkBadBlock' not provided by flash
 *       driver.
 */
#define AW_LFFS_CONFIG_ERASE_BLOCK_BEFORE_MARK_BAD

/**
 * \brief AW_LFFS_CONFIG_PAGE_WRITE_VERIFY
 * \note verify page data after write, for extra safe data storage.
 *      It's recommented to enable if you are using MLC NAND or low quality NAND
 *      flash chip.
 */
#define AW_LFFS_CONFIG_PAGE_WRITE_VERIFY

/**
 * \brief AW_LFFS_CONFIG_BAD_BLOCK_POLICY_STRICT
 * \note If this config is enabled, AW_LFFS will report the block as 'bad' if any
 *       bit-flips found;
 *       otherwise, AW_LFFS report bad block only when ECC failed or reported
 *       by low level flash driver.
 *
 * \note Enable this will ensure your data always be stored on completely good
 *       blocks. Probably should not enable this for most NAND flash.
 */
//#define AW_LFFS_CONFIG_BAD_BLOCK_POLICY_STRICT


/**
 * \brief AW_LFFS_CONFIG_LFFS_REFRESH_BLOCK
 * \note If this config is enabled, when bit flip(s) detected and corrected by
 *       ECC, AW_LFFS will copy block data
 *       to a new flash block and erase the old block (not mark it as 'bad').
 *
 * \note AW_LFFS_CONFIG_BAD_BLOCK_POLICY_STRICT should be disabled if this config 
 *       is choosen.
 */
#define AW_LFFS_CONFIG_LFFS_REFRESH_BLOCK


/**
 * \brief AW_LFFS_CONFIG_ENABLE_PAGE_DATA_CRC
 * \note If this is enabled, AW_LFFS save page data CRC16 sum in mini header,
 *       it provides extra protection for data integrity.
 *       Enable this if your CPU has enough processing power.
 */
//#define AW_LFFS_CONFIG_ENABLE_PAGE_DATA_CRC


#define AW_LFFS_CONFIG_NAME_HASH_TABLE     1


/** micros for calculating buffer sizes */

/**
 *  \def AW_LFFS_BLOCK_INFO_BUFFER_SIZE
 *  \brief calculate memory bytes for block info caches
 */
#define AW_LFFS_BLOCK_INFO_BUFFER_SIZE(n_pages_per_block)  \
            (                                           \
                (                                       \
                    sizeof(lffs_BlockInfo) +            \
                    sizeof(lffs_PageSpare) * n_pages_per_block \
                 ) * AW_LFFS_MAX_CACHED_BLOCK_INFO              \
            )

/**
 *  \def AW_LFFS_PAGE_BUFFER_SIZE
 *  \brief calculate memory bytes for page buffers
 */
#define AW_LFFS_PAGE_BUFFER_SIZE(n_page_size)  \
            ((sizeof(lffs_Buf) + n_page_size) * AW_LFFS_MAX_PAGE_BUFFERS)

/**
 *  \def AW_LFFS_TREE_BUFFER_SIZE
 *  \brief calculate memory bytes for tree nodes
 */
#define AW_LFFS_TREE_BUFFER_SIZE(n_blocks)     (sizeof(TreeNode) * n_blocks)


#define AW_LFFS_SPARE_BUFFER_SIZE      \
    (AW_LFFS_MAX_SPARE_BUFFERS * AW_LFFS_MAX_SPARE_SIZE)


/**
 *  \def AW_LFFS_STATIC_BUFF_SIZE
 *  \brief calculate total memory usage of lffs system
 */
#define AW_LFFS_STATIC_BUFF_SIZE(n_pages_per_block, n_page_size, n_blocks) \
            (       \
                AW_LFFS_BLOCK_INFO_BUFFER_SIZE(n_pages_per_block) + \
                AW_LFFS_PAGE_BUFFER_SIZE(n_page_size) + \
                AW_LFFS_TREE_BUFFER_SIZE(n_blocks) + \
                AW_LFFS_SPARE_BUFFER_SIZE \
             )

#define AW_LFFS_BUF_CHECK_SUM  0




/** \brief config check */
#if (AW_LFFS_MAX_PAGE_BUFFERS - AW_LFFS_CLONE_BUFFERS_THRESHOLD) < 3
#error "AW_LFFS_MAX_PAGE_BUFFERS is too small"
#endif

#if (AW_LFFS_MAX_DIRTY_PAGES_IN_A_BLOCK < 2)
#error "AW_LFFS_MAX_DIRTY_PAGES_IN_A_BLOCK should >= 2"
#endif

#if (AW_LFFS_MAX_PAGE_BUFFERS - AW_LFFS_CLONE_BUFFERS_THRESHOLD - 1 < \
         AW_LFFS_MAX_DIRTY_PAGES_IN_A_BLOCK)
#error "AW_LFFS_MAX_DIRTY_PAGES_IN_A_BLOCK should < (AW_LFFS_MAX_PAGE_BUFFERS - AW_LFFS_CLONE_BUFFERS_THRESHOLD)"
#endif

#if defined(AW_LFFS_CONFIG_PAGE_WRITE_VERIFY) && \
    (AW_LFFS_CLONE_BUFFERS_THRESHOLD < 2)
#error "AW_LFFS_CLONE_BUFFERS_THRESHOLD should >= 2 when AW_LFFS_CONFIG_PAGE_WRITE_VERIFY is enabled."
#endif

#if AW_LFFS_CONFIG_USE_STATIC_MEMORY_ALLOCATOR + \
    AW_LFFS_CONFIG_USE_SYSTEM_MEMORY_ALLOCATOR > 1
#error "Please enable ONLY one memory allocator"
#endif

#if AW_LFFS_CONFIG_USE_STATIC_MEMORY_ALLOCATOR + \
    AW_LFFS_CONFIG_USE_SYSTEM_MEMORY_ALLOCATOR == 0
#error "Please enable ONE of memory allocators"
#endif

#if defined(AW_LFFS_CONFIG_USE_GLOBAL_FS_LOCK) && \
    defined(AW_LFFS_CONFIG_USE_PER_DEVICE_LOCK)
#error "enable either AW_LFFS_CONFIG_USE_GLOBAL_FS_LOCK or AW_LFFS_CONFIG_USE_PER_DEVICE_LOCK, not both"
#endif

#if (AW_LFFS_MAX_OBJECT_HANDLE > (1 << AW_LFFS_FD_SIGNATURE_SHIFT))
#error "Please increase AW_LFFS_FD_SIGNATURE_SHIFT !"
#endif

#if AW_LFFS_CONFIG_MAX_PENDING_BLOCKS < 2
#error "Please increase AW_LFFS_CONFIG_MAX_PENDING_BLOCKS, normally 4"
#endif

#if defined(AW_LFFS_CONFIG_BAD_BLOCK_POLICY_STRICT) && \
    defined(AW_LFFS_CONFIG_LFFS_REFRESH_BLOCK)
#error "AW_LFFS_CONFIG_LFFS_REFRESH_BLOCK conflict with AW_LFFS_CONFIG_BAD_BLOCK_POLICY_STRICT !"
#endif


#ifdef __cplusplus
}
#endif

#endif /* __AW_LFFS_CONFIG_H */

/* end of file */




