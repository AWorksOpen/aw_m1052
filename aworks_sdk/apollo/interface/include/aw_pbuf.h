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
 * \brief 通用PBUF组件接口.
 *
 * 本模块提供用于通信中对数据缓冲区资源的管理，数据缓冲区具备引用计数机制，
 * 适用于多个模块或者通道共用相同缓存资源时，对缓冲区资源的管理,
 * 提高缓冲区资源的使用效率，降低内存的开销。
 *
 * 使用本模块需要包含头文件 aw_pbuf.h
 *
 * \par 简单示例
 * \code
 *
 *   gp_test_pool_buf = aw_pbuf_alloc(&g_pbuf_pool,       // PBUF结构体变量指针
 *                                     AW_PBUF_TYPE_POOL, // AW_PBUF_TYPE_POOL类型
 *                                     35,                // PBUF的数据长度
 *                                     NULL,              // 空
 *                                     NULL,              // 回调函数为空
 *                                     NULL);             // 回调函数参数为空
 *   //  使用中...........
 *
 *   aw_pbuf_free(gp_test_pool_buf);          // 释放该PBUF的运用
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 16-03-05 bob, first implementation.
 * \endinternal
 */

#ifndef __AW_PBUF_H /* { */
#define __AW_PBUF_H

/**
 * \addtogroup grp_aw_if_pbuf
 * \copydoc aw_pbuf.h
 * @{
 */

#include "aw_types.h"
#include "aw_common.h"
#include "aw_list.h"
#include "aw_pool.h"
#include "aw_sem.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * \brief PBUF 类型枚举
 *
 *        申请一个PBUF时需要指定PBUF的类型。
 *
 *        AW_PBUF_TYPE_RAM 类型PBUF的 数据缓冲区和PBUF头在内存堆中并由组件分配，
 *        特点是整块数据缓冲区是连续的一块存储区 域，其分配与释放由组件负责。
 *
 *        AW_PBUF_TYPE_POOL 类型的PBUF数据缓冲区和PBUF头由内存池提供，该内存池的
 *        内存和池分块大小由对象初始化时指定，由于池分块大小是固定的，所以当用户
 *        要求分配一个较大PBUF时组件会申请多个内存池分块并把它们链起来返回首个
 *        PBUF的指针，其分配与释放由组件负责，该类型的主要应用于驱动程序。
 *
 *        AW_PBUF_TYPE_ROM/REF 类型PBUF的PBUF头由内存池提供，该内存池的内存在对
 *        象初始化的时候指定，而PBUF的数据缓冲区由用户提供，在使用aw_pbuf_alloc
 *        接口时可以指定PBUF数据缓冲区，所以这两种类型的PBUF的PBUF头的申请与释放
 *        由组件负责，数据缓冲区的申请释放由用户负责，当调用aw_pbuf_free后这两种
 *        类型的PBUF的引用计数为0时，组件会释放PBUF头，这两种类型唯一不同是数据
 *        缓冲区存储区域,AW_PBUF_TYPE_ROM类型缓冲区在ROM中。
 */
typedef enum {

    /** \brief 数据缓冲区和PBUF头都在内存堆中分配（组件提供），
     *         适用于应用程序与协议栈
     */
    AW_PBUF_TYPE_RAM = 0,

    /** \brief 数据缓冲区由用户提供（ROM），PBUF头由组件提供（内存池），
     *适用于应用程序 
     */
    AW_PBUF_TYPE_ROM,

    /** \brief 数据缓冲区由用户提供，PBUF头由组件提供（内存池），
     *         适用于应用程序
     */
    AW_PBUF_TYPE_REF,

    /** \brief 数据缓冲区和PBUF头在内存池中分配，由组件提供，适用于驱动程序 */
    AW_PBUF_TYPE_POOL
} aw_pbuf_type_t;

/**
 * \brief PBUF 类型结构体
 */
struct aw_pbuf {

    struct aw_pbuf      *p_next;      /**< \brief PBUF节点指针 */

    void                *p_payload;   /**< \brief 指向PBUF的数据缓冲区（对象）*/

    /** \brief 当前PBUF及之后链上所有PBUF（同属一个包）数据缓冲区长度和，
     *         在通信过程中，一个数据包可能由多个PBUF链接而成，这个变量记录当前
     *         PBUF所属的数据包还有多少数据存储空间。
     *         规则：ret_len(p) = ret_len(p->next) + len(p->next)
     */
    size_t               tot_len;

    size_t               len;         /**< \brief PBUF数据缓冲区长度 */

    aw_pbuf_type_t       type;        /**< \brief PBUF类型 */

