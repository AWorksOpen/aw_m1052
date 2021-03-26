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
 * \brief rtk 消息队列相关定义和外部接口函数
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
 * \brief 初始化一个消息队列
 *
 * 不能在中断中调用
 *
 * \param[in] p_msgq            struct rtk_msgq结构
 * \param[in] p_msgq_node_array 一个msgq节点数组
 * \param[in] buff              msgq的数据存储内存首地址
 * \param[in] buffer_size       msgq的数据存储内存大小
 * \param[in] unit_size         msgq的每个消息的大小
 *
 * \retval    失败返回NULL，成功则返回执行rtk_msgq结构的指针
 */
struct rtk_msgq *rtk_msgq_init(
        struct rtk_msgq *p_msgq,
        struct rtk_msgq_node *p_msgq_node_array,
        uint8_t *buff,
        uint32_t buffer_size,
        uint32_t unit_size );

/**
 * \brief 查询消息队列中是否没有消息(为空）
 *
 * 不能在中断中调用
 *
 * \param[in]   p_msgq          struct rtk_msgq结构
 * \param[out]  p_is_empty      当函数返回AW_OK时，用于输出是否为空
 *                              0表示不为空，1表示为空
 * \retval      0               查询消息队列操作成功，否则返回负的错误码
 * \retval      -AW_EINVAL      消息队列无效，或是p_is_empty无效
 * \retval      -AW_EPERM       在中断中操作
 */
int rtk_msgq_is_empty( struct rtk_msgq *p_msgq,int *p_is_empty);

/**
 * \brief 查询消息队列是否已满(不能再发送消息)
 *
 * 不能在中断中调用
 *
 * \param[in]   p_msgq          struct rtk_msgq结构
 * \param[out]  p_is_full       当函数返回AW_OK时，用于输出是否已满
 *                                  0表示未满，1表示已满
 * \retval      0               查询消息队列操作成功，否则返回负的错误码
 * \retval      -AW_EINVAL      消息队列无效，或是p_is_full无效
 * \retval      -AW_EPERM       在中断中操作
 */
int rtk_msgq_is_full( struct rtk_msgq *p_msgq, int *p_is_full);

/**
 * \brief 终止一个消息队列，使得消息队列无效
 * 所有等待在消息队列上(读写)的任务都会得到返回值-AW_ENXIO。
 * 不能在中断中调用
 *
 * \param[in]   p_msgq          struct rtk_msgq结构
 * \retval      0               操作成功，否则返回负的错误码
 * \retval      -AW_EINVAL      消息队列无效
 * \retval      -AW_EPERM       在中断中操作
 */
int rtk_msgq_terminate( struct rtk_msgq *p_msgq );


/**
 * \brief 从消息队列上接收一个消息
 *
 * \param[in]   p_msgq          struct rtk_msgq结构
 * \param[in]   buff            存储接收到的消息的内存，可以为NULL
 *                              如果buff是NULL，则仅仅将消息从消息队列中移除
 * \param[in]   buff_size       存储消息的内存大小，如果内存大小小于消息的大小
 *                              则消息会被截断
 * \param[in]   tick            超时时间，即如果没有消息，最大等待接收时间
 *                              AW_WAIT_FOREVER表示永久等待
 * \retval      0               操作成功，否则返回负的错误码
 * \retval      -AW_EINVAL      参数无效
 * \retval      -AW_EPERM       在中断中操作
 * \retval      -AW_ETIME       接收超时
 */
int rtk_msgq_receive(
        struct rtk_msgq *p_msgq,
        void *buff,
        size_t buff_size,
        aw_tick_t tick );

/**
 * \brief 从消息队列上发送一个消息
 *
 * \param[in]   p_msgq          struct rtk_msgq结构
 * \param[in]   buff            要发送消息的内存首地址
 * \param[in]   size            要发送消息的大小，如果消息大小大于消息队列中
 *                              每个消息的大小，则发送的消息会被截断
 * \param[in]   tick            超时时间，即如果发送空间，最大等待发送时间
 *                              AW_WAIT_FOREVER表示永久等待
 * \retval      0               操作成功，否则返回负的错误码
 * \retval      -AW_EINVAL      参数无效
 * \retval      -AW_EPERM       在中断中操作
 * \retval      -AW_ETIME       发送超时
 */
int rtk_msgq_send(
        struct rtk_msgq *p_msgq,
        const void *buff,
        size_t size,
        aw_tick_t tick );

/**
 * \brief 清理一个消息队列，使得消息队列中没有消息
 *
 * 不能在中断中调用
 *
 * \param[in]   p_msgq          struct rtk_msgq结构
 * \retval      0               操作成功，否则返回负的错误码
 * \retval      -AW_EINVAL      消息队列无效
 * \retval      -AW_EPERM       在中断中操作
 */
int rtk_msgq_clear( struct rtk_msgq *p_msgq );

/**
 * \brief 指定的消息队列是否有效
 *
 * 不能在中断中调用
 *
 * \param[in]   p_msgq          struct rtk_msgq结构
 * \retval      AW_TRUE         有效
 * \retval      AW_FALSE        无效
 */
aw_bool_t rtk_msgq_is_valid( struct rtk_msgq *p_msgq );

/**
 *  @}
 */


#endif          /*__RTK_MSGQ_H__*/
