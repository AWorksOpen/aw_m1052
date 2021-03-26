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
 * \brief 内存池
 *
 * 使用本服务需要包含头文件 aw_pool.h
 *
 * 内存池提供了一种快速有效的内存管理方式。使用内存池减少了从堆中分配内存的次数，
 * 从而降低了频繁分配、释放内存所导致的内存碎片。
 *
 * 内存池中的所有对象都具有相同的大小。
 *
 * \par 初始化内存池
 * \code
 *
 * static aw_pool_t    my_pool;    // 内存池对象
 * static aw_pool_id_t my_pool_id; // 内存池句柄
 * static int          bulk[100];  // 内存块
 *
 * static void pool_init(void)
 * {
 *     my_pool_id = aw_pool_init(&my_pool,
 *                                bulk,
 *                         sizeof(bulk),
 *                         sizeof(bulk[0]));
 * }
 * \endcode
 *
 * \par 申请和返回内存池单元
 * \code
 *
 * static void pool_test(void)
 * {
 *     int *item = aw_pool_item_get(my_pool_id);
 *
 *     aw_pool_item_return(my_pool_id, item);
 * }
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.01 14-11-05  orz, add aw_pool_item_size() function.
 * - 1.00 13-09-04  zen, first implementation.
 * \endinternal
 */

#ifndef __AW_POOL_H
#define __AW_POOL_H

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus	*/

#include "aw_compiler.h"

/**
 * \addtogroup grp_aw_if_pool
 * \copydoc aw_pool.h
 * @{
 */

/** \brief 内存池的定义 */
struct aw_pool {

    /** \brief 空闲块链表入口 */
    void *p_free;

    /** \brief 内存池的原始入口 */
    void *p_start;

    /** \brief 内存池管理的最后一块内存块 */
    void *p_end;

    /** \brief 内存池单元的最大值 */
    size_t item_size;

    /** \brief 内存池单元的总数 */
    size_t item_count;

    /** \brief 剩余空闲块个数 */
    size_t nfree;

    /**
     * \brief 空闲块的最低余量。
     *
     * 这是一个统计量，用来返回内存池生命周期内剩余量的最低值。
     *
     * \sa aw_pool_margin_get()
     */
    size_t nmin;

};

/** \brief 内存池 */
typedef struct aw_pool aw_pool_t;

/** \brief 内存池句柄 */
typedef aw_pool_t *aw_pool_id_t;

/**
 * \brief 内存池实例初始化
 *
 * \param[in] p_pool       内存池实例指针
 * \param[in] p_pool_mem   内存池单元使用空间的起始地址
 * \param[in] pool_size    内存池单元空间总大小
 * \param[in] item_size    单个内存池单元空间大小
 *
 * \return 若成功，返回内存池句柄；当失败时，返回NULL
 */
aw_pool_id_t aw_pool_init(aw_pool_t *p_pool,
                          void      *p_pool_mem,
                          size_t     pool_size,
                          size_t     item_size);

/**
 * \brief 从内存池中获取一个单元
 *
 * \param[in] pool_id      内存池句柄 
 *
 * \return 若成功，返回一个内存池单元指针；当失败时，返回NULL
 */
void *aw_pool_item_get (aw_pool_id_t pool_id);

/**
 * \brief 释放一个内存池单元
 *
 * \param[in] pool_id  内存池句柄
 * \param[in] p_item   内存池单元指针
 *
 * \retval AW_OK       释放成功
 * \retval -AW_EFAULT  释放失败
 */
aw_err_t aw_pool_item_return (aw_pool_id_t pool_id, void *p_item);


/**
 * \brief 获取当前内存池，分配余量的下界
 *
 * 用来查看内存池的使用情况，分配的峰值。
 *
 * \param[in] pool_id   内存池句柄 
 *
 * \return 返回内存池结构体的 \e nmin 成员
 */
size_t aw_pool_margin_get (aw_pool_id_t pool_id);

/**
 * \brief 获取内存池单元空间大小
 *
 * \param[in] pool_id  内存池句柄
 *
 * \return 返回内存池单元空间大小
 */
aw_static_inline size_t aw_pool_item_size (aw_pool_id_t pool_id)
{
    return pool_id->item_size;
}


/** @}  grp_aw_if_pool_lib */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AW_POOL_H */

/* end of file */
