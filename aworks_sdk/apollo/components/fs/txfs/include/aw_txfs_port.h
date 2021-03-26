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
 * \brief TXFS AWorks adapter interfaces
 *
 * \internal
 * \par modification history:
 * - 1.00 17-07-04  mkr, first implementation
 * \endinternal
 */
#ifndef __AW_TXFS_ADAPTER_H
#define __AW_TXFS_ADAPTER_H

#include "aw_txfs_config.h"
#include "aw_txfs_types.h"
#include "aw_txfs_lib_private.h"

#include "aw_vdebug.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_txfs_adapter
 * @{
 */

/** \brief debug print */
#if (AW_TXFS_DEBUG_ENABLE == 1)
#define print_info(...)  AW_INFOF((__VA_ARGS__))
#define printErr(...)    print_info((__VA_ARGS__))
#else
#define print_info(...)  AW_INFOF((__VA_ARGS__))
#define printErr(...)
#endif

/** \brief 块设备命令可选参数 */
typedef enum aw_txfs_blkdev_cmd {
    TXFS_BLKDEV_DISKSIZE = 0, /**< \brief 块设备大小，返回值p_arg大小为 uint64_t */
    TXFS_BLKDEV_BLKSIZE  = 1, /**< \brief 块设备中块的大小, p_arg为 uint32_t */
    TXFS_BLKDEV_BLKS     = 2, /**< \brief 块设备中块的个数, p_arg为 uint32_t */
    TXFS_BLKDEV_ID       = 3, /**< \brief 块设备ID */
    TXFS_BLKDEV_STATUS   = 4, /**< \brief 块设备状态, p_arg为NULL */
} aw_txfs_blkdev_cmd_t;


/******************************************************************************
 * external functions
 *
 * \note When porting to other platform, please achieve the following functions
 *****************************************************************************/

/**
 * \brief Allocate a memory block
 *
 * \param[in] msize  memory size
 *
 * \return the pointer to memory or NULL
 */
void *aw_txfs_mem_alloc(size_t msize);

/**
 * \brief Free a memory block
 *
 * \param[in] mblock  Pointer to the memory block to free
 *
 * \return N/A
 */
void aw_txfs_mem_free(void *mblock);

/**
 * \brief txfs block io memory allocate
 *
 * \param[in] p_dev  block device
 * \param[in] num    number blocks
 */
void *aw_txfs_bio_alloc (void *p_dev, size_t num);

#if (AW_TXFS_DEBUG_MEMALLOC_USAGE == 1)
/**
 * \brief dynamic memory usage in TXFS
 *
 * \return the dynamic memory allocate time.
 *         when TXFS was unmounted, it should be 0.
 */
int32_t aw_txfs_memalloc_get (void);
#endif

/**
 * \brief return number of milliseconds since midnight Jan 1, 1970
 *
 * \note This routine returns the number of milliseconds since midnight,
 *       January 1, 1970.
 *
 * \return  number of milliseconds since midnight, January 1, 1970
 */
//extern aw_txfs_txfstime_t aw_txfs_time_get(void){return 0;};

/**
 * \brief getting id of block device
 */
uint8_t aw_txfs_blkid_get (void *p_dev);

/**
 * \brief 获取块设备信息
 * \param[in] p_dev   指向块设备对象的指针
 * \param[in] cmd     命令参数 \see aw_txfs_blkdev_cmd_t
 * \param[out] p_arg  返回命令参数相关的信息
 *
 * \return OK 执行成功；ERROR 执行出错
 */
aw_err_t aw_txfs_blkdev_ioctl (void *p_dev, int cmd, void *p_arg);

/**
 * \brief 写数据到块设备
 *
 * \param[in] p_dev  指向存储设备的指针
 * \param[in] p_dat  待写入的数据缓存区地址
 * \param[in] offset 偏移值，即写入的起始地址
 * \param[in] size   数据个数
 *
 * \return AW_OK or AW_ERROR
 */
aw_err_t aw_txfs_disk_write (void     *p_dev,  char *p_dat,
                             uint64_t  offset, int   size);

/**
 * \brief 从块设备读数据
 *
 * \param[in] p_dev  指向存储设备的指针
 * \param[in] p_dat  待写入的数据缓存区地址
 * \param[in] offset 偏移值，即写入的起始地址
 * \param[in] size   数据个数
 *
 * \return AW_OK or AW_ERROR
 */
aw_err_t aw_txfs_disk_read (void     *p_dev,  char *p_dat,
                            uint64_t  offset, int   size);

/**
 * \brief 系统节拍频率
 *
 * \return 系统节拍频率
 */
uint32_t aw_txfs_sys_clkrate_get(void);

/**
 * \brief 初始化原子变量
 *
 * \param[in] atomic  原子变量
 *
 * \return 无
 */
void aw_txfs_atomic_init (atomic_t *atomic);

/**
 * \brief 原子操作-递增
 *
 * \return 返回递增前的值
 */
int aw_txfs_atomic_inc(atomic_t *atomic);

/**
 * \brief 原子操作-递减
 *
 * \return 返回递减前的值
 */
int aw_txfs_atomic_dec(atomic_t *atomic);

/**
 * \brief 原子操作-获取值
 *
 * \return 数值
 */
int aw_txfs_atomic_get(atomic_t *atomic);


#ifdef __cplusplus
}
#endif


#endif /* __AW_TXFS_ADAPTER_H */

/* end of file */

