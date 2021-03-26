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
 * \brief �ڴ��
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ� aw_pool.h
 *
 * �ڴ���ṩ��һ�ֿ�����Ч���ڴ����ʽ��ʹ���ڴ�ؼ����˴Ӷ��з����ڴ�Ĵ�����
 * �Ӷ�������Ƶ�����䡢�ͷ��ڴ������µ��ڴ���Ƭ��
 *
 * �ڴ���е����ж��󶼾�����ͬ�Ĵ�С��
 *
 * \par ��ʼ���ڴ��
 * \code
 *
 * static aw_pool_t    my_pool;    // �ڴ�ض���
 * static aw_pool_id_t my_pool_id; // �ڴ�ؾ��
 * static int          bulk[100];  // �ڴ��
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
 * \par ����ͷ����ڴ�ص�Ԫ
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

/** \brief �ڴ�صĶ��� */
struct aw_pool {

    /** \brief ���п�������� */
    void *p_free;

    /** \brief �ڴ�ص�ԭʼ��� */
    void *p_start;

    /** \brief �ڴ�ع�������һ���ڴ�� */
    void *p_end;

    /** \brief �ڴ�ص�Ԫ�����ֵ */
    size_t item_size;

    /** \brief �ڴ�ص�Ԫ������ */
    size_t item_count;

    /** \brief ʣ����п���� */
    size_t nfree;

    /**
     * \brief ���п�����������
     *
     * ����һ��ͳ���������������ڴ������������ʣ���������ֵ��
     *
     * \sa aw_pool_margin_get()
     */
    size_t nmin;

};

/** \brief �ڴ�� */
typedef struct aw_pool aw_pool_t;

/** \brief �ڴ�ؾ�� */
typedef aw_pool_t *aw_pool_id_t;

/**
 * \brief �ڴ��ʵ����ʼ��
 *
 * \param[in] p_pool       �ڴ��ʵ��ָ��
 * \param[in] p_pool_mem   �ڴ�ص�Ԫʹ�ÿռ����ʼ��ַ
 * \param[in] pool_size    �ڴ�ص�Ԫ�ռ��ܴ�С
 * \param[in] item_size    �����ڴ�ص�Ԫ�ռ��С
 *
 * \return ���ɹ��������ڴ�ؾ������ʧ��ʱ������NULL
 */
aw_pool_id_t aw_pool_init(aw_pool_t *p_pool,
                          void      *p_pool_mem,
                          size_t     pool_size,
                          size_t     item_size);

/**
 * \brief ���ڴ���л�ȡһ����Ԫ
 *
 * \param[in] pool_id      �ڴ�ؾ�� 
 *
 * \return ���ɹ�������һ���ڴ�ص�Ԫָ�룻��ʧ��ʱ������NULL
 */
void *aw_pool_item_get (aw_pool_id_t pool_id);

/**
 * \brief �ͷ�һ���ڴ�ص�Ԫ
 *
 * \param[in] pool_id  �ڴ�ؾ��
 * \param[in] p_item   �ڴ�ص�Ԫָ��
 *
 * \retval AW_OK       �ͷųɹ�
 * \retval -AW_EFAULT  �ͷ�ʧ��
 */
aw_err_t aw_pool_item_return (aw_pool_id_t pool_id, void *p_item);


/**
 * \brief ��ȡ��ǰ�ڴ�أ������������½�
 *
 * �����鿴�ڴ�ص�ʹ�����������ķ�ֵ��
 *
 * \param[in] pool_id   �ڴ�ؾ�� 
 *
 * \return �����ڴ�ؽṹ��� \e nmin ��Ա
 */
size_t aw_pool_margin_get (aw_pool_id_t pool_id);

/**
 * \brief ��ȡ�ڴ�ص�Ԫ�ռ��С
 *
 * \param[in] pool_id  �ڴ�ؾ��
 *
 * \return �����ڴ�ص�Ԫ�ռ��С
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
