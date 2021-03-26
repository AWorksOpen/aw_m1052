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
 * \brief AWBus DMA 接口头文件
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * #include "awbl_dma.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 13-09-30  zen, first implementation
 * \endinternal
 */

#ifndef __AWBL_DMA_H
#define __AWBL_DMA_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "apollo.h"
#include "aw_spinlock.h"
#include "aw_sem.h"
#include "aw_dma.h"

/** \brief DMA 通道状态 */
typedef enum awbl_dma_chan_status {

    /** \brief 未初始化 */
    AWBL_DMA_CHAN_STATUS_NOT_INITED = 0,

    /** \brief 通道空闲 */
    AWBL_DMA_CHAN_STATUS_IDLE,

    /** \brief 通道运行 */
    AWBL_DMA_CHAN_STATUS_RUN,

    /** \brief 通道暂停 */
    AWBL_DMA_CHAN_STATUS_PAUSED,

    /** \brief 通道出错 */
    AWBL_DMA_CHAN_STATUS_ERROR

} awbl_dma_chan_status_t;

/** \brief DMA 控制器状态 */
typedef enum awbl_dma_status {

    /** \brief 未初始化 */
    AWBL_DMA_STATUS_NOT_INITED = 0,

    /** \brief DMA 空闲 */
    AWBL_DMA_STATUS_IDLE,

    /** \brief DMA 运行 */
    AWBL_DMA_STATUS_RUN,

    /** \brief DMA 暂停 */
    AWBL_DMA_STATUS_PAUSE,

    /** \brief DMA 出错 */
    AWBL_DMA_STATUS_ERROR

} awbl_dma_status_t;

/**
 * \brief AWBus DMA 通道实例
 */
typedef struct awbl_dma_chan_res {

    /** \brief 这些成员由DMA管理器初始化和更新 */
    bool                      allocated;    /**< \brief 是否已被分配 */
    void                     *p_dmac_spec;  /**< \brief DMA驱动定义 */
    struct awbl_dma_service  *p_serv;       /**< \brief 对应的DMA服务实例 */
    awbl_dma_chan_status_t    status;       /**< \brief DMA 通道状态 */
    struct aw_list_head       trans_list;   /**< \brief 传输请求列表 */
    aw_spinlock_isr_t         isr_lock;     /**< \brief 保护列表的访问  */

    AW_MUTEX_DECL(dev_lock);

    /** \brief 这些成员由DMA驱动初始化 */
    unsigned    chan_no;        /**< \brief 通道号 */
    unsigned    chan_pri;       /**< \brief 通道优先级 */
    bool        sup_any;        /**< \brief 是否支持 AW_DMA_CHAN_ANY 请求*/
    uint32_t    flags;          /**< \brief 特性标志 (保留，填0) */
    void       *p_dmac_ctrl;    /**< \brief 由DMA驱动使用 */

} awbl_dma_chan_res_t;

/** \brief 锁定DMA通道资源 */
#define AWBL_DMA_CHAN_RES_LOCK(p_chan_res) \
    AW_MUTEX_LOCK((p_chan_res)->dev_lock, AW_SEM_WAIT_FOREVER)

/** \brief 释放DMA通道资源 */
#define AWBL_DMA_CHAN_RES_UNLOCK(p_chan_res) \
    AW_MUTEX_UNLOCK((p_chan_res)->dev_lock)

/** \brief 锁定DMA通道资源(自旋锁方式) */
#define AWBL_DMA_CHAN_RES_SPIN_LOCK(p_chan_res) \
    aw_spinlock_isr_take(&(p_chan_res)->isr_lock)

/** \brief 释放DMA通道资源(自旋锁方式) */
#define AWBL_DMA_CHAN_RES_SPIN_UNLOCK(p_chan_res) \
    aw_spinlock_isr_give(&(p_chan_res)->isr_lock)



/** \brief 从控制器的传输队列取出DMA请求 */
aw_static_inline aw_dma_trans_req_t *
    awbl_dma_trans_req_pop(awbl_dma_chan_res_t *p_chan_res)
{
    AWBL_DMA_CHAN_RES_SPIN_LOCK(p_chan_res);

    if (aw_list_empty_careful(&p_chan_res->trans_list)) {
        AWBL_DMA_CHAN_RES_SPIN_UNLOCK(p_chan_res);
        return NULL;
    } else {
        struct aw_list_head *p_node = p_chan_res->trans_list.next;
        aw_list_del(p_node);
        AWBL_DMA_CHAN_RES_SPIN_UNLOCK(p_chan_res);
        return aw_list_entry(p_node, aw_dma_trans_req_t, node);
    }
}

/**
 * \brief 初始化DMA通道实例
 *
 * \param p_chan_res    DMA 通道实例
 * \param chan_no       实例在 DMA 控制器上对应的通道号
 * \param chan_pri      默认优先级
 * \param sup_any       是否支持 AW_DMA_CHAN_ANY 请求
 * \param flags         特性标志 (保留，填0)
 */
static aw_inline void awbl_dma_chan_res_init(awbl_dma_chan_res_t *p_chan_res,
                                             unsigned             chan_no,
                                             unsigned             chan_pri,
                                             bool                 sup_any,
                                             uint32_t             flags,
                                             void                *p_dmac_ctrl)
{
    p_chan_res->chan_no     = chan_no;
    p_chan_res->chan_pri    = chan_pri;
    p_chan_res->sup_any     = sup_any;
    p_chan_res->flags       = flags;
    p_chan_res->p_dmac_ctrl = p_dmac_ctrl;
}

