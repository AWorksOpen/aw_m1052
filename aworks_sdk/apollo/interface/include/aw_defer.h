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
 * \brief 延迟作业组件
 *
 * \note  用户可以将一系列工作节点添加至延迟作业中处理使用时需要用户不断调用
 *        aw_defer_task_entry(可以将其作为线程入口)或者调用 AW_DEFER_START
 *
 * 使用本服务需要包含头文件
 * \code
 * #include aw_defer.h
 * \endcode
 *
 * \par 简单示例
 * \code

    #include "aw_defer.h"
    #include "aw_vdebug.h"
    #include "aw_sem.h"

    aw_local struct aw_defer        __g_defer;
    aw_local struct aw_defer_djob   __g_djob;
    AW_SEMB_DECL_STATIC(__g_sem);

    aw_local void  __job_call(void *p_arg)
    {
        AW_INFOF(("job done, job data: 0x%08x\n", (uint32_t)p_arg));
        AW_SEMB_GIVE(__g_sem);
    }

    void  test_defer(void)
    {
        aw_err_t  ret = AW_OK;

        // 初始化一个延迟作业
        aw_defer_init(&__g_defer);

        // 启动延迟作业
        AW_DEFER_START(&__g_defer, 3, 0, 4096, "defer");

        // 初始化一个工作节点
        aw_defer_job_init(&__g_djob, __job_call, (void *)0x12345678);

        // 初始化信号量
        AW_SEMB_INIT(__g_sem, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);

        // 将一个工作节点添加至延迟作业处理
        ret = aw_defer_commit(&__g_defer, &__g_djob);
        if (ret != AW_OK) {
            AW_ERRF(("aw_defer_commit err: %d\n", ret));
        }

        // 等待当前工作已完成
        AW_SEMB_TAKE(__g_sem, AW_SEM_WAIT_FOREVER);

        // 判断当前工作节点是否可用
        if (aw_defer_job_is_usable(&__g_defer, &__g_djob)) {
            AW_INFOF(("curr job can usable\n"));
        }
    }

 * \endcode
 *
 * // 更多内容待添加。。。
 *
 * \internal
 * \par modification history:
 * - 1.10 17-03-24  may, first implementation
 * \endinternal
 */

#ifndef __AW_DEFER_H
#define __AW_DEFER_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_aw_if_defer
 * \copydoc aw_defer.h
 * @{
 */

#include "aw_task.h"
#include "aw_sem.h"
#include "aw_list.h"

/**
 * \brief 延迟作业对象
 */
struct aw_defer
{
    struct aw_list_head head;   /**< \brief 节点 */
    AW_SEMC_DECL(sem);          /**< \brief 使用的计数信号量 */
};

/**
 * \brief 工作对象
 */
struct aw_defer_djob
{
    struct aw_list_head node;   /**< \brief 节点 */
    void (*func)(void *p_arg);  /**< \brief 执行函数 */
    void *param;                /**< \brief 执行函数参数 */
};

/**
 * \brief 初始化延迟作业
 *
 * \param[in] p_this       延迟作业节点指针
 *
 */
void aw_defer_init (struct aw_defer   *p_this);

/**
 * \brief 启动延迟作业，这里创建 tcnt 个线程执行延迟作业，这些线程优先级都为tprio，
 *        堆栈大小stack_size，线程名字 tname。若当前系统不支持创建优先级相同的线程，
 *        用户可以自己创建若干个线程，然后将 aw_defer_task_entry 作为这些线程的入
 *        口，参数为 p_defer
 *
 * \param[in] p_defer       延迟作业指针
 * \param[in] tcnt          线程数量
 * \param[in] tprio         线程优先级
 * \param[in] stack_size    线程堆栈大小
 * \param[in] tname         线程名字
 *
 */
#define AW_DEFER_START(p_defer, tcnt, tprio, stack_size, tname) \
    do { \
        struct  aw_defer_task {     \
            struct  __aw_defer_task  {              \
                AW_TASK_DECL(task, (stack_size));   \
            } task[tcnt];   \
        };                  \
                            \
        aw_local struct  aw_defer_task  __g_defer_task;  \
            \
        int          i = 0;         \
        aw_task_id_t task_id;       \
                                    \
        for (i = 0; i < AW_NELEMENTS(__g_defer_task.task); i++) {   \
            task_id = AW_TASK_INIT(__g_defer_task.task[i].task,     \
                                   (tname), \
                                   (tprio), \
                                   (stack_size),        \
                                   (aw_pfuncvoid_t *)aw_defer_task_entry, \
                                   (void*)(p_defer));   \
            if (task_id != NULL) {          \
                aw_task_startup(task_id);   \
            }   \
        }       \
    } while(0)

/**
 * \brief 初始化一个工作节点
 *
 * \param[in] p_this    job 节点指针
 * \param[in] pfunc     job 回调函数
 * \param[in] p_arg     job 回调函数的参数
 *
 */
void aw_defer_job_init(struct aw_defer_djob   *p_this,
                       void                  (*pfunc) (void *),
                       void                   *p_arg);

/**
 * \brief 判断工作节点是否可用
 *
 * \param[in] p_this  延迟作业节点指针
 * \param[in] p_job   工作节点指针
 *
 * \retval  AW_TRUE     节点可用
 * \retval  AW_FALSE    节点不可用, 已经被添加到队列中
 *
 * \par 示例
 * \code
 *  void timer_isr (void *param)
 *  {
 *      if (aw_defer_job_is_usable(&g_myjob) ) {
 *          // 节点可用，将其添加到队列中
 *          aw_defer_commit(&g_myjob);
 *      } else {
 *          // 节点不可用，已经被添加到队列中
 *      }
 *  }
 * \endcode
 */
aw_bool_t aw_defer_job_is_usable (struct aw_defer      *p_this, 
                                  struct aw_defer_djob *p_job);

/**
 * \brief 将一个工作节点添加至延迟作业中处理，节点添加后只能被执行一次
 *       （即会被自动清除）,若当前工作节点已添加，那直接返回错误
 *
 * \param[in] p_this    延时作业节点指针
 * \param[in] p_job     job 节点指针
 *
 * \retval  AW_OK    当 list_empty(&p_job->node)为真或者
 *                  (p_job->node.next == NULL && p_job->node.prev == NULL)时，
 *                  成功加到队列中。
 *
 * \retval -AW_EEXIST  加入队列失败, 节点仍然在队列中。
 *
 * \attention       节点被处理完后, list_empty(&p_job->node)为真。
 *                  特别地，在使用list_empty(&p_job->node)时，要注意关中断，
 *                  或者使用 aw_defer_job_is_usable()代替。
 */
aw_err_t aw_defer_commit( struct aw_defer *p_this, struct aw_defer_djob *p_job);

/**
 * \brief 从延时作业中取消当前工作
 *
 * \param[in] p_this  延迟作业节点指针
 * \param[in] p_job   工作节点指针
 *
 * \retval  AW_OK       取消成功
 * \retval  -AW_EINVAL  参数有错
 *
 */
aw_err_t aw_defer_abort(struct aw_defer *p_this, struct aw_defer_djob *p_job);

/**
 * \brief 即延迟作业中工作真正被执行的地方，用户可以创建若干个线程，
 *        将其作为线程执行入口，并且要将延迟作业指针作为线程参数传入
 *
 * \param[in] p_this    延时作业节点指针
 * 
 * \return 无
 */
void aw_defer_task_entry (struct aw_defer *p_this);

/** @} grp_aw_if_defer */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AW_DEFER_H */

/* end of file */