    uint8_t              flags;       /**< \brief PBUF标志 */
#define AW_PBUF_FLAGS_CUSTOM (1 << 0) /**< \brief 用户自定义的PBUF */

    size_t               ref;         /**< \brief 引用计数值 */

    AW_MUTEX_DECL(       mutex);      /**< \brief 互斥量,并发保护 */

    struct aw_pbuf_pool *p_pbuf_pool; /**< \brief PBUF所属于的内存池 */
};

/**
 * \brief CUSTOM类型PBUF回调函数类型定义
 */
typedef void (*aw_pbuf_custom_callback_t)(struct aw_pbuf *p, void *p_arg);

/**
 * \brief pbuf_custom类型，用于用户自定义，一般用户对象会继承PBUF
 */
struct aw_pbuf_custom {

    /** \brief PBUF结构体变量  */
    struct aw_pbuf                       pbuf;

    /** \brief PBUF回调函数，常用于回收用户对象内存（析构） */
    aw_pbuf_custom_callback_t            pfn_app_free;

    /** \brief 回调函数参数 */
    void                                *p_arg;
};

/**
 * \brief pbuf组件对象结构体
 */
struct aw_pbuf_pool {

    /** \brief 内存池，用于分配AW_PBUF_TYPE_POOL类型的PBUF节点和
     *         数据缓冲区需要的内存 
     */
    aw_pool_t            pool_pbuf_pool;

    /** \brief AW_PBUF_TYPE_POOL类型的PBUF数据缓冲区长度 */
    size_t               pool_pbuf_item_size;

    /** \brief PBUF数据缓冲区内存对齐长度 */
    size_t               pbuf_align_size;

    /** \brief 内存池，用于分配AW_PBUF_TYPE_ROM/REF类型的PBUF头需要的内存 */
    aw_pool_t            rom_ref_pbuf_pool;

    /** \brief PBUF POOL对象节点 */
    struct aw_list_head  node;
};

/**
 * \brief 通用PBUF组件初始化
 *
 * \retval 无
 */
void aw_pbuf_init(void);

/**
 * \brief PBUF对象初始化（用户需提供所需的内存池，根据用户的应用来确定参数）
 *
 * \param[in] p_pbuf_pool      指向PBUF对象的指针
 * \param[in] p_pool_mem       AW_PBUF_TYPE_POOL类型PBUF内存池首地址
 * \param[in] pool_mem_size    AW_PBUF_TYPE_POOL类型PBUF内存池大小
 *
 * \param[in] p_romref_mem     AW_PBUF_TYPE_ROM/REF类型PBUF内存池首地址
 * \param[in] romref_mem_size  AW_PBUF_TYPE_ROM/REF类型PBUF内存池大小
 *
 * \param[in] data_buf_size    POOL类型PBUF的数据缓冲区大小
 * \param[in] align_size       内存对齐长度
 *
 * \retval  AW_OK      初始化完成
 * \retval -AW_EINVAL  参数错误
 * \retval -AW_EBUSY   内存池已经被使用
 */
aw_err_t aw_pbuf_pool_init (struct aw_pbuf_pool *p_pbuf_pool,

                            void                *p_pool_mem,
                            size_t               pool_mem_size,

                            void                *p_romref_mem,
                            size_t               romref_mem_size,

                            size_t               data_buf_size,
                            size_t               align_size);

/**
 * \brief 对象去初始化
 *
 * \param[in] p_pbuf_pool  指向PBUF对象的指针
 *
 * \retval 无
 */
void aw_pbuf_pool_uninit (struct aw_pbuf_pool *p_pbuf_pool);

/**
 * \brief 申请一个PBUF
 *
 * \param[in] p_pbuf_pool  指向PBUF的指针
 * \param[in] type         PBUF的类型
 * \param[in] size         PBUF的数据长度
 * \param[in] p_mem        PBUF的payload数据缓冲区,当type为AW_PBUF_TYPE_ROM/REF
 *                         时由用户提供，其他类型为空。如果不为空，这里请注意
 *                         数据缓冲区内存对齐问题
 *
 * \retval 申请成功，返回为PBUF指针
 * \retval NULL   操作失败,失败原因可查看errno
 */
struct aw_pbuf *aw_pbuf_alloc (struct aw_pbuf_pool *p_pbuf_pool,
                               aw_pbuf_type_t       type,
                               size_t               size,
                               void                *p_mem);

/**
 * \brief 用户自分配的PBUF定义（用于PBUF扩展）
 *
 * \param[in] p_pbuf        指向PBUF的指针
 * \param[in] p_mem         自定义PBUF的内存
 * \param[in] size          PBUF的数据长度
 * \param[in] pfn_app_free  PBUF释放回调函数（当PBUF引用计数为0时会调用）
 * \param[in] p_arg         回调函数参数
 *
 * \retval 申请成功，返回为PBUF指针
 * \retval NULL,操作失败，失败原因可查看errno
 */
