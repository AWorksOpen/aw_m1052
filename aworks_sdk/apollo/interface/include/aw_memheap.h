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
 * \brief �ѹ�����
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ� aw_memheap.h
 *
 * \par ��ʼ���ڴ��
 * \code
 * 
 * aw_memheap_t my_heap;  
 *
 * static void memheap_init(void)
 * {
 *      aw_memheap_init(&my_heap,
 *                      "my_heap",   // �ڴ�ѵ�����
 *                       MEM_ADDR,   // ���õĵ�ַ�ռ�
 *                       MEM_SIZE);  // ��ַ�ռ��С
 * }
 * \endcode
 *
 * \par ���롢�ͷ��ڴ�
 * \code
 *
 * static void memheap_init(void)
 * {
 *      void *ptr = aw_memheap_alloc(&my_heap, 32);
 *     
 *      aw_memheap_free(ptr);    
 * }
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 14-06-13  zen, first impementation
 * \endinternal
 */

#ifndef __AW_MEMHEAP_H
#define __AW_MEMHEAP_H

#include "aworks.h"
#include "aw_sem.h"

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus	*/

/**
 * \addtogroup grp_aw_if_memheap
 * \copydoc aw_memheap.h
 * @{
 */

/** \brief �ڴ�ѵ��ڴ浥Ԫ�Ķ��� */
struct aw_memheap_item {

    /** \brief �ڴ浥Ԫ��ħ��������дԽ�� */
    uint32_t                magic;    

    /** \brief ���ڴ��ָ�� */                  
    struct aw_memheap      *pool_ptr;                   

     /** \brief ��һ���ڴ浥Ԫ */
    struct aw_memheap_item *next;        

    /** \brief ǰһ���ڴ浥Ԫ */              
    struct aw_memheap_item *prev;                       

    /** \brief ��һ�����е��ڴ浥Ԫ */
    struct aw_memheap_item *next_free;    

    /** \brief ǰһ�����е��ڴ浥Ԫ */              
    struct aw_memheap_item *prev_free;     

};

/** \brief �ڴ�ѵ��ڴ浥Ԫ */
typedef struct aw_memheap_item aw_memheap_item_t;

/**
 * \brief �ڴ�ѵĶ���
 *
 * \note �û���Ӧ���޸Ľṹ��ĳ�Ա��
 */
struct aw_memheap {

    /** \brief �ѵ����� */
    const char             *name;    

    /** \brief ���ڴ����ʼ��ַ */           
    void                   *start_addr;         

    /** \brief ���ڴ�Ĵ�С */
    uint32_t                pool_size;   

    /** \brief ���ô�С */       
    uint32_t                available_size;     

    /** \brief ��������ֵ */
    uint32_t                max_used_size;      

    /** \brief ��ǰʹ�õĵ�Ԫ���� */
    struct aw_memheap_item *block_list;         

    /** \brief ���е�Ԫ���� */
    struct aw_memheap_item *free_list;         

    /** \brief ���п������ͷ */ 
    struct aw_memheap_item  free_header;        

    /** \brief ������ */
    AW_MUTEX_DECL(lock);     
};

/** \brief �ڴ�� */
typedef struct aw_memheap aw_memheap_t;

/**
 * \brief ��ʼ���ڴ��
 *
 * \param[in,out]  memheap     ���ڴ�ʹ�õĽṹ��
 * \param[in]      name        ���ڴ������
 * \param[in]      start_addr  ���ڴ����ʼ��ַ
 * \param[in]      size        ���ڴ�Ĵ�С
 *
 * \retval AW_OK ��ʼ���ɹ�
 */
aw_err_t aw_memheap_init(struct aw_memheap *memheap,
                         const char        *name,
                         void              *start_addr,
                         uint32_t           size);


/**
 * \brief ���ڴ�ѷ����ڴ�
 *
 * \param[in]  heap   Ŀ���ڴ��
 * \param[in]  size   �����ڴ�Ĵ�С
 *
 * \return ���ɹ������ط���õ����ڴ��ַ����ʧ�ܣ�����NULL
 */
void *aw_memheap_alloc(struct aw_memheap *heap, uint32_t size);


/**
 * \brief ����ڴ�Ĵ�С
 *
 * \param[in]  heap   Ŀ���ڴ��
 * \param[in]  ptr    �ڴ��ַ
 *
 * \return ���� \e ptr ��ָ����ڴ�ռ��С 
 */
size_t aw_memheap_memsize(struct aw_memheap *heap, void *ptr);


/**
 * \brief �ı��ѷ����ڴ�ռ�Ĵ�С
 *
 * \param[in]  heap      Ŀ���ڴ��
 * \param[in]  ptr       �ڴ��ַ
 * \param[in]  newsize   ���·���Ĵ�С
 *
 * \return �ɹ�ʱ�����������СҪ����ڴ�ռ��ַ��
 *         ʧ��ʱ������NULL��
 */
void *aw_memheap_realloc(struct aw_memheap *heap, void *ptr, size_t newsize);


/**
 * \brief �ͷ��ڴ�
 *
 * \param[in]  ptr ���ڴ�ѷ���õ����ڴ��ַ
 */
void aw_memheap_free(void *ptr);

/** @}  grp_aw_if_memheap */

#ifdef __cplusplus
}
#endif	/* __cplusplus 	*/

#endif /* __AW_MEMHEAP_H    */

/* end of file */
