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
 * \file  rtk_msgq.h
 * \brief rtk ��Ϣ������ض�����ⲿ�ӿں���
 *
 * \internal
 * \par modification history:
 * - 1.00 18-07-24  sni, first implement
 * \endinternal
 */

#ifndef         __RTK_MSGQ_H__
#define         __RTK_MSGQ_H__

/**
 *  \defgroup MSGQ_API message queue API
 *  @{
 */
#include "aw_types.h"
struct rtk_msgq_node
{
    struct rtk_msgq_node *next;
};

struct rtk_msgq
{
    /** read semaphore */
    struct rtk_semc                 semc_read;          
    /** write semaphore */
    struct rtk_semc                 semc_write;         
    /** buffer size */
    int                             buff_size;         
    /** max unit  */
    int                             count;              
    /** element size */
    size_t                          unit_size;          
    /** read list head */
    struct rtk_msgq_node           *full_list_head;     
    /** read list tail */
    struct rtk_msgq_node           *full_list_tail;     
    /** not used storages */
    struct rtk_msgq_node           *free_list_head;     
    /** dynamic allocated */
    uint8_t                        *buff;               
    /** \brief pointer to node array */
    struct rtk_msgq_node           *p_msgq_nodes;       
    uint32_t                        ref_count;
    unsigned long                   magic_num;

};

typedef struct {
    struct rtk_msgq            *p_msgq;
    struct rtk_msgq_node       *p_msgq_node;
    uint8_t                    *p_msgq_storage;
    uint32_t                    msgq_storage_size;
}rtk_msgq_info_t;

