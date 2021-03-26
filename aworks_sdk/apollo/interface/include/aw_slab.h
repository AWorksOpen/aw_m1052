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
 * \brief slab�ڴ��������
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ� aw_slab.h
 *
 *  ʹ�ñ����񣬿��Լ�����ΪƵ�����룬�ͷ�С�ڴ����ɵ��ڲ���Ƭ��
 *  ���ή���ڴ�ʹ����(��Ч����)��
 *  Ϊ��ʡȥ�û���Ϥ���ýӿ�aw_slab_catenate�Ĳ������ṩ�ӿ�
 *  aw_slab_easy_catenate()��ֻ��Ҫ�ṩҪ������ڴ��ַ�ʹ�С���ɣ�
 *  ���ҿ��Զ�ε��ã��ﵽ�������ڴ��Ч����
 *
 * \par ��ʼ��slab������
 * \code
 *
 * static struct aw_slab foo;
 *
 * static void init_aw_slab(void)
 * {
 *     aw_slab_init(&foo);
 * } 
 * \endcode 
 *  
 * \par ��slab����������ڴ�
 * \code
 *
 * #define TRUNK_SIZE 1048576
 *  
 * static void add_trunk(void)
 * {
 *      void *p_trunk;
 *
 *      p_trunk = aw_mem_alloc(TRUNK_SIZE);
 *
 *      aw_slab_easy_catenate(&foo, p_trunk, TRUNK_SIZE); 
 * }
 * \endcode
 * 
 * \par ������ͷ��ڴ�
 * \code
 * int test_aw_slab(void)
 * {
 *     void *object;
 *
 *     object = aw_slab_malloc(&foo, 1024);
 *
 *     if(object) {
 *          aw_slab_free(&foo, object);    
 *     }  
 * }
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.01 17-03-09  imp, add aw_slab_memalign()
 * - 1.00 17-03-03  imp, first implementation
 * \endinternal
 */

#ifndef __AW_SLAB_H
#define __AW_SLAB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aworks.h"
#include "aw_types.h"
#include "aw_errno.h"
#include "aw_list.h"

/**
 * \addtogroup grp_aw_if_slab
 * \copydoc aw_slab.h
 * @{
 */

/** \brief �������ӡ� */
#define AW_SLAB_USING_HOOK  

/** \brief ��¼�ڴ��������� */
#define AW_SLAB_MEM_STATS   

/** \brief zone���������ֵ�������72�� */
#define AW_SLAB_NZONES_MAX      72   


/**
 * \brief aw_slab�ṹ��
 */
struct aw_slab {

#ifdef AW_SLAB_MEM_STATS

    /** \brief ��ǰ�����ȥ���ڴ�. */
    size_t used_mem;

    /** \brief �������ȥ���ڴ�. */
    size_t max_mem;   
#endif

#ifdef AW_SLAB_USING_HOOK

    /** 
     * \brief �����ڴ�ʱʹ�õĹ��Ӻ����� 
     *
     * ��Ҫ����� AW_SLAB_USING_HOOK ��
     *
     * \param[in] ptr   ����������ڴ��ַ
     * \param[in] size  ����Ĵ�С
     */
    void (*pfn_malloc_hook)(void *ptr, size_t size); 

    /** 
     * \brief �����ڴ�ʱʹ�õĹ��Ӻ�����   
     *
     * ��Ҫ����� AW_SLAB_USING_HOOK ��
     *
     * \param[in] ptr  ���յ��ڴ��ַ
     */
    void (*pfn_free_hook)(void *ptr);                
#endif

    /** \brief ���ڴ����. */
    struct aw_list_head heap;   
};

/**
 * \brief ��ʼ��slab��������
 *
 * \param[in,out] p_slab   slab������
 */
extern
void aw_slab_init (struct aw_slab *p_slab);


/**
 * \brief ����������ݴ���Ĳ������ø��ڴ�����ԡ�
 *
 * \param[in,out] p_slab       slab������
 * \param[in] begin_addr       Ҫ��aw_slab������ڴ����ʼ��ַ
 * \param[in] end_addr         Ҫ��aw_slab������ڴ�Ľ�����ַ
 * \param[in] page_bits        ҳ���С����1����page_bits���
 * \param[in] thresh           ����zone�ĸ�����ֵ
 * \param[in] zone_min_sz      aw_slab�Ĺ������zone����ռ�ڴ����Сֵ
 * \param[in] zone_max_sz      aw_slab�Ĺ������zone����ռ�ڴ�����ֵ
 * \param[in] zone_max_limit   ��aw_slab���ܹ�����ڴ�����ֵ
 *                             ������Ĵ�С�����ֵ������ͨ��aw_slab����
 *
 * \retval  AW_OK       �ɹ�
 * \retval -AW_EINVAL   ����Ĳ����޷����ڴ����ӵ�aw_slab
 */
extern
aw_err_t  aw_slab_heap_catenate (struct aw_slab *p_slab,
                                 void           *begin_addr, 
                                 void           *end_addr,
                                 uint8_t         page_bits,
                                 uint8_t         thresh,
                                 uint32_t        zone_min_sz,
                                 uint32_t        zone_max_sz,
                                 uint32_t        zone_max_limit);

