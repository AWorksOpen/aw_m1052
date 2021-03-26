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
 * \brief 作业队列库
 *
 * 使用本服务需要包含头文件 aw_job_queue.h
 *
 * 用户可以选择在创建作业队列时，是否加入标准作业池；
 * 这样，当新建作业时，可以选择利用作业队列内部的空闲作业池
 * 或者选择用户提供作业的空间，并填充相关信息。
 *
 * \par 使用标准作业池
 * \code
 *
 * #define STD_JOB_NUM     10
 * #define MY_JOB_PRIO     20
 *
 * static aw_job_queue_id_t     my_jobq_id;
 * static aw_qjob_std_pool_t    my_pool;
 * static aw_qjob_std_pool_id_t my_pool_id = &my_pool;
 *
 * static void std_jobq_init(void)
 * {  
 *     aw_job_queue_std_pool_init(my_pool_id);  
 *
 *     aw_job_queue_std_jobs_alloc(my_pool_id, STD_JOB_NUM); 
 *
 *     my_jobq_id = aw_job_queue_create(my_pool_id);   
 * }
 *
 * static void add_std_job(void)
 * {
 *     aw_job_queue_std_post(my_jobq_id,
 *                           MY_JOB_PRIO,
 *                           callback_func,
 *                    (void*)0,
 *                    (void*)0,
 *                    (void*)0,
 *                    (void*)0);
 * } 
 * \endcode 
 *
 * \par 使用用户提供的作业空间
 * \code
 *
 * #define MY_JOB_PRIO     20
 *
 * static aw_qjob_t       user_job;
 * static aw_job_queue_t  my_jobq;
 *
 * static void user_jobq_init(void)
 * {  
 *     aw_job_queue_init(&my_jobq, NULL);   
 * }
 *
 * static void add_user_job(void)
 * {
 *     aw_job_queue_fill_job(&user_job, 
 *                           callback_func, 
 *                    (void*)0, 
 *                           MY_JOB_PRIO, 
 *                           0);
 *
 *     aw_job_queue_post(&my_jobq, &user_job);
 * } 
 * \endcode 
 *
 * \internal
 * \par modification history:
 * - 1.01 15-06-02  deo, add aw_job_queue_fill_job
 * - 1.00 14-03-19  zen, first implementation
 * \endinternal
 */

#ifndef __AW_JOB_QUEUE_H
#define __AW_JOB_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_task.h"
#include "aw_sem.h"
#include "aw_list.h"
#include "aw_spinlock.h"

/**
 * \addtogroup grp_aw_if_job_queue
 * \copydoc aw_job_queue.h
 * @{
 */

#ifndef AW_SRCFG_QJOB_NUM_PRI
#define AW_SRCFG_QJOB_NUM_PRI   32  /**< \brief 作业队列优先级数量 */
#endif

#if AW_SRCFG_QJOB_NUM_PRI > 32
#error "AW_SRCFG_QJOB_NUM_PRI can not bigger than 32"
#endif

/** \brief 当前作业已经在作业队列中 */
#define AW_QJOB_ENQUEUED    0x100

/** \brief 当前作业是从作业队列池中分配出来的 */
#define AW_QJOB_POOL_ALLOC  0x200

/** \brief 作业队列忙 */
#define AW_QJOB_BUSY        0x20 

/** \brief 测试当前作业是否忙 */
#define AW_QJOB_IS_BUSY(p_job)  ((p_job)->flags & AW_QJOB_BUSY)

/** \brief 设置当前作业忙 */
#define AW_QJOB_SET_BUSY(p_job) ((p_job)->flags |= AW_QJOB_BUSY)

/** \brief 设置当前作业空闲 */
#define AW_QJOB_CLR_BUSY(p_job) ((p_job)->flags &= ~AW_QJOB_BUSY)


/** \brief 作业队列最低优先级 */
#define AW_QJOB_PRI_LOWEST   0   

/** \brief 作业队列最高优先级 */
#define AW_QJOB_PRI_HIGHEST  31    

/** \brief 作业队列优先级数量 */
#define AW_QJOB_NUM_PRI      32    

/** \brief 作业队列优先级掩码 */
#define AW_QJOB_PRI_MASK     0x1f