struct aw_pbuf *aw_pbuf_alloced_custom (struct aw_pbuf_custom    *p_pbuf,
                                        void                     *p_mem,
                                        size_t                    size,

                                        aw_pbuf_custom_callback_t pfn_app_free,
                                        void                     *p_arg);

/**
 * \brief 重新调整pbuf包的大小（只支持缩小不支持扩充）
 *
 * \param[in] p_pbuf  指向PBUF的指针
 * \param[in] size    PBUF调整之后的长度
 *
 * \retval  AW_OK       操作成功
 * \retval -AW_EINVAL   参数错误
 * \retval -AW_ENOTSUP  不支持扩展P_PBUF数据长度
 */
aw_err_t aw_pbuf_realloc (struct aw_pbuf *p_pbuf, size_t size);


/**
 * \brief 调整包首个PBUF的大小
 *
 * \param[in] p_pbuf  指向PBUF的指针
 * \param[in] size    增加的大小（可正可负）
 *
 * \retval  AW_OK      设备信息设置完成
 * \retval -AW_EINVAL  参数错误
 * \retval  AW_ERROR   size太大或太小，因为调整后数据缓冲区将覆盖PBUF头或溢出
 *
 * \note 调整后的空间可存储其他信息，调整范围在数据缓冲区空间之内,这个接口可用于
 *       裁剪数据帧的头部
 */
aw_err_t aw_pbuf_header (struct aw_pbuf *p_pbuf, ssize_t size);

/**
 * \brief 增加PBUF的引用计数值
 *
 * \param[in] p_pbuf  指向PBUF的指针
 *
 * \retval AW_OK       成功
 * \retval -AW_EINVAL  参数错误
 *
 * \note 在使用一个非自己创建分配的PBUF时，需要调用此接口表示引用了这个PBUF
 */
aw_err_t aw_pbuf_ref(struct aw_pbuf *p_pbuf);

/**
 * \brief 释放p_pbuf的引用
 *
 * \param[in] p_pbuf  指向PBUF的指针
 *
 * \retval 释放掉的PBUF个数
 *
 * \note 如果引用计数为空，系统会回收该PBUF资源
 */
size_t aw_pbuf_free (struct aw_pbuf *p_pbuf);

/**
 * \brief 统计PUBF链的长度
 *
 * \param[in] p_pbuf 指向PBUF的指针
 *
 * \retval 链中的PBUF个数
 */
size_t aw_pbuf_cnt_get (struct aw_pbuf *p_pbuf);

/**
 * \brief 将p_tail链p_head串成一个包
 *
 * \param[in] p_head  包中首个PBUF指针（p_head必须为一个包而不是一个包队列）
 * \param[in] p_tail  加入包的PBUF链
 *
 * \retval  AW_OK       操作成功
 * \retval -AW_EINVAL   参数错误
 * \retval -AW_ENOTSUP  p_tail已经存在链p_head中，操作不允许
 *
 * \note 调用此接口后p_tail引用无效，这个接口将p_tail加入p_head包中。注意p_head
 *       和p_tail之间不能是交叉的，也就是说如果p_head和p_tail链了相同的PBUF,
 *       那么p_head和p_tail不能使用这个接口。
 */
aw_err_t aw_pbuf_pkt_expand (struct aw_pbuf *p_head, struct aw_pbuf *p_tail);

/**
 * \brief 将p_tail和p_head链起来
 *
 * \param[in] p_pkt_fifo  指向包队列(p_head必须为包队列头指针)
 * \param[in] p_pkt       指向链入的包（由一个或多个PBUF组成）
 *
 * \retval  AW_OK       操作成功
 * \retval -AW_EINVAL   参数错误
 * \retval -AW_ENOTSUP  p_tail已经存在链p_head中，操作不允许
 *
 * \note p_pkt_fifo是一个包队列的头，p_pkt包加入包队列p_pkt_fifo中，调用此接口后
         p_pkt引用无效。这个接口与上面aw_pbuf_pkt_expand不同，一个数据包可能由一
         个PBUF组成的，也可能是有多个PBUF组成的，aw_pbuf_pkt_expand接口是将p_pkt
         链加入包p_pkt_fifo中，而aw_pbuf_pkt_chain是将包加入包队列中。注意
         p_pkt_fifo和p_pkt之间不能是交叉的，也就是说如果p_pkt_fifo和p_pkt链了
         相同的PBUF,那么p_pkt_fifo和p_pkt不能使用这个接口。
 */
