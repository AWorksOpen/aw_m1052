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
 * \brief ����hash����ȡhash�ڵ�
 *
 * \param[in]       p_node          ��Ż�ȡ���Ľڵ�
 * \param[in]       p_tab           hash��
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
 * \brief hash�����ӣ������������ͬ�ؼ��ֵĽ��
 *
 * \param[in]       p_hash      hash���ַ
 * \param[in]       p_key       �ؼ���ָ��
 * \param[in]       p_node      ��ǰ��㣬����ΪNULL
 *
 * \retval  AW_OK    : ��ӳɹ�
 *          -EINVAL  : ������Ч
 *          -EEXIST  : �ùؼ��ֵĽ���Ѿ�����
 *          -ENOTSUP : �ùؼ�����Ч
 */
int aw_lffs_hash_add (struct aw_lffs_hash_table  *p_hash,
                      const void                 *p_key,
                      struct aw_lffs_hash_node   *p_node);

/**
 * \brief ɾ��hash��㣬�ùؼ���Ӧ����Ψһ��
 *
 * \param[in]       p_hash      hash���ַ
 * \param[in]       p_key       �ؼ���ָ��
 *
 * \retval  AW_OK     : ��ӳɹ�
 *          -EINVAL   : ������Ч
 *          -ENODATA  : �Ҳ������
 */
int aw_lffs_hash_del (struct aw_lffs_hash_table  *p_hash,
                      const void                 *p_key);

/**
 * \brief hash�����ӣ����������ͬ�ؼ��ֵĽ�㣬������ַ���ܣ�����ͬһ����㣩
 *
 * \param[in]       p_hash      hash���ַ
 * \param[in]       p_key       �ؼ���ָ��
 * \param[in]       p_node      ��ǰ��㣬����ΪNULL
 *
 * \retval  AW_OK    : ��ӳɹ�
 *          -EINVAL  : ������Ч
 *          -ENOTSUP : �ùؼ�����Ч
 *
 * \note  �øýӿ���ӵĽ�㣬����ʱӦ���� aw_lffs_same_hash_next_find() �ӿ�
 */
int aw_lffs_hash_add_ex (struct aw_lffs_hash_table  *p_hash,
                         const void                 *p_key,
                         struct aw_lffs_hash_node   *p_node);

/**
 * \brief ɾ��hash���
 *
 * \param[in]       p_hash      hash���ַ
 * \param[in]       p_node      hash��㣬����ΪNULL
 *
 * \retval  AW_OK     : ��ӳɹ�
 *          -EINVAL   : ������Ч
 */
int aw_lffs_hash_del_ex (struct aw_lffs_hash_table  *p_hash,
                         struct aw_lffs_hash_node   *p_node);

/**
 * \brief ��ȡp_node�ڵ����һ����㣬p_nodeΪ�����ȡ��һ���ڵ�
 *
 * \param[in]       p_hash      hash���ַ
 * \param[in]       p_node      ��ǰ��㣬��ΪNULL
 * \param[in]       p_key       �ؼ���ָ��
 *
 * \retval  NULL: ����Ϊ��
 *          ELSE: ���ҵ�hash���ָ��
 */
struct aw_lffs_hash_node *aw_lffs_hash_next_find (
    struct aw_lffs_hash_table  *p_hash,
    struct aw_lffs_hash_node   *p_node);

/**
 * \brief ��ȡ��p_node�Ĺؼ�����ͬ����һ����㣬p_nodeΪ�����ȡ�ùؼ��ֵĵ�һ��
 *        �ڵ�
 *
 * \param[in]       p_hash      hash���ַ
 * \param[in]       p_node      ��ǰ��㣬��ΪNULL
 * \param[in]       p_key       �ؼ���ָ��
 *
 * \retval  NULL: ����Ϊ��
 *          ELSE: ���ҵ�hash���ָ��
 */
struct aw_lffs_hash_node *aw_lffs_same_hash_next_find (
    struct aw_lffs_hash_table  *p_hash,
    struct aw_lffs_hash_node   *p_node,
    const void                 *p_key);

/**
 * \brief ��ȡָ����hash�����Ԫ������
 *
 * \param[in]       p_hash      hash���ַ
 * \param[in]       list_index  hash�����id
 *
 * \retval  �������е�Ԫ������
 */
int aw_lffs_hash_list_num_get (struct aw_lffs_hash_table  *p_hash, 
                               int                         list_index);

/**
 * \brief ��ȡhash�����ܵ�Ԫ������
 *
 * \param[in]       p_hash      hash���ַ
 *
 * \retval  ��hash���е�Ԫ������
 */
int aw_lffs_hash_num_get (struct aw_lffs_hash_table  *p_hash);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* _AW_LFFS_HASH_H_ */

/* end of file */

