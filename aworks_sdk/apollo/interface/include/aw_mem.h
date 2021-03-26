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
 * \brief �ڴ�����׼�ӿ�
 *
 * ʹ�ñ�������Ҫ��������ͷ�ļ���
 * \code
 * #include "aw_mem.h"
 * \endcode
 * �������ṩ�˶�̬�ڴ����Ľӿ�
 * \par ��ʾ��
 * \code
 *
 * \endcode
 *
 * // �������ݴ���ӡ�����
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
 * \brief �����ַ����Ķ�̬�ڴ�
 * \attention �жϲ��ɵ���
 *
 * \param[in] size    �����ڴ��С
 * \param[in] align   ��ַ�����С
 *
 * \retval    NULL ʧ��/ !NULL �ɹ����ض�̬�ڴ��ַ
 */
void *aw_mem_align(size_t size, size_t align);

/**
 * \brief     ���䶯̬�ڴ�
 * \attention �жϲ��ɵ���
 *
 * \param[in] size    �����ڴ��С
 *
 * \retval    NULL ʧ��/ !NULL �ɹ����ض�̬�ڴ��ַ
 */
void *aw_mem_alloc(size_t size);

/**
 * \brief       ���䶯̬�ڴ棬��СΪnelem * size�� ����ʼ��Ϊ 0
 * \attention   �жϲ��ɵ���
 *
 * \param[in]   nelem   ��Ŀ
 * \param[in]   size    ��С
 *
 * \retval    NULL ʧ��/ !NULL �ɹ����ض�̬�ڴ��ַ
 */
void *aw_mem_calloc(size_t nelem, size_t size);

/**
 * \brief       �ͷŷ���Ķ�̬�ڴ�
 * \attention   �жϲ��ɵ���
 *
 * \param[in]   ptr   ��̬�����ַ
 */
void aw_mem_free(void *ptr);


/**
 * \brief �����ַ����ȡ��ʹ���ڴ�ռ��С��
 *
 * \param[in]   ptr   ��̬�����ַ
 *
 * \return ��ַ����Ӧ�Ŀռ��С
 */
size_t aw_mem_size(void *ptr);

/**
 * \brief ���·��䶯̬�ڴ�
 *
 * \param[in]   ptr         �ѷ��䶯̬�ڴ��ַ
 * \param[in]   newsize     ��С
 *
 * \retval    NULL ʧ��/ !NULL �ɹ����ض�̬�ڴ��ַ
 */
void *aw_mem_realloc(void *ptr, size_t newsize);



/**\brief �ѹ���������*/
typedef aw_psp_heap_id_t aw_heap_id_t;

/**
 * \brief �Ӷѹ������з����ַ����Ķ�̬�ڴ�
 * \attention �жϲ��ɵ���
 *
 * \param[in] heap_id �ѹ����������ַ
 * \param[in] size    �����ڴ��С
 * \param[in] align   ��ַ�����С
 *
 * \retval    NULL ʧ��/ !NULL �ɹ����ض�̬�ڴ��ַ
 */
void *aw_memp_align(aw_heap_id_t heap_id,size_t size, size_t align);

/**
 * \brief �Ӷѹ������з��䶯̬�ڴ�
 * \attention �жϲ��ɵ���
 *
 * \param[in] heap_id �ѹ����������ַ
 * \param[in] size    �����ڴ��С
 *
 * \retval    NULL ʧ��/ !NULL �ɹ����ض�̬�ڴ��ַ
 */
void *aw_memp_alloc(aw_heap_id_t heap_id,size_t size);

/**
 * \brief �Ӷѹ������з��䶯̬�ڴ棬��СΪnelem * size�� ����ʼ��Ϊ 0
 * \attention �жϲ��ɵ���
 *
 * \param[in]   heap_id �ѹ����������ַ
 * \param[in]   nelem   ��Ŀ
 * \param[in]   size    ��С
 *
 * \retval    NULL ʧ��/ !NULL �ɹ����ض�̬�ڴ��ַ
 */
void *aw_memp_calloc(aw_heap_id_t heap_id,size_t nelem, size_t size);

/**
 * \brief      �ͷŶѹ���������Ķ�̬�ڴ�
 * \attention  �жϲ��ɵ���
 *
 * \param[in]   heap_id �ѹ����������ַ
 * \param[in]   ptr     ��̬�����ַ
 */
void aw_memp_free(aw_heap_id_t heap_id, void *ptr);

/**
 * \brief �Ӷѹ����������·��䶯̬�ڴ�
 *
 * \param[in]   heap_id     �ѹ����������ַ
 * \param[in]   ptr         �ѷ��䶯̬�ڴ��ַ
 * \param[in]   newsize     ��С
 *
 * \retval    NULL ʧ��/ !NULL �ɹ����ض�̬�ڴ��ַ
 */
void *aw_memp_realloc(aw_heap_id_t heap_id, void *ptr, size_t newsize);

/** @}  grp_aw_if_mem */

#ifdef __cplusplus
}
#endif  /* __cplusplus     */

#endif  /* __AW_MEM_H */

/* end of file */
