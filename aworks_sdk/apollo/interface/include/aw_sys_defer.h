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
 * \brief  系统延迟作业组件
 *
 * \note
 *      1. 若用户有一些函数需要被异步调用，并且想提交到一些公共的线程去执行，
 *         那么 defer 就很适用了，只需定义 defer 对象并启动，然后将工作节点 
 *         commit 到该 defer 中即可
 *
 *      2. 当前系统默认配置有两个延迟作业组件，包括 nor_defer 和 isr_defer，
 *         ISR defer：    一般用于处理紧急工作
 *         Normal defer： 一般用于处理非紧工作
 *         aw_prj_params.h中配置这两个 defer 线程的堆栈大小：
 *         \#define AW_CFG_ISR_DEFER_TASK_STACK_SIZE    (1024 * 10)
 *         \#define AW_CFG_NOR_DEFER_TASK_STACK_SIZE    (1024 * 10)
 *
 *      3.  ISR defer:        aw_isr_defer_cfg.c
 *          Normal defer:     aw_nor_defer_cfg.c
 *
 * 使用本服务需要包含以下头文件
 * \code
 * #include "aw_sys_defer.h"
 * \endcode
 *
 * \par 使用示例
 * \code

    #include "aw_sys_defer.h"
    #include "aw_vdebug.h"
    #include "aw_sem.h"

    aw_local struct aw_defer_djob   __g_djob;
    AW_SEMB_DECL_STATIC(__g_sem);

    aw_local void  __job_call(void *p_arg)
    {
        AW_INFOF(("job done, job data: 0x%08x\n", (uint32_t)p_arg));
        AW_SEMB_GIVE(__g_sem);
    }

    void test_sys_defer(void)
    {
        aw_err_t  ret = AW_OK;

        // 初始化信号量
        AW_SEMB_INIT(__g_sem, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);

        // 初始化一个工作节点
        aw_defer_job_init(&__g_djob, __job_call, (void *)0x12345678);

        // 将一个工作节点添加至系统中断延迟作业中处理
        ret = aw_sys_defer_commit(AW_SYS_DEFER_TYPE_ISR, &__g_djob);
        if (ret != AW_OK) {
            AW_ERRF(("aw_sys_defer_commit err: %d\n", ret));
        }

        // 等待当前工作已完成
        AW_SEMB_TAKE(__g_sem, AW_SEM_WAIT_FOREVER);

        // 判断当前工作节点是否可用
        if (aw_sys_defer_job_is_usable(AW_SYS_DEFER_TYPE_ISR, &__g_djob)) {
            AW_INFOF(("curr job can usable\n"));
        }

        // 将一个工作节点添加至系统普通延迟作业中处理
        ret = aw_sys_defer_commit(AW_SYS_DEFER_TYPE_NOR, &__g_djob);
        if (ret != AW_OK) {
            AW_ERRF(("aw_sys_defer_commit err: %d\n", ret));
        }

        // 等待当前工作已完成
        AW_SEMB_TAKE(__g_sem, AW_SEM_WAIT_FOREVER);

        // 判断当前工作节点是否可用
        if (aw_sys_defer_job_is_usable(AW_SYS_DEFER_TYPE_NOR, &__g_djob)) {
            AW_INFOF(("curr job can usable\n"));
        }
    }

 * \endcode
 *
 * \internal
 * \par modification history
 * - 1.00 17-03-31  may, first implementation.
 * \endinternal
 */

