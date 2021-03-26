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
 * \brief
 *
 * \internal
 * \par modification history:
 * - 170923, vih, first implementation.
 * \endinternal
 */

#ifndef _AW_LFFS_HASH_H_
#define _AW_LFFS_HASH_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_lffs_config.h"

/**
 * \brief 遍历hash表，获取hash节点
 *
 * \param[in]       p_node          存放获取到的节点
 * \param[in]       p_tab           hash表
 */
#define aw_lffs_hash_for_each(p_node, p_tab) \
    for (p_node = NULL; \
         (p_node = aw_lffs_hash_next_find(p_tab, p_node)) != NULL;)


#define aw_lffs_hash_entry(ptr, type, member)  container_of(ptr, type, member)

struct aw_lffs_hash_node {
    struct aw_list_head node;
    const void         *p_key;
};

struct aw_lffs_hash_table {
    struct aw_list_head      *array;
    int                       array_num;
    int                       data_len;
    int                       key_len;
    int                     (*pfn_hash) (const void *p_key);
};

int aw_lffs_hash_table_init (struct aw_lffs_hash_table *p_hash,
                             struct aw_list_head       *array,
                             int                        array_num,
                             int                      (*pfn_hash) (const void *p_key),
                             int                        key_len);

int aw_lffs_hash_table_uninit (struct aw_lffs_hash_table *p_hash);

struct aw_lffs_hash_node * aw_lffs_hash_find (struct aw_lffs_hash_table *p_hash,
                                              const void                *p_key);

/**
 * \brief hash结点添加，不允许添加相同关键字的结点
 *
 * \param[in]       p_hash      hash表地址
 * \param[in]       p_key       关键字指针
 * \param[in]       p_node      当前结点，不能为NULL
 *
 * \retval  AW_OK    : 添加成功
 *          -EINVAL  : 参数无效
 *          -EEXIST  : 该关键字的结点已经存在
 *          -ENOTSUP : 该关键字无效
 */
int aw_lffs_hash_add (struct aw_lffs_hash_table  *p_hash,
                      const void                 *p_key,
                      struct aw_lffs_hash_node   *p_node);

/**
 * \brief 删除hash结点，该关键字应该是唯一的
 *
 * \param[in]       p_hash      hash表地址
 * \param[in]       p_key       关键字指针
 *
 * \retval  AW_OK     : 添加成功
 *          -EINVAL   : 参数无效
 *          -ENODATA  : 找不到结点
 */
int aw_lffs_hash_del (struct aw_lffs_hash_table  *p_hash,
                      const void                 *p_key);

/**
 * \brief hash结点添加，允许添加相同关键字的结点，但结点地址不能（不能同一个结点）
 *
 * \param[in]       p_hash      hash表地址
 * \param[in]       p_key       关键字指针
 * \param[in]       p_node      当前结点，不能为NULL
 *
 * \retval  AW_OK    : 添加成功
 *          -EINVAL  : 参数无效
 *          -ENOTSUP : 该关键字无效
 *
 * \note  用该接口添加的结点，查找时应该用 aw_lffs_same_hash_next_find() 接口
 */
int aw_lffs_hash_add_ex (struct aw_lffs_hash_table  *p_hash,
                         const void                 *p_key,
                         struct aw_lffs_hash_node   *p_node);

/**
 * \brief 删除hash结点
 *
 * \param[in]       p_hash      hash表地址
 * \param[in]       p_node      hash结点，不能为NULL
 *
 * \retval  AW_OK     : 添加成功
 *          -EINVAL   : 参数无效
 */
int aw_lffs_hash_del_ex (struct aw_lffs_hash_table  *p_hash,
                         struct aw_lffs_hash_node   *p_node);

/**
 * \brief 获取p_node节点的下一个结点，p_node为空则获取第一个节点
 *
 * \param[in]       p_hash      hash表地址
 * \param[in]       p_node      当前结点，可为NULL
 * \param[in]       p_key       关键字指针
 *
 * \retval  NULL: 查找为空
 *          ELSE: 查找到hash结点指针
 */
struct aw_lffs_hash_node *aw_lffs_hash_next_find (
    struct aw_lffs_hash_table  *p_hash,
    struct aw_lffs_hash_node   *p_node);

/**
 * \brief 获取和p_node的关键字相同的下一个结点，p_node为空则获取该关键字的第一个
 *        节点
 *
 * \param[in]       p_hash      hash表地址
 * \param[in]       p_node      当前结点，可为NULL
 * \param[in]       p_key       关键字指针
 *
 * \retval  NULL: 查找为空
 *          ELSE: 查找到hash结点指针
 */
struct aw_lffs_hash_node *aw_lffs_same_hash_next_find (
    struct aw_lffs_hash_table  *p_hash,
    struct aw_lffs_hash_node   *p_node,
    const void                 *p_key);

/**
 * \brief 获取指定的hash链表的元素数量
 *
 * \param[in]       p_hash      hash表地址
 * \param[in]       list_index  hash链表的id
 *
 * \retval  该链表中的元素数量
 */
int aw_lffs_hash_list_num_get (struct aw_lffs_hash_table  *p_hash, 
                               int                         list_index);

/**
 * \brief 获取hash表中总的元素数量
 *
 * \param[in]       p_hash      hash表地址
 *
 * \retval  该hash表中的元素数量
 */
int aw_lffs_hash_num_get (struct aw_lffs_hash_table  *p_hash);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* _AW_LFFS_HASH_H_ */

/* end of file */

