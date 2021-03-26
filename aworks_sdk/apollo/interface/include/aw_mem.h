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
 * \brief 内存管理标准接口
 *
 * 使用本服务需要包含以下头文件：
 * \code
 * #include "aw_mem.h"
 * \endcode
 * 本服务提供了动态内存管理的接口
 * \par 简单示例
 * \code
 *
 * \endcode
 *
 * // 更多内容待添加。。。
 *
 * \internal
 * \par modification history:
 * - 1.00 13-09-22  zen, first implementation
 * \endinternal
 */

#ifndef __AW_MEM_H
#define __AW_MEM_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus    */

#include "aw_psp_mem.h"

/**
 * \addtogroup grp_aw_if_mem
 * \copydoc aw_mem.h
 * @{
 */

/**
 * \brief 分配地址对齐的动态内存
 * \attention 中断不可调用
 *
 * \param[in] size    分配内存大小
 * \param[in] align   地址对齐大小
 *
 * \retval    NULL 失败/ !NULL 成功返回动态内存地址
 */
void *aw_mem_align(size_t size, size_t align);

/**
 * \brief     分配动态内存
 * \attention 中断不可调用
 *
 * \param[in] size    分配内存大小
 *
 * \retval    NULL 失败/ !NULL 成功返回动态内存地址
 */
void *aw_mem_alloc(size_t size);

/**
 * \brief       分配动态内存，大小为nelem * size， 并初始化为 0
 * \attention   中断不可调用
 *
 * \param[in]   nelem   数目
 * \param[in]   size    大小
 *
 * \retval    NULL 失败/ !NULL 成功返回动态内存地址
 */
void *aw_mem_calloc(size_t nelem, size_t size);

/**
 * \brief       释放分配的动态内存
 * \attention   中断不可调用
 *
 * \param[in]   ptr   动态分配地址
 */
void aw_mem_free(void *ptr);


/**
 * \brief 传入地址，获取可使用内存空间大小。
 *
 * \param[in]   ptr   动态分配地址
 *
 * \return 地址所对应的空间大小
 */
size_t aw_mem_size(void *ptr);

/**
 * \brief 重新分配动态内存
 *
 * \param[in]   ptr         已分配动态内存地址
 * \param[in]   newsize     大小
 *
 * \retval    NULL 失败/ !NULL 成功返回动态内存地址
 */
void *aw_mem_realloc(void *ptr, size_t newsize);



/**\brief 堆管理器类型*/
typedef aw_psp_heap_id_t aw_heap_id_t;

/**
 * \brief 从堆管理器中分配地址对齐的动态内存
 * \attention 中断不可调用
 *
 * \param[in] heap_id 堆管理器对象地址
 * \param[in] size    分配内存大小
 * \param[in] align   地址对齐大小
 *
 * \retval    NULL 失败/ !NULL 成功返回动态内存地址
 */
void *aw_memp_align(aw_heap_id_t heap_id,size_t size, size_t align);

/**
 * \brief 从堆管理器中分配动态内存
 * \attention 中断不可调用
 *
 * \param[in] heap_id 堆管理器对象地址
 * \param[in] size    分配内存大小
 *
 * \retval    NULL 失败/ !NULL 成功返回动态内存地址
 */
void *aw_memp_alloc(aw_heap_id_t heap_id,size_t size);

/**
 * \brief 从堆管理器中分配动态内存，大小为nelem * size， 并初始化为 0
 * \attention 中断不可调用
 *
 * \param[in]   heap_id 堆管理器对象地址
 * \param[in]   nelem   数目
 * \param[in]   size    大小
 *
 * \retval    NULL 失败/ !NULL 成功返回动态内存地址
 */
void *aw_memp_calloc(aw_heap_id_t heap_id,size_t nelem, size_t size);

/**
 * \brief      释放堆管理器分配的动态内存
 * \attention  中断不可调用
 *
 * \param[in]   heap_id 堆管理器对象地址
 * \param[in]   ptr     动态分配地址
 */
void aw_memp_free(aw_heap_id_t heap_id, void *ptr);

/**
 * \brief 从堆管理器中重新分配动态内存
 *
 * \param[in]   heap_id     堆管理器对象地址
 * \param[in]   ptr         已分配动态内存地址
 * \param[in]   newsize     大小
 *
 * \retval    NULL 失败/ !NULL 成功返回动态内存地址
 */
void *aw_memp_realloc(aw_heap_id_t heap_id, void *ptr, size_t newsize);

/** @}  grp_aw_if_mem */

#ifdef __cplusplus
}
#endif  /* __cplusplus     */

#endif  /* __AW_MEM_H */

/* end of file */