/**
 * \brief ����������� \e mem_size ѡ����ʵĲ������ø��ڴ�ķ���ʱ��
 *
 * \param[in,out] p_slab    slab������
 * \param[in] p_mem        �ڴ�ռ����ʼ��ַ
 * \param[in] mem_size     �ڴ�ռ�Ĵ�С
 *
 * \retval  AW_OK          �ɹ�
 * \retval -AW_EINVAL      ������Ч
 */
extern
aw_err_t  aw_slab_easy_catenate(struct aw_slab *p_slab, 
                                char           *p_mem, 
                                uint32_t        mem_size);


/**
 * \brief ����������������� \e size �ֽڵ��ڴ�������ߡ�
 *
 * \param[in] p_slab     slab������
 * \param[in] size       ������ڴ��С
 *
 * \return ���ɹ����䣬�򷵻ظ��ڴ��ַ�����򣬷���NULL��
 */
extern
void *aw_slab_malloc(struct aw_slab *p_slab, size_t size);


/**
 * \brief ������������·�������� \e size �ֽڴ�С���ڴ档
 *
 * \param[in] p_slab     slab������
 * \param[in] ptr        ��Ҫ���·�����ڴ�ռ�
 * \param[in] size       ���·���Ĵ�С
 *
 * \retval   NULL        �ڴ治������������sizeΪ0
 * \retval   �ڴ��ַ    ����ɹ�
 *
 * \note �����ز���NULLʱ����Ӧ����ʹ�� \e ptr ��ŵĵ�ַ��
 */
extern
void *aw_slab_realloc(struct aw_slab *p_slab, void *ptr, size_t size);


/**
 * \brief ������������������ \e count ������СΪ \e size �ֽڵ��ڴ�������ߡ�
 *
 * \param[in] p_slab     slab������
 * \param[in] count      ����ĸ���
 * \param[in] size       ����Ĵ�С
 *
 * \retval   NULL        �ڴ治����߲�����Ч
 * \retval   �ڴ��ַ    ����ɹ�
 */
extern
void *aw_slab_calloc(struct aw_slab *p_slab, size_t count, size_t size);

/**
 * \brief ��������� \e ptr ָ����ڴ淵�ظ� \e p_slab ����Ķ��
 *
 * \param[in] p_slab     slab������
 * \param[in] ptr        ���յ��ڴ�
 *
 */
extern
void  aw_slab_free(struct aw_slab *p_slab, void *ptr);


/**
 * \brief ���������Ѱ�Ҷ��� \e alignment �ֽڵ��ڴ棬Ȼ�����������ߡ�
 *
 * ���� \e alignment ��������2���ݵ�Ҫ�󣬵�����������ڴ治��ʱ��
 * \e pp_memptr �᷵��NULL��
 *       
 * \param[in]     p_slab       slab������
 * \param[in,out] pp_memptr    ��ŷ��ص�ַ
 * \param[in]     alignment    ������ֽ�
 * \param[in]     size         ����Ĵ�С
 *
 * \retval  AW_OK              �����ɹ�
 * \retval -AW_ENOMEM          �ڴ治��
 */
extern 
aw_err_t aw_slab_memalign(struct aw_slab *p_slab, 
                          void          **pp_memptr, 
                          size_t          alignment, 
                          size_t          size);


#ifdef AW_SLAB_MEM_STATS

/**
 * \brief ��������᷵���ڴ�ʹ�������
 *
 * ��Ҫ����� AW_SLAB_MEM_STATS ��
 *
 * \param[in]     p_slab        slab������
 * \param[in,out] p_total       �ܹ�ӵ�е��ڴ�
 * \param[in,out] p_used        ��ǰʹ����
 * \param[in,out] p_max_used    �ڴ����ķ�ֵ
 *
 * \retval  AW_OK       �����ɹ�
 * \retval -AW_ENOMEM   �ڴ治��
 */
extern
aw_err_t  aw_slab_memory_info (struct aw_slab *p_slab, 
                               uint32_t       *p_total, 
                               uint32_t       *p_used, 
                               uint32_t       *p_max_used);
#endif


#ifdef AW_SLAB_USING_HOOK

/**
 * \brief �������������һ�����Ӻ���������̬�ڴ����ɹ�ʱ���á�
 *
 * ��Ҫ����� AW_SLAB_USING_HOOK ��
 *
 * \param[in] p_slab    slab������
 * \param[in] pfn_hook  ���Ӻ���
 *
 */
extern
void aw_slab_malloc_sethook(struct aw_slab *p_slab,
                            void          (*pfn_hook)(void *ptr, size_t size));

/**
 * \brief �������������һ�����Ӻ��������ڴ����ʱ���á�
 *
 * ��Ҫ����� AW_SLAB_USING_HOOK ��
 *
 * \param[in] p_slab    slab������
 * \param[in] pfn_hook  ���Ӻ���
 *
 */
extern
void aw_slab_free_sethook(struct aw_slab *p_slab,
                          void          (*pfn_hook)(void *ptr));

#endif

/** @}  grp_aw_if_slab */

#ifdef __cplusplus
}
#endif

#endif /* __AW_SLAB_H */

/* end of file */