aw_err_t aw_pbuf_pkt_chain (struct aw_pbuf *p_pkt_fifo, struct aw_pbuf *p_pkt);

/**
 * \brief 将p_pbuf转换为type类型的PBUF
 *
 * \param[in] p_pbuf  指向需要转换的PBUF
 * \param[in] type    转换之后的类型（支持AW_PBUF_TYPE_RAM和AW_PBUF_TYPE_POOL）
 *
 * \retval 转换成功,指向p_buf的指针
 * \retval NULL,转换失败，失败原因可查看errno
 */
struct aw_pbuf* aw_pbuf_type_change (struct aw_pbuf *p_pbuf,
                                     aw_pbuf_type_t  type);

/**
 * \brief PBUF节点数据拷贝（PBUF之间）
 *
 * \param[in] p_to    目标PBUF
 * \param[in] p_from  源PBUF
 *
 * \retval  AW_OK      拷贝成功
 * \retval -AW_EINVAL  参数错误
 * \retval -AW_ENOMEM  p_to长度不能小于p_from长度
 */
aw_err_t aw_pbuf_copy (struct aw_pbuf *p_to, struct aw_pbuf *p_from);

/**
 * \brief PBUF数据拷贝（PBUF拷贝到缓冲区）
 *
 * \param[in] p_pbuf  源PBUF
 * \param[in] p_data  指向数据缓冲区首地址
 * \param[in] len     拷贝的数据长度
 * \param[in] offset  PBUF数据区缓存偏移
 *
 * \retval 拷贝成功的数据量，小于0为拷贝失败,失败原因可查看errno
 */
ssize_t aw_pbuf_data_read (struct aw_pbuf *p_pbuf,
                           void           *p_data,
                           size_t          len,
                           size_t          offset);

/**
 * \brief PBUF数据拷贝（缓冲区拷贝到PBUF）
 *
 * \param[in] p_pbuf  目标PBUF
 * \param[in] p_data  指向数据缓冲区首地址
 * \param[in] len     拷贝的数据长度
 * \param[in] offset  PBUF数据区缓存偏移
 *
 * \retval 拷贝成功的数据量，小于0为拷贝失败,失败原因可查看errno
 */
ssize_t aw_pbuf_data_write (struct aw_pbuf *p_pbuf,
                            const void     *p_data,
                            size_t          len,
                            size_t          offset);

/**
 * \brief 获取PBUF中指定位置的数据（一个字节）
 *
 * \param[in] p_pbuf  指向操作的PBUF
 * \param[in] offset  该字节在PBUF中的偏移
 *
 * \retval 非0 获取的字符
 * \retval 0  获取失败(p_pbuf缓冲区长度小于offset或参数错误)
 */
uint8_t aw_pbuf_char_get (struct aw_pbuf* p_pbuf, size_t offset);

/**
 * \brief 比较PBUF中的字符串
 *
 * \param[in] p_pbuf  比较的PBUF
 * \param[in] offset  PBUF的数据偏移
 * \param[in] p_str   比较的字符串
 * \param[in] len     比较的字符串长度
 *
 * \retval 0            比较成功
 * \retval ssize_t < 0  比较错误,失败原因可查看errno
 * \retval ssize_t > 0  失败(从字符串p_str第ssize_t字节处比较失败)
 */
ssize_t aw_pbuf_memcmp (struct aw_pbuf *p_pbuf,
                        size_t          offset,
                        const void     *p_str,
                        size_t          len);
/**
 * \brief 匹配PBUF中的字符串（从PBUF的start_offset位置开始匹配字符串）
 *
 * \param[in] p_pbuf        指向需要匹配的PBUF
 * \param[in] p_str         被匹配的字符串
 * \param[in] len           字符串长度
 * \param[in] start_offset  PBUF的偏移
 *
 * \return 匹配到字符串的地址,小于0为匹配失败,失败原因可查看errno
 */
ssize_t aw_pbuf_memfind (struct aw_pbuf *p_pbuf,
                         const void     *p_str,
                         size_t          len,
                         size_t          start_offset);

/**
 * \brief PBUF字符串匹配（在PBUF中匹配一个字符串，返回改字符串的地址）
 *
 * \param[in] p_pbuf    指向需要匹配的PBUF
 * \param[in] p_substr  被匹配的字符串
 *
 * \return 匹配到字符串的地址,小于0为匹配失败,失败原因可查看errno
 */
ssize_t aw_pbuf_strstr (struct aw_pbuf *p_pbuf, const char *p_substr);


#ifdef __cplusplus
}
#endif

/** @} grp_aw_if_pbuf */

#endif /* } __AW_PBUF_H */

/* end of file */