#ifndef __AW_SYS_DEFER_H
#define __AW_SYS_DEFER_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_aw_if_sys_defer
 * \copydoc aw_sys_defer.h
 * @{
 */

#include "aworks.h"
#include "aw_defer.h"

/** \brief sys_defer 类型  */
typedef enum {
    AW_SYS_DEFER_TYPE_ISR,  /**< \brief 紧急类型 */
    AW_SYS_DEFER_TYPE_NOR   /**< \brief 普通类型 */
} aw_sys_defer_t;

/**
 * \brief 申明系统全局defer对象，具体定义分别在
 *        aw_isr_defer_cfg.c和aw_nor_defer_cfg.c 
 */
 
/** \brief 声明全局紧急类型defer对象 */
aw_export struct aw_defer g_sys_isr_defer;  

/** \brief 声明全局普通类型defer对象 */
aw_export struct aw_defer g_sys_nor_defer;  

/**
 * \brief 判断工作节点是否可用
 *
 * \param[in] defer_t  系统延迟作业类型
 * \param[in] p_job    工作节点指针
 *
 * \retval  AW_TRUE     节点可用
 * \retval  AW_FALSE    节点不可用, 已经被添加到队列中
 *
 * \par 示例
 * \code
 *  void timer_isr (void *param)
 *  {
 *      if (aw_sys_defer_job_is_usable(AW_SYS_DEFER_TYPE_ISR, &g_myjob) ) {
 *          // 节点可用，将其添加到队列中
 *          aw_sys_defer_commit(AW_SYS_DEFER_TYPE_ISR, &g_myjob);
 *      } else {
 *          // 节点不可用，已经被添加到队列中
 *      }
 *  }
 * \endcode
 */
aw_static_inline aw_bool_t aw_sys_defer_job_is_usable (
    aw_sys_defer_t defer_t, struct aw_defer_djob *p_job)
{
    if (defer_t == AW_SYS_DEFER_TYPE_ISR) {
        return  aw_defer_job_is_usable(&g_sys_isr_defer, p_job);
    } else if (defer_t == AW_SYS_DEFER_TYPE_NOR) {
        return  aw_defer_job_is_usable(&g_sys_nor_defer, p_job);
    }

    return  AW_FALSE;
}

/**
 * \brief 将一个工作节点添加至系统延迟作业中处理，节点添加后只能被执行一次（即会
 *        被自动清除）,若当前工作节点已添加，那直接返回错误
 *
 * \param[in] defer_t  系统延迟作业类型
 * \param[in] p_job    job 节点指针
 *
 * \retval  AW_OK    当 list_empty(&p_job->node)为真或者
 *                  (p_job->node.next == NULL && p_job->node.prev == NULL)时，
 *                  成功加到队列中。
 *
 * \retval -AW_EEXIST  加入队列失败, 节点仍然在队列中。
 * \retval -AW_EINVAL  传入参数错误
 *
 * \attention       节点被处理完后, list_empty(&p_job->node)为真。
 *                  特别地，在使用list_empty(&p_job->node)时，要注意关中断，
 *                  或者使用 aw_defer_job_is_usable()代替。
 */
aw_static_inline aw_err_t aw_sys_defer_commit(aw_sys_defer_t        defer_t,
                                              struct aw_defer_djob *p_job)
{
    if (defer_t == AW_SYS_DEFER_TYPE_ISR) {
        return  aw_defer_commit(&g_sys_isr_defer, p_job);
    } else if (defer_t == AW_SYS_DEFER_TYPE_NOR) {
        return  aw_defer_commit(&g_sys_nor_defer, p_job);
    }

    return  -AW_EINVAL;
}

/**
 * \brief 从系统延时作业中取消当前工作
 *
 * \param[in] defer_t  系统延迟作业类型
 * \param[in] p_job    工作节点指针
 *
 * \retval  AW_OK       取消成功
 * \retval  -AW_EINVAL  参数有错
 *
 */
aw_static_inline aw_err_t aw_sys_defer_abort(aw_sys_defer_t        defer_t,
                                             struct aw_defer_djob *p_job)
{
    if (defer_t == AW_SYS_DEFER_TYPE_ISR) {
        return  aw_defer_abort(&g_sys_isr_defer, p_job);
    } else if (defer_t == AW_SYS_DEFER_TYPE_NOR) {
        return  aw_defer_abort(&g_sys_nor_defer, p_job);
    }

    return  -AW_EINVAL;
}

/** @} grp_aw_if_sys_defer */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AW_SYS_DEFER_H */

/* end of file */