/** 
 * \brief 基本作业的定义
 *
 * 这是基本作业的定义，可放到其他结构体里，用于扩展作业的定义。
 * 一般的执行函数，都会以该结构体的指针作为传入参数。
 */
struct aw_qjob {

    /** \brief 作业执行的函数  */
    aw_pfuncvoid_t      func;

    /** \brief 作业执行的函数的参数 */
    void               *p_arg;

    /** \brief 作业的优先级 */
    uint16_t            pri;

    /** \brief 作业的标记 */
    uint16_t            flags;

    /** \brief 作业队列的节点 */
    struct aw_list_head node;
};

/** \brief 基本作业 */
typedef struct aw_qjob aw_qjob_t;

/**
 * \brief 标准作业的定义
 * \extends aw_qjob
 */
struct aw_qjob_std {
    
    /** \brief 基本作业 */
    aw_qjob_t base;
    
    /** \brief 标准参数1 */
    void     *arg1;
    
    /** \brief 标准参数2 */
    void     *arg2;
    
    /** \brief 标准参数3 */
    void     *arg3;
    
    /** \brief 标准参数4 */
    void     *arg4;

};

/** \brief 标准作业 */
typedef struct aw_qjob_std aw_qjob_std_t;


/** \brief 标准作业池的定义 */
struct aw_qjob_std_pool {
    
    /** \brief 标准作业的空闲链表 */
    struct aw_list_head  free;
    
    /** \brief 标准作业链表 */
    struct aw_list_head  blocks;    
    
    /** \brief 提供的作业队列数量 */
    size_t               ref_count; 
    
    /** \brief 自旋锁指针 */
    aw_spinlock_isr_t    *p_lock;

};

/** \brief 标准作业池 */
typedef struct aw_qjob_std_pool aw_qjob_std_pool_t;

/** \brief 标准作业池 ID */
typedef aw_qjob_std_pool_t  *aw_qjob_std_pool_id_t;


/** \brief 作业队列的定义 */
struct aw_job_queue {

    /** \brief 等待队列 */
    struct aw_list_head  jobs[AW_SRCFG_QJOB_NUM_PRI];

    /** \brief 位图，记录当前活动的优先级 */
    uint32_t             active_pri;

    /** \brief 标准作业池 */
    aw_qjob_std_pool_t  *p_std_pool;
    
    /** \brief 当前运行的任务 ID */
    aw_task_id_t         task_id;   

    /**
     * \name 成员变量
     * @{
     */
    AW_SEMB_DECL(semb);  /**< \brief 二进制互斥锁 */
    /** @} */

    /** \brief 作业队列标记 */
    uint32_t             flags;

/** \brief 作业队列正在运行 */
#define AW_JOBQ_FLG_RUNNING  0x01

/** \brief 作业队列刚创建 */
#define AW_JOBQ_FLG_CREATED  0x02

    /** \brief 保留给用户空间 */
    void           *user_val;

    /** \brief 自旋锁 */
    aw_spinlock_isr_t   lock;

};

/** \brief 作业队列 */
typedef struct aw_job_queue  aw_job_queue_t;

/**
 * \brief 作业队列 ID
 */
typedef aw_job_queue_t *aw_job_queue_id_t;

/**
 * \brief 填充作业
 *
 * 等同于初始化，添加到作业队列后，才会执行相关操作。
 *
 * \param[in]  p_job   作业对象
 * \param[in]  func    要执行的函数
 * \param[in]  p_arg   要执行的函数参数
 * \param[in]  pri     作业的优先级
 * \param[in]  flags   作业的标记
 *
 * \return 无
 */
void aw_job_queue_fill_job (aw_qjob_t      *p_job,
                            aw_pfuncvoid_t  func,
                            void           *p_arg,
                            uint16_t        pri,
                            uint16_t        flags);

/**
 * \brief 删除作业队列
 *
 * 注意在删除该作业队列后，不要再次引用该 ID。 
 *
 * \param[in]  job_queue_id   作业队列 ID
 *
 * \return 无
 */
void aw_job_queue_delete (aw_job_queue_id_t job_queue_id);