#define RTK_MSGQ_DECL(name, msg_size, msg_count ) \
        static struct rtk_msgq      name##msgq; \
        static struct rtk_msgq_node name##node_storage[(msg_count)]; \
        static uint8_t              name##storage[(msg_count)*(msg_size)]; \
        rtk_msgq_info_t aw_const    name = { \
                                        &name##msgq, \
                                        name##node_storage, \
                                        name##storage, \
                                        sizeof(name##storage) \
                                    };

#define RTK_MSGQ_INIT(name, msg_size, msg_count)  \
    rtk_msgq_init( &(name).p_msgq, (name).p_msgq_node,\
            (name).p_msgq_storage, (name).msgq_storage_size, msg_size )

/**
 * \brief ��ʼ��һ����Ϣ����
 *
 * �������ж��е���
 *
 * \param[in] p_msgq            struct rtk_msgq�ṹ
 * \param[in] p_msgq_node_array һ��msgq�ڵ�����
 * \param[in] buff              msgq�����ݴ洢�ڴ��׵�ַ
 * \param[in] buffer_size       msgq�����ݴ洢�ڴ��С
 * \param[in] unit_size         msgq��ÿ����Ϣ�Ĵ�С
 *
 * \retval    ʧ�ܷ���NULL���ɹ��򷵻�ִ��rtk_msgq�ṹ��ָ��
 */
struct rtk_msgq *rtk_msgq_init(
        struct rtk_msgq *p_msgq,
        struct rtk_msgq_node *p_msgq_node_array,
        uint8_t *buff,
        uint32_t buffer_size,
        uint32_t unit_size );

/**
 * \brief ��ѯ��Ϣ�������Ƿ�û����Ϣ(Ϊ�գ�
 *
 * �������ж��е���
 *
 * \param[in]   p_msgq          struct rtk_msgq�ṹ
 * \param[out]  p_is_empty      ����������AW_OKʱ����������Ƿ�Ϊ��
 *                              0��ʾ��Ϊ�գ�1��ʾΪ��
 * \retval      0               ��ѯ��Ϣ���в����ɹ������򷵻ظ��Ĵ�����
 * \retval      -AW_EINVAL      ��Ϣ������Ч������p_is_empty��Ч
 * \retval      -AW_EPERM       ���ж��в���
 */
int rtk_msgq_is_empty( struct rtk_msgq *p_msgq,int *p_is_empty);

/**
 * \brief ��ѯ��Ϣ�����Ƿ�����(�����ٷ�����Ϣ)
 *
 * �������ж��е���
 *
 * \param[in]   p_msgq          struct rtk_msgq�ṹ
 * \param[out]  p_is_full       ����������AW_OKʱ����������Ƿ�����
 *                                  0��ʾδ����1��ʾ����
 * \retval      0               ��ѯ��Ϣ���в����ɹ������򷵻ظ��Ĵ�����
 * \retval      -AW_EINVAL      ��Ϣ������Ч������p_is_full��Ч
 * \retval      -AW_EPERM       ���ж��в���
 */
int rtk_msgq_is_full( struct rtk_msgq *p_msgq, int *p_is_full);

/**
 * \brief ��ֹһ����Ϣ���У�ʹ����Ϣ������Ч
 * ���еȴ�����Ϣ������(��д)�����񶼻�õ�����ֵ-AW_ENXIO��
 * �������ж��е���
 *
 * \param[in]   p_msgq          struct rtk_msgq�ṹ
 * \retval      0               �����ɹ������򷵻ظ��Ĵ�����
 * \retval      -AW_EINVAL      ��Ϣ������Ч
 * \retval      -AW_EPERM       ���ж��в���
 */
int rtk_msgq_terminate( struct rtk_msgq *p_msgq );


/**
 * \brief ����Ϣ�����Ͻ���һ����Ϣ
 *
 * \param[in]   p_msgq          struct rtk_msgq�ṹ
 * \param[in]   buff            �洢���յ�����Ϣ���ڴ棬����ΪNULL
 *                              ���buff��NULL�����������Ϣ����Ϣ�������Ƴ�
 * \param[in]   buff_size       �洢��Ϣ���ڴ��С������ڴ��СС����Ϣ�Ĵ�С
 *                              ����Ϣ�ᱻ�ض�
 * \param[in]   tick            ��ʱʱ�䣬�����û����Ϣ�����ȴ�����ʱ��
 *                              AW_WAIT_FOREVER��ʾ���õȴ�
 * \retval      0               �����ɹ������򷵻ظ��Ĵ�����
 * \retval      -AW_EINVAL      ������Ч
 * \retval      -AW_EPERM       ���ж��в���
 * \retval      -AW_ETIME       ���ճ�ʱ
 */
int rtk_msgq_receive(
        struct rtk_msgq *p_msgq,
        void *buff,
        size_t buff_size,
        aw_tick_t tick );

/**
 * \brief ����Ϣ�����Ϸ���һ����Ϣ
 *
 * \param[in]   p_msgq          struct rtk_msgq�ṹ
 * \param[in]   buff            Ҫ������Ϣ���ڴ��׵�ַ
 * \param[in]   size            Ҫ������Ϣ�Ĵ�С�������Ϣ��С������Ϣ������
 *                              ÿ����Ϣ�Ĵ�С�����͵���Ϣ�ᱻ�ض�
 * \param[in]   tick            ��ʱʱ�䣬��������Ϳռ䣬���ȴ�����ʱ��
 *                              AW_WAIT_FOREVER��ʾ���õȴ�
 * \retval      0               �����ɹ������򷵻ظ��Ĵ�����
 * \retval      -AW_EINVAL      ������Ч
 * \retval      -AW_EPERM       ���ж��в���
 * \retval      -AW_ETIME       ���ͳ�ʱ
 */
int rtk_msgq_send(
        struct rtk_msgq *p_msgq,
        const void *buff,
        size_t size,
        aw_tick_t tick );

/**
 * \brief ����һ����Ϣ���У�ʹ����Ϣ������û����Ϣ
 *
 * �������ж��е���
 *
 * \param[in]   p_msgq          struct rtk_msgq�ṹ
 * \retval      0               �����ɹ������򷵻ظ��Ĵ�����
 * \retval      -AW_EINVAL      ��Ϣ������Ч
 * \retval      -AW_EPERM       ���ж��в���
 */
int rtk_msgq_clear( struct rtk_msgq *p_msgq );

/**
 * \brief ָ������Ϣ�����Ƿ���Ч
 *
 * �������ж��е���
 *
 * \param[in]   p_msgq          struct rtk_msgq�ṹ
 * \retval      AW_TRUE         ��Ч
 * \retval      AW_FALSE        ��Ч
 */
aw_bool_t rtk_msgq_is_valid( struct rtk_msgq *p_msgq );

/**
 *  @}
 */


#endif          /*__RTK_MSGQ_H__*/