/**
 * \brief AWBus DMA 服务函数
 */
struct awbl_dma_servopts {

    /**
     * \brief DMA 通道分配探测
     *
     * DMA 管理器在分配对应的 DMA 通道之前，会调用此函数将控制权交给 DMA 驱动，
     * DMA 驱动需要判断 p_chan_res 所描述的通道能否被分配: 若不能被分配，需要返
     * AW_ERROR ; 如能够被分配，则初始化对应的 DMA 通道，最后返回 AW_OK；
     *
     * \param p_serv        DMA 服务
     * \param p_chan_res    通道资源
     * \param chan_no       DMA 通道号
     * \param chan_pri      DMA 通道优先级 (传输队列号)
     * \param flags         特性标志 (保留)
     * \param p_dmac_spec   由DMA控制器驱动定义
     *
     * \retval AW_OK        可以分配
     * \retval AW_ERROR     不可以分配
     *
     * \attention 本函数需要在 awbl_dev_init2() 中能够使用
     */
   aw_err_t  (*pfunc_chan_alloc_probe)(struct awbl_dma_service  *p_serv,
                                       struct awbl_dma_chan_res *p_chan_res);

    /**
     * \brief 释放DMA通道
     * \attention 本函数需要在 awbl_dev_init2() 中能够使用
     */
    aw_err_t (*pfunc_dma_chan_free)(struct awbl_dma_service  *p_serv,
                                    awbl_dma_chan_res_t      *chan_res);



    /**
     * \brief 启动传输
     * \attention 本函数需要在 awbl_dev_init2() 中能够使用
     */
    aw_err_t (*launch_transfer) (awbl_dma_chan_res_t *chan_res);

    /**
     * \brief 设置传输
     */
    aw_err_t (*transfer_setup) (struct awbl_dma_service  *p_serv,
                                awbl_dma_chan_res_t      *chan_res,
                                aw_dma_trans_req_t       *p_trans_req);

};

/**
 * \brief AWBus DMA 服务实例
 */
typedef struct awbl_dma_service {
    bool    initialized;        /**< \brief 初始化是否完成 */

    size_t  queue_num;          /**< \brief 传输队列数，每个队列有不同优先级*/
    size_t  chan_num;           /**< \brief 通道数*/

    struct awbl_dma_chan_res  *p_chan_res;      /**< \brief 通道资源 */

    const struct awbl_dma_servopts *p_servopts; /**< \brief DMA 服务的相关函数 */

    awbl_dma_status_t status;           /**< \brief DMA 状态 */
    awbl_dev_t *p_dmac;                 /**< \brief DMA 控制器 */

    AW_MUTEX_DECL(dev_lock);            /**< \brief 服务互斥 */

    unsigned last_chan_id;              /**< \brief 上次分配的通道号 */
    struct awbl_dma_service *p_next;    /**< \brief 指向下一个 DMA 服务 */
} awbl_dma_service_t;

/** \brief 锁定DMA通道资源(自旋锁方式) */
#define AWBL_DMA_SERV_SPIN_LOCK(p_serv) \
    aw_spinlock_isr_take(&(p_serv)->isr_lock)

/** \brief 释放DMA通道资源(自旋锁方式) */
#define AWBL_DMA_SERV_SPIN_UNLOCK(p_serv) \
    aw_spinlock_isr_give(&(p_serv)->isr_lock)


/**
 * \brief 初始化DMA服务实例(AWBus 初始化第1阶段由驱动调用)
 *
 * \param p_chan_res    DMA 通道实例
 * \param chan_no       实例在 DMA 控制器上对应的通道号
 * \param chan_pri      默认优先级
 * \param sup_any       是否支持 AW_DMA_CHAN_ANY 请求
 * \param flags         特性标志 (保留，填0)
 */
static aw_inline void
    awbl_dma_service_init(awbl_dma_service_t             *p_serv,
                          size_t                          chan_num,
                          size_t                          queue_num,
                          struct awbl_dma_chan_res       *p_chan_res,
                          const struct awbl_dma_servopts *p_servopts)
{
    int i;

    p_serv->chan_num   = chan_num;
    p_serv->queue_num  = queue_num;
    p_serv->p_chan_res = p_chan_res;
    p_serv->p_servopts = p_servopts;

    p_serv->last_chan_id = chan_num - 1;
    p_serv->p_next       = NULL;
    p_serv->initialized  = false;

    for (i = 0; i < p_serv->chan_num; i++) {

        p_chan_res[i].allocated   = false;
        p_chan_res[i].p_dmac_spec = NULL;
        p_chan_res[i].p_serv      = p_serv;
        p_chan_res[i].status      = AWBL_DMA_CHAN_STATUS_IDLE;

        aw_list_head_init(&p_chan_res[i].trans_list);
        aw_spinlock_isr_init(&p_chan_res[i].isr_lock, 0);
        AW_MUTEX_INIT(p_chan_res[i].dev_lock, AW_SEM_Q_PRIORITY);
    }

    AW_MUTEX_INIT(p_serv->dev_lock, AW_SEM_Q_PRIORITY);

    p_serv->status      = AWBL_DMA_STATUS_IDLE;
    p_serv->initialized = true;
}

/**
 * \brief AWBus DMA 接口模块初始化
 *
 * \attention 本函数应当在 awbl_dev_init1() 之后、在 awbl_dev_init2() 之前调用
 */
void awbl_dma_lib_init(void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_DMA_H */

/* end of file */

