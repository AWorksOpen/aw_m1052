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
 * \brief expand I/O system.
 *
 * \internal
 * \par modification history:
 * - 16-01-07 deo, first implementation.
 * \endinternal
 */

#ifndef __AW_EIO_H
#define __AW_EIO_H


#ifdef __cplusplus
extern "C" {
#endif

#include "aw_list.h"
#include "aw_sem.h"
#include "aw_pool.h"
#include "aw_spinlock.h"
#include "aw_pbuf.h"

/**
 * \brief EIO 设备结构体
 */
struct aw_eio_dev {
    struct aw_list_head        node;     /**< \brief 设备节点 */

    const char                *name;     /**< \brief 设备名 */

    const struct aw_eio_funs  *p_funs;   /**< \brief 设备服务，由EIO驱动提供 */

    aw_spinlock_isr_t          lock;     /**< \brief 设备自旋锁 */

    struct aw_list_head        pipes;    /**< \brief 该设备的通道链链头 */

    uint32_t                   np;       /**< \brief 该设备的总通道数 */
};

/**
 * \brief EIO 通道结构体
 */
struct aw_eio_pipe {
    uint32_t                id;          /**< \brief 通道ID, 用于区分通道属性 */
#define AW_EIO_IN        0x80000000      /**< \brief 可读通道（单工） */
#define AW_EIO_OUT       0x40000000      /**< \brief 可写通道（单工） */
#define AW_EIO_INOUT     0xC0000000      /**< \brief 可读写通道（半双工） */

    struct aw_list_head     node;        /**< \brief 通道节点 */

    struct aw_list_head     work_list;   /**< \brief 等待传输的数据包链 */

    struct aw_list_head     done_list;   /**< \brief 已完成传输的数据包链 */

    AW_SEMC_DECL(           done_sem);   /**< \brief 通道信号量,用于同步包的传输 */

    aw_spinlock_isr_t       lock;        /**< \brief 通道自旋锁 */

    struct aw_eio_obj      *p_obj;       /**< \brief 通道所属EIO对象 */

    void                   *p_priv;      /**< \brief 保留给驱动程序使用 */
};

/**
 * \brief EIO 对象结构体
 */
struct aw_eio_obj {
    struct aw_eio_dev   *p_dev;         /**< \brief EIO对象的设备 */
    void                *p_mem;         /**< \brief EIO对象的内存池的内存 */
    struct aw_pool       pkt_pool;      /**< \brief EIO对象的内存池 */
};

/**
 * \brief EIO 数据包结构体
 */
struct aw_eio_pkt {
    struct aw_list_head   node;        /**< \brief 数据包节点 */

    struct aw_eio_obj    *p_obj;       /**< \brief 数据包所属的对象 */

    void                 *p_buf;       /**< \brief 包的数据缓冲区 */
    size_t                len;         /**< \brief 数据缓冲区长度 */

    void                 *p_cur_buf;   /**< \brief 当前数据包正在传输的位置 */

    int                   opt;         /**< \brief 数据包操作属性 */
#define AW_EIO_READ        0           /**< \brief 读操作 */
#define AW_EIO_WRITE       1           /**< \brief 写操作 */

    int                   status;      /**< \brief 数据包传输状态 */

    int                   ret_len;     /**< \brief 完成传输数据长度 */

    uint8_t               flags;       /**< \brief 数据包标志 */
#define AW_EIO_FLAGS_POOL       0x01   /**< \brief 该数据包包头由内存池提供 */
#define AW_EIO_FLAGS_PBUF       0x02   /**< \brief 该数据包数据区为PBUF格式 */

    /** \brief 数据包传输完成回调函数 */
    aw_bool_t              (*pfn_complete) (void *arg);

    /** \brief 回调函数参数 */
    void                 *p_arg;

    void                 *p_priv;        /**< \brief 保留给驱动程序使用 */
};

/**
 * \brief EIO 设备服务结构体
 */
struct aw_eio_funs {

    /** \brief 启动数据传输，驱动应注意重复启动的情况 */
    aw_err_t (*pfn_start) (struct aw_eio_dev  *p_dev,
                           struct aw_eio_pipe *p_pipe);

    /** \brief 取消正在传输的所有数据包的传输操作 */
    void (*pfn_abort) (struct aw_eio_dev  *p_dev,
                       struct aw_eio_pipe *p_pipe);

    /** \brief 取消一个数据包（等待传输或正在传输）的传输 */
    void (*pfn_cancel) (struct aw_eio_dev  *p_dev,
                        struct aw_eio_pipe *p_pipe,
                        struct aw_eio_pkt  *p_pkt);

    /** \brief 设备控制 */
    aw_err_t (*pfn_control) (struct aw_eio_dev *p_dev,
                             int                cmd,
                             void              *p_arg);
};


/**
 * \brief EIO 服务初始化
 *
 * \retval 无
 */
void aw_eio_init (void);

/**
 * \brief EIO 对象初始化
 *
 * \param[in]       p_obj   EIO 对象
 * \param[in]       name    EIO 设备名
 * \param[in]       p_mem   用于创建数据包池的内存
 * \param[in]       size    内存的大小
 *
 * \retval AW_OK          初始化成功
 * \retval -AW_EINVAL     参数错误
 * \retval -AW_ENODEV     没有发现名为name的设备
 * \retval -AW_ENOMEM     数据包初始化失败
 */
aw_err_t aw_eio_obj_init (struct aw_eio_obj  *p_obj,
                          const char         *name,
                          void               *p_mem,
                          size_t              size);

/**
 * \brief EIO 对象去初始化
 *
 * \param[in]       p_obj   EIO 对象
 *
 * \retval AW_OK       去初始化成功
 * \retval -AW_EINVAL  参数错误
 * \retval AW_ERROR    去初始化失败
 */
aw_err_t aw_eio_obj_uninit (struct aw_eio_obj  *p_obj);

/**
 * \brief 打开一个 EIO 通道
 *
 * \param[in]       p_obj   EIO 对象
 * \param[in]       pipe_id 通道ID
 *
 * \retval 非NULL   指向打开的通道
 * \retval NULL     打开失败
 */
struct aw_eio_pipe *aw_eio_pipe_open (struct aw_eio_obj *p_obj,
                                      uint32_t           pipe_id);

/**
 * \brief 关闭一个 EIO 通道
 *
 * \param[in]    p_pipe   EIO 通道
 *
 * \retval 无
 */
void aw_eio_pipe_close (struct aw_eio_pipe *p_pipe);

/**
 * \brief EIO 同步读
 *
 * \param[in]       p_pipe   EIO 通道
 * \param[in]       p_buf    EIO 数据缓冲区
 * \param[in]       len      读取的长度
 * \param[in]       timeout  超时（ms）
 *
 * \retval len         读取成功
 * \retval 0 ~ len     读取超时，返回已经传输完成的长度
 * \retval -AW_EINVAL  参数错误
 * \retval -AW_ENOMEM  初始化信号量失败
 *
 * \note 这个接口从EIO通道读取len个字节数据，
 *       在timeout时间内未完成此操作时停止操
 *       作并返回已经读取到的字节数。
 */
int aw_eio_read (struct aw_eio_pipe *p_pipe,
                 void               *p_buf,
                 size_t              len,
                 int                 timeout);

/**
 * \brief EIO 同步写
 *
 * \param[in]       p_pipe   EIO 通道
 * \param[in]       p_buf    EIO 数据缓冲区
 * \param[in]       len      写入的长度
 * \param[in]       timeout  超时（ms）
 *
 * \retval len         读取成功
 * \retval 0 ~ len     读取超时，返回已经传输完成的长度
 * \retval -AW_EINVAL  参数错误
 * \retval -AW_ENOMEM  初始化信号量失败
 *
 * \note 这个接口向EIO通道写入len个字节数据，
 *       在timeout内时间未完成此操作时停止操
 *       作并返回已经写入的字节数。
 */
int aw_eio_write (struct aw_eio_pipe *p_pipe,
                  void               *p_buf,
                  size_t              len,
                  int                 timeout);

/**
 * \brief ISSUE/RECLAIM模式下申请一个异步传输操作
 *
 * \param[in]       p_pipe       EIO 通道
 * \param[in]       opt          异步传输的操作
 * \param[in]       p_buf        数据缓冲区
 * \param[in]       len          传输的数据长度
 * \param[in]       pfn_complete 完成传输回调函数
 * \param[in]       p_arg        回调函数参数
 *
 * \retval AW_OK       申请成功
 * \retval -AW_ENOMEM  EIO包内存池空
 * \retval -AW_EINVAL  参数错误
 *
 * \note 这个接口向通道申请一次异步传输，如果pfn_complete不为空，此传输完成
 *       后会调用pfn_complete回调函数，如果pfn_complete为空，此传输完成后会
 *       将传输完的数据和结果存放在该通道内，用户使用aw_eio_reclaim可以从通
 *       道回收该传输操作。
 */
int aw_eio_issue (struct aw_eio_pipe *p_pipe,
                  int                 opt,
                  void               *p_buf,
                  size_t              len,
                  aw_bool_t          (*pfn_complete) (void *arg),
                  void               *p_arg);

/**
 * \brief ISSUE/RECLAIM模式下回收一个异步传输操作
 *
 * \param[in]        p_pipe     通道
 * \param[out]       p_opt      传输的操作
 * \param[out]       pp_buf     传输的数据缓冲区的指针
 * \param[out]       p_ret_len  传输完成的长度
 * \param[out]       p_status   传输的完成状态
 * \param[in]        timeout    超时（ms）
 * \param[in]        abort      取消标志
 *
 * \retval AW_OK      回收成功
 * \retval -AW_ETIME  回收超时
 * \retval -AW_EINVAL 参数错误
 *
 * \note 这个接口与aw_eio_issue结合使用，用于回收一个异步传输操作，异步传输操
 *       作的申请与回收遵循FIFO原则，先申请的操作先被回收后申请的操作后被回收。
 *       aw_eio_reclaim接口返回后pp_buf指向回收到的缓冲区首地址，其他返回数据
 *       分别在p_opt，p_ret_len，p_status中。调用此接口后timeout时间内如果没有
 *       回收到传输操作时，如果abort为TRUE时会强制终止正在传输的数据操作，如果
 *       有该操作，返回该操作，abort为FALSE会一直等到该操作完成时才返回。
 */
int aw_eio_reclaim(struct aw_eio_pipe *p_pipe,
                   int                *p_opt,
                   void              **pp_buf,
                   size_t             *p_ret_len,
                   int                *p_status,
                   int                 timeout,
                   aw_bool_t           abort);

/**
 * \brief ISSUE/RECLAIM模式下申请一个异步传输操作
 *
 * \param[in]       p_pipe       EIO 通道
 * \param[in]       opt          异步传输的操作
 * \param[in]       p_pbuf       数据缓冲区
 * \param[in]       len          传输的数据长度
 * \param[in]       pfn_complete 完成传输回调函数
 * \param[in]       p_arg        回调函数参数
 *
 * \retval AW_OK       申请成功
 * \retval -AW_ENOMEM  EIO包内存池空
 * \retval -AW_EINVAL  参数错误
 *
 * \note 这个接口向通道申请一次异步传输，如果pfn_complete不为空，此传输完成
 *       后会调用pfn_complete回调函数，如果pfn_complete为空，此传输完成后会
 *       将传输完的数据和结果存放在该通道内，用户使用aw_eio_pbuf_reclaim可以
 *       从通道回收该传输操作。
 */
int aw_eio_pbuf_issue (struct aw_eio_pipe *p_pipe,
                       int                 opt,
                       struct aw_pbuf     *p_pbuf,
                       size_t              len,
                       aw_bool_t          (*pfn_complete) (void *arg),
                       void               *p_arg);

/**
 * \brief ISSUE/RECLAIM模式下回收一个异步传输操作
 *
 * \param[in]        p_pipe     通道
 * \param[out]       p_opt      传输的操作
 * \param[out]       pp_pbuf    传输的数据缓冲区的指针
 * \param[out]       p_ret_len  传输完成的长度
 * \param[out]       p_status   传输的完成状态
 * \param[in]        timeout    超时（ms）
 * \param[in]        abort      取消标志
 *
 * \retval AW_OK      回收成功
 * \retval -AW_ETIME  回收超时
 * \retval -AW_EINVAL 参数错误
 *
 * \note 这个接口与aw_eio_pbuf_issue结合使用，用于回收一个异步传输操作，异步传
 *       输操作的申请与回收遵循FIFO原则，先申请的操作先被回收后申请的操作后被回
 *       收。这个接口返回后pp_buf指向回收到的缓冲区首地址，其他返回数据分别在
 *       p_opt，p_ret_len，p_status中。调用此接口后timeout时间内如果没有回收到传
 *       输操作时，如果abort为TRUE时会强制终止正在传输的数据操作，如果有该操作，
 *       返回该操作，abort为FALSE会一直等到该操作完成时才返回。
 */
int aw_eio_pbuf_reclaim(struct aw_eio_pipe  *p_pipe,
                        int                 *p_opt,
                        struct aw_pbuf     **pp_pbuf,
                        size_t              *p_ret_len,
                        int                 *p_status,
                        int                  timeout,
                        aw_bool_t            abort);

/**
 * \brief EIO 设备控制
 *
 * \param[in]       p_obj   EIO 对象
 * \param[in]       cmd     控制命令
 * \param[in,out]   p_arg   控制参数
 *
 * \retval -AW_EINVAL    参数错误
 * \retval -AW_ENOTSUP   驱动没有实现该服务
 * \retval AW_OK         取消成功
 *
 * \note 这个接口控制EIO设备的相关配置，如配置通信速率等
 */
aw_err_t aw_eio_ctrl (struct aw_eio_obj *p_obj,
                      int                cmd,
                      void              *p_arg);

/**
 * \brief 取消通道内所有数据包的传输
 *
 * \param[in]         p_pipe  EIO 通道
 *
 * \retval -AW_EINVAL    参数错误
 * \retval -AW_ENOTSUP   驱动没有实现该服务
 * \retval AW_OK         取消成功
 */
aw_err_t aw_eio_abort (struct aw_eio_pipe *p_pipe);

/**
 * \brief 创建一个 EIO 设备，由EIO设备驱动初始化的时候调用添加设备
 *
 * \param[in]       p_dev   EIO 设备指针
 * \param[in]       name    EIO 设备名
 * \param[in]       p_funs  设备服务指针
 *
 * \retval -AW_EINVAL     参数错误
 * \retval -AW_EEXIST     该设备已经注册
 * \retval AW_OK          注册成功
 *
 * \note 先创建设备，后添加通道，这个接口在aw_eio_dev_pipe_add前调用
 */
aw_err_t aw_eio_dev_create (struct aw_eio_dev  *p_dev,
                            const char         *name,
                            struct aw_eio_funs *p_funs);

/**
 * \brief 删除 EIO 设备
 *
 * \param[in]   p_dev   EIO 设备
 *
 * \retval -AW_EINVAL  参数错误
 * \retval AW_OK       操作成功
 */
aw_err_t aw_eio_dev_delete (struct aw_eio_dev *p_dev);

/**
 * \brief 向 EIO 设备中添加通道
 *
 * \param[in]       p_dev   EIO 设备
 * \param[in]       p_pipe  EIO 通道指针
 * \param[in]       id      通道ID
 * \param[in]       p_priv  通道参数（驱动定义该参数）
 *
 * \retval 无
 *
 * \note 由EIO设备驱动初始化的时候调用添加设备通道，先创建设备，
 *       后添加通道，这个接口在aw_eio_dev_create后调用
 */
void aw_eio_dev_pipe_add (struct aw_eio_dev  *p_dev,
                          struct aw_eio_pipe *p_pipe,
                          uint32_t            id,
                          void               *p_priv);

/**
 * \brief 从EIO设备中删除通道
 *
 * \param[in]    p_dev     EIO 设备
 * \param[in]    p_pipe    EIO 通道
 *
 * \retval 无
 *
 * \note 由EIO设备驱动去初始化的时候调用删除设备通道
 */
void aw_eio_dev_pipe_del (struct aw_eio_dev  *p_dev,
                          struct aw_eio_pipe *p_pipe);

/**
 * \brief 从通道获取一个数据包
 *
 * \param[in]       p_pipe   EIO通道
 *
 * \retval 非NULL   获取的一个数据包
 * \retval NULL     通道为空
 *
 * \note 这个接口为EIO向下接口，当EIO调用start服务启动通道传输之后，
 *       驱动会从该通道不断获取数据包进行传输，传输完会将完成的数据
 *       包提交给EIO,直到通道为空停止传输
 */
struct aw_eio_pkt *aw_eio_dev_pkt_get (struct aw_eio_pipe *p_pipe);

/**
 * \brief 获取数据包的一块数据缓冲区
 *
 * \param[in]       p_pkt     指向正在传输的数据包
 * \param[out]      pp_buf    指向返回的数据缓冲区
 * \param[out]      p_len     返回的数据缓冲区的长度
 *
 * \retval AW_OK        读取成功
 * \retval -AW_EINVAL   参数错误
 * \retval -AW_ENOMEM   数据包已空
 *
 * \note 这个接口为EIO向下接口，获取指定数据包的一块数据缓冲区，一个数据包
 *       p_pkt的缓冲区可能是多个且非连续的，使用这个接口不断获取数据包的数
 *       据缓冲区分块并传输，直到数据包中已经没有缓冲区时表示这一包数据已经
 *       完成传输操作，可以调用aw_eio_dev_pkt_done提交这一包数据
 */
aw_err_t aw_eio_pkt_buf_get (struct aw_eio_pkt *p_pkt,
                             void             **pp_buf,
                             size_t            *p_len);

/**
 * \brief 提交一个已完成的数据包
 *
 * \param[in]       p_pipe   EIO 通道
 * \param[in]       p_pkt    已经传输完成的数据包
 *
 * \retval 无
 *
 * \note 这个接口为EIO向下接口，EIO设备驱动完成一个包的传输后调用这个接口
 *       提交数据包给EIO
 */
void aw_eio_dev_pkt_done (struct aw_eio_pipe *p_pipe,
                          struct aw_eio_pkt  *p_pkt);

#ifdef __cplusplus
}
#endif


#endif /* __AW_EIO_H */
