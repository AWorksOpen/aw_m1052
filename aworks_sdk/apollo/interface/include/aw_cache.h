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
 * \brief cache 管理标准接口
 *
 * 使用本服务需要包含头文件 aw_cache.h
 * 
 * 本服务提供了管理 cache 的接口
 * \par 简单示例
 * \code
 * \endcode
 *
 * // 更多内容待添加。。。
 *
 * \internal
 * \par modification history:
 * - 1.00 13-09-04  zen, first implementation
 * \endinternal
 */

#ifndef __AW_CACHE_H
#define __AW_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus	*/

/**
 * \addtogroup grp_aw_if_cache
 * \copydoc aw_cache.h
 * @{
 */

#include "aw_psp_cache.h"

/** \brief 缓存线大小 */
#define AW_CACHE_LINE_SIZE  AW_PSP_CACHE_LINE_SIZE

/**
 * \brief 将所有可以被回写缓存中指定范围的数据回写到内存
 *
 * \param[in] address 起始地址,若该地址不是缓存线的起始地址，则将使用该地址所在
 *                    缓存线的起始地址
 * \param[in] nbytes  长度, 将会被 round up 到缓存线大小
 *
 * \note 通过 AW_CACHE_LINE_SIZE 得到缓存线(cache line)大小
 */
void aw_cache_flush (void *address, size_t nbytes);

/**
 * \brief 在所有缓存中将指定范围的数据标记为无效, 重新从内存中读取
 *
 * \param[in] address   起始地址,若该地址不是缓存线的起始地址，则将使用该地址所在
 *                      缓存线的起始地址
 * \param[in] nbytes    长度, 将会被 round up 到缓存线大小
 *
 * \note 通过 AW_CACHE_LINE_SIZE 得到缓存线(cache line)大小
 */
void aw_cache_invalidate (void *address, size_t nbytes);

/**
 * \brief 分配 cache-safe 的缓冲区用于 DMA 设备或驱动
 *
 * \param[in] nbytes    长度, 将会被 round up 到缓存线大小
 *
 * \return 缓存区起始地址 或 NULL
 *
 * \sa aw_cache_dma_free()
 */
void *aw_cache_dma_malloc (size_t nbytes);

/**
 * \brief 分配 cache-safe 的缓冲区用于 DMA 设备或驱动
 *
 * \param[in] nbytes    长度, 将会被 round up 到缓存线大小
 * \param[in] align     对齐长度
 *
 * \return 缓存区起始地址 或 NULL
 *
 * \sa aw_cache_dma_free()
 */
void *aw_cache_dma_align (size_t nbytes, size_t align);


/**
 * \brief 释放 cache-safe 的缓冲区
 *
 * \param[in] ptr 缓存区起始地址(调用 aw_cache_dma_malloc()获取的)
 *
 * \sa aw_cache_dma_malloc()
 */
void aw_cache_dma_free (void *ptr);

/** @}  grp_aw_if_cache */

#ifdef __cplusplus
}
#endif	/* __cplusplus 	*/

#endif	/* __AW_CACHE_H */

/* end of file */