/**
 * \brief 将作业加入到作业队列
 *
 * 若加入成功，会修改作业的标记，标记其已加入等待队列，
 * 并更新作业队列的活动优先级位图。
 *
 * \param[in] job_queue_id 作业队列 ID
 * \param[in] p_job        要加入的作业
 *
 * \retval  AW_OK      发布成功
 * \retval -AW_EINVAL  参数无效
 * \retval -AW_EBUSY   作业队列忙
 *
 * \sa aw_job_queue_std_post()
 */
aw_err_t aw_job_queue_post (aw_job_queue_id_t job_queue_id, aw_qjob_t *p_job);


/**
 * \brief 申请标准作业加入到作业队列
 *
 * 若申请成功，会将这个作业从空闲链表放到等待队列，
 * 并更新作业队列的优先级位图。用户既可以通过 aw_job_queue_post()
 * 加入用户提供的作业空间，也可以从 aw_job_queue_std_post() 申请标准作业池
 * 的作业空间，执行相应操作。
 *
 * \param[in] job_queue_id  作业队列 ID
 * \param[in] priority      优先级
 * \param[in] func          执行的函数
 * \param[in] arg1          参数1
 * \param[in] arg2          参数2
 * \param[in] arg3          参数3
 * \param[in] arg4          参数4
 *
 * \retval  AW_OK      申请成功
 * \retval -AW_EINVAL  参数无效
 * \retval -AW_EBUSY   作业队列忙
 * \retval -AW_ENOENT  没有空闲作业可以添加
 */
aw_err_t aw_job_queue_std_post (aw_job_queue_id_t job_queue_id,
                                int               priority,
                                aw_pfuncvoid_t    func,
                                void             *arg1,
                                void             *arg2,
                                void             *arg3,
                                void             *arg4);

/**
 * \brief 作业队列处理例程
 *
 * 处理作业队列里的作业，一般放在一个单独的线程执行这个例程。
 *
 * \param[in]  job_queue_id   作业队列 ID
 *
 * \return 假若当前作业队列在运行，就返回 -AW_EINVAL，
 *         若一切就绪，则不会从该函数返回，一直执行。
 */
aw_err_t aw_job_queue_process (aw_job_queue_id_t job_queue_id);


/**
 * \brief 从标准作业池创建一个作业队列
 *
 * 当成功创建时，会返回作业队列 ID。
 *
 * \param[in]  std_pool_id  作业队列的标准作业池 
 *
 * \return 若成功，返回作业队列ID；若失败，返回NULL。
 */
aw_job_queue_id_t aw_job_queue_create (aw_qjob_std_pool_id_t std_pool_id);


/**
 * \brief 初始化作业队列
 *
 * 初始化成功，会将 \e std_pool_id 的成员 \e ref_count 加1。
 * 若传入的 \e std_pool_id 为NULL，则作业队列没有标准作业池，
 * 所有作业的空间由用户提供。
 *
 * \param[in,out]  p_jobq       要初始化的作业队列 
 * \param[in]      std_pool_id  标准作业队列池 ID
 *
 * \retval  AW_OK      初始化成功
 * \retval -AW_EINVAL  参数无效
 */
aw_err_t aw_job_queue_init (aw_job_queue_t        *p_jobq,
                            aw_qjob_std_pool_id_t  std_pool_id);


/**
 * \brief 分配新的作业到标准作业池的空闲链表
 *
 * \param[in,out] std_pool_id  标准作业池 ID 
 * \param[in]     njobs        作业数量
 *
 * \retval  AW_OK       分配成功
 * \retval -AW_EINVAL   参数无效
 * \retval -AW_ENOMEM   内存不足
 */
aw_err_t aw_job_queue_std_jobs_alloc (aw_qjob_std_pool_id_t std_pool_id,
                                      size_t                njobs);

/**
 * \brief 初始化标准作业池
 *
 * \param[in,out] std_pool_id  标准作业池 ID 
 *
 * \retval  AW_OK  初始化成功
 */
aw_err_t aw_job_queue_std_pool_init (aw_qjob_std_pool_id_t std_pool_id);


/** @} grp_aw_if_job_queue */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AW_JOB_QUEUE_H */

/* end of file */
